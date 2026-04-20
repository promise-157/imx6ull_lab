#ifndef ADVANCED_STATE_MACHINE_TEMPLATE_H
#define ADVANCED_STATE_MACHINE_TEMPLATE_H

#include <QDebug>

/**
 * 【二阶：高级生命周期状态机模板】
 * 适用场景：具有复杂网络重连、心跳超时、通信握手的核心业务（如 MQTT 客户端、TCP 通信）。
 * 核心原理：严格区分状态的 Enter（进入）、Update（持续）、Exit（退出）动作。
 */

enum class AdvancedSysState {
    INIT,           // 初始化
    DISCONNECTED,   // 未连接网络
    CONNECTING,     // 正在拨号/连接
    WORKING         // 正常工作
};

class AdvancedNetworkManager {
private:
    AdvancedSysState m_state = AdvancedSysState::INIT;
    int m_timeoutMs = 0; // 内部超时计数器

public:
    AdvancedNetworkManager() {
        // 初始状态流转
        transitionTo(AdvancedSysState::DISCONNECTED);
    }

    // 1. 统一的状态切换枢纽（万物总管）
    void transitionTo(AdvancedSysState nextState) {
        if (m_state == nextState) return;

        // --- A. 处理旧状态的退出动作 (OnExit) ---
        if (m_state == AdvancedSysState::CONNECTING) {
            qDebug() << "[Advanced FSM] 退出连接中状态，正在清理半开连接...";
        } else if (m_state == AdvancedSysState::WORKING) {
            qDebug() << "[Advanced FSM] 退出工作状态，准备断开...";
        }

        // --- B. 真正改变状态 ---
        m_state = nextState;

        // --- C. 处理新状态的进入动作 (OnEnter) ---
        switch (m_state) {
            case AdvancedSysState::DISCONNECTED:
                qDebug() << "[Advanced FSM] 进入断开状态：关闭所有底层硬件IO";
                // 发起过一会儿重连的定时器动作等
                break;
                
            case AdvancedSysState::CONNECTING:
                qDebug() << "[Advanced FSM] 进入连接状态：开始发起 TCP/MQTT 握手...";
                m_timeoutMs = 5000; // 强制设置 5 秒超时底线
                break;
                
            case AdvancedSysState::WORKING:
                qDebug() << "[Advanced FSM] 进入工作状态：连接成功！开始启动上报任务";
                break;
                
            default: break;
        }
    }

    // 2. 放到主循环或定时器里持续执行 (OnUpdate)
    void tick(int deltaTimeMs) {
        switch (m_state) {
            case AdvancedSysState::CONNECTING:
                m_timeoutMs -= deltaTimeMs;
                if (m_timeoutMs <= 0) {
                    qDebug() << "[Advanced FSM] 连接超时！触发自我保护，退回断开状态";
                    transitionTo(AdvancedSysState::DISCONNECTED); // 状态机自驱动降级！
                }
                break;
                
            case AdvancedSysState::WORKING:
                // m_timeoutMs -= deltaTimeMs; // 如果用来做心跳超时机制
                // do_work();
                break;
                
            case AdvancedSysState::DISCONNECTED:
                // 可以加一个逻辑：断开 10 秒后自动转入 CONNECTING 重试
                break;
                
            default:
                break;
        }
    }
};

#endif // ADVANCED_STATE_MACHINE_TEMPLATE_H