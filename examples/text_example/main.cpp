#include "entry_point.h"
#include "text_render/tr.h"

int main() {

    sc::Camera<float, sc::VecArray> camera;

    const std::string fontPath = "/Library/Fonts/Arial Unicode.ttf";

    auto textRenderer = tr::TextRenderer(fontPath, 120.f);
    textRenderer.padding()[0] = 1;
    textRenderer.padding()[1] = 1;

    auto sf = [](const sc::PixelSample<float>&, std::size_t, std::size_t)
    {
        return sc::utils::Vec<float, 3>{1.f, 1.f, 1.f};
    };

    auto ff = [&camera, &sf, &textRenderer](sc::GLFWRenderer& renderer, std::size_t frame, std::size_t time)
    {
        for (const auto& ps : sc::makeViewFromCamera(camera))
        {
            sc::utils::Vec<float, 3> color = sf(ps, frame, time);
            renderer.setPixel(ps.pixelX, ps.pixelY, color);
        }
        textRenderer.renderText(renderer, "Hello, world!\nASDASDASDASD\nTESTTEST", sc::utils::Vec<float, 3>{1.f, 0.f, 0.f});
    };
    sc::initPerFrameRender(camera, ff);
}