# CommStudio Modbus UI 与功能设计指导（早期方案存档）

> **2026-09-27 更新：当前有效规范为 [CommStudio Modbus UI 设计规范](docs/UI_DESIGN_SPEC.md)。**
>
> 本文保留早期功能探索，不再作为 UI 实施依据。下文“不要做树”“交互重新设计”“高级参数统一 Drawer”“默认十列”和科技感升级等建议已被用户后续要求替代。当前必须采用 COM → 多设备树、紧凑完整工具栏、正确按钮语义、嵌套弹窗及靠右关闭钮。未实现能力见 [路线图](MODBUS_ROADMAP.md)，不能从本文的建议推断已完成。

## 历史方案原文（非当前规范）

> 目标：不推翻 CommStudio 当前已经完成的视觉体系和页面骨架，在现有“左侧工作区导航 + Modbus 连接配置栏 + 主工作区 Tab + 底部消息 Dock”结构上，吸收 MThings 在通道、设备、数据点、轮询、报文、仿真与统计方面的能力，并做成更适合 Qt 6 / Qt Quick 的现代化工业调试工作台。
>
> 原则：功能能力对齐，交互流程重新设计；保留 CommStudio 已有 UI 风格，只做结构增强、信息层级优化和轻度赛博科技感升级。

## 1. 现有界面评估与总体改造原则

从当前截图看，CommStudio 的 Modbus 页面骨架已经非常适合作为最终框架，不建议改成 MThings 那种传统“树 + 表格 + 大量弹窗/菜单”的桌面软件布局。

当前页面可以抽象为 5 个稳定区域：

```text
┌──────────────────────────────────────────────────────────────────────────────┐
│ A. 顶部应用栏：CommStudio / SERIAL / MODBUS / BLUETOOTH / NETWORK / 全局状态 │
├───────────────┬──────────────────────────────────────────────────────────────┤
│ B. 工作区导航 │ C. Modbus 连接/设备栏 │ D. Modbus 主工作区                  │
│               │                    │  Tab：轮询 / 手动 / 点表 / Raw      │
│               │                    │                                      │
│               │                    │                                      │
├───────────────┴────────────────────┴────────────────────────────────────────┤
│ E. 消息 / HEX / ASCII / 报文监视 Dock                                      │
└──────────────────────────────────────────────────────────────────────────────┘
```

这 5 个区域全部保留。

需要改变的不是“页面长什么样”，而是每个区域承载的逻辑：

- 左侧第一栏继续只做整个 CommStudio 的模块导航，不向里面塞 Modbus 细节。
- 左侧第二栏从“单设备连接配置”升级为“通道 + 设备上下文面板”。
- 中央区域继续保持 Tab 工作模式，但补齐点表、轮询、仿真、报文实验等完整能力。
- 不增加永久右侧属性栏，避免压缩现在已经很舒服的主工作区；复杂属性统一用右侧 Drawer / Sheet。
- 底部消息区升级为真正的协议 Trace Dock，而不是简单日志框。

最重要的数据模型统一为：

```text
Channel
  └── Device
       ├── Point Definition
       ├── Poll Plan
       ├── Transaction
       ├── Trend
       ├── Alarm / History
       └── Statistics
```

也就是说，当前 UI 外观基本不动，但内部必须从“一个串口 + 一个 Unit ID + 一个请求”升级成多通道、多设备、多点表的工程模型。

---

## 2. 左侧 Modbus 面板：从“连接配置”升级为“通道 / 设备上下文”

### 2.1 保留当前视觉位置和宽度

截图中第二栏宽度、输入框尺寸、按钮风格都可以继续使用，不建议拉宽。

现状：

```text
连接配置
模式       RTU 主站
端口
波特率     9600
校验位     None
Unit ID    1
超时/重试  1000 / 1
[连接主站]
```

建议升级后分为三个折叠区：

```text
连接配置
────────────────
▼ 通道
  模式          RTU 主站
  端口          COM5
  波特率        115200
  校验          None
  数据位        8
  停止位        1
  [ 高级参数 ]
  ● 已连接       12.4 KB/s
  [ 断开连接 ]

▼ 设备
  [搜索设备...]   [+]
  ● 01  主控制器
  ● 02  电表
  ○ 03  离线设备
  [+ 添加] [扫描]

▶ 会话参数
```

### 2.2 Unit ID 不再属于“通道参数”

