#pragma once

#include "object/iobject.h"

#include <memory>


namespace rmc::internal
{

template<typename NumericT>
struct SdfResult
{
    const object::SceneObject<NumericT>* closestObject = nullptr;
    NumericT distance;
};

} // namespace rmc::internal
