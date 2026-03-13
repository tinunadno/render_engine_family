#pragma once
#include <string>
#include <cstdio>
#include <algorithm>

#ifndef STB_TRUETYPE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#endif
#include "stb/stb_truetype.h"

namespace tr {

struct SymbolCache {
    int glyph_index;
    int x0, y0, x1, y1;
    unsigned char* data;
};

struct TextRenderSettings {
    sc::utils::Vec<float, 2> padding;
    float pt;
    TextRenderSettings() {
        padding = sc::utils::Vec<float, 2>{0, 0};
        pt = 20.0f;
    }
};

class TextRenderer {
public:
    TextRenderer() : _info(), _ttfBuffer(nullptr), _settings(), _scale(0), _ascent(0), _descent(0), _line_gap(0) {}

    TextRenderer(const std::string& ttfPath, float pt) : TextRenderer() {
        _settings.pt = pt;
        FILE* file = fopen(ttfPath.c_str(), "rb");
        if (!file) return;

        fseek(file, 0, SEEK_END);
        long size = ftell(file);
        fseek(file, 0, SEEK_SET);

        _ttfBuffer = static_cast<unsigned char*>(malloc(size));
        if (!_ttfBuffer) {
            fclose(file);
            return;
        }
        fread(_ttfBuffer, 1, size, file);
        fclose(file);

        if (!stbtt_InitFont(&_info, _ttfBuffer, 0)) {
            free(_ttfBuffer);
            _ttfBuffer = nullptr;
            return;
        }

        updateMetrics(pt);
    }

    void changeFontSize(float pt) {
        if (_ttfBuffer) updateMetrics(pt);
    }

    void updateMetrics(float pt) {
        _settings.pt = pt;
        _scale = stbtt_ScaleForPixelHeight(&_info, pt);
        stbtt_GetFontVMetrics(&_info, &_ascent, &_descent, &_line_gap);
    }

    [[nodiscard]] float getMaxPt(const sc::utils::Vec<float, 2>& targetSize) const {
        float target_line_height = std::min(targetSize[0], targetSize[1]);

        int units_total_line = _ascent - _descent + _line_gap;
        if (units_total_line <= 0) return 0.0f;

        float target_scale = target_line_height / static_cast<float>(units_total_line);

        float font_size_px = static_cast<float>(_ascent - _descent) * target_scale;

        return font_size_px * 0.75f;
    }

    void renderText(sc::GLFWRenderer& renderer, const std::string& text,
                    const sc::utils::Vec<float, 3>& color,
                    sc::utils::Vec<int, 2> targetPos = sc::utils::Vec<int, 2>{0, 0}) {
        if (!_ttfBuffer) return;

        int x_pos = targetPos[0] + _settings.padding[0];
        int y_pos = targetPos[1] + _settings.padding[1];


        int nlCount = 1;
        for (auto s : text) {
            if (s == '\n') nlCount++;
        }
        std::vector<int> paddings;
        paddings.reserve(nlCount);
        int maxY = -std::numeric_limits<int>::max();
        for (auto s : text) {
            SymbolCache symbol;
            if (_symbolCache.find(s) != _symbolCache.end()) {
                symbol = _symbolCache[s];
            } else {
                symbol = readSymbolInfo(s);
                _symbolCache[s] = symbol;
            }
            maxY = std::max(maxY, symbol.y1 - symbol.y0);
            if (s == '\n') {
                paddings.emplace_back(maxY);
                maxY = -std::numeric_limits<int>::max();
            }
        }
        paddings.emplace_back(maxY);

        int paddingIdx = 0;
        for (auto s : text) {
            if (s == '\n') {
                y_pos += paddings[paddingIdx] + _settings.padding[1];
                paddingIdx++;
                x_pos = targetPos[0];
                continue;
            }
            SymbolCache symbol = _symbolCache[s];

            int glyph_w = symbol.x1 - symbol.x0;
            int glyph_h = symbol.y1 - symbol.y0;

            int w = static_cast<int>(renderer.getRenderWidth());
            int h = static_cast<int>(renderer.getRenderHeight());
            int symbolYDiff = paddings[paddingIdx] - glyph_h;
            for (int y = 0; y < glyph_h; y++) {
                for (int x = 0; x < glyph_w; x++) {
                    int out_x = x_pos + x;
                    int out_y = y_pos + y + symbolYDiff;
                    if (out_x >= 0 && out_x < w && out_y >= 0 && out_y < h) {
                        auto val = symbol.data[y * glyph_w + x];
                        if (val) {
                            float scalar = static_cast<float>(val + 128) / 256.f;
                            auto old_color = renderer.getPixel(out_x, out_y);
                            renderer.setPixel(out_x, out_y,
                                color * scalar + old_color * (1.0f - scalar)
                                );
                        }
                    }
                }
            }
            x_pos += glyph_w + _settings.padding[0];
        }
    }

    sc::utils::Vec<float, 2>& padding() { return _settings.padding; }

    ~TextRenderer() {
        if (_ttfBuffer) free(_ttfBuffer);
        for (const auto& it : _symbolCache) {
            free(it.second.data);
        }
    }

private:

    SymbolCache readSymbolInfo(char s) {
        int glyph_index = stbtt_FindGlyphIndex(&_info, s);
        if (glyph_index == 0) return {};

        int x0, y0, x1, y1;
        stbtt_GetGlyphBitmapBox(&_info, glyph_index, _scale, _scale, &x0, &y0, &x1, &y1);

        int glyph_w = x1 - x0;
        int glyph_h = y1 - y0;

        auto temp_glyph = static_cast<unsigned char*>(calloc(glyph_w * glyph_h, 1));
        if (!temp_glyph) return {};

        stbtt_MakeGlyphBitmap(&_info, temp_glyph,
                              glyph_w, glyph_h,
                              glyph_w, _scale,
                              _scale, glyph_index);

        return {glyph_index,  x0, y0, x1, y1,  temp_glyph};
    }

    [[nodiscard]] int get_font_v_metrics_px() const {
        return (_ascent - _descent + _line_gap) * _scale;
    }

    std::unordered_map<char, SymbolCache> _symbolCache;

    stbtt_fontinfo _info;
    unsigned char* _ttfBuffer;
    TextRenderSettings _settings;
    float _scale;
    int _ascent;
    int _descent;
    int _line_gap;
};

} // namespace tr