这是当前结构里最需要调整的一点。

Unit ID / Slave ID 本质属于设备，而不是串口/TCP 通道本身。如果把 Unit ID 永久放在连接配置里，那么未来无法自然支持：

- 一根 RS485 总线挂多个从站；
- 一次扫描多个站号；
- 同一 TCP 通道访问多个 Unit ID；
- 每个设备拥有独立点表、轮询计划、超时统计。

因此建议：

- “连接配置”只保留通信链路参数；
- Unit ID 移入“设备”节点；
- 单设备快速模式下，创建连接后自动生成 Device 01，因此用户体验仍然不会变复杂。

### 2.3 模式选择建议

模式下拉框建议提供：

```text
RTU 主站
ASCII 主站
TCP 主站
RTU 从站模拟
ASCII 从站模拟
TCP 从站模拟
```

其中网络类模式切换时，面板字段动态变化：

**RTU / ASCII**

```text
串口
波特率
数据位
停止位
校验
流控（高级）
```

**TCP Client**

```text
目标 IP / Host
目标端口
本地 IP（高级）
本地端口（高级）
Keep Alive
自动重连
```

如果未来需要 TCP Server，可作为 TCP 从站模拟的一部分。

### 2.4 高级参数不要堆在主面板

以下参数统一放到“高级参数”右侧 Drawer：

- Timeout；
- Retry；
- 最小帧间隔；
- 地址偏移；
- CRC 字节序；
- Coil 位序；
- 写缓存时间；
- 自动重连周期；
- TCP 并发请求数；
- 自动轮询；
- 轮询最小周期；
- 广播地址。

这样当前截图的简洁性可以保留。

### 2.5 连接按钮状态设计

当前“连接主站”按钮保持原位置，但建议形成 4 态：

```text
未连接     [连接主站]
连接中     [● 正在连接...]  禁止重复点击
已连接     [断开连接]
异常       [重新连接]
```

按钮上方增加一条极细状态线即可，不需要大面积霓虹：

- Cyan：正常；
- Amber：重连；
- Magenta / Red：异常；
- Gray：未连接。

---

## 3. 中央 Tab 保持现结构，但重新定义职责

现有 Tab：

```text
轮询任务 | 手动读写 | 寄存器表 | Raw Test Center
```

这套结构是合理的，可以保留。建议只将“寄存器表”在产品文案上升级为：

```text
数据点
```

或者保留原名，但二级说明显示：

```text
寄存器表
Point Table
```

因为点表不仅包含 Holding Register，还会包含 Coil / Discrete Input / Input Register。

推荐最终 Tab：

```text
轮询任务   手动读写   数据点   Raw Test Center
```

当模式为“从站模拟”时动态增加：

```text
仿真数据
```

不建议把“曲线、历史、统计”全部继续做成一级 Tab，否则主工作区会很快膨胀。它们应该通过工具按钮 / 浮动工作台打开。

### 3.1 数据点 Tab：整个 Modbus 模块的核心

MThings 的核心价值实际上不是轮询按钮，而是将协议寄存器定义抽象成长期可复用的数据点。因此 CommStudio 中“数据点”应作为整个 Modbus 的基础配置中心。

推荐表格列：

```text
✓ | 名称 | 区域 | 地址 | 当前值 | 写入值 | 类型 | 单位 | 状态 | 更新时间
```

默认只显示这 10 列。

点击“更多列”后可显示：

```text
数量
位偏移
位数
原始类型
显示类型
Byte Order
Word Order
系数
偏移
小数位
轮询
周期
Timeout
Retry
分组
备注
```

不要一开始把 20 多列全部展开。

#### 数据区

提供：

```text
Coil                0x
Discrete Input      1x
Holding Register    4x
Input Register      3x
```

UI 可以用四种很弱的语义色 Badge 区分，而不是整行换色。

#### 原始类型

至少支持：

```text
BIT
UINT16 / INT16
UINT32 / INT32
UINT64 / INT64
FLOAT32 / FLOAT64
BCD
BYTES
STRING
```

#### 显示类型

```text
DEC
HEX
BIN
FLOAT
STRING
ENUM
TIMESTAMP
BIT FIELD
```

原始类型和显示类型分离。

例如：

```text
原始：UINT16
显示：ENUM
0 = STOP
1 = RUN
2 = FAULT
```

#### 字节序 / 字序

不要仅做“Big/Little”两个晦涩下拉框。

