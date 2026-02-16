#include <random>

#include "main_pipeline.h"
#include "model/io.h"
#include "utils/graphics_tools.h"

inline sc::utils::Vec<float, 3> sphericalToEuclidian(float theta, float phi, float r) {
    const float thetaCos = std::cos(theta);
    const float thetaSin = std::sin(theta);
    const float phiCos   = std::cos(phi);
    const float phiSin   = std::sin(phi);

    return sc::utils::Vec<float, 3>{
        thetaCos * phiSin * r,
        thetaCos * phiCos * r,
        thetaSin * r
    };
}

inline void generateParticles(mrc::ParticleSystem<float>& particles, std::size_t count) {
    particles.positions.resize(count);
    particles.colors.resize(count);
    particles.sizes.resize(count);
    particles.enableRender.resize(count);


    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution distribR(0.f, 3.f);
    std::uniform_real_distribution distribPhi(0.f, .2f);
    std::uniform_real_distribution distribTheta(-.9f, .9f);

    const sc::utils::Vec<float, 3> aColor = sc::utils::Vec<float, 3>{255, 255, 255} / 255.f;
    const sc::utils::Vec<float, 3> bColor = sc::utils::Vec<float, 3>{204, 51, 0} / 255.f;

    for (int i = 0; i < count; ++i) {
        const float r = distribR(gen);
        const float phi = distribPhi(gen);
        const float theta = distribTheta(gen);
        particles.positions[i] = sphericalToEuclidian(theta, phi, r);
        const float t = r / 3.f;
        particles.colors[i] = (bColor - aColor) * t + aColor;
        particles.sizes[i] = 0.01f;
        particles.enableRender[i] = true;
    }
}

void updateParticles(mrc::ParticleSystem<float>& particles) {
    auto rot = sc::utils::Vec<float, 3>{0.f, 0.f, .01f};
    auto base = sc::utils::Vec<float, 3>{0.f, 0.f, 0.f};
    for (auto& p : particles.positions) {
        const float t = 1.f - sc::utils::distance(base, p) / 3.f + .1f;
        p = sc::utils::rotateEuler(p, rot * t);
    }
    // for (std::size_t i = 0; i < particles.enableRender.size(); ++i) {
    //     if (particles.positions[i][0] > 0) particles.enableRender[i] = false;
    //     else particles.enableRender[i] = true;
    // }
}

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

    generateParticles(particles, 10000);

    auto efmu = [&particles](std::size_t, std::size_t) { updateParticles(particles); };

    mrc::initMrcRender(camera, models, ls,
        efmu, { }, {},
        sc::utils::Vec<int, 2>{-1,-1}, 60, {}, &particles);

    return 0;
}
