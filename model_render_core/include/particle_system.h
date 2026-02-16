#pragma once

#include "utils/vec.h"

#include <vector>
#include <array>
#include <cmath>

namespace mrc
{

/// SoA particle system for efficient screen-space impostor rendering.
/// All particles share the same billboard geometry (e.g. a circle
/// approximated by triangles).  Per-particle data is stored in
/// parallel arrays for cache-friendly traversal.
template<typename NumericT>
struct ParticleSystem
{
    // Per-particle data (SoA)
    std::vector<sc::utils::Vec<NumericT, 3>> positions;   // world-space centres
    std::vector<sc::utils::Vec<float, 3>>    colors;      // RGB [0,1]
    std::vector<NumericT>                     sizes;       // world-space radius

    // Unit-space template vertices in [-1,1]x[-1,1].
    // At render time each vertex is transformed:
    // worldPos = centre + right * v.x * size + up * v.y * size
    std::vector<sc::utils::Vec<NumericT, 2>> billboardVerts;
    std::vector<std::array<std::size_t, 3>>  billboardFaces;

    [[nodiscard]] std::size_t count() const { return positions.size(); }
};

/// Generate a simple camera-facing quad (2 triangles).
template<typename NumericT>
void makeQuadBillboard(ParticleSystem<NumericT>& ps)
{
    ps.billboardVerts = {
        {NumericT(-1), NumericT(-1)},
        {NumericT( 1), NumericT(-1)},
        {NumericT( 1), NumericT( 1)},
        {NumericT(-1), NumericT( 1)},
    };
    ps.billboardFaces = {
        {0, 1, 2},
        {0, 2, 3},
    };
}

/// Generate a circle approximation using @p segments triangles
/// arranged as a triangle fan around the origin.
template<typename NumericT>
void makeCircleBillboard(ParticleSystem<NumericT>& ps, int segments = 8)
{
    ps.billboardVerts.clear();
    ps.billboardFaces.clear();

    // Centre vertex
    ps.billboardVerts.push_back(sc::utils::Vec<NumericT, 2>{NumericT(0), NumericT(0)});

    const NumericT twoPi = NumericT(2) * NumericT(M_PI);
    for (int i = 0; i <= segments; ++i)
    {
        NumericT angle = twoPi * NumericT(i) / NumericT(segments);
        ps.billboardVerts.push_back(sc::utils::Vec<NumericT, 2>{
            std::cos(angle),
            std::sin(angle)
        });
    }

    for (int i = 1; i <= segments; ++i)
    {
        ps.billboardFaces.push_back({
            0,
            static_cast<std::size_t>(i),
            static_cast<std::size_t>(i + 1)
        });
    }
}

} // namespace mrc
