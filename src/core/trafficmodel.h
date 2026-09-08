#pragma once

#include <QAbstractListModel>
#include <QByteArray>
#include <QTimer>
#include <QVector>

// 统一收发流量模型：所有调试助手（串口 / Modbus / 蓝牙 / 网络）的接收区
// 都绑定一个 TrafficModel 实例。行数据先进入 pending 缓冲，由 30 ms 定时器
// 批量刷入视图，避免高速数据流下逐行刷新拖垮 UI（设计手册 2.3 / 6.9）。
// 模型只保留最近 maxRows 行（环形缓冲），全量数据可经 exportToFile 落盘。
class TrafficModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(bool paused READ paused WRITE setPaused NOTIFY pausedChanged)
    Q_PROPERTY(int maxRows READ maxRows WRITE setMaxRows NOTIFY maxRowsChanged)
    Q_PROPERTY(qint64 maxBytes READ maxBytes WRITE setMaxBytes NOTIFY maxBytesChanged)

public:
    enum Direction {
        Rx,
        Tx,
        Info,
        Error
    };
    Q_ENUM(Direction)

    enum Roles {
        TimeRole = Qt::UserRole + 1,
        DirRole,
        DataHexRole,
        DataTextRole,
        PeerRole,
        SizeRole,
        PlainRole
    };

    explicit TrafficModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    int count() const { return m_rows.size(); }
    bool paused() const { return m_paused; }
    void setPaused(bool paused);
    int maxRows() const { return m_maxRows; }
    void setMaxRows(int rows);
    qint64 maxBytes() const { return m_maxBytes; }
    void setMaxBytes(qint64 bytes);

    Q_INVOKABLE void clear();

    // 二进制收发行：同时生成 HEX 与 ASCII 两栏文本，由视图按显示模式取用。
    Q_INVOKABLE void append(int direction, const QByteArray &payload, const QString &peer = QString());
    // 纯文本信息行（连接成功、异常提示等），按 UTF-8 展示。
    Q_INVOKABLE void appendInfo(const QString &text, bool isError = false);
    Q_INVOKABLE QString rowText(int row) const;
    Q_INVOKABLE bool exportToFile(const QUrl &path) const;

    void flushPending();

signals:
    void countChanged();
    void pausedChanged();
    void maxRowsChanged();
    void maxBytesChanged();

private:
    struct Row {
        qint64 timestampMs = 0;
        int direction = Rx;
        bool plain = false;
        QByteArray payload;
        QString peer;
    };

    static QString formatTime(qint64 timestampMs);
    static QString toHexText(const QByteArray &payload);
    static QString toAsciiText(const QByteArray &payload);

    QVector<Row> m_rows;      // 已发布行（仅保留 maxRows）
    QVector<Row> m_pending;   // 待刷新行
    bool m_paused = false;
    int m_maxRows = 10000;
    qint64 m_maxBytes = 32 * 1024 * 1024;
    qint64 m_pendingBytes = 0;
    qint64 m_rowBytes = 0;
    QTimer m_flushTimer;
};
