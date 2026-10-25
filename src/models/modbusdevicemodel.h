#pragma once

#include <QAbstractListModel>
#include <QVector>
#include <QVariantList>
#include <QVariantMap>

class ModbusDeviceModel final : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Role {
        NameRole = Qt::UserRole + 1,
        UnitIdRole,
        OnlineRole,
        LabelRole,
        IdRole,
        EnabledRole
    };
    Q_ENUM(Role)

    explicit ModbusDeviceModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    int selectedIndex() const { return m_selectedIndex; }
    void setSelectedIndex(int index);
    int addDevice(const QString &name, int unitId);
    bool removeDevice(int index);
    bool setUnitId(int index, int unitId);
    int unitId(int index) const;
    Q_INVOKABLE QVariantList definitions() const;
    Q_INVOKABLE QVariantMap advancedOptions(int index) const;
    QVariantMap advancedOptionsForId(const QString &deviceId) const;
    Q_INVOKABLE bool setAdvancedOptions(int index, const QVariantMap &options);
    Q_INVOKABLE bool copyAdvancedOptions(int sourceIndex, int targetIndex);
    bool restore(const QVariantList &definitions);
    QString deviceId(int index) const;
    bool isDeviceEnabled(const QString &deviceId) const;
    Q_INVOKABLE bool setDeviceEnabled(int index, bool enabled);
    Q_INVOKABLE bool renameDevice(int index, const QString &name);
    void setOnline(const QString &id, bool online);
signals:
    void definitionsChanged();

private:
    struct Device {
        QString name;
        int unitId = 1;
        bool online = false;
        QString id;
        QVariantMap advanced;
        bool enabled = true;
    };

    QVector<Device> m_devices;
    int m_selectedIndex = 0;
};
