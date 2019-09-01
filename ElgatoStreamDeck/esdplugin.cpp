#include "esdplugin.h"

ESDPlugin::ESDPlugin(int &port, QString &pluginUUID, QString &registerEvent, QString &info, QObject *parent) : QObject(parent)
{
 // Initialize plugin
 this->m_port = port;
 this->m_pluginUUID = pluginUUID;
 this->m_registerEvent = registerEvent;
 this->m_info = info;

 // Initialize device informations
 this->m_deviceType = kESDSDKDeviceType_StreamDeck; // By default device is StreamDeck
 this->m_deviceID = "UnknownID";
 this->m_deviceConnected = false; // By default device is not connected

#ifdef DEBUG
    qDebug()<<"Websocket port: "<<this->m_port;
    qDebug()<<"Plugin UUID: "<<this->m_pluginUUID;
    qDebug()<<"RegisterEvent: "<<this->m_registerEvent;
    qDebug()<<"Info: "<<this->m_info;
#endif
}

ESDPlugin::~ESDPlugin()
{
#ifdef DEBUG
    qDebug()<<"ESDPlugin will now exit";
#endif
    this->Stop();
}

bool ESDPlugin::validateCmdArguments()
{
    // Check websocket port
    if(this->m_port <= 0)
    {
#ifdef DEBUG
        qDebug()<<"ERROR: Websocket port can't be less than or equal 0!";
#endif
        return false;
    }

    // Check pluginUUID
    if(this->m_pluginUUID.isEmpty())
    {
#ifdef DEBUG
        qDebug()<<"ERROR: PluginUUID can't be empty!";
#endif
        return false;
    }

    // Check registerEvent
    if(this->m_registerEvent.isEmpty())
    {
#ifdef DEBUG
        qDebug()<<"ERROR: RegisterEvent can't be empty!";
#endif
        return false;
    }

    // Check info
    if(this->m_info.isEmpty())
    {
#ifdef DEBUG
        qDebug()<<"ERROR: Info can't be empty!";
#endif
        return false;
    }

    return true;
}

bool ESDPlugin::Start()
{
    // Validate command line arguments
    if(!validateCmdArguments())
    {
        return false;
    }

    // Open websocket
    connect(&m_websocket,&QWebSocket::connected,this,&ESDPlugin::streamdeck_onConnected);
    connect(&m_websocket,&QWebSocket::disconnected,this,&ESDPlugin::streamdeck_onDisconnected);

    m_websocket.open("ws://localhost:"+QString::number(m_port));

    // Return websocket state
    if(m_websocket.isValid())
    {
        return false;
    }

    return true;
}

bool ESDPlugin::Stop()
{
    if(m_websocket.isValid())
    {
#ifdef DEBUG
    qDebug()<<"Communication is closed!";
#endif
        m_websocket.close(QWebSocketProtocol::CloseCodeNormal,"Normal exit");
        return true;
    }

    return false;
}

bool ESDPlugin::IsDeviceConnected()
{
    return m_deviceConnected;
}

// ------ COMMUNICATION --------

void ESDPlugin::writeJSON(QJsonObject &jsonObject)
{
    QJsonDocument doc(jsonObject);
    QString jsonStr(doc.toJson(QJsonDocument::Compact));

#ifdef DEBUG
    qInfo()<<"Sending JSON string: "<<jsonStr;
#endif

    m_websocket.sendTextMessage(jsonStr);
}

void ESDPlugin::streamdeck_onConnected()
{
#ifdef DEBUG
    qDebug()<<"Websocket is open!";
#endif

    connect(&m_websocket, &QWebSocket::textMessageReceived, this, &ESDPlugin::streamdeck_onDataReceived);

    // Register plugin
    QJsonObject pluginRegisterData;
    pluginRegisterData["event"]=this->m_registerEvent;
    pluginRegisterData["uuid"] =this->m_pluginUUID;

    writeJSON(pluginRegisterData);
}

void ESDPlugin::streamdeck_onDisconnected()
{
#ifdef DEBUG
    qDebug()<<"Websocket is disconnected! Close reason: "<<m_websocket.closeReason();
#endif
}

void ESDPlugin::streamdeck_onDataReceived(QString message)
{
#ifdef DEBUG
    qDebug()<<"Received JSON: "<<message;
#endif

    QJsonDocument jsonMessage = QJsonDocument::fromJson(message.toUtf8());
    QJsonObject jsonMessagObject = jsonMessage.object();

    bool messageOk = parseMessage(jsonMessagObject);

#ifdef DEBUG
    if(!messageOk)
    {
        qDebug()<<"Could not parse received message!";
    }
#endif
}

