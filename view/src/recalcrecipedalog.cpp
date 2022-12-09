#include "recalcrecipedalog.h"
#include "ui_recalcrecipedalog.h"

namespace view
{

RecalcRecipeDalog::RecalcRecipeDalog(double totalWeight, QWidget *parent) :
    QDialog(parent),
    m_ui(std::make_shared<Ui::RecalcRecipeDalog>())
{
    m_ui->setupUi(this);
    setWeight(totalWeight);
}

double RecalcRecipeDalog::weight() const
{
    return m_ui->doubleSpinBox->value();
}

void RecalcRecipeDalog::setWeight(double w)
{
    m_ui->doubleSpinBox->setValue(w);
}

}
