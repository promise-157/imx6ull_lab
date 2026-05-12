#include "AppBootstrap.h"
#include "SystemManager.h"

#include "AudioHal.h"
#include "HardwareHal.h"
#include "HardwareService.h"
#include "BusHal.h"
#include "BusService.h"
#include "MediaService.h"
#include "VideoHal.h"
#include "VideoService.h"

namespace AppBootstrap {

void initAudioSubsystem() {
  SystemManager &sysMgr = SystemManager::instance();
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
  sysMgr.registerModule(new BusHal(), true);
  sysMgr.registerModule(new BusService(), true);
}

void initAllSubsystems() {
  initAudioSubsystem();
  initVideoSubsystem();
  initHardwareSubsystem();
}

} // namespace AppBootstrap
