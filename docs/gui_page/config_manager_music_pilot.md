# ConfigManager 音乐模块试点说明

## 这玩意是啥

`ConfigManager` 是一个全局 JSON 配置读取器，负责提供“默认配置”。

- 代码位置：
  - `components/core_utils/include/ConfigManager.h`
  - `components/core_utils/src/ConfigManager.cpp`
- 配置文件：
  - `apps/demo/config.json`
  - 构建时会复制到可执行文件目录

它的作用不是替代 `QSettings`，而是作为“系统级默认值来源”。

## 这次在音乐功能怎么接的

本次试点改动：

1. 程序启动时加载 JSON：
   - `apps/demo/main.cpp`
   - `ConfigManager::instance().loadConfig(applicationDirPath + "/config.json")`
2. 音乐页读取策略升级（`components/gui_pages/src/MusicPage.cpp`）：
   - `config.json` 提供默认值
   - `config.ini` 提供用户运行时覆盖值
   - 覆盖优先级：`config.ini > config.json > 代码兜底`

### 当前音乐配置键

在 `apps/demo/config.json` 的 `Music` 组：

- `DefaultDir`：默认音乐目录（支持相对路径，基于 `applicationDirPath`）
- `DefaultVolume`：默认音量（0-100）
- `DefaultPlayMode`：默认播放模式
  - `0` = 列表循环
  - `1` = 单曲循环
  - `2` = 随机

## 运行时文件是怎么配合的

- `config.json`：项目级默认配置（建议纳入版本管理）
- `config.ini`：设备/用户本地配置（运行时自动写入）

音乐模块会把这些值写回 `config.ini`：

- `Paths/music_dir`
- `Music/volume`
- `Music/play_mode`

## 你以后怎么用

### 1. 加新默认项

先在 `apps/demo/config.json` 增加键值，再在业务代码中通过 `ConfigManager` 读取并提供兜底值。

示例：

```cpp
int retry = ConfigManager::instance().getInt("Network", "RetryCount", 3);
```

### 2. 想要“用户可改且记住”

保持这个模式：

1. 启动读取：`QSettings(key, default_from_ConfigManager)`
2. 用户操作后写回：`QSettings::setValue(...)`

## 适合放进 ConfigManager 的内容

- 默认目录、默认开关、默认阈值、默认端口
- 板卡差异配置（路径、设备节点、超时参数）

## 不建议放进 ConfigManager 的内容

- 频繁变化的运行态值（播放进度、临时状态）
- 用户每秒都会变动的细粒度状态

这些更适合 `QSettings` 或运行时内存状态。
