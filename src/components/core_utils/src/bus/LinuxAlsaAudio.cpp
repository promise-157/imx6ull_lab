#include "bus/LinuxAlsaAudio.h"
#include <alsa/asoundlib.h>
#include <fcntl.h>
#include <iostream>
#include <string.h>
#include <unistd.h>

#define PCM_DEV "hw:0,0"

LinuxAlsaAudio::LinuxAlsaAudio() {}

LinuxAlsaAudio::~LinuxAlsaAudio() {
  stopRecord();
  stopPlayback();
}

void LinuxAlsaAudio::setStatusCallback(StatusCallback cb) {
  m_statusCallback = cb;
}

void LinuxAlsaAudio::setAudioDataCallback(AudioDataCallback cb) {
  m_audioDataCallback = cb;
}

bool LinuxAlsaAudio::startRecord(const std::string &filePath) {
  if (m_isRecording || m_isPlaying)
    return false; // Prevent logic conflict

  if (m_recordThread.joinable()) {
    m_recordThread.join();
  }

  m_isRecording = true;
  m_recordThread =
      std::thread(&LinuxAlsaAudio::recordThreadFunc, this, filePath);
  return true;
}

void LinuxAlsaAudio::stopRecord() {
  if (m_isRecording) {
    m_isRecording = false;
    // 强制终止 ALSA 阻塞，如果不 drop，可能 join 死锁
    if (m_pcmHandle) {
      snd_pcm_drop((snd_pcm_t *)m_pcmHandle);
    }
  }
  if (m_recordThread.joinable()) {
    m_recordThread.join();
  }
}

bool LinuxAlsaAudio::startPlayback(const std::string &filePath) {
  if (m_isRecording || m_isPlaying)
    return false;

  if (m_playThread.joinable()) {
    m_playThread.join();
  }

  m_isPlaying = true;
  m_playThread =
      std::thread(&LinuxAlsaAudio::playbackThreadFunc, this, filePath);
  return true;
}

void LinuxAlsaAudio::stopPlayback() {
  if (m_isPlaying) {
    m_isPlaying = false;
    if (m_pcmHandle) {
      snd_pcm_drop((snd_pcm_t *)m_pcmHandle);
    }
  }
  if (m_playThread.joinable()) {
    m_playThread.join();
  }
}

