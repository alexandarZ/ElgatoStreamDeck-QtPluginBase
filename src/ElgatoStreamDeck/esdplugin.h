#ifndef ESDPLUGIN_H
#define ESDPLUGIN_H

//==============================================================================
/**
@file       ESDPlugin.h
@brief      Defines plugin source which communicates with Elgato Stream Deck application
@copyright  (c) 2019, Aleksandar Zdravkovic
            This source code is licensed under MIT licence
**/
//==============================================================================

#include <QObject>
#include <QWebSocket>
#include <QJsonDocument>
#include "ElgatoStreamDeck/esddevicemodel.h"
#include "ElgatoStreamDeck/esdactionmodel.h"
#include "ElgatoStreamDeck/esdpluginmodel.h"
#include "ElgatoStreamDeck/esdeventmodel.h"

#ifdef DEBUG
#include <QDebug>
#endif

class ESDPluginBase : public QObject
{
    Q_OBJECT
public:
    explicit ESDPluginBase(int& port,QString& pluginUUID, QString& registerEvent, QString& info,QObject *parent = nullptr);
    explicit ESDPluginBase(ESDPluginModel& model,QObject* parent=nullptr);

    ~ESDPluginBase();

    bool Connect();
    bool Disconnect();

    bool IsDeviceConnected();

private:
    void writeJSON(QJsonObject& jsonObject);
    bool parseMessage(QJsonObject& jsonObject);

signals:
    void DeviceDidConnect(QString& deviceId, QJsonObject& deviceInfo);
    void DeviceDidDisconnect(QString& deviceId);

    void KeyDown(QString& context,QString& deviceId,QJsonObject& payload);
    void KeyUp(QString& context,QString& deviceId,QJsonObject& payload);
    void WillAppear(QString& context,QString& deviceId,QJsonObject& payload);
    void WillDisappear(QString& context,QString& deviceId,QJsonObject& payload);
    void TitleParametersDidChange(QString& context, QString& deviceId,QJsonObject& payload);
    void ApplicationDidLaunch(QString& event,QJsonObject& payload);
    void ApplicationDidTerminate(QString& event, QJsonObject& payload);
    void PropertyInspectorDidAppear(QString& context, QString& deviceId,QJsonObject& payload);
    void PropertyInspectorDidDisappear(QString& context, QString& deviceId,QJsonObject& payload);
    void SendToPlugin(QString& context, QString& deviceId,QJsonObject& payload);
    void DidReceiveSettings(QString& context, QString& deviceId,QJsonObject& payload);
    void DidReceiveGlobalSettings(QString& context, QString& deviceId,QJsonObject& payload);
    void SystemDidWakeUp();

public slots:
    void SetSettings(QJsonObject& settings, QString& context);
    void SetGlobalSettings(QJsonObject& settings, QString& context);
    void GetSettings(QString& context);
    void GetGlobalSettings(QJsonObject& context);
    void OpenURL(QString& url);
    void SetState(int& state, QString& context);
    void SetTitle(QString& title, QString &context, ESDSDKTarget target=0);
    void SetImage(QString& base64Str, QString& context, ESDSDKTarget target=0);
    void ShowAlert(QString& context);
    void ShowOk(QString& context);
    void SendToPropertyInspector(QString& action,QString& context,QJsonObject& payload);
    void SwitchProfile(QString& deviceId, QString& profileName);
    void LogMessage(QString& message);
    void Cleanup(); //For closing connection before exiting in application

private slots:
    void streamdeck_onConnected();
    void streamdeck_onDisconnected();
    void streamdeck_onDataReceived(QString message);

private:
    ESDPluginModel m_plugin;
    QWebSocket m_websocket;
    ESDSDKDeviceType m_deviceType;
    QString m_deviceID;
    bool m_deviceConnected;
};

#endif // ESDPLUGIN_H
