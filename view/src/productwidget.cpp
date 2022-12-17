#include <QMessageBox>
#include <QTimer>

#include <model/types.h>
#include <model/kcal.h>
#include <controller/nslcontroller.h>
#include <controller/settings.h>

#include "productwidget.h"
#include "waitbox.h"
#include "experimentitemwidget.h"
#include "ingredientdialog.h"
#include "experimentdialog.h"
#include "recipeitemdialog.h"
#include "recalcrecipedalog.h"
#include "conclusiondialog.h"
#include "ui_productwidget.h"

namespace view
{

ProductWidget::ProductWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(std::make_shared<Ui::ProductWidget>())
{
    m_ui->setupUi(this);
    setupWidgets();

    connect(&controller::NSLController::instance(), &controller::NSLController::productChanged, this, &ProductWidget::updateUi);
    connect(m_ui->productTabWidget, &QTabWidget::currentChanged, [this](int idx) {
        m_cachedTabsStates[m_productTabCurrentIndex] = saveWidgetsStates();
        m_productTabCurrentIndex = idx;
        updateUi();
        if (auto widgetsStates = m_cachedTabsStates[m_productTabCurrentIndex])
            loadWidgetsStates(widgetsStates);
    });

    connect(m_ui->removeIngredientButton, &QAbstractButton::clicked, this, &ProductWidget::onRemoveIngredientClicked);
    connect(m_ui->addIngredientButton, &QAbstractButton::clicked, this, &ProductWidget::onAddIngredientClicked);
    connect(m_ui->changeIngredientButton, &QAbstractButton::clicked, this, &ProductWidget::onChangeIngredientClicked);
    connect(m_ui->ingredientsTable, &QTableWidget::cellDoubleClicked, this, &ProductWidget::onIngredientsTableCellDblClicked);
    connect(m_ui->ingredientsTable, &QTableWidget::itemSelectionChanged, this, &ProductWidget::onIngredientsTableSelectionChanged);

    connect(m_ui->removeExperimentButton, &QAbstractButton::clicked, this, &ProductWidget::onRemoveExperimentClicked);
    connect(m_ui->addExperimentButton, &QAbstractButton::clicked, this, &ProductWidget::onAddExperimentClicked);
    connect(m_ui->changeExperimentButton, &QAbstractButton::clicked, this, &ProductWidget::onChangeExperimenClicked);
    connect(m_ui->experimentsList, &QListWidget::itemSelectionChanged, this, &ProductWidget::onExperimentsListSelectionChanged);
    connect(m_ui->markBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ProductWidget::onExperimentMarkChanged);

    connect(m_ui->removeFromRecipeButton, &QAbstractButton::clicked, this, &ProductWidget::onRemoveFromRecipeClicked);
    connect(m_ui->addToRecipeButton, &QAbstractButton::clicked, this, &ProductWidget::onAddToRecipeClicked);
    connect(m_ui->changeInRecipeButton, &QAbstractButton::clicked, this, &ProductWidget::onChangeInRecipeClicked);
    connect(m_ui->recalcInRecipeButton, &QAbstractButton::clicked, this, &ProductWidget::onRecalcInRecipeClicked);
    connect(m_ui->recipeIngredientsTable, &QTableWidget::cellDoubleClicked, this, &ProductWidget::onRecipeIngredientsTableCellDblClicked);
    connect(m_ui->recipeIngredientsTable, &QTableWidget::itemSelectionChanged, this, &ProductWidget::onRecipeIngredientsTableSelectionChanged);

    connect(m_ui->changeConclusionButton, &QAbstractButton::clicked, this, &ProductWidget::onChangeConclusionClicked);
    connect(m_ui->conclusionBrowser, &TextBrowser::doubleClicked, this, &ProductWidget::onChangeConclusionClicked);

    updateUi();
}

void ProductWidget::showWaitBox()
{
    closeWaitBox();

    auto waitBox = findChild<WaitBox*>(QString(), Qt::FindDirectChildrenOnly);
    if (!waitBox)
        waitBox = new WaitBox(this);

    waitBox->exec();
}

void ProductWidget::closeWaitBox()
{
    auto waitBox = findChild<WaitBox*>(QString(), Qt::FindDirectChildrenOnly);

    if (waitBox)
    {
        waitBox->accept();
        delete waitBox;
    }
}

void ProductWidget::onRemoveIngredientClicked()
{
    auto& ctrl = controller::NSLController::instance();
    const auto& product = ctrl.product();
    auto list = m_ui->ingredientsTable->selectionModel()->selectedRows();

    QString messageBoxText = "Do you want to remove the following ingredients: {\n";
    for (const auto& item : list)
        messageBoxText.append("\t").append(QString::fromStdWString(product->ingredients[item.row()].name)).append("\n");
    messageBoxText.append("}");

    if (QMessageBox::question(this, "", messageBoxText) != QMessageBox::Yes)
        return;

    std::vector<uint32_t> affectedExperiments;
    for (uint32_t experimentIndex = 0; experimentIndex < product->experiments.size(); ++experimentIndex)
    {
        const auto& experiment = product->experiments[experimentIndex];
        for (const auto& recipeItem : experiment.recipe)
        {
            bool found = false;
            for (const auto& selIngredientItem : list)
                if (recipeItem.first == selIngredientItem.row())
                {
                    affectedExperiments.push_back(experimentIndex);
                    found = true;
                    break;
                }
            if (found)
                break;
        }
    }
    if (!affectedExperiments.empty())
    {
        messageBoxText= "Can't remove the ingredient because it's included in the following experiments {\n";
        for (auto experimentIndex : affectedExperiments)
            messageBoxText.append("\t").append(QString::fromStdWString(product->experiments[experimentIndex].name)).append("\n");
        messageBoxText.append("}");

        QMessageBox::warning(this, "", messageBoxText);
        return;
    }

    auto widgetsStaes = saveWidgetsStates();

    QTimer::singleShot(0, [this, &ctrl, list, widgetsStaes]() {
        for (auto iter = list.rbegin(); iter != list.rend(); ++iter)
            ctrl.removeIngredient(iter->row());
        closeWaitBox();
        loadWidgetsStates(widgetsStaes);
    });

    showWaitBox();
}

void ProductWidget::onAddIngredientClicked()
{
    model::Ingredient ingredient{L"", 0., 0., 0.};
    if (showIngredientDialog(-1, "Add ingredient", ingredient))
    {
        auto& ctrl = controller::NSLController::instance();

        auto widgetsStaes = saveWidgetsStates();
        widgetsStaes->ingredientsTableSelectedRows = {ctrl.product()->ingredients.size()};

        QTimer::singleShot(0, [this, &ctrl, ingredient, widgetsStaes]() {
            ctrl.addIngredient(ingredient);
            closeWaitBox();
            loadWidgetsStates(widgetsStaes);
        });

        showWaitBox();
    }
}

void ProductWidget::onChangeIngredientClicked()
{
    onIngredientsTableCellDblClicked(m_ui->ingredientsTable->selectionModel()->selectedRows().first().row(), -1);
}

void ProductWidget::onIngredientsTableSelectionChanged()
{
    m_ui->removeIngredientButton->setEnabled(!m_ui->ingredientsTable->selectionModel()->selectedRows().empty());
    m_ui->changeIngredientButton->setEnabled(m_ui->ingredientsTable->selectionModel()->selectedRows().size() == 1);
}

void ProductWidget::onIngredientsTableCellDblClicked(int row, int)
{
    model::Ingredient changedIngredient;
    if (showIngredientDialog(row, "Change ingredient", changedIngredient))
    {
        auto& ctrl = controller::NSLController::instance();

        auto widgetsStates = saveWidgetsStates();

        QTimer::singleShot(0, [this, &ctrl, row, changedIngredient, widgetsStates]() {
            ctrl.changeIngredient(row, changedIngredient);
            closeWaitBox();
            loadWidgetsStates(widgetsStates);
        });

        showWaitBox();
    }
}

void ProductWidget::onRemoveExperimentClicked()
{
    auto& ctrl = controller::NSLController::instance();
    const auto& product = ctrl.product();
    auto list = m_ui->experimentsList->selectionModel()->selectedRows();

    QString messageBoxText = "Do you want to remove the following experiments: {\n";
    for (const auto& item : list)
        messageBoxText.append("\t").append(QString::fromStdWString(product->experiments[item.row()].name)).append("\n");
    messageBoxText.append("}");

    if (QMessageBox::question(this, "", messageBoxText) != QMessageBox::Yes)
        return;

    auto widgetsStates = saveWidgetsStates();

    QTimer::singleShot(0, [this, &ctrl, list, widgetsStates]() {
        for (auto iter = list.rbegin(); iter != list.rend(); ++iter)
            ctrl.removeExperiment(iter->row());
        closeWaitBox();
        loadWidgetsStates(widgetsStates);
    });

    showWaitBox();
}

void ProductWidget::onAddExperimentClicked()
{
    model::Experiment experiment{L"", model::Color({0,0,0}), {}};
    if (showExperimentDialog(-1, "Add experiment", experiment))
    {
        auto& ctrl = controller::NSLController::instance();

        auto widgetsStates = saveWidgetsStates();
        widgetsStates->experimentsListSelectedRows = {ctrl.product()->experiments.size()};

        QTimer::singleShot(0, [this, &ctrl, experiment, widgetsStates]() {
            ctrl.addExperiment(experiment);
            closeWaitBox();
            loadWidgetsStates(widgetsStates);
        });

        showWaitBox();
    }
}

void ProductWidget::onChangeExperimenClicked()
{
    model::Experiment changedExperiment;
    int row = currentExperimentIndex();
    if (showExperimentDialog(row, "Change exoeriment", changedExperiment))
    {
        auto& ctrl = controller::NSLController::instance();

        auto widgetsStates = saveWidgetsStates();

        QTimer::singleShot(0, [this, &ctrl, row, changedExperiment, widgetsStates]() {
            ctrl.changeExperiment(row, changedExperiment);
            closeWaitBox();
            loadWidgetsStates(widgetsStates);
        });

        showWaitBox();
    }
}

void ProductWidget::onExperimentsListSelectionChanged()
{
    auto selectionList = m_ui->experimentsList->selectionModel()->selectedRows();

    m_ui->removeExperimentButton->setEnabled(!selectionList.empty());
    m_ui->changeExperimentButton->setEnabled(selectionList.size() == 1);

    if (selectionList.size() == 1)
    {
        m_ui->experimentScrollArea->setVisible(true);
        updateExperimentAreaUi();
    }
    else
        m_ui->experimentScrollArea->setVisible(false);
}

void ProductWidget::onExperimentMarkChanged(int markIndex)
{
    auto& ctrl = controller::NSLController::instance();
    int row = currentExperimentIndex();
    auto changedExperiment = ctrl.product()->experiments[row];
    changedExperiment.mark = static_cast<model::Mark>(markIndex);

    auto widgetsStates = saveWidgetsStates();

    QTimer::singleShot(0, [this, &ctrl, row, changedExperiment, widgetsStates]() {
        ctrl.changeExperiment(row, changedExperiment);
        closeWaitBox();
        loadWidgetsStates(widgetsStates);
    });

    showWaitBox();
}

void ProductWidget::onRemoveFromRecipeClicked()
{
    auto& ctrl = controller::NSLController::instance();
    const auto& product = ctrl.product();

    int curExperimentIndex = currentExperimentIndex();
    auto list = m_ui->recipeIngredientsTable->selectionModel()->selectedRows();

    QString messageBoxText = "Do you want to remove the following recipe items: {\n";
    for (const auto& item : list)
        messageBoxText.
                append("\t").
                append(QString::fromStdWString(product->ingredients[product->experiments[curExperimentIndex].recipe[item.row()].first].name)).
                append("\n");
    messageBoxText.append("}");

    if (QMessageBox::question(this, "", messageBoxText) != QMessageBox::Yes)
        return;

    auto widgetsStates = saveWidgetsStates();

    QTimer::singleShot(0, [this, &ctrl, list, curExperimentIndex, widgetsStates]() {
        for (auto iter = list.rbegin(); iter != list.rend(); ++iter)
            ctrl.removeFromRecipe(curExperimentIndex, iter->row());
        closeWaitBox();
        loadWidgetsStates(widgetsStates);
    });

    showWaitBox();
}

void ProductWidget::onAddToRecipeClicked()
{
    auto& ctrl = controller::NSLController::instance();
    const auto& product = ctrl.product();

    model::RecipeItem recipeItem = std::make_pair(0, 0.);
    if (showRecipeItemDialog(-1, "Add recipe item", recipeItem))
    {
        int curExperimentIndex = currentExperimentIndex();
        const auto& currentExperiment = product->experiments[curExperimentIndex];

        int recipeItemIndex = -1;
        for (int i = 0; i < currentExperiment.recipe.size(); ++i)
        {
            const auto& item = currentExperiment.recipe[i];
            if (recipeItem.first == item.first)
            {
                recipeItemIndex = i;
                break;
            }
        }

        if (recipeItemIndex != -1)
        {
            QMessageBox::warning(this, "",
                                 QString::fromStdWString(product->ingredients[recipeItem.first].name) + " is already included to the recipe.");
        }
        else
        {
            auto widgetsStates = saveWidgetsStates();
            widgetsStates->experimentRecipeTableSelectedRows = { product->experiments[curExperimentIndex].recipe.size() };

            QTimer::singleShot(0, [this, &ctrl, curExperimentIndex, recipeItem, widgetsStates]() {
                ctrl.addToRecipe(curExperimentIndex, recipeItem);
                closeWaitBox();
                loadWidgetsStates(widgetsStates);
            });

            showWaitBox();
        }
    }
}

void ProductWidget::onChangeInRecipeClicked()
{
    onRecipeIngredientsTableCellDblClicked(m_ui->recipeIngredientsTable->selectionModel()->selectedRows().first().row(), 0);
}

void ProductWidget::onRecalcInRecipeClicked()
{
    auto& ctrl = controller::NSLController::instance();
    const auto& product = ctrl.product();

    model::Recipe changedRecipe;
    if (showRecalcRecipeDialog("Recalculate recipe", changedRecipe))
    {
        auto widgetsStates = saveWidgetsStates();

        QTimer::singleShot(0, [this, &ctrl, changedRecipe, widgetsStates]() {
            ctrl.changeRecipe(currentExperimentIndex(), changedRecipe);
            closeWaitBox();
            loadWidgetsStates(widgetsStates);
        });

        showWaitBox();
    }
}

void ProductWidget::onRecipeIngredientsTableSelectionChanged()
{
    m_ui->removeFromRecipeButton->setEnabled(!m_ui->recipeIngredientsTable->selectionModel()->selectedRows().empty());
    m_ui->changeInRecipeButton->setEnabled(m_ui->recipeIngredientsTable->selectionModel()->selectedRows().size() == 1);
}

void ProductWidget::onRecipeIngredientsTableCellDblClicked(int row, int)
{
    auto& ctrl = controller::NSLController::instance();
    const auto& product = ctrl.product();

    int curExperimentIndex = currentExperimentIndex();

    if (row >= product->experiments[curExperimentIndex].recipe.size())
        return;

    model::RecipeItem changedRecipeItem;
    if (showRecipeItemDialog(row, "Change recipe item", changedRecipeItem))
    {
        auto widgetsStates = saveWidgetsStates();

        QTimer::singleShot(0, [this, &ctrl, curExperimentIndex, row, changedRecipeItem, widgetsStates]() {
            ctrl.changeInRecipe(curExperimentIndex, row, changedRecipeItem);
            closeWaitBox();
            loadWidgetsStates(widgetsStates);
        });

        showWaitBox();
    }
}

void ProductWidget::onChangeConclusionClicked()
{   
    int curExperimentIndex = currentExperimentIndex();

    std::wstring changedConclusion;
    if (showConclusionDialog("Change conclusion", changedConclusion))
    {
        auto& ctrl = controller::NSLController::instance();

        auto widgetsStates = saveWidgetsStates();

        QTimer::singleShot(0, [this, &ctrl, curExperimentIndex, changedConclusion, widgetsStates]() {
            ctrl.changeConclusion(curExperimentIndex, changedConclusion);
            closeWaitBox();
            loadWidgetsStates(widgetsStates);
        });

        showWaitBox();
    }
}

void ProductWidget::updateUi()
{
    auto currentTabIndex = m_ui->productTabWidget->currentIndex();

    auto& ctrl = controller::NSLController::instance();
    auto product = ctrl.product();

    if (currentTabIndex == ingredientsTabIndex())
    {
        while (m_ui->ingredientsTable->rowCount())
            m_ui->ingredientsTable->removeRow(0);

        for (int i = 0; i < product->ingredients.size(); ++i)
        {
            const auto& ingredient = product->ingredients[i];
            m_ui->ingredientsTable->insertRow(i);

            m_ui->ingredientsTable->setItem(i, 0, new QTableWidgetItem(QString::fromStdWString(ingredient.name)));
            m_ui->ingredientsTable->setItem(i, 1, new QTableWidgetItem(QString::number(ingredient.protein)));
            m_ui->ingredientsTable->setItem(i, 2, new QTableWidgetItem(QString::number(ingredient.fat)));
            m_ui->ingredientsTable->setItem(i, 3, new QTableWidgetItem(QString::number(ingredient.carb)));
            m_ui->ingredientsTable->setItem(i, 4, new QTableWidgetItem(QString::number(model::calculateKcal(ingredient.protein, ingredient.fat, ingredient.carb))));
        }

        m_ui->ingredientsTable->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);

        onIngredientsTableSelectionChanged();
    }
    else if (currentTabIndex == experimentsTabIndex())
    {
        m_ui->experimentsList->clear();
        for (const auto& experiment : product->experiments)
        {
            auto listWidgetItem = new QListWidgetItem;
            m_ui->experimentsList->addItem(listWidgetItem);
            m_ui->experimentsList->setItemWidget(listWidgetItem, new ExperimentItemWidget(experiment, this));
        }

        onExperimentsListSelectionChanged();
    }
}

