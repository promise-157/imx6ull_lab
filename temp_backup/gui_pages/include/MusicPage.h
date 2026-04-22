#ifndef MUSICPAGE_H
#define MUSICPAGE_H

#include "IAppModule.h"
#include <QMediaPlayer>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSlider>
#include <QTimer>
#include <QPaintEvent>

class CdWidget : public QWidget {
    Q_OBJECT
public:
    explicit CdWidget(QWidget *parent = nullptr);
    void startRotation();
    void stopRotation();
protected:
    void paintEvent(QPaintEvent *event) override;
private slots:
    void updateRotation();
private:
    QTimer *m_timer;
    qreal m_angle;
    QPixmap m_cdPixmap;
    QPixmap m_maskPixmap;
};

class MusicPage : public IAppModule {
    Q_OBJECT
public:
    explicit MusicPage(QWidget *parent = nullptr);
    ~MusicPage() override;

    // ----- IAppModule 必须实现的接口 -----
    QString appName() const override { return "音乐播放器"; }
    QString appIconPath() const override { return ":/res/images/music/play.png"; }
    void stopService() override { forceStop(); }
    // ------------------------------------

    void forceStop();
    bool isPlaying() const { return m_isPlaying; }

    enum PlayMode {
        LoopList,
        LoopSingle,
        Random
    };

private slots:
    void togglePlay();
    void toggleDrawer();
    void scanMediaFiles();
    void onFileSelected(QListWidgetItem *item);
    void changeMediaDir();
    void playNext(bool isAutoPlay = false);
    void playPrevious();
    void togglePlayMode();
    void toggleFavorite();
    void changeVolume(int value);
    void onPositionChanged(qint64 position);
    void onDurationChanged(qint64 duration);
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void onSliderMoved(int position);

private:
    void initUI();
    void initStyle();
    void loadSettings();
    void saveSettings(QString path);
    void updateBtnStyle(QPushButton* btn);
    QString formatTime(qint64 ms);

    QMediaPlayer *m_player;
    bool m_isPlaying = false;
    PlayMode m_playMode = LoopList;
    QString m_currentDir;

    QPushButton *m_backBtn;
    QPushButton *m_closeBtn;
    QLabel      *m_topTitleLabel;
    QPushButton *m_browseBtn;
    CdWidget    *m_cdWidget;
    QWidget     *m_drawer;
    QListWidget *m_fileList;

    QSlider     *m_progressSlider;
    QLabel      *m_currentTimeLabel;
    QLabel      *m_totalTimeLabel;
    
    QPushButton *m_playModeBtn;
    QPushButton *m_prevBtn;
    QPushButton *m_playBtn;
    QPushButton *m_nextBtn;
    QPushButton *m_volumeBtn;
    QSlider     *m_volumeSlider;
    QPushButton *m_favoriteBtn;
};
#endif
