#include "ConfigManager.h"
#include <QFile>
#include <QJsonDocument>
#include <QDebug>

ConfigManager& ConfigManager::instance() {
    static ConfigManager _instance; // C++11 线程安全的局部静态单例
    return _instance;
}

bool ConfigManager::loadConfig(const QString& filePath) {
    QFile file(filePath);
    // 1. 底线编程：检查文件是否能成功打开
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[Config] Failed to open config file:" << filePath << ". Will use default values.";
        return false;
    }
    
    QString val = file.readAll();
    file.close();
    
    // 2. 底线编程：检查 JSON 格式是否损坏或写错
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8(), &error);
    if (error.error != QJsonParseError::NoError || !doc.isObject()) {
        qWarning() << "[Config] JSON Parse Error:" << error.errorString() << ". Will use default values.";
        return false;
    }
    
    m_configObj = doc.object();
    qInfo() << "[Config] Successfully loaded config file:" << filePath;
    return true;
}

QString ConfigManager::getString(const QString& group, const QString& key, const QString& defaultValue) const {
    if (m_configObj.contains(group) && m_configObj[group].isObject()) {
        QJsonObject groupObj = m_configObj[group].toObject();
        if (groupObj.contains(key)) {
            return groupObj[key].toString();
        }
    }
    return defaultValue; // 找不到时返回用户传入的兜底值
}

int ConfigManager::getInt(const QString& group, const QString& key, int defaultValue) const {
    if (m_configObj.contains(group) && m_configObj[group].isObject()) {
        QJsonObject groupObj = m_configObj[group].toObject();
        if (groupObj.contains(key)) {
            return groupObj[key].toInt(defaultValue);
        }
    }
    return defaultValue;
}

bool ConfigManager::getBool(const QString& group, const QString& key, bool defaultValue) const {
    if (m_configObj.contains(group) && m_configObj[group].isObject()) {
        QJsonObject groupObj = m_configObj[group].toObject();
        if (groupObj.contains(key)) {
            return groupObj[key].toBool(defaultValue);
        }
    }
    return defaultValue;
}