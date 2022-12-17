#ifndef ADDINGREDIENTWIDGET_H
#define ADDINGREDIENTWIDGET_H

#include <memory>

#include <QDialog>

#include <model/forwarddecl.h>

namespace Ui {
class IngredientDialog;
}

namespace view
{

class IngredientDialog : public QDialog
{
    Q_OBJECT

public:
    IngredientDialog(const model::Ingredient&, QWidget *parent = 0);

    QString name() const;
    double protein() const;
    double fat() const;
    double carb() const;

private:
    void recalculateKcal(double);

    std::shared_ptr<Ui::IngredientDialog> m_ui;
};

}

#endif // ADDINGREDIENTWIDGET_H
