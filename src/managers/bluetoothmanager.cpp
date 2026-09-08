#include "bluetoothmanager.h"

#include "core/hexcodec.h"
#include "studio.h"

#include <QCoreApplication>
#include <QFile>

#if QT_CONFIG(permissions)
#include <QPermissions>
#endif

namespace {

Studio *g_studio = nullptr;

struct KnownUuid {
    const char *uuid;
    const char *name;
};

// 常见 GATT / 服务 UUID 名称速查表。
const KnownUuid knownUuids[] = {
    { "00001800-0000-1000-8000-00805f9b34fb", "Generic Access" },
    { "00001801-0000-1000-8000-00805f9b34fb", "Generic Attribute" },
    { "0000180a-0000-1000-8000-00805f9b34fb", "Device Information" },
    { "0000180d-0000-1000-8000-00805f9b34fb", "Heart Rate" },
    { "0000180f-0000-1000-8000-00805f9b34fb", "Battery Service" },
    { "00002a00-0000-1000-8000-00805f9b34fb", "Device Name" },
    { "00002a01-0000-1000-8000-00805f9b34fb", "Appearance" },
    { "00002a05-0000-1000-8000-00805f9b34fb", "Service Changed" },
    { "00002a19-0000-1000-8000-00805f9b34fb", "Battery Level" },
    { "00002a29-0000-1000-8000-00805f9b34fb", "Manufacturer Name" },
    { "00002a24-0000-1000-8000-00805f9b34fb", "Model Number" },
    { "00002a26-0000-1000-8000-00805f9b34fb", "Firmware Revision" },
    { "00002a37-0000-1000-8000-00805f9b34fb", "Heart Rate Measurement" },
    { "6e400001-b5a3-f393-e0a9-e50e24dcca9e", "Nordic UART Service" },
    { "6e400002-b5a3-f393-e0a9-e50e24dcca9e", "Nordic UART RX (Write)" },
    { "6e400003-b5a3-f393-e0a9-e50e24dcca9e", "Nordic UART TX (Notify)" },
};

QString knownUuidName(const QString &uuid)
{
    for (const KnownUuid &entry : knownUuids) {
        if (uuid.compare(QLatin1String(entry.uuid), Qt::CaseInsensitive) == 0)
            return QLatin1String(entry.name);
    }
    return QString();
}

} // namespace

BluetoothManager::BluetoothManager(QObject *parent)
    : QObject(parent)
    , m_traffic(new TrafficModel(this))
{
    if (qobject_cast<Studio *>(parent))
        g_studio = qobject_cast<Studio *>(parent);

    m_traffic->appendInfo(QStringLiteral("蓝牙助手就绪 · 扫描前请确认系统蓝牙已开启"));
    refreshAdapters();
}

bool BluetoothManager::backendSupported() const
{
    return QBluetoothDeviceDiscoveryAgent::supportedDiscoveryMethods()
           != QBluetoothDeviceDiscoveryAgent::DiscoveryMethod::NoMethod;
}

QVariantList BluetoothManager::adapters() const
{
    QVariantList result;
    const auto interfaces = QBluetoothLocalDevice::allDevices();
    for (const QBluetoothHostInfo &info : interfaces) {
        QBluetoothLocalDevice device(info.address());
        QVariantMap map;
        map.insert(QStringLiteral("name"), info.name());
        map.insert(QStringLiteral("address"), info.address().toString());
        map.insert(QStringLiteral("valid"), device.isValid());
        QString modeText;
        switch (device.hostMode()) {
        case QBluetoothLocalDevice::HostConnectable:
            modeText = QStringLiteral("可连接"); break;
        case QBluetoothLocalDevice::HostDiscoverable:
        case QBluetoothLocalDevice::HostDiscoverableLimitedInquiry:
            modeText = QStringLiteral("可发现"); break;
        case QBluetoothLocalDevice::HostPoweredOff:
            modeText = QStringLiteral("已关闭"); break;
        default:
            modeText = QStringLiteral("未知"); break;
        }
        map.insert(QStringLiteral("mode"), modeText);
        result.append(map);
    }
    return result;
}

void BluetoothManager::refreshAdapters()
{
    emit adaptersChanged();
}

bool BluetoothManager::scanning() const
{
    return m_discoveryAgent && m_discoveryAgent->isActive();
}

void BluetoothManager::requestPermission()
{
#if QT_CONFIG(permissions)
    QBluetoothPermission permission;
    permission.setCommunicationModes(QBluetoothPermission::CommunicationModes(
        QBluetoothPermission::Access | QBluetoothPermission::Advertise));
    switch (qApp->checkPermission(permission)) {
    case Qt::PermissionStatus::Undetermined:
        qApp->requestPermission(permission, this, [this](const QPermission &) {
            startScan();
        });
        return;
    case Qt::PermissionStatus::Denied:
        m_traffic->appendInfo(QStringLiteral("蓝牙权限被拒绝，扫描无法进行"), true);
        if (g_studio)
            g_studio->log(QStringLiteral("error"), QStringLiteral("蓝牙权限被拒绝"));
        return;
    case Qt::PermissionStatus::Granted:
        break;
    }
#endif
    // 授权后真正启动扫描。
    QTimer::singleShot(0, this, [this] { startScan(); });
}

