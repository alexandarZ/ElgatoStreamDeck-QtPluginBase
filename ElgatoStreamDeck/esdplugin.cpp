#include "esdplugin.h"

ESDPluginBase::ESDPluginBase(int &port, QString &pluginUUID, QString &registerEvent, QString &info, QObject *parent) : QObject(parent)
{
 // Initialize plugin
 this->m_plugin = ESDPluginModel(port,pluginUUID,registerEvent,info);

 // Initialize device informations
 this->m_deviceType = kESDSDKDeviceType_StreamDeck; // By default device is StreamDeck
 this->m_deviceID = "UnknownID";
 this->m_deviceConnected = false; // By default device is not connected

#ifdef DEBUG
    qDebug()<<"ESDPluginBase => Websocket port: "<<port;
    qDebug()<<"ESDPluginBase => Plugin UUID: "<<pluginUUID;
    qDebug()<<"ESDPluginBase => RegisterEvent: "<<registerEvent;
    qDebug()<<"ESDPluginBase => Info: "<<info;
#endif
}

ESDPluginBase::~ESDPluginBase()
{
    this->Disconnect();
}

bool ESDPluginBase::Connect()
{
    // Validate command line arguments
    if(!this->m_plugin.ValidateParameters())
    {
        return false;
    }

    m_websocket.open("ws://localhost:"+QString::number(m_plugin.port));

    if(m_websocket.isValid())
    {
        return false;
    }

    connect(&m_websocket,&QWebSocket::connected,this,&ESDPluginBase::streamdeck_onConnected);
    connect(&m_websocket,&QWebSocket::disconnected,this,&ESDPluginBase::streamdeck_onDisconnected);

    return true;
}

bool ESDPluginBase::Disconnect()
{
    if(m_websocket.isValid())
    {
#ifdef DEBUG
    qDebug()<<"ESDPluginBase => Communication is closed!";
#endif
        disconnect(&m_websocket, &QWebSocket::textMessageReceived, this, &ESDPluginBase::streamdeck_onDataReceived);
        disconnect(&m_websocket,&QWebSocket::connected,this,&ESDPluginBase::streamdeck_onConnected);
        disconnect(&m_websocket,&QWebSocket::disconnected,this,&ESDPluginBase::streamdeck_onDisconnected);

        m_websocket.close(QWebSocketProtocol::CloseCodeNormal,"Normal exit");
        return true;
    }

    return false;
}

bool ESDPluginBase::IsDeviceConnected()
{
    return m_deviceConnected;
}

// ------ COMMUNICATION --------

void ESDPluginBase::writeJSON(QJsonObject &jsonObject)
{
    QJsonDocument doc(jsonObject);
    QString jsonStr(doc.toJson(QJsonDocument::Compact));

#ifdef DEBUG
    qInfo()<<"ESDPluginBase => WriteJSON: "<<jsonStr;
#endif

    m_websocket.sendTextMessage(jsonStr);
}

void ESDPluginBase::streamdeck_onConnected()
{
#ifdef DEBUG
    qDebug()<<"ESDPluginBase => WebSocket is open!";
#endif

    connect(&m_websocket, &QWebSocket::textMessageReceived, this, &ESDPluginBase::streamdeck_onDataReceived);

    // Register plugin
    QJsonObject pluginRegisterData;
    m_plugin.ToJson(pluginRegisterData);
    writeJSON(pluginRegisterData);
}

void ESDPluginBase::streamdeck_onDisconnected()
{
#ifdef DEBUG
    qDebug()<<"ESDPluginBase => Websocket is closed! Close reason: "<<m_websocket.closeReason();
#endif
    disconnect(&m_websocket, &QWebSocket::textMessageReceived, this, &ESDPluginBase::streamdeck_onDataReceived);
    disconnect(&m_websocket,&QWebSocket::connected,this,&ESDPluginBase::streamdeck_onConnected);
    disconnect(&m_websocket,&QWebSocket::disconnected,this,&ESDPluginBase::streamdeck_onDisconnected);
}

void ESDPluginBase::streamdeck_onDataReceived(QString message)
{
#ifdef DEBUG
    qDebug()<<"ESDPluginBase => Received JSON: "<<message;
#endif

    QJsonDocument jsonMessage = QJsonDocument::fromJson(message.toUtf8());
    QJsonObject jsonMessagObject = jsonMessage.object();

    bool messageOk = parseMessage(jsonMessagObject);

#ifdef DEBUG
    if(!messageOk)
    {
        qDebug()<<"ESDPluginBase => Could not parse received message!";
    }
#endif
}

