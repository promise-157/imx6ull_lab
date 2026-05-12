#include "VideoPage.h"
#include "EventBus.h"
#include <QCoreApplication>
#include <QFile>
#include <QFileDialog>
#include <QSettings>
#include <QStyle>
#include <QTime>

VideoPage::VideoPage(QWidget *parent) : IAppModule(parent) {
  this->setObjectName("VideoPage");

  QFile file(":/res/style/VideoPage.qss");
  if (file.open(QFile::ReadOnly)) {
    this->setStyleSheet(QLatin1String(file.readAll()));
    file.close();
  }

  QVBoxLayout *rootLayout = new QVBoxLayout(this);
  rootLayout->setContentsMargins(0, 0, 0, 0);
  rootLayout->setSpacing(0);

  // ================= 顶部栏 =================
  QWidget *topBar = new QWidget(this);
  topBar->setObjectName("VideoTopBar");
  topBar->setFixedHeight(50);
  QHBoxLayout *topLayout = new QHBoxLayout(topBar);
  topLayout->setContentsMargins(10, 0, 10, 0);

  m_closeBtn = new QPushButton(topBar);
  m_closeBtn->setFixedSize(20, 20);
  m_closeBtn->setToolTip("彻底退出");
  m_closeBtn->setObjectName("VideoCloseBtn");

  m_backBtn = new QPushButton(topBar);
  m_backBtn->setFixedSize(20, 20);
  m_backBtn->setToolTip("收起后台");
  m_backBtn->setObjectName("VideoBackBtn");

  m_topTitleLabel = new QLabel("Local Video Player", topBar);
  m_topTitleLabel->setAlignment(Qt::AlignCenter);
  m_topTitleLabel->setObjectName("VideoTopTitle");

  m_browseBtn = new QPushButton(topBar);
  m_browseBtn->setFixedSize(24, 24);
  m_browseBtn->setToolTip("选择视频目录");
  m_browseBtn->setObjectName("VideoBrowseBtn");

  m_menuBtn = new QPushButton(topBar);
  m_menuBtn->setFixedSize(24, 24);
  m_menuBtn->setToolTip("视频列表");
  m_menuBtn->setObjectName("VideoMenuBtn");

  topLayout->addWidget(m_closeBtn);
  topLayout->addSpacing(10);
  topLayout->addWidget(m_backBtn);
  topLayout->addStretch();
  topLayout->addWidget(m_topTitleLabel);
  topLayout->addStretch();
  topLayout->addWidget(m_browseBtn);
  topLayout->addSpacing(15);
  topLayout->addWidget(m_menuBtn);

  rootLayout->addWidget(topBar);

  // ================= 主体显示区域 =================
  QWidget *mainBody = new QWidget(this);
  QHBoxLayout *mainBodyLayout = new QHBoxLayout(mainBody);
  mainBodyLayout->setContentsMargins(0, 0, 0, 0);
  mainBodyLayout->setSpacing(0);

  // 左侧：视频区域 + 控制栏
  QWidget *playerContainer = new QWidget(mainBody);
  QVBoxLayout *playerLayout = new QVBoxLayout(playerContainer);
  playerLayout->setContentsMargins(0, 0, 0, 0);
  playerLayout->setSpacing(0);

  m_videoWidget = new QVideoWidget(playerContainer);
  m_videoWidget->setObjectName("VideoWidget");
  // 关键修正：保持视频长宽比缩放
  m_videoWidget->setAspectRatioMode(Qt::KeepAspectRatio);

  // -- 控制栏 --
  QWidget *controlBar = new QWidget(playerContainer);
  controlBar->setFixedHeight(60);
  controlBar->setObjectName("VideoControlBar");
  QHBoxLayout *cLayout = new QHBoxLayout(controlBar);
  cLayout->setContentsMargins(15, 0, 15, 0);

  m_prevBtn = new QPushButton(controlBar);
  m_prevBtn->setFixedSize(30, 30);
  m_prevBtn->setObjectName("VideoPrevBtn");

  m_playBtn = new QPushButton(controlBar);
  m_playBtn->setFixedSize(40, 40);
  m_playBtn->setObjectName("VideoPlayBtn");
  m_playBtn->setProperty("isPlaying", false);

  m_nextBtn = new QPushButton(controlBar);
  m_nextBtn->setFixedSize(30, 30);
  m_nextBtn->setObjectName("VideoNextBtn");

  m_timeLabel = new QLabel("00:00 / 00:00", controlBar);
  m_timeLabel->setObjectName("VideoTimeLabel");

  m_progressSlider = new QSlider(Qt::Horizontal, controlBar);
  m_progressSlider->setObjectName("VideoProgressSlider");

  QLabel *volIcon = new QLabel(controlBar);
  volIcon->setFixedSize(24, 24);
  volIcon->setObjectName("VideoVolIcon");

  m_volumeSlider = new QSlider(Qt::Horizontal, controlBar);
  m_volumeSlider->setFixedWidth(80);
  m_volumeSlider->setRange(0, 100);
  m_volumeSlider->setValue(50);
  m_volumeSlider->setObjectName("VideoVolSlider");

  cLayout->addWidget(m_prevBtn);
  cLayout->addSpacing(10);
  cLayout->addWidget(m_playBtn);
  cLayout->addSpacing(10);
  cLayout->addWidget(m_nextBtn);
  cLayout->addSpacing(20);
  cLayout->addWidget(m_timeLabel);
  cLayout->addSpacing(10);
  cLayout->addWidget(m_progressSlider);
  cLayout->addSpacing(20);
  cLayout->addWidget(volIcon);
  cLayout->addWidget(m_volumeSlider);

  playerLayout->addWidget(m_videoWidget, 1);
  playerLayout->addWidget(controlBar);

  // 右侧：播放列表抽屉
  m_drawer = new QWidget(mainBody);
  m_drawer->setFixedWidth(240);
  m_drawer->setVisible(false);
  m_drawer->setObjectName("VideoDrawer");

  QVBoxLayout *drawerLayout = new QVBoxLayout(m_drawer);
  drawerLayout->setContentsMargins(5, 10, 5, 10);

  QLabel *listTitle = new QLabel("视频列表", m_drawer);
  listTitle->setAlignment(Qt::AlignCenter);
  listTitle->setObjectName("VideoListTitle");

  m_fileList = new QListWidget(m_drawer);
  m_fileList->setObjectName("VideoFileList");

  drawerLayout->addWidget(listTitle);
  drawerLayout->addWidget(m_fileList);

  mainBodyLayout->addWidget(playerContainer, 1);
  mainBodyLayout->addWidget(m_drawer);

  rootLayout->addWidget(mainBody, 1);

  // ================= 播放器引擎 =================
  m_player = new QMediaPlayer(this);
  m_player->setVideoOutput(m_videoWidget);

  // 信号绑定
  connect(m_playBtn, &QPushButton::clicked, this, &VideoPage::togglePlay);
  connect(m_menuBtn, &QPushButton::clicked, this, &VideoPage::toggleDrawer);
  connect(m_browseBtn, &QPushButton::clicked, this, &VideoPage::changeVideoDir);
  connect(m_fileList, &QListWidget::itemClicked, this,
          &VideoPage::onFileSelected);

  connect(m_prevBtn, &QPushButton::clicked, this, &VideoPage::playPrevious);
  connect(m_nextBtn, &QPushButton::clicked, this, &VideoPage::playNext);
  connect(m_volumeSlider, &QSlider::valueChanged, this,
          &VideoPage::changeVolume);

  connect(m_player, &QMediaPlayer::positionChanged, this,
          &VideoPage::onPositionChanged);
  connect(m_player, &QMediaPlayer::durationChanged, this,
          &VideoPage::onDurationChanged);
  connect(m_progressSlider, &QSlider::sliderMoved, this,
          &VideoPage::onSliderMoved);
  connect(m_player, &QMediaPlayer::mediaStatusChanged, this,
          [this](QMediaPlayer::MediaStatus status) {
            if (status == QMediaPlayer::EndOfMedia) {
              playNext();
            }
          });

  connect(m_backBtn, &QPushButton::clicked, this, &VideoPage::requestMinimize);
  connect(m_closeBtn, &QPushButton::clicked, this, [this]() {
    m_player->stop();
    emit requestClose();
  });

  EventBus::getInstance()->subscribe(
      "svc/pub/video/list", this, [this](const QVariant &payload) {
        m_fileList->clear();
        m_fileList->addItems(payload.toStringList());
      });

  EventBus::getInstance()->subscribe(
      "svc/pub/video/current_index", this, [this](const QVariant &payload) {
        const int index = payload.toInt();
        if (index >= 0 && index < m_fileList->count()) {
          m_fileList->setCurrentRow(index);
          m_topTitleLabel->setText(m_fileList->item(index)->text());
        }
      });

  EventBus::getInstance()->subscribe(
      "svc/pub/video/play_path", this, [this](const QVariant &payload) {
        const QString filePath = payload.toString();
        if (filePath.isEmpty())
          return;
        m_player->setMedia(QUrl::fromLocalFile(filePath));
        m_player->play();
        m_isPlaying = true;
        m_playBtn->setProperty("isPlaying", true);
        m_playBtn->style()->unpolish(m_playBtn);
        m_playBtn->style()->polish(m_playBtn);
      });

  loadSettings();
  scanVideoFiles();
}