void BluetoothManager::startScan()
{
    // 平台能力预检：MinGW 官方包的 Qt Bluetooth 为 Dummy 后端，
    // 不支持任何发现方法，提前给出可操作的提示而不是晦涩的扫描错误。
    if (QBluetoothDeviceDiscoveryAgent::supportedDiscoveryMethods()
        == QBluetoothDeviceDiscoveryAgent::DiscoveryMethod::NoMethod) {
        const QString hint = QStringLiteral(
            "当前 Qt 构建为 Dummy 蓝牙后端，不支持扫描。"
            "请改用带 Windows 蓝牙后端的 Qt 构建（如 MSVC 官方包）后重新编译运行。");
        m_traffic->appendInfo(hint, true);
        if (g_studio)
            g_studio->log(QStringLiteral("warning"),
                          QStringLiteral("蓝牙扫描不可用: Qt Dummy 后端"));
        emit toastRequested(QStringLiteral("当前 Qt 构建不含 Windows 蓝牙后端，扫描不可用"), false);
        return;
    }
#if QT_CONFIG(permissions)
    QBluetoothPermission permission;
    permission.setCommunicationModes(QBluetoothPermission::CommunicationModes(
        QBluetoothPermission::Access | QBluetoothPermission::Advertise));
    switch (qApp->checkPermission(permission)) {
    case Qt::PermissionStatus::Undetermined:
        requestPermission();
        return;
    case Qt::PermissionStatus::Denied:
        m_traffic->appendInfo(QStringLiteral("蓝牙权限被拒绝，扫描无法进行"), true);
        return;
    case Qt::PermissionStatus::Granted:
        break;
    }
#endif
    if (!m_discoveryAgent) {
        m_discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
        m_discoveryAgent->setLowEnergyDiscoveryTimeout(12000);
        connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
                this, [this](const QBluetoothDeviceInfo &info) {
                    addDiscoveredDevice(info);
                });
        connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceUpdated,
                this, [this](const QBluetoothDeviceInfo &info, QBluetoothDeviceInfo::Fields) {
                    addDiscoveredDevice(info);
                });
        connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished, this, [this] {
            m_traffic->appendInfo(QStringLiteral("扫描结束 · 共发现 %1 个设备").arg(m_deviceRows.size()));
            emit scanningChanged();
        });
        connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::errorOccurred,
                this, [this](QBluetoothDeviceDiscoveryAgent::Error error) {
                    if (error == QBluetoothDeviceDiscoveryAgent::NoError)
                        return;
                    m_traffic->appendInfo(QStringLiteral("扫描错误: %1")
                                              .arg(m_discoveryAgent->errorString()), true);
                    if (g_studio)
                        g_studio->log(QStringLiteral("error"),
                                      QStringLiteral("蓝牙扫描错误: %1")
                                          .arg(m_discoveryAgent->errorString()));
                    emit scanningChanged();
                });
    }
    if (m_discoveryAgent->isActive())
        return;
    m_deviceRows.clear();
    rebuildDeviceList();
    m_traffic->appendInfo(QStringLiteral("开始扫描 · 最长 12 秒"));
    m_discoveryAgent->start(
        m_leOnly ? QBluetoothDeviceDiscoveryAgent::DiscoveryMethods(
                       QBluetoothDeviceDiscoveryAgent::DiscoveryMethod::LowEnergyMethod)
                 : QBluetoothDeviceDiscoveryAgent::DiscoveryMethods(
                       QBluetoothDeviceDiscoveryAgent::DiscoveryMethod::ClassicMethod
                           | QBluetoothDeviceDiscoveryAgent::DiscoveryMethod::LowEnergyMethod));
    emit scanningChanged();
}

