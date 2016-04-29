#include "tcpserver.h"

#include <QTime>

TcpServer::TcpServer(/*QTcpServer*/ QObject *parent) : QTcpServer(parent)
{
    qDebug() << this << QTime::currentTime().toString() << " TCP Server started";
}

TcpServer::~TcpServer()
{
    qDebug() << this << QTime::currentTime().toString() << " TCP Server finished";
}

bool TcpServer::listen(const QHostAddress &address, quint16 port)
{
    if(!QTcpServer::listen(address, port)) {
        qDebug() << this << QTime::currentTime().toString() << " Listen error";
        return false;
    }

    m_thread = new QThread(this);
    m_connections = new TcpConnections();

    connect(m_thread, &QThread::started, m_connections, &TcpConnections::start,   Qt::QueuedConnection);
    connect(this, &TcpServer::accepting, m_connections, &TcpConnections::accept,  Qt::QueuedConnection);
    connect(this, &TcpServer::finished,  m_connections, &TcpConnections::quit,    Qt::QueuedConnection);
    connect(m_connections, &TcpConnections::finished, this, &TcpServer::complete, Qt::QueuedConnection);

    m_connections->moveToThread(m_thread);
    m_thread->start();

    return true;
}

void TcpServer::close()
{
    qDebug() << this << QTime::currentTime().toString() << " Closing server";
    emit finished();
    QTcpServer::close();

}

quint64 TcpServer::port()
{
    if(isListening()) {
        return this->serverPort();
    } else {
        return 1000;
    }
}

void TcpServer::incomingConnection(qintptr descriptor)
{
    qDebug() << this << QTime::currentTime().toString() << " Attempting to accept connection" << descriptor;
    TcpConnection *connect = new TcpConnection();
    accept(descriptor, connect);
}

void TcpServer::accept(qintptr descriptor, TcpConnection *connection)
{
    qDebug() << this << QTime::currentTime().toString() << " Accepting the connection";
    connection->moveToThread(m_thread);
    emit accepting(descriptor, connection);
}

void TcpServer::complete()
{
    if(!m_thread)
        qWarning() << this << QTime::currentTime().toString() << " Exiting complete there was no thread";

    qDebug() << this << QTime::currentTime().toString() << " Complete called, destroy thread";

    delete m_connections;

    qDebug() << this << QTime::currentTime().toString() << " Quiting thread";

    m_thread->quit();
    m_thread->wait();

    delete m_thread;

    qDebug() << this << QTime::currentTime().toString() << " Complete";
}