void ProductWidget::updateExperimentAreaUi()
{
    auto& ctrl = controller::NSLController::instance();
    auto product = ctrl.product();
    const auto& experiment = product->experiments[currentExperimentIndex()];

    const int headlineSize = 15;

    m_ui->experimentName->setText(
                "<html><head/><body><p><span style=\"font-size:" + QString::number(headlineSize) + "pt;\">" +
                QString::fromStdWString(experiment.name) +
                "</span></p></body></html>");

    QPixmap colorPixmap(headlineSize, headlineSize);
    colorPixmap.fill(experiment.color);
    m_ui->experimentColor->setPixmap(colorPixmap);

    m_ui->markBox->setCurrentIndex(static_cast<int>(experiment.mark));

    while (m_ui->recipeIngredientsTable->rowCount())
        m_ui->recipeIngredientsTable->removeRow(0);

    double totalWeight = 0., totalProtein = 0., totalFat = 0., totalCarb = 0.;
    for (const auto& ingredient : experiment.recipe)
        totalWeight += ingredient.second;

    for (int i = 0; i < experiment.recipe.size(); ++i)
    {
        const auto& ingredientIndexWeight = experiment.recipe[i];
        const auto& ingredient = product->ingredients[ingredientIndexWeight.first];

        m_ui->recipeIngredientsTable->insertRow(i);

        m_ui->recipeIngredientsTable->setItem(i, 0, new QTableWidgetItem(QString::fromStdWString(ingredient.name)));
        m_ui->recipeIngredientsTable->setItem(i, 1, new QTableWidgetItem(QString::number(ingredientIndexWeight.second)));

        int persentage = (totalWeight > 0.) ?
                    static_cast<int>(ingredientIndexWeight.second / totalWeight * 100. + .5) :
                    0;
        m_ui->recipeIngredientsTable->setItem(i, 2, new QTableWidgetItem(QString::number(persentage) + "%"));

        double protein = ingredient.protein / 100. * ingredientIndexWeight.second;
        totalProtein += protein;
        m_ui->recipeIngredientsTable->setItem(i, 3, new QTableWidgetItem(QString::number(protein)));

        double fat = ingredient.fat / 100. * ingredientIndexWeight.second;
        totalFat += fat;
        m_ui->recipeIngredientsTable->setItem(i, 4, new QTableWidgetItem(QString::number(fat)));

        double carb = ingredient.carb / 100. * ingredientIndexWeight.second;
        totalCarb += carb;
        m_ui->recipeIngredientsTable->setItem(i, 5, new QTableWidgetItem(QString::number(carb)));

        double kcal = model::calculateKcal(protein, fat, carb);
        m_ui->recipeIngredientsTable->setItem(i, 6, new QTableWidgetItem(QString::number(kcal)));
    }

    std::array<QString, 7> totalItemNames {
        "Total",
        QString::number(totalWeight),
        "100%",
        QString::number(totalProtein),
        QString::number(totalFat),
        QString::number(totalCarb),
        QString::number(model::calculateKcal(totalProtein, totalFat, totalCarb))
    };

    QFont totalItemFont;
    totalItemFont.setBold(true);

    m_ui->recipeIngredientsTable->insertRow(experiment.recipe.size());
    for (int i = 0; i < totalItemNames.size(); ++i)
    {
        auto item = new QTableWidgetItem(totalItemNames[i]);
        item->setFlags(item->flags().setFlag(Qt::ItemFlag::ItemIsSelectable, false));
        item->setFont(totalItemFont);
        m_ui->recipeIngredientsTable->setItem(experiment.recipe.size(), i, item);
    }

    if (totalWeight > 0. && std::abs(totalWeight - 100.) > 1e-2)
    {
        const double protein100g = totalProtein / totalWeight * 100.;
        const double fat100g = totalFat / totalWeight * 100.;
        const double carb100g = totalCarb / totalWeight * 100.;

        totalItemNames = {
            "Total / 100g",
            "100",
            "100%",
            QString::number(protein100g),
            QString::number(fat100g),
            QString::number(carb100g),
            QString::number(model::calculateKcal(protein100g, fat100g, carb100g)),
        };

        m_ui->recipeIngredientsTable->insertRow(experiment.recipe.size() + 1);
        for (int i = 0; i < totalItemNames.size(); ++i)
        {
            auto item = new QTableWidgetItem(totalItemNames[i]);
            item->setFlags(item->flags().setFlag(Qt::ItemFlag::ItemIsSelectable, false));
            item->setFont(totalItemFont);
            m_ui->recipeIngredientsTable->setItem(experiment.recipe.size() + 1, i, item);
        }
    }

    m_ui->recipeIngredientsTable->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);

    onRecipeIngredientsTableSelectionChanged();

    m_ui->conclusionBrowser->setHtml(QString::fromStdWString(experiment.conclusion));
}

