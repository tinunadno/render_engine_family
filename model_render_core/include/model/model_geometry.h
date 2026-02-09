#pragma once

#include <utils/vec.h>

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

}