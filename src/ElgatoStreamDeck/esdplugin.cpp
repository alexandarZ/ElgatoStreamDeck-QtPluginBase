#include "esdplugin.h"

ESDPluginBase::ESDPluginBase(const ESDPluginModel& pluginData, QObject* parent) : QObject(parent)
{
    m_plugin = pluginData;
    m_deviceType = kESDSDKDeviceType_StreamDeck;
    m_deviceID = "UnknownID";
    m_deviceConnected = false;

#ifdef DEBUG
    qDebug()<<"ESDPluginBase => Websocket port: "<< m_plugin.port;
    qDebug()<<"ESDPluginBase => Plugin UUID: "<< m_plugin.pluginUUID;
    qDebug()<<"ESDPluginBase => RegisterEvent: "<< m_plugin.registerEvent;
    qDebug()<<"ESDPluginBase => Info: "<< m_plugin.info;
#endif

    connect(&m_websocketConnection, &CreativeDevLabs::ESDConnection::ConnectionOpened, this, &ESDPluginBase::connection_IsOpen);
    connect(&m_websocketConnection, &CreativeDevLabs::ESDConnection::ConnectionClosed, this, &ESDPluginBase::connection_IsClosed);
    connect(&m_websocketConnection, &CreativeDevLabs::ESDConnection::DataReceived, this, &ESDPluginBase::connection_DataReceived);
}

ESDPluginBase::~ESDPluginBase()
{
    m_websocketConnection.Disconnect();

    disconnect(&m_websocketConnection, &CreativeDevLabs::ESDConnection::ConnectionOpened, this, &ESDPluginBase::connection_IsOpen);
    disconnect(&m_websocketConnection, &CreativeDevLabs::ESDConnection::ConnectionClosed, this, &ESDPluginBase::connection_IsClosed);
    disconnect(&m_websocketConnection, &CreativeDevLabs::ESDConnection::DataReceived, this, &ESDPluginBase::connection_DataReceived);
}

void ESDPluginBase::Connect()
{
    m_plugin.ValidateParameters();
    m_websocketConnection.Connect("ws://localhost:"+QString::number(m_plugin.port));
}

bool ESDPluginBase::IsDeviceConnected()
{
    return m_deviceConnected;
}

bool ESDPluginBase::IsPluginConnected()
{
    return m_websocketConnection.IsConnectionOpen();
}

// ------ CONNECTION --------

void ESDPluginBase::connection_IsOpen()
{
    // Register plugin
    QJsonObject pluginRegisterData;
    m_plugin.ToJson(pluginRegisterData);
    m_websocketConnection.WriteMessage(pluginRegisterData);

    emit ConnectionOpened();
}

void ESDPluginBase::connection_IsClosed()
{
    emit ConnectionClosed();
}

void ESDPluginBase::connection_DataReceived(QJsonObject &json)
{
    parseMessage(json);
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

    m_websocketConnection.WriteMessage(jsonObject);
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

    m_websocketConnection.WriteMessage(jsonObject);
}

void ESDPluginBase::SendToPropertyInspector(QString &action, QString &context, QJsonObject &payload)
{
    QJsonObject jsonObject;

    jsonObject[kESDSDKCommonEvent]   = kESDSDKEventSendToPropertyInspector;
    jsonObject[kESDSDKCommonContext] = context;
    jsonObject[kESDSDKCommonAction]  = action;
    jsonObject[kESDSDKCommonPayload] = payload;

    m_websocketConnection.WriteMessage(jsonObject);
}

void ESDPluginBase::SetState(int &state, QString &context)
{
    QJsonObject jsonObject;

    QJsonObject payload;
    payload[kESDSDKPayloadState] = state;

    jsonObject[kESDSDKCommonEvent] = kESDSDKEventSetState;
    jsonObject[kESDSDKCommonContext] = context;
    jsonObject[kESDSDKCommonPayload] = payload;

    m_websocketConnection.WriteMessage(jsonObject);
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

    m_websocketConnection.WriteMessage(jsonObject);
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
    m_websocketConnection.WriteMessage(jsonObject);
}

void ESDPluginBase::ShowAlert(QString &context)
{
    QJsonObject jsonObject;

    jsonObject[kESDSDKCommonEvent]   = kESDSDKEventShowAlert;
    jsonObject[kESDSDKCommonContext] = context;

    m_websocketConnection.WriteMessage(jsonObject);
}

void ESDPluginBase::ShowOk(QString &context)
{
    QJsonObject jsonObject;

    jsonObject[kESDSDKCommonEvent] = kESDSDKEventShowOK;
    jsonObject[kESDSDKCommonContext] = context;

    m_websocketConnection.WriteMessage(jsonObject);
}

void ESDPluginBase::SetSettings(QJsonObject &settings, QString &context)
{
    QJsonObject jsonObject;

    jsonObject[kESDSDKCommonEvent] = kESDSDKEventSetSettings;
    jsonObject[kESDSDKCommonContext] = context;
    jsonObject[kESDSDKCommonPayload] = settings;

    m_websocketConnection.WriteMessage(jsonObject);
}

void ESDPluginBase::SetGlobalSettings(QJsonObject &settings, QString &context)
{
    QJsonObject jsonObject;

    jsonObject[kESDSDKCommonEvent] = kESDSDKEventSetGlobalSettings;
    jsonObject[kESDSDKCommonContext] = context;
    jsonObject[kESDSDKCommonPayload] = settings;

    m_websocketConnection.WriteMessage(jsonObject);
}

void ESDPluginBase::GetSettings(QString &context)
{
   QJsonObject jsonObject;
   jsonObject[kESDSDKCommonEvent] = kESDSDKEventGetSettings;
   jsonObject[kESDSDKCommonContext] = context;

   m_websocketConnection.WriteMessage(jsonObject);
}

void ESDPluginBase::GetGlobalSettings(QJsonObject &context)
{
    QJsonObject jsonObject;
    jsonObject[kESDSDKCommonEvent] = kESDSDKEventGetGlobalSettings;
    jsonObject[kESDSDKCommonContext] = context;

    m_websocketConnection.WriteMessage(jsonObject);
}

void ESDPluginBase::OpenURL(QString &url)
{
    QJsonObject jsonObject;
    QJsonObject payload;

    payload["url"]=url;

    jsonObject[kESDSDKCommonEvent] = kESDSDKEventOpenURL;
    jsonObject[kESDSDKCommonPayload] = payload;

    m_websocketConnection.WriteMessage(jsonObject);
}

