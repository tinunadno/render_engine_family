#pragma once

#pragma once

#include <GLFW/glfw3.h>
#include <vector>
#include <cassert>

#include "utils/vec.h"

namespace sc
{

class GLFWRenderer
{
public:
    GLFWRenderer(int width, int height, const char* title = "Renderer")
        : _width(width), _height(height), _buffer(width * height * 3, 0)
    {
        if (!glfwInit())
            throw std::runtime_error("GLFW init failed");

        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        _window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title, nullptr, nullptr);
        if (!_window)
        {
            glfwTerminate();
            throw std::runtime_error("GLFW window creation failed");
        }

        glfwMakeContextCurrent(_window);


        glViewport(0, 0, static_cast<int>(width), static_cast<int>(height));
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, width, 0, height, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }

    ~GLFWRenderer()
    {
        if (_window)
            glfwDestroyWindow(_window);
        glfwTerminate();
    }


    void setPixel(std::size_t x, std::size_t y, const utils::Vec<float, 3>& color)
    {
        assert(x < _width && y < _height);
        std::size_t idx = (_height - 1 - y) * _width * 3 + x * 3;
        _buffer[idx + 0] = static_cast<unsigned char>(std::clamp(color[0], 0.f, 1.f) * 255.f);
        _buffer[idx + 1] = static_cast<unsigned char>(std::clamp(color[1], 0.f, 1.f) * 255.f);
        _buffer[idx + 2] = static_cast<unsigned char>(std::clamp(color[2], 0.f, 1.f) * 255.f);
    }


    void show()
    {
        if (!_window) return;

        glClear(GL_COLOR_BUFFER_BIT);
        glDrawPixels(static_cast<GLsizei>(_width), static_cast<GLsizei>(_height),
                     GL_RGB, GL_UNSIGNED_BYTE, _buffer.data());

        glfwSwapBuffers(_window);
        glfwPollEvents();
    }

    [[nodiscard]] bool shouldClose() const
    {
        return _window ? glfwWindowShouldClose(_window) : true;
    }

private:
    std::size_t _width, _height;
    std::vector<unsigned char> _buffer;
    GLFWwindow* _window = nullptr;
};

} // namespace sc