with open('src/MusicPage_copy.cpp', 'r') as f:
    text = f.read()

# Add setStyleSheet for folder icon to m_browseBtn
old_str = """    m_browseBtn = new QPushButton(topBar);
    m_browseBtn->setObjectName("BrowseBtn");
    m_browseBtn->setFixedSize(30, 30);
    m_browseBtn->setToolTip("选择音乐目录");"""

new_str = """    m_browseBtn = new QPushButton(topBar);
    m_browseBtn->setObjectName("BrowseBtn");
    m_browseBtn->setFixedSize(24, 24);
    m_browseBtn->setToolTip("选择音乐目录");
    m_browseBtn->setStyleSheet("QPushButton { border-image: url(:/res/images/music/folder_new.png); background: transparent; }");"""

text = text.replace(old_str, new_str)

with open('src/MusicPage_copy.cpp', 'w') as f:
    f.write(text)
