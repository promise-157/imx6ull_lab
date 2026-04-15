#ifndef MUSICPAGE_H
#define MUSICPAGE_H

#include <QWidget>
#include <QMediaPlayer>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

class MusicPage : public QWidget {
    Q_OBJECT
public:
    explicit MusicPage(QWidget *parent = nullptr);

private slots:
    void togglePlay();          // 播放/暂停控制
    void toggleDrawer();        // 侧边歌单抽屉开关
    void scanMediaFiles();      // 扫描本地目录
    void onFileSelected(QListWidgetItem *item); // 点击歌单切歌
    void changeMediaDir();      // 工具栏：修改搜索路径

private:
    void initStyle();           // 加载私有 QSS
    void loadSettings();        // 读取 config.ini
    void saveSettings(QString path); 
    void updateBtnStyle(QPushButton* btn); // 刷新 QSS 状态

    QMediaPlayer *m_player;
    bool m_isPlaying = false;
    QString m_currentDir;

    // UI 组件
    QPushButton *m_playBtn;
    QPushButton *m_toggleBtn;
    QWidget     *m_drawer;
    QListWidget *m_fileList;
    QLabel      *m_titleLabel;
};

#endif