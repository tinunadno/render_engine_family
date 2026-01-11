#pragma once

#include "icurvature.h"
#include "utils/vec.h"

namespace rmc::curvature
{

template<typename NumericT>
struct BlackHole : public curvature::ICurvature<NumericT>
{
    sc::utils::Vec<NumericT, 3> center;
    NumericT schwarzschildRadius; // Радиус горизонта событий
    NumericT influenceRadius;     // Радиус, где начинаем считать (оптимизация)
    NumericT strength;            // Множитель силы

    void deflect(
        const sc::utils::Vec<NumericT,3>& pos,
        sc::utils::Vec<NumericT,3>& dir,
        NumericT& stepSize) override
    {
        auto rVec = center - pos;
        NumericT dist = sc::utils::len(rVec);

        // Оптимизация: если далеко, ничего не делаем
        if (dist > influenceRadius) return;

        // 1. Ограничиваем шаг (STEP CONTROL)
        // Мы не можем шагнуть на 100км, если находимся в 10км от дыры.
        // Эвристика: шаг не больше 5-10% от расстояния до центра дыры.
        NumericT maxStep = dist * NumericT(0.02);
        if (stepSize > maxStep)
        {
            stepSize = maxStep;
        }

        // 2. Искривляем луч (BENDING)
        // Сила гравитации ~ 1/r^2.
        // Мы поворачиваем вектор Dir в сторону центра.
        // Коэффициент зависит от stepSize (чем дольше летим, тем сильнее поворачиваем).
        NumericT force = strength / (dist * dist + NumericT(1e-6));

        // dir += vectorToCenter * force * stepSize
        // (Это упрощенная интеграция Эйлера)
        dir += sc::utils::norm(rVec) * (force * stepSize);
        dir = sc::utils::norm(dir); // Обязательно нормализуем обратно
    }
};

} // namespace rmc::curvature