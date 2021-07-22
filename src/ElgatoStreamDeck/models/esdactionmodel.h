#ifndef ESDACTIONMODEL_H
#define ESDACTIONMODEL_H

//==============================================================================
/**
@file       ESDDeviceModel.h
@brief      Defines action model used in communication with stream deck application
@copyright  (c) 2019, Aleksandar Zdravkovic
            This source code is licensed under MIT licence
**/
//==============================================================================

#include "idatamodel.h"

class ESDActionModel : IDataModel
{
public:
    ESDActionModel()  = default;
    ~ESDActionModel() = default;

    void FromJson(const QJsonObject& json);

    QString context,deviceId,event;
    QJsonObject payload;
};

#endif // ESDACTIONMODEL_H
