#include "esdeventmodel.h"

bool ESDEventModel::FromJson(QJsonObject &json)
{
    this->event = json["event"].toString();
    if(json.contains("payload"))
    {
        this->payload = json["payload"].toObject();
    }
    return true;
}