bool ESDPluginBase::parseMessage(QJsonObject &jsonObject)
{
    // Device info message
    if(jsonObject.contains("device"))
    {
        ESDDeviceModel deviceModel;
        deviceModel.FromJson(jsonObject);

        if(deviceModel.event == kESDSDKEventDeviceDidConnect)
        {
            this->m_deviceConnected = true;
            emit DeviceDidConnect(deviceModel.deviceId,deviceModel.deviceInfo);
            return true;
        }
        else if(deviceModel.event == kESDSDKEventDeviceDidDisconnect)
        {
            this->m_deviceConnected = false;
            emit DeviceDidDisconnect(deviceModel.deviceId);
            return true;
        }
    }

    // Action message
    if(jsonObject.contains("action"))
    {
        ESDActionModel action;
        action.FromJson(jsonObject);

        // KeyDown event
        if(action.event==kESDSDKEventKeyDown)
        {
            emit KeyDown(action.context,action.deviceId,action.payload);
            return true;
        }

        // KeyUp event
        if(action.event==kESDSDKEventKeyUp)
        {
            emit KeyUp(action.context,action.deviceId,action.payload);
            return true;
        }

        // WillAppear event
        if(action.event==kESDSDKEventWillAppear)
        {
           emit WillAppear(action.context,action.deviceId,action.payload);
           return true;
        }

        // WillDisappear event
        if(action.event==kESDSDKEventWillDisappear)
        {
           emit WillDisappear(action.context,action.deviceId,action.payload);
           return true;
        }

        //TitleParametersDidChange event
        if(action.event==kESDSDKEventTitleParametersDidChange)
        {
            emit TitleParametersDidChange(action.context,action.deviceId,action.payload);
            return true;
        }

        // PropertyInspectorDidAppear event
        if(action.event==kESDSDKEventPropertyInspectorDidAppear)
        {
            emit PropertyInspectorDidAppear(action.context,action.deviceId,action.payload);
            return true;
        }

        // PropertyInspectorDiDisappear event
        if(action.event==kESDSDKEventPropertyInspectorDidDisappear)
        {
            emit PropertyInspectorDidDisappear(action.context,action.deviceId,action.payload);
            return true;
        }

        // SendToPlugin event
        if(action.event==kESDSDKEventSendToPlugin)
        {
            emit SendToPlugin(action.context,action.deviceId,action.payload);
            return true;
        }

        // DidReceiveSettings
        if(action.event==kESDSDKEventDidReceiveSettings)
        {
            emit DidReceiveSettings(action.context,action.deviceId,action.payload);
            return true;
        }

        // DidReceiveGlobalSettings
        if(action.event==kESDSDKEventDidReceiveGlobalSettings)
        {
            emit DidReceiveGlobalSettings(action.context,action.deviceId,action.payload);
            return true;
        }
    }

    // Event message
    else if(jsonObject.contains("event"))
    {
        ESDEventModel eventMsg;
        eventMsg.FromJson(jsonObject);

        // ApplicationDidLaunch event
        if(eventMsg.event==kESDSDKEventApplicationDidLaunch)
        {
            emit ApplicationDidLaunch(eventMsg.event,eventMsg.payload);
            return true;
        }

        // ApplicationDidTerminate event
        if(eventMsg.event==kESDSDKEventApplicationDidTerminate)
        {
            emit ApplicationDidTerminate(eventMsg.event,eventMsg.payload);
            return true;
        }

        // SystemDidWakeUp
        if(eventMsg.event == kESDSDKEventSystemDidWakeUp)
        {
            emit SystemDidWakeUp();
            return true;
        }
    }

    return false;
}

// ------ SLOTS --------

void ESDPluginBase::LogMessage(QString &message)
{
    QJsonObject jsonObject;

    jsonObject[kESDSDKCommonEvent] = kESDSDKEventLogMessage;

    QJsonObject payload;
    payload[kESDSDKPayloadMessage] = message;
    jsonObject[kESDSDKCommonPayload] = payload;

    writeJSON(jsonObject);
}

void ESDPluginBase::Cleanup()
{
    this->Disconnect();
}

void ESDPluginBase::SwitchProfile(QString &deviceId, QString &profileName)
{
    QJsonObject jsonObject;

    jsonObject[kESDSDKCommonEvent]   = kESDSDKEventSwitchToProfile;
    jsonObject[kESDSDKCommonContext] = m_plugin.pluginUUID;
    jsonObject[kESDSDKCommonDevice]  = deviceId;

    QJsonObject payload;
    payload[kESDSDKPayloadProfile]   = profileName;
    jsonObject[kESDSDKCommonPayload] = payload;

    writeJSON(jsonObject);
}

