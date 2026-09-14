#pragma once

#include <QObject>
#include <QVariantList>

// 应用级门面：运行日志、全局收发统计、主题持久化（QSettings）。
// 各传输管理器把自己的事件/字节计数汇入此处，LogPage 与状态栏统一读取。
class Studio : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList logs READ logs NOTIFY logsChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage WRITE setStatusMessage NOTIFY statusMessageChanged)
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)
    Q_PROPERTY(bool startDark READ startDark CONSTANT)
    Q_PROPERTY(qint64 totalRx READ totalRx NOTIFY totalsChanged)
    Q_PROPERTY(qint64 totalTx READ totalTx NOTIFY totalsChanged)
    Q_PROPERTY(int activeConnections READ activeConnections NOTIFY totalsChanged)
    Q_PROPERTY(QString version READ version CONSTANT)

public:
    explicit Studio(QObject *parent = nullptr);

    QVariantList logs() const { return m_logs; }
    QString statusMessage() const { return m_statusMessage; }
    QString language() const { return m_language; }
    bool startDark() const;
    qint64 totalRx() const { return m_totalRx; }
    qint64 totalTx() const { return m_totalTx; }
    int activeConnections() const { return m_activeConnections; }
    QString version() const { return QStringLiteral("0.1.0"); }
    QString localize(const QString &english, const QString &chinese) const;

    Q_INVOKABLE void log(const QString &level, const QString &message);
    Q_INVOKABLE void clearLogs();
    Q_INVOKABLE bool saveLogs(const QUrl &path) const;
    Q_INVOKABLE void saveTheme(bool dark);
    Q_INVOKABLE void saveValue(const QString &key, const QVariant &value);
    Q_INVOKABLE QVariant loadValue(const QString &key, const QVariant &fallback = {}) const;

    void addTraffic(qint64 rx, qint64 tx, int activeConnectionsDelta);
    void setStatusMessage(const QString &message);
    void setLanguage(const QString &language);

signals:
    void logsChanged();
    void statusMessageChanged();
    void languageChanged();
    void totalsChanged();

private:
    void appendLog(const QString &time, const QString &level, const QString &message);

    QVariantList m_logs;
    QString m_statusMessage = QStringLiteral("Ready");
    QString m_language = QStringLiteral("en");
    qint64 m_totalRx = 0;
    qint64 m_totalTx = 0;
    int m_activeConnections = 0;
};
