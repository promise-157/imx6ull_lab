#ifndef SIMPLE_STATE_MACHINE_TEMPLATE_H
#define SIMPLE_STATE_MACHINE_TEMPLATE_H

#include <QDebug>

/**
 * 【一阶：极简状态机模板】
 * 适用场景：LED 闪烁控制、按键消抖、极其简单的线性流水线任务。
 * 核心原理：只用 Enum 区分状态，在 update() 或者定时器内部通过 switch 进行循环控制。
 */

enum class SimpleDeviceState {
    OFF,            // 全关
    CONNECTING,     // 正在连接（慢闪）
    CONNECTED,      // 已连接（常亮）
    ERROR_BLINK     // 报错（快闪）
};

class SimpleDeviceController {
private:
    SimpleDeviceState m_currentState = SimpleDeviceState::OFF;
    int m_blinkCounter = 0;

public:
    SimpleDeviceController() = default;

    // 1. 外部调用这个函数改变状态
    void changeState(SimpleDeviceState newState) {
        if (m_currentState == newState) return; // 防抖，状态没变就不管
        
        m_currentState = newState;
        m_blinkCounter = 0; // 切换状态时，必须重置内部计数器
        qDebug() << "[Simple FSM] 状态切换为:" << static_cast<int>(newState);
    }

    // 2. 这个函数放到一个定时器里一直跑 (比如每 100ms 调用一次)
    void tick() {
        switch (m_currentState) {
            case SimpleDeviceState::OFF:
                // 确保硬件处于关闭状态
                break;
                
            case SimpleDeviceState::CONNECTED:
                // 确保硬件处于常亮状态
                break;
                
            case SimpleDeviceState::CONNECTING:
                // 慢闪逻辑：每 500ms(假设tick是100ms, 则执行5次) 翻转一次
                m_blinkCounter++;
                if (m_blinkCounter >= 5) {
                    qDebug() << "[Simple FSM] LED Toggle (Slow)";
                    m_blinkCounter = 0;
                }
                break;
                
            case SimpleDeviceState::ERROR_BLINK:
                // 快闪逻辑：每 100ms(1次) 翻转一次
                qDebug() << "[Simple FSM] LED Toggle (Fast)";
                break;
        }
    }
};

#endif // SIMPLE_STATE_MACHINE_TEMPLATE_H