bool ProductWidget::showIngredientDialog(int ingredientIndex, const QString &title, model::Ingredient& resultIngredient) const
{   
    auto& ctrl = controller::NSLController::instance();
    const auto& product = ctrl.product();

    model::Ingredient ingredient = ingredientIndex == -1 ?
                model::Ingredient{L"", 0., 0., 0.0} :
                product->ingredients[ingredientIndex];

    IngredientDialog ingredientWidget(ingredient, const_cast<ProductWidget*>(this));
    ingredientWidget.setWindowTitle(title);
    ingredientWidget.adjustSize();

    bool result = false;
    if (ingredientWidget.exec() == QDialog::Accepted)
    {
        QString newName = ingredientWidget.name();
        while (!newName.isEmpty() && newName[0].isSpace()) newName.remove(0, 1);
        while (!newName.isEmpty() && newName[newName.size()-1].isSpace()) newName.remove(newName.size()-1, 1);

        bool isNameEmpty = newName.isEmpty();
        bool isNameContained = false;
        for (int i = 0; i < product->ingredients.size(); ++i)
        {
            if (i == ingredientIndex) continue;

            if (QString::fromStdWString(product->ingredients[i].name) == newName)
            {
                isNameContained = true;
                break;
            }
        }

        if (isNameEmpty || isNameContained)
        {
            QString messageBoxText;
            if (isNameEmpty) messageBoxText = "Ingredient name can't be empty.";
            if (isNameContained) messageBoxText = "Ingredient " + newName + " is already contained in the list.";

            QMessageBox::warning(&ingredientWidget, "", messageBoxText);
            result = showIngredientDialog(ingredientIndex, title, resultIngredient);
        }
        else
        {
            resultIngredient.name = newName.toStdWString();
            resultIngredient.protein = ingredientWidget.protein();
            resultIngredient.fat = ingredientWidget.fat();
            resultIngredient.carb = ingredientWidget.carb();
            result = true;
        }
    }

    return result;
}

