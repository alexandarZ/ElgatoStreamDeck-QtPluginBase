#include "esddevicemodel.h"

bool ESDDeviceModel::FromJson(QJsonObject &json)
{
    this->deviceId = json["device"].toString();
    this->event = json["event"].toString();

    if(json.contains("deviceInfo"))
    {
        QJsonObject deviceInfo = json["deviceInfo"].toObject();
    }

    return true;
}
