#pragma once

#include "shading/material.h"
#include "utils/vec.h"

namespace rmc
{

template<typename NumericT>
struct RayMarchingResult
{
    sc::utils::Vec<NumericT, 3> finalPosition;
    sc::utils::Vec<NumericT, 3> normal;
    shader::Material material;
    NumericT distance;
    bool reachedThreshold;
};

} // namespace rmc