bool ProductWidget::showExperimentDialog(int experimentIndex, const QString& title, model::Experiment &resultExperiment) const
{
    auto& ctrl = controller::NSLController::instance();
    const auto& product = ctrl.product();

    QColor initColor = experimentIndex == -1 ?
                QColor(0, 0, 0) :
                product->experiments[experimentIndex].color;
    QString initName = experimentIndex == -1 ?
                "Experiment #" + QString::number(product->experiments.size() + 1) :
                QString::fromStdWString(product->experiments[experimentIndex].name);

    ExperimentDialog experimentWidget(initColor, initName, const_cast<ProductWidget*>(this));
    experimentWidget.setWindowTitle(title);
    experimentWidget.adjustSize();

    bool result = false;
    if (experimentWidget.exec() == QDialog::Accepted)
    {
        QString newName = experimentWidget.name();
        while (!newName.isEmpty() && newName[0].isSpace()) newName.remove(0, 1);
        while (!newName.isEmpty() && newName[newName.size()-1].isSpace()) newName.remove(newName.size()-1, 1);

        bool isNameEmpty = newName.isEmpty();
        bool isNameContained = false;
        for (int i = 0; i < product->experiments.size(); ++i)
        {
            if (i == experimentIndex) continue;

            if (QString::fromStdWString(product->experiments[i].name) == newName)
            {
                isNameContained = true;
                break;
            }
        }

        if (isNameEmpty || isNameContained)
        {
            QString messageBoxText;
            if (isNameEmpty) messageBoxText = "Experiment name can't be empty.";
            if (isNameContained) messageBoxText = "Experiment " + newName + " is already contained in the list.";

            QMessageBox::warning(&experimentWidget, "", messageBoxText);
            result = showExperimentDialog(experimentIndex, title, resultExperiment);
        }
        else
        {
            if (experimentIndex != -1)
                resultExperiment = product->experiments[experimentIndex];
            resultExperiment.name = newName.toStdWString();
            resultExperiment.color = model::Color::fromQColor(experimentWidget.color());
            result = true;
        }
    }

    return result;
}

