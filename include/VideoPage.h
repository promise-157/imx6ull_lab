#ifndef VIDEOPAGE_H
#define VIDEOPAGE_H

#include <QWidget>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

class VideoPage : public QWidget {
    Q_OBJECT
public:
    explicit VideoPage(QWidget *parent = nullptr);

private slots:
    void togglePlay();
    void toggleDrawer();
    void scanVideoFiles();
    void onFileSelected(QListWidgetItem *item);
    void changeVideoDir();

private:
    void initStyle();
    void loadSettings();
    void saveSettings(QString path);
    void updateBtnStyle(QPushButton* btn);

    QMediaPlayer *m_player;
    QVideoWidget *m_videoWidget;
    bool m_isPlaying = false;
    QString m_currentDir;

    QPushButton *m_playBtn;
    QPushButton *m_toggleBtn;
    QWidget     *m_drawer;
    QListWidget *m_fileList;
};

#endif