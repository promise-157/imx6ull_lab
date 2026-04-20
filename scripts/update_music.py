import re

with open('src/MusicPage.cpp', 'r') as f:
    content = f.read()

# 1. Update Top Bar UI layout and style
# We want to find topBar layout logic.
new_topbar_code = """    // 新风格顶部栏
    QWidget *topBar = new QWidget(this);
    topBar->setObjectName("MusicTopBar");
    topBar->setFixedHeight(50);
    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(10, 0, 10, 0);

    // 关闭(左1)
    m_closeBtn = new QPushButton(topBar);
    m_closeBtn->setFixedSize(20, 20);
    m_closeBtn->setToolTip("彻底退出");
    m_closeBtn->setStyleSheet("QPushButton { border-image: url(res/images/music/close_new.png); background: transparent; }");

    // 最小化(左2)
    m_backBtn = new QPushButton(topBar);
    m_backBtn->setFixedSize(20, 20);
    m_backBtn->setToolTip("收起后台");
    m_backBtn->setStyleSheet("QPushButton { border-image: url(res/images/music/minimize_new.png); background: transparent; }");

    m_topTitleLabel = new QLabel("Local Music Player", topBar);
    m_topTitleLabel->setObjectName("TopTitle");
    m_topTitleLabel->setAlignment(Qt::AlignCenter);

    // 原来的BrowseBtn (可保留在原来位置或者调整)
    m_browseBtn = new QPushButton(topBar);
    m_browseBtn->setObjectName("BrowseBtn");
    m_browseBtn->setFixedSize(30, 30);
    m_browseBtn->setToolTip("选择音乐目录");
    
    // 菜单(最右侧)
    QPushButton *m_menuBtn = new QPushButton(topBar);
    m_menuBtn->setFixedSize(24, 24);
    m_menuBtn->setToolTip("播放列表");
    m_menuBtn->setStyleSheet("QPushButton { border-image: url(res/images/music/menu_new.png); background: transparent; }");
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
"""

# Replace the block that initializes topBar
# The old one had:
# QWidget *topBar = new QWidget(this); ... topLayout->addWidget(m_browseBtn);
pattern = re.compile(r'QWidget \*topBar = new QWidget\(this\);.*?topLayout->addWidget\(m_browseBtn\);', re.DOTALL)
content = pattern.sub(new_topbar_code, content)

with open('src/MusicPage_copy.cpp', 'w') as f:
    f.write(content)
