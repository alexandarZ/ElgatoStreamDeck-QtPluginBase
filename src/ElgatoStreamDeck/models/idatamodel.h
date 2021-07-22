#ifndef IPLUGINMODEL_H
#define IPLUGINMODEL_H

#include <QJsonObject>
#include <QJsonDocument>
#include <QStringList>
#include <QString>
#include "../esdsdkdefines.h"

class IDataModel
{
public:
    IDataModel() = default;

    virtual ~IDataModel() = default;

    // Convert QJsonObject data into model
    virtual void FromJson(const QJsonObject& json) { }

    // Convert model into QJsonObject
    virtual void ToJson(QJsonObject& json) { }

protected:

    // Check does QJsonObject contains keys from the list
    bool containKeys(const QJsonObject& json, const QStringList& keyList)
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
