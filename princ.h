// CC-BY Edouard.Thiel@univ-amu.fr - 22/01/2019

#ifndef PRINC_H
#define PRINC_H

#include "ui_princ.h"

class Princ : public QMainWindow, private Ui::Princ
{
    Q_OBJECT

public:
    explicit Princ(QWidget *parent = 0);



public slots:
    void setSliderRadius(double radius);
    void paramStartButton();
    void paramStopButton();
    void paramSliderChanged(int value);

protected slots:
    void onSliderRadius(int value);

private slots:
    void on_MoteurButton_clicked();
    void on_actionCommandes_triggered();
};

#endif // PRINC_H
