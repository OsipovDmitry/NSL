#ifndef RECIPEDIALOG_H
#define RECIPEDIALOG_H

#include <memory>

#include <QDialog>

#include <model/forwarddecl.h>

namespace Ui {
class RecipeItemDialog;
}

namespace view
{

class RecipeItemDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RecipeItemDialog(const model::IngredientsList&, const model::RecipeItem&, QWidget *parent = 0);

    int ingredientIndex() const;
    void setIngredientIndex(int);

    double weight() const;
    void setWeight(double);

private:
    std::shared_ptr<Ui::RecipeItemDialog> m_ui;
};

}

#endif // RECIPEDIALOG_H
