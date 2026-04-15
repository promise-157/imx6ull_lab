#include "VideoPage.h"
#include <QSettings>
#include <QCoreApplication>
#include <QDir>
#include <QFileDialog>
#include <QStyle>
#include <QFile>

VideoPage::VideoPage(QWidget *parent) : QWidget(parent) {
    this->setObjectName("VideoPage");
    initStyle();

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);

    // --- 视频与控制区 ---
    QWidget *videoContainer = new QWidget();
    QVBoxLayout *vAreaLayout = new QVBoxLayout(videoContainer);
    vAreaLayout->setContentsMargins(0,0,0,0);
    vAreaLayout->setSpacing(0);

    m_videoWidget = new QVideoWidget();
    m_videoWidget->setObjectName("VideoCanvas");

    QWidget *controlBar = new QWidget();
    controlBar->setObjectName("VideoControlBar");
    controlBar->setFixedHeight(80);
    QHBoxLayout *cLayout = new QHBoxLayout(controlBar);

    m_playBtn = new QPushButton();
    m_playBtn->setObjectName("VideoPlayBtn");
    m_playBtn->setFixedSize(60, 60);
    m_playBtn->setProperty("state", "play");

    cLayout->addStretch();
    cLayout->addWidget(m_playBtn);
    cLayout->addStretch();

    vAreaLayout->addWidget(m_videoWidget, 1);
    vAreaLayout->addWidget(controlBar);

    // --- 抽屉柄 ---
    m_toggleBtn = new QPushButton();
    m_toggleBtn->setObjectName("VideoToggleBtn");
    m_toggleBtn->setFixedWidth(35);
    m_toggleBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    m_toggleBtn->setProperty("state", "closed");

    // --- 视频列表抽屉 ---
    m_drawer = new QWidget();
    m_drawer->setObjectName("VideoDrawer");
    m_drawer->setFixedWidth(240);
    m_drawer->setVisible(false);
    
    QVBoxLayout *drawerLayout = new QVBoxLayout(m_drawer);
    QPushButton *btnDir = new QPushButton("Set Path");
    btnDir->setObjectName("VideoDirBtn");
    
    m_fileList = new QListWidget();
    m_fileList->setObjectName("VideoList");

    drawerLayout->addWidget(new QLabel("MOVIE LIST"));
    drawerLayout->addWidget(m_fileList);
    drawerLayout->addWidget(btnDir);

    mainLayout->addWidget(videoContainer, 1);
    mainLayout->addWidget(m_toggleBtn);
    mainLayout->addWidget(m_drawer);

    m_player = new QMediaPlayer(this);
    m_player->setVideoOutput(m_videoWidget);

    connect(m_playBtn, &QPushButton::clicked, this, &VideoPage::togglePlay);
    connect(m_toggleBtn, &QPushButton::clicked, this, &VideoPage::toggleDrawer);
    connect(btnDir, &QPushButton::clicked, this, &VideoPage::changeVideoDir);
    connect(m_fileList, &QListWidget::itemClicked, this, &VideoPage::onFileSelected);

    loadSettings();
    scanVideoFiles();
}

void VideoPage::initStyle() {
    QFile file(":/res/style/VideoPage.qss");
    if(file.open(QFile::ReadOnly)) {
        this->setStyleSheet(QLatin1String(file.readAll()));
        file.close();
    }
}

void VideoPage::updateBtnStyle(QPushButton* btn) {
    btn->style()->unpolish(btn);
    btn->style()->polish(btn);
    btn->update();
}

void VideoPage::togglePlay() {
    if (m_player->mediaStatus() == QMediaPlayer::NoMedia) return;
    m_isPlaying = !m_isPlaying;
    if (m_isPlaying) m_player->play(); else m_player->pause();
    m_playBtn->setProperty("state", m_isPlaying ? "pause" : "play");
    updateBtnStyle(m_playBtn);
}

void VideoPage::toggleDrawer() {
    bool vis = m_drawer->isVisible();
    m_drawer->setVisible(!vis);
    m_toggleBtn->setProperty("state", vis ? "closed" : "open");
    updateBtnStyle(m_toggleBtn);
}

void VideoPage::onFileSelected(QListWidgetItem *item) {
    m_player->setMedia(QUrl::fromLocalFile(m_currentDir + "/" + item->text()));
    m_player->play();
    m_isPlaying = true;
    m_playBtn->setProperty("state", "pause");
    updateBtnStyle(m_playBtn);
}

void VideoPage::loadSettings() {
    QSettings set(QCoreApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat);
    m_currentDir = set.value("Paths/video_dir", QCoreApplication::applicationDirPath() + "/media").toString();
}

void VideoPage::saveSettings(QString path) {
    QSettings set(QCoreApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat);
    set.setValue("Paths/video_dir", path);
}

void VideoPage::scanVideoFiles() {
    QDir dir(m_currentDir);
    m_fileList->clear();
    m_fileList->addItems(dir.entryList({"*.mp4", "*.avi", "*.mkv"}, QDir::Files));
}

void VideoPage::changeVideoDir() {
    QString path = QFileDialog::getExistingDirectory(this, "Select Video Folder", m_currentDir);
    if(!path.isEmpty()){ m_currentDir = path; saveSettings(path); scanVideoFiles(); }
}