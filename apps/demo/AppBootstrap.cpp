#include "AppBootstrap.h"
#include "SystemManager.h"

// 所有的硬件和服务具体头文件只允许在这里出现！
// 系统核心和 UI 完全不知道这些类的存在
#include "AudioHal.h"
#include "HardwareHal.h"
#include "HardwareService.h"
#include "MediaService.h"
#include "VideoHal.h"
#include "VideoService.h"

namespace AppBootstrap {

void initAudioSubsystem() {
  SystemManager &sysMgr = SystemManager::instance();

  // 装配音频底层驱动与媒体服务，交给 SystemManager 托管独立线程生命周期
  sysMgr.registerModule(new AudioHal(), true);
  sysMgr.registerModule(new MediaService(), true);
}

void initVideoSubsystem() {
  SystemManager &sysMgr = SystemManager::instance();
  sysMgr.registerModule(new VideoHal(), true);
  sysMgr.registerModule(new VideoService(), true);
}

void initHardwareSubsystem() {
  SystemManager &sysMgr = SystemManager::instance();
  sysMgr.registerModule(new HardwareHal(), true);
  sysMgr.registerModule(new HardwareService(), true);
}

void initAllSubsystems() {
  // --- 1. 组装音频相关功能 ---
  initAudioSubsystem();

  // --- 2. 组装其他功能（如视频、蓝牙、网络等，未来可扩展）---
  initVideoSubsystem();
  initHardwareSubsystem();
}

} // namespace AppBootstrap
