#ifndef ESDPLUGINMODEL_H
#define ESDPLUGINMODEL_H

//==============================================================================
/**
@file       ESDPluginModel.h
@brief      Stores informations about plugin
@copyright  (c) 2019, Aleksandar Zdravkovic
            This source code is licensed under MIT licence
**/
//==============================================================================


#include <QString>
#include "idatamodel.h"

class ESDPluginModel : IDataModel
{
public:
    ESDPluginModel() = default;
    ESDPluginModel(int port,QString pluginUUID,QString registerEvent, QString info);
    ~ESDPluginModel() = default;

    bool ValidateParameters();
    bool FromJson(QJsonObject& json);
    bool ToJson(QJsonObject& json);

    int port;
    QString pluginUUID,registerEvent,info;
};

#endif // ESDPLUGINMODEL_H
