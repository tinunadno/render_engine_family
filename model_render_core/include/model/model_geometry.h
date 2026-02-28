#pragma once

#include <utils/vec.h>
#include <string>
#include <vector>

namespace mrc {

template<typename NumericT>
class ModelGeometry
{
public:

    using Face = std::array<std::array<std::size_t, 3>, 3>;

    ModelGeometry():
        _verticies(),
        _uv(),
        _normals(),
        _faces(),
        _pos(),
        _rot()
    {}

    ~ModelGeometry() = default;

    ModelGeometry(const ModelGeometry& model):
        _verticies(model._verticies),
        _uv(model._uv),
        _normals(model._normals),
        _faces(model._faces),
        _pos(model._pos),
        _rot(model._rot)
    {}

    ModelGeometry(ModelGeometry&& model) noexcept:
        _verticies(std::move(model._verticies)),
        _uv(std::move(model._uv)),
        _normals(std::move(model._normals)),
        _faces(std::move(model._faces)),
        _pos(std::move(model._pos)),
        _rot(std::move(model._rot))
    {}

    ModelGeometry& operator=(const ModelGeometry& model) = default;

    const std::vector<sc::utils::Vec<NumericT, 3>>& verticies() const {return _verticies;}
    const std::vector<sc::utils::Vec<NumericT, 2>>& uv() const {return _uv;}
    const std::vector<sc::utils::Vec<NumericT, 3>>& normals() const {return _normals;}
    [[nodiscard]] const std::vector<Face>& faces() const {return _faces;}
    const sc::utils::Vec<NumericT, 3>& pos() const {return _pos;}
    const sc::utils::Vec<NumericT, 3>& rot() const {return _rot;}

    std::vector<sc::utils::Vec<NumericT, 3>>& verticies() {return _verticies;}
    std::vector<sc::utils::Vec<NumericT, 2>>& uv() {return _uv;}
    std::vector<sc::utils::Vec<NumericT, 3>>& normals() {return _normals;}
    [[nodiscard]] std::vector<Face>& faces() {return _faces;}
    sc::utils::Vec<NumericT, 3>& pos() {return _pos;}
    sc::utils::Vec<NumericT, 3>& rot() {return _rot;}

    std::array<sc::utils::Vec<NumericT, 3>, 3> getPolygon(std::size_t faceIdx,
        const std::vector<sc::utils::Vec<NumericT, 3>>& vertSource) const {
        return {vertSource[_faces[faceIdx][0][0]],
                vertSource[_faces[faceIdx][1][0]],
                vertSource[_faces[faceIdx][2][0]]};
    }

private:

    std::vector<sc::utils::Vec<NumericT, 3>> _verticies;
    std::vector<sc::utils::Vec<NumericT, 2>> _uv;
    std::vector<sc::utils::Vec<NumericT, 3>> _normals;
    std::vector<Face> _faces;

    sc::utils::Vec<NumericT, 3> _pos;
    sc::utils::Vec<NumericT, 3> _rot;
};

/// Submesh: a range of faces with an associated material.
/// materialIndex = -1 means use default material.
template<typename NumericT>
struct Submesh {
    size_t faceFirst;
    size_t faceCount;
    int materialIndex;  // -1 for default, >=0 for materials[index]
    std::string materialName;
};

/// ModelGeometryWithSubmeshes: adds submesh support to ModelGeometry.
/// This is used by Model for multi-material rendering.
template<typename NumericT>
class ModelGeometryWithSubmeshes
{
public:

    using Face = std::array<std::array<std::size_t, 3>, 3>;

    ModelGeometryWithSubmeshes():
        _verticies(),
        _uv(),
        _normals(),
        _faces(),
        _submeshes(),
        _pos(),
        _rot()
    {}

    ~ModelGeometryWithSubmeshes() = default;

    ModelGeometryWithSubmeshes(const ModelGeometryWithSubmeshes& model):
        _verticies(model._verticies),
        _uv(model._uv),
        _normals(model._normals),
        _faces(model._faces),
        _submeshes(model._submeshes),
        _pos(model._pos),
        _rot(model._rot)
    {}

    ModelGeometryWithSubmeshes(ModelGeometryWithSubmeshes&& model) noexcept:
        _verticies(std::move(model._verticies)),
        _uv(std::move(model._uv)),
        _normals(std::move(model._normals)),
        _faces(std::move(model._faces)),
        _submeshes(std::move(model._submeshes)),
        _pos(std::move(model._pos)),
        _rot(std::move(model._rot))
    {}

    ModelGeometryWithSubmeshes& operator=(const ModelGeometryWithSubmeshes& model) = default;

    // Compatibility: can convert from ModelGeometry
    explicit ModelGeometryWithSubmeshes(const ModelGeometry<NumericT>& geo):
        _verticies(geo.verticies()),
        _uv(geo.uv()),
        _normals(geo.normals()),
        _faces(geo.faces()),
        _submeshes(),
        _pos(geo.pos()),
        _rot(geo.rot())
    {}

    const std::vector<sc::utils::Vec<NumericT, 3>>& verticies() const {return _verticies;}
    const std::vector<sc::utils::Vec<NumericT, 2>>& uv() const {return _uv;}
    const std::vector<sc::utils::Vec<NumericT, 3>>& normals() const {return _normals;}
    [[nodiscard]] const std::vector<Face>& faces() const {return _faces;}
    const sc::utils::Vec<NumericT, 3>& pos() const {return _pos;}
    const sc::utils::Vec<NumericT, 3>& rot() const {return _rot;}

    std::vector<sc::utils::Vec<NumericT, 3>>& verticies() {return _verticies;}
    std::vector<sc::utils::Vec<NumericT, 2>>& uv() {return _uv;}
    std::vector<sc::utils::Vec<NumericT, 3>>& normals() {return _normals;}
    [[nodiscard]] std::vector<Face>& faces() {return _faces;}
    sc::utils::Vec<NumericT, 3>& pos() {return _pos;}
    sc::utils::Vec<NumericT, 3>& rot() {return _rot;}

    const std::vector<Submesh<NumericT>>& submeshes() const {return _submeshes;}
    std::vector<Submesh<NumericT>>& submeshes() {return _submeshes;}

    std::array<sc::utils::Vec<NumericT, 3>, 3> getPolygon(std::size_t faceIdx,
        const std::vector<sc::utils::Vec<NumericT, 3>>& vertSource) const {
        return {vertSource[_faces[faceIdx][0][0]],
                vertSource[_faces[faceIdx][1][0]],
                vertSource[_faces[faceIdx][2][0]]};
    }

private:

    std::vector<sc::utils::Vec<NumericT, 3>> _verticies;
    std::vector<sc::utils::Vec<NumericT, 2>> _uv;
    std::vector<sc::utils::Vec<NumericT, 3>> _normals;
    std::vector<Face> _faces;
    std::vector<Submesh<NumericT>> _submeshes;

    sc::utils::Vec<NumericT, 3> _pos;
    sc::utils::Vec<NumericT, 3> _rot;
};

} // namespace mrc
