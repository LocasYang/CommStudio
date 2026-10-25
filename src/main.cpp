#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QQuickWindow>
#include <QQuickItem>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QSGRendererInterface>
#include <QSurfaceFormat>
#include <QTimer>

#include "managers/modbusmanager.h"
#include "managers/modbusworkspace.h"
#include "managers/bluetoothmanager.h"
#include "managers/networkmanager.h"
#include "managers/serialmanager.h"
#include "managers/studio.h"
#include "managers/toolboxmanager.h"
#include "windowchrome.h"

#include <QFile>
#include <QTextStream>
#include <QSettings>

namespace {

// GUI 子系统没有控制台：设置 COMMSTUDIO_MSG_LOG 时把 Qt 消息落盘便于诊断。
void fileMessageHandler(QtMsgType type, const QMessageLogContext &context,
                        const QString &message)
{
    const QString path = qEnvironmentVariable("COMMSTUDIO_MSG_LOG");
    if (path.isEmpty())
        return;
    QFile file(path);
    if (!file.open(QIODevice::Append | QIODevice::Text))
        return;
    const char *level = type == QtFatalMsg ? "FATAL"
                        : type == QtCriticalMsg ? "CRITICAL"
                        : type == QtWarningMsg ? "WARN"
                        : type == QtInfoMsg ? "INFO" : "DEBUG";
    QTextStream(&file) << level << ": " << message
                       << (context.file ? QStringLiteral(" (%1:%2)")
                                              .arg(context.file).arg(context.line)
                                         : QString())
                       << '\n';
}

} // namespace