void VideoPage::keyPressEvent(QKeyEvent *event) {
  // 按空格也可以控制播放暂停
  if (event->key() == Qt::Key_Space) {
    togglePlay();
  }
}

void VideoPage::togglePlay() {
  if (m_player->mediaStatus() == QMediaPlayer::NoMedia) {
    if (m_fileList->count() > 0) {
      m_fileList->setCurrentRow(0);
      onFileSelected(m_fileList->currentItem());
    }
    return;
  }
  m_isPlaying = !m_isPlaying;
  if (m_isPlaying) {
    m_player->play();
    m_playBtn->setProperty("isPlaying", true);
  } else {
    m_player->pause();
    m_playBtn->setProperty("isPlaying", false);
  }
  m_playBtn->style()->unpolish(m_playBtn);
  m_playBtn->style()->polish(m_playBtn);
}

void VideoPage::toggleDrawer() { m_drawer->setVisible(!m_drawer->isVisible()); }

void VideoPage::onFileSelected(QListWidgetItem *item) {
  if (!item)
    return;
  const int index = m_fileList->row(item);
  EventBus::getInstance()->publish("svc/req/video/play_index",
                                   QVariant::fromValue(index));
}

void VideoPage::playNext() {
  EventBus::getInstance()->publish("svc/req/video/next");
}

