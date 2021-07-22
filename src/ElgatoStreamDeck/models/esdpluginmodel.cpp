#include "esdpluginmodel.h"
#include <QDebug>

ESDPluginModel::ESDPluginModel(int port, QString pluginUUID, QString registerEvent, QString info)
{
    this->port = port;
    this->pluginUUID = pluginUUID;
    this->registerEvent = registerEvent;
    this->info = info;
}

void ESDPluginModel::ValidateParameters()
{
    // Check websocket port
    if(this->port <= 0)
    {
        throw std::runtime_error("ESDPluginModel => ERROR: Websocket port can't be less than or equal 0!");
    }

    // Check pluginUUID
    if(this->pluginUUID.isEmpty())
    {
        throw std::runtime_error("ESDPluginModel => ERROR: PluginUUID can't be empty!");
    }

    // Check registerEvent
    if(this->registerEvent.isEmpty())
    {
        throw std::runtime_error("ESDPluginModel => ERROR: RegisterEvent can't be empty!");
    }

    // Check info
    if(this->info.isEmpty())
    {
        throw std::runtime_error("ESDPluginModel => ERROR: Info can't be empty!");
    }
}

void ESDPluginModel::FromJson(const QJsonObject &json)
{
    QStringList keyList = {"port","uuid","event","info"};

    if(!IDataModel::containKeys(json,keyList))
    {
        std::string jsonStr = QJsonDocument(json).toJson(QJsonDocument::Compact).toStdString();
        throw std::runtime_error("ESDPluginModel => JSON \"" + jsonStr + "\" does not contain valid keys!");
    }

    this->port = json["port"].toInt();
    this->pluginUUID = json ["uuid"].toString();
    this->registerEvent = json["event"].toString();
    this->info = json["info"].toString();
}

void ESDPluginModel::ToJson(QJsonObject &json)
{
    json["uuid"] = this->pluginUUID;
    json["event"] = this->registerEvent;
}