void ESDPluginBase::SendToPropertyInspector(QString &action, QString &context, QJsonObject &payload)
{
    QJsonObject jsonObject;

    jsonObject[kESDSDKCommonEvent]   = kESDSDKEventSendToPropertyInspector;
    jsonObject[kESDSDKCommonContext] = context;
    jsonObject[kESDSDKCommonAction]  = action;
    jsonObject[kESDSDKCommonPayload] = payload;

    writeJSON(jsonObject);
}

void ESDPluginBase::SetState(int &state, QString &context)
{
    QJsonObject jsonObject;

    QJsonObject payload;
    payload[kESDSDKPayloadState] = state;

    jsonObject[kESDSDKCommonEvent] = kESDSDKEventSetState;
    jsonObject[kESDSDKCommonContext] = context;
    jsonObject[kESDSDKCommonPayload] = payload;

    writeJSON(jsonObject);
}

void ESDPluginBase::SetTitle(QString &title, QString& context, ESDSDKTarget target)
{
    QJsonObject jsonObject;

    jsonObject[kESDSDKCommonEvent] = kESDSDKEventSetTitle;
    jsonObject[kESDSDKCommonContext] = context;

    QJsonObject payload;
    payload[kESDSDKPayloadTarget] = target;
    payload[kESDSDKPayloadTitle] = title;
    jsonObject[kESDSDKCommonPayload] = payload;

    writeJSON(jsonObject);
}

void ESDPluginBase::SetImage(QString &base64Str, QString& context,ESDSDKTarget target)
{
    QJsonObject jsonObject;
    const QString prefix = "data:image/png;base64,";

    jsonObject[kESDSDKCommonEvent] = kESDSDKEventSetImage;
    jsonObject[kESDSDKCommonContext] = context;

    QJsonObject payload;
    payload[kESDSDKPayloadTarget] = target;

    if (base64Str.contains(prefix))
    {
        payload[kESDSDKPayloadImage] = base64Str;
    }
    else
    {
        payload[kESDSDKPayloadImage] = prefix + base64Str;
    }

    jsonObject[kESDSDKCommonPayload] = payload;
    writeJSON(jsonObject);
}

void ESDPluginBase::ShowAlert(QString &context)
{
    QJsonObject jsonObject;

    jsonObject[kESDSDKCommonEvent]   = kESDSDKEventShowAlert;
    jsonObject[kESDSDKCommonContext] = context;

    writeJSON(jsonObject);
}

void ESDPluginBase::ShowOk(QString &context)
{
    QJsonObject jsonObject;

    jsonObject[kESDSDKCommonEvent] = kESDSDKEventShowOK;
    jsonObject[kESDSDKCommonContext] = context;

    writeJSON(jsonObject);
}

void ESDPluginBase::SetSettings(QJsonObject &settings, QString &context)
{
    QJsonObject jsonObject;

    jsonObject[kESDSDKCommonEvent] = kESDSDKEventSetSettings;
    jsonObject[kESDSDKCommonContext] = context;
    jsonObject[kESDSDKCommonPayload] = settings;

    writeJSON(jsonObject);
}

void ESDPluginBase::SetGlobalSettings(QJsonObject &settings, QString &context)
{
    QJsonObject jsonObject;

    jsonObject[kESDSDKCommonEvent] = kESDSDKEventSetGlobalSettings;
    jsonObject[kESDSDKCommonContext] = context;
    jsonObject[kESDSDKCommonPayload] = settings;

    writeJSON(jsonObject);
}

void ESDPluginBase::GetSettings(QString &context)
{
   QJsonObject jsonObject;
   jsonObject[kESDSDKCommonEvent] = kESDSDKEventGetSettings;
   jsonObject[kESDSDKCommonContext] = context;

   writeJSON(jsonObject);
}

void ESDPluginBase::GetGlobalSettings(QJsonObject &context)
{
    QJsonObject jsonObject;
    jsonObject[kESDSDKCommonEvent] = kESDSDKEventGetGlobalSettings;
    jsonObject[kESDSDKCommonContext] = context;

    writeJSON(jsonObject);
}

void ESDPluginBase::OpenURL(QString &url)
{
    QJsonObject jsonObject;
    QJsonObject payload;

    payload["url"]=url;

    jsonObject[kESDSDKCommonEvent] = kESDSDKEventOpenURL;
    jsonObject[kESDSDKCommonPayload] = payload;

    writeJSON(jsonObject);
}

