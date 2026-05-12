#ifndef LINUX_ALSA_AUDIO_H
#define LINUX_ALSA_AUDIO_H

#include <atomic>
#include <functional>
#include <string>
#include <thread>

class LinuxAlsaAudio {
public:
  LinuxAlsaAudio();
  ~LinuxAlsaAudio();

  // 状态回调：消息内容，是否是播放/录音完成的结束事件
  using StatusCallback =
      std::function<void(const std::string &msg, bool isFinished)>;
  void setStatusCallback(StatusCallback cb);

  // 原始 PCM 数据流回调 (用于模型计算)
  using AudioDataCallback = std::function<void(const char *data, int bytes)>;
  void setAudioDataCallback(AudioDataCallback cb);

  bool startRecord(const std::string &filePath);
  void stopRecord();

  bool startPlayback(const std::string &filePath);
  void stopPlayback();

private:
  void recordThreadFunc(std::string filePath);
  void playbackThreadFunc(std::string filePath);

  std::atomic<bool> m_isRecording{false};
  std::atomic<bool> m_isPlaying{false};

  std::thread m_recordThread;
  std::thread m_playThread;

  void *m_pcmHandle{nullptr};

  StatusCallback m_statusCallback;
  AudioDataCallback m_audioDataCallback;
};

#endif // LINUX_ALSA_AUDIO_H
