#ifndef TYPES_H
#define TYPES_H

#include <string>

#include <model/color.h>
#include <model/forwarddecl.h>

namespace model
{

struct Ingredient
{
    std::wstring name;
    double protein;
    double fat;
    double carb;
};

enum class Mark : uint32_t { NoMark = 0, Like = 1, Dislike = 2};

struct Experiment
{
    std::wstring name;
    Color color;
    Recipe recipe;
    Mark mark;
    std::wstring conclusion;
};

struct Product
{
    std::wstring name;
    IngredientsList ingredients;
    ExperimentsList experiments;
};

}

#endif // TYPES_H