bool ProductWidget::showRecipeItemDialog(int recipeItemIndex, const QString& title, model::RecipeItem& resultRecipeItem) const
{
    auto& ctrl = controller::NSLController::instance();
    const auto& product = ctrl.product();

    model::RecipeItem recipeItem = recipeItemIndex == -1 ?
                model::RecipeItem(0, 0.) :
                product->experiments[currentExperimentIndex()].recipe[recipeItemIndex];

    RecipeItemDialog recipeItemWidget(product->ingredients, recipeItem, const_cast<ProductWidget*>(this));
    recipeItemWidget.setWindowTitle(title);
    recipeItemWidget.adjustSize();

    bool result = false;
    if (recipeItemWidget.exec() == QDialog::Accepted)
    {
        if (recipeItemIndex != -1)
            resultRecipeItem = recipeItem;
        resultRecipeItem.first = recipeItemWidget.ingredientIndex();
        resultRecipeItem.second = recipeItemWidget.weight();
        result = true;
    }

    return result;
}

bool ProductWidget::showRecalcRecipeDialog(const QString& title, model::Recipe& resultRecipe) const
{
    auto& ctrl = controller::NSLController::instance();
    const auto& product = ctrl.product();

    const model::Recipe& recipe = product->experiments[currentExperimentIndex()].recipe;

    double totalWeight = 0.;
    for (const auto& item : recipe)
        totalWeight += item.second;

    if (totalWeight == 0.)
    {
        QMessageBox::warning(const_cast<ProductWidget*>(this), "", "The original weight of the product must be greater than zero for recalculation.");
        return false;
    }

    RecalcRecipeDalog recalcWidget(totalWeight, const_cast<ProductWidget*>(this));
    recalcWidget.setWindowTitle(title);
    recalcWidget.adjustSize();

    bool result = false;
    if (recalcWidget.exec() == QDialog::Accepted)
    {
        if (QMessageBox::question(&recalcWidget,
                                  "",
                                  "Are you sure you want to recalculate the weight of the ingredients?\nYou will have lost the previous values.")
                == QMessageBox::Yes)
        {
            const double coef = recalcWidget.weight() / totalWeight;

            resultRecipe = recipe;
            for (auto& item : resultRecipe)
                item.second *= coef;

            result = true;
        }
    }

    return result;
}

