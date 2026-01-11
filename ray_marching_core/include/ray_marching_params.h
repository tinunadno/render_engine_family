#pragma once

namespace rmc
{

template<typename NumericT>
struct RayMarchingParams
{
    NumericT threshold;
    NumericT maxIterations;
    NumericT maxDistance;
};

} // namespace rmc