void BluetoothManager::addDiscoveredDevice(const QBluetoothDeviceInfo &info)
{
    const bool isLe = info.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration;
    if (m_leOnly && !isLe)
        return;

    QString key;
    if (info.address().isNull() && !info.deviceUuid().isNull())
        key = info.deviceUuid().toString(); // Windows BLE 以 UUID 标识设备
    else
        key = info.address().toString();
    if (key.isEmpty())
        return;

    for (DeviceRow &row : m_deviceRows) {
        if (row.key == key) {
            row.rssi = info.rssi();
            row.hasRssi = true;
            rebuildDeviceList();
            return;
        }
    }

    DeviceRow row;
    row.info = info;
    row.key = key;
    row.name = info.name().isEmpty()
                   ? QStringLiteral("(未知设备)") : info.name();
    row.type = (info.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration)
                   ? (info.coreConfigurations() & QBluetoothDeviceInfo::BaseRateCoreConfiguration
                          ? QStringLiteral("双模") : QStringLiteral("BLE"))
                   : QStringLiteral("Classic");
    row.rssi = info.rssi();
    row.hasRssi = info.rssi() != 0 && info.rssi() != -127;
    for (const QBluetoothUuid &uuid : info.serviceUuids())
        row.serviceUuids << uuid.toString();
    const QList<quint16> manufacturerIds = info.manufacturerIds();
    if (!manufacturerIds.isEmpty()) {
        QString manufacturers;
        for (quint16 id : manufacturerIds) {
            if (!manufacturers.isEmpty())
                manufacturers += QStringLiteral(", ");
            manufacturers += QStringLiteral("0x%1(%2 字节)")
                                 .arg(id, 4, 16, QLatin1Char('0')).toUpper()
                                 .arg(info.manufacturerData(id).size());
        }
        row.manufacturer = manufacturers;
    }
    m_deviceRows.append(row);
    rebuildDeviceList();
}

void BluetoothManager::rebuildDeviceList()
{
    QVariantList list;
    for (const DeviceRow &row : m_deviceRows) {
        if (!m_nameFilter.isEmpty()
            && !row.name.contains(m_nameFilter, Qt::CaseInsensitive)
            && !row.key.contains(m_nameFilter, Qt::CaseInsensitive))
            continue;
        QVariantMap map;
        map.insert(QStringLiteral("name"), row.name);
        map.insert(QStringLiteral("key"), row.key);
        map.insert(QStringLiteral("type"), row.type);
        map.insert(QStringLiteral("rssi"), row.hasRssi ? row.rssi : 0);
        map.insert(QStringLiteral("hasRssi"), row.hasRssi);
        map.insert(QStringLiteral("services"), row.serviceUuids.join(QStringLiteral(" ")));
        map.insert(QStringLiteral("manufacturer"), row.manufacturer);
        list.append(map);
    }
    m_devices = list;
    emit devicesChanged();
}

void BluetoothManager::stopScan()
{
    if (m_discoveryAgent && m_discoveryAgent->isActive()) {
        m_discoveryAgent->stop();
        m_traffic->appendInfo(QStringLiteral("扫描已停止"));
        emit scanningChanged();
    }
}

void BluetoothManager::clearDevices()
{
    m_deviceRows.clear();
    rebuildDeviceList();
}

void BluetoothManager::setNameFilter(const QString &filter)
{
    if (m_nameFilter == filter)
        return;
    m_nameFilter = filter;
    rebuildDeviceList();
    emit nameFilterChanged();
}

void BluetoothManager::setLeOnly(bool enabled)
{
    if (m_leOnly == enabled)
        return;
    m_leOnly = enabled;
    emit leOnlyChanged();
}

QString BluetoothManager::leState() const
{
    if (!m_controller)
        return QStringLiteral("未连接");
    switch (m_controller->state()) {
    case QLowEnergyController::UnconnectedState: return QStringLiteral("未连接");
    case QLowEnergyController::ConnectingState: return QStringLiteral("连接中");
    case QLowEnergyController::ConnectedState: return QStringLiteral("已连接");
    case QLowEnergyController::DiscoveringState: return QStringLiteral("发现服务中");
    case QLowEnergyController::DiscoveredState: return QStringLiteral("服务已发现");
    case QLowEnergyController::ClosingState: return QStringLiteral("断开中");
    default: return QStringLiteral("错误");
    }
}

int BluetoothManager::mtu() const
{
    return m_controller ? m_controller->mtu() : 0;
}

void BluetoothManager::setSelectedService(int index)
{
    if (m_selectedService == index)
        return;
    m_selectedService = index;
    emit selectedServiceChanged();
}

void BluetoothManager::connectDevice(int index)
{
    if (index < 0 || index >= m_deviceRows.size())
        return;
    stopScan();
    const QBluetoothDeviceInfo info = m_deviceRows.at(index).info;
    m_connectedKey = m_deviceRows.at(index).key;
    m_connectedName = m_deviceRows.at(index).name;
    connectLeController(info);
}

