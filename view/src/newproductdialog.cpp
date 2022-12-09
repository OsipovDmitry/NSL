#include "newproductdialog.h"
#include "ui_newproductdialog.h"

namespace view
{

NewProductDialog::NewProductDialog(const QString& name, QWidget *parent) :
    QDialog(parent),
    m_ui(std::make_shared<Ui::NewProductDialog>())
{
    m_ui->setupUi(this);
    setProductName(name);
}

void NewProductDialog::setProductName(const QString& name)
{
    m_ui->nameEdit->setText(name);
}

QString NewProductDialog::productName() const
{
    return m_ui->nameEdit->text();
}

}
