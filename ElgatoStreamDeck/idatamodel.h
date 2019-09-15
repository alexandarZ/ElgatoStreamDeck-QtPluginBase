#ifndef IPLUGINMODEL_H
#define IPLUGINMODEL_H

#include <QJsonObject>
#include <QStringList>
#include <QString>
#include "ElgatoStreamDeck/esdsdkdefines.h"

class IDataModel
{
public:
    IDataModel() = default;
    virtual ~IDataModel() = default;
    virtual bool FromJson(QJsonObject& json) {return false;}
    virtual bool ToJson(QJsonObject& json) {return false;}

protected:
    bool containKeys(QJsonObject& json, QStringList& keyList)
    {
        foreach (QString key, keyList) {
            if(!json.contains(key))
            {
              return false;
            }
        }
        return true;
    }
};

#endif // IPLUGINMODEL_H
