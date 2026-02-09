#pragma once

#include "graphics_tools.h"

#include <vector>
#include <array>
#include <algorithm>

namespace mrc::gt
{

/// Default tile size (pixels).  32x32 tile z-buffer (4 KB for float)
/// must fit comfortably in L1 cache.
constexpr int TILE_SIZE = 32;



/// Clip a triangle against the near plane and project the result.
/// Writes 0, 1, or 2 projected triangles into @p out.
/// Returns the number of triangles written.
template<typename NumericT>
std::size_t clipAndProject(
    const std::array<internal::ClipVertex<NumericT>, 3>& clipVerts,
    const sc::Camera<NumericT, sc::VecArray>& camera,
    std::array<std::array<internal::ProjectedVertex<NumericT>, 3>, 2>& out)
{
    const auto& c0 = clipVerts[0];
    const auto& c1 = clipVerts[1];
    const auto& c2 = clipVerts[2];

    if (triangleTriviallyClipped(c0, c1, c2))
        return 0;

    const bool c0Out = c0.clip[2] < -c0.clip[3];
    const bool c1Out = c1.clip[2] < -c1.clip[3];
    const bool c2Out = c2.clip[2] < -c2.clip[3];
    const int outCount = int(c0Out) + int(c1Out) + int(c2Out);

    if (outCount == 3)
        return 0;

    if (outCount == 1)
    {
        internal::ClipVertex<NumericT> in[2];
        internal::ClipVertex<NumericT> outV;
        int idx = 0;
        if (!c0Out) in[idx++] = c0; else outV = c0;
        if (!c1Out) in[idx++] = c1; else outV = c1;
        if (!c2Out) in[idx]   = c2; else outV = c2;

        auto [t1, t2] = handleOneClipOut(in[0], in[1], outV, camera);
        out[0] = t1;
        out[1] = t2;
        return 2;
    }

    if (outCount == 2)
    {
        internal::ClipVertex<NumericT> outV[2];
        internal::ClipVertex<NumericT> inV;
        int idx = 0;
        if (c0Out) outV[idx++] = c0; else inV = c0;
        if (c1Out) outV[idx++] = c1; else inV = c1;
        if (c2Out) outV[idx]   = c2; else inV = c2;

        out[0] = handleTwoClipOut(outV[0], outV[1], inV, camera);
        return 1;
    }

    // outCount == 0 eg fully inside
    out[0] = {
        clipToProjectedVertex(c0, camera),
        clipToProjectedVertex(c1, camera),
        clipToProjectedVertex(c2, camera)
    };
    return 1;
}

template<typename NumericT, typename FragmentShader>
void rasterizeTriangleInRect(
    const std::array<internal::ProjectedVertex<NumericT>, 3>& tri,
    const FragmentShader& shader,
    internal::SceneCache<NumericT>& cache,
    int rx0, int ry0, int rx1, int ry1)
{
    auto& renderer = cache.renderer;
    auto& zBuffer  = cache.zBuffer;

    const auto& v0 = tri[0].pixel;
    const auto& v1 = tri[1].pixel;
    const auto& v2 = tri[2].pixel;

    // Bounding box clamped to clip rect
    int minX = std::max(rx0, static_cast<int>(std::min({v0[0], v1[0], v2[0]})));
    int maxX = std::min(rx1 - 1, static_cast<int>(std::max({v0[0], v1[0], v2[0]})));
    int minY = std::max(ry0, static_cast<int>(std::min({v0[1], v1[1], v2[1]})));
    int maxY = std::min(ry1 - 1, static_cast<int>(std::max({v0[1], v1[1], v2[1]})));

    if (minX > maxX || minY > maxY)
        return;

    // Signed area (2x)
    const NumericT area = edgeFunction(v0, v1, v2);
    if (area == NumericT(0))
        return;
    const NumericT invArea = NumericT(1) / area;
    const bool posArea = area > NumericT(0);

    const NumericT e0_dx = v2[1] - v1[1];
    const NumericT e0_dy = v1[0] - v2[0];
    const NumericT e1_dx = v0[1] - v2[1];
    const NumericT e1_dy = v2[0] - v0[0];
    const NumericT e2_dx = v1[1] - v0[1];
    const NumericT e2_dy = v0[0] - v1[0];

    // Perspective-correct attribute pre-division
    const auto a0 = tri[0].attr * tri[0].invW;
    const auto a1 = tri[1].attr * tri[1].invW;
    const auto a2 = tri[2].attr * tri[2].invW;

    // Initial edge values at pixel center (minX + 0.5, minY + 0.5)
    const NumericT sx = NumericT(minX) + NumericT(0.5);
    const NumericT sy = NumericT(minY) + NumericT(0.5);

    NumericT e0_row = (sx - v1[0]) * (v2[1] - v1[1]) - (sy - v1[1]) * (v2[0] - v1[0]);
    NumericT e1_row = (sx - v2[0]) * (v0[1] - v2[1]) - (sy - v2[1]) * (v0[0] - v2[0]);
    NumericT e2_row = (sx - v0[0]) * (v1[1] - v0[1]) - (sy - v0[1]) * (v1[0] - v0[0]);

    for (int y = minY; y <= maxY; ++y)
    {
        NumericT e0 = e0_row;
        NumericT e1 = e1_row;
        NumericT e2 = e2_row;
        bool entered = false;

        for (int x = minX; x <= maxX; ++x)
        {
            const bool inside = posArea
                ? (e0 >= NumericT(0) && e1 >= NumericT(0) && e2 >= NumericT(0))
                : (e0 <= NumericT(0) && e1 <= NumericT(0) && e2 <= NumericT(0));

            if (inside)
            {
                entered = true;

                const NumericT w0 = e0 * invArea;
                const NumericT w1 = e1 * invArea;
                const NumericT w2 = e2 * invArea;

                const NumericT baryInvW =
                    w0 * tri[0].invW +
                    w1 * tri[1].invW +
                    w2 * tri[2].invW;
                const NumericT z = NumericT(1) / baryInvW;

                if (z > 0 && z < zBuffer[y][x])
                {
                    auto attr = (a0 * w0 + a1 * w1 + a2 * w2) * z;

                    FragmentInput<NumericT> frag{
                        attr.uv,
                        attr.normal,
                        attr.worldPos,
                        cache.camera.pos(),
                        attr.tangent,
                        attr.bitangent,
                        z,
                        cache.lights
                    };

                    zBuffer[y][x] = z;
                    renderer.setPixel(x, y, shader(frag));
                }
            }
            else if (entered)
            {
                break;
            }

            e0 += e0_dx;
            e1 += e1_dx;
            e2 += e2_dx;
        }

        e0_row += e0_dy;
        e1_row += e1_dy;
        e2_row += e2_dy;
    }
}



/// Bin projected triangles into screen-space tiles then rasterize
/// each tile. Processing one tile at a time keeps z-buffer and
/// framebuffer data in L1 cache.
template<typename NumericT, typename FragmentShader>
void rasterizeTiled(
    const std::vector<std::array<internal::ProjectedVertex<NumericT>, 3>>& triangles,
    const FragmentShader& shader,
    internal::SceneCache<NumericT>& cache)
{
    if (triangles.empty())
        return;

    const int W = static_cast<int>(cache.renderer.getRenderWidth());
    const int H = static_cast<int>(cache.renderer.getRenderHeight());
    const int tilesX = (W + TILE_SIZE - 1) / TILE_SIZE;
    const int tilesY = (H + TILE_SIZE - 1) / TILE_SIZE;
    const int nTiles = tilesX * tilesY;

    // ---- Pass 1: compute bounding-box tile ranges & count per tile ----

    struct TileRange { int tx0, ty0, tx1, ty1; };
    std::vector<TileRange> ranges(triangles.size());
    std::vector<int> count(nTiles, 0);

    for (std::size_t i = 0; i < triangles.size(); ++i)
    {
        const auto& tri = triangles[i];
        const auto& p0 = tri[0].pixel;
        const auto& p1 = tri[1].pixel;
        const auto& p2 = tri[2].pixel;

        int bx0 = std::max(0,     static_cast<int>(std::min({p0[0], p1[0], p2[0]})));
        int bx1 = std::min(W - 1, static_cast<int>(std::max({p0[0], p1[0], p2[0]})));
        int by0 = std::max(0,     static_cast<int>(std::min({p0[1], p1[1], p2[1]})));
        int by1 = std::min(H - 1, static_cast<int>(std::max({p0[1], p1[1], p2[1]})));

        if (bx0 > bx1 || by0 > by1)
        {
            ranges[i] = {0, 0, -1, -1};  // empty sentinel
            continue;
        }

        const int tx0 = bx0 / TILE_SIZE;
        const int tx1 = std::min(bx1 / TILE_SIZE, tilesX - 1);
        const int ty0 = by0 / TILE_SIZE;
        const int ty1 = std::min(by1 / TILE_SIZE, tilesY - 1);
        ranges[i] = {tx0, ty0, tx1, ty1};

        for (int ty = ty0; ty <= ty1; ++ty)
            for (int tx = tx0; tx <= tx1; ++tx)
                ++count[ty * tilesX + tx];
    }

    std::vector<int> offset(nTiles + 1, 0);
    for (int t = 0; t < nTiles; ++t)
        offset[t + 1] = offset[t] + count[t];

    std::vector<std::size_t> indices(static_cast<std::size_t>(offset[nTiles]));
    std::fill(count.begin(), count.end(), 0);   // reuse as write cursors

    for (std::size_t i = 0; i < triangles.size(); ++i)
    {
        const auto& r = ranges[i];
        if (r.tx0 > r.tx1)
            continue;

        for (int ty = r.ty0; ty <= r.ty1; ++ty)
        {
            for (int tx = r.tx0; tx <= r.tx1; ++tx)
            {
                const int tile = ty * tilesX + tx;
                indices[static_cast<std::size_t>(offset[tile] + count[tile])] = i;
                ++count[tile];
            }
        }
    }

    for (int ty = 0; ty < tilesY; ++ty)
    {
        for (int tx = 0; tx < tilesX; ++tx)
        {
            const int tile = ty * tilesX + tx;
            const int begin = offset[tile];
            const int end   = offset[tile + 1];
            if (begin == end)
                continue;

            const int x0 = tx * TILE_SIZE;
            const int y0 = ty * TILE_SIZE;
            const int x1 = std::min(x0 + TILE_SIZE, W);
            const int y1 = std::min(y0 + TILE_SIZE, H);

            for (int j = begin; j < end; ++j)
                rasterizeTriangleInRect(triangles[indices[j]], shader, cache,
                                        x0, y0, x1, y1);
        }
    }
}

} // namespace mrc::gt
