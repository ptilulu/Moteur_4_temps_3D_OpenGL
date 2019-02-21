#ifndef AIDEBUTTON_H
#define AIDEBUTTON_H

#include <QDialog>

namespace Ui {
class AideButton;
}

class AideButton : public QDialog
{
    Q_OBJECT

public:
    explicit AideButton(QWidget *parent = nullptr);
    ~AideButton();

private:
    Ui::AideButton *ui;
};

#endif // AIDEBUTTON_H
