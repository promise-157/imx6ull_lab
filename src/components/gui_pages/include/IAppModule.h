#ifndef IAPPMODULE_H
#define IAPPMODULE_H

#include <QWidget>
#include <QString>

// 应用模块基类，所有子应用（音乐、视频）都必须继承该接口以实现解耦
class IAppModule : public QWidget {
    Q_OBJECT
public:
    explicit IAppModule(QWidget *parent = nullptr) : QWidget(parent) {}
    virtual ~IAppModule() = default;

    // 获取应用名称 (例如 "音乐播放器")
    virtual QString appName() const = 0;
    // 获取应用图标资源路径
    virtual QString appIconPath() const = 0;
    // 释放应用资源 (在彻底关闭时调用)
    virtual void stopService() {}

signals:
    // 应用发出的标准控制信号
    void requestMinimize(); // 收起后台信号
    void requestClose();    // 彻底关闭自己信号
};

#endif
