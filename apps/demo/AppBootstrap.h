#pragma once

namespace AppBootstrap {
/**
 * @brief 初始化音频子系统（挂载 AudioHal 和 MediaService）
 */
void initAudioSubsystem();

/**
 * @brief 一键聚合加载当前 Demo 应用所需的所有子系统
 * @note 这里的组装过程对 main.cpp 彻底黑盒，由该装配车间全权负责耦合具体类
 */
void initAllSubsystems();
} // namespace AppBootstrap