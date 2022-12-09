#include "conclusiondialog.h"
#include "ui_conclusiondialog.h"

namespace view
{

ConclusionDialog::ConclusionDialog(const QString &text, QWidget *parent) :
    QDialog(parent),
    m_ui(std::make_shared<Ui::ConclusionDialog>())
{
    m_ui->setupUi(this);

    setText(text);
}

QString ConclusionDialog::text() const
{
    return m_ui->textEdit->text();
}

void ConclusionDialog::setText(const QString &text)
{
    m_ui->textEdit->setText(text);
}

}
