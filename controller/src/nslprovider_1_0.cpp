#include <QFile>
#include <QDataStream>
#include <QString>

#include "nslprovider_1_0.h"

namespace controller
{

NSLProvider_1_0::NSLProvider_1_0()
    : NSLProviderBase(1, 0)
{
}

std::shared_ptr<model::Product> NSLProvider_1_0::load(const std::string& filename, std::wstring& message)
{
    QFile file(QString::fromStdString(filename));
    if (!file.open(QIODevice::ReadOnly))
    {
        message = L"Can't open file: " + QString::fromStdString(filename).toStdWString() + L" for reading.";
        return nullptr;
    }

    QDataStream stream(&file);

    NSLHeader header;
    if (!readHeader(stream, header))
    {
        message = L"Can't read the header of " + QString::fromStdString(filename).toStdWString();
        file.close();
        return nullptr;
    }

    if (!checkHeader(header))
    {
        message = L"The header of " + QString::fromStdString(filename).toStdWString() + L" is damaged.";
        file.close();
        return nullptr;
    }

    auto result = std::make_shared<model::Product>();

    // common
    QString productName;
    stream >> productName;
    result->name = productName.toStdWString();

    // ingredients
    uint32_t numIngredients;
    stream >> numIngredients;
    result->ingredients.reserve(numIngredients);
    for (uint32_t i = 0; i < numIngredients; ++i)
    {
        model::Ingredient ingredient;
        QString name;
        stream >> name;
        ingredient.name = name.toStdWString();
        stream >> ingredient.protein >> ingredient.fat >> ingredient.carb;
        result->ingredients.push_back(ingredient);
    }

    // experiments
    uint32_t numExperiments;
    stream >> numExperiments;
    result->experiments.reserve(numExperiments);
    for (uint32_t i = 0; i < numExperiments; ++i)
    {
        model::Experiment experiment;
        QString name;
        stream >> name;
        experiment.name = name.toStdWString();
        stream.readRawData(reinterpret_cast<char*>(experiment.color.data()), experiment.color.size() * sizeof(experiment.color.front()));
        uint32_t numRecipeItems;
        stream >> numRecipeItems;
        experiment.recipe.reserve(numRecipeItems);
        for (uint32_t j = 0; j < numRecipeItems; ++j)
        {
            model::RecipeItem item;
            stream >> item.first >> item.second;
            experiment.recipe.push_back(item);
        }
        uint32_t mark;
        stream >> mark;
        experiment.mark = static_cast<model::Mark>(mark);
        QString conclusion;
        stream >> conclusion;
        experiment.conclusion = conclusion.toStdWString();
        result->experiments.push_back(experiment);
    }

    file.close();
    return result;
}

bool NSLProvider_1_0::save(const std::string& filename, std::shared_ptr<model::Product> product, std::wstring& message)
{
    QFile file(QString::fromStdString(filename));
    if (!file.open(QIODevice::WriteOnly))
    {
        message = L"Can't open file: " + QString::fromStdString(filename).toStdWString() + L" for writing.";
        return false;
    }

    QDataStream stream(&file);
    writeHeader(stream);

    // common
    stream << QString::fromStdWString(product->name);

    // ingredients
    stream << static_cast<uint32_t>(product->ingredients.size());
    for (const auto& ingredient : product->ingredients)
        stream << QString::fromStdWString(ingredient.name)
               << ingredient.protein
               << ingredient.fat
               << ingredient.carb;

    // experiments
    stream << static_cast<uint32_t>(product->experiments.size());
    for (const auto& experiment : product->experiments)
    {
        stream << QString::fromStdWString(experiment.name);
        stream.writeRawData(reinterpret_cast<const char*>(experiment.color.data()), experiment.color.size() * sizeof(experiment.color.front()));
        stream << static_cast<uint32_t>(experiment.recipe.size());
        for (const auto& recipeItem : experiment.recipe)
            stream << recipeItem.first << recipeItem.second;
        stream << static_cast<uint32_t>(experiment.mark);
        stream << QString::fromStdWString(experiment.conclusion);
    }

    file.close();
    return true;
}

}