void BluetoothManager::connectLeController(const QBluetoothDeviceInfo &info)
{
    disconnectDevice();
    m_controller = QLowEnergyController::createCentral(info, this);

    connect(m_controller, &QLowEnergyController::connected, this, [this] {
        m_traffic->appendInfo(QStringLiteral("BLE 已连接，开始发现服务…"));
        emit leStateChanged();
        discoverServices();
    });
    connect(m_controller, &QLowEnergyController::disconnected, this, [this] {
        m_traffic->appendInfo(QStringLiteral("BLE 连接已断开"), true);
        if (g_studio) {
            g_studio->log(QStringLiteral("warning"), QStringLiteral("BLE %1 断开").arg(m_connectedName));
            g_studio->addTraffic(0, 0, -1);
        }
        qDeleteAll(m_serviceObjects);
        m_serviceObjects.clear();
        m_services.clear();
        m_uartMode = false;
        m_uartService = nullptr;
        m_notifyActive = false;
        emit servicesChanged();
        emit leStateChanged();
    });
    connect(m_controller, &QLowEnergyController::errorOccurred, this,
            [this](QLowEnergyController::Error error) {
                if (error == QLowEnergyController::NoError)
                    return;
                const QString message = QStringLiteral("BLE 错误: %1").arg(m_controller->errorString());
                m_traffic->appendInfo(message, true);
                if (g_studio)
                    g_studio->log(QStringLiteral("error"), message);
                emit leStateChanged();
            });
    connect(m_controller, &QLowEnergyController::stateChanged,
            this, &BluetoothManager::leStateChanged);

    m_traffic->appendInfo(QStringLiteral("正在连接 BLE 设备 %1 …").arg(m_connectedName));
    m_controller->connectToDevice();
    emit leStateChanged();
}

void BluetoothManager::disconnectDevice()
{
    if (!m_controller)
        return;
    if (m_uartService) {
        m_uartService = nullptr;
        m_uartMode = false;
    }
    m_controller->disconnectFromDevice();
    m_controller->deleteLater();
    m_controller = nullptr;
    qDeleteAll(m_serviceObjects);
    m_serviceObjects.clear();
    m_services.clear();
    m_notifyActive = false;
    emit servicesChanged();
    emit leStateChanged();
}

void BluetoothManager::discoverServices()
{
    if (!m_controller)
        return;
    connect(m_controller, &QLowEnergyController::serviceDiscovered, this, [this](const QBluetoothUuid &) {
        emit leStateChanged();
    });
    connect(m_controller, &QLowEnergyController::discoveryFinished,
            this, &BluetoothManager::rebuildServiceList);
    m_controller->discoverServices();
}

