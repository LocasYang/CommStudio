#pragma once

#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothLocalDevice>
#include <QBluetoothServiceDiscoveryAgent>
#include <QBluetoothSocket>
#include <QLowEnergyController>
#include <QLowEnergyService>
#include <QTimer>
#include <QUrl>
#include <QVariantList>

#include "core/trafficmodel.h"

// 蓝牙助手后端：
//  - 本地适配器状态；
//  - BLE / Classic 扫描（RSSI 实时合并更新）；
//  - BLE GATT Client：服务发现、特征读写、Notify/Indicate 订阅、MTU 显示；
//  - BLE UART 模板（Nordic UART Service 等自定义 UUID 直连终端）；
//  - Classic RFCOMM(SPP) 服务发现与串口式终端。
class BluetoothManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(TrafficModel *traffic READ traffic CONSTANT)
    Q_PROPERTY(QVariantList adapters READ adapters NOTIFY adaptersChanged)
    Q_PROPERTY(bool backendSupported READ backendSupported CONSTANT)
    Q_PROPERTY(bool scanning READ scanning NOTIFY scanningChanged)
    Q_PROPERTY(QVariantList devices READ devices NOTIFY devicesChanged)
    Q_PROPERTY(QString nameFilter READ nameFilter WRITE setNameFilter NOTIFY nameFilterChanged)
    Q_PROPERTY(bool leOnly READ leOnly WRITE setLeOnly NOTIFY leOnlyChanged)
    Q_PROPERTY(QString leState READ leState NOTIFY leStateChanged)
    Q_PROPERTY(QString connectedDeviceName READ connectedDeviceName NOTIFY leStateChanged)
    Q_PROPERTY(int mtu READ mtu NOTIFY leStateChanged)
    Q_PROPERTY(int rssi READ rssi NOTIFY leStateChanged)
    Q_PROPERTY(QVariantList services READ services NOTIFY servicesChanged)
    Q_PROPERTY(int selectedService READ selectedService WRITE setSelectedService NOTIFY selectedServiceChanged)
    // RFCOMM
    Q_PROPERTY(bool rfcommConnected READ rfcommConnected NOTIFY rfcommChanged)
    Q_PROPERTY(bool rfcommDiscovering READ rfcommDiscovering NOTIFY rfcommChanged)
    Q_PROPERTY(QVariantList rfcommServices READ rfcommServices NOTIFY rfcommChanged)
    Q_PROPERTY(bool hexSend READ hexSend WRITE setHexSend NOTIFY hexSendChanged)
    Q_PROPERTY(bool escapeEnabled READ escapeEnabled WRITE setEscapeEnabled NOTIFY escapeEnabledChanged)
    Q_PROPERTY(QString lineEnding READ lineEnding WRITE setLineEnding NOTIFY lineEndingChanged)
    Q_PROPERTY(qint64 rxBytes READ rxBytes NOTIFY statsChanged)
    Q_PROPERTY(qint64 txBytes READ txBytes NOTIFY statsChanged)
    Q_PROPERTY(bool notifyActive READ notifyActive NOTIFY leStateChanged)