bool ESDPlugin::parseMessage(QJsonObject &jsonObject)
{
    // Device info message
    if(jsonObject.contains("device"))
    {
        ESDDeviceModel deviceModel;
        deviceModel.FromJsonObject(jsonObject);

        if(deviceModel.event == kESDSDKEventDeviceDidConnect)
        {
            this->m_deviceConnected = true;
            emit DeviceConnected(deviceModel.deviceId,deviceModel.deviceInfo);
            return true;
        }
        else if(deviceModel.event == kESDSDKEventDeviceDidDisconnect)
        {
            this->m_deviceConnected = false;
            emit DeviceDisconnected(deviceModel.deviceId);
            return true;
        }
    }

    // Action message
    if(jsonObject.contains("action"))
    {
        ESDActionModel action;
        action.FromJsonObject(jsonObject);

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

        // ApplicationDidLaunch event
        if(action.event==kESDSDKEventApplicationDidLaunch)
        {
            emit ApplicationDidLaunch(action.context,action.deviceId,action.payload);
            return true;
        }

        // ApplicationDidTerminate event
        if(action.event==kESDSDKEventApplicationDidTerminate)
        {
            emit ApplicationDidTerminate(action.context,action.deviceId,action.payload);
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

    return false;
}

// ------ SLOTS --------

void ESDPlugin::LogMessage(QString &message)
{
    QJsonObject jsonObject;

    jsonObject[kESDSDKCommonEvent] = kESDSDKEventLogMessage;

    QJsonObject payload;
    payload[kESDSDKPayloadMessage] = message;
    jsonObject[kESDSDKCommonPayload] = payload;

    writeJSON(jsonObject);
}

void ESDPlugin::SwitchProfile(QString &deviceId, QString &profileName)
{
    QJsonObject jsonObject;

    jsonObject[kESDSDKCommonEvent]   = kESDSDKEventSwitchToProfile;
    jsonObject[kESDSDKCommonContext] = this->m_pluginUUID;
    jsonObject[kESDSDKCommonDevice]  = deviceId;

    QJsonObject payload;
    payload[kESDSDKPayloadProfile]   = profileName;
    jsonObject[kESDSDKCommonPayload] = payload;

    writeJSON(jsonObject);
}

void ESDPlugin::SendToPropertyInspector(QString &action, QString &context, QJsonObject &payload)
{
    QJsonObject jsonObject;

    jsonObject[kESDSDKCommonEvent]   = kESDSDKEventSendToPropertyInspector;
    jsonObject[kESDSDKCommonContext] = context;
    jsonObject[kESDSDKCommonAction]  = action;
    jsonObject[kESDSDKCommonPayload] = payload;

    writeJSON(jsonObject);
}

void ESDPlugin::SetState(int &state, QString &context)
{
    QJsonObject jsonObject;

    QJsonObject payload;
    payload[kESDSDKPayloadState] = state;

    jsonObject[kESDSDKCommonEvent] = kESDSDKEventSetState;
    jsonObject[kESDSDKCommonContext] = context;
    jsonObject[kESDSDKCommonPayload] = payload;

    writeJSON(jsonObject);
}

void ESDPlugin::SetTitle(QString &title, QString& context, ESDSDKTarget target)
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

void ESDPlugin::SetImage(QString &base64Str, QString& context,ESDSDKTarget target)
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

void ESDPlugin::ShowAlert(QString &context)
{
    QJsonObject jsonObject;

    jsonObject[kESDSDKCommonEvent]   = kESDSDKEventShowAlert;
    jsonObject[kESDSDKCommonContext] = context;

    writeJSON(jsonObject);
}

void ESDPlugin::ShowOk(QString &context)
{
    QJsonObject jsonObject;

    jsonObject[kESDSDKCommonEvent] = kESDSDKEventShowOK;
    jsonObject[kESDSDKCommonContext] = context;

    writeJSON(jsonObject);
}

void ESDPlugin::SetSettings(QJsonObject &settings, QString &context)
{
    QJsonObject jsonObject;

    jsonObject[kESDSDKCommonEvent] = kESDSDKEventSetSettings;
    jsonObject[kESDSDKCommonContext] = context;
    jsonObject[kESDSDKCommonPayload] = settings;

    writeJSON(jsonObject);
}

void ESDPlugin::SetGlobalSettings(QJsonObject &settings, QString &context)
{
    QJsonObject jsonObject;

    jsonObject[kESDSDKCommonEvent] = kESDSDKEventSetGlobalSettings;
    jsonObject[kESDSDKCommonContext] = context;
    jsonObject[kESDSDKCommonPayload] = settings;

    writeJSON(jsonObject);
}

void ESDPlugin::GetSettings(QString &context)
{
   QJsonObject jsonObject;
   jsonObject[kESDSDKCommonEvent] = kESDSDKEventGetSettings;
   jsonObject[kESDSDKCommonContext] = context;

   writeJSON(jsonObject);
}

void ESDPlugin::GetGlobalSettings(QJsonObject &context)
{
    QJsonObject jsonObject;
    jsonObject[kESDSDKCommonEvent] = kESDSDKEventGetGlobalSettings;
    jsonObject[kESDSDKCommonContext] = context;

    writeJSON(jsonObject);
}

void ESDPlugin::OpenURL(QString &url)
{
    QJsonObject jsonObject;
    QJsonObject payload;

    payload["url"]=url;

    jsonObject[kESDSDKCommonEvent] = kESDSDKEventOpenURL;
    jsonObject[kESDSDKCommonPayload] = payload;

    writeJSON(jsonObject);
}