推荐下拉中直接可视化：

```text
AB CD      Byte BE / Word BE
BA DC      Byte LE / Word BE
CD AB      Byte BE / Word LE
DC BA      Byte LE / Word LE
```

FLOAT32 选中时甚至可以在 Drawer 中增加实时预览：

```text
收到：42 48 00 00
ABCD → 50.0
BADC → 50.0? 
CDAB → ...
DCBA → ...
```

这样现场调试效率会很高。

### 3.2 数据点详情使用右侧 Drawer

当前界面没有永久右栏，这是优点，不应该为了“高级感”硬塞一个 Inspector。

建议双击数据行或者点击“编辑”后，从右侧滑出 380~460 px Drawer：

```text
DATA POINT INSPECTOR
────────────────────
名称        DC Bus Voltage
区域        Holding Register
地址        0x0100
数量        2

Data Decode
原始类型    FLOAT32
Byte Order  AB CD
Word Order  CD AB
显示类型    Float
小数位      2

Engineering
系数        0.1
偏移        0
单位        V
范围        0 ~ 1000

Polling
参与轮询    ON
周期        100 ms
Timeout     inherit
Retry       inherit

[取消]               [应用]
```

这种方案比大量 Modal Dialog 更符合你现在的 UI。

### 3.3 数据点工具栏

数据点 Tab 顶部建议：

```text
[+ 新增] [复制] [删除] [批量编辑] [导入] [导出]

搜索名称 / 地址          区域筛选   分组   更多列
```

支持：

- Ctrl / Shift 多选；
- 批量修改；
- Excel/CSV 直接粘贴；
- 拖拽排序；
- 按区块 / 地址自动排序；
- 点表复制到其他设备；
- JSON/CSV 工程模板导入导出。

### 3.4 轮询任务 Tab：任务视角，而不是寄存器视角

当前截图里轮询任务表头已经有：

```text
启 | 名称 | 从站 | FC | 地址 | 数量 | 周期 | 类型 | 状态
```

这个方向可以保留。

但建议轮询任务分两层：

**简单模式**

用户在数据点表里勾选“轮询”，系统自动生成轮询计划。

**高级模式**

用户手动创建 Poll Task：

```text
任务名：Fast Telemetry
Device：Main Controller / ID 1
Function：03
Start：0x0100
Length：16
Period：20 ms
Priority：High
Timeout：100 ms
Retry：1
```

推荐表格：

```text
启用 | 名称 | 设备 | FC | 地址范围 | 周期 | 成功率 | Avg RTT | 最近状态
```

鼠标悬停状态显示：

```text
Last TX    14:24:22.310
Last RX    14:24:22.316
RTT        6.2 ms
Success    99.98 %
Timeout    2
Exception  0
```

### 3.5 自动组包

需要在底层实现 MThings 类似的连续地址合并能力，但交互不要做复杂。

数据点设置里只有一个：

```text
读取策略
● 自动优化
○ 每点独立请求
○ 按表格顺序合并
```

“自动优化”时：

```text
40001 U16
40002 U16
40003 FLOAT32 (2 Reg)
40005 U16
```

可自动合并成：

```text
FC03 Start=0 Count=5
```

允许的空洞范围、最大连续寄存器数放进高级参数。

### 3.6 手动读写 Tab

这部分定位为“无需建立点表即可快速测试”。

推荐布局仍放在主工作区，不新开窗口：

```text
设备       [Main Controller / ID 1]
功能码     [03 Read Holding Registers]
起始地址   [0x0000]
数量       [10]

[发送]
```

读取结果下方使用结构化结果表：

```text
Address    HEX     UINT16    INT16
0x0000     0064    100       100
0x0001     FF9C    65436     -100
```

选择两寄存器后允许临时解析为：

```text
UINT32 / INT32 / FLOAT32
ABCD / BADC / CDAB / DCBA
```

写操作根据 FC 自动切换输入模式：

```text
05 Write Single Coil
06 Write Single Register
0F Write Multiple Coils
10 Write Multiple Registers
```

写入前建议使用二次确认的场景仅限批量写 / 广播写 / 可配置危险设备；普通单寄存器写不要每次弹框。

### 3.7 Raw Test Center

这个 Tab 可以保留英文名，因为已经形成你产品里的专业工具味道。

建议分成三块：

