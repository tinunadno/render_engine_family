#include "main_pipeline.h"
#include "model/io.h"
#include "utils/graphics_tools.h"
#include <iostream>

int main() {
    std::cerr << "=== Multi-Material Heart ===" << std::endl;

    sc::Camera<float, sc::VecArray> camera;
    camera.pos()[2] = 40.0f;  // Move back to see the heart
    camera.setLen(0.5);
    camera.setRes(sc::utils::Vec<float, 2>{1000, 800});

    // Load heart model
    const std::string objFile = std::string(PROJECT_DIR) + "/heart.obj";
    std::cerr << "Loading heart model: " << objFile << std::endl;

    std::vector<mrc::Model<float>> models;
    std::vector<mrc::LightSource<float>> ls;

    try {
        auto model = mrc::io::readFromObjFile<float>(objFile.c_str());
        models.emplace_back(std::move(model));
        std::cerr << "Heart loaded successfully!" << std::endl;
        std::cerr << "Vertices: " << models[0].verticies().size() << std::endl;
        std::cerr << "Faces: " << models[0].faces().size() << std::endl;
        std::cerr << "Materials: " << models[0].materials.size() << std::endl;
        std::cerr << "Submeshes: " << models[0].submeshes().size() << std::endl;

        // Print submesh details with material names
        for (size_t i = 0; i < models[0].submeshes().size(); ++i) {
            const auto& sm = models[0].submeshes()[i];
            std::string matName = sm.materialName;
            std::cerr << "  Submesh " << i << ": "
                      << "mat='" << sm.materialName << "' "
                      << "faces=" << sm.faceFirst << "-" << (sm.faceFirst + sm.faceCount - 1)
                      << " matIdx=" << sm.materialIndex << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error loading model: " << e.what() << std::endl;
        return 1;
    }

    // Setup lighting
    ls.emplace_back(
        sc::utils::Vec<float, 3>{0.f, 10.f, 30.f},
        sc::utils::Vec<float, 3>{0.f, -1.f, -1.f},
        sc::utils::Vec<float, 3>{1.f, 1.f, 1.f},
        1.5f
    );

    // Second light for better visibility
    ls.emplace_back(
        sc::utils::Vec<float, 3>{-20.f, 10.f, 20.f},
        sc::utils::Vec<float, 3>{1.f, -0.5f, -1.f},
        sc::utils::Vec<float, 3>{1.0f, 0.9f, 0.8f},
        1.0f
    );

    std::cerr << "=== Starting render ===" << std::endl;
    std::cerr << "Controls:" << std::endl;
    std::cerr << "  WASD - Move camera" << std::endl;
    std::cerr << "  Shift/Ctrl - Move up/down" << std::endl;
    std::cerr << "  Mouse drag - Rotate camera" << std::endl;

    // Use default BlinnFongShaderFactory which supports both:
    // - Model path for backward compatibility (old examples)
    // - Material path for multi-material (submeshes)
    ::mrc::initMrcRender(camera, models, ls, {}, {}, {}, {}, 60, {});

    return 0;
}
