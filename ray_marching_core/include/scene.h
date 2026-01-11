#pragma once

#include "utils/vec.h"
#include "object/iobject.h"
#include "internal.h"
#include "curvature/icurvature.h"

#include <vector>
#include <memory>

namespace rmc
{

template<typename NumericT>
using LightSource = sc::utils::Vec<NumericT, 3>;

template<typename NumericT>
class Scene
{
public:
    using ObjectPtr = object::SceneObject<NumericT>;
    using CurvPtr = std::shared_ptr<curvature::ICurvature<NumericT>>;
    using objectList = std::vector<ObjectPtr>;
    using curvList = std::vector<CurvPtr>;

    void sdf(const sc::utils::Vec<NumericT,3>& p, internal::SdfResult<NumericT>& out) const
    {
        out.distance = std::numeric_limits<NumericT>::max();
        out.closestObject = nullptr;

        for (const auto& obj : _objects)
        {
            NumericT d = obj.shape->sdf(p);
            if (d < out.distance)
            {
                out.distance = d;
                out.closestObject = &obj;
            }
        }
    }

    void applyCurvature(const sc::utils::Vec<NumericT,3>& pos, sc::utils::Vec<NumericT,3>& dir, NumericT& stepSize) const
    {
        for (const auto& curv : _curvatures)
        {
            curv->deflect(pos, dir, stepSize);
        }
    }

    void addObject(const ObjectPtr& oPtr)
    {
        _objects.push_back(oPtr);
    }

    void addCurvature(const CurvPtr& cPtr)
    {
        _curvatures.push_back(cPtr);
    }

private:
    objectList _objects;
    curvList _curvatures;
};

} // namespace