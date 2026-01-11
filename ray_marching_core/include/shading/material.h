#pragma once

#include "utils/vec.h"

namespace rmc::shader
{

class Material
{
public:

    Material()
        : color(sc::utils::Vec<float, 3>{1.f, 0.f, 0.f})
    { }

    Material(const sc::utils::Vec<float, 3>& color_)
        : color(color_)
    { }

    sc::utils::Vec<float, 3> color;
};

} // namespace rmc::material