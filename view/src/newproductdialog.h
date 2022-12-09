#ifndef NEWPRODUCTDIALOG_H
#define NEWPRODUCTDIALOG_H

#include <memory>

#include <QDialog>

namespace Ui {
class NewProductDialog;
}

namespace view
{

class NewProductDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewProductDialog(const QString&, QWidget *parent = 0);

    void setProductName(const QString&);
    QString productName() const;

private:
    std::shared_ptr<Ui::NewProductDialog> m_ui;
};

}

#endif // NEWPRODUCTDIALOG_H
