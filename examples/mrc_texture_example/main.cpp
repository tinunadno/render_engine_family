#include "main_pipeline.h"
#include "model/io.h"
#include "utils/graphics_tools.h"
#include "text_render/tr.h"


template<std::size_t N>
std::string vecToStr(const sc::utils::Vec<float, N>& vec) {
    std::string ret = "";
    for (std::size_t i = 0; i < N - 1; ++i) {
        ret += std::to_string(vec[i]);
        ret += ", ";
    }
    ret += std::to_string(vec[N - 1]);
    return ret;
}

std::string sceneObjectsToText(const std::vector<mrc::Model<float>>& models,
                               const sc::Camera<float, sc::VecArray>& c,
                               const std::vector<mrc::LightSource<float>>& ls) {
    std::string ret;
    ret += "camera: \n";
    ret += "\tpos: " + vecToStr(c.pos()) + "\n\trot: " + vecToStr(c.rot()) + "\n";
    ret += "models: \n";
    for (std::size_t i = 0; i < models.size(); ++i) {
        const auto& m = models[i];
        ret += "\tobject" + std::to_string(i) + ": \n";
        ret += "\t\tpos: " + vecToStr(m.pos()) + "\n";
        ret += "\t\trot: " + vecToStr(m.rot()) + "\n";
    }
    return ret;
}

int main() {
    sc::Camera<float, sc::VecArray> camera;
    camera.pos()[2] = 2.0f;
    camera.setLen(0.3);
    camera.setRes(sc::utils::Vec<float, 2>{1000, 800});

    const std::string fontPath = "/Library/Fonts/Arial Unicode.ttf";

    auto textRenderer = tr::TextRenderer(fontPath, 20.f);
    textRenderer.padding()[0] = 1;
    textRenderer.padding()[1] = 1;

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

    std::size_t lastTime = 0;

    auto cd = [&textRenderer, &lastTime, &camera, &models, &ls]
            (std::size_t f, std::size_t t, sc::GLFWRenderer& renderer, const sc::utils::Mat<float, 4, 4>&,
            const std::vector<std::vector<float>>&) {

        std::size_t elapsed = t - lastTime;
        lastTime = t;
        float fps = 1000.f / static_cast<float>(elapsed);
        textRenderer.renderText(renderer,
            "frames: " + std::to_string(f)
            + "\ntime: " + std::to_string(t)
            + "\nfps: " + std::to_string(fps)
            + "\nms/frame: " + std::to_string(elapsed)
            + "\n" + sceneObjectsToText(models, camera, ls),
            sc::utils::Vec<float, 3>{0.f, 1.f, 0.f});

    };

    mrc::initMrcRender(camera, models, ls, {}, cd, customKeyHandlers);

    return 0;
}
