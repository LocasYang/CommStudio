#pragma once
#include <QObject>
#include <QVariantList>
#include "modbusmanager.h"

class ModbusWorkspace final : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ModbusManager *current READ current NOTIFY currentChanged)
    Q_PROPERTY(int currentIndex READ currentIndex NOTIFY currentChanged)
    Q_PROPERTY(QVariantList channels READ channels NOTIFY channelsChanged)
public:
    explicit ModbusWorkspace(QObject *parent = nullptr);
    ModbusManager *current() const { return m_channels.at(m_current).manager; }
    ModbusManager *legacy() const { return m_channels.first().manager; }
    int currentIndex() const { return m_current; }
    QVariantList channels() const;
    Q_INVOKABLE QObject *channelBackend(int index) const;
    Q_INVOKABLE bool copyDevicePoints(int sourceChannel, int sourceDevice,
                                     int targetChannel, int targetDevice);
    Q_INVOKABLE void select(int index);
    Q_INVOKABLE void add(const QString &name);
    Q_INVOKABLE void addChannel(const QString &name, const QString &mode);
    Q_INVOKABLE void remove(int index);
    Q_INVOKABLE void rename(int index, const QString &name);
    Q_INVOKABLE void setAlias(int index, const QString &alias);
    Q_INVOKABLE void openChannel(int index);
    Q_INVOKABLE void closeChannel(int index);
    Q_INVOKABLE void refreshPorts();
    Q_INVOKABLE void closeAll();
signals:
    void currentChanged();
    void channelsChanged();
    void toastRequested(const QString &message, bool ok);
private:
    struct Channel { QString id; QString name; ModbusManager *manager; };
    QList<Channel> m_channels;
    int m_current = 0;
    void append(const QString &id, const QString &name);
    void syncSerialPorts();
    void save();
};
