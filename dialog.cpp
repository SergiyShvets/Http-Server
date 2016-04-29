#include "dialog.h"
#include "ui_dialog.h"

#include <QDebug>
#include <QTime>
#include <QString>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    short height = 100;
    short width = 200;

    this->setWindowTitle("Server");
    this->setFixedSize(width, height);

    //by default start button is enable
    ui->buttonStart->setEnabled(true);
    ui->buttonStop->setEnabled(false);
}

Dialog::~Dialog()
{
    delete ui;
}


void Dialog::on_buttonStart_clicked()
{
    if(m_server.listen(QHostAddress::Any, ui->spinBox->value())) {

        qDebug() << this << QTime::currentTime().toString() << " Server started";
        m_server.setRate(1536000);
        m_server.setRoot("/home/devds/Документы/Qt Workspace/WebSite");
        setStarted(true);

    } else {

        qCritical() << m_server.errorString();
        setStarted(false);
    }


}

void Dialog::on_buttonStop_clicked()
{
    m_server.close();
    setStarted(false);
}



void Dialog::setStarted(bool started)
{
    if(started) {

        ui->buttonStart->setEnabled(false);
        ui->buttonStop->setEnabled(true);

    } else {

        ui->buttonStart->setEnabled(true);
        ui->buttonStop->setEnabled(false);

    }

}


