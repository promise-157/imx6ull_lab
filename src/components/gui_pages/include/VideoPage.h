#ifndef VIDEOPAGE_H
#define VIDEOPAGE_H

#include "IAppModule.h"
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QKeyEvent>
#include <QMouseEvent>

class VideoPage : public IAppModule {
    Q_OBJECT
public:
    explicit VideoPage(QWidget *parent = nullptr);
    ~VideoPage() override = default;

    QString appName() const override { return "视频播放器"; }
    QString appIconPath() const override { return ":/res/images/app_icons/video.png"; }
    void stopService() override { 
        if (m_player) m_player->stop(); 
    }

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void togglePlay();
    void toggleDrawer();
    void scanVideoFiles();
    void onFileSelected(QListWidgetItem *item);
    void changeVideoDir();
    
    void playNext();
    void playPrevious();
    void changeVolume(int value);
    void onPositionChanged(qint64 position);
    void onDurationChanged(qint64 duration);
    void onSliderMoved(int position);

private:
    void loadSettings();
    void saveSettings(QString path);
    QString formatTime(qint64 ms);

    QPushButton *m_backBtn;
    QPushButton *m_closeBtn;
    QPushButton *m_browseBtn;
    QPushButton *m_menuBtn;
    QLabel *m_topTitleLabel;

    QMediaPlayer *m_player;
    QVideoWidget *m_videoWidget;
    bool m_isPlaying = false;
    QString m_currentDir;

    QPushButton *m_playBtn;
    QPushButton *m_prevBtn;
    QPushButton *m_nextBtn;
    
    QSlider *m_progressSlider;
    QLabel *m_timeLabel;
    
    QSlider *m_volumeSlider;
    
    QWidget     *m_drawer;
    QListWidget *m_fileList;
};
#endif
