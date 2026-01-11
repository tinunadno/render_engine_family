#pragma once

namespace rmc::curvature
{

template<typename NumericT>
class ICurvature
{
public:
    virtual ~ICurvature() = default;
    virtual void deflect(
        const sc::utils::Vec<NumericT,3>& pos,
        sc::utils::Vec<NumericT,3>& dir,
        NumericT& stepSize
    ) = 0;
};

} // namespace rmc::curvature