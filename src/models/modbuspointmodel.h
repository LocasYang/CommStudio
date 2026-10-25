#pragma once
#include <QAbstractTableModel>
#include <QVariantMap>
#include <QVector>
#include <QUrl>
#include <QSortFilterProxyModel>
#include <QSet>
#include <QPointF>
#include <QStringList>

// Definitions are persistent; samples and timestamps are session-only.
class ModbusPointModel final : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(QString deviceId READ deviceId WRITE setDeviceId NOTIFY deviceIdChanged)
    Q_PROPERTY(QAbstractItemModel *filtered READ filtered CONSTANT)
    Q_PROPERTY(QString search READ search WRITE setSearch NOTIFY filterChanged)
    Q_PROPERTY(int area READ area WRITE setArea NOTIFY filterChanged)
    Q_PROPERTY(int areaMask READ areaMask WRITE setAreaMask NOTIFY filterChanged)
    Q_PROPERTY(int rowFilterMode READ rowFilterMode WRITE setRowFilterMode NOTIFY filterChanged)
    Q_PROPERTY(int count READ pointCount NOTIFY definitionsChanged)
    Q_PROPERTY(int selectedCount READ selectedCount NOTIFY selectionChanged)
    Q_PROPERTY(QString groupFilter READ groupFilter WRITE setGroupFilter NOTIFY filterChanged)
    Q_PROPERTY(QVariantList groups READ groups NOTIFY groupsChanged)
public:
    enum Role { PointRole = Qt::UserRole + 1 };
    explicit ModbusPointModel(QObject *parent = nullptr);
    ~ModbusPointModel() override;
    int rowCount(const QModelIndex &parent = {}) const override;
    int columnCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    QString deviceId() const { return m_deviceId; }
    void setDeviceId(const QString &id);
    QAbstractItemModel *filtered() const { return m_filter; }
    QString search() const { return m_search; }
    int area() const { return m_area; }
    int areaMask() const { return m_areaMask; }
    int rowFilterMode() const { return m_rowFilterMode; }
    int pointCount() const { return rowCount(); }
    int selectedCount() const { return m_selected.size(); }
    QString groupFilter() const { return m_groupFilter; }
    QVariantList groups() const;
    void setSearch(const QString &search);
    void setArea(int area);
    void setAreaMask(int mask);
    void setRowFilterMode(int mode);
    void setGroupFilter(const QString &group);
    Q_INVOKABLE QVariantMap pointAt(int row) const;
    Q_INVOKABLE bool savePoint(int row, const QVariantMap &point);
    Q_INVOKABLE int suggestedAddress(int functionCode = 3) const;
    Q_INVOKABLE bool addPoints(int count, int functionCode, int startAddress);
    Q_INVOKABLE int findNext(const QString &name, int afterProxyRow = -1) const;
    Q_INVOKABLE void removePoint(int row);
    Q_INVOKABLE void copyPoint(int row);
    Q_INVOKABLE bool exportFile(const QUrl &url, const QVariantMap &metadata = {}) const;
    Q_INVOKABLE bool importFile(const QUrl &url);
    Q_INVOKABLE QVariantList pollingPlans(int strategy = 0, int maxGap = 0, int maxRegisters = 125) const;
    QVariantList pollingPlansForDevice(const QString &deviceId, int strategy, int maxBitGap,
                                      int maxRegisterGap, int maxBits, int maxRegisters) const;
    Q_INVOKABLE bool pasteClipboard();
    Q_INVOKABLE void selectPoint(int row, bool additive, bool range);
    Q_INVOKABLE void toggleSelectAll();
    Q_INVOKABLE int selectedQuantityLimit() const;
    Q_INVOKABLE bool updateSelected(const QVariantMap &changes);
    Q_INVOKABLE bool updateSelectedField(const QString &field, const QString &text);
    Q_INVOKABLE bool pasteCells(int sourceRow, const QStringList &fields, const QString &text);
    Q_INVOKABLE bool pasteCellsFromClipboard(int sourceRow, const QStringList &fields);
    Q_INVOKABLE bool setWriteValue(int row, const QString &value);
    Q_INVOKABLE bool togglePointPolling(int row);
    Q_INVOKABLE bool togglePointBatchWrite(int row);
    Q_INVOKABLE bool setVisiblePointIds(const QStringList &visibleIds);
    Q_INVOKABLE void deleteSelected();
    Q_INVOKABLE void copySelectedToDevice(const QString &deviceId);
    Q_INVOKABLE bool copyCurrentDeviceToDevice(const QString &deviceId);
    bool replaceDevicePoints(const QString &deviceId, const QVariantList &sourcePoints);
    Q_INVOKABLE QVariantMap parsePdu(int startAddress, const QString &pduText) const;
    Q_INVOKABLE void movePoint(int from, int to);
    Q_INVOKABLE void sortByAddress();
    Q_INVOKABLE QVariantList trendSeries(int windowMs = 60000) const;
    Q_INVOKABLE bool exportHistory(const QUrl &url) const;
    Q_INVOKABLE QVariantList definitions() const;
    bool restore(const QVariantList &points);
    bool hasDevice(const QString &id) const;
    bool removeDevice(const QString &id);
    void receive(const QString &device, int fc, int start, const QVector<quint16> &values);
signals:
    void deviceIdChanged();
    void definitionsChanged();
    void validationError(const QString &message);
    void filterChanged();
    void groupsChanged();
    void selectionChanged();
private:
    static bool normalize(QVariantMap &point);
    bool appendDefinitions(const QVariantList &definitions);
    QList<int> visibleRows() const;
    QVector<QVariantMap> m_points;
    QHash<QString, QVariantMap> m_samples;
    QString m_deviceId;
    QSortFilterProxyModel *m_filter;
    QString m_search;
    int m_area = 0;
    int m_areaMask = 0x0F;
    int m_rowFilterMode = 0;
    QString m_groupFilter;
    QSet<QString> m_selected;
    int m_selectionAnchor = -1;
    QHash<QString, QList<QPointF>> m_history;
};

