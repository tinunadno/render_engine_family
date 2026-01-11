#pragma once

#include "ray_marching_params.h"
#include "ray_marching_result.h"
#include "scene.h"
#include "utils/ray.h"

namespace rmc
{


template<typename NumericT>
RayMarchingResult<NumericT> marchRay(
    const sc::utils::Ray<NumericT, 3>& ray,
    const Scene<NumericT>& scene,
    const RayMarchingParams<NumericT>& params = {
        NumericT(1e-6), NumericT(80), NumericT(100)
    })
{
    RayMarchingResult<NumericT> ret{};
    internal::SdfResult<NumericT> sdfResult;

    sc::utils::Vec<NumericT, 3> pos = ray.pos();
    sc::utils::Vec<NumericT, 3> dir = sc::utils::norm(ray.rot());

    NumericT totalTraveled = 0;

    for (int i = 0; i < params.maxIterations; ++i)
    {
        scene.sdf(pos, sdfResult);
        NumericT currentStep = sdfResult.distance;
        if (totalTraveled > params.maxDistance) break;
        if (currentStep <= params.threshold)
        {
            ret.finalPosition = pos;
            ret.normal = sdfResult.closestObject->shape->normal(pos);
            ret.distance = totalTraveled;
            ret.material = sdfResult.closestObject->material;
            ret.reachedThreshold = true;
            return ret;
        }
        scene.applyCurvature(pos, dir, currentStep);
        pos += dir * currentStep;
        totalTraveled += currentStep;
    }

    ret.reachedThreshold = false;
    ret.distance = totalTraveled;
    return ret;
}


} // namespace rmc