```text
Request Builder
────────────────
Device     ID 01
PDU        03 00 00 00 0A
[自动封装 RTU] [发送]

Frame Preview
────────────────
01 03 00 00 00 0A C5 CD
│  │  └────────┘ └───┘
ID FC      PDU       CRC

Response / Decoder
────────────────
RX: 01 03 14 ... CRC
RTT: 4.8 ms
```

增加：

- 自定义 Function Code；
- 循环发送；
- 发送次数；
- 间隔；
- Timeout；
- RTT min/max/avg；
- 响应异常码解析；
- 保存测试模板；
- 将当前请求固定为快捷卡片。

高级测试模式可加入：

```text
Loop Count
Interval Jitter
随机地址
随机数据
异常帧注入（从站模拟）
```

---

## 4. 底部消息区域升级为 Protocol Trace Dock

你现在截图最下方的“消息 HEX + ASCII”区域位置非常好，建议直接作为统一 Trace Dock，不再另做 MThings 式报文窗口。

当前工具：

```text
HEX+ASCII | 暂停 | 清屏 | 导出 | 自动跟随
```

全部保留。

建议补充：

```text
过滤器
[全部通道] [全部设备] [全部 FC] [TX/RX/ERR] [搜索]
```

每一条报文建议展示：

```text
14:24:22.547  TX  COM5  ID01  FC03  8B  01 03 00 00 00 0A C5 CD  4.8ms
14:24:22.552  RX  COM5  ID01  FC03  25B 01 03 14 ...             OK
```

颜色只用于关键语义：

- TX：Cyan；
- RX：Green / Teal；
- Exception：Magenta / Red；
- Timeout：Amber；
- Connect/Disconnect：Purple。

### 4.1 单击报文展开协议解析

这是非常推荐加入的功能。

单击一条记录后，消息 Dock 可以向上临时展开到 260~320 px：

```text
MODBUS RTU FRAME
────────────────────────
Slave Address     0x01
Function          0x03 Read Holding Registers
Start Address     0x0000
Quantity          10
CRC               0xCDC5   ✓ Valid

Mapped Points
────────────────────────
40001  Bus Voltage   398.2 V
40003  Current       10.15 A
40005  Status        RUN
```

这样可以直接利用已经配置的数据点反向解释报文。

### 4.2 报文性能保护

高波特率情况下不要让 QML TextArea 无限追加。

建议：

```text
后台 Packet Ring Buffer：50k ~ 200k 条
UI 当前窗口：1000 ~ 5000 条
UI 刷新节流：30~60 Hz
批量 append，而不是每帧刷新 QML
```

当速率过高显示提示：

```text
LIVE TRACE  12.6k msg/s
Rendering throttled
```

数据本身仍完整保存。

---

## 5. 在当前布局里加入设备扫描、仿真、曲线和统计

### 5.1 设备扫描

不增加新页面。

点击左侧设备区：

```text
[扫描]
```

打开右侧 Drawer：

```text
DEVICE SCANNER
────────────────
范围       1 ~ 247
请求       FC03
地址       0
Timeout    100 ms

[开始扫描]

01   5 ms    ONLINE
02   --      NO RESPONSE
03   8 ms    ONLINE
...

[添加已发现设备]
```

扫描时主界面顶部的小状态统计显示进度即可。

### 5.2 从站仿真

当左侧“模式”切换为：

```text
RTU 从站模拟
TCP 从站模拟
```

中央 Tab 自动变成：

```text
仿真数据 | Raw Test Center
```

仿真数据表仍复用“数据点”模型，只多一列：

```text
生成策略
```

策略：

```text
固定值
线性变化
抛物线
正弦波
方波
随机值
序列数据
绑定其他点
脚本
```

配置在右侧 Drawer 中完成。

例如正弦：

```text
Offset      230
Amplitude   10
Frequency   0.2 Hz
Noise       ±0.2
Min         200
Max         260
Update      20 ms
```

可以进一步加你自己的特色：

```text
故障注入
CRC 错误
异常功能码
随机 Timeout
响应延迟
丢帧概率
错误字节数
```

这会让 CommStudio 比普通 Modbus Poll 工具更适合嵌入式联调和可靠性测试。

### 5.3 曲线

不建议加“曲线”一级 Tab。

在“数据点”表格顶部放：

```text
[📈 趋势]
```

勾选多个数据点后点击，打开独立 Workspace Panel / 浮动窗口。

建议功能：

