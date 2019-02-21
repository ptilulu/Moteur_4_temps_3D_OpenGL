#include "aidebutton.h"
#include "ui_aidebutton.h"

AideButton::AideButton(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AideButton)
{
    ui->setupUi(this);
}

AideButton::~AideButton()
{
    delete ui;
}
