import re

with open('src/VideoPage.cpp', 'r') as f:
    content = f.read()

# Change inheritance
content = content.replace('VideoPage::VideoPage(QWidget *parent) : QWidget(parent)', 'VideoPage::VideoPage(QWidget *parent) : IAppModule(parent)')

# Add topBar
new_topbar_code = """    // 新风格顶部栏
    QWidget *topBar = new QWidget(this);
    topBar->setObjectName("VideoTopBar");
    topBar->setFixedHeight(50);
    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(10, 0, 10, 0);

    // 关闭(左1)
    m_closeBtn = new QPushButton(topBar);
    m_closeBtn->setFixedSize(20, 20);
    m_closeBtn->setToolTip("彻底退出");
    m_closeBtn->setStyleSheet("QPushButton { border-image: url(res/images/video/close_new.png); background: transparent; }");

    // 最小化(左2)
    m_backBtn = new QPushButton(topBar);
    m_backBtn->setFixedSize(20, 20);
    m_backBtn->setToolTip("收起后台");
    m_backBtn->setStyleSheet("QPushButton { border-image: url(res/images/video/minimize_new.png); background: transparent; }");

    QLabel *m_topTitleLabel = new QLabel("Local Video Player", topBar);
    m_topTitleLabel->setObjectName("VideoTopTitle");
    m_topTitleLabel->setAlignment(Qt::AlignCenter);

    // 菜单(最右侧)
    QPushButton *m_menuBtn = new QPushButton(topBar);
    m_menuBtn->setFixedSize(24, 24);
    m_menuBtn->setToolTip("播放列表");
    m_menuBtn->setStyleSheet("QPushButton { border-image: url(res/images/video/menu_new.png); background: transparent; }");
    connect(m_menuBtn, &QPushButton::clicked, this, &VideoPage::toggleDrawer);

    topLayout->addWidget(m_closeBtn);
    topLayout->addSpacing(10);
    topLayout->addWidget(m_backBtn);
    topLayout->addStretch();
    topLayout->addWidget(m_topTitleLabel);
    topLayout->addStretch();
    topLayout->addWidget(m_menuBtn);

    mainLayout->addWidget(topBar);

    // 原来的 body
    QWidget *mainBody = new QWidget(this);
    QHBoxLayout *bodyLayout = new QHBoxLayout(mainBody);
    bodyLayout->setContentsMargins(0,0,0,0);
"""
# Replace layout root
content = content.replace("""    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);""", """    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);
""" + new_topbar_code)

# Fix where it was adding things to mainLayout
content = content.replace("mainLayout->addWidget(videoContainer, 1);", "bodyLayout->addWidget(videoContainer, 1);")
content = content.replace("mainLayout->addWidget(m_toggleBtn);", "bodyLayout->addWidget(m_toggleBtn);")
content = content.replace("mainLayout->addWidget(m_drawer);", "bodyLayout->addWidget(m_drawer);\n    mainLayout->addWidget(mainBody, 1);")

# We also need to connect the close/back buttons to requestMinimize and requestClose
content += """
// 顶部按钮事件要在构造函数里绑定，但这里用一个黑科技方法，放在构造函数结尾的修改
"""
content = re.sub(r'loadSettings\(\);\s*scanVideoFiles\(\);', r'loadSettings(); scanVideoFiles(); connect(m_backBtn, &QPushButton::clicked, this, &VideoPage::requestMinimize); connect(m_closeBtn, &QPushButton::clicked, this, [this](){ m_player->stop(); emit requestClose(); });', content)

# Change header include
content = content.replace('#include "VideoPage.h"', '#include "VideoPage_copy.h"')

with open('src/VideoPage_copy.cpp', 'w') as f:
    f.write(content)
