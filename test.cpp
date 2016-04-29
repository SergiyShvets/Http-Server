#include "test.h"

#include <QDebug>
#include <QTime>

Test::Test(QObject *parent) : QObject(parent)
{

}

Test::~Test()
{

}

void Test::doTest()
{
    m_source.setFileName("/home/devds/Документы/Qt Workspace/WebServer.tar.gz");
    m_destination.setFileName("/home/devds/Документы/Qt Workspace/WebZaga.tar.gz");
    m_transfer.setSource(&m_source);
    m_transfer.setDestination(&m_destination);
    m_transfer.setRate(2046);
    m_transfer.setSize(250);

    if(!m_source.open(QFile::ReadOnly)) {

        qCritical() << this << QTime::currentTime().toString()
                << " Could not open the source";
            return;
    }

    if(!m_destination.open(QFile::WriteOnly)) {

        qCritical() << this << QTime::currentTime().toString()
                << " Could not open the destination";

        m_source.close();
            return;
    }


    connect(&m_transfer, &RateTransfer::started,    this, &Test::started);
    connect(&m_transfer, &RateTransfer::transfered, this, &Test::transfered);
    connect(&m_transfer, &RateTransfer::finished,   this, &Test::finished);
    connect(&m_transfer, &RateTransfer::error,      this, &Test::error);

    m_transfer.start();

}

void Test::started()
{
    qDebug() << this << QTime::currentTime().toString()
            << " Transfer started";
}

void Test::transfered(int bytes)
{
    qDebug() << this << QTime::currentTime().toString()
              << " Transfered" << bytes;
}

void Test::finished()
{
    qDebug() << this << QTime::currentTime().toString()
             << " Finished";

    m_source.close();
    m_destination.close();
}

void Test::error()
{
    qDebug() << this << QTime::currentTime().toString()
            << " Error = " << m_transfer.errorString();
}
