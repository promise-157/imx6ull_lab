# Music Player Refactor (Horizontal Architecture)

- Extracted **AudioHal** (`components/hal/src/AudioHal.cpp`):
  - Wraps `QMediaPlayer` inside the HAL layer.
  - Supports `doPlay()`, `doPause()`, `doStop()`, `doSeek()`, `doSetVolume()`.
  - Emits telemetry `hal/pub/audio/position`, `hal/pub/audio/duration`, `hal/pub/audio/status`.

- Extracted **MediaService** (`components/service/src/MediaService.cpp`):
  - Runs file I/O operations (`QDir`, `entryList`) on a pure Qt Worker Thread dynamically via `MediaServiceWorker`.
  - Emits `svc/pub/music/list` representing the playlist text.

- Decoupled **MusicPage** (`components/gui_pages/src/MusicPage.cpp`):
  - Completely ripped off file scanning `scanMediaFiles()`.
  - Removed `QMediaPlayer* m_player` context.
  - Now pure UI manipulation interacting EXCLUSIVELY with `EventBus`.
