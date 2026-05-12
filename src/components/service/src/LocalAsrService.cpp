#include "LocalAsrService.h"
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QElapsedTimer>
#include <cstring> // memset 必须引入
#include <string>  // 必须引入
// 如果你想在编译时真正链接 sherpa-ncnn 的静态库，请在 CMake 中打开此宏
#ifdef ENABLE_LOCAL_ASR
#include "sherpa-ncnn/c-api/c-api.h"
#endif

void LocalAsrService::stopExiting() { m_isExiting = true; }

LocalAsrService::LocalAsrService(QObject *parent)
    : QObject(parent), m_recognizer(nullptr), m_stream(nullptr) {}

LocalAsrService::~LocalAsrService() {
#ifdef ENABLE_LOCAL_ASR
  if (m_stream) {
    DestroyStream((SherpaNcnnStream *)m_stream);
  }
  if (m_recognizer) {
    DestroyRecognizer((SherpaNcnnRecognizer *)m_recognizer);
  }
#endif
}

bool LocalAsrService::initModel(const QString &modelDir) {
  if (modelDir.isEmpty())
    return false;

#ifdef ENABLE_LOCAL_ASR
  qDebug() << "LocalAsrService: 准备加载 NCNN 语音模型：" << modelDir;
  QDir d(modelDir);
  if (!d.exists()) {
    qWarning() << "本地模型目录不存在！";
    return false;
  }

  SherpaNcnnModelConfig model_config;
  memset(&model_config, 0, sizeof(model_config));

  // 使用类的 std::string 成员变量固化内存，防止临时对象销毁
  m_tokens = d.absoluteFilePath("tokens.txt").toStdString();
  m_encoder_p =
      d.absoluteFilePath("encoder_jit_trace-pnnx.ncnn.param").toStdString();
  m_encoder_b =
      d.absoluteFilePath("encoder_jit_trace-pnnx.ncnn.bin").toStdString();
  m_decoder_p =
      d.absoluteFilePath("decoder_jit_trace-pnnx.ncnn.param").toStdString();
  m_decoder_b =
      d.absoluteFilePath("decoder_jit_trace-pnnx.ncnn.bin").toStdString();
  m_joiner_p =
      d.absoluteFilePath("joiner_jit_trace-pnnx.ncnn.param").toStdString();
  m_joiner_b =
      d.absoluteFilePath("joiner_jit_trace-pnnx.ncnn.bin").toStdString();

  model_config.tokens = m_tokens.c_str();
  model_config.encoder_param = m_encoder_p.c_str();
  model_config.encoder_bin = m_encoder_b.c_str();
  model_config.decoder_param = m_decoder_p.c_str();
  model_config.decoder_bin = m_decoder_b.c_str();
  model_config.joiner_param = m_joiner_p.c_str();
  model_config.joiner_bin = m_joiner_b.c_str();
  model_config.num_threads = 1; // I.MX6ULL 双核Cortex-A7 可以给1核跑

  SherpaNcnnDecoderConfig decoder_config;
  memset(&decoder_config, 0, sizeof(decoder_config));
  decoder_config.decoding_method = "greedy_search";
  decoder_config.num_active_paths = 4;

  SherpaNcnnRecognizerConfig config;
  memset(&config, 0, sizeof(config));
  config.model_config = model_config;
  config.decoder_config = decoder_config;
  config.enable_endpoint = 1; // 开启自动VAD端点检测（静音检测）
  config.rule1_min_trailing_silence = 2.4f; // 2.4秒没声音自动断句
  config.rule2_min_trailing_silence = 1.2f;
  config.rule3_min_utterance_length = 20.0f;
  config.feat_config.sampling_rate = 16000;
  config.feat_config.feature_dim = 80;

  m_recognizer = CreateRecognizer(&config);
  if (!m_recognizer) {
    qWarning()
        << "本地大模型 Recognizer 创建失败，可能由于内存不足或路径错误！";
    return false;
  }

  // 初始化一个数据流通道
  m_stream = CreateStream((SherpaNcnnRecognizer *)m_recognizer);
  qDebug() << "本地大模型装载就绪";
  return true;
#else
  qWarning()
      << "LocalAsrService 编译时未开启 "
         "ENABLE_LOCAL_ASR。请参考交叉编译指南引入 Sherpa-NCNN 后重新编译！";
  return false;
#endif
}