void LinuxAlsaAudio::recordThreadFunc(std::string filePath) {
  snd_pcm_t *pcm = nullptr;
  snd_pcm_hw_params_t *hwparams = nullptr;
  int ret;
  unsigned int rate = 44100;
  snd_pcm_uframes_t period_size = 1024;
  unsigned int periods = 16;
  unsigned char *buf = nullptr;
  int fd = -1;

  ret = snd_pcm_open(&pcm, PCM_DEV, SND_PCM_STREAM_CAPTURE, 0);
  if (0 > ret) {
    if (m_statusCallback)
      m_statusCallback("录音开卡失败: " + std::string(snd_strerror(ret)), true);
    m_isRecording = false;
    return;
  }
  m_pcmHandle = pcm;

  snd_pcm_hw_params_malloc(&hwparams);
  snd_pcm_hw_params_any(pcm, hwparams);
  snd_pcm_hw_params_set_access(pcm, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED);
  snd_pcm_hw_params_set_format(pcm, hwparams, SND_PCM_FORMAT_S16_LE);
  snd_pcm_hw_params_set_rate(pcm, hwparams, rate, 0);
  snd_pcm_hw_params_set_channels(pcm, hwparams, 2);
  snd_pcm_hw_params_set_period_size(pcm, hwparams, period_size, 0);
  snd_pcm_hw_params_set_periods(pcm, hwparams, periods, 0);

  ret = snd_pcm_hw_params(pcm, hwparams);
  snd_pcm_hw_params_free(hwparams);
  if (0 > ret) {
    snd_pcm_close(pcm);
    if (m_statusCallback)
      m_statusCallback("录音参数设置失败: " + std::string(snd_strerror(ret)),
                       true);
    m_isRecording = false;
    return;
  }

  int buf_bytes = period_size * 4; // 16-bit stereo = 4 bytes per frame
  buf = (unsigned char *)malloc(buf_bytes);

  fd = open(filePath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
  if (fd < 0) {
    free(buf);
    snd_pcm_close(pcm);
    if (m_statusCallback)
      m_statusCallback("录音文件创建失败", true);
    m_isRecording = false;
    return;
  }

  if (m_statusCallback)
    m_statusCallback("正在录音中...", false);

  while (m_isRecording) {
    ret = snd_pcm_readi(pcm, buf, period_size);
    if (ret == -EPIPE) {
      snd_pcm_prepare(pcm); // Overrun
      continue;
    } else if (ret > 0) {
      // 数据供上层AI流式推理
      if (m_audioDataCallback) {
        int bytes = ret * 4; // 16bit = 2 bytes, 2 channels = 4 bytes per frame
        m_audioDataCallback((const char *)buf, bytes);
      }
      write(fd, buf, ret * 4);
    } else if (ret < 0) {
      break; // Other errors
    }
  }

  // Cleanup
  snd_pcm_drop(pcm);
  close(fd);
  free(buf);
  snd_pcm_close(pcm);
  m_pcmHandle = nullptr;

  m_isRecording = false;
  if (m_statusCallback)
    m_statusCallback("录音完毕，已保存", true);
}

void LinuxAlsaAudio::playbackThreadFunc(std::string filePath) {
  snd_pcm_t *pcm = nullptr;
  snd_pcm_hw_params_t *hwparams = nullptr;
  int ret;
  unsigned int rate = 44100;
  snd_pcm_uframes_t period_size = 1024;
  unsigned int periods = 16;
  unsigned char *buf = nullptr;
  int fd = -1;

  fd = open(filePath.c_str(), O_RDONLY);
  if (fd < 0) {
    if (m_statusCallback)
      m_statusCallback("找不到录音文件", true);
    m_isPlaying = false;
    return;
  }

  ret = snd_pcm_open(&pcm, PCM_DEV, SND_PCM_STREAM_PLAYBACK, 0);
  if (0 > ret) {
    close(fd);
    if (m_statusCallback)
      m_statusCallback("播放开卡失败: " + std::string(snd_strerror(ret)), true);
    m_isPlaying = false;
    return;
  }
  m_pcmHandle = pcm;

  snd_pcm_hw_params_malloc(&hwparams);
  snd_pcm_hw_params_any(pcm, hwparams);
  snd_pcm_hw_params_set_access(pcm, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED);
  snd_pcm_hw_params_set_format(pcm, hwparams, SND_PCM_FORMAT_S16_LE);
  snd_pcm_hw_params_set_rate(pcm, hwparams, rate, 0);
  snd_pcm_hw_params_set_channels(pcm, hwparams, 2);
  snd_pcm_hw_params_set_period_size(pcm, hwparams, period_size, 0);
  snd_pcm_hw_params_set_periods(pcm, hwparams, periods, 0);

  ret = snd_pcm_hw_params(pcm, hwparams);
  snd_pcm_hw_params_free(hwparams);
  if (0 > ret) {
    close(fd);
    snd_pcm_close(pcm);
    if (m_statusCallback)
      m_statusCallback("播放参数设置失败", true);
    m_isPlaying = false;
    return;
  }

  int buf_bytes = period_size * 4;
  buf = (unsigned char *)malloc(buf_bytes);
  if (m_statusCallback)
    m_statusCallback("正在播放录音...", false);

  while (m_isPlaying) {
    ret = read(fd, buf, buf_bytes);
    if (ret <= 0)
      break; // EOF or error

    int frames = ret / 4;
    int pcm_ret = snd_pcm_writei(pcm, buf, frames);
    if (pcm_ret == -EPIPE) {
      snd_pcm_prepare(pcm); // Underrun
      snd_pcm_writei(pcm, buf, frames);
    } else if (pcm_ret < 0) {
      break; // Other error
    }
  }

  // Cleanup
  snd_pcm_drop(pcm);
  close(fd);
  free(buf);
  snd_pcm_close(pcm);
  m_pcmHandle = nullptr;

  m_isPlaying = false;
  if (m_statusCallback)
    m_statusCallback("播放结束。", true);
}