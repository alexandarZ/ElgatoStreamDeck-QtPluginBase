#ifndef ESDEVENTMODEL_H
#define ESDEVENTMODEL_H

#include "idatamodel.h"

class ESDEventModel : IDataModel
{
public:
    ESDEventModel() = default;
    ~ESDEventModel() = default;

    bool FromJson(QJsonObject& json);

    QString event;
    QJsonObject payload;
};

#endif // ESDEVENTMODEL_H
