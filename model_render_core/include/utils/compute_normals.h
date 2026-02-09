#pragma once

#include "utils/vec.h"
#include "../model/model.h"

namespace mrc {

/// Compute face normal from model (object-space vertices).
template<typename NumericT>
sc::utils::Vec<NumericT, 3> getFaceNormal(const Model<NumericT>& model, std::size_t faceIdx) {
    const auto& v0 = model.verticies()[model.faces()[faceIdx][0][0]];
    const auto& v1 = model.verticies()[model.faces()[faceIdx][1][0]];
    const auto& v2 = model.verticies()[model.faces()[faceIdx][2][0]];
    return sc::utils::norm(sc::utils::cross(v1 - v0, v2 - v0));
}

/// Compute face normal from already-transformed (world-space) vertices.
template<typename NumericT>
sc::utils::Vec<NumericT, 3> getFaceNormal(
    const std::vector<sc::utils::Vec<NumericT, 3>>& transformedVerts,
    const typename ModelGeometry<NumericT>::Face& face)
{
    const auto& v0 = transformedVerts[face[0][0]];
    const auto& v1 = transformedVerts[face[1][0]];
    const auto& v2 = transformedVerts[face[2][0]];
    return sc::utils::norm(sc::utils::cross(v1 - v0, v2 - v0));
}

} // namespace mrc
