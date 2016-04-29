#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "tcpconnection.h"
#include "tcpconnections.h"

#include <QObject>
#include <QTcpServer>
#include <QDebug>
#include <QThread>

class TcpServer : public QTcpServer
{
    Q_OBJECT

public:

    explicit TcpServer(/*QTcpServer*/ QObject *parent = 0);
    ~TcpServer();

    virtual bool listen(const QHostAddress &address, quint16 port);
    virtual void close();
    virtual quint64 port();

protected:

    QThread *m_thread;
    TcpConnections *m_connections;
    virtual void incomingConnection(qintptr descriptor);
    virtual void accept(qintptr descriptor, TcpConnection *connection);

signals:

    void accepting(qintptr handle, TcpConnection *connection);
    void finished();

public slots:

    void complete();
};

#endif // TCPSERVER_H