void BluetoothManager::rebuildServiceList()
{
    if (!m_controller)
        return;
    qDeleteAll(m_serviceObjects);
    m_serviceObjects.clear();
    m_services.clear();
    m_uartService = nullptr;

    const QList<QBluetoothUuid> serviceUuids = m_controller->services();
    for (const QBluetoothUuid &uuid : serviceUuids) {
        QLowEnergyService *service = m_controller->createServiceObject(uuid, this);
        if (!service)
            continue;

        connect(service, &QLowEnergyService::stateChanged, this, [this, service] {
            if (service->state() == QLowEnergyService::RemoteServiceDiscovered)
                rebuildServiceList();
        });
        connect(service, &QLowEnergyService::characteristicChanged, this,
                [this](const QLowEnergyCharacteristic &characteristic, const QByteArray &value) {
                    m_rxBytes += value.size();
                    m_traffic->append(TrafficModel::Rx, value,
                                      QStringLiteral("Notify %1").arg(characteristic.uuid().toString()));
                    if (g_studio)
                        g_studio->addTraffic(value.size(), 0, 0);
                    emit statsChanged();
                    rebuildServiceList();
                });
        connect(service, &QLowEnergyService::characteristicRead, this,
                [this](const QLowEnergyCharacteristic &characteristic, const QByteArray &value) {
                    m_traffic->append(TrafficModel::Rx, value,
                                      QStringLiteral("Read %1").arg(characteristic.uuid().toString()));
                    rebuildServiceList();
                });
        connect(service, &QLowEnergyService::characteristicWritten, this,
                [this](const QLowEnergyCharacteristic &characteristic, const QByteArray &value) {
                    m_traffic->append(TrafficModel::Rx, value,
                                      QStringLiteral("WriteAck %1").arg(characteristic.uuid().toString()));
                    rebuildServiceList();
                });
        connect(service, &QLowEnergyService::errorOccurred, this,
                [this, service](QLowEnergyService::ServiceError error) {
                    m_traffic->appendInfo(QStringLiteral("服务操作错误 (%1): %2")
                                              .arg(service->serviceUuid().toString())
                                              .arg(error), true);
                });

        m_serviceObjects.append(service);

        QVariantMap serviceMap;
        const QString uuidText = uuid.toString();
        serviceMap.insert(QStringLiteral("uuid"), uuidText);
        const QString known = knownUuidName(uuidText);
        serviceMap.insert(QStringLiteral("name"), known.isEmpty() ? QStringLiteral("自定义服务") : known);
        serviceMap.insert(QStringLiteral("custom"), known.isEmpty());

        QVariantList characteristics;
        for (const QLowEnergyCharacteristic &characteristic : service->characteristics()) {
            const QString charUuid = characteristic.uuid().toString();
            QVariantMap charMap;
            charMap.insert(QStringLiteral("uuid"), charUuid);
            const QString charKnown = knownUuidName(charUuid);
            charMap.insert(QStringLiteral("name"), charKnown.isEmpty() ? QStringLiteral("特征") : charKnown);
            charMap.insert(QStringLiteral("properties"), propertyText(characteristic));
            const int charProps = int(characteristic.properties());
            charMap.insert(QStringLiteral("canRead"), bool(charProps & int(QLowEnergyCharacteristic::Read)));
            charMap.insert(QStringLiteral("canWrite"), bool(charProps & int(QLowEnergyCharacteristic::Write)));
            charMap.insert(QStringLiteral("canWriteNoResponse"),
                           bool(charProps & int(QLowEnergyCharacteristic::WriteNoResponse)));
            charMap.insert(QStringLiteral("canNotify"), bool(charProps & int(QLowEnergyCharacteristic::Notify)));
            charMap.insert(QStringLiteral("canIndicate"), bool(charProps & int(QLowEnergyCharacteristic::Indicate)));
            charMap.insert(QStringLiteral("valueHex"), HexCodec::toHex(characteristic.value()));
            charMap.insert(QStringLiteral("valueText"), QString::fromUtf8(characteristic.value()));
            charMap.insert(QStringLiteral("valueSize"), characteristic.value().size());

            QVariantList descriptors;
            for (const QLowEnergyDescriptor &descriptor : characteristic.descriptors()) {
                QVariantMap descMap;
                descMap.insert(QStringLiteral("uuid"), descriptor.uuid().toString());
                descMap.insert(QStringLiteral("valueHex"), HexCodec::toHex(descriptor.value()));
                descriptors.append(descMap);
            }
            charMap.insert(QStringLiteral("descriptors"), descriptors);
            characteristics.append(charMap);
        }
        serviceMap.insert(QStringLiteral("characteristics"), characteristics);
        m_services.append(serviceMap);

        service->discoverDetails();
    }

    m_traffic->appendInfo(QStringLiteral("服务发现完成 · 共 %1 个服务").arg(m_services.size()));
    if (g_studio) {
        g_studio->log(QStringLiteral("success"), QStringLiteral("BLE %1 服务发现完成 (%2 个服务)")
                          .arg(m_connectedName).arg(m_services.size()));
        g_studio->addTraffic(0, 0, 1);
    }
    if (m_selectedService >= m_services.size())
        m_selectedService = 0;
    emit servicesChanged();
    emit selectedServiceChanged();
    emit leStateChanged();

    // UART 模式挂起请求。
    if (m_uartMode && !m_uartService && !m_uartPendingServiceUuid.isEmpty())
        enableUartMode(m_uartPendingServiceUuid, m_uartPendingTxUuid, m_uartPendingRxUuid);
}

QLowEnergyService *BluetoothManager::serviceObjectAt(int index) const
{
    if (index < 0 || index >= m_services.size() || index >= m_serviceObjects.size())
        return nullptr;
    return m_serviceObjects.at(index);
}

QString BluetoothManager::uuidName(const QBluetoothUuid &uuid)
{
    return knownUuidName(uuid.toString());
}

QString BluetoothManager::propertyText(const QLowEnergyCharacteristic &characteristic)
{
    const int properties = int(characteristic.properties());
    QStringList parts;
    if (properties & int(QLowEnergyCharacteristic::Read)) parts << QStringLiteral("Read");
    if (properties & int(QLowEnergyCharacteristic::Write)) parts << QStringLiteral("Write");
    if (properties & int(QLowEnergyCharacteristic::WriteNoResponse)) parts << QStringLiteral("WriteNR");
    if (properties & int(QLowEnergyCharacteristic::Notify)) parts << QStringLiteral("Notify");
    if (properties & int(QLowEnergyCharacteristic::Indicate)) parts << QStringLiteral("Indicate");
    if (properties & int(QLowEnergyCharacteristic::Broadcasting)) parts << QStringLiteral("Broadcast");
    return parts.join(QStringLiteral(" · "));
}

void BluetoothManager::readCharacteristic(int serviceIndex, int charIndex)
{
    QLowEnergyService *service = serviceObjectAt(serviceIndex);
    if (!service)
        return;
    const QVariantMap serviceMap = m_services.at(serviceIndex).toMap();
    const QVariantList chars = serviceMap.value(QStringLiteral("characteristics")).toList();
    if (charIndex < 0 || charIndex >= chars.size())
        return;
    const QString uuidText = chars.at(charIndex).toMap().value(QStringLiteral("uuid")).toString();
    for (const QLowEnergyCharacteristic &characteristic : service->characteristics()) {
        if (characteristic.uuid().toString().compare(uuidText, Qt::CaseInsensitive) == 0) {
            if (!(int(characteristic.properties()) & int(QLowEnergyCharacteristic::Read))) {
                m_traffic->appendInfo(QStringLiteral("该特征不支持 Read"), true);
                return;
            }
            service->readCharacteristic(characteristic);
            return;
        }
    }
}

