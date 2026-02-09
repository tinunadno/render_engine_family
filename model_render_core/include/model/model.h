#pragma once

#include "material.h"
#include "model_geometry.h"
#include "utils/vec.h"

namespace mrc {

template<typename NumericT>
struct Model
{
    using Face = typename ModelGeometry<NumericT>::Face;

    ModelGeometry<NumericT> geometry;
    Material<NumericT> material;




    const std::vector<sc::utils::Vec<NumericT, 3>>& verticies() const { return geometry.verticies(); }
    std::vector<sc::utils::Vec<NumericT, 3>>& verticies() { return geometry.verticies(); }

    const std::vector<sc::utils::Vec<NumericT, 2>>& uv() const { return geometry.uv(); }
    std::vector<sc::utils::Vec<NumericT, 2>>& uv() { return geometry.uv(); }

    const std::vector<sc::utils::Vec<NumericT, 3>>& normals() const { return geometry.normals(); }
    std::vector<sc::utils::Vec<NumericT, 3>>& normals() { return geometry.normals(); }

    [[nodiscard]] const std::vector<typename ModelGeometry<NumericT>::Face>& faces() const { return geometry.faces(); }
    [[nodiscard]] std::vector<typename ModelGeometry<NumericT>::Face>& faces() { return geometry.faces(); }

    const sc::utils::Vec<NumericT, 3>& pos() const { return geometry.pos(); }
    sc::utils::Vec<NumericT, 3>& pos() { return geometry.pos(); }

    const sc::utils::Vec<NumericT, 3>& rot() const { return geometry.rot(); }
    sc::utils::Vec<NumericT, 3>& rot() { return geometry.rot(); }

    std::array<sc::utils::Vec<NumericT, 3>, 3> getPolygon(
        std::size_t faceIdx,
        const std::vector<sc::utils::Vec<NumericT, 3>>& vertSource) const
    {
        return geometry.getPolygon(faceIdx, vertSource);
    }
};

} // namespace mrc
