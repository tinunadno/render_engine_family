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
        sc::utils::Vec<float, 3>{0.f, 3.f, 0.f},
        sc::utils::Vec<float, 3>{0.f, -1.f, 0.f},
        sc::utils::Vec<float, 3>{1.f, 1.f, 1.f},
        1.f
    );

    bool isLightControl = false;
    bool isObjectControl = false;
    bool isRotation = false;

    auto handleInputs = [&models, &ls, &camera, &isLightControl,
        &isObjectControl, &isRotation](int axes, float step)
    {
        if (isObjectControl) {
            if (isRotation)
                models[0].rot()[axes] += step;
            else
                models[0].pos()[axes] += step;
        } else if (isLightControl) {
            if (isRotation)
                ls[0].direction[axes] += step;
            else
                ls[0].position[axes] += step;
        } else {
            mrc::internal::handleCameraMovement(axes, step, camera);
        }
    };

    constexpr float stepSize = 0.05f;
    std::vector<std::pair<std::vector<int>, std::function<void()>>> customKeyHandlers = {
        // just changing the editors
        {{GLFW_KEY_LEFT_ALT, GLFW_KEY_L}, [&isLightControl, &isObjectControl]()
            { isLightControl = !isLightControl; if (isLightControl && isObjectControl) isObjectControl = false; },},
        {{GLFW_KEY_LEFT_ALT, GLFW_KEY_O}, [&isLightControl, &isObjectControl]()
            { isObjectControl = !isObjectControl; if (isObjectControl && isLightControl) isLightControl = false; },},
        {{GLFW_KEY_LEFT_ALT, GLFW_KEY_R}, [&isRotation](){ isRotation = !isRotation; }},
        // overriding default keys
        {{GLFW_KEY_W}, [&handleInputs](){ handleInputs(2, stepSize); }},
        {{GLFW_KEY_A}, [&handleInputs](){ handleInputs(0, -stepSize); }},
        {{GLFW_KEY_S}, [&handleInputs](){ handleInputs(2, -stepSize); }},
        {{GLFW_KEY_D}, [&handleInputs](){ handleInputs(0, stepSize); }},
        {{GLFW_KEY_LEFT_SHIFT}, [&handleInputs](){ handleInputs(1, stepSize); }},
        {{GLFW_KEY_LEFT_CONTROL}, [&handleInputs](){ handleInputs(1, -stepSize); }},
    };

    mrc::initMrcRender(camera, models, ls, {}, {}, customKeyHandlers);

    return 0;
}
