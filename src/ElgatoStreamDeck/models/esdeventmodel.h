#ifndef ESDEVENTMODEL_H
#define ESDEVENTMODEL_H

#include "idatamodel.h"

class ESDEventModel : IDataModel
{
public:
    ESDEventModel() = default;
    ~ESDEventModel() = default;

    void FromJson(const QJsonObject& json);

    QString event;
    QJsonObject payload;
};

#endif // ESDEVENTMODEL_H
