#include "run_sfm.h"

void savePoints(const std::string& path, const std::vector<Eigen::Vector3d>& points) {
    const std::size_t totalSize = sizeof(std::size_t) + sizeof(double) * 3 * points.size();
    char* data = static_cast<char*>(malloc(totalSize));
    char* ptr = data;
    *reinterpret_cast<std::size_t *>(ptr) = points.size();
    ptr += sizeof(std::size_t);
    for (auto & point : points) {
        for (std::size_t j = 0; j < 3; j++) {
            assert(ptr - data < totalSize);
            *reinterpret_cast<double *>(ptr) = point.data()[j];
            ptr += sizeof(double);
        }
    }
    FILE* file_pointer;
    file_pointer = fopen(path.c_str(), "wb");
    size_t elements_written = fwrite(data, 1, totalSize, file_pointer);

    if (elements_written != totalSize)
        std::cerr << "Error writing points to file";
    else
        std::cout << "writed " << totalSize << " bytes to " << path << std::endl;

    fclose(file_pointer);

    free(data);
}

int main(int argc, const char* argv[]) {

    if (argc != 4) {
        std::cerr << "expected 3 arguments, image dir, database path, points save path\n";
        return 1;
    }

    std::string imagesPath(argv[1]);
    std::string dbPath(argv[2]);
    std::string savePath(argv[3]);

    const auto pnts = runSfM(dbPath, imagesPath);

    std::cout << "total Points: " << pnts.size() << std::endl;

    if (pnts.empty()) {
        std::cerr << "No Points" << std::endl;
        return 1;
    }
    savePoints(savePath, pnts);
    return 0;
}