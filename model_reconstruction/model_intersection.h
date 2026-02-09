#pragma once

#include "../model_render_core/include/model/model.h"

#include <manifold/manifold.h>
#include <vector>
#include <array>


namespace mi {
    manifold::MeshGL ObjectToMeshGL(const mrc::Model<float>& obj);

    mrc::Model<float> MeshGLToObject(const manifold::MeshGL& mesh);

    mrc::Model<float> ComputeIntersection(const std::vector<mrc::Model<float>>& inputObjects);

} // namespace mi
