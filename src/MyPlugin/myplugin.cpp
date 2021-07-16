#include "myplugin.h"
#ifdef DEBUG
#include <QDebug>
#endif

MyPlugin::MyPlugin(QObject *parent) : QObject(parent)
{

}

void MyPlugin::ESDKeyDown(QString &context,QString& deviceId, QJsonObject &payload)
{
    QString title="0";
    emit SetActionTitle(title,this->context,kESDSDKTarget_HardwareAndSoftware);
}

void MyPlugin::ESDKeyUp(QString &context, QString &deviceId, QJsonObject &payload)
{
    QString title="1";
    emit SetActionTitle(title,this->context,kESDSDKTarget_HardwareAndSoftware);
}

void MyPlugin::ESDWillAppear(QString &context, QString &deviceId, QJsonObject &payload)
{
    this->context = context;
    this->deviceId = deviceId;
}

void MyPlugin::ESDWillDisappear(QString &context, QString &deviceId, QJsonObject &payload)
{
    //Nothing to do
}
