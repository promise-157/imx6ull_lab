import re
import sys

def modify_h(file_path):
    with open(file_path, "r", encoding="utf-8") as f:
        content = f.read()
    
    # Remove QMediaPlayer includes and members
    content = content.replace('#include <QMediaPlayer>\n', '#include "EventBus.h"\n#include <QVariant>\n')
    content = content.replace("    QMediaPlayer *m_player;\n", "")
    content = content.replace("    void onPositionChanged(qint64 position);\n", "")
    content = content.replace("    void onDurationChanged(qint64 duration);\n", "")
    content = content.replace("    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);\n", "    void onMediaStatusChanged(int status);\n")

    with open(file_path, "w", encoding="utf-8") as f:
        f.write(content)

def modify_cpp(file_path):
    with open(file_path, "r", encoding="utf-8") as f:
        content = f.read()

    # Replace QMediaPlayer creation
    content = content.replace('  m_player = new QMediaPlayer(this);\n\n', '')
    
    # Replace init signals and slots for player
    # Since we can't easily parse C++ with regex safely, 
    # we just replace the known blocks exactly.
    old_signals = '''  // 播放器状态同步
  connect(m_player, &QMediaPlayer::positionChanged, this,
          &MusicPage::onPositionChanged);
  connect(m_player, &QMediaPlayer::durationChanged, this,
          &MusicPage::onDurationChanged);
  connect(m_progressSlider, &QSlider::sliderMoved, this,
          &MusicPage::onSliderMoved);
  connect(m_player, &QMediaPlayer::mediaStatusChanged, this,
          &MusicPage::onMediaStatusChanged);'''

    new_signals = '''  // 播放器状态同步由 EventBus 订阅替代
  EventBus::getInstance().subscribe("hal/pub/audio/position", [this](const QVariant& payload) {
      QMetaObject::invokeMethod(this, [this, payload](){
          qint64 pos = payload.toLongLong();
          m_progressSlider->setValue(pos);
          m_currentTimeLabel->setText(formatTime(pos));
      }, Qt::QueuedConnection);
  });
  EventBus::getInstance().subscribe("hal/pub/audio/duration", [this](const QVariant& payload) {
      QMetaObject::invokeMethod(this, [this, payload](){
          qint64 dur = payload.toLongLong();
          m_progressSlider->setMaximum(dur);
          m_totalTimeLabel->setText(formatTime(dur));
      }, Qt::QueuedConnection);
  });
  EventBus::getInstance().subscribe("hal/pub/audio/status", [this](const QVariant& payload) {
      QMetaObject::invokeMethod(this, [this, payload](){
          onMediaStatusChanged(payload.toInt());
      }, Qt::QueuedConnection);
  });
  EventBus::getInstance().subscribe("svc/pub/music/list", [this](const QVariant& payload) {
      QMetaObject::invokeMethod(this, [this, payload](){
          m_fileList->clear();
          m_fileList->addItems(payload.toStringList());
      }, Qt::QueuedConnection);
  });

  connect(m_progressSlider, &QSlider::sliderMoved, this,
          &MusicPage::onSliderMoved);'''

    content = content.replace(old_signals, new_signals)

    # remove slider volume from m_player directly
    content = content.replace('  m_player->setVolume(50);\n', '')

    old_scan = '''void MusicPage::scanMediaFiles() {
  QDir dir(m_currentDir);
  m_fileList->clear();
  m_fileList->addItems(dir.entryList({"*.mp3", "*.wav"}, QDir::Files));
}'''
    new_scan = '''void MusicPage::scanMediaFiles() {
  EventBus::getInstance().publish("svc/req/music/scan", QVariant::fromValue(m_currentDir));
}'''
    content = content.replace(old_scan, new_scan)

    # Replace stop and play implementations
    old_stop = '''void MusicPage::forceStop() {
  if (m_isPlaying) {
    m_player->stop();
    m_isPlaying = false;
    m_playBtn->setProperty("playing", false);
    updateBtnStyle(m_playBtn);
    m_cdWidget->stopRotation();
  }
}'''
    new_stop = '''void MusicPage::forceStop() {
  if (m_isPlaying) {
    EventBus::getInstance().publish("hal/req/audio/stop");
    m_isPlaying = false;
    m_playBtn->setProperty("playing", false);
    updateBtnStyle(m_playBtn);
    m_cdWidget->stopRotation();
  }
}'''
    content = content.replace(old_stop, new_stop)

    old_toggle = '''void MusicPage::togglePlay() {
  if (m_fileList->count() == 0)
    return;

  if (m_isPlaying) {
    m_player->pause();
    m_isPlaying = false;
    m_playBtn->setProperty("playing", false);
    updateBtnStyle(m_playBtn);
    m_cdWidget->stopRotation();
  } else {
    if (m_fileList->currentRow() < 0) {
      m_fileList->setCurrentRow(0);
    }
    onFileSelected(m_fileList->currentItem());
  }
}'''
    new_toggle = '''void MusicPage::togglePlay() {
  if (m_fileList->count() == 0) return;

  if (m_isPlaying) {
    EventBus::getInstance().publish("hal/req/audio/pause");
    m_isPlaying = false;
    m_playBtn->setProperty("playing", false);
    updateBtnStyle(m_playBtn);
    m_cdWidget->stopRotation();
  } else {
    // Resume or play new
    EventBus::getInstance().publish("hal/req/audio/play");
    m_isPlaying = true;
    m_playBtn->setProperty("playing", true);
    updateBtnStyle(m_playBtn);
    m_cdWidget->startRotation();
  }
}'''
    content = content.replace(old_toggle, new_toggle)

    old_next = '''void MusicPage::playNext(bool isAutoPlay) {
  if (m_fileList->count() == 0)
    return;

  int currentRow = m_fileList->currentRow();
  int total = m_fileList->count();

  if (m_playMode == Random) {
    currentRow = qrand() % total;
  } else if (m_playMode == LoopSingle && isAutoPlay) {
    // 保持当前 row
  } else {
    // LoopList 或 手动切歌
    currentRow = (currentRow + 1) % total;
  }

  m_fileList->setCurrentRow(currentRow);
  onFileSelected(m_fileList->item(currentRow));
}'''
    new_next = '''void MusicPage::playNext(bool isAutoPlay) {
  EventBus::getInstance().publish("svc/req/music/next", QVariant::fromValue((int)m_playMode));
}'''
    content = content.replace(old_next, new_next)

    old_prev = '''void MusicPage::playPrevious() {
  if (m_fileList->count() == 0)
    return;

  int currentRow = m_fileList->currentRow();
  int total = m_fileList->count();

  if (m_playMode == Random) {
    currentRow = qrand() % total;
  } else {
    currentRow = (currentRow - 1 + total) % total;
  }

  m_fileList->setCurrentRow(currentRow);
  onFileSelected(m_fileList->item(currentRow));
}'''
    new_prev = '''void MusicPage::playPrevious() {
  EventBus::getInstance().publish("svc/req/music/prev", QVariant::fromValue((int)m_playMode));
}'''
    content = content.replace(old_prev, new_prev)

    old_onselect = '''void MusicPage::onFileSelected(QListWidgetItem *item) {
  if (!item)
    return;

  QString fileName = item->text();
  QString filePath = QDir(m_currentDir).absoluteFilePath(fileName);

  m_player->setMedia(QUrl::fromLocalFile(filePath));
  m_player->play();

  m_isPlaying = true;
  m_playBtn->setProperty("playing", true);
  updateBtnStyle(m_playBtn);

  m_cdWidget->startRotation();
}'''
    new_onselect = '''void MusicPage::onFileSelected(QListWidgetItem *item) {
  if (!item) return;
  int index = m_fileList->row(item);
  EventBus::getInstance().publish("svc/req/music/play_index", QVariant::fromValue(index));

  m_isPlaying = true;
  m_playBtn->setProperty("playing", true);
  updateBtnStyle(m_playBtn);
  m_cdWidget->startRotation();
}'''
    content = content.replace(old_onselect, new_onselect)

    old_status = '''void MusicPage::onMediaStatusChanged(QMediaPlayer::MediaStatus status) {
  if (status == QMediaPlayer::EndOfMedia) {
    playNext(true);
  }
}'''
    new_status = '''void MusicPage::onMediaStatusChanged(int status) {
  // 7 is QMediaPlayer::EndOfMedia. If we pass 7 from AudioHal
  if (status == 7) { 
    playNext(true);
  }
}'''
    content = content.replace(old_status, new_status)
    content = content.replace("void MusicPage::onPositionChanged(qint64 position)","//void MusicPage::onPositionChanged(qint64 position)")
    content = content.replace("void MusicPage::onDurationChanged(qint64 duration)","//void MusicPage::onDurationChanged(qint64 duration)")

    with open(file_path, "w", encoding="utf-8") as f:
        f.write(content)


modify_h("/home/promise/imx6ull_lab/components/gui_pages/include/MusicPage.h")
modify_cpp("/home/promise/imx6ull_lab/components/gui_pages/src/MusicPage.cpp")
