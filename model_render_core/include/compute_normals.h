#pragma once

#include "utils/vec.h"
#include "model.h"

namespace mrc {
template<typename NumericT>
sc::utils::Vec<NumericT, 3> getFaceNormal(const Model<NumericT> &model, std::size_t faceIdx) {
    const auto &v0 = model.verticies()[model.faces()[faceIdx][0][0]];
    const auto &v1 = model.verticies()[model.faces()[faceIdx][1][0]];
    const auto &v2 = model.verticies()[model.faces()[faceIdx][2][0]];
    return sc::utils::norm(sc::utils::cross(v1 - v0, v2 - v0));
}
} // namespace mrc
