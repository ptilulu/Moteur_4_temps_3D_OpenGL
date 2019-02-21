#ifndef PARAM_H
#define PARAM_H

#include <QDialog>

namespace Ui {
class param;
}

class param : public QDialog
{
    Q_OBJECT

public:
    explicit param(QWidget *parent = nullptr);
    ~param();

private slots:
    void on_StartButton_clicked();

    void on_StopButton_clicked();

    void on_AccelSlider_valueChanged(int value);

signals:
    void StartbuttonPush();
    void StopbuttonPush();
    void SliderAccelChanged(int value);

private:
    Ui::param *ui;
};

#endif // PARAM_H
