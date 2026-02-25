#include <random>

#include "main_pipeline.h"
#include "model/io.h"
#include "utils/graphics_tools.h"
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

void readParticles(mrc::ParticleSystem<float>& ps, const std::string& pointsPath) {
    int fd = open(pointsPath.c_str(), O_RDONLY);
    if (fd == -1)
        { std::cerr << "Error opening the file" << std::endl; return; }

    struct stat st{};
    fstat(fd, &st);
    std::size_t fileSize = st.st_size;

    if (fileSize <= sizeof(std::size_t))
        {close(fd); return;}

    auto *byte_ptr = static_cast<std::uint8_t*>
            (mmap(nullptr, fileSize, PROT_READ, MAP_SHARED, fd, 0));
    if (byte_ptr == MAP_FAILED)
        {std::cerr << "Error mapping the file" << std::endl; close(fd); return; }

    std::size_t pointsCount = *reinterpret_cast<std::size_t*>(byte_ptr);

    if (pointsCount * 3 * sizeof(double) + sizeof(std::size_t) != fileSize)
        {munmap(byte_ptr, fileSize); close(fd); return;}

    ps.colors.resize(pointsCount);
    ps.enableRender.resize(pointsCount);
    ps.positions.resize(pointsCount);
    ps.sizes.resize(pointsCount);

    const auto* ptr = reinterpret_cast<double*>(byte_ptr + sizeof(std::size_t));
    for (std::size_t i = 0; i < pointsCount; ++i) {
        ps.positions[i][0] = static_cast<float>(*ptr++);
        ps.positions[i][1] = static_cast<float>(*ptr++);
        ps.positions[i][2] = static_cast<float>(*ptr++);
        ps.sizes[i] = .01f;
        ps.colors[i] = sc::utils::Vec<float, 3>{1.f, 1.f, 1.f};
        ps.enableRender[i] = true;
    }

    munmap(byte_ptr, fileSize);
    close(fd);
}

int main() {
    sc::Camera<float, sc::VecArray> camera;
    camera.pos()[2] = 2.0f;
    camera.setLen(0.3);
    camera.setRes(sc::utils::Vec<float, 2>{1000, 800});

    std::vector<mrc::Model<float>> models;
    std::vector<mrc::LightSource<float>> ls;

    mrc::ParticleSystem<float> particles;
    mrc::makeCircleBillboard(particles, 5);

    const std::string pointsPath = std::string(PROJECT_DIR) + "/points.pnts";

    readParticles(particles, pointsPath);

    mrc::initMrcRender(camera, models, ls,
        { }, { }, {},
        sc::utils::Vec<int, 2>{-1,-1}, 60, {}, &particles);

    return 0;
}
