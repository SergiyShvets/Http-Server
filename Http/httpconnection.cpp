#include "httpconnection.h"

#include <QTime>


HttpConnection::HttpConnection(QObject *parent) : TcpConnection(parent)
{
    qDebug() << this << QTime::currentTime().toString()
             << " HTTP Connection created";

    //TEXT
    m_mime.insert("txt",  "text/plain");
    m_mime.insert("htm",  "text/html");
    m_mime.insert("html", "text/html");
    m_mime.insert("js",   "text/javascript");
    m_mime.insert("rtf",  "text/rtf");
    m_mime.insert("xml",  "text/xml");
    //IMAGES
    m_mime.insert("jpg",  "image/jpg");
    m_mime.insert("jpeg", "image/jpeg");
    m_mime.insert("gif",  "image/gif");
    m_mime.insert("png",  "image/png");
    //VIDEOS
    m_mime.insert("avi",  "video/avi");
    m_mime.insert("mpg",  "video/mpg");
    m_mime.insert("mpeg", "video/mpeg");
    m_mime.insert("mp4",  "video/mp4");
    m_mime.insert("mkv",  "video/x-matroska");
    m_mime.insert("qt",   "video/quicktime");
    m_mime.insert("webm", "video/webm");
    m_mime.insert("wmv",  "video/x-ms-wmv");
    m_mime.insert("flv",  "video/x-flv");
    //AUDIOS
    m_mime.insert("mp3",  "audio/mp3");
    m_mime.insert("wav",  "audio/vnd.wav");
    //APPS
    m_mime.insert("tar",  "application/x-tar");
    m_mime.insert("zip",  "application/zip");
    m_mime.insert("pdf",  "application/pdf");

}


HttpConnection::~HttpConnection()
{
    qDebug() << this << QTime::currentTime().toString()
             << " HTTP Connection destroyed";
}

QString HttpConnection::root()
{
    return m_root;
}

void HttpConnection::setRoot(QString path)
{
    qDebug() << this << QTime::currentTime().toString()
             << " Root set to: " << path;
    m_root = path;
}

int HttpConnection::rate()
{
    return m_rate;
}

void HttpConnection::setRate(int value)
{
    qDebug() << this << QTime::currentTime().toString()
             << " Rate set to: " << value;
    m_rate = value;
}

void HttpConnection::processGet(QByteArray data)
{
     qDebug() << this << QTime::currentTime().toString()
              << " Process GET";

     QByteArray eoh;
     eoh.append("\r\n\r\n");

     QString header = m_request.value("server_header", "");

     header.append(data);

     m_request.insert("server_header", header);

     if(header.contains(eoh)) {

         //End of header found, parse the header
          QStringList lines = header.split("\r\n");
          for(int i = 0; i < lines.count(); i++) {

              QString line = lines.at(i);

              if(i==0) {

              //First line has  3 value METHOD PATH VERSION
              QStringList items = line.split(" ");

              if(items.count() >= 1)
                  m_request.insert("http_method", items.at(0).trimmed());

              if(items.count() >= 2) {
                  // "/somedir/spmefile.txt"
                  QString path = items.at(1).trimmed();
                  path = QUrl::fromPercentEncoding(path.toLatin1());
                  m_request.insert("http_path", path);
                 }

              if(items.count() >= 3)
                  m_request.insert("http_version", items.at(2).trimmed());

              } else {

                  if(!line.contains(":") && !line.isEmpty()) {

                      qWarning() << this << QTime::currentTime().toString()
                               << " Skipping line due to missing ':' " << line;
                      continue;
                  }

                  int pos = line.indexOf(":");

                  QString key = line.mid(0, pos);
                  QString value = line.mid(pos+1);

                  m_request.insert(key.trimmed(), value.trimmed());

              }//else

          }//for

          //attempt to handle request
          handleRequest();
     }//if
}

