#include <model/types.h>

#include "recipeitemdialog.h"
#include "ui_recipeitemdialog.h"

namespace view
{

RecipeItemDialog::RecipeItemDialog(const model::IngredientsList& ingredientsList, const model::RecipeItem& item, QWidget *parent) :
    QDialog(parent),
    m_ui(std::make_shared<Ui::RecipeItemDialog>())
{
    m_ui->setupUi(this);

    for (const auto& ingredient : ingredientsList)
        m_ui->ingredientsBox->addItem(QString::fromStdWString(ingredient.name));

    setIngredientIndex(item.first);
    setWeight(item.second);
}


int RecipeItemDialog::ingredientIndex() const
{
    return m_ui->ingredientsBox->currentIndex();
}

void RecipeItemDialog::setIngredientIndex(int index)
{
    m_ui->ingredientsBox->setCurrentIndex(index);
}

double RecipeItemDialog::weight() const
{
    return m_ui->weightBox->value();
}

void RecipeItemDialog::setWeight(double value)
{
    m_ui->weightBox->setValue(value);
}

}
