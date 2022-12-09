#include <QColorDialog>

#include "experimentdialog.h"
#include "ui_experimentdialog.h"

namespace view
{

ExperimentDialog::ExperimentDialog(const QColor &color, const QString& name, QWidget *parent) :
    QDialog(parent),
    m_ui(std::make_shared<Ui::ExperimentDialog>())
{
    m_ui->setupUi(this);
    setColor(color);
    setName(name);

    connect(m_ui->colorButton, &QPushButton::clicked, this, &ExperimentDialog::onColorButtonClicked);
}

QColor ExperimentDialog::color() const
{
    return m_ui->colorButton->icon().pixmap(1, 1).toImage().pixel(0,0);
}

void ExperimentDialog::setColor(const QColor &color)
{
    QPixmap pixmap(16, 16);
    pixmap.fill(color);

    m_ui->colorButton->setIcon(QIcon(pixmap));
}

QString ExperimentDialog::name() const
{
    return m_ui->nameEdit->text();
}

void ExperimentDialog::setName(const QString& name)
{
    m_ui->nameEdit->setText(name);
}

void ExperimentDialog::onColorButtonClicked()
{
    auto chosenColor = QColorDialog::getColor(color(), this, "", QColorDialog::DontUseNativeDialog);
    if (chosenColor.isValid())
        setColor(chosenColor);
}

}
