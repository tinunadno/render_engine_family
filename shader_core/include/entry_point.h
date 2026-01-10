#pragma once

#include "camera/camera.h"
#include "render.h"
#include "glfw_render.h"

#include <chrono>
#include <thread>

#ifdef ENABLE_LOG
#include <iostream>
#endif

namespace sc
{

template<typename T, std::size_t N>
using VecArray = utils::Vec<T, N>;


template<typename NumericT, typename ShadeFunction>
void initRender(Camera<NumericT, VecArray>& camera, ShadeFunction sf)
{
    using namespace std::chrono_literals;

    GLFWRenderer renderer(
        static_cast<int>(camera.res()[0])
        , static_cast<int>(camera.res()[1]));

    std::size_t time = 0;
    std::size_t frame = 0;
    while (!renderer.shouldClose())
    {
        auto start = std::chrono::system_clock::now();
        render(renderer, sc::makeViewFromCamera(camera), frame++, time,sf);
        renderer.show();
        auto end = std::chrono::system_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        time += elapsedTime;
#ifdef ENABLE_LOG
        std::cout << "[FRAME]: " << frame << " ";
        std::cout << "[TIME ms]:  " << time << " ";
        std::cout << "[RENDER TIME ms]: " << elapsedTime << " ";
        std::cout << "[FPS]: " << 1000. / static_cast<float>(elapsedTime) << "\n";
#endif
        std::this_thread::sleep_for(10ms); // TODO
    }
}

} // namespace sc