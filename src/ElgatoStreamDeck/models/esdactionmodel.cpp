#include "esdactionmodel.h"

void ESDActionModel::FromJson(const QJsonObject &json)
{
    this->deviceId = json["device"].toString();
    this->event = json["event"].toString();
    this->context = json["context"].toString();
    this->payload = json["payload"].toObject();
}