- 最多 32~50 条曲线；
- 左右 Y 轴；
- 自动量程；
- 游标；
- 参考线；
- 时间窗口；
- Pause；
- PNG；
- CSV；
- Min / Max / Avg；
- 警戒线。

不要把曲线直接塞进当前表格下方，否则会和 Trace Dock 抢空间。

### 5.4 链路统计

右上角现在已有：

```text
TX 0 · RX 0 · 异常 0 · 响应 0 ms
```

这个位置非常适合继续扩展，不要另建常驻统计栏。

鼠标点击该区域弹出 Popover：

```text
SESSION STATISTICS
──────────────────
TX Frames       123420
RX Frames       123417
TX Bytes        8.6 MB
RX Bytes        32.4 MB
Timeout         3
Exception       0
CRC Error       0
Frame Error     0
Success         99.997%

RTT
Current         4.3 ms
Average         4.8 ms
Min             3.9 ms
Max             21.4 ms

[重置] [打开统计详情]
```

统计详情可以再做长期曲线。

---

## 6. 现有界面的科技感升级建议

当前视觉是“深蓝灰 + 低饱和薰衣草蓝”，基础是好的，不建议改成典型游戏式 Cyberpunk。

推荐方向：

```text
Industrial Dark
+
Aurora Cyan
+
Electric Violet
+
少量 Magenta Alert
```

### 6.1 色彩语义

现有主色可以保留：

```text
Background 0     #1B1F2A 左右
Background 1     #212735 左右
Panel             #252B3A 左右
Primary           当前的 Lavender Blue
```

新增几个仅用于状态的高纯度颜色：

```text
Cyber Cyan        连接 / TX / Action
Aurora Green      RX / Success
Electric Violet   Selected / Active Tab
Amber             Warning / Timeout
Magenta           Protocol Exception
```

不要让所有 Border 都发光。

### 6.2 Tab

当前选中 Tab 是整块浅蓝背景。

可以改成：

```text
未选：透明 + 细边框
Hover：淡蓝半透明底
Active：暗色底 + 顶部/底部 2 px Cyan/Violet Gradient Line
```

这样更“科技工作站”，也比整块淡紫按钮更精致。

### 6.3 面板边框

现在大量 1px 白灰边框比较明显。

建议：

```text
普通 Border：10~18% opacity
Active Border：Primary 40~60%
Focus Border：Cyan 70%
```

鼠标 Focus 时才产生 2~4 px 很轻的 Glow。

### 6.4 数据状态动画

允许少量微动效：

- Connected 状态点缓慢呼吸；
- 新 RX 帧出现时 RX 数字轻微闪一下；
- 正在扫描时使用 moving highlight；
- 当前轮询任务运行时状态图标旋转或 pulse；
- 值发生变化时单元格背景 150~250 ms 淡入淡出。

不要做：

- 大面积扫描线；
- 背景粒子；
- 持续闪烁；
- 大面积 Neon Glow；
- 高亮粉紫文字铺满界面。

这是工程工具，不是游戏 HUD。

---

## 7. Qt 6 / QML 实现结构建议

建议保持 UI 与协议层完全分离。

```text
src/
├── protocol/modbus/
│   ├── ModbusChannel
│   ├── ModbusDevice
│   ├── ModbusPoint
│   ├── ModbusCodec
│   ├── ModbusTransaction
│   ├── ModbusPollScheduler
│   ├── ModbusFrameParser
│   ├── ModbusSimulator
│   └── ModbusStatistics
│
├── transport/
│   ├── SerialTransport
│   ├── TcpTransport
│   └── UdpTransport
│
├── models/
│   ├── DeviceListModel
│   ├── PointTableModel
│   ├── PollTaskModel
│   ├── PacketTraceModel
│   └── StatisticsModel
│
└── qml/modbus/
    ├── ModbusPage.qml
    ├── ModbusContextPanel.qml
    ├── DeviceList.qml
    ├── PollPage.qml
    ├── ManualPage.qml
    ├── PointTablePage.qml
    ├── RawTestPage.qml
    ├── SimulatorPage.qml
    ├── TraceDock.qml
    ├── PointInspectorDrawer.qml
    ├── ChannelAdvancedDrawer.qml
    ├── DeviceScannerDrawer.qml
    └── StatisticsPopover.qml
```

### 7.1 PointTable

大量数据不要用 QML ListModel 保存业务数据。

