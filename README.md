# 3D Snake · Cocos2d-x 3.2 贪吃蛇

使用 **Cocos2d-x 3.2** 编写的 3D 贪吃蛇学习项目，最初用于尝试该版本的 3D 显示和动作能力。项目包含关卡选择、JSON 地图、传送门与多种道具，以及 SQLite 玩家记录。

原项目说明更新于 **2014-10-08**，当时仍处于开发阶段。本页整理了已有玩法、代码结构和工程依赖，并保留原说明中的未完成事项。

## 玩法

以关卡式冒险模式为目标，通过控制蛇移动、吃食物与道具，完成关卡设定的目标。

- **Windows**：方向键控制移动方向。
- **Android**：滑动屏幕控制方向。
- **Block**：障碍物，阻挡移动。
- **Food**：增加蛇身长度。
- **Transform Door**：成对的传送门，从一个入口进入、另一个出口出现。
- **Apple**：获得分数。
- **Star**：加速并提高得分倍率。
- **Ball**：暂时减速。

关卡文件位于 `Resources/map/`，描述胜利条件、障碍物和传送门等内容。当前代码仍保留原型逻辑，例如 `loadMap(int level)` 内固定读取 `map/1.json`；不应把已有的选关界面理解为完整的多关卡流程已经完成。

## 代码与设计

| 路径 | 内容 |
| --- | --- |
| `Classes/MainLayer.*` | 主界面 |
| `Classes/SelectLevelLayer.*`、`TouchTableView.*`、`ScrollMenu.*` | 关卡选择与列表交互 |
| `Classes/MapLayer.*` | 游戏场景、地图读取、输入与胜负判断 |
| `Classes/Snake.*` | 蛇的移动、身体与状态 |
| `Classes/Item.*` | 道具基类、具体效果与道具工厂 |
| `Classes/UserRecord.*`、`Classes/sqlite/` | SQLite 玩家数据与分数保存 |
| `Classes/GameDefine.*`、`VisibleRect.*` | 公共定义、模型配置与显示区域计算 |
| `Resources/map/`、`Resources/models/` | JSON 地图和 3D 模型资源 |

原项目记录的几个实现思路：

- 主界面、选关和游戏使用不同场景组织，便于切换。
- 尽量在 `onEnterTransitionDidFinish()` 后加载下一场景资源，减少切换期间同时占用的内存。这是当时的设计意图，不是本次性能测试结论。
- 道具统一继承基础类型，由工厂管理生成、消失和效果。
- 通过 JSON 描述地图，用 SQLite 保存和读取玩家记录。
- 选关列表使用 TableView，并为列表项加入入场动画。
- 当时采用移动与旋转组合动作实现 3D 蛇身转向；原说明记录的转弯衔接问题仍列于下方。

## 构建前准备

**仓库没有附带工程引用的 `cocos2d/` 引擎目录。** 直接克隆后打开工程，会缺少引擎项目、头文件及相关依赖。应先准备与 Cocos2d-x 3.2 相符的引擎和依赖，按照现有工程要求放置或调整路径，不能直接用最新版引擎替换并假定兼容。

### Windows

- 入口为 `proj.win32/Snake.sln`。
- 工程使用 Visual Studio 2013 的 `v120` 工具集，提供 `Debug | Win32` 和 `Release | Win32`。
- 工程通过 `../cocos2d/` 引用引擎、音频、扩展和第三方库。先补齐这些依赖，再检查资源搜索路径与运行目录。

### Android 与其他平台

- Android 入口位于 `proj.android/`，保留旧式 SDK / NDK、Eclipse / Ant 工程。历史说明见 [proj.android/README.md](proj.android/README.md)。
- `proj.android/build-cfg.json` 从 `Resources/` 复制资源，并引用 `../cocos2d/`。
- 当前 `Android.mk` 的源码清单尚未包含 Windows 工程中的全部游戏模块，例如选关和玩家记录相关文件；恢复 Android 构建时需要核对，不保证现有配置直接可用。
- 仓库还保留 iOS / macOS、Linux 和 Windows Phone 工程文件；这些入口未在本次重新验证。

## 尚未完成

原 README 明确记录的事项：

1. 关卡编辑器尚未编写。
2. 蛇转弯时身体看起来会出现断开的效果。

此外，现有工程与资源按旧版本工具链组织，完整关卡流程、各平台构建和现代系统兼容性仍需实际验证。本次只整理文档，未修改游戏代码，也未重新构建或运行测试。
