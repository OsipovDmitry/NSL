#ifndef MODEL_FORWARD_H
#define MODEL_FORWARD_H

#include <vector>

namespace model
{

struct Color;

struct Ingredient;
using IngredientsList = std::vector<Ingredient>;

using RecipeItem = std::pair<uint32_t, double>; // <ingredient index, weight>
using Recipe = std::vector<RecipeItem>;

enum class Mark : uint32_t;

struct Experiment;
using ExperimentsList = std::vector<Experiment>;

struct Product;

}

#endif // MODEL_FORWARD_H