bool ProductWidget::showConclusionDialog(const QString& title, std::wstring& resultText) const
{
    auto& ctrl = controller::NSLController::instance();
    const auto& product = ctrl.product();

    ConclusionDialog conclusionDialog(QString::fromStdWString(product->experiments[currentExperimentIndex()].conclusion), const_cast<ProductWidget*>(this));
    conclusionDialog.setWindowTitle(title);

    bool result = false;
    if (conclusionDialog.exec() == QDialog::Accepted)
    {
        resultText = conclusionDialog.text().toStdWString();
        result = true;
    }

    return result;
}

int ProductWidget::ingredientsTabIndex() const
{
    return m_ui->productTabWidget->indexOf(m_ui->ingredientsTab);
}

int ProductWidget::experimentsTabIndex() const
{
    return m_ui->productTabWidget->indexOf(m_ui->experimentsTab);
}

int ProductWidget::currentExperimentIndex() const
{
    return m_ui->experimentsList->currentRow();
}

void ProductWidget::setupWidgets()
{
    m_cachedTabsStates.resize(m_ui->productTabWidget->count());

    m_ui->markBox->addItem("No mark", static_cast<int>(model::Mark::NoMark));
    m_ui->markBox->addItem(QIcon(":/res/like.png"), "", static_cast<int>(model::Mark::Like));
    m_ui->markBox->addItem(QIcon(QPixmap(":/res/like.png").transformed(QTransform::fromScale(1., -1.))), "", static_cast<int>(model::Mark::Dislike));

    m_productTabCurrentIndex = ingredientsTabIndex();
    m_ui->productTabWidget->setCurrentIndex(m_productTabCurrentIndex);

    m_ui->experimentColor->setVisible(controller::Settings::instance().experimentShowColor());

    m_ui->markLabel->setVisible(controller::Settings::instance().experimentShowMark());
    m_ui->markBox->setVisible(controller::Settings::instance().experimentShowMark());

    m_ui->ingredientsTable->horizontalHeader()->setSectionHidden(4, !controller::Settings::instance().showKcalInIngredients());
    m_ui->recipeIngredientsTable->horizontalHeader()->setSectionHidden(6, !controller::Settings::instance().showKcalInExperiments());
}

