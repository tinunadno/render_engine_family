#include "main_pipeline.h"
#include "model/io.h"
#include "snapshot.h"
#include "sfm/run_sfm.h"
#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif
#include "model/thirdparty/stb_image_write.h" // hehe

inline float dotThree(const sc::utils::Vec<float, 2>& a, const sc::utils::Vec<float, 2>& b,
                      const sc::utils::Vec<float, 2>& c) {
    return (b[0] - a[0]) * (c[1] - a[1]) - (b[1] - a[1]) * (c[0] - a[0]);
}

inline std::vector<std::size_t> calculateConvexHull(
    std::vector<sc::utils::Vec<float, 2>>& projectedPoints)
{
    std::ranges::sort(projectedPoints, [](const auto& a, const auto& b) {
        return (a[0] < b[0]) || (a[0] == b[0] && a[1] < b[1]);
    });
    std::vector<std::size_t> upperConvexHullIndices{0, 1};
    for (std::size_t i = 2; i < projectedPoints.size(); ++i) {
        while (upperConvexHullIndices.size() >= 2) {
            const auto& p1 = projectedPoints[*(upperConvexHullIndices.end() - 2)];
            const auto& p2 = projectedPoints[upperConvexHullIndices.back()];
            const auto& p3 = projectedPoints[i];
            if (dotThree(p1, p2, p3) < 0.f) {
                break;
            }
            upperConvexHullIndices.pop_back();
        }
        upperConvexHullIndices.push_back(i);
    }
    std::vector<std::size_t> hull;
    for (std::size_t i = projectedPoints.size(); i-- > 0; ) {
        while (hull.size() >= 2) {
            const auto& p1 = projectedPoints[*(hull.end() - 2)];
            const auto& p2 = projectedPoints[hull.back()];
            const auto& p3 = projectedPoints[i];
            if (dotThree(p1, p2, p3) < 0.f) {
                break;
            }
            hull.pop_back();
        }
        hull.push_back(i);
    }

    hull.insert(hull.end(), upperConvexHullIndices.begin() + 1, upperConvexHullIndices.end());

    return hull;
}

int main() {

    sc::Camera<float, sc::VecArray> cam;
    cam.pos()[2] = 2.0f;
    cam.setLen(0.3);

    const std::string modelPath = std::string(PROJECT_DIR) + "/../mrc_texture_example/objects/rock.obj";
    std::vector<mrc::Model<float>> models;
    models.emplace_back(mrc::readFromObjFile<float>(modelPath.c_str()));

    std::vector lights = {mrc::LightSource{
        sc::utils::Vec<float, 3>{0.f, 4.f, 0.f},
        sc::utils::Vec<float, 3>{0.f, -1.f, 0.f},
        sc::utils::Vec<float, 3>{1.f, 1.f, 1.f},
        2.f
    }};

    std::vector<sc::utils::Vec<float, 2>> lastRoi;
    const unsigned char* lastBuffer = nullptr;

    auto roiDrawer = [&models, &cam, &lastRoi, &lastBuffer](
        std::size_t,
        std::size_t,
        sc::GLFWRenderer& renderer,
        const sc::utils::Mat<float, 4, 4>& proj,
        const std::vector<std::vector<float>>& zBuffer)
    {
        for (std::size_t i = 0; i < models.size(); ++i)
        {
            const auto& model = models[i];
            std::vector<sc::utils::Vec<float, 2>> projectedPoints;
            for (const auto& v : model.verticies())
            {
                auto tempV = sc::utils::rotateEuler(v, model.rot()) + model.pos();
                auto projV = mrc::projectVertex(tempV, proj, cam);
                float currentZ = 1.f / projV.invW;
                if (projV.pixel[0] < 0 || projV.pixel[0] >= zBuffer[0].size()
                    || projV.pixel[1] < 0 || projV.pixel[1] >= zBuffer.size())
                    continue;
                if (currentZ > 0 && currentZ - .05f <
                    zBuffer[static_cast<std::size_t>(projV.pixel[1])]
                           [static_cast<std::size_t>(projV.pixel[0])])
                    projectedPoints.emplace_back(projV.pixel);
            }
            auto hull = calculateConvexHull(projectedPoints);

            Snapshot snapshot;

            lastRoi.clear();
            lastRoi.reserve(hull.size());
            std::ranges::transform(hull, std::back_inserter(lastRoi),
                                   [&projectedPoints](std::size_t idx) {
                                       return projectedPoints[idx];
                                   });
            lastBuffer = renderer.getBufferPtr();

            for (std::size_t j = 1; j < hull.size(); ++j) {
                mrc::gt::drawLine(
                    renderer,
                    projectedPoints[hull[j]],
                    projectedPoints[hull[(j + 1) % (hull.size() - 1)]],
                    sc::utils::Vec<float, 3>(1., 0., 0.)
                );
            }
        }
    };

    std::size_t imCount = 0;
    std::string baseDBPath = std::string(PROJECT_DIR) + "/db";

    std::string imagesPath = baseDBPath + "/images";



    std::vector<std::pair<std::vector<int>, std::function<void()>>> customKeys = {
        {{GLFW_KEY_LEFT_ALT, GLFW_KEY_E}, [&lastRoi, &lastBuffer, &cam, &imCount, &imagesPath]() {

            // mirorring buffer horizontally
            const std::size_t stride = static_cast<std::size_t>(cam.res()[0]) * 3;
            std::vector resultingImage(lastBuffer, lastBuffer
                + static_cast<std::size_t>(cam.res()[1]) * stride);
            auto row_begin = [&resultingImage, &stride](std::size_t row) {
                return resultingImage.begin() + row * stride;
            };
            const auto height = static_cast<std::size_t>(cam.res()[1]);
            for (std::size_t row = 0; row < height / 2; ++row) {
                auto top_row = row_begin(row);
                auto bottom_row = row_begin(height - 1 - row);
                std::swap_ranges(top_row, top_row + stride, bottom_row);
            }

            std::string filename = imagesPath + "/image" + std::to_string(imCount++) + ".png";
            int success = stbi_write_png(filename.c_str(), cam.res()[0], cam.res()[1],
                3, resultingImage.data(), stride);
            if (!success) {
                std::cerr << "Error writing png image to file: " << filename << std::endl;
            }
        }}
    };

    mrc::initMrcRender(cam, models, lights, {}, roiDrawer, customKeys);

    std::string dbPath = baseDBPath + "/database.db";

    const auto pnts = runSfM(dbPath, imagesPath);

    for (const auto& pnt: pnts) {
        std::cout << pnt.data()[0] << " " << pnt.data()[1] << " " << pnt.data()[2] << std::endl;
    }


    return 0;
}