void BluetoothManager::writeCharacteristic(int serviceIndex, int charIndex, const QString &text,
                                           bool hexMode, bool withResponse)
{
    QLowEnergyService *service = serviceObjectAt(serviceIndex);
    if (!service)
        return;
    const QVariantMap serviceMap = m_services.at(serviceIndex).toMap();
    const QVariantList chars = serviceMap.value(QStringLiteral("characteristics")).toList();
    if (charIndex < 0 || charIndex >= chars.size())
        return;
    const QString uuidText = chars.at(charIndex).toMap().value(QStringLiteral("uuid")).toString();
    for (const QLowEnergyCharacteristic &characteristic : service->characteristics()) {
        if (characteristic.uuid().toString().compare(uuidText, Qt::CaseInsensitive) == 0) {
            QString error;
            const QByteArray payload = HexCodec::buildPayload(text, hexMode, true,
                                                              QStringLiteral("none"), &error);
            if (payload.isEmpty() && !error.isEmpty()) {
                emit toastRequested(error, false);
                return;
            }
            service->writeCharacteristic(characteristic, payload,
                                         withResponse ? QLowEnergyService::WriteWithResponse
                                                      : QLowEnergyService::WriteWithoutResponse);
            m_txBytes += payload.size();
            m_traffic->append(TrafficModel::Tx, payload,
                              QStringLiteral("%1 %2").arg(withResponse ? QStringLiteral("Write")
                                                                        : QStringLiteral("WriteNR"),
                                                          characteristic.uuid().toString()));
            if (g_studio)
                g_studio->addTraffic(0, payload.size(), 0);
            emit statsChanged();
            return;
        }
    }
}

void BluetoothManager::setNotify(int serviceIndex, int charIndex, bool enable, bool indicate)
{
    QLowEnergyService *service = serviceObjectAt(serviceIndex);
    if (!service)
        return;
    const QVariantMap serviceMap = m_services.at(serviceIndex).toMap();
    const QVariantList chars = serviceMap.value(QStringLiteral("characteristics")).toList();
    if (charIndex < 0 || charIndex >= chars.size())
        return;
    const QString uuidText = chars.at(charIndex).toMap().value(QStringLiteral("uuid")).toString();
    for (const QLowEnergyCharacteristic &characteristic : service->characteristics()) {
        if (characteristic.uuid().toString().compare(uuidText, Qt::CaseInsensitive) == 0) {
            QLowEnergyDescriptor cccd;
            const QBluetoothUuid cccdUuid(QBluetoothUuid::DescriptorType::ClientCharacteristicConfiguration);
            for (const QLowEnergyDescriptor &descriptor : characteristic.descriptors()) {
                if (descriptor.uuid() == cccdUuid) {
                    cccd = descriptor;
                    break;
                }
            }
            if (!cccd.isValid()) {
                m_traffic->appendInfo(QStringLiteral("该特征不存在 CCCD，无法订阅 Notify/Indicate"), true);
                return;
            }
            QByteArray value;
            if (enable)
                value = indicate ? QByteArrayLiteral("\x02\x00") : QByteArrayLiteral("\x01\x00");
            else
                value = QByteArrayLiteral("\x00\x00");
            service->writeDescriptor(cccd, value);
            m_notifyActive = enable && !indicate;
            m_traffic->appendInfo(QStringLiteral("%1 %2")
                                      .arg(enable ? (indicate ? QStringLiteral("开启 Indicate")
                                                              : QStringLiteral("开启 Notify"))
                                                   : QStringLiteral("关闭订阅"),
                                           characteristic.uuid().toString()));
            emit leStateChanged();
            return;
        }
    }
}