void LocalAsrService::feedAudio(QByteArray bytes) {
#ifdef ENABLE_LOCAL_ASR
  if (!m_recognizer || !m_stream) {
    qWarning() << "[ASR 错误] 模型引擎未就绪或被销毁，丢弃当前音频帧！";
    return;
  }

  int bytesCount = bytes.size();
  if (bytesCount == 0) {
    qWarning() << "[ASR 警告] 收到0字节大小的空音频帧！";
    return;
  }

  const char *data = bytes.constData();
  int total_shorts = bytesCount / sizeof(int16_t);
  const int16_t *pcm16 = reinterpret_cast<const int16_t *>(data);

  // [关键修复] 底层 ALSA 固定采集为 44100Hz + 立体声(2-Channels)
  // 而 Sherpa-NCNN 的 zipformer 模型严格要求 16000Hz + 单声道(1-Channel)
  // 如果不转换，它听到的声音频率和通道全错，自然“未识别出有效文字”
  std::vector<float> float_pcm;
  float_pcm.reserve(total_shorts / 5); // 预留空间

  double ratio = 44100.0 / 16000.0;
  double current_idx = 0.0;

  while (true) {
    int frame_idx = static_cast<int>(current_idx);
    int L_idx = frame_idx * 2;
    int R_idx = frame_idx * 2 + 1;

    if (R_idx >= total_shorts)
      break;

    // 提取左右声道并混合为单声道，做浮点归一化 (-1.0f 到 1.0f)
    float l_val = pcm16[L_idx] / 32768.0f;
    float r_val = pcm16[R_idx] / 32768.0f;
    float mono_val = (l_val + r_val) / 2.0f;

    // 增加增益放大声音(可选，防止硬件麦克风音量太小，放大2倍并截断)
    mono_val *= 2.0f;
    if (mono_val > 1.0f)
      mono_val = 1.0f;
    if (mono_val < -1.0f)
      mono_val = -1.0f;

    float_pcm.push_back(mono_val);
    current_idx += ratio;
  }

  qDebug() << "[ASR] 接收到音频 大小:" << float_pcm.size()
           << " samples. 准备提取声学特征并压入计算流...";

  QElapsedTimer timer;
  timer.start();

  // 将这一段浮点音频喂入识别流
  AcceptWaveform((SherpaNcnnStream *)m_stream, 16000, float_pcm.data(),
                 float_pcm.size());
  qDebug() << "  -> [步骤1] 特征提取与入流完毕，耗时:" << timer.elapsed()
           << "ms";

  // 循环解码（此处为流式追加解码）
  QString progressBar = "[推理块数]: ";
  int decodedFrames = 0;
  timer.restart();

  while (!m_isExiting && IsReady((SherpaNcnnRecognizer *)m_recognizer,
                                 (SherpaNcnnStream *)m_stream)) {
    qDebug()
        << "  -> [步骤2] 正在进行繁重的 NCNN 神经网络前向矩阵计算(Decode)...";
    Decode((SherpaNcnnRecognizer *)m_recognizer, (SherpaNcnnStream *)m_stream);
    progressBar += "█";
    decodedFrames++;
  }

  if (decodedFrames > 0) {
    // 进度条打印，由于没有最终结尾换行符等问题，正常用 qDebug 显示即可
    qDebug() << progressBar << " 本轮解码了" << decodedFrames
             << "个特征块，耗时:" << timer.elapsed() << "ms";
  }
  SherpaNcnnResult *res = GetResult((SherpaNcnnRecognizer *)m_recognizer,
                                    (SherpaNcnnStream *)m_stream);
  if (res && res->text && strlen(res->text) > 0) {
    QString txt = QString::fromUtf8(res->text);
    emit partialResultReady(txt);
  }

  // 检查是否静音超过设定值，自动触发断句 VAD 断点
  if (IsEndpoint((SherpaNcnnRecognizer *)m_recognizer,
                 (SherpaNcnnStream *)m_stream)) {
    qDebug() << "\n=========================================================";
    qDebug() << "[ASR VAD断点] 检测到长时间静音 (用户已停止说话或超时) ! ";

    if (res && res->text && strlen(res->text) > 0) {
      QString finalTxt = QString::fromUtf8(res->text);
      qDebug() << "[ASR 最终识别结果] \U0001F680: \"" << finalTxt << "\"";
      emit finalResultReady(finalTxt);
    } else {
      qDebug() << "[ASR 最终识别结果] \U0001F6AB: (噪音或未识别出有效文字)";
    }
    qDebug() << "=========================================================\n";
    Reset((SherpaNcnnRecognizer *)m_recognizer, (SherpaNcnnStream *)m_stream);
  }

  DestroyResult(res);
#endif
}