std::shared_ptr<ProductWidget::CachedWidgetsStates> ProductWidget::saveWidgetsStates() const
{
    auto widgetsStates = std::make_shared<CachedWidgetsStates>();

    // ingredients table
    widgetsStates->ingredientsTableSelectedRows.clear();
    for (const auto& item : m_ui->ingredientsTable->selectionModel()->selectedRows())
        widgetsStates->ingredientsTableSelectedRows.push_back(item.row());

    // experiments lsit
    widgetsStates->experimentsListSelectedRows.clear();
    for (const auto& item : m_ui->experimentsList->selectionModel()->selectedRows())
        widgetsStates->experimentsListSelectedRows.push_back(item.row());

    // recipe table
    widgetsStates->experimentRecipeTableSelectedRows.clear();
    for (const auto& item : m_ui->recipeIngredientsTable->selectionModel()->selectedRows())
        widgetsStates->experimentRecipeTableSelectedRows.push_back(item.row());

    return widgetsStates;
}

void ProductWidget::loadWidgetsStates(std::shared_ptr<CachedWidgetsStates> widgetsStates)
{
    // ingredients table
    m_ui->ingredientsTable->selectionModel()->clear();
    m_ui->ingredientsTable->setSelectionMode(QAbstractItemView::MultiSelection);
    for (auto row : widgetsStates->ingredientsTableSelectedRows)
        m_ui->ingredientsTable->selectRow(row);
    m_ui->ingredientsTable->setSelectionMode(QAbstractItemView::ExtendedSelection);

    // experiments lsit
    m_ui->experimentsList->selectionModel()->clear();
    m_ui->experimentsList->setSelectionMode(QAbstractItemView::MultiSelection);
    for (auto row : widgetsStates->experimentsListSelectedRows)
        m_ui->experimentsList->setCurrentRow(row);
    m_ui->experimentsList->setSelectionMode(QAbstractItemView::ExtendedSelection);

    // ingredients table
    m_ui->recipeIngredientsTable->selectionModel()->clear();
    m_ui->recipeIngredientsTable->setSelectionMode(QAbstractItemView::MultiSelection);
    for (auto row : widgetsStates->experimentRecipeTableSelectedRows)
        m_ui->recipeIngredientsTable->selectRow(row);
    m_ui->recipeIngredientsTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

}