void BluetoothManager::enableUartMode(const QString &serviceUuid, const QString &txUuid,
                                      const QString &rxUuid)
{
    if (!m_controller || m_services.isEmpty()) {
        // 连接尚未完成时先挂起，服务发现结束后自动绑定。
        m_uartMode = true;
        m_uartPendingServiceUuid = serviceUuid;
        m_uartPendingTxUuid = txUuid;
        m_uartPendingRxUuid = rxUuid;
        return;
    }
    m_uartMode = true;
    m_uartPendingServiceUuid = serviceUuid;
    m_uartPendingTxUuid = txUuid;
    m_uartPendingRxUuid = rxUuid;

    for (int i = 0; i < m_services.size(); ++i) {
        const QVariantMap serviceMap = m_services.at(i).toMap();
        if (serviceMap.value(QStringLiteral("uuid")).toString()
                .compare(serviceUuid, Qt::CaseInsensitive) != 0)
            continue;
        QLowEnergyService *service = serviceObjectAt(i);
        if (!service)
            continue;
        const QVariantList chars = serviceMap.value(QStringLiteral("characteristics")).toList();
        for (const QVariant &entry : chars) {
            const QString uuid = entry.toMap().value(QStringLiteral("uuid")).toString();
            if (uuid.compare(txUuid, Qt::CaseInsensitive) == 0)
                m_uartTxChar = findCharacteristic(service, uuid);
            if (uuid.compare(rxUuid, Qt::CaseInsensitive) == 0)
                m_uartRxChar = findCharacteristic(service, uuid);
        }
        if (!m_uartTxChar.isValid() || !m_uartRxChar.isValid()) {
            m_traffic->appendInfo(QStringLiteral("UART 特征未找到，请核对 UUID"), true);
            return;
        }
        m_uartService = service;
        setNotify(i, charIndexInService(service, m_uartRxChar), true, false);
        m_traffic->appendInfo(QStringLiteral("BLE UART 模式已启用 · 写 %1 / 订阅 %2")
                                  .arg(m_uartTxChar.uuid().toString(),
                                       m_uartRxChar.uuid().toString()));
        return;
    }
    m_traffic->appendInfo(QStringLiteral("未找到 UART 服务 UUID: %1").arg(serviceUuid), true);
}

int BluetoothManager::charIndexInService(QLowEnergyService *service, const QLowEnergyCharacteristic &target) const
{
    const QList<QLowEnergyCharacteristic> chars = service->characteristics();
    for (int i = 0; i < chars.size(); ++i) {
        if (chars.at(i).uuid() == target.uuid())
            return i;
    }
    return -1;
}

QLowEnergyCharacteristic BluetoothManager::findCharacteristic(QLowEnergyService *service, const QString &uuid)
{
    for (const QLowEnergyCharacteristic &characteristic : service->characteristics()) {
        if (characteristic.uuid().toString().compare(uuid, Qt::CaseInsensitive) == 0)
            return characteristic;
    }
    return QLowEnergyCharacteristic();
}

void BluetoothManager::disableUartMode()
{
    m_uartMode = false;
    m_uartService = nullptr;
    m_traffic->appendInfo(QStringLiteral("BLE UART 模式已关闭"));
}

QString BluetoothManager::send(const QString &text)
{
    QString error;
    const QByteArray payload = HexCodec::buildPayload(text, m_hexSend, m_escapeEnabled,
                                                      m_lineEnding, &error);
    if (payload.isEmpty() && !error.isEmpty()) {
        emit toastRequested(error, false);
        return QString();
    }
    if (payload.isEmpty())
        return QString();

    // UART 模式：走绑定的写特征，按 MTU 自动分片。
    if (m_uartMode && m_uartService && m_uartTxChar.isValid()) {
        const int chunk = mtu() > 3 ? mtu() - 3 : 20;
        for (int offset = 0; offset < payload.size(); offset += chunk) {
            const QByteArray part = payload.mid(offset, chunk);
            m_uartService->writeCharacteristic(m_uartTxChar, part,
                                               QLowEnergyService::WriteWithoutResponse);
            m_traffic->append(TrafficModel::Tx, part, QStringLiteral("BLE UART"));
            m_txBytes += part.size();
            if (g_studio)
                g_studio->addTraffic(0, part.size(), 0);
        }
        emit statsChanged();
        return text;
    }

    // RFCOMM SPP。
    if (m_rfcommSocket && m_rfcommSocket->state() == QBluetoothSocket::SocketState::ConnectedState) {
        m_rfcommSocket->write(payload);
        m_txBytes += payload.size();
        m_traffic->append(TrafficModel::Tx, payload, QStringLiteral("RFCOMM"));
        if (g_studio)
            g_studio->addTraffic(0, payload.size(), 0);
        emit statsChanged();
        return text;
    }

    emit toastRequested(QStringLiteral("蓝牙未连接，无法发送"), false);
    return QString();
}