void VideoPage::playPrevious() {
  EventBus::getInstance()->publish("svc/req/video/prev");
}

void VideoPage::changeVolume(int value) { m_player->setVolume(value); }

void VideoPage::onPositionChanged(qint64 position) {
  if (!m_progressSlider->isSliderDown()) {
    m_progressSlider->setValue(position);
  }
  m_timeLabel->setText(formatTime(position) + " / " +
                       formatTime(m_player->duration()));
}

void VideoPage::onDurationChanged(qint64 duration) {
  m_progressSlider->setRange(0, duration);
  m_timeLabel->setText(formatTime(m_player->position()) + " / " +
                       formatTime(duration));
}

void VideoPage::onSliderMoved(int position) { m_player->setPosition(position); }

QString VideoPage::formatTime(qint64 ms) {
  int seconds = (ms / 1000) % 60;
  int minutes = (ms / 60000) % 60;
  int hours = (ms / 3600000) % 24;
  QTime time(hours, minutes, seconds);
  return hours > 0 ? time.toString("hh:mm:ss") : time.toString("mm:ss");
}

void VideoPage::loadSettings() {
  QSettings set(QCoreApplication::applicationDirPath() + "/config.ini",
                QSettings::IniFormat);
  m_currentDir = set.value("Paths/video_dir",
                           QCoreApplication::applicationDirPath() + "/media")
                     .toString();
}

void VideoPage::saveSettings(QString path) {
  QSettings set(QCoreApplication::applicationDirPath() + "/config.ini",
                QSettings::IniFormat);
  set.setValue("Paths/video_dir", path);
}

void VideoPage::scanVideoFiles() {
  EventBus::getInstance()->publish("svc/req/video/scan",
                                   QVariant::fromValue(m_currentDir));
}

void VideoPage::changeVideoDir() {
  QString path = QFileDialog::getExistingDirectory(this, "Select Video Folder",
                                                   m_currentDir);
  if (!path.isEmpty()) {
    m_currentDir = path;
    saveSettings(path);
    scanVideoFiles();
  }
}
