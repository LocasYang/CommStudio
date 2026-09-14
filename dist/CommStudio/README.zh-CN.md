# CommStudio · 一体化通信调试平台

[English](README.md) | [简体中文](README.zh-CN.md)

CommStudio 是面向嵌入式、工业控制、物联网和设备研发工程师的 Qt 6 桌面通信
调试上位机。它将串口、Modbus、蓝牙和网络调试汇集在同一个原生 C++ / Qt Quick
应用中，并共享统一的收发终端、编码校验、自动化与运行日志。

## 功能概览

- **串口**：端口枚举、完整通信参数、HEX/文本与转义序列、行尾、自动重连、
  定时发送、原始文件分块、DTR/RTS/Break、引脚状态和字节统计。
- **Modbus**：RTU/TCP 主站、常用功能码、超时重试、轮询任务、数值解码/编码、
  寄存器表、响应解析以及 RTU/TCP 从站模拟。
- **蓝牙**：适配器状态、BLE/Classic 扫描、GATT 读写订阅、Nordic UART 预置与
  Classic RFCOMM（SPP）。
- **网络**：TCP 客户端/服务端和 UDP、客户端会话、单播/广播/组播、应用分帧和
  自动回复规则。
- **通用能力**：统一终端、命令历史、HEX/ASCII 转换、校验计算、运行日志、浅深
  色主题，以及中英文界面切换。

## 语言

软件默认以 **English** 启动。打开 **Settings → Language**，选择 **English** 或
**中文**；切换立即生效，并会在下次启动时保留。

本 README 默认英文。可通过顶部链接在中文与英文版之间往返切换。

## 构建与运行

环境要求：Qt 6.6.3 `msvc2019_64`、Visual Studio Build Tools、CMake 和 Ninja。

```powershell
# 编译、部署 Qt 运行时并同步 dist。
powershell -NoProfile -ExecutionPolicy Bypass -File scripts\build.ps1 -RunTests

# 在 dist\CommStudio 中创建可独立运行的发布包。
powershell -NoProfile -ExecutionPolicy Bypass -File scripts\package.ps1
```

开发环境运行：

```powershell
$env:PATH = "E:\QT\6.6.3\msvc2019_64\bin;$env:PATH"
.\build-msvc\CommStudio.exe
```

## 工程结构

```text
src/        C++ 入口、核心协议工具与各通信管理器
qml/        主窗口、主题、语言辅助、通用组件和页面
resources/  Windows 应用图标资源
tests/      核心单元测试
```

## 运行时说明

- 主题与语言偏好使用 QSettings 保存到 `HKCU\Software\TBEA\CommStudio`。
- 若蓝牙日志显示 “Dummy backend running”，表示当前 Qt Bluetooth 包没有可用的
  Windows 后端，扫描与连接功能不可用。
