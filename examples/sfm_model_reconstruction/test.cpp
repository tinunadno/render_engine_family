#include "sfm/run_sfm.h"

int main() {
    std::string baseDBPath = std::string(PROJECT_DIR) + "/db";
    std::string imagesPath = baseDBPath + "/test_images";
    std::string dbPath = baseDBPath + "/database.db";

    const auto pnts = runSfM(dbPath, imagesPath);

    std::cout << pnts.size() << std::endl;

    for (const auto& pnt: pnts) {
        std::cout << pnt.data()[0] << " " << pnt.data()[1] << " " << pnt.data()[2] << std::endl;
    }

}