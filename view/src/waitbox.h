#ifndef WAITBOX_H
#define WAITBOX_H

#include <memory>

#include <QDialog>

namespace Ui {
class WaitBox;
}

namespace view
{

class WaitBox : public QDialog
{
    Q_OBJECT

public:
    explicit WaitBox(QWidget *parent = 0);

private:
    std::shared_ptr<Ui::WaitBox> m_ui;
};

}

#endif // WAITBOX_H
