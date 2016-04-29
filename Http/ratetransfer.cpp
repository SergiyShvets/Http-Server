#include "ratetransfer.h"

RateTransfer::RateTransfer(QObject *parent) : QObject(parent)
{
    qDebug() << this << QTime::currentTime().toString()
             << " Transfer created";
    setDefaults();
}

RateTransfer::~RateTransfer()
{
    qDebug() << this << QTime::currentTime().toString()
             << " Transfer destroyed";
}

int RateTransfer::rate()
{
    return m_rate;
}

void RateTransfer::setRate(int value)
{
    m_rate = value;
    qDebug() << this << QTime::currentTime().toString()
             << " Rate set to" << value;
}

int RateTransfer::size()
{
    return m_size;
}

void RateTransfer::setSize(int value)
{
    m_size = value;
    qDebug() << this << QTime::currentTime().toString()
             << " Size set to" << value;
}

qint64 RateTransfer::maximum()
{
    return m_maximum;
}

void RateTransfer::setMaximum(qint64 value)
{
    m_maximum = value;
    qDebug() << this << QTime::currentTime().toString()
             << " Maximum set to" << value;
}

QIODevice *RateTransfer::source()
{
    return m_source;
}

void RateTransfer::setSource(QIODevice *device)
{
    m_source = device;
    qDebug() << this << QTime::currentTime().toString()
             << " Source set to " << device;

    if(m_source->isSequential())
        connect(m_source, &QIODevice::readyRead, this, &RateTransfer::readyRead);
}

QIODevice *RateTransfer::destination()
{
    return m_destination;
}

void RateTransfer::setDestination(QIODevice *device)
{
    m_destination = device;
    qDebug() << this << QTime::currentTime().toString()
             << " Destination set to " << device;

    if(m_source->isSequential())
        connect(m_destination, &QIODevice::bytesWritten, this, &RateTransfer::bytesWritten);
}

bool RateTransfer::isTransfering()
{
    return m_transfering;
}

QString RateTransfer::errorString()
{
    return m_error;
}

void RateTransfer::setDefaults()
{
    qDebug() << this << QTime::currentTime().toString()
             << " Setting the deffaults";

    m_rate = 0;
    m_size = 1024;
    m_maximum = 0;
    m_transfered = 0;
    m_source = 0;
    m_destination = 0;
    m_transfering = false;
    m_scheduled = false;
    m_timer.setInterval(5);
    m_error = "";
}

bool RateTransfer::checkDevices()
{
    if(!m_source) {
        m_transfering = false;
        m_error = "No source device";
        qDebug() << this << QTime::currentTime().toString()
                 << m_error;
        emit error();
        return false;
    }

    if(!m_destination) {
        m_transfering = false;
        m_error = "No destination device";
        qDebug() << this << QTime::currentTime().toString()
                 << m_error;
        emit error();
        return false;
    }

    if(!m_source->isOpen() || !m_source->isReadable()) {
        m_transfering = false;
        m_error = "Source device is not open or readable!";
        qDebug() << this << QTime::currentTime().toString()
                 << m_error;
        emit error();
        return false;
    }

    if(!m_destination->isOpen() || !m_destination->isWritable()) {
        m_transfering = false;
        m_error = "Destination device is not open or writable!";
        qDebug() << this << QTime::currentTime().toString()
                 << m_error;
        emit error();
        return false;
    }

    return true;
}

bool RateTransfer::checkTransfer()
{
    if(!m_transfering) {

        m_error = "No transfering, aborting!";
        qDebug() << this << QTime::currentTime().toString()
                 << m_error;
        emit error();
        return false;
    }

    if(m_transfered >= m_rate) {

        m_error = "Rat exeeded, not allowed to transfer!";
        qDebug() << this << QTime::currentTime().toString()
                 << m_error;
        emit error();
        return false;
    }

    return true;
}

void RateTransfer::scheduleTransfer()
{
    qDebug() << this << QTime::currentTime().toString()
             << "Schedule Transfer called";

    if(m_scheduled) {
        qWarning() << this << QTime::currentTime().toString()
                 << "Exeting schedule transfer due to: waiting in timer";
        return;
    }

    if(!m_transfering) {
        qWarning() << this << QTime::currentTime().toString()
                 << "Exeting schedule transfer due to: not transfering";
        return;
    }

    if(m_source->bytesAvailable() <= 0) {
        qWarning() << this << QTime::currentTime().toString()
                 << "Exeting schedule transfer due to: no bytes available to be read";
        return;
    }

    int prediction = m_transfered + m_size;

    if(prediction <= m_rate) {
        qDebug() << this << QTime::currentTime().toString()
                 << "Calling transfer from schedule transfer";
        transfer();

    } else {

        int current = QTime::currentTime().msec();
        int delay = 1000 - current;

        qDebug() << this << QTime::currentTime().toString()
                 << " Rate limit(" << m_rate << ")"
                 << " exeeded in prediction (" << m_transfered
                 << " to" << prediction << ") delaying transfer for "
                 << delay << "ms";

        m_transfered = 0;
        m_scheduled = true;
        m_timer.singleShot(delay, this, &RateTransfer::transfer);
    }

}

void RateTransfer::start()
{
    qDebug() << this << QTime::currentTime().toString()
             << "Start called";
    if(m_transfering) {
        qDebug() << this << QTime::currentTime().toString()
                 << "Already Transfering!";
        return;
    }

    m_error = "";
    if(!checkDevices())
        return;

    m_transfering = true;
    m_transfered = 0;

    emit started();

    if(!m_source->isSequential() && m_source->bytesAvailable() > 0) {

        qDebug() << this << QTime::currentTime().toString()
                 << "Starting transfer called by Schedule Transfer";
        scheduleTransfer();
    }

}

void RateTransfer::stop()
{
    qDebug() << this << QTime::currentTime().toString()
             << "Stopping the transfer";
    m_timer.stop();
    m_transfering = false;
}

void RateTransfer::transfer()
{
    m_scheduled = false;
    qDebug() << this << QTime::currentTime().toString()
             << "Transfering at maximum of " << m_rate << "bytes per second";
    m_error = "";

    if(!checkDevices())
        return;

    if(!checkTransfer())
        return;

    qDebug() << this << QTime::currentTime().toString()
             << "Reading from source";

    QByteArray buffer;
    buffer = m_source->read(m_size);

    qDebug() << this << QTime::currentTime().toString()
             << "Writing to destination " << buffer.length();

    m_destination->write(buffer);
    m_transfered += buffer.length();
    emit transfered(m_transfered);

    if(m_maximum > 0 && m_transfered >= m_maximum) {

        qDebug() << this << QTime::currentTime().toString()
                 << "Stopping due to maximum limit reached";
        emit finished();
        stop();
    }

    if(!m_source->isSequential() && m_source->bytesAvailable() == 0) {
        qDebug() << this << QTime::currentTime().toString()
                 << "Stopping due to end of file";
        emit finished();
        stop();
    }

    if(m_transfering == false)
        return;

    if(!m_source->isSequential() && m_source->bytesAvailable() > 0) {
        qDebug() << this << QTime::currentTime().toString()
                 << "Source still has bytes, scheduling a transfer";

        scheduleTransfer();

    }
}

void RateTransfer::readyRead()
{
    qDebug() << this << QTime::currentTime().toString()
             << "Ready read signaled";
    scheduleTransfer();
}

void RateTransfer::bytesWritten(qint64 bytes)
{
    Q_UNUSED(bytes)
    qDebug() << this << QTime::currentTime().toString()
             << "Bytes written signaled";
    scheduleTransfer();
}
