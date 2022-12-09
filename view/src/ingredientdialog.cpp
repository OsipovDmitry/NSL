#include <model/types.h>

#include "ingredientdialog.h"
#include "ui_Ingredientdialog.h"

namespace view
{

IngredientDialog::IngredientDialog(const model::Ingredient &ingredient, QWidget *parent):
    QDialog(parent),
    m_ui(std::make_shared<Ui::IngredientDialog>())
{
    m_ui->setupUi(this);

    m_ui->nameEdit->setText(QString::fromStdWString(ingredient.name));
    m_ui->proteinEdit->setValue(ingredient.protein);
    m_ui->fatEdit->setValue(ingredient.fat);
    m_ui->carbEdit->setValue(ingredient.carb);

    m_ui->nameEdit->setFocus();
}

QString IngredientDialog::name() const
{
    return m_ui->nameEdit->text();
}

double IngredientDialog::protein() const
{
    return m_ui->proteinEdit->value();
}

double IngredientDialog::fat() const
{
    return m_ui->fatEdit->value();
}
double IngredientDialog::carb() const
{
    return m_ui->carbEdit->value();
}

}

