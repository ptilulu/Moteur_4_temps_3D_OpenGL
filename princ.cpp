// CC-BY Edouard.Thiel@univ-amu.fr - 22/01/2019

#include "princ.h"
#include "param.h"
#include <QDebug>

Princ::Princ(QWidget *parent) :
    QMainWindow(parent)
{
    setupUi(this);

    connect (glarea, SIGNAL(radiusChanged(double)), this, SLOT(setSliderRadius(double)));
    connect (sli_radius, SIGNAL(valueChanged(int)), this, SLOT(onSliderRadius(int)));
}


void Princ::setSliderRadius(double radius)
{
    qDebug() << __FUNCTION__ << radius << sender();
    int value = radius*20;
    if (sli_radius->value() != value) {
        qDebug() << "  sli_radius->setvalue()";
        sli_radius->setValue(value);
    }
}

void Princ::onSliderRadius(int value)
{
    qDebug() << __FUNCTION__ << value << sender();
    qDebug() << "  emit radiusChanged()";
    emit glarea->radiusChanged(value/20.0);
}

void Princ::paramStartButton(){
    qDebug() << "emit paramStartButton";
    glarea->m_timer->start();
}

void Princ::paramStopButton(){
    qDebug() << "emit paramStopButton";
    glarea->m_timer->stop();
}

void Princ::paramSliderChanged(int value){
    qDebug() << "emit paramSliderChanged";
    glarea->m_timer->setInterval(value/10000);
    qDebug() << "emit paramSliderChanged start";
}


void Princ::on_MoteurButton_clicked()
{
    param p;
    connect(&p, SIGNAL(StartbuttonPush()), this, SLOT(paramStartButton()));
    connect(&p, SIGNAL(StopbuttonPush()), this, SLOT(paramStopButton()));
    connect(&p, SIGNAL(SliderAccelChanged(int)), this, SLOT(paramSliderChanged(int)));
    if(p.exec()){

    }

}
