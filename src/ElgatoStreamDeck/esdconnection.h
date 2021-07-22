#ifndef ESDCONNECTION_H
#define ESDCONNECTION_H

//============================================================================
/**
@file       ESDConnection.h
@brief      Defines Web Socket connection with Elgato Stream Deck application
@copyright  (c) 2021, Aleksandar Zdravkovic
            This source code is licensed under MIT licence
**/
//============================================================================

#include <QObject>
#include <QWebSocket>
#include <QJsonObject>

namespace CreativeDevLabs {

class ESDConnection : public QObject
{
    Q_OBJECT
public:
    explicit ESDConnection(QObject *parent = nullptr);
     ~ESDConnection();

    bool IsConnectionOpen();
    void Connect(QString websocketAddress);
    void Disconnect();

public slots:
    void WriteMessage(const QJsonObject& jsonObject);

private slots:
    void websocket_onConnected();
    void websocket_onDisconnected();
    void websocket_onDataReceived(QString message);

signals:
    void DataReceived(QJsonObject& message);
    void ConnectionClosed();
    void ConnectionOpened();

private:
    QString m_websocketAddress = "";
    QWebSocket m_websocket;
};

}

#endif // ESDCONNECTION_H
