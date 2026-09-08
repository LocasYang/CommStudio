#include "studio.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QFile>
#include <QSettings>
#include <QTextStream>
#include <QUrl>

Studio::Studio(QObject *parent)
    : QObject(parent)
{
    appendLog(QDateTime::currentDateTime().toString(QStringLiteral("HH:mm:ss")),
              QStringLiteral("info"), QStringLiteral("CommStudio 已启动"));
}

bool Studio::startDark() const
{
    QSettings settings;
    // The serial workbench was redesigned around the terminal-oriented
    // TuiSerial workflow. A versioned key gives existing installations the
    // new professional default once, while subsequent user choices persist.
    return settings.value(QStringLiteral("theme/dark-v2"), true).toBool();
}

void Studio::log(const QString &level, const QString &message)
{
    appendLog(QDateTime::currentDateTime().toString(QStringLiteral("HH:mm:ss")), level, message);
}

void Studio::appendLog(const QString &time, const QString &level, const QString &message)
{
    m_logs.append(QVariantMap{
        { QStringLiteral("time"), time },
        { QStringLiteral("level"), level },
        { QStringLiteral("message"), message }
    });
    // 运行日志同样只保留最近 2000 条，防止长时间运行内存增长。
    while (m_logs.size() > 2000)
        m_logs.removeFirst();
    emit logsChanged();
}

void Studio::clearLogs()
{
    m_logs.clear();
    emit logsChanged();
}

bool Studio::saveLogs(const QUrl &path) const
{
    const QString local = path.toLocalFile();
    if (local.isEmpty())
        return false;
    QFile file(local);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    for (const QVariant &entry : m_logs) {
        const QVariantMap row = entry.toMap();
        stream << row.value(QStringLiteral("time")).toString() << '\t'
               << row.value(QStringLiteral("level")).toString() << '\t'
               << row.value(QStringLiteral("message")).toString() << '\n';
    }
    return true;
}

void Studio::saveTheme(bool dark)
{
    QSettings settings;
    settings.setValue(QStringLiteral("theme/dark-v2"), dark);
}

void Studio::saveValue(const QString &key, const QVariant &value)
{
    QSettings settings;
    settings.setValue(key, value);
}

QVariant Studio::loadValue(const QString &key, const QVariant &fallback) const
{
    QSettings settings;
    return settings.value(key, fallback);
}

void Studio::addTraffic(qint64 rx, qint64 tx, int activeConnectionsDelta)
{
    if (rx == 0 && tx == 0 && activeConnectionsDelta == 0)
        return;
    m_totalRx += rx;
    m_totalTx += tx;
    m_activeConnections = qMax(0, m_activeConnections + activeConnectionsDelta);
    emit totalsChanged();
}

void Studio::setStatusMessage(const QString &message)
{
    if (m_statusMessage == message)
        return;
    m_statusMessage = message;
    emit statusMessageChanged();
}
