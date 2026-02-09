#include "main_pipeline.h"
#include "model/io.h"
#include "utils/graphics_tools.h"

int main() {
    sc::Camera<float, sc::VecArray> camera;
    camera.pos()[2] = 2.0f;
    camera.setLen(0.3);
    camera.setRes(sc::utils::Vec<float, 2>{1000, 800});
    const std::string obj1File = std::string(PROJECT_DIR) + "/objects/rock.obj";

    std::vector<mrc::Model<float>> models;
    std::vector<mrc::LightSource<float>> ls;
    models.emplace_back(mrc::io::readFromObjFile<float>(obj1File.c_str()));
    ls.emplace_back(
        sc::utils::Vec<float, 3>{0.f, 1.3f, 0.f},
        sc::utils::Vec<float, 3>{0.f, -1.f, 0.f},
        sc::utils::Vec<float, 3>{1.f, 1.f, 1.f},
        1.f
    );

    std::vector<std::pair<std::vector<int>, std::function<void()>>> customKeyHandlers = {
        {{GLFW_KEY_I}, [&ls](){ ls[0].position[2] -= .1f; }},
        {{GLFW_KEY_J}, [&ls](){ ls[0].position[0] -= .1f; }},
        {{GLFW_KEY_K}, [&ls](){ ls[0].position[2] += .1f; }},
        {{GLFW_KEY_L}, [&ls](){ ls[0].position[0] += .1f; }},
        {{GLFW_KEY_U}, [&ls](){ ls[0].position[1] += .1f; }},
        {{GLFW_KEY_O}, [&ls](){ ls[0].position[1] -= .1f; }},
        {{80, 79}, [&ls](){ ls[0].intensity = 0.f; }},
        {{80, 85}, [&ls](){ ls[0].intensity = 1.f; }},
    };


    auto coneObject = mrc::readFromObjFile<float>((std::string(PROJECT_DIR) + "/cone.obj").c_str());

    auto lightSourcesDrawer = [&ls, &coneObject, &camera](std::size_t, std::size_t, sc::GLFWRenderer& renderer,
            const sc::utils::Mat<float, 4, 4>& proj, const std::vector<std::vector<float>>&) {
        for (const auto& lightSource : ls)
        {
            std::vector<sc::utils::Vec<float, 2>> projectedPoints;
            projectedPoints.reserve(coneObject.verticies().size());
            for (const auto& v : coneObject.verticies())
            {
                auto tempV = sc::utils::rotateEuler(v, lightSource.direction) + lightSource.position;
                auto projV = mrc::projectVertex(tempV, proj, camera);
                projectedPoints.emplace_back(projV.pixel);
            }
            for (const auto& face : coneObject.faces())
            {
                mrc::gt::drawLine(renderer,
                    projectedPoints[face[0][0]], projectedPoints[face[1][0]],
                    sc::utils::Vec<float, 3>{1.f, 0.1, 0.f});
                mrc::gt::drawLine(renderer,
                    projectedPoints[face[1][0]], projectedPoints[face[2][0]],
                    sc::utils::Vec<float, 3>{1.f, 0.1, 0.f});
                mrc::gt::drawLine(renderer,
                    projectedPoints[face[2][0]], projectedPoints[face[0][0]],
                    sc::utils::Vec<float, 3>{1.f, 0.1, 0.f});
            }
        }
    };

    mrc::initMrcRender(camera, models, ls, {}, lightSourcesDrawer, customKeyHandlers);

    return 0;
}
