#ifndef MYPLUGIN_H
#define MYPLUGIN_H

//==============================================================================
/**
@file       MyPlugin.h
@brief      Just an example how to communicate with stream deck app using ESDPlugin
@copyright  (c) 2019, Aleksandar Zdravkovic
            This source code is licensed under MIT licence
**/
//==============================================================================

#include <QObject>
#include <ElgatoStreamDeck/esdsdkdefines.h>

class MyPlugin : public QObject
{
    Q_OBJECT
public:
    explicit MyPlugin(QObject *parent = nullptr);
    ~MyPlugin() = default;

signals:
    void SetActionTitle(QString& title, QString& context,ESDSDKTarget target);
public slots:
    void ESDKeyDown(QString& context,QString& deviceId,QJsonObject& payload);
    void ESDKeyUp(QString& context,QString& deviceId,QJsonObject& payload);
    void ESDWillAppear(QString& context,QString& deviceId,QJsonObject& payload);
    void ESDWillDisappear(QString& context, QString& deviceId,QJsonObject& payload);
private:
    QString deviceId,context;
};

#endif // MYPLUGIN_H
