#pragma once

#include "model.h"

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <unordered_map>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <cstdint>

// stb_image: define implementation here.
// NOTE: if io.h is included from multiple translation units in the same
// executable, move the two lines below into exactly ONE .cpp file and
// keep only the bare #include in this header.
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include "thirdparty/stb_image.h"

namespace mrc::io
{


namespace detail
{

inline std::string extractDir(const char* path)
{
    std::string s(path);
    auto pos = s.find_last_of('/');
    if (pos == std::string::npos)
        pos = s.find_last_of('\\');
    if (pos == std::string::npos)
        return "";
    return s.substr(0, pos + 1);
}

inline std::string resolvePath(const std::string& baseDir, const std::string& path)
{
    if (!path.empty() && path[0] == '/')
        return path;
    return baseDir + path;
}

inline std::pair<const char*, std::size_t> mmapFile(const char* path)
{
    int fd = open(path, O_RDONLY);
    if (fd == -1)
        throw std::runtime_error(std::string("io: cannot open ") + path);

    off_t sz = lseek(fd, 0, SEEK_END);
    if (sz <= 0) { close(fd); throw std::runtime_error(std::string("io: empty file ") + path); }

    void* data = mmap(nullptr, static_cast<std::size_t>(sz), PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);
    if (data == MAP_FAILED)
        throw std::runtime_error(std::string("io: mmap failed ") + path);

    return { static_cast<const char*>(data), static_cast<std::size_t>(sz) };
}

inline void skipSpaces(const char*& p, const char* end)
{
    while (p < end && (*p == ' ' || *p == '\t')) ++p;
}

inline void skipLine(const char*& p, const char* end)
{
    while (p < end && *p != '\n') ++p;
    if (p < end) ++p;
}

inline std::string readRestOfLine(const char*& p, const char* end)
{
    skipSpaces(p, end);
    const char* start = p;
    while (p < end && *p != '\n' && *p != '\r')
        ++p;
    const char* e = p;
    while (e > start && (*(e - 1) == ' ' || *(e - 1) == '\t')) --e;
    if (p < end) ++p;
    return {start, e};
}

inline float readFloat(const char*& p, const char* /*end*/)
{
    return std::strtof(p, const_cast<char**>(&p));
}



template<typename NumericT>
struct MtlEntry {
    sc::utils::Vec<NumericT, 3> kd{1, 1, 1};
    sc::utils::Vec<NumericT, 3> ka{0, 0, 0};
    sc::utils::Vec<NumericT, 3> ks{0, 0, 0};
    NumericT ns = 0;
    std::string mapKd;
    std::string mapNs;
    std::string mapBump;
};

template<typename NumericT>
std::unordered_map<std::string, MtlEntry<NumericT>>
parseMtlFile(const char* path, const std::string& baseDir)
{
    std::unordered_map<std::string, MtlEntry<NumericT>> materials;

    auto [ptr, size] = mmapFile(path);
    const char* end = ptr + size;
    const char* p = ptr;

    std::string currentName;

    while (p < end)
    {
        while (p < end && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r'))
            ++p;
        if (p >= end) break;

        if (std::strncmp(p, "newmtl", 6) == 0 && (p[6] == ' ' || p[6] == '\t'))
        {
            p += 6;
            currentName = readRestOfLine(p, end);
            materials[currentName] = MtlEntry<NumericT>{};
        }
        else if (p[0] == 'K' && p[1] == 'd' && (p[2] == ' ' || p[2] == '\t'))
        {
            p += 2;
            auto& m = materials[currentName];
            m.kd[0] = readFloat(p, end); m.kd[1] = readFloat(p, end); m.kd[2] = readFloat(p, end);
            skipLine(p, end);
        }
        else if (p[0] == 'K' && p[1] == 'a' && (p[2] == ' ' || p[2] == '\t'))
        {
            p += 2;
            auto& m = materials[currentName];
            m.ka[0] = readFloat(p, end); m.ka[1] = readFloat(p, end); m.ka[2] = readFloat(p, end);
            skipLine(p, end);
        }
        else if (p[0] == 'K' && p[1] == 's' && (p[2] == ' ' || p[2] == '\t'))
        {
            p += 2;
            auto& m = materials[currentName];
            m.ks[0] = readFloat(p, end); m.ks[1] = readFloat(p, end); m.ks[2] = readFloat(p, end);
            skipLine(p, end);
        }
        else if (p[0] == 'N' && p[1] == 's' && (p[2] == ' ' || p[2] == '\t'))
        {
            p += 2;
            materials[currentName].ns = readFloat(p, end);
            skipLine(p, end);
        }
        else if (std::strncmp(p, "map_Kd", 6) == 0 && (p[6] == ' ' || p[6] == '\t'))
        {
            p += 6;
            std::string texPath = readRestOfLine(p, end);
            materials[currentName].mapKd = resolvePath(baseDir, texPath);
        }
        else if (std::strncmp(p, "map_Ns", 6) == 0 && (p[6] == ' ' || p[6] == '\t'))
        {
            p += 6;
            std::string texPath = readRestOfLine(p, end);
            materials[currentName].mapNs = resolvePath(baseDir, texPath);
        }
        else if (std::strncmp(p, "map_Bump", 8) == 0 && (p[8] == ' ' || p[8] == '\t'))
        {
            p += 8;
            std::string texPath = readRestOfLine(p, end);
            materials[currentName].mapBump = resolvePath(baseDir, texPath);
        }
        else if (std::strncmp(p, "bump", 4) == 0 && (p[4] == ' ' || p[4] == '\t'))
        {
            p += 4;
            std::string texPath = readRestOfLine(p, end);
            materials[currentName].mapBump = resolvePath(baseDir, texPath);
        }
        else
        {
            skipLine(p, end);
        }
    }

    munmap(const_cast<char*>(ptr), size);
    return materials;
}



template<typename NumericT>
std::shared_ptr<Texture<NumericT>> loadTexture(const std::string& path)
{
    if (path.empty()) return nullptr;

    int w, h, channels;
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &channels, 3);
    if (!data) {
        std::cerr << "FATAL ERROR: Texture file not found or cannot be opened: " << path
                  << " (" << stbi_failure_reason() << ")\n";
        // Explicitly fail here - user REQUIREMENT: explicit assert/error, not silent fallback
        throw std::runtime_error("Failed to load texture: " + path);
    }

    // REQUIREMENT: Verify dimensions > 0
    if (w <= 0 || h <= 0) {
        std::cerr << "FATAL ERROR: Invalid texture dimensions: w=" << w << " h=" << h
                  << " path=" << path << "\n";
        throw std::runtime_error("Invalid texture dimensions: " + path);
    }

    // REQUIREMENT: Detailed loading logs - first 16 bytes in hex
    size_t pixelCount = static_cast<size_t>(w) * static_cast<size_t>(h);
    size_t bytesToCheck = std::min(static_cast<size_t>(16), pixelCount * 3);
    std::cerr << "TEXTURE LOAD SUCCESS: path=" << path
              << " w=" << w << " h=" << h << " channels=" << channels
              << " pixelCount=" << pixelCount << "\n";

    // Print first 16 bytes in hex
    std::cerr << "DATA (first 16 bytes): ";
    for (size_t i = 0; i < bytesToCheck; ++i) {
        std::cerr << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(data[i]);
        if ((i + 1) % 8 == 0) std::cerr << " ";
    }
    std::cerr << "\n";

    // REQUIREMENT: Control sum and min/max per channel
    uint64_t sumR = 0, sumG = 0, sumB = 0;
    uint8_t minR = 255, minG = 255, minB = 255;
    uint8_t maxR = 0, maxG = 0, maxB = 0;
    for (size_t i = 0; i < pixelCount * 3; i += 3) {
        sumR += data[i]; sumG += data[i + 1]; sumB += data[i + 2];
        minR = std::min(minR, data[i]);
        minG = std::min(minG, data[i + 1]);
        minB = std::min(minB, data[i + 2]);
        maxR = std::max(maxR, data[i]);
        maxG = std::max(maxG, data[i + 1]);
        maxB = std::max(maxB, data[i + 2]);
    }
    std::cerr << "CHANNEL R: sum=" << sumR << " min=" << static_cast<int>(minR)
              << " max=" << static_cast<int>(maxR) << "\n";
    std::cerr << "CHANNEL G: sum=" << sumG << " min=" << static_cast<int>(minG)
              << " max=" << static_cast<int>(maxG) << "\n";
    std::cerr << "CHANNEL B: sum=" << sumB << " min=" << static_cast<int>(minB)
              << " max=" << static_cast<int>(maxB) << "\n";

    auto tex = Texture<NumericT>::fromRawBytes(
        data, static_cast<std::size_t>(w), static_cast<std::size_t>(h));
    stbi_image_free(data);

    return std::make_shared<Texture<NumericT>>(std::move(tex));
}

template<typename NumericT>
Material<NumericT> buildMaterial(const MtlEntry<NumericT>& entry)
{
    Material<NumericT> mat;
    mat.baseColor = entry.kd;
    mat.ambient   = (entry.ka[0] + entry.ka[1] + entry.ka[2]) / NumericT(3);
    mat.specular  = (entry.ks[0] + entry.ks[1] + entry.ks[2]) / NumericT(3);
    mat.shininess = (entry.ns > 0) ? entry.ns : NumericT(32);

    // REQUIREMENT: Log which texture is being loaded
    std::cerr << "MATERIAL BUILD: Loading diffuse map_Kd=" << entry.mapKd << "\n";
    mat.diffuseMap = loadTexture<NumericT>(entry.mapKd);

    // Load optional maps with logging
    if (!entry.mapNs.empty()) {
        std::cerr << "MATERIAL BUILD: Loading roughness map_Ns=" << entry.mapNs << "\n";
        mat.roughnessMap = loadTexture<NumericT>(entry.mapNs);
    }
    if (!entry.mapBump.empty()) {
        std::cerr << "MATERIAL BUILD: Loading normal map_Bump=" << entry.mapBump << "\n";
        mat.normalMap = loadTexture<NumericT>(entry.mapBump);
    }

    return mat;
}



inline std::array<std::size_t, 3> parseFaceVertex(const char*& p, const char* /*end*/)
{
    constexpr std::size_t MISSING = SIZE_MAX;
    std::array<std::size_t, 3> r{MISSING, MISSING, MISSING};

    long v = std::strtol(p, const_cast<char**>(&p), 10);
    r[0] = static_cast<std::size_t>(v - 1);

    if (*p == '/') {
        ++p;
        if (*p != '/') {
            long vt = std::strtol(p, const_cast<char**>(&p), 10);
            r[1] = static_cast<std::size_t>(vt - 1);
        }
        if (*p == '/') {
            ++p;
            long vn = std::strtol(p, const_cast<char**>(&p), 10);
            r[2] = static_cast<std::size_t>(vn - 1);
        }
    }
    return r;
}

} // namespace detail



template<typename NumericT>
Model<NumericT> readFromObjFile(
    const char* path,
    const sc::utils::Vec<NumericT, 3>& pos = sc::utils::Vec<NumericT, 3>{0, 0, 0},
    const sc::utils::Vec<NumericT, 3>& rot = sc::utils::Vec<NumericT, 3>{0, 0, 0})
{
    using Face = typename ModelGeometryWithSubmeshes<NumericT>::Face;

    std::string baseDir = detail::extractDir(path);

    auto [ptr, size] = detail::mmapFile(path);
    const char* end = ptr + size;
    const char* p = ptr;

    std::vector<sc::utils::Vec<NumericT, 3>> vertices;
    std::vector<sc::utils::Vec<NumericT, 2>> uvs;
    std::vector<sc::utils::Vec<NumericT, 3>> normals;
    std::vector<Face> faces;

    std::string mtlLibPath;

    // Material tracking for submeshes
    std::string activeMatName;
    std::string pendingMatName;  // For usemtl encountered before any faces

    // Track face ranges for each material
    struct MaterialRange {
        std::string matName;
        size_t faceFirst;
        size_t faceCount;
    };
    std::vector<MaterialRange> materialRanges;
    size_t currentRangeStart = 0;

    while (p < end)
    {
        while (p < end && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r'))
            ++p;
        if (p >= end) break;

        if (*p == '#')
        {
            detail::skipLine(p, end);
        }
        else if (std::strncmp(p, "mtllib", 6) == 0 && (p[6] == ' ' || p[6] == '\t'))
        {
            p += 6;
            mtlLibPath = detail::readRestOfLine(p, end);
        }
        else if (std::strncmp(p, "usemtl", 6) == 0 && (p[6] == ' ' || p[6] == '\t'))
        {
            p += 6;
            pendingMatName = detail::readRestOfLine(p, end);

            // Close current range if there are faces in it
            if (faces.size() > currentRangeStart && !activeMatName.empty()) {
                materialRanges.push_back({activeMatName, currentRangeStart, faces.size() - currentRangeStart});
                currentRangeStart = faces.size();
            }
            // Set the new active material
            activeMatName = pendingMatName;
        }
        else if (p[0] == 'v' && p[1] == 'n' && (p[2] == ' ' || p[2] == '\t'))
        {
            p += 2;
            NumericT x = detail::readFloat(p, end);
            NumericT y = detail::readFloat(p, end);
            NumericT z = detail::readFloat(p, end);
            normals.emplace_back(x, y, z);
            detail::skipLine(p, end);
        }
        else if (p[0] == 'v' && p[1] == 't' && (p[2] == ' ' || p[2] == '\t'))
        {
            p += 2;
            NumericT u = detail::readFloat(p, end);
            NumericT v = detail::readFloat(p, end);
            uvs.emplace_back(u, v);
            detail::skipLine(p, end);
        }
        else if (p[0] == 'v' && (p[1] == ' ' || p[1] == '\t'))
        {
            p += 1;
            NumericT x = detail::readFloat(p, end);
            NumericT y = detail::readFloat(p, end);
            NumericT z = detail::readFloat(p, end);
            vertices.emplace_back(x, y, z);
            detail::skipLine(p, end);
        }
        else if (p[0] == 'f' && (p[1] == ' ' || p[1] == '\t'))
        {
            p += 1;

            std::vector<std::array<std::size_t, 3>> faceVerts;
            while (p < end && *p != '\n' && *p != '\r')
            {
                detail::skipSpaces(p, end);
                if (p >= end || *p == '\n' || *p == '\r') break;
                faceVerts.push_back(detail::parseFaceVertex(p, end));
            }
            detail::skipLine(p, end);

            for (std::size_t i = 2; i < faceVerts.size(); ++i)
            {
                Face face{};
                face[0] = faceVerts[0];
                face[1] = faceVerts[i - 1];
                face[2] = faceVerts[i];
                faces.push_back(face);
            }
        }
        else
        {
            detail::skipLine(p, end);
        }
    }

    munmap(const_cast<char*>(ptr), size);

    // Close final range
    if (faces.size() > currentRangeStart) {
        materialRanges.push_back({activeMatName, currentRangeStart, faces.size() - currentRangeStart});
    } else if (!activeMatName.empty() && materialRanges.empty()) {
        // usemtl with no faces - ignore it
    }

    ModelGeometryWithSubmeshes<NumericT> geometry;
    geometry.verticies() = std::move(vertices);
    geometry.uv()        = std::move(uvs);
    geometry.normals()   = std::move(normals);
    geometry.faces()     = std::move(faces);
    geometry.pos()       = pos;
    geometry.rot()       = rot;

    Model<NumericT> model;
    model.geometry = std::move(geometry);

    // Default material for faces before first usemtl or unknown materials
    Material<NumericT> defaultMaterial;
    defaultMaterial.baseColor = sc::utils::Vec<NumericT, 3>{0.7f, 0.7f, 0.7f};
    defaultMaterial.ambient = 0.1f;
    defaultMaterial.specular = 0.5f;
    defaultMaterial.shininess = 32.0f;
    model.material = defaultMaterial;

    // Load MTL file and create submeshes
    std::unordered_map<std::string, detail::MtlEntry<NumericT>> mtlMap;
    if (!mtlLibPath.empty())
    {
        std::string fullMtlPath = detail::resolvePath(baseDir, mtlLibPath);
        try {
            mtlMap = detail::parseMtlFile<NumericT>(fullMtlPath.c_str(), baseDir);
        }
        catch (const std::exception& e) {
            std::cerr << "io: warning: could not load MTL: " << e.what() << "\n";
        }
    }

    // Build materials vector and name->index map
    model.matByName.clear();
    model.materials.clear();

    for (const auto& [name, entry] : mtlMap) {
        model.matByName[name] = model.materials.size();
        model.materials.push_back(detail::buildMaterial(entry));
    }

    // Set backward-compatible material to first loaded material, or default
    if (!model.materials.empty()) {
        model.material = model.materials[0];
    }

    // Build submeshes from material ranges
    for (const auto& range : materialRanges) {
        if (range.faceCount == 0) {
            continue;  // Skip empty ranges (usemtl without faces)
        }

        Submesh<NumericT> submesh;
        submesh.faceFirst = range.faceFirst;
        submesh.faceCount = range.faceCount;
        submesh.materialName = range.matName;

        // Look up material index
        auto it = model.matByName.find(range.matName);
        if (it != model.matByName.end()) {
            submesh.materialIndex = static_cast<int>(it->second);
        } else {
            submesh.materialIndex = -1;  // Use default material
        }

        model.submeshes().push_back(submesh);
    }

#ifdef DEBUG_SUBMESH_LOADING
    std::cout << "DEBUG: Loaded model with " << model.submeshes().size() << " submeshes\n";
    for (size_t i = 0; i < model.submeshes().size(); ++i) {
        const auto& sm = model.submeshes()[i];
        std::cout << "  Submesh " << i << ": faceFirst=" << sm.faceFirst
                  << ", faceCount=" << sm.faceCount
                  << ", materialName=" << sm.materialName
                  << ", materialIndex=" << sm.materialIndex << "\n";
    }
#endif

    return model;
}

} // namespace mrc::io


namespace mrc {
    using io::readFromObjFile;
} // namespace mrc
