#include "main_pipeline.h"
#include "model/io.h"
#include "utils/graphics_tools.h"

int main() {
    sc::Camera<float, sc::VecArray> camera;
    camera.pos()[2] = 2.0f;
    camera.setLen(0.3);
    camera.setRes(sc::utils::Vec<float, 2>{1000, 800});
    const std::string obj1File = std::string(PROJECT_DIR) + "/../mrc_texture_example/objects/rock.obj";

    std::vector<mrc::Model<float>> models;
    std::vector<mrc::LightSource<float>> ls;
    // models.emplace_back(mrc::io::readFromObjFile<float>(obj1File.c_str()));
    ls.emplace_back(
        sc::utils::Vec<float, 3>{0.f, 3.f, 0.f},
        sc::utils::Vec<float, 3>{0.f, -1.f, 0.f},
        sc::utils::Vec<float, 3>{1.f, 1.f, 1.f},
        1.f
    );

    mrc::ParticleSystem<float> particles;
    mrc::makeCircleBillboard(particles, 8);

    std::size_t count  = 10000;
    particles.positions.resize(count);
    particles.colors.resize(count);
    particles.sizes.resize(count);
    for (int i = 0; i < count; ++i) {
        particles.positions[i] = sc::utils::Vec<float, 3>{static_cast<float>(i) / 10.f, 0, 0};
        particles.colors[i] = sc::utils::Vec<float, 3>{1.f, 0.f, 0.f};  // red
        particles.sizes[i] = 0.05f;
    }

    mrc::initMrcRender(camera, models, ls,
        { }, { }, {},
        sc::utils::Vec<int, 2>{-1,-1}, 60, {}, &particles);

    return 0;
}
