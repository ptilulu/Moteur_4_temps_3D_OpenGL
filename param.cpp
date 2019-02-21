#include "param.h"
#include "ui_param.h"
#include <QDebug>

param::param(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::param)
{
    ui->setupUi(this);
}

param::~param()
{
    delete ui;
}

void param::on_StartButton_clicked()
{
    qDebug() << "Start depuis param";
    emit StartbuttonPush();
}

void param::on_StopButton_clicked()
{
    qDebug() << "Stop depuis param";
    emit StopbuttonPush();
}

void param::on_AccelSlider_valueChanged(int value)
{
    qDebug() << "Slider depuis param";
    emit SliderAccelChanged(value);
}