推荐：

```text
C++ QAbstractTableModel
        ↓
QSortFilterProxyModel
        ↓
QML TableView
```

点表模型只存最新值，不要为每个采样点建立 QObject。

### 7.2 Transport 和协议线程

推荐：

```text
UI Thread
   │
   ├── Model / View
   │
Worker Thread
   ├── Serial / TCP IO
   ├── Frame Parse
   ├── Transaction Manager
   └── Poll Scheduler
```

QML 不直接触碰串口。

### 7.3 Transaction Manager

所有操作统一经过：

```text
Manual Request
Poll Request
Write Request
Raw Test Request
         ↓
Transaction Queue
         ↓
Transport
         ↓
Response Matching
         ↓
Decode
         ↓
Point Model / Trace / Statistics
```

RTU 同一通道一般串行执行事务。

TCP 可以使用 Transaction ID 做并发匹配，后期再开放“最大并发请求数”。

### 7.4 优先级

建议：

```text
P0：用户手动写
P1：用户手动读 / Raw Test
P2：高优先级轮询
P3：普通轮询
P4：后台低速任务
```

这样用户点击“读取”不会因为后台几十个轮询任务而卡很久。

---

## 8. 建议最终界面形态

最终不要重新设计成另一套软件，直接在你现在截图基础上演进为：

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│ CommStudio                       MODBUS                       ● CONNECTED     │
│                                         TX 12k · RX 12k · ERR 0 · 4.8 ms    │
├──────────────┬───────────────────┬──────────────────────────────────────────┤
│ WORKSPACES   │ MODBUS CONTEXT    │ 轮询任务  手动读写  数据点  Raw Test    │
│              │                   │ ───────────────────────────────────────  │
│ 串口调试     │ ▼ CHANNEL         │                                          │
│ Modbus       │ RTU Master        │              主工作区                    │
│ 蓝牙         │ COM5 / 115200     │                                          │
│ 网络         │ 8-N-1             │                                          │
│              │ ● CONNECTED       │                                          │
│ 工具箱       │                   │                                          │
│ 运行日志     │ ▼ DEVICES         │                                          │
│ 设置         │ ● 01 Main Ctrl    │                                          │
│              │ ● 02 Meter        │                                          │
│              │ ○ 03 Offline      │                                          │
│              │                   │                                          │
│              │ [+] [SCAN]        │                                          │
├──────────────┴───────────────────┴──────────────────────────────────────────┤
│ TRACE  HEX+ASCII   Channel:ALL Device:ALL FC:ALL    Pause Clear Export Auto │
│ 14:24:22.547 TX COM5 ID01 FC03  01 03 00 00 00 0A C5 CD                   │
│ 14:24:22.552 RX COM5 ID01 FC03  01 03 14 ...                   4.8ms OK     │
└─────────────────────────────────────────────────────────────────────────────┘
```

这一版最大的优势是：

1. 用户第一眼看到的软件仍然是现在的 CommStudio，而不是突然变成 MThings。
2. 简单用户依旧可以“选 COM → 连接 → 手动读写”，不会因为工程化设计变复杂。
3. 高级用户可以进一步建立多个 Device、点表、轮询、曲线和仿真。
4. Serial / Bluetooth / Network 模块未来也可以复用同一套 Workspace / Trace / Drawer 交互规范。
5. UI 保持现代、克制、科技感强，同时仍然是一个长时间使用不会疲劳的工程工具。

## 9. 推荐实际开发优先级

如果现在就按这套改，不建议先做视觉特效，推荐开发顺序：

**第一优先级：把数据模型改正确**

Channel → Device → Point，Unit ID 从全局连接配置中拆出去。

**第二优先级：完成数据点 Tab**

数据类型、地址、字节序、字序、系数、偏移、枚举、位域。

**第三优先级：统一 Transaction Manager**

轮询、手动读、写、Raw Test 全部走同一事务队列。

**第四优先级：升级 Trace Dock**

原始帧、解析、筛选、导出、RTT、异常信息。

**第五优先级：轮询任务和自动组包**

将点表真正转换为高效率数据采集能力。

**第六优先级：Slave Simulator / Device Scan / Statistics / Trend**

这是形成产品差异化的部分。

**最后：视觉打磨**

增加 Cyan / Violet 状态语义、微动效、Drawer、Popover 和轻量 Glow，而不是重新换整套皮肤。

