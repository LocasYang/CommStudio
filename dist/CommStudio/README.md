# CommStudio · 一体化通信调试平台

面向嵌入式、工业控制、物联网和设备研发工程师的桌面通信调试上位机，将
**串口调试助手、Modbus 调试助手、蓝牙调试助手、网络调试助手** 集成到同一个
Qt 6 / Qt Quick 应用中，并共享统一的收发终端、编码校验、自动化与日志体系。

原生 C++（Qt 6.6.3 MSVC），无 Python 依赖。界面设计令牌与 KeilPlugin Studio
同源（浅色 / 深色主题、六色主色令牌、DWM 原生标题栏融合）。

## 功能总览

- **串口调试**：端口枚举（设备描述 / VID:PID）、全参数配置、自定义波特率、
  HEX/文本收发与转义序列、行尾追加、断线自动重连（0.5s→5s 退避）、定时发送、
  文件分块发送（进度 / 速率 / 取消）、DTR/RTS/Break 与 CTS/DSR/DCD/RI 引脚状态、
  字节统计（速率 / 峰值）。
- **Modbus**：RTU / TCP 主站（FC 01/02/03/04/05/06/0F/10）、超时与重试、
  多任务轮询调度（严格串行）、数据解码（int16/uint16/int32/uint32/float32 ×
  ABCD/BADC/CDAB/DCBA、比例 / 偏移 / 单位）、寄存器表（协议地址 / 显示地址双列）、
  写前预览、异常码翻译、从站模拟器（RTU / TCP，四种数据区 × static/random/
  counter/ramp/sine 数据源）、Raw Test Center（原始 ADU 收发、自动 CRC、
  响应解析、CRC 校验提示）、轮询任务 CSV 导入。
- **蓝牙**：本地适配器状态、BLE / Classic 扫描（RSSI 实时更新、名称过滤）、
  BLE GATT Client（服务发现、特征读写、Notify / Indicate 订阅、MTU 显示）、
  BLE UART 模式（Nordic UART 预置，按 MTU 自动分片）、Classic RFCOMM(SPP)
  服务发现与终端。
- **网络**：TCP Client（自动重连、本地绑定）、TCP Server（多客户端会话表、
  定向 / 广播发送、踢除客户端）、UDP（单播 / 广播 / 组播、双端地址显示）、
  TCP 应用层分帧（原始流 / 按行 / 定长 / 空闲间隔）、自动回复规则（包含 /
  正则 / HEX 前缀匹配）。
- **通用能力**：统一收发终端（文本 / HEX / HEX+ASCII、时间戳、RX/TX 分色、
  30 ms 批量刷新、环形缓冲 10000 行 / 32 MB、暂停 / 清屏 / 导出）、命令历史（↑/↓）、
  工具箱（Sum8/16、XOR、CRC8、CRC16-Modbus、CRC16-CCITT、CRC32 校验计算器，
  HEX 多类型多字节序解析，数值编码，文本 ↔ HEX）、运行日志（分级 / 导出）、
  浅色 / 深色主题与主色令牌设置（QSettings 持久化）。

## 构建与运行

环境：Qt 6.6.3 msvc2019_64 + Visual Studio Build Tools + CMake + Ninja（工具链自动探测，与 KeilPlugin Studio
相同布局；也可用 `QT_ROOT` 环境变量或 `-QtRoot` 指定）。

```powershell
# 日常构建：编译 + 部署 Qt 运行时 + 同步 dist（-RunTests 跑单元测试）
powershell -NoProfile -ExecutionPolicy Bypass -File scripts\build.ps1 -RunTests

# 发布打包：dist\CommStudio 自包含目录，拷到任何机器即可运行
powershell -NoProfile -ExecutionPolicy Bypass -File scripts\package.ps1
```

运行（开发环境）：

```powershell
$env:PATH = "I:\QT\6.6.3\msvc2019_64\bin;$env:PATH"
.\build-msvc\CommStudio.exe
```

调试截图辅助参数：`--page=0..6`、`--dark`；环境变量 `COMMSTUDIO_SCREENSHOT=<png>`
可在启动后自动抓图退出（`COMMSTUDIO_SCREENSHOT_SIZE=1360x860`、
`COMMSTUDIO_SCREENSHOT_DELAY` 控制延时）。

## 目录结构

```text
src/
  main.cpp             GUI 入口（QML 引擎 + 后端单例注册 + 截图辅助）
  windowchrome.*       DWM 原生标题栏融合
  core/
    trafficmodel.*     统一收发流量模型（30 ms 批量刷新、环形缓冲、导出）
    checksumengine.*   Sum8/16、XOR、CRC8、CRC16-Modbus、CRC16-CCITT、CRC32
    hexcodec.*         HEX 宽松解析、转义序列、行尾追加
    modbuscodec.*      RTU/TCP ADU 组帧与解析、异常码、32 位字序解码
  managers/
    studio.*           运行日志、全局收发统计、主题持久化
    serialmanager.*    串口传输（枚举 / 重连 / 定时 / 文件 / 引脚）
    modbusmanager.*    双引擎 Modbus（QModbusClient/Server + 自研 Codec）
    bluetoothmanager.* BLE 扫描 / GATT / UART 模式 / RFCOMM
    networkmanager.*   TCP Client/Server、UDP、分帧、自动回复
    toolboxmanager.*   校验计算器与编码转换
qml/
  Main.qml             主窗口（头部 / 侧边导航 / 状态栏 / 主题面板 / Toast）
  Theme.qml            设计令牌（与 KeilPlugin Studio 同源）
  components/          AppButton / AppCard / AppComboBox / NavItem / StatusPill /
                       AppTextField / AppCheckBox / FormRow / TerminalView / SendPanel
  dialogs/ConfirmDialog.qml
  pages/               串口 / Modbus / 蓝牙 / 网络 / 工具箱 / 运行日志 / 设置
tests/                 CommStudioCoreTest 单元测试
```

## 设计原则

- 连接方式只是入口：四种助手共享同一终端、发送、编码、校验与日志底座。
- 接收数据先入模型、30 ms 批量刷 UI，不在 readyRead 里直接刷新复杂界面。
- Modbus 内部统一 0 基协议地址，UI 双列显示协议地址与 4xxxx 显示地址。
- TCP 不把一次 readyRead 当作一帧，按用户选择的分帧策略拆包；UDP 保留
  数据报边界；BLE 按 GATT 特征事件展示。

## 运行时能力说明

- **蓝牙后端**：当前发布构建使用 Qt MSVC 套件；如果启动日志仍提示
  "Dummy backend running"，说明当前 Qt Bluetooth 包没有加载 Windows 后端，
  此时界面可操作但扫描 / 连接不可用，需要替换为带 Windows 后端的 Qt 构建。
- **主题持久化**：浅色 / 深色与主色令牌写入 QSettings（注册表
  `HKCU\Software\TBEA\CommStudio`），重启后自动恢复。
