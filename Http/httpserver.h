#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "tcpserver.h"
#include "httpconnection.h"

#include <QString>
#include <QDebug>
#include <QObject>
#include <QCoreApplication>


class HttpServer : public TcpServer
{
    Q_OBJECT

public:

    explicit HttpServer(QObject *parent = 0);
    ~HttpServer();

    QString root();
    void setRoot(QString path);
    int rate();
    void setRate(int value);

protected:

    QString m_root;
    int m_rate;
    virtual void incomingConnection(qintptr descriptor);

};

#endif // HTTPSERVER_H
