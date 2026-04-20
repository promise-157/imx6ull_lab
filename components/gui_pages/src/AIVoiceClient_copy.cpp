#include "AIVoiceClient_copy.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

AIVoiceClient_copy::AIVoiceClient_copy(QObject *parent) 
    : QObject(parent), m_networkManager(new QNetworkAccessManager(this)) {
        
    connect(m_networkManager, &QNetworkAccessManager::finished, 
            this, &AIVoiceClient_copy::onNetworkReplyFinished);
}

AIVoiceClient_copy::~AIVoiceClient_copy() {
}

void AIVoiceClient_copy::sendTextIntent(const QString &textCommand) {
    // 假设这是调用了某种云端轻量化/大模型 API
    // 例如百度的 UNIT 接口，或你个人部署的 LLM 服务
    // 此处写网络框架的架子，未来可以用实际的 URL
    QNetworkRequest request(QUrl("http://your-ai-api-server/v1/intent"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["query"] = textCommand;
    json["device"] = "imx6ull";
    
    QJsonDocument doc(json);
    m_networkManager->post(request, doc.toJson());
    
    qDebug() << "[AI_Voice] -> Sending literal intent query:" << textCommand;
}

void AIVoiceClient_copy::onNetworkReplyFinished(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        
        if(!jsonDoc.isNull() && jsonDoc.isObject()) {
            QJsonObject jsonOpt = jsonDoc.object();
            
            // 假设 AI 平台返回例如: { "action": "open_app", "target": "music", "hw_cmd": "volume", "hw_val": "up" }
            QString action = jsonOpt.value("action").toString();
            
            if (action == "open_app") {
                QString targetApp = jsonOpt.value("target").toString();
                emit openAppRequested(targetApp);
            }
            else if (action == "hardware_control") {
                QString hwCmd = jsonOpt.value("hw_cmd").toString();
                QString hwVal = jsonOpt.value("hw_val").toString();
                emit executeHardwareAction(hwCmd, hwVal);
            }
            
            qDebug() << "[AI_Voice] <- Success logic processed. Action:" << action;
        } else {
            qDebug() << "[AI_Voice] ! Payload is not JSON. Response:" << responseData;
        }
    } else {
        qDebug() << "[AI_Voice] API Post Failed:" << reply->errorString();
        // 网络不通的话，此时可以回退到本地的超级轻量的固定词汇匹配（如PocketSphinx）
        // 也可以报错给用户
    }
    reply->deleteLater();
}

void AIVoiceClient_copy::publishMqttCommand(const QString &topic, const QString &payload) {
    // 这里预留了给 MQTT 的出口
    // 待后续您想要连入 Mosquitto 时，在这调用 QtMQTT 的 QMQTT::Client->publish() 扩展
    qDebug() << "[MQTT_Out] Topic:" << topic << ", Payload:" << payload;
}
