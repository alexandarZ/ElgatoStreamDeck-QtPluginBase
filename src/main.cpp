#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFile>
#include "ElgatoStreamDeck/esdplugin.h"
#include "ElgatoStreamDeck/models/esdpluginmodel.h"
#include <MyPlugin/myplugin.h>
#include <stdexcept>

void textMessageLogHandler(QtMsgType type, const QMessageLogContext &, const QString & msg)
{
    QString logMessage = "";
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");

    switch (type)
    {
    case QtInfoMsg:
        logMessage = QString("[INFO]: %1").arg(msg);
        break;
    case QtDebugMsg:
        logMessage = QString("[DEBUG]: %1").arg(msg);
        break;
    case QtWarningMsg:
        logMessage = QString("[WARNING]: %1").arg(msg);
    break;
    case QtCriticalMsg:
        logMessage = QString("[CRITICAL]: %1").arg(msg);
    break;
    case QtFatalMsg:
        logMessage = QString("[FATAL]: %1").arg(msg);
    break;
    }

    QFile outFile("esdplugin-logs.log");

    try
    {
        outFile.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream textStream(&outFile);
        textStream << timestamp << " " << logMessage << Qt::endl;
        textStream.flush();
    }
    catch (std::runtime_error& error)
    {
        // User does not have enough permissions to write into log file?
    }
}

ESDPluginModel parseCommandLineParameters(const QCoreApplication& application)
{
    QCommandLineOption portOption("port","","port","0");
    QCommandLineOption pluginUUIDOption("pluginUUID","","pluginUUID","");
    QCommandLineOption registerEventOption("registerEvent","","registerEvent","");
    QCommandLineOption infoOption("info","","info","");
    QCommandLineParser cmdParser;

    cmdParser.addHelpOption();
    cmdParser.addOption(portOption);
    cmdParser.addOption(pluginUUIDOption);
    cmdParser.addOption(registerEventOption);
    cmdParser.addOption(infoOption);
    cmdParser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);

    try
    {
        cmdParser.process(application);
    }
    catch (const std::runtime_error& error)
    {
       throw std::runtime_error("FAILED to read commmand line parameters! Error: " + std::string(error.what()));
    }

    int websocketPort = cmdParser.value(portOption).toInt();
    QString pluginUUID = cmdParser.value(pluginUUIDOption);
    QString registerEvent = cmdParser.value(registerEventOption);
    QString info = cmdParser.value(infoOption);

    return ESDPluginModel(websocketPort, pluginUUID, registerEvent, info);
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    qInstallMessageHandler(textMessageLogHandler);

    auto pluginParameters = parseCommandLineParameters(app);

    ESDPluginBase esdPlugin(pluginParameters);
    MyPlugin actionInstance;

    try
    {
        actionInstance.connect(&esdPlugin,&ESDPluginBase::WillAppear,&actionInstance,&MyPlugin::ESDWillAppear);
        actionInstance.connect(&esdPlugin,&ESDPluginBase::KeyDown,&actionInstance,&MyPlugin::ESDKeyDown);
        actionInstance.connect(&esdPlugin,&ESDPluginBase::KeyUp,&actionInstance,&MyPlugin::ESDKeyUp);
        actionInstance.connect(&esdPlugin,&ESDPluginBase::WillDisappear,&actionInstance,&MyPlugin::ESDWillDisappear);
        esdPlugin.connect(&actionInstance,&MyPlugin::SetActionTitle,&esdPlugin,&ESDPluginBase::SetTitle);

        esdPlugin.Connect();
    }
    catch (const std::runtime_error& error)
    {
        std::string errorMsg = error.what();
        qDebug()<<QString::fromStdString(errorMsg);

        //TODO: Disconnect-uj sve dogadjaje
        //TODO: Povezi se na ConnectionOpened i ConnectionClosed
    }

    return app.exec();
}
