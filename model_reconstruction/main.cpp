#include "main_pipeline.h"
#include "snapshot.h"
#include "model_intersection.h"

inline float dotThree(const sc::utils::Vec<float, 2>& a, const sc::utils::Vec<float, 2>& b,
                      const sc::utils::Vec<float, 2>& c) {
    return (b[0] - a[0]) * (c[1] - a[1]) - (b[1] - a[1]) * (c[0] - a[0]);
}

std::vector<Snapshot> runFirstRender()
{
    sc::Camera<float, sc::VecArray> camera;
    camera.pos()[2] = 2.0f;
    camera.setLen(0.3);
    const char* objFile = "/Users/yura/stuff/clion/curved_space_render_engine/model_render_core_example/test.obj";

    std::vector<mrc::Model<float>> models;
    models.emplace_back(mrc::readFromObjFile<float>(objFile));

    std::vector<sc::utils::Vec<float, 2>> lastRoi;
    std::mutex lastRoiMutex;

    auto roiDrawer = [&models, &camera, &lastRoi, &lastRoiMutex](
        std::size_t,
        std::size_t,
        sc::GLFWRenderer& renderer,
        sc::utils::Mat<float, 4, 4>& proj)
    {
        std::vector<sc::utils::Vec<float, 2>> projectedPoints(models[0].verticies().size());
        for (std::size_t i = 0; i < models[0].verticies().size(); ++i) {
            auto tempV = sc::utils::rotateEuler(models[0].verticies()[i], models[0].rot()) + models[0].pos();
            auto projV = mrc::projectVertex(tempV, proj, camera);
            projectedPoints[i] = projV.pixel;
        }
        std::sort(projectedPoints.begin(), projectedPoints.end(), [](const auto& a, const auto& b) {
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

        {
            std::lock_guard lock(lastRoiMutex);
            lastRoi.clear();
            lastRoi.reserve(hull.size());

            std::transform(hull.begin(), hull.end(), std::back_inserter(lastRoi),
                [&projectedPoints](std::size_t i) {
                    return projectedPoints[i];
                });
        }

        for (std::size_t i = 1; i < hull.size(); ++i) {
            mrc::gt::drawLine(
                renderer,
                projectedPoints[hull[i]],
                projectedPoints[hull[(i + 1) % (hull.size() - 1)]],
                sc::utils::Vec<float, 3>(1., 0., 0.)
                );
        }
    };

    std::vector<Snapshot> snapshots;

    std::vector<std::pair<int, std::function<void()>>> customKeyHandlers = { // creating snapshot
        {GLFW_KEY_E, [&snapshots, &camera, &lastRoi, &lastRoiMutex]() {
            std::lock_guard lock(lastRoiMutex);
            snapshots.emplace_back(camera, std::move(lastRoi));
        }}
    };

    mrc::initMrcRender(camera, models, { }, roiDrawer, customKeyHandlers);

    return snapshots;
}

std::vector<mrc::Model<float>> runSecondRender(const std::vector<Snapshot>& snapshots)
{
    sc::Camera<float, sc::VecArray> camera;
    camera.pos()[2] = 2.0f;
    camera.setLen(0.3);

    auto models = snapshotsToModels(snapshots);

    mrc::initMrcRender(camera, models);
    return models;
}

void runThirdRender(const std::vector<mrc::Model<float>>& coneModels)
{
    sc::Camera<float, sc::VecArray> camera;
    camera.pos()[2] = 2.0f;
    camera.setLen(0.3);
    std::vector<mrc::Model<float>> models;
    models.emplace_back(mi::ComputeIntersection(coneModels));

    mrc::initMrcRender(camera, models);
}

int main()
{
    auto snapshots = runFirstRender();
    auto models = runSecondRender(snapshots);
    runThirdRender(models);

    return 0;
}
