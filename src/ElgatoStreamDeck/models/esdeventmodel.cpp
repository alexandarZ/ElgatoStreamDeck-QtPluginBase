#include "esdeventmodel.h"

void ESDEventModel::FromJson(const QJsonObject &json)
{
    this->event = json["event"].toString();
    if(json.contains("payload"))
    {
        this->payload = json["payload"].toObject();
    }
}
