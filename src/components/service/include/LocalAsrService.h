#ifndef LOCALASRSERVICE_H
#define LOCALASRSERVICE_H

#include <QByteArray>
#include <QObject>
#include <QString>
#include <atomic>
#include <string>
#include <vector>

class LocalAsrService : public QObject {
  Q_OBJECT
public:
  explicit LocalAsrService(QObject *parent = nullptr);
  ~LocalAsrService();

public slots:
  // 初始化 NCNN 端侧大模型 (传入模型文件目录)
  bool initModel(const QString &modelDir);

  // 强制解码并获取目前的话语
  QString getResultText();

  // 重置此段语音对话流
  void reset();

  // 喂入实时录制的 PCM 数据 (改成接受 QByteArray，方便配合 QueuedConnection
  // 进行跨线程调用)
  Q_INVOKABLE void recognizeWav(QString filePath);
  void feedAudio(QByteArray data);

  // 通知停止推理退出
  void stopExiting();

signals:
  // 回调事件：当检测到句子有更新时抛出
  void partialResultReady(QString text);
  // 回调事件：当一句话说完并确定时抛出
  void finalResultReady(QString text);

private:
  void *m_recognizer; // 隐藏 SherpaNcnnRecognizer* 避免主程序头文件污染
  void *m_stream;     // 隐藏 SherpaNcnnStream*

  std::atomic<bool> m_isExiting{false}; // 退出标志，中断while解码

  // 保存模型路径的内存，防止 ncnn c_str() 失效悬空
  std::string m_tokens;
  std::string m_encoder_p;
  std::string m_encoder_b;
  std::string m_decoder_p;
  std::string m_decoder_b;
  std::string m_joiner_p;
  std::string m_joiner_b;
};

#endif // LOCALASRSERVICE_H
