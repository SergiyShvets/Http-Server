#include "tcpconnection.h"

#include <QTime>

TcpConnection::TcpConnection(QObject *parent) : QObject(parent)
{
    // make a log
    qDebug() << this << QTime::currentTime().toString() << " TCP Connection created";
}

TcpConnection::~TcpConnection()
{
    qDebug() << this << QTime::currentTime().toString() << " TCP Connection destroyed";
}

void TcpConnection::setSocket(QTcpSocket *socket)
{
    m_socket = socket;

    connect(m_socket, &QTcpSocket::connected,    this, &TcpConnection::connected);
    connect(m_socket, &QTcpSocket::disconnected, this, &TcpConnection::disconnected);
    connect(m_socket, &QTcpSocket::readyRead,    this, &TcpConnection::readyRead);
    connect(m_socket, &QTcpSocket::bytesWritten, this, &TcpConnection::bytesWritten);
    connect(m_socket, &QTcpSocket::stateChanged, this, &TcpConnection::stateChanged);
    connect(m_socket,
            static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error),
                                                 this, &TcpConnection::error);

}

QTcpSocket *TcpConnection::getSocket()
{
    if(!sender()) {
        qDebug() << this << QTime::currentTime().toString() << " sender error ";
        return 0;
    }
    return static_cast<QTcpSocket*>(sender());
}

void TcpConnection::connected()
{
    if(!sender()) {
        qDebug() << this << QTime::currentTime().toString() << " sender error ";
        return;
    }
    qDebug() << this << QTime::currentTime().toString() << " Connected: " << sender();
}

void TcpConnection::disconnected()
{
    if(!sender()) {
        qDebug() << this << QTime::currentTime().toString() << " sender error ";
        return;
    }

    qDebug() << this << QTime::currentTime().toString() << " Disconnected: " << getSocket();
}

void TcpConnection::readyRead()
{
    if(!sender()) {
        qDebug() << this << QTime::currentTime().toString() << " sender error ";
        return;
    }

    qDebug() << this << QTime::currentTime().toString() << " Ready read: " << getSocket();
    /*
    QTcpSocket *socket = getSocket();
    if(!socket)
        return;

    socket->close();
    */
}

void TcpConnection::bytesWritten(qint64 bytes)
{
    if(!sender()) {
        qDebug() << this << QTime::currentTime().toString() << " sender error ";
        return;
    }

    qDebug() << this << QTime::currentTime().toString() << " Bytes written: "
             << getSocket() << "number of bytes: " << bytes;
}

void TcpConnection::stateChanged(QAbstractSocket::SocketState socketState)
{
    if(!sender()) {
        qDebug() << this << QTime::currentTime().toString() << " sender error ";
        return;
    }

    qDebug() << this << QTime::currentTime().toString() << " State changed: "
             << getSocket() << "state: " << socketState;
}

void TcpConnection::error(QAbstractSocket::SocketError socketError)
{

    if(!sender()) {
        qDebug() << this << QTime::currentTime().toString() << " sender error ";
        return;
    }

    qDebug() << this << QTime::currentTime().toString() << " Error: "
             << getSocket() << "error type: " << socketError;

}
