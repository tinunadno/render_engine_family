#include "entry_point.h"
#include "text_io/tio.h"

int main()
{

    sc::Camera<float, sc::VecArray> camera;
    const std::string fontPath = "/Library/Fonts/Arial Unicode.ttf";

    auto textRenderer = tr::TextRenderer(fontPath, 20.f);
    tio::TextTerminal tt(std::move(textRenderer));
    sc::GLFWRenderer* renderer_ = nullptr;
    bool terminalActive = false;

    auto ff = [&camera, &tt, &renderer_, &terminalActive](sc::GLFWRenderer& renderer, std::size_t frame, std::size_t time)
    {
        if (!renderer_) renderer_ = &renderer;
        for (const auto& ps : sc::makeViewFromCamera(camera))
        {
            sc::utils::Vec<float, 3> color = sc::utils::Vec<float, 3>{0.f, 0.f, 0.f};
            renderer.setPixel(ps.pixelX, ps.pixelY, color);
        }
        if (terminalActive) tt.render(renderer);
    };

    auto commandCallback = [&tt, &terminalActive](const std::string& s) {
        if (s.empty()) terminalActive = false;
        tt.print(s);
    };
    tt.setCallback(commandCallback);

    auto readHandler = [&tt] (sc::GLFWRenderer& renderer, int key) {
        tt.readChar(renderer, key);
    };

    std::vector<std::pair<std::vector<int>, std::function<void()>>> customKeyHandlers = {
        {{GLFW_KEY_LEFT_ALT, GLFW_KEY_T}, [&renderer_, &readHandler, &terminalActive]() {
            if (renderer_) {
                renderer_->setInputHandler(readHandler);
                renderer_->startInput();
                terminalActive = true;
            }
        } }
    };

    sc::initPerFrameRender(camera, ff, {}, customKeyHandlers);

    return 0;
}