int main(int argc, char *argv[])
{
    if (!qEnvironmentVariableIsEmpty("COMMSTUDIO_MSG_LOG"))
        qInstallMessageHandler(fileMessageHandler);

    if (qEnvironmentVariableIsEmpty("QSG_RENDER_LOOP"))
        qputenv("QSG_RENDER_LOOP", QByteArrayLiteral("threaded"));

    QSurfaceFormat surfaceFormat = QSurfaceFormat::defaultFormat();
    surfaceFormat.setSamples(4);
    QSurfaceFormat::setDefaultFormat(surfaceFormat);
    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);

    QGuiApplication app(argc, argv);
    QGuiApplication::setOrganizationName(QStringLiteral("TBEA"));
    QGuiApplication::setApplicationName(QStringLiteral("CommStudio"));
    const QString profileDirectory = qEnvironmentVariable("COMMSTUDIO_PROFILE_DIR");
    if (!profileDirectory.isEmpty()) {
        QSettings::setDefaultFormat(QSettings::IniFormat);
        QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, profileDirectory);
    }
    if (!profileDirectory.isEmpty() && qEnvironmentVariableIsSet("COMMSTUDIO_SCREENSHOT")) {
        QSettings settings;
        settings.setValue("ui/language", qEnvironmentVariableIsSet("COMMSTUDIO_SCREENSHOT_ENGLISH") ? "en-US" : "zh-CN");
        settings.setValue("theme/dark-v2", !qEnvironmentVariableIsSet("COMMSTUDIO_SCREENSHOT_LIGHT"));
    }
    QGuiApplication::setApplicationDisplayName(QStringLiteral("CommStudio"));
    QGuiApplication::setWindowIcon(
        QIcon(QStringLiteral(":/qt/qml/CommStudio/qml/assets/commstudio-mark.svg")));

    QQuickStyle::setStyle(QStringLiteral("Basic"));

    Studio *studio = new Studio(&app);
    qmlRegisterSingletonInstance<Studio>("CommStudio.Backend", 1, 0, "Studio", studio);
    auto *serialManager = new SerialManager(studio);
    qmlRegisterSingletonInstance<SerialManager>("CommStudio.Backend", 1, 0, "SerialManager", serialManager);
    // Offline visual regression only: replay captured bytes into the display
    // model, never through send()/openConnection() or a real serial port.
    if (!profileDirectory.isEmpty() && qEnvironmentVariableIsSet("COMMSTUDIO_SCREENSHOT")) {
        QFile fixture(qEnvironmentVariable("COMMSTUDIO_SCREENSHOT_SERIAL_TRAFFIC"));
        if (!fixture.fileName().isEmpty() && fixture.open(QIODevice::ReadOnly)) {
            serialManager->traffic()->clear();
            serialManager->traffic()->appendInfo(studio->localize(QStringLiteral("Offline display replay · no hardware connection"),
                                                                  QStringLiteral("离线显示回放 · 未连接硬件")));
            for (const QJsonValue &value : QJsonDocument::fromJson(fixture.readAll()).array()) {
                const QJsonObject row = value.toObject();
                serialManager->traffic()->append(row.value("tx").toBool() ? TrafficModel::Tx : TrafficModel::Rx,
                                                QByteArray::fromHex(row.value("hex").toString().toLatin1()),
                                                QStringLiteral("REPLAY"));
                serialManager->traffic()->flushPending();
            }
        }
        if (qEnvironmentVariableIsSet("COMMSTUDIO_SCREENSHOT_SERIAL_ENDING"))
            serialManager->setLineEnding(qEnvironmentVariable("COMMSTUDIO_SCREENSHOT_SERIAL_ENDING"));
    }
    auto *modbusWorkspace = new ModbusWorkspace(studio);
    if (!profileDirectory.isEmpty() && qEnvironmentVariableIsSet("COMMSTUDIO_SCREENSHOT_TEMPLATE"))
        modbusWorkspace->current()->points()->importFile(QUrl::fromLocalFile(qEnvironmentVariable("COMMSTUDIO_SCREENSHOT_TEMPLATE")));
    qmlRegisterSingletonInstance<ModbusWorkspace>("CommStudio.Backend", 1, 0, "ModbusWorkspace", modbusWorkspace);
    qmlRegisterSingletonInstance<ModbusManager>("CommStudio.Backend", 1, 0, "ModbusManager", modbusWorkspace->legacy());
    qmlRegisterSingletonInstance<BluetoothManager>("CommStudio.Backend", 1, 0, "BluetoothManager",
                                                   new BluetoothManager(studio));
    qmlRegisterSingletonInstance<NetworkManager>("CommStudio.Backend", 1, 0, "NetworkManager",
                                                 new NetworkManager(studio));
    qmlRegisterSingletonInstance<ToolboxManager>("CommStudio.Backend", 1, 0, "ToolboxManager",
                                                 new ToolboxManager(&app));
    qmlRegisterSingletonInstance<WindowChrome>("CommStudio.Backend", 1, 0, "WindowChrome",
                                               new WindowChrome(&app));

    QQmlApplicationEngine engine;
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed, &app,
                     [] { QCoreApplication::exit(-1); }, Qt::QueuedConnection);
    engine.loadFromModule("CommStudio", "Main");

    const QByteArray screenshotPath = qgetenv("COMMSTUDIO_SCREENSHOT");
    if (!screenshotPath.isEmpty()) {
        const QByteArray screenshotSize = qgetenv("COMMSTUDIO_SCREENSHOT_SIZE");
        if (!engine.rootObjects().isEmpty()) {
            if (auto *window = qobject_cast<QQuickWindow *>(engine.rootObjects().constFirst())) {
                bool pageOk = false;
                const int page = qgetenv("COMMSTUDIO_SCREENSHOT_PAGE").toInt(&pageOk);
                if (pageOk && page >= 0 && page <= 6)
                    window->setProperty("currentPage", page);
                if (qEnvironmentVariableIsSet("COMMSTUDIO_SCREENSHOT_NAV_EXPANDED"))
                    window->setProperty("navigationCollapsed", false);
                if (qEnvironmentVariableIsSet("COMMSTUDIO_SCREENSHOT_NAV_TOGGLE"))
                    QMetaObject::invokeMethod(window, "toggleNavigation");
                if (auto *terminal = window->findChild<QObject *>("serialTerminal")) {
                    if (qEnvironmentVariableIsSet("COMMSTUDIO_SCREENSHOT_SERIAL_HEX"))
                        terminal->setProperty("displayMode", 2);
                    if (qEnvironmentVariableIsSet("COMMSTUDIO_SCREENSHOT_SERIAL_SHOW_TX"))
                        terminal->setProperty("showSentMessages", true);
                }
                if (!profileDirectory.isEmpty() && qEnvironmentVariableIsSet("COMMSTUDIO_SCREENSHOT_SERIAL_TRAFFIC")) {
                    if (auto *input = window->findChild<QObject *>("sendInput"))
                        input->setProperty("text", QStringLiteral("AT"));
                }
                if (qEnvironmentVariableIsSet("COMMSTUDIO_SCREENSHOT_ENDING_MENU")) {
                    QTimer::singleShot(500, window, [window] {
                        if (auto *combo = window->findChild<QObject *>("sendLineEndingCombo")) {
                            if (auto *popup = combo->property("popup").value<QObject *>())
                                QMetaObject::invokeMethod(popup, "open");
                        }
                    });
                }
                if (auto *monitor = window->findChild<QObject *>("modbusDataMonitor")) {
                    if (qEnvironmentVariableIsSet("COMMSTUDIO_SCREENSHOT_TREE_DETAILS"))
                        monitor->setProperty("showDeviceDetails", true);
                    if (qEnvironmentVariableIsSet("COMMSTUDIO_SCREENSHOT_TRAFFIC_HIDDEN"))
                        monitor->setProperty("trafficVisible", false);
                }
                if (qEnvironmentVariableIsSet("COMMSTUDIO_SCREENSHOT_TRAFFIC_EXPANDED")) {
                    if (auto *dock = window->findChild<QObject *>("modbusTrafficDock"))
                        QMetaObject::invokeMethod(dock, "inspectExpanded");
                }
                if (auto *modbusPage = window->findChild<QObject *>("modbusPage")) {
                    if (qEnvironmentVariableIsSet("COMMSTUDIO_SCREENSHOT_POINT_EDITOR"))
                        QMetaObject::invokeMethod(modbusPage, "inspectNewPoint");
                    if (qEnvironmentVariableIsSet("COMMSTUDIO_SCREENSHOT_CHANNEL_EDITOR"))
                        QMetaObject::invokeMethod(modbusPage, "inspectNewChannel");
                    if (qEnvironmentVariableIsSet("COMMSTUDIO_SCREENSHOT_REMOVE_CHANNEL"))
                        QMetaObject::invokeMethod(modbusPage, "inspectRemoveChannel");
                    if (qEnvironmentVariableIsSet("COMMSTUDIO_SCREENSHOT_CHANNEL_MANAGER"))
                        QMetaObject::invokeMethod(modbusPage, "inspectChannelManager");
                    if (qEnvironmentVariableIsSet("COMMSTUDIO_SCREENSHOT_CHANNEL_CONFIG"))
                        QMetaObject::invokeMethod(modbusPage, "inspectChannelConfiguration");
                    if (qEnvironmentVariableIsSet("COMMSTUDIO_SCREENSHOT_DEVICE_ADVANCED"))
                        QMetaObject::invokeMethod(modbusPage, "inspectDeviceAdvancedParameters");
                    if (qEnvironmentVariableIsSet("COMMSTUDIO_SCREENSHOT_STARTUP_WRITE"))
                        QTimer::singleShot(300, modbusPage, [modbusPage] {
                            QMetaObject::invokeMethod(modbusPage, "inspectStartupWriteParameters");
                        });
                    if (qEnvironmentVariableIsSet("COMMSTUDIO_SCREENSHOT_BATCH_QUANTITY"))
                        QMetaObject::invokeMethod(modbusPage, "inspectBatchQuantity");
                    if (qEnvironmentVariableIsSet("COMMSTUDIO_SCREENSHOT_FILTER_ROWS"))
                        QMetaObject::invokeMethod(modbusPage, "inspectRowsFilter");
                    if (qEnvironmentVariableIsSet("COMMSTUDIO_SCREENSHOT_FILTER_COLUMNS"))
                        QMetaObject::invokeMethod(modbusPage, "inspectColumnsFilter");
                    if (qEnvironmentVariableIsSet("COMMSTUDIO_SCREENSHOT_BATCH_WRITE"))
                        QMetaObject::invokeMethod(modbusPage, "inspectBatchWriteDetails");
                    if (qEnvironmentVariableIsSet("COMMSTUDIO_SCREENSHOT_COPY_CONFIGURATION"))
                        QMetaObject::invokeMethod(modbusPage, "inspectCopyConfiguration");
                    if (qEnvironmentVariableIsSet("COMMSTUDIO_SCREENSHOT_TEMPLATE_EXPORT"))
                        QMetaObject::invokeMethod(modbusPage, "inspectTemplateExport");
                    if (qEnvironmentVariableIsSet("COMMSTUDIO_SCREENSHOT_STATISTICS"))
                        QMetaObject::invokeMethod(modbusPage, "inspectStatistics");
                }
                const QList<QByteArray> parts = screenshotSize.toLower().split('x');
                if (parts.size() == 2) {
                    bool widthOk = false;
                    bool heightOk = false;
                    const int width = parts.at(0).toInt(&widthOk);
                    const int height = parts.at(1).toInt(&heightOk);
                    if (widthOk && heightOk)
                        window->resize(width, height);
                }
            }
        }
        bool delayOk = false;
        const int configuredDelay = qgetenv("COMMSTUDIO_SCREENSHOT_DELAY").toInt(&delayOk);
        const int screenshotDelay = delayOk ? qMax(250, configuredDelay) : 1500;
        QTimer::singleShot(screenshotDelay, &app, [&app, &engine, screenshotPath, modbusWorkspace] {
            if (!engine.rootObjects().isEmpty()) {
                if (auto *window = qobject_cast<QQuickWindow *>(engine.rootObjects().constFirst())) {
                    window->grabWindow().save(QString::fromLocal8Bit(screenshotPath));
                    // Optional headless layout evidence, confined to screenshot runs.
                    if (qEnvironmentVariableIsSet("COMMSTUDIO_SCREENSHOT_METRICS")) {
                        QJsonObject metrics{{"navigationCollapsed", window->property("navigationCollapsed").toBool()},
                                            {"width", window->width()}, {"height", window->height()},
                                            {"channelOpen", modbusWorkspace->current()->isOpen()},
                                            {"polling", modbusWorkspace->current()->polling()},
                                            {"points", modbusWorkspace->current()->points()->pointCount()}};
                        for (const char *name : {"navigationRail", "modbusTreePanel", "modbusPointPanel",
                                                 "modbusPointToolbar", "modbusPointDetailsToggle",
                                                 "modbusTablePane", "modbusTrafficDock"}) {
                            if (auto *item = window->findChild<QQuickItem *>(QString::fromLatin1(name))) {
                                const QPointF position = item->mapToScene(QPointF());
                                metrics.insert(QString::fromLatin1(name), QJsonObject{
                                    {"x", position.x()}, {"y", position.y()}, {"width", item->width()},
                                    {"height", item->height()}, {"visible", item->isVisible()}});
                            }
                        }
                        QFile evidence(QString::fromLocal8Bit(screenshotPath) + QStringLiteral(".json"));
                        if (evidence.open(QIODevice::WriteOnly))
                            evidence.write(QJsonDocument(metrics).toJson());
                    }
                }
            }
            app.quit();
        });
    }

    return app.exec();
}
