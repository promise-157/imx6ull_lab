#include "MusicPage.h"
#include <QSettings>
#include <QCoreApplication>
#include <QDir>
#include <QFileDialog>
#include <QStyle>
#include <QFile>
#include <QPainter>
#include <QTime>
#include <QRandomGenerator>

// ==========================================
// CdWidget 实现：用于绘制带有旋转动画的CD封面
// ==========================================
CdWidget::CdWidget(QWidget *parent) : QWidget(parent), m_angle(0.0) {
    setMinimumSize(300, 300);
    m_cdPixmap = QPixmap(":/res/images/music/cd.png");
    
    m_timer = new QTimer(this);
    m_timer->setInterval(30); // 约 33 帧/秒
    connect(m_timer, &QTimer::timeout, this, &CdWidget::updateRotation);
}

void CdWidget::startRotation() {
    m_timer->start();
}

void CdWidget::stopRotation() {
    m_timer->stop();
}

void CdWidget::updateRotation() {
    m_angle += 1.0; // 每次转动的角度
    if (m_angle >= 360.0) m_angle -= 360.0;
    update(); 
}

void CdWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    if (m_cdPixmap.isNull()) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); 
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    int side = qMin(width(), height());
    painter.translate(width() / 2.0, height() / 2.0);
    painter.rotate(m_angle);

    int imgSize = side * 0.9; 
    QRect targetRect(-imgSize / 2, -imgSize / 2, imgSize, imgSize);
    painter.drawPixmap(targetRect, m_cdPixmap);
}


// ==========================================
// MusicPage 实现
// ==========================================
MusicPage::MusicPage(QWidget *parent) : IAppModule(parent) {
    this->setAttribute(Qt::WA_StyledBackground, true);
    this->setObjectName("MusicPage");

    m_player = new QMediaPlayer(this);

    initUI();
    initStyle();
    loadSettings();
    scanMediaFiles();

    // -- 信号与槽 --
    connect(m_playBtn, &QPushButton::clicked, this, &MusicPage::togglePlay);
    connect(m_nextBtn, &QPushButton::clicked, this, [this](){ playNext(false); });
    connect(m_prevBtn, &QPushButton::clicked, this, &MusicPage::playPrevious);
    connect(m_browseBtn, &QPushButton::clicked, this, &MusicPage::changeMediaDir);
    connect(m_fileList, &QListWidget::itemClicked, this, &MusicPage::onFileSelected);
    connect(m_backBtn, &QPushButton::clicked, this, &MusicPage::requestMinimize);
    connect(m_closeBtn, &QPushButton::clicked, this, [this](){ forceStop(); emit requestClose(); }); 

    // 播放模式、收藏、音量
    connect(m_playModeBtn, &QPushButton::clicked, this, &MusicPage::togglePlayMode);
    connect(m_favoriteBtn, &QPushButton::clicked, this, &MusicPage::toggleFavorite);

    // 播放器状态同步
    connect(m_player, &QMediaPlayer::positionChanged, this, &MusicPage::onPositionChanged);
    connect(m_player, &QMediaPlayer::durationChanged, this, &MusicPage::onDurationChanged);
    connect(m_progressSlider, &QSlider::sliderMoved, this, &MusicPage::onSliderMoved);
    connect(m_player, &QMediaPlayer::mediaStatusChanged, this, &MusicPage::onMediaStatusChanged);
}

MusicPage::~MusicPage() {}

