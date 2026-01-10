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
    GLFWRenderer(int renderResX, int renderResY, const utils::Vec<int, 2>& windowRes, const char* title = "Renderer")
        : _renderWidth(renderResX)
        , _renderHeight(renderResY)
        , _windowWidth(windowRes[0])
        , _windowHeight(windowRes[1])
        , _buffer(renderResX * renderResY * 3, 0)
    {
        if (!glfwInit())
            throw std::runtime_error("GLFW init failed");

        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        _window = glfwCreateWindow(_windowWidth, _windowHeight, title, nullptr, nullptr);
        if (!_window)
        {
            glfwTerminate();
            throw std::runtime_error("GLFW window creation failed");
        }

        glfwMakeContextCurrent(_window);
        glViewport(0, 0, _windowWidth, _windowHeight);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, 1, 0, 1, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glEnable(GL_TEXTURE_2D);
        glGenTextures(1, &_textureId);
        glBindTexture(GL_TEXTURE_2D, _textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }

    ~GLFWRenderer()
    {
        if (_textureId)
            glDeleteTextures(1, &_textureId);
        if (_window)
            glfwDestroyWindow(_window);
        glfwTerminate();
    }

    void setPixel(std::size_t x, std::size_t y, const utils::Vec<float, 3>& color)
    {
        assert(x < _renderWidth && y < _renderHeight);
        std::size_t idx = (_renderHeight - 1 - y) * _renderWidth * 3 + x * 3;

        _buffer[idx + 0] = static_cast<unsigned char>(std::clamp(color[0], 0.f, 1.f) * 255.f);
        _buffer[idx + 1] = static_cast<unsigned char>(std::clamp(color[1], 0.f, 1.f) * 255.f);
        _buffer[idx + 2] = static_cast<unsigned char>(std::clamp(color[2], 0.f, 1.f) * 255.f);
    }

    void show() const
    {
        if (!_window) return;

        glClear(GL_COLOR_BUFFER_BIT);
        glBindTexture(GL_TEXTURE_2D, _textureId);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                     static_cast<GLsizei>(_renderWidth), static_cast<GLsizei>(_renderHeight),
                     0, GL_RGB, GL_UNSIGNED_BYTE, _buffer.data());
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 0.0f);
            glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, 0.0f);
            glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 1.0f);
            glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, 1.0f);
        glEnd();

        glfwSwapBuffers(_window);
        glfwPollEvents();
    }

    [[nodiscard]] bool shouldClose() const
    {
        return _window ? glfwWindowShouldClose(_window) : true;
    }
    [[nodiscard]] std::size_t getRenderWidth() const { return _renderWidth; }
    [[nodiscard]] std::size_t getRenderHeight() const { return _renderHeight; }

private:
    std::size_t _renderWidth, _renderHeight;
    int _windowWidth, _windowHeight;

    std::vector<unsigned char> _buffer;
    GLFWwindow* _window = nullptr;
    unsigned int _textureId = 0;
};

} // namespace sc