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
#include "esdconnection.h"
#include <QJsonDocument>
#include "models/esddevicemodel.h"
#include "models/esdactionmodel.h"
#include "models/esdpluginmodel.h"
#include "models/esdeventmodel.h"

#ifdef DEBUG
#include <QDebug>
#endif

class ESDPluginBase : public QObject
{
    Q_OBJECT
public:
    explicit ESDPluginBase(const ESDPluginModel& pluginData,QObject* parent=nullptr);

    ~ESDPluginBase();

    void Connect();
    void Disconnect();

    bool IsDeviceConnected();
    bool IsPluginConnected();

private:
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

    // Local signal to notify application that WS is closed
    void ConnectionClosed();

    // Local signal to notify application that WS is open
    void ConnectionOpened();

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

private slots:
    void connection_IsOpen();
    void connection_IsClosed();
    void connection_DataReceived(QJsonObject& json);

private:
    ESDPluginModel m_plugin;
    CreativeDevLabs::ESDConnection m_websocketConnection;
    ESDSDKDeviceType m_deviceType;
    QString m_deviceID;
    bool m_deviceConnected;
};

#endif // ESDPLUGIN_H
