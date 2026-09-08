#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QQuickWindow>
#include <QSGRendererInterface>
#include <QSurfaceFormat>
#include <QTimer>

#include "managers/modbusmanager.h"
#include "managers/bluetoothmanager.h"
#include "managers/networkmanager.h"
#include "managers/serialmanager.h"
#include "managers/studio.h"
#include "managers/toolboxmanager.h"
#include "windowchrome.h"

#include <QFile>
#include <QTextStream>

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
    QGuiApplication::setApplicationDisplayName(QStringLiteral("CommStudio"));
    QGuiApplication::setWindowIcon(
        QIcon(QStringLiteral(":/qt/qml/CommStudio/qml/assets/commstudio-mark.svg")));

    QQuickStyle::setStyle(QStringLiteral("Basic"));

    Studio *studio = new Studio(&app);
    qmlRegisterSingletonInstance<Studio>("CommStudio.Backend", 1, 0, "Studio", studio);
    qmlRegisterSingletonInstance<SerialManager>("CommStudio.Backend", 1, 0, "SerialManager",
                                                new SerialManager(studio));
    qmlRegisterSingletonInstance<ModbusManager>("CommStudio.Backend", 1, 0, "ModbusManager",
                                                new ModbusManager(studio));
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
        QTimer::singleShot(screenshotDelay, &app, [&app, &engine, screenshotPath] {
            if (!engine.rootObjects().isEmpty()) {
                if (auto *window = qobject_cast<QQuickWindow *>(engine.rootObjects().constFirst()))
                    window->grabWindow().save(QString::fromLocal8Bit(screenshotPath));
            }
            app.quit();
        });
    }

    return app.exec();
}
