#pragma once

#include "utils/vec.h"

namespace sc::internal
{

template<typename NumericT>
class CameraView
{
public:
    using Vec3 = utils::Vec<NumericT, 3>;

    Vec3 origin;
    Vec3 forward;
    Vec3 right;
    Vec3 up;

    Vec3 pixelOrigin;
    Vec3 stepX;
    Vec3 stepY;

    std::size_t width  = 0;
    std::size_t height = 0;

    class Iterator;
    Iterator begin() const { return Iterator(this, 0); }
    Iterator end() const { return Iterator(this, width * height); }
};

} // namespace sc::internal