#pragma once

#include "entry_point.h"
#include "model/model.h"
#include "utils/point_projection.h"
#include "utils/graphics_tools.h"
#include "utils/compute_normals.h"
#include "utils/vertices_transform.h"

#include <memory>
#include <algorithm>

namespace mrc {

namespace internal
{

template<typename NumericT>
struct DefaultShaderFactory
{
    auto operator()(const Model<NumericT>& model) const
    {
        return [&mat = model.material](const FragmentInput<NumericT>& frag)
            -> sc::utils::Vec<float, 3>
        {
            sc::utils::Vec<float, 3> color{
                static_cast<float>(mat.baseColor[0]),
                static_cast<float>(mat.baseColor[1]),
                static_cast<float>(mat.baseColor[2])
            };

            if (mat.diffuseMap)
                color = mat.diffuseMap->sample(frag.uv);

            
            sc::utils::Vec<float, 3> lightDir{0.577350f, 0.577350f, 0.577350f};
            auto n = sc::utils::norm(
                sc::utils::Vec<float, 3>{
                    static_cast<float>(frag.normal[0]),
                    static_cast<float>(frag.normal[1]),
                    static_cast<float>(frag.normal[2])
                });
            float NdotL = std::max(0.f, sc::utils::dot(n, lightDir));
            float amb = static_cast<float>(mat.ambient);

            return color * (amb + (1.f - amb) * NdotL);
        };
    }
};



template<typename NumericT, typename MakeShader>
void renderSingleFrame(const std::vector<Model<NumericT>>& models,
                       const sc::Camera<NumericT, sc::VecArray>& camera,
                       std::vector<std::vector<NumericT>>& zBuffer,
                       sc::GLFWRenderer& renderer,
                       const sc::utils::Mat<NumericT, 4, 4>& projView,
                       MakeShader&& makeShader)
{
    for (const auto& model : models)
    {
        auto shader = makeShader(model);

        auto transformedVerts = internal::transformVerticies(
            model.verticies(), model.pos(), model.rot());
        auto transformedNormals = internal::transformNormals(
            model.normals(), model.rot());

        for (std::size_t f = 0; f < model.faces().size(); ++f)
        {
            const auto& face = model.faces()[f];

            
            auto faceNormal = getFaceNormal(transformedVerts, face);

            std::array<internal::ClipVertex<NumericT>, 3> clipVerts;
            for (int i = 0; i < 3; ++i)
            {
                auto wsPos = transformedVerts[face[i][0]];

                internal::VertexAttributes<NumericT> attr;
                attr.worldPos = wsPos;

                
                if (face[i][1] < model.uv().size())
                    attr.uv = model.uv()[face[i][1]];

                
                if (face[i][2] < transformedNormals.size())
                    attr.normal = transformedNormals[face[i][2]];
                else
                    attr.normal = faceNormal;

                clipVerts[i] = wsToClip(wsPos, projView, attr);
            }

            gt::processTriangle(renderer, clipVerts, camera, zBuffer, shader);
        }
    }
}

template<typename NumericT>
void handleCameraMovement(int axis, NumericT distance,
                          sc::Camera<NumericT, sc::VecArray>& camera)
{
    const auto forward = getForward(camera.rot());
    const auto right   = getRight(forward);
    const auto up      = getUp(forward, right);

    sc::utils::Vec<NumericT, 3> d{0., 0., 0.};
    d[axis] = distance;

    camera.pos() += forward * d[2];
    camera.pos() += right   * d[0];
    camera.pos() += up      * d[1];
}

} 



template<typename NumericT,
    typename EachFrameModelUpdate = decltype([](std::size_t, std::size_t){ }),
    typename CustomDrawer =
        decltype([](std::size_t, std::size_t, sc::GLFWRenderer&, const sc::utils::Mat<NumericT, 4, 4>&,
            const std::vector<std::vector<NumericT>>&){ }),
    typename MakeShader = internal::DefaultShaderFactory<NumericT>>
void initMrcRender(sc::Camera<NumericT, sc::VecArray>& camera,
                   const std::vector<Model<NumericT>>& models,
                   EachFrameModelUpdate efmu = { },
                   CustomDrawer cd = { },
                   const std::vector<std::pair<int, std::function<void()>>>& customKeyHandlers = {},
                   sc::utils::Vec<int, 2> windowResolution = sc::utils::Vec<int, 2>{-1, -1},
                   unsigned int targetFrameRateMs = 60,
                   MakeShader makeShader = { })
{
    using Mat4 = sc::utils::Mat<NumericT, 4, 4>;

    std::vector zBuffer(
        static_cast<std::size_t>(camera.res()[1]),
        std::vector(
            static_cast<std::size_t>(camera.res()[0]),
          std::numeric_limits<float>::max()
        )
    );

    const auto usc = [&zBuffer, &camera] {
        for (auto& buf : zBuffer)
            std::fill(buf.begin(), buf.end(), std::numeric_limits<float>::max());
        Mat4 view = mrc::getViewMatrix(camera);
        Mat4 proj = mrc::getProjectionMatrix(camera);
        return std::pair{view, proj};
    };

    auto ff = [&efmu, &usc, &cd, &models, &camera, &zBuffer, makeShader = std::move(makeShader)](
        sc::GLFWRenderer& renderer, std::size_t frame, std::size_t time) mutable
    {
        auto [view, proj] = usc();
        auto viewProj = proj * view;
        efmu(frame, time);
        internal::renderSingleFrame(models, camera, zBuffer, renderer, viewProj, makeShader);
        cd(frame, time, renderer, viewProj, zBuffer);
    };

    constexpr NumericT stepSize = .5;
    constexpr NumericT rotSize = 1. / 100.;

    std::vector<std::pair<int, std::function<void()>>> keyHandlers = {
        {GLFW_KEY_W, [&camera](){ internal::handleCameraMovement(2, NumericT(stepSize), camera); }},
        {GLFW_KEY_A, [&camera](){ internal::handleCameraMovement(0, NumericT(-stepSize), camera); }},
        {GLFW_KEY_S, [&camera](){ internal::handleCameraMovement(2, NumericT(-stepSize), camera); }},
        {GLFW_KEY_D, [&camera](){ internal::handleCameraMovement(0, NumericT(stepSize), camera); }},
        {GLFW_KEY_LEFT_SHIFT, [&camera](){ internal::handleCameraMovement(1, NumericT(stepSize), camera); }},
        {GLFW_KEY_LEFT_CONTROL, [&camera](){ internal::handleCameraMovement(1, NumericT(-stepSize), camera); }},
    };

    keyHandlers.insert(keyHandlers.end(), customKeyHandlers.begin(), customKeyHandlers.end());

    auto mouseHandler = [&camera](double dx, double dy) {
      camera.rot()[0] -= dy * rotSize;
      camera.rot()[1] += dx * rotSize;
    };

    sc::initPerFrameRender(camera, ff, {}, keyHandlers, mouseHandler, windowResolution, targetFrameRateMs);
}



template<typename NumericT,
    typename EachFrameModelUpdate = decltype([](std::size_t, std::size_t){ }),
    typename CustomDrawer =
        decltype([](std::size_t, std::size_t, sc::GLFWRenderer&, const sc::utils::Mat<NumericT, 4, 4>&,
            const std::vector<std::vector<NumericT>>&){ }),
    typename MakeShader = internal::DefaultShaderFactory<NumericT>>
auto makeMrcWindow(sc::Camera<NumericT, sc::VecArray>& camera,
                   const std::vector<Model<NumericT>>& models,
                   EachFrameModelUpdate efmu = { },
                   CustomDrawer cd = { },
                   const std::vector<std::pair<int, std::function<void()>>>& customKeyHandlers = {},
                   sc::utils::Vec<int, 2> windowResolution = sc::utils::Vec<int, 2>{-1, -1},
                   unsigned int targetFrameRateMs = 60,
                   const char* title = "Model Renderer",
                   MakeShader makeShader = { })
{
    using Mat4 = sc::utils::Mat<NumericT, 4, 4>;

    auto zBuffer = std::make_shared<std::vector<std::vector<NumericT>>>(
        static_cast<std::size_t>(camera.res()[1]),
        std::vector<NumericT>(
            static_cast<std::size_t>(camera.res()[0]),
            std::numeric_limits<NumericT>::max()
        )
    );

    auto ff = [&models, &camera, zBuffer,
               efmu = std::move(efmu), cd = std::move(cd),
               makeShader = std::move(makeShader)](
        sc::GLFWRenderer& renderer, std::size_t frame, std::size_t time) mutable
    {
        for (auto& buf : *zBuffer)
            std::fill(buf.begin(), buf.end(), std::numeric_limits<NumericT>::max());

        Mat4 view = getViewMatrix(camera);
        Mat4 proj = getProjectionMatrix(camera);
        auto viewProj = proj * view;

        efmu(frame, time);
        internal::renderSingleFrame(models, camera, *zBuffer, renderer, viewProj, makeShader);
        cd(frame, time, renderer, viewProj, *zBuffer);
    };

    constexpr NumericT stepSize = .5;
    constexpr NumericT rotSize = 1. / 100.;

    std::vector<std::pair<int, std::function<void()>>> keyHandlers = {
        {GLFW_KEY_W, [&camera](){ internal::handleCameraMovement(2, NumericT(stepSize), camera); }},
        {GLFW_KEY_A, [&camera](){ internal::handleCameraMovement(0, NumericT(-stepSize), camera); }},
        {GLFW_KEY_S, [&camera](){ internal::handleCameraMovement(2, NumericT(-stepSize), camera); }},
        {GLFW_KEY_D, [&camera](){ internal::handleCameraMovement(0, NumericT(stepSize), camera); }},
        {GLFW_KEY_LEFT_SHIFT, [&camera](){ internal::handleCameraMovement(1, NumericT(stepSize), camera); }},
        {GLFW_KEY_LEFT_CONTROL, [&camera](){ internal::handleCameraMovement(1, NumericT(-stepSize), camera); }},
    };

    keyHandlers.insert(keyHandlers.end(), customKeyHandlers.begin(), customKeyHandlers.end());

    auto mouseHandler = [&camera](double dx, double dy) {
      camera.rot()[0] -= dy * rotSize;
      camera.rot()[1] += dx * rotSize;
    };

    return sc::makePerFrameWindow(camera, std::move(ff),
        [](std::size_t, std::size_t){},
        keyHandlers, mouseHandler, windowResolution, targetFrameRateMs, title);
}

} 
