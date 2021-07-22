#include "esdconnection.h"
#include <QJsonDocument>

namespace CreativeDevLabs {

ESDConnection::ESDConnection(QObject *parent) : QObject(parent)
{

}

ESDConnection::~ESDConnection()
{
    Disconnect();
}

bool ESDConnection::IsConnectionOpen()
{
    return m_websocket.isValid();
}

void ESDConnection::Connect(QString websocketAddress)
{
    if(m_websocket.isValid())
    {
        qWarning()<<"ESDConnection => Connection is already open, can't open it twice!";
        return;
    }

    m_websocketAddress = websocketAddress;

    try
    {
        m_websocket.open(m_websocketAddress);

        if(!m_websocket.isValid())
        {
            throw std::runtime_error("ESDConnection => FAILED to open websocket!");
        }

        connect(&m_websocket,&QWebSocket::connected,this,&ESDConnection::websocket_onConnected);
        connect(&m_websocket,&QWebSocket::disconnected,this,&ESDConnection::websocket_onDisconnected);
    }
    catch (...)
    {
        throw std::runtime_error("ESDConnection => FAILED to open websocket! Unknown exception happened");
    }
}

void ESDConnection::Disconnect()
{
    if(m_websocket.isValid())
    {
        disconnect(&m_websocket, &QWebSocket::textMessageReceived, this, &ESDConnection::websocket_onDataReceived);
        disconnect(&m_websocket, &QWebSocket::connected, this,&ESDConnection::websocket_onConnected);
        disconnect(&m_websocket, &QWebSocket::disconnected, this,&ESDConnection::websocket_onDisconnected);

        m_websocket.close(QWebSocketProtocol::CloseCodeNormal, "Normal exit");
    }
}

void ESDConnection::WriteMessage(const QJsonObject &jsonObject)
{
    QJsonDocument doc(jsonObject);
    QString jsonStr(doc.toJson(QJsonDocument::Compact));

    if(m_websocket.isValid())
    {
        m_websocket.sendTextMessage(jsonStr);
#ifdef DEBUG
    qDebug()<<"ESDConnection => WriteMessage: "<<jsonStr;
#endif
    }
    else
    {
        qWarning()<<"ESDConnection => Failed to write message \""<<jsonStr << "\" ! Web socket is closed.";
    }
}

void ESDConnection::websocket_onConnected()
{
    connect(&m_websocket, &QWebSocket::textMessageReceived, this, &ESDConnection::websocket_onDataReceived);
    emit ConnectionOpened();

#ifdef DEBUG
    qDebug()<<"ESDConnection => WebSocket connection is established!";
#endif
}

void ESDConnection::websocket_onDisconnected()
{
#ifdef DEBUG
    qDebug()<<"ESDConnection => Websocket is closed! Close reason: "<<m_websocket.closeReason();
#endif

    disconnect(&m_websocket, &QWebSocket::textMessageReceived, this, &ESDConnection::websocket_onDataReceived);
    disconnect(&m_websocket, &QWebSocket::connected, this, &ESDConnection::websocket_onConnected);
    disconnect(&m_websocket, &QWebSocket::disconnected, this, &ESDConnection::websocket_onDisconnected);

    emit ConnectionClosed();
}

void ESDConnection::websocket_onDataReceived(QString message)
{
#ifdef DEBUG
    qDebug()<<"ESDConnection => Received JSON message \""<<message<<"\"";
#endif

  QJsonDocument jsonMessageDocument = QJsonDocument::fromJson(message.toUtf8());
  QJsonObject jsonMessage = jsonMessageDocument.object();

  emit DataReceived(jsonMessage);
}

}
