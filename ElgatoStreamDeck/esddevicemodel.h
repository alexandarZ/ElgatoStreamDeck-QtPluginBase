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

#include <QString>
#include <QJsonObject>
#include "ElgatoStreamDeck/esdsdkdefines.h"

class ESDDeviceModel
{
public:
    ESDDeviceModel()  = default;
    ~ESDDeviceModel() = default;

    void FromJsonObject(QJsonObject& json);

    QString event,deviceId;
    QJsonObject deviceInfo;
};

#endif // ESDDEVICEMODEL_H
