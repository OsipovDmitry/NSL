#ifndef PRODUCTWIDGET_H
#define PRODUCTWIDGET_H

#include <memory>

#include <QWidget>

#include <model/forwarddecl.h>

namespace Ui {
class ProductWidget;
}

namespace view
{

class ProductWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProductWidget(QWidget *parent = 0);

private:
    void showWaitBox();
    void closeWaitBox();

    void onRemoveIngredientClicked();
    void onAddIngredientClicked();
    void onChangeIngredientClicked();
    void onIngredientsTableSelectionChanged();
    void onIngredientsTableCellDblClicked(int, int);

    void onRemoveExperimentClicked();
    void onAddExperimentClicked();
    void onChangeExperimenClicked();
    void onExperimentsListSelectionChanged();
    void onExperimentMarkChanged(int);

    void onRemoveFromRecipeClicked();
    void onAddToRecipeClicked();
    void onChangeInRecipeClicked();
    void onRecalcInRecipeClicked();
    void onRecipeIngredientsTableSelectionChanged();
    void onRecipeIngredientsTableCellDblClicked(int, int);

    void onChangeConclusionClicked();

    void updateUi();
    void updateExperimentAreaUi();
    bool showIngredientDialog(int, const QString&, model::Ingredient&) const;
    bool showExperimentDialog(int, const QString&, model::Experiment&) const;
    bool showRecipeItemDialog(int, const QString&, model::RecipeItem&) const;
    bool showRecalcRecipeDialog(const QString&, model::Recipe&) const;
    bool showConclusionDialog(const QString&, std::wstring&) const;

    int ingredientsTabIndex() const;
    int experimentsTabIndex() const;
    int currentExperimentIndex() const;

    void setupWidgets();

    struct CachedWidgetsStates;
    std::shared_ptr<CachedWidgetsStates> saveWidgetsStates() const;
    void loadWidgetsStates(std::shared_ptr<CachedWidgetsStates>);

    std::shared_ptr<Ui::ProductWidget> m_ui;
    int m_productTabCurrentIndex;

    struct CachedWidgetsStates
    {
        std::vector<int> ingredientsTableSelectedRows;
        std::vector<int> experimentsListSelectedRows;
        std::vector<int> experimentRecipeTableSelectedRows;
    };
    std::vector<std::shared_ptr<CachedWidgetsStates>> m_cachedTabsStates;
};

}

#endif // PRODUCTWIDGET_H
