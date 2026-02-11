#pragma once

#include "utils/vec.h"

struct Snapshot
{
    std::vector<sc::utils::Vec<float, 2>> roi;
    std::vector<std::uint8_t> renderBuffer;
    Snapshot(const std::vector<sc::utils::Vec<float, 2>>& roi, const std::vector<std::uint8_t>& renderBuffer)
        : roi(roi), renderBuffer(std::move(renderBuffer)) { }
    Snapshot(): roi(), renderBuffer() { }
};