QString LocalAsrService::getResultText() {
#ifdef ENABLE_LOCAL_ASR
  if (!m_recognizer || !m_stream)
    return "";
  SherpaNcnnResult *res = GetResult((SherpaNcnnRecognizer *)m_recognizer,
                                    (SherpaNcnnStream *)m_stream);
  QString txt = "";
  if (res && res->text) {
    txt = QString::fromUtf8(res->text);
  }
  DestroyResult(res);
  return txt;
#else
  return "端侧AI模型未编译启用";
#endif
}

void LocalAsrService::reset() {
#ifdef ENABLE_LOCAL_ASR
  if (m_recognizer && m_stream) {
    Reset((SherpaNcnnRecognizer *)m_recognizer, (SherpaNcnnStream *)m_stream);
  }
#endif
}

void LocalAsrService::recognizeWav(QString filePath) {
#ifdef ENABLE_LOCAL_ASR
  if (!m_recognizer || !m_stream) {
    qWarning() << "[ASR 错误] 模型未就绪！";
    return;
  }
  QFile f(filePath);
  if (!f.open(QIODevice::ReadOnly)) {
    qWarning() << "[ASR 错误] 无法打开录音文件:" << filePath;
    return;
  }
  QByteArray bytes = f.readAll();
  f.close();
  
  if (bytes.size() == 0) {
    qWarning() << "[ASR 警告] 录音文件为空！";
    return;
  }

  qDebug() << "[ASR] 读取整段录音文件，大小:" << bytes.size() << "bytes";
  // 直接复用我们写好的音频处理逻辑
  feedAudio(bytes);
  
  // feedAudio里会循环解码。如果这段完整的音频里模型没有主动判定 VAD endpoint，
  // 我们就在整段结束后强制给个终止符，保证它输出最终文字。
  // 注意：因为我们改成整段喂入，最好在这里加入 InputFinished 来强行终结话语。
  qDebug() << "[ASR] 整段音频喂入完毕，强制断句并获取最后结果...";
  
  // 必须传入0，告诉解码器没有更多音频了
  InputFinished((SherpaNcnnStream *)m_stream);
  
  // 先把剩下的全解完
  while (!m_isExiting && IsReady((SherpaNcnnRecognizer *)m_recognizer, (SherpaNcnnStream *)m_stream)) {
      Decode((SherpaNcnnRecognizer *)m_recognizer, (SherpaNcnnStream *)m_stream);
  }

  SherpaNcnnResult *res = GetResult((SherpaNcnnRecognizer *)m_recognizer, (SherpaNcnnStream *)m_stream);
  if (res && res->text && strlen(res->text) > 0) {
      QString finalTxt = QString::fromUtf8(res->text);
      qDebug() << "[ASR 最终识别结果] \U0001F680: \"" << finalTxt << "\"";
      emit finalResultReady(finalTxt);
  } else {
      qDebug() << "[ASR 最终识别结果] \U0001F6AB: (噪音或未识别出有效文字)";
      // 我们也可以返回空文本，防止UI卡在“正在提取...”
      emit finalResultReady("");
  }
  qDebug() << "=========================================================\n";
  DestroyResult(res);
  
  // 重置流准备迎接下一次录音
  Reset((SherpaNcnnRecognizer *)m_recognizer, (SherpaNcnnStream *)m_stream);

#else
  qWarning() << "本地大模型未开启";
#endif
}
