with open('src/VideoPage.cpp', 'r') as f:
    text = f.read()

# Fix bodyLayout issue
text = text.replace("bodyLayout->addWidget(videoContainer, 1);", "mainLayout->addWidget(videoContainer, 1);")
text = text.replace("bodyLayout->addWidget(m_toggleBtn);", "mainLayout->addWidget(m_toggleBtn);")
text = text.replace("bodyLayout->addWidget(m_drawer);", "mainLayout->addWidget(m_drawer);")

# Also fix the duplicate connect calls at the bottom and the trailing comment outside the file
import re
text = re.sub(r'// 连接新加入的顶栏退出按钮与信号.*?emit requestClose[^}]*?\}\);', '', text, flags=re.DOTALL)
text = text.replace("loadSettings(); scanVideoFiles(); connect(m_backBtn, &QPushButton::clicked, this, &VideoPage::requestMinimize); connect(m_closeBtn, &QPushButton::clicked, this, [this](){ m_player->stop(); emit requestClose(); });\n}", "loadSettings();\n    scanVideoFiles();\n    connect(m_backBtn, &QPushButton::clicked, this, &VideoPage::requestMinimize);\n    connect(m_closeBtn, &QPushButton::clicked, this, [this](){\n        m_player->stop();\n        emit requestClose();\n    });\n}")
text = text.replace("\n// 顶部按钮事件要在构造函数里绑定，但这里用一个黑科技方法，放在构造函数结尾的修改\n", "")

with open('src/VideoPage_copy.cpp', 'w') as f:
    f.write(text)
