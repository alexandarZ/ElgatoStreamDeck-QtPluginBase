#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFile>
#include "ElgatoStreamDeck/esdplugin.h"
#include <MyPlugin/myplugin.h>

void textMessageLogger(QtMsgType type, const QMessageLogContext &, const QString & msg)
{
    QString txt;

    switch (type)
    {
    case QtInfoMsg:
        txt = QString("Info: %1").arg(msg);
        break;
    case QtDebugMsg:
        txt = QString("Debug: %1").arg(msg);
        break;
    case QtWarningMsg:
        txt = QString("Warning: %1").arg(msg);
    break;
    case QtCriticalMsg:
        txt = QString("Critical: %1").arg(msg);
    break;
    case QtFatalMsg:
        txt = QString("Fatal: %1").arg(msg);
    break;
    }

    QFile outFile("log");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << endl;
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    qInstallMessageHandler(textMessageLogger);

    // Parse command line arguments
    QCommandLineParser cmdParser;
    cmdParser.addHelpOption();
    QCommandLineOption portOption("port","","port","0");
    QCommandLineOption pluginuuidOption("pluginUUID","","pluginUUID","");
    QCommandLineOption registerEventOption("registerEvent","","registerEvent","");
    QCommandLineOption infoOption("info","","info","");
    cmdParser.addOption(portOption);
    cmdParser.addOption(pluginuuidOption);
    cmdParser.addOption(registerEventOption);
    cmdParser.addOption(infoOption);
    cmdParser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);

    cmdParser.process(app);

    int websocketPort = cmdParser.value(portOption).toInt();
    QString pluginUUID = cmdParser.value(pluginuuidOption);
    QString registerEvent = cmdParser.value(registerEventOption);
    QString info = cmdParser.value(infoOption);

    // Start communication with streamdeck app
    ESDPlugin esdPlugin(websocketPort,pluginUUID,registerEvent,info);
    if(!esdPlugin.Start())
    {
       app.exit(1);
    }

    // Listen for stream deck events
    MyPlugin actionInstance;
    actionInstance.connect(&esdPlugin,&ESDPlugin::WillAppear,&actionInstance,&MyPlugin::ESDWillAppear);
    actionInstance.connect(&esdPlugin,&ESDPlugin::KeyDown,&actionInstance,&MyPlugin::ESDKeyDown);
    actionInstance.connect(&esdPlugin,&ESDPlugin::KeyUp,&actionInstance,&MyPlugin::ESDKeyUp);
    esdPlugin.connect(&actionInstance,&MyPlugin::SetActionTitle,&esdPlugin,&ESDPlugin::SetTitle);

    return app.exec();
}
