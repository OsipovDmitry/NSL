#include <controller/nslcontroller.h>

#include "nslprovider_1_0.h"

namespace controller
{

NSLController& NSLController::instance()
{
    static NSLController controller;
    return controller;
}

void NSLController::registerNSLProvider(std::shared_ptr<INSLProvider> provider)
{
    auto version = provider->version();

    if (m_providers.count(version))
    {
        emit critical(L"Provider [" + std::to_wstring(version.first) + L"," + std::to_wstring(version.second) + L"is alread registered");
        return;
    }

    m_providers.insert({version, provider});
}

std::shared_ptr<const model::Product> NSLController::product() const
{
    return m_product;
}

const std::string &NSLController::productFilename() const
{
    return m_productFilename;
}

bool NSLController::isThereUnsavedChanges() const
{
    return m_unsavedChanges;
}

void NSLController::onNewFile(const std::wstring& productName)
{
    m_product = std::make_shared<model::Product>();
    m_product->name = productName;
    m_productFilename.clear();
    m_unsavedChanges = true; // product file has been created and already has the product name!

    emit fileChanged();
}

void NSLController::onOpenFile(const std::string& filename)
{
    NSLHeader header;
    if (!NSLProviderBase::loadHeader(filename, header))
    {
        emit error(L"Can't read the file " + QString::fromStdString(filename).toStdWString());
        return;
    }

    if (!m_providers.count(header.version))
    {
        emit error(L"Provider [" + std::to_wstring(header.version.first) + L"." + std::to_wstring(header.version.second) + L"] is not registered.");
        return;
    }

    std::wstring errorMessage;
    auto product = m_providers.at(header.version)->load(filename, errorMessage);
    if (!product)
    {
        emit error(errorMessage);
        return;
    }

    m_product = product;
    m_productFilename = filename;
    m_unsavedChanges = false;

    emit fileChanged();

}

void NSLController::onSaveFile(const std::string& filename)
{
    if (!m_product)
    {
        emit error(L"No product is loaded.");
        return;
    }

    if (m_providers.empty())
    {
        emit error(L"No provider is registered.");
        return;
    }

    std::wstring errorMessage;
    if (!m_providers.rbegin()->second->save(filename, m_product, errorMessage))
    {
        emit error(errorMessage);
        return;
    }

    m_unsavedChanges = false;
    emit fileChanged();
}

void NSLController::onCloseFile()
{
    m_product = nullptr;
    m_productFilename.clear();
    m_unsavedChanges = false;


    emit fileChanged();
}

void NSLController::removeIngredient(uint32_t ingredientIndex)
{
    if (!m_product)
    {
        error(L"The product has not been loaded.");
        return;
    }

    if (ingredientIndex >= m_product->ingredients.size())
    {
        error(L"The product doesn't have ingredient #" + std::to_wstring(ingredientIndex));
        return;
    }

    m_product->ingredients.erase(m_product->ingredients.begin() + ingredientIndex);
    m_unsavedChanges = true;
    emit productChanged();
}

void NSLController::addIngredient(const model::Ingredient& newIngredient)
{
    if (!m_product)
    {
        error(L"The product has not been loaded.");
        return;
    }

    m_product->ingredients.push_back(newIngredient);
    m_unsavedChanges = true;
    emit productChanged();
}

void NSLController::changeIngredient(uint32_t ingredientIndex, const model::Ingredient& newIngredient)
{
    if (!m_product)
    {
        error(L"The product has not been loaded.");
        return;
    }

    if (ingredientIndex >= m_product->ingredients.size())
    {
        error(L"The product doesn't have ingredient #" + std::to_wstring(ingredientIndex));
        return;
    }

    m_product->ingredients[ingredientIndex] = newIngredient;
    m_unsavedChanges = true;
    emit productChanged();
}

void NSLController::removeExperiment(uint32_t experimentIndex)
{
    if (!m_product)
    {
        error(L"The product has not been loaded.");
        return;
    }

    if (experimentIndex >= m_product->experiments.size())
    {
        error(L"The product doesn't have experiement #" + std::to_wstring(experimentIndex));
        return;
    }

    m_product->experiments.erase(m_product->experiments.begin() + experimentIndex);
    m_unsavedChanges = true;
    emit productChanged();
}

void NSLController::addExperiment(const model::Experiment& newExperiment)
{
    if (!m_product)
    {
        error(L"The product has not been loaded.");
        return;
    }

    m_product->experiments.push_back(newExperiment);
    m_unsavedChanges = true;
    emit productChanged();
}

void NSLController::changeExperiment(uint32_t experimentIndex, const model::Experiment& newExperiment)
{
    if (!m_product)
    {
        error(L"The product has not been loaded.");
        return;
    }

    if (experimentIndex >= m_product->experiments.size())
    {
        error(L"The product doesn't have experiment #" + std::to_wstring(experimentIndex));
        return;
    }

    m_product->experiments[experimentIndex] = newExperiment;
    m_unsavedChanges = true;
    emit productChanged();
}

void NSLController::removeFromRecipe(uint32_t experimentIndex, uint32_t recipeItemIndex)
{
    if (!m_product)
    {
        error(L"The product has not been loaded.");
        return;
    }

    if (experimentIndex >= m_product->experiments.size())
    {
        error(L"The product doesn't have experiement #" + std::to_wstring(experimentIndex));
        return;
    }

    if (recipeItemIndex >= m_product->experiments[experimentIndex].recipe.size())
    {
        error(L"The experiment doesn't have recipe item #" + std::to_wstring(recipeItemIndex));
        return;
    }

    m_product->experiments[experimentIndex].recipe.erase(m_product->experiments[experimentIndex].recipe.begin() + recipeItemIndex);
    m_unsavedChanges = true;
    emit productChanged();
}

void NSLController::addToRecipe(uint32_t experimentIndex, const model::RecipeItem& newRecipeItem)
{
    if (!m_product)
    {
        error(L"The product has not been loaded.");
        return;
    }

    if (experimentIndex >= m_product->experiments.size())
    {
        error(L"The product doesn't have experiement #" + std::to_wstring(experimentIndex));
        return;
    }

    m_product->experiments[experimentIndex].recipe.push_back(newRecipeItem);
    m_unsavedChanges = true;
    emit productChanged();
}

void NSLController::changeInRecipe(uint32_t experimentIndex, uint32_t recipeItemIndex, const model::RecipeItem& newRecipeItem)
{
    if (!m_product)
    {
        error(L"The product has not been loaded.");
        return;
    }

    if (experimentIndex >= m_product->experiments.size())
    {
        error(L"The product doesn't have experiment #" + std::to_wstring(experimentIndex));
        return;
    }

    if (recipeItemIndex >= m_product->experiments[experimentIndex].recipe.size())
    {
        error(L"The experiment doesn't have recipe item #" + std::to_wstring(recipeItemIndex));
        return;
    }

    m_product->experiments[experimentIndex].recipe[recipeItemIndex] = newRecipeItem;
    m_unsavedChanges = true;
    emit productChanged();
}

void NSLController::changeRecipe(uint32_t experimentIndex, const model::Recipe& newRecipe)
{
    if (!m_product)
    {
        error(L"The product has not been loaded.");
        return;
    }

    if (experimentIndex >= m_product->experiments.size())
    {
        error(L"The product doesn't have experiment #" + std::to_wstring(experimentIndex));
        return;
    }


    m_product->experiments[experimentIndex].recipe = newRecipe;
    m_unsavedChanges = true;
    emit productChanged();
}

void NSLController::changeConclusion(uint32_t experimentIndex, const std::wstring& newConclusion)
{
    if (!m_product)
    {
        error(L"The product has not been loaded.");
        return;
    }

    if (experimentIndex >= m_product->experiments.size())
    {
        error(L"The product doesn't have experiment #" + std::to_wstring(experimentIndex));
        return;
    }

    m_product->experiments[experimentIndex].conclusion = newConclusion;
    m_unsavedChanges = true;
    emit productChanged();
}

NSLController::NSLController()
    : QObject()
    , m_product()
    , m_productFilename()
    , m_unsavedChanges(false)
{
    registerNSLProvider(std::make_shared<NSLProvider_1_0>());

//        {
//            auto product = std::make_shared<model::Product>();
//            product->name = L"Молоко сгущеное с сахарозаменителем";

//            product->ingredients.push_back(model::Ingredient{L"Молоко сухое 26%", 23.8, 26, 38.7}); // 0
//            product->ingredients.push_back(model::Ingredient{L"Молоко сухое 1,5%", 32, 1.5, 54}); // 1
//            product->ingredients.push_back(model::Ingredient{L"Масло сливочное 82,5%", 0.6, 82.5, 0.8}); // 2
//            product->ingredients.push_back(model::Ingredient{L"Вода", 0, 0, 0}); // 3
//            product->ingredients.push_back(model::Ingredient{L"Крахмал кукурузный", 0, 0, 85}); // 4

//            product->experiments.push_back(model::Experiment{L"Test1", model::Color({127, 127, 255}), {{0, 50.}, {2, 13.}, {3, 23.}}});
//            product->experiments.push_back(model::Experiment{L"Test2", model::Color({255, 0, 127}), {{1, 50.}, {2, 13.}, {3, 33.}}});
//            product->experiments.push_back(model::Experiment{L"Test3", model::Color({255, 127, 0}), {{0, 25.}, {1, 25.}, {3, 50.}}});
//            product->experiments.push_back(model::Experiment{L"Test4", model::Color({255, 255, 0}), {{0, 25.}, {1, 25.}, {3, 31.}}});
//            product->experiments.push_back(model::Experiment{L"Test5", model::Color({0, 255, 255}), {{0, 50.}, {3, 100.}, {4, 10.}}});
//            product->experiments.push_back(model::Experiment{L"Test6", model::Color({0, 0, 255}), {{0, 75.}, {3, 187.5}, {4, 15.}}});

//            std::wstring message;
//            NSLProvider_1_0().save("123.nsl", product, message);
//    }
}

}
