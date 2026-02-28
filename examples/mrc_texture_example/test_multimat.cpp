#include "main_pipeline.h"
#include "model/io.h"
#include "utils/graphics_tools.h"
#include <iostream>

int main() {
    std::cerr << "TEST: Starting...\n" << std::flush;

    sc::Camera<float, sc::VecArray> camera;
    camera.pos()[2] = 3.0f;
    camera.setLen(0.5);
    camera.setRes(sc::utils::Vec<float, 2>{1000, 800});

    // Test with multi-material model
    const std::string objFile = std::string(PROJECT_DIR) + "/multi_material_test/simple_test.obj";
    std::cerr << "Loading model: " << objFile << "\n" << std::flush;

    std::vector<mrc::Model<float>> models;
    std::vector<mrc::LightSource<float>> ls;

    try {
        auto model = mrc::io::readFromObjFile<float>(objFile.c_str());
        models.emplace_back(std::move(model));
        std::cerr << "Model loaded successfully!\n" << std::flush;
        std::cerr << "Vertices: " << models[0].verticies().size() << "\n" << std::flush;
        std::cerr << "Faces: " << models[0].faces().size() << "\n" << std::flush;
        std::cerr << "Materials: " << models[0].materials.size() << "\n" << std::flush;
        std::cerr << "Submeshes: " << models[0].submeshes().size() << "\n" << std::flush;
    } catch (const std::exception& e) {
        std::cerr << "Error loading model: " << e.what() << "\n" << std::flush;
        return 1;
    }

    ls.emplace_back(
        sc::utils::Vec<float, 3>{0.f, 3.f, 0.f},
        sc::utils::Vec<float, 3>{0.f, -1.f, 0.f},
        sc::utils::Vec<float, 3>{1.f, 1.f, 1.f},
        1.f
    );

    std::cerr << "TEST: About to init render...\n" << std::flush;
    mrc::initMrcRender(camera, models, ls);

    return 0;
}
