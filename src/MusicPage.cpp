#include "MusicPage.h"
#include <QSettings>
#include <QCoreApplication>
#include <QDir>
#include <QFileDialog>
#include <QStyle>
#include <QFile>

MusicPage::MusicPage(QWidget *parent) : QWidget(parent) {
    // 1. 设置对象名（用于 QSS 选择器）
    this->setObjectName("MusicPage");
    
    // 2. 加载私有样式表
    initStyle();

    // 3. 构建布局 [内容区 | 切换柄 | 歌单区]
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);

    // --- 内容区 ---
    QWidget *contentArea = new QWidget();
    contentArea->setObjectName("MusicContentArea");
    QVBoxLayout *contentLayout = new QVBoxLayout(contentArea);
    
    m_titleLabel = new QLabel("Ready to Play");
    m_titleLabel->setObjectName("MusicTitle");
    m_titleLabel->setAlignment(Qt::AlignCenter);

    QLabel *albumArt = new QLabel();
    albumArt->setObjectName("AlbumArt");
    albumArt->setPixmap(QPixmap(":/res/images/music/cd.png").scaled(300, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    albumArt->setAlignment(Qt::AlignCenter);

    m_playBtn = new QPushButton();
    m_playBtn->setObjectName("PlayBtn");
    m_playBtn->setFixedSize(80, 80);
    m_playBtn->setProperty("state", "play"); // 初始状态

    contentLayout->addStretch();
    contentLayout->addWidget(m_titleLabel);
    contentLayout->addWidget(albumArt);
    contentLayout->addSpacing(30);
    contentLayout->addWidget(m_playBtn, 0, Qt::AlignCenter);
    contentLayout->addStretch();

    // --- 切换柄 (三角形图标按钮) ---
    m_toggleBtn = new QPushButton();
    m_toggleBtn->setObjectName("ToggleBtn");
    m_toggleBtn->setFixedWidth(35);
    m_toggleBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    m_toggleBtn->setProperty("state", "closed"); // 初始状态

    // --- 歌单抽屉 ---
    m_drawer = new QWidget();
    m_drawer->setObjectName("MusicDrawer");
    m_drawer->setFixedWidth(250);
    m_drawer->setVisible(false); // 默认收起
    
    QVBoxLayout *drawerLayout = new QVBoxLayout(m_drawer);
    QPushButton *btnDir = new QPushButton("Browse...");
    btnDir->setObjectName("DirBtn");
    
    m_fileList = new QListWidget();
    m_fileList->setObjectName("MusicList");

    drawerLayout->addWidget(new QLabel("PLAYLIST", m_drawer));
    drawerLayout->addWidget(m_fileList);
    drawerLayout->addWidget(btnDir);

    mainLayout->addWidget(contentArea, 1);
    mainLayout->addWidget(m_toggleBtn);
    mainLayout->addWidget(m_drawer);

    // 4. 逻辑初始化
    m_player = new QMediaPlayer(this);
    loadSettings();
    scanMediaFiles();

    // 5. 信号绑定
    connect(m_playBtn, &QPushButton::clicked, this, &MusicPage::togglePlay);
    connect(m_toggleBtn, &QPushButton::clicked, this, &MusicPage::toggleDrawer);
    connect(btnDir, &QPushButton::clicked, this, &MusicPage::changeMediaDir);
    connect(m_fileList, &QListWidget::itemClicked, this, &MusicPage::onFileSelected);
}

void MusicPage::initStyle() {
    QFile file(":/res/style/MusicPage.qss");
    if (file.open(QFile::ReadOnly)) {
        this->setStyleSheet(QLatin1String(file.readAll()));
        file.close();
    }
}

void MusicPage::updateBtnStyle(QPushButton* btn) {
    btn->style()->unpolish(btn);
    btn->style()->polish(btn);
    btn->update();
}

void MusicPage::toggleDrawer() {
    bool isVisible = m_drawer->isVisible();
    m_drawer->setVisible(!isVisible);
    m_toggleBtn->setProperty("state", isVisible ? "closed" : "open");
    updateBtnStyle(m_toggleBtn);
}

void MusicPage::togglePlay() {
    if (m_player->mediaStatus() == QMediaPlayer::NoMedia) return;
    m_isPlaying = !m_isPlaying;
    if (m_isPlaying) m_player->play(); else m_player->pause();
    
    m_playBtn->setProperty("state", m_isPlaying ? "pause" : "play");
    updateBtnStyle(m_playBtn);
}

void MusicPage::onFileSelected(QListWidgetItem *item) {
    QString path = m_currentDir + "/" + item->text();
    m_player->setMedia(QUrl::fromLocalFile(path));
    m_titleLabel->setText(item->text());
    m_player->play();
    m_isPlaying = true;
    m_playBtn->setProperty("state", "pause");
    updateBtnStyle(m_playBtn);
}

void MusicPage::loadSettings() {
    QSettings set(QCoreApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat);
    m_currentDir = set.value("Paths/music_dir", QCoreApplication::applicationDirPath() + "/media").toString();
}

void MusicPage::saveSettings(QString path) {
    QSettings set(QCoreApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat);
    set.setValue("Paths/music_dir", path);
}

void MusicPage::scanMediaFiles() {
    QDir dir(m_currentDir);
    m_fileList->clear();
    m_fileList->addItems(dir.entryList({"*.mp3", "*.wav"}, QDir::Files));
}

void MusicPage::changeMediaDir() {
    QString path = QFileDialog::getExistingDirectory(this, "Select Music Folder", m_currentDir);
    if (!path.isEmpty()) {
        m_currentDir = path;
        saveSettings(path);
        scanMediaFiles();
    }
}