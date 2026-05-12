#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QString>
#include <QJsonObject>

// 单例模式：全局唯一的配置管理器
class ConfigManager {
public:
    // 获取全局唯一实例
    static ConfigManager& instance();

    // 加载并解析 JSON 配置文件
    bool loadConfig(const QString& filePath);

    // --- 万能参数获取接口（带有“防御性兜底”默认值） ---
    
    // 获取字符串 (分组名, 键名, 如果没找到的默认值)
    QString getString(const QString& group, const QString& key, const QString& defaultValue = "") const;
    
    // 获取整数
    int getInt(const QString& group, const QString& key, int defaultValue = 0) const;
    
    // 获取布尔值 (true/false)
    bool getBool(const QString& group, const QString& key, bool defaultValue = false) const;

private:
    // 私有化构造函数，防止外部 new / 拷贝
    ConfigManager() = default;
    ~ConfigManager() = default;
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    // 存储解析后的 JSON 数据
    QJsonObject m_configObj;
};

#endif // CONFIGMANAGER_H