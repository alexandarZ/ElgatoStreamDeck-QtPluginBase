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
#include <QJsonObject>
#include <QJsonDocument>
#include "ElgatoStreamDeck/esdsdkdefines.h"
#include "ElgatoStreamDeck/esddevicemodel.h"
#include "ElgatoStreamDeck/esdactionmodel.h"

#ifdef DEBUG
#include <QDebug>
#endif

class ESDPlugin : public QObject
{
    Q_OBJECT
public:
    explicit ESDPlugin(int& port,QString& pluginUUID, QString& registerEvent, QString& info,QObject *parent = nullptr);
    ~ESDPlugin();

    bool Start();
    bool Stop();

    bool IsDeviceConnected();

private:
    bool validateCmdArguments();
    void writeJSON(QJsonObject& jsonObject);
    bool parseMessage(QJsonObject& jsonObject);

signals:
    void DeviceConnected(QString& deviceId, QJsonObject& deviceInfo);
    void DeviceDisconnected(QString& deviceId);

    void KeyDown(QString& context,QString& deviceId,QJsonObject& payload);
    void KeyUp(QString& context,QString& deviceId,QJsonObject& payload);
    void WillAppear(QString& context,QString& deviceId,QJsonObject& payload);
    void WillDisappear(QString& context,QString& deviceId,QJsonObject& payload);
    void TitleParametersDidChange(QString& context, QString& deviceId,QJsonObject& payload);
    void ApplicationDidLaunch(QString& context, QString& deviceId,QJsonObject& payload);
    void ApplicationDidTerminate(QString& context, QString& deviceId,QJsonObject& payload);
    void PropertyInspectorDidAppear(QString& context, QString& deviceId,QJsonObject& payload);
    void PropertyInspectorDidDisappear(QString& context, QString& deviceId,QJsonObject& payload);
    void SendToPlugin(QString& context, QString& deviceId,QJsonObject& payload);
    void DidReceiveSettings(QString& context, QString& deviceId,QJsonObject& payload);
    void DidReceiveGlobalSettings(QString& context, QString& deviceId,QJsonObject& payload);

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
    void streamdeck_onConnected();
    void streamdeck_onDisconnected();
    void streamdeck_onDataReceived(QString message);

private:
    int m_port;
    QString m_pluginUUID,m_registerEvent,m_info;
    QWebSocket m_websocket;
    ESDSDKDeviceType m_deviceType;
    QString m_deviceID;
    bool m_deviceConnected;
};

#endif // ESDPLUGIN_H
