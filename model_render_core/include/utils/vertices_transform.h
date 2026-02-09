#pragma once
#include <vector>

#include "utils/vec.h"

namespace mrc::internal
{

template <typename NumericT>
std::vector<sc::utils::Vec<NumericT, 3>> transformVerticies(
    const std::vector<sc::utils::Vec<NumericT, 3>>& verticies,
    const sc::utils::Vec<NumericT, 3>& pos,
    const sc::utils::Vec<NumericT, 3>& rot)
{
    std::vector<sc::utils::Vec<NumericT, 3>> ret;
    ret.reserve(verticies.size());
    for (const auto& v : verticies)
    {
        ret.emplace_back(rotateEuler(v, rot) + pos);
    }
    return ret;
}

/// Transform normals by rotation only (no translation).
template <typename NumericT>
std::vector<sc::utils::Vec<NumericT, 3>> transformNormals(
    const std::vector<sc::utils::Vec<NumericT, 3>>& normals,
    const sc::utils::Vec<NumericT, 3>& rot)
{
    std::vector<sc::utils::Vec<NumericT, 3>> ret;
    ret.reserve(normals.size());
    for (const auto& n : normals)
    {
        ret.emplace_back(rotateEuler(n, rot));
    }
    return ret;
}

} // namespace mrc::internal