void MusicPage::initUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(0);

    // ================= 1. 顶部栏 =================
        // 新风格顶部栏
    QWidget *topBar = new QWidget(this);
    topBar->setObjectName("MusicTopBar");
    topBar->setFixedHeight(50);
    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(10, 0, 10, 0);

    // 关闭(左1)
    m_closeBtn = new QPushButton(topBar);
    m_closeBtn->setFixedSize(20, 20);
    m_closeBtn->setToolTip("彻底退出");
    m_closeBtn->setStyleSheet("QPushButton { border-image: url(:/res/images/music/close_new.png); background: transparent; }");

    // 最小化(左2)
    m_backBtn = new QPushButton(topBar);
    m_backBtn->setFixedSize(20, 20);
    m_backBtn->setToolTip("收起后台");
    m_backBtn->setStyleSheet("QPushButton { border-image: url(:/res/images/music/minimize_new.png); background: transparent; }");

    m_topTitleLabel = new QLabel("Local Music Player", topBar);
    m_topTitleLabel->setObjectName("TopTitle");
    m_topTitleLabel->setAlignment(Qt::AlignCenter);

    // 原来的BrowseBtn (可保留在原来位置或者调整)
    m_browseBtn = new QPushButton(topBar);
    m_browseBtn->setObjectName("BrowseBtn");
    m_browseBtn->setFixedSize(24, 24);
    m_browseBtn->setToolTip("选择音乐目录");
    m_browseBtn->setStyleSheet("QPushButton { border-image: url(:/res/images/music/folder_new.png); background: transparent; }");
    
    // 菜单(最右侧)
    QPushButton *m_menuBtn = new QPushButton(topBar);
    m_menuBtn->setFixedSize(24, 24);
    m_menuBtn->setToolTip("播放列表");
    m_menuBtn->setStyleSheet("QPushButton { border-image: url(:/res/images/music/menu_new.png); background: transparent; }");
    connect(m_menuBtn, &QPushButton::clicked, this, &MusicPage::toggleDrawer);

    // 左侧按钮
    topLayout->addWidget(m_closeBtn);
    topLayout->addSpacing(10);
    topLayout->addWidget(m_backBtn);
    
    // 居中标题
    topLayout->addStretch();
    topLayout->addWidget(m_topTitleLabel);
    topLayout->addStretch();
    
    // 右侧按钮
    topLayout->addWidget(m_browseBtn);
    topLayout->addSpacing(15);
    topLayout->addWidget(m_menuBtn);


    // ================= 2. 中间区域 =================
    QWidget *centerArea = new QWidget(this);
    QHBoxLayout *centerLayout = new QHBoxLayout(centerArea);
    
    m_cdWidget = new CdWidget(centerArea);
    
    m_drawer = new QWidget(centerArea);
    m_drawer->setObjectName("MusicDrawer");
    m_drawer->setFixedWidth(250);
    QVBoxLayout *drawerLayout = new QVBoxLayout(m_drawer);
    drawerLayout->setContentsMargins(0,0,0,0);
    QLabel *listTitle = new QLabel("PLAYLIST", m_drawer);
    listTitle->setObjectName("ListTitle");
    m_fileList = new QListWidget(m_drawer);
    m_fileList->setObjectName("MusicList");
    drawerLayout->addWidget(listTitle);
    drawerLayout->addWidget(m_fileList);

    centerLayout->addStretch(1);
    centerLayout->addWidget(m_cdWidget, 2);
    centerLayout->addStretch(1);
    centerLayout->addWidget(m_drawer); 

    // ================= 3. 底部控制栏 =================
    QWidget *bottomBar = new QWidget(this);
    bottomBar->setObjectName("BottomBar");
    QVBoxLayout *bottomLayout = new QVBoxLayout(bottomBar);

    QHBoxLayout *progressLayout = new QHBoxLayout();
    m_currentTimeLabel = new QLabel("00:00");
    m_currentTimeLabel->setObjectName("TimeLabel");
    m_totalTimeLabel = new QLabel("00:00");
    m_totalTimeLabel->setObjectName("TimeLabel");
    m_progressSlider = new QSlider(Qt::Horizontal);
    m_progressSlider->setObjectName("ProgressSlider");
    
    progressLayout->addWidget(m_currentTimeLabel);
    progressLayout->addWidget(m_progressSlider);
    progressLayout->addWidget(m_totalTimeLabel);

    QHBoxLayout *ctrlLayout = new QHBoxLayout();
    
    m_playModeBtn = new QPushButton();
    m_playModeBtn->setObjectName("ModeBtn");
    m_playModeBtn->setProperty("mode", "list");
    m_playModeBtn->setFixedSize(40, 40);

    m_prevBtn = new QPushButton();
    m_prevBtn->setObjectName("PrevBtn");
    m_prevBtn->setFixedSize(50, 50);

    m_playBtn = new QPushButton();
    m_playBtn->setObjectName("PlayBtn");
    m_playBtn->setProperty("state", "play");
    m_playBtn->setFixedSize(65, 65);

    m_nextBtn = new QPushButton();
    m_nextBtn->setObjectName("NextBtn");
    m_nextBtn->setFixedSize(50, 50);

    m_volumeBtn = new QPushButton();
    m_volumeBtn->setObjectName("VolumeBtn");
    m_volumeBtn->setProperty("state", "on");
    m_volumeBtn->setFixedSize(40, 40);

    m_volumeSlider = new QSlider(Qt::Horizontal);
    m_volumeSlider->setObjectName("VolumeSlider");
    m_volumeSlider->setRange(0, 100);
    m_volumeSlider->setValue(50);
    m_volumeSlider->setFixedWidth(80);
    m_player->setVolume(50);
    connect(m_volumeSlider, &QSlider::valueChanged, this, &MusicPage::changeVolume);

    m_favoriteBtn = new QPushButton();
    m_favoriteBtn->setObjectName("FavoriteBtn");
    m_favoriteBtn->setProperty("state", "no");
    m_favoriteBtn->setFixedSize(40, 40);

    ctrlLayout->addWidget(m_playModeBtn);
    ctrlLayout->addStretch();
    ctrlLayout->addWidget(m_prevBtn);
    ctrlLayout->addSpacing(15);
    ctrlLayout->addWidget(m_playBtn);
    ctrlLayout->addSpacing(15);
    ctrlLayout->addWidget(m_nextBtn);
    ctrlLayout->addStretch();
    ctrlLayout->addWidget(m_volumeBtn);
    ctrlLayout->addWidget(m_volumeSlider);
    ctrlLayout->addSpacing(10);
    ctrlLayout->addWidget(m_favoriteBtn);

    bottomLayout->addLayout(progressLayout);
    bottomLayout->addLayout(ctrlLayout);

    mainLayout->addWidget(topBar);
    mainLayout->addWidget(centerArea, 1);
    mainLayout->addWidget(bottomBar);
}

