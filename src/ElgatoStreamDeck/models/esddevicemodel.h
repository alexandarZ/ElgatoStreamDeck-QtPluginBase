#ifndef ESDDEVICEMODEL_H
#define ESDDEVICEMODEL_H

//==============================================================================
/**
@file       ESDDeviceModel.h
@brief      Defines device model used in communication with stream deck application
@copyright  (c) 2019, Aleksandar Zdravkovic
            This source code is licensed under MIT licence
**/
//==============================================================================

#include "idatamodel.h"

class ESDDeviceModel : IDataModel
{
public:
    ESDDeviceModel()  = default;
    ~ESDDeviceModel() = default;

    void FromJson(const QJsonObject& json);

    QString event,deviceId;
    QJsonObject deviceInfo;
};

#endif // ESDDEVICEMODEL_H