void BluetoothManager::startRfcommDiscovery()
{
    if (QBluetoothDeviceDiscoveryAgent::supportedDiscoveryMethods()
        == QBluetoothDeviceDiscoveryAgent::DiscoveryMethod::NoMethod) {
        m_traffic->appendInfo(QStringLiteral(
            "当前 Qt 构建为 Dummy 蓝牙后端，Classic 服务发现不可用。"), true);
        emit toastRequested(QStringLiteral("当前 Qt 构建不含 Windows 蓝牙后端，服务发现不可用"), false);
        return;
    }
    if (m_serviceAgent) {
        if (m_serviceAgent->isActive())
            return;
    } else {
        m_serviceAgent = new QBluetoothServiceDiscoveryAgent(this);
        connect(m_serviceAgent, &QBluetoothServiceDiscoveryAgent::serviceDiscovered, this,
                [this](const QBluetoothServiceInfo &info) {
                    if (info.socketProtocol() != QBluetoothServiceInfo::RfcommProtocol)
                        return;
                    QVariantMap map;
                    map.insert(QStringLiteral("device"), info.device().name());
                    map.insert(QStringLiteral("service"), info.serviceDescription());
                    map.insert(QStringLiteral("uuid"), info.serviceUuid().toString());
                    m_rfcommServices.append(map);
                    m_rfcommInfos.append(info);
                    emit rfcommChanged();
                });
        connect(m_serviceAgent, &QBluetoothServiceDiscoveryAgent::finished, this, [this] {
            m_traffic->appendInfo(QStringLiteral("RFCOMM 服务发现完成 · %1 个服务")
                                      .arg(m_rfcommServices.size()));
            emit rfcommChanged();
        });
        connect(m_serviceAgent, &QBluetoothServiceDiscoveryAgent::errorOccurred, this, [this](QBluetoothServiceDiscoveryAgent::Error error) {
            if (error != QBluetoothServiceDiscoveryAgent::NoError) {
                m_traffic->appendInfo(QStringLiteral("RFCOMM 发现错误: %1")
                                          .arg(m_serviceAgent->errorString()), true);
                emit rfcommChanged();
            }
        });
    }
    m_rfcommServices.clear();
    m_rfcommInfos.clear();
    m_serviceAgent->start(QBluetoothServiceDiscoveryAgent::FullDiscovery);
    m_traffic->appendInfo(QStringLiteral("开始 Classic 服务发现…"));
    emit rfcommChanged();
}

void BluetoothManager::connectRfcomm(int serviceIndex)
{
    if (serviceIndex < 0 || serviceIndex >= m_rfcommInfos.size())
        return;
    disconnectRfcomm();
    const QBluetoothServiceInfo info = m_rfcommInfos.at(serviceIndex);
    m_rfcommSocket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol, this);
    connect(m_rfcommSocket, &QBluetoothSocket::connected, this, [this] {
        m_traffic->appendInfo(QStringLiteral("RFCOMM 已连接"));
        if (g_studio) {
            g_studio->log(QStringLiteral("success"), QStringLiteral("RFCOMM 已连接"));
            g_studio->addTraffic(0, 0, 1);
        }
        emit rfcommChanged();
    });
    connect(m_rfcommSocket, &QBluetoothSocket::disconnected, this, [this] {
        m_traffic->appendInfo(QStringLiteral("RFCOMM 已断开"));
        if (g_studio)
            g_studio->addTraffic(0, 0, -1);
        emit rfcommChanged();
    });
    connect(m_rfcommSocket, &QBluetoothSocket::errorOccurred, this, [this](QBluetoothSocket::SocketError error) {
        if (error == QBluetoothSocket::SocketError::NoSocketError)
            return;
        m_traffic->appendInfo(QStringLiteral("RFCOMM 错误: %1").arg(m_rfcommSocket->errorString()), true);
    });
    connect(m_rfcommSocket, &QBluetoothSocket::readyRead, this, [this] {
        const QByteArray data = m_rfcommSocket->readAll();
        if (data.isEmpty())
            return;
        m_rxBytes += data.size();
        m_traffic->append(TrafficModel::Rx, data, QStringLiteral("RFCOMM"));
        if (g_studio)
            g_studio->addTraffic(data.size(), 0, 0);
        emit statsChanged();
    });
    m_rfcommSocket->connectToService(info);
}

void BluetoothManager::disconnectRfcomm()
{
    if (!m_rfcommSocket)
        return;
    m_rfcommSocket->disconnectFromService();
    m_rfcommSocket->deleteLater();
    m_rfcommSocket = nullptr;
    emit rfcommChanged();
}

bool BluetoothManager::rfcommConnected() const
{
    return m_rfcommSocket && m_rfcommSocket->state() == QBluetoothSocket::SocketState::ConnectedState;
}

bool BluetoothManager::rfcommDiscovering() const
{
    return m_serviceAgent && m_serviceAgent->isActive();
}

void BluetoothManager::setHexSend(bool enabled)
{
    if (m_hexSend == enabled)
        return;
    m_hexSend = enabled;
    emit hexSendChanged();
}

void BluetoothManager::setEscapeEnabled(bool enabled)
{
    if (m_escapeEnabled == enabled)
        return;
    m_escapeEnabled = enabled;
    emit escapeEnabledChanged();
}

void BluetoothManager::setLineEnding(const QString &ending)
{
    if (m_lineEnding == ending)
        return;
    m_lineEnding = ending;
    emit lineEndingChanged();
}