void HttpConnection::handleRequest()
{
    qDebug() << this << QTime::currentTime().toString()
             << " Handle the request";

    QString file = m_request.value("http_path", "");

    //Strip out any directory jumps
    file = file.replace("..", "");

    // /mydir/something/something + /test.jpg
    QString actualFile = m_root + file;
    QFileInfo fi(actualFile);
    QByteArray responce;

    //If it is a directory, check for index.html
    if(fi.isDir()) {

        qDebug() << this << QTime::currentTime().toString()
                 << " Client is requesting a directory...";
        QString indexFile = actualFile + "/index.html";
        QFileInfo fIndex(indexFile);

        if(fIndex.exists()) {

            qDebug() << this << QTime::currentTime().toString()
                     << " setting /to /index.html";
            fi.setFile(indexFile);

        } else {

            qWarning() << this << QTime::currentTime().toString()
                     << " Index file is missing" << indexFile;

        }
    }

        //send the file if exist
    if(fi.exists() && fi.isFile()) {

        //YES it exists
        QString mime = getMimeType(fi.fileName());
        qDebug() << this << QTime::currentTime().toString()
                 << "Sending file: " << fi.filePath();

        m_responce.insert("code", "200");
        m_responce.insert("path", fi.filePath());

        responce.append("HTTP/1.1 200 OK\r\n");
        responce.append("Content-Type: " + mime + "\r\n");
        responce.append("Content-Lenght: " + QString::number(fi.size()) + "\r\n");
        responce.append("Connection: close\r\n");
        responce.append("\r\n");

    } else {

         m_responce.insert("code", "404");

        //NO is doesnt
        responce.append("HTTP/1.1 404 NOT FOUND\r\n");
        responce.append("Connection: close\r\n");
        responce.append("\r\n");

    }

    qDebug() << this << QTime::currentTime().toString()
             << " Writing header to socket " << file;


    m_socket->write(responce);
}

void HttpConnection::sendFile(QString file)
{

    if(!m_socket)
        return;

    qDebug() << this << QTime::currentTime().toString()
             << " Sending file" << file;


    m_file = new QFile(file, this);
    m_transfer = new RateTransfer(this);

    qDebug() << this << QTime::currentTime().toString()
             << " Created" << m_transfer;

    connect(m_transfer, &RateTransfer::started,    this, &HttpConnection::started);
    connect(m_transfer, &RateTransfer::finished,   this, &HttpConnection::finished);
    connect(m_transfer, &RateTransfer::error,      this, &HttpConnection::transferError);
    connect(m_transfer, &RateTransfer::transfered, this, &HttpConnection::transfered);

    if(!m_file->open(QFile::ReadOnly)){

        qWarning() << this << QTime::currentTime().toString()
                 << " Could not open file " << file;
        m_socket->close();
    }

    m_transfer->setSource(m_file);
    m_transfer->setDestination(m_socket);
    m_transfer->setRate(m_rate);
    m_transfer->setSize(1024);

    qDebug() << this << QTime::currentTime().toString()
             << " Starting file transfer...";

    m_responce.remove("code");
    m_transfer->start();
}

QString HttpConnection::getMimeType(QString path)
{
    int pos = path.indexOf(".");
    if(pos <= 0)
        return "application/octet-stream";

    QString ext = path.mid(pos+1).toLower();

    if(m_mime.contains(ext)) {
        return m_mime.value(ext);
    }

    return "application/octet-stream";
}

void HttpConnection::connected()
{
    qDebug() << this << QTime::currentTime().toString()
             << " Connected" << getSocket();
}

void HttpConnection::disconnected()
{
    qDebug() << this << QTime::currentTime().toString()
             << " Disconnected" << getSocket();
}

void HttpConnection::readyRead()
{
    if(!m_socket)
        return;

    qDebug() << this << QTime::currentTime().toString()
             << " Ready read" << m_socket;

    QByteArray data = m_socket->readAll();
    processGet(data);
}

void HttpConnection::bytesWritten(qint64 bytes)
{
    Q_UNUSED (bytes)
    if(!m_socket)
        return;

    QString code = m_responce.value("code", "");

    if(!code.isEmpty()){

        qDebug() << this << QTime::currentTime().toString()
                 << " Code = " << code;

    }

    if(code == "200") {

         QString file = m_responce.value("path", "");

         qDebug() << this << QTime::currentTime().toString()
                  << "  Attempting send file" << file;

         sendFile(file);

         //close the socket


    } else if (code == "404") {

        m_socket->close();
        return;
    }
}

void HttpConnection::stateChanged(QAbstractSocket::SocketState socketState)
{
    qDebug() << this << QTime::currentTime().toString()
             << " State changed: " << m_socket << socketState;
}

void HttpConnection::error(QAbstractSocket::SocketError socketError)
{
    qDebug() << this << QTime::currentTime().toString()
             << " Error: " << m_socket << socketError;
}

void HttpConnection::started()
{
    qDebug() << this << QTime::currentTime().toString()
             << " File transfer started! ";
}

void HttpConnection::transfered(qint64 bytes)
{
    qDebug() << this << QTime::currentTime().toString()
             << " File transfered. " << bytes;
}

void HttpConnection::finished()
{
    qDebug() << this << QTime::currentTime().toString()
             << " File transfer finished. ";

    m_file->close();
    m_socket->close();
}

void HttpConnection::transferError()
{
    qDebug() << this << QTime::currentTime().toString()
             << " File transfer error: " << m_transfer->errorString();

    m_file->close();
    m_socket->close();
}
