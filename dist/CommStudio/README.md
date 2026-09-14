# CommStudio · Integrated Communication Workbench

[English](README.md) | [简体中文](README.zh-CN.md)

CommStudio is a Qt 6 desktop workbench for embedded, industrial-control, IoT,
and device engineers. It brings Serial, Modbus, Bluetooth, and Network
debugging into one native C++ / Qt Quick application with a shared traffic
terminal, codecs, automation, and runtime log.

## Highlights

- **Serial** — port discovery, full communication parameters, HEX/text and
  escape sequences, line endings, auto reconnect, timed sending, raw file
  chunks, DTR/RTS/Break, pin state, and byte statistics.
- **Modbus** — RTU/TCP master, common function codes, retries, polling tasks,
  value decoding/encoding, register table, response parsing, and RTU/TCP slave
  simulation.
- **Bluetooth** — adapter status, BLE/Classic scan, GATT read/write/notify,
  Nordic UART preset, and Classic RFCOMM (SPP).
- **Network** — TCP client/server and UDP, client sessions, unicast/broadcast/
  multicast, application framing, and auto-reply rules.
- **Shared tools** — traffic terminal, command history, HEX/ASCII conversion,
  checksums, run log, light/dark themes, and English/Chinese UI switching.

## Language

The application starts in **English**. Open **Settings → Language** and choose
**English** or **中文**; the choice applies immediately and is remembered for the
next launch.

This README defaults to English. Use the link at the top to switch to the
Chinese edition and back.

## Build and run

Requirements: Qt 6.6.3 `msvc2019_64`, Visual Studio Build Tools, CMake, and
Ninja.

```powershell
# Build, deploy Qt runtime files, and synchronize dist.
powershell -NoProfile -ExecutionPolicy Bypass -File scripts\build.ps1 -RunTests

# Create a self-contained package in dist\CommStudio.
powershell -NoProfile -ExecutionPolicy Bypass -File scripts\package.ps1
```

For a development launch:

```powershell
$env:PATH = "E:\QT\6.6.3\msvc2019_64\bin;$env:PATH"
.\build-msvc\CommStudio.exe
```

## Project layout

```text
src/
  main.cpp             Application entry point and QML singleton registration
  core/                Traffic model and protocol/codec utilities
  managers/            Serial, Modbus, Bluetooth, Network, settings, and logs
qml/
  Main.qml             Main window, navigation, status bar, and toast
  I18n.qml             English/Chinese UI language helper
  Theme.qml            Shared visual design tokens
  components/          Reusable controls, terminal, and send panel
  pages/               Assistant pages, tools, log, and settings
resources/             Windows application icon resources
tests/                 Core unit tests
```

## Runtime notes

- Theme and language preferences are persisted with QSettings under
  `HKCU\Software\TBEA\CommStudio`.
- A Bluetooth “Dummy backend running” message means the selected Qt Bluetooth
  package lacks a usable Windows backend; scanning and connecting will then be
  unavailable.
