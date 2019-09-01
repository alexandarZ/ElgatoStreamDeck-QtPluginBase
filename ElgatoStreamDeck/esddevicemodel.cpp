#include "esddevicemodel.h"

void ESDDeviceModel::FromJsonObject(QJsonObject &json)
{
    this->deviceId = json["device"].toString();
    this->event = json["event"].toString();

    if(json.contains("deviceInfo"))
    {
        QJsonObject deviceInfo = json["deviceInfo"].toObject();
    }
}
