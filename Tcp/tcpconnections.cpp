#include "tcpconnections.h"

#include <QTime>

TcpConnections::TcpConnections(QObject *parent) : QObject(parent)
{
   qDebug() << this << QTime::currentTime().toString() << " TCP Connections created";
}

TcpConnections::~TcpConnections()
{
    qDebug() << this << QTime::currentTime().toString() << " TCP Connections destroyed";
}

int TcpConnections::count()
{
    QReadWriteLock lock;
    lock.lockForRead();
    int value = m_connections.count();
    lock.unlock();
    return value;
}

void TcpConnections::removeSocket(QTcpSocket *socket)
{
    if(!socket) {
         qDebug() << this << QTime::currentTime().toString() << " Socket error";
         return;
    }

    if(!m_connections.contains(socket)) {
         qDebug() << this << QTime::currentTime().toString() << " Connection socket error";
         return;
    }

     qDebug() << this << QTime::currentTime().toString() << " Removeing socket: " << socket;

     if(socket->isOpen()) {
          qDebug() << this << QTime::currentTime().toString()
                   << " Socket is open, attemptin to close it" << socket;
          socket->disconnect();
          socket->close();
     }

      qDebug() << this << QTime::currentTime().toString()
               << " Deleting socket " << socket;

      m_connections.remove(socket);
      socket->deleteLater();

       qDebug() << this << QTime::currentTime().toString()
                << " Clients count: " << m_connections.count();
}

void TcpConnections::disconnected()
{
    if(!sender()) {
         qDebug() << this << QTime::currentTime().toString() << " sender error";
         return;
    }

    qDebug() << this << QTime::currentTime().toString()
             << " Disconnecting socket " << sender();

    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());

    if(!socket) {
         qDebug() << this << QTime::currentTime().toString() << " socket error";
         return;
    }

    removeSocket(socket);
}

void TcpConnections::error(QAbstractSocket::SocketError socketError)
{
    if(!sender()) {
         qDebug() << this << QTime::currentTime().toString() << " sender error";
         return;
    }

    qDebug() << this << QTime::currentTime().toString()
             << " Error in socket" << sender() << " error: "<< socketError;

    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());

    if(!socket) {
         qDebug() << this << QTime::currentTime().toString() << " socket error";
         return;
    }

    removeSocket(socket);
}

void TcpConnections::start()
{
     qDebug() << this << QTime::currentTime().toString()
              << " Connection started on: " << QThread::currentThread();

}

void TcpConnections::quit()
{
    if(!sender()) {
         qDebug() << this << QTime::currentTime().toString() << " sender error";
         return;
    }

     qDebug() << this << QTime::currentTime().toString() << " Connection quiting";

     foreach (QTcpSocket *socket, m_connections.keys()) {

          qDebug() << this << QTime::currentTime().toString()
                   << " Closing socket" << socket;

          removeSocket(socket);
     }

      qDebug() << this << QTime::currentTime().toString() << " Finshing";
      emit finished();
}

void TcpConnections::accept(qintptr handle, TcpConnection *connection)
{
    //qDebug << this << "Accept";

    QTcpSocket *socket = new QTcpSocket(this);

    if(!socket->setSocketDescriptor(handle)) {

         qWarning() << this << QTime::currentTime().toString()
                    << " Could not accept connection" << handle;

         connection->deleteLater();
         return;
    }

    connect(socket, &QTcpSocket::disconnected, this, &TcpConnections::disconnected);
    connect(socket, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error),
                                               this, &TcpConnections::error);

    connection->moveToThread(QThread::currentThread());
    connection->setSocket(socket);

    m_connections.insert(socket, connection);

     qDebug() << this << QTime::currentTime().toString()
              << " Clients =  " << m_connections.count();


     emit socket->connected();
}
