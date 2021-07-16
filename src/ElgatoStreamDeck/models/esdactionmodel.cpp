#include "esdactionmodel.h"

bool ESDActionModel::FromJson(QJsonObject &json)
{
    this->deviceId = json["device"].toString();
    this->event = json["event"].toString();
    this->context = json["context"].toString();
    this->payload = json["payload"].toObject();
    return true;
}
