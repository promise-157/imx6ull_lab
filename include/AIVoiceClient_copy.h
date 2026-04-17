#ifndef AIVOICECLIENT_COPY_H
#define AIVOICECLIENT_COPY_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonObject>
#include <QJsonDocument>
#include <QString>

/**
 * AI语音/意图控制客户端 (网络+核心路由)
 * 这是一个独立的后台通信与系统服务接管模块，能够：
 * 1. 负责语音上传/文本传达到大文件或轻量API端。
 * 2. 接收结构化 JSON 指令后，通过 Qt 信号槽抛出 action 控制命令。
 */
class AIVoiceClient_copy : public QObject {
    Q_OBJECT
public:
    explicit AIVoiceClient_copy(QObject *parent = nullptr);
    ~AIVoiceClient_copy();

    // 核心业务：发送文字或音频意图去云大模型 (这里先打通文本->JSON API)
    void sendTextIntent(const QString &textCommand);
    
    // MQTT 预留接口: 后续可以直接在这里桥接发送给 Broker
    void publishMqttCommand(const QString &topic, const QString &payload);

signals:
    // 解析出 AI 指明要操作的主应用 (如 "music", "video", "hardware")
    void openAppRequested(QString appKey);
    // 控制某个软硬件开关 (如 "led:on", "volume:down")
    void executeHardwareAction(QString action, QString value);

private slots:
    void onNetworkReplyFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *m_networkManager;
};

#endif // AIVOICECLIENT_COPY_H