void MusicPage::initStyle() {
    // 读取原始的样式表路径
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

void MusicPage::togglePlay() {
    if (m_player->mediaStatus() == QMediaPlayer::NoMedia || m_fileList->count() == 0) {
        if (m_fileList->count() > 0 && m_fileList->currentRow() < 0) {
            m_fileList->setCurrentRow(0);
            onFileSelected(m_fileList->currentItem());
        }
        return;
    }

    if (m_player->state() == QMediaPlayer::PlayingState) {
        m_player->pause();
        m_isPlaying = false;
        m_cdWidget->stopRotation();
    } else {
        m_player->play();
        m_isPlaying = true;
        m_cdWidget->startRotation();
    }
    
    m_playBtn->setProperty("state", m_isPlaying ? "pause" : "play");
    updateBtnStyle(m_playBtn);
}

void MusicPage::playNext(bool isAutoPlay) {
    if(m_fileList->count() == 0) return;
    int currentRow = m_fileList->currentRow();
    if(currentRow < 0) currentRow = 0;
    
    int nextRow = currentRow;

    if (isAutoPlay && m_playMode == LoopSingle) {
        nextRow = currentRow;
    } else if (m_playMode == Random) {
        nextRow = QRandomGenerator::global()->bounded(m_fileList->count());
    } else {
        nextRow = (currentRow + 1) % m_fileList->count();
    }
    
    m_fileList->setCurrentRow(nextRow);
    onFileSelected(m_fileList->currentItem());
}

void MusicPage::playPrevious() {
    if(m_fileList->count() == 0) return;
    int currentRow = m_fileList->currentRow();
    int prevRow = (currentRow - 1 >= 0) ? currentRow - 1 : m_fileList->count() - 1;
    m_fileList->setCurrentRow(prevRow);
    onFileSelected(m_fileList->currentItem());
}

void MusicPage::onFileSelected(QListWidgetItem *item) {
    if(!item) return;
    QString path = m_currentDir + "/" + item->text();
    m_player->setMedia(QUrl::fromLocalFile(path));
    m_topTitleLabel->setText(item->text());
    
    m_player->play();
    m_isPlaying = true;
    m_playBtn->setProperty("state", "pause");
    updateBtnStyle(m_playBtn);
    
    m_cdWidget->startRotation();
}

void MusicPage::onMediaStatusChanged(QMediaPlayer::MediaStatus status) {
    if (status == QMediaPlayer::EndOfMedia) {
        playNext(true); 
    }
}

// === 新增扩展功能 ===
void MusicPage::togglePlayMode() {
    m_playMode = static_cast<PlayMode>((m_playMode + 1) % 3);
    
    if (m_playMode == LoopList) {
        m_playModeBtn->setProperty("mode", "list");
    } else if (m_playMode == LoopSingle) {
        m_playModeBtn->setProperty("mode", "single");
    } else {
        m_playModeBtn->setProperty("mode", "random");
    }
    updateBtnStyle(m_playModeBtn);
}

void MusicPage::toggleFavorite() {
    QString state = m_favoriteBtn->property("state").toString();
    if(state == "no") {
        m_favoriteBtn->setProperty("state", "yes");
    } else {
        m_favoriteBtn->setProperty("state", "no");
    }
    updateBtnStyle(m_favoriteBtn);
}

void MusicPage::changeVolume(int value) {
    m_player->setVolume(value);
    if(value == 0) {
        m_volumeBtn->setProperty("state", "off");
    } else {
        m_volumeBtn->setProperty("state", "on");
    }
    updateBtnStyle(m_volumeBtn);
}

// === 进度条与时间同步 ===
QString MusicPage::formatTime(qint64 ms) {
    int totalSeconds = ms / 1000;
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;
    return QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));
}

void MusicPage::onPositionChanged(qint64 position) {
    if(!m_progressSlider->isSliderDown()) {
        m_progressSlider->setValue(position);
    }
    m_currentTimeLabel->setText(formatTime(position));
}

void MusicPage::onDurationChanged(qint64 duration) {
    m_progressSlider->setRange(0, duration);
    m_totalTimeLabel->setText(formatTime(duration));
}

void MusicPage::onSliderMoved(int position) {
    m_player->setPosition(position);
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

void MusicPage::toggleDrawer() {
    m_drawer->setVisible(!m_drawer->isVisible());
}
void MusicPage::forceStop() {
    if (m_isPlaying) {
        m_player->stop();
        m_isPlaying = false;
        m_playBtn->setProperty("playing", false);
        updateBtnStyle(m_playBtn);
        m_cdWidget->stopRotation();
    }
}