public:
    explicit BluetoothManager(QObject *parent = nullptr);

    TrafficModel *traffic() const { return m_traffic; }
    QVariantList adapters() const;
    bool backendSupported() const;
    bool scanning() const;
    QVariantList devices() const { return m_devices; }
    QString nameFilter() const { return m_nameFilter; }
    void setNameFilter(const QString &filter);
    bool leOnly() const { return m_leOnly; }
    void setLeOnly(bool enabled);
    QString leState() const;
    QString connectedDeviceName() const { return m_connectedName; }
    int mtu() const;
    int rssi() const { return m_connectedRssi; }
    QVariantList services() const { return m_services; }
    int selectedService() const { return m_selectedService; }
    void setSelectedService(int index);
    bool rfcommConnected() const;
    bool rfcommDiscovering() const;
    QVariantList rfcommServices() const { return m_rfcommServices; }
    bool hexSend() const { return m_hexSend; }
    void setHexSend(bool enabled);
    bool escapeEnabled() const { return m_escapeEnabled; }
    void setEscapeEnabled(bool enabled);
    QString lineEnding() const { return m_lineEnding; }
    void setLineEnding(const QString &ending);
    qint64 rxBytes() const { return m_rxBytes; }
    qint64 txBytes() const { return m_txBytes; }
    bool notifyActive() const { return m_notifyActive; }

    Q_INVOKABLE void refreshAdapters();
    Q_INVOKABLE void startScan();
    Q_INVOKABLE void stopScan();
    Q_INVOKABLE void clearDevices();
    Q_INVOKABLE void connectDevice(int index);
    Q_INVOKABLE void disconnectDevice();
    Q_INVOKABLE void readCharacteristic(int serviceIndex, int charIndex);
    Q_INVOKABLE void writeCharacteristic(int serviceIndex, int charIndex, const QString &text,
                                         bool hexMode, bool withResponse);
    Q_INVOKABLE void setNotify(int serviceIndex, int charIndex, bool enable, bool indicate);
    // BLE UART 直连模式：connectDevice 后调用，绑定写特征与 Notify 特征。
    Q_INVOKABLE void enableUartMode(const QString &serviceUuid, const QString &txUuid,
                                    const QString &rxUuid);
    Q_INVOKABLE void disableUartMode();
    Q_INVOKABLE QString send(const QString &text);
    Q_INVOKABLE void startRfcommDiscovery();
    Q_INVOKABLE void connectRfcomm(int serviceIndex);
    Q_INVOKABLE void disconnectRfcomm();

signals:
    void adaptersChanged();
    void scanningChanged();
    void devicesChanged();
    void nameFilterChanged();
    void leOnlyChanged();
    void leStateChanged();
    void servicesChanged();
    void selectedServiceChanged();
    void rfcommChanged();
    void hexSendChanged();
    void escapeEnabledChanged();
    void lineEndingChanged();
    void statsChanged();
    void toastRequested(const QString &text, bool ok);

private:
    struct DeviceRow {
        QBluetoothDeviceInfo info;
        QString key;
        QString name;
        QString type;
        qint16 rssi = -127;
        bool hasRssi = false;
        QStringList serviceUuids;
        QString manufacturer;
    };

    void requestPermission();
    void rebuildDeviceList();
    void addDiscoveredDevice(const QBluetoothDeviceInfo &info);
    void connectLeController(const QBluetoothDeviceInfo &info);
    void discoverServices();
    void rebuildServiceList();
    QLowEnergyService *serviceObjectAt(int index) const;
    static QString uuidName(const QBluetoothUuid &uuid);
    static QString propertyText(const QLowEnergyCharacteristic &characteristic);
    static QLowEnergyCharacteristic findCharacteristic(QLowEnergyService *service, const QString &uuid);
    int charIndexInService(QLowEnergyService *service, const QLowEnergyCharacteristic &target) const;

    TrafficModel *m_traffic = nullptr;
    QBluetoothDeviceDiscoveryAgent *m_discoveryAgent = nullptr;
    QBluetoothServiceDiscoveryAgent *m_serviceAgent = nullptr;
    QBluetoothLocalDevice *m_localDevice = nullptr;

    QVector<DeviceRow> m_deviceRows;
    QVariantList m_devices;
    QString m_nameFilter;
    bool m_leOnly = true;

    QLowEnergyController *m_controller = nullptr;
    QVector<QLowEnergyService *> m_serviceObjects;
    QVariantList m_services;
    int m_selectedService = 0;
    QString m_connectedName;
    QString m_connectedKey;
    qint16 m_connectedRssi = -127;
    bool m_notifyActive = false;

    // UART 模式
    bool m_uartMode = false;
    QLowEnergyService *m_uartService = nullptr;
    QLowEnergyCharacteristic m_uartTxChar;
    QLowEnergyCharacteristic m_uartRxChar;
    QString m_uartPendingServiceUuid;
    QString m_uartPendingTxUuid;
    QString m_uartPendingRxUuid;

    // RFCOMM
    QBluetoothSocket *m_rfcommSocket = nullptr;
    QVariantList m_rfcommServices;
    QVector<QBluetoothServiceInfo> m_rfcommInfos;

    bool m_hexSend = false;
    bool m_escapeEnabled = true;
    QString m_lineEnding = QStringLiteral("none");
    qint64 m_rxBytes = 0;
    qint64 m_txBytes = 0;
};
