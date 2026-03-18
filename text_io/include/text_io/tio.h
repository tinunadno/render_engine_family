#pragma once

#include "text_render/tr.h"
#include "utils/vec.h"
#include "glfw_render.h"

namespace tio {

namespace internal {
char keyToChar(int key, int shiftPressed = false) {

    if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z) {
        char baseChar = 'a' + (key - GLFW_KEY_A);
        return shiftPressed ? std::toupper(baseChar) : baseChar;
    }


    switch(key) {

        case GLFW_KEY_0: return shiftPressed ? ')' : '0';
        case GLFW_KEY_1: return shiftPressed ? '!' : '1';
        case GLFW_KEY_2: return shiftPressed ? '@' : '2';
        case GLFW_KEY_3: return shiftPressed ? '#' : '3';
        case GLFW_KEY_4: return shiftPressed ? '$' : '4';
        case GLFW_KEY_5: return shiftPressed ? '%' : '5';
        case GLFW_KEY_6: return shiftPressed ? '^' : '6';
        case GLFW_KEY_7: return shiftPressed ? '&' : '7';
        case GLFW_KEY_8: return shiftPressed ? '*' : '8';
        case GLFW_KEY_9: return shiftPressed ? '(' : '9';


        case GLFW_KEY_SPACE:         return ' ';
        case GLFW_KEY_APOSTROPHE:    return shiftPressed ? '"' : '\'';
        case GLFW_KEY_COMMA:         return shiftPressed ? '<' : ',';
        case GLFW_KEY_MINUS:          return shiftPressed ? '_' : '-';
        case GLFW_KEY_PERIOD:         return shiftPressed ? '>' : '.';
        case GLFW_KEY_SLASH:          return shiftPressed ? '?' : '/';
        case GLFW_KEY_SEMICOLON:      return shiftPressed ? ':' : ';';
        case GLFW_KEY_EQUAL:          return shiftPressed ? '+' : '=';
        case GLFW_KEY_LEFT_BRACKET:   return shiftPressed ? '{' : '[';
        case GLFW_KEY_BACKSLASH:      return shiftPressed ? '|' : '\\';
        case GLFW_KEY_RIGHT_BRACKET:  return shiftPressed ? '}' : ']';
        case GLFW_KEY_GRAVE_ACCENT:   return shiftPressed ? '~' : '`';


        case GLFW_KEY_ENTER:          return '\n';
        case GLFW_KEY_TAB:            return '\t';
        case GLFW_KEY_BACKSPACE:      return '\b';
        case GLFW_KEY_ESCAPE:         return 0x1B;

        default:
            return 0x0;
    }
}

struct InputBuilder {
    std::deque<char> back;
    std::deque<char> front;
    void moveLeft() {
        if (back.empty()) return;
        char c = back.back();
        front.push_front(c);
        back.pop_back();
    }
    void moveRight() {
        if (front.empty()) return;
        char c = front.front();
        back.push_back(c);
        front.pop_front();
    }
    std::string buildLine(bool drawCursor = false) const {
        std::string ret;
        for (const auto c : back) { ret += c; }
        if (drawCursor) ret += '|';
        for (const auto c : front) { ret += c; }
        return ret;
    }
    void addSymbol(char c) {
        back.push_back(c);
    }
    void rmSymbol() {
        if (back.empty()) return;
        back.pop_back();
    }
    void clear() {
        back.clear();
        front.clear();
    }
    void fromString(const std::string& text) {
        clear();
        back = std::deque(text.begin(), text.end());
    }
};

}

class TextTerminal {
public:

    using finalizeCallback = std::function<void(const std::string&)>;

    TextTerminal();
    TextTerminal(tr::TextRenderer&& tr_): _renderer(tr_),
                                          _terminalColor(sc::utils::Vec<float, 3>{0, 1.f, 0}) {}

    void print(const std::string& text) {
        if (_buffer.size() > _maxLines) {
            _buffer.pop_front();
        }
        _buffer.push_back(text);
    }

    void clear() {
        _buffer.clear();
    }
    void readChar(sc::GLFWRenderer& renderer, int key) {
        if (key == GLFW_KEY_ESCAPE) {
            renderer.stopInput();
            if (_callback) _callback("");
        }
        if (key == GLFW_KEY_ENTER) {
            appendStory();
            if (_callback) _callback(_story.front());
            return;
        }
        if (key == GLFW_KEY_BACKSPACE) { _inputBuilder.rmSymbol(); return; }
        if (key == GLFW_KEY_LEFT) { _inputBuilder.moveLeft(); return; }
        if (key == GLFW_KEY_RIGHT) { _inputBuilder.moveRight(); return; }
        if (key == GLFW_KEY_UP) { moveStory(); return; }
        if (key == GLFW_KEY_DOWN) { moveStory(false); return; }
        char s = internal::keyToChar(key);
        if (s == '\0') return;
        _inputBuilder.addSymbol(s);
    }

    void render(sc::GLFWRenderer& renderer) {
        std::string buffer;
        for (const auto& ln : _buffer) {
            buffer += ln + "\n";
        }
        buffer += _inputBuilder.buildLine(true);
        _renderer.renderText(renderer, buffer, _terminalColor);
    }

    void setMaxLines(int lines) {
        _maxLines = lines;
    }

    void setColor(const sc::utils::Vec<float, 3>& color) {
        _terminalColor = color;
    }
    void setCallback(const finalizeCallback& callback) {
        _callback = callback;
    }
private:

    void moveStory(bool up = true) {
        if (up) {
            _inputBuilder.fromString(_story.front());
            _story.push_back(_story.front());
            _story.pop_front();
        } else {
            _inputBuilder.fromString(_story.back());
            _story.push_front(_story.back());
            _story.pop_back();
        }
    }

    void appendStory() {
        if (_story.size() > _maxStory) {
            _story.pop_back();
        }
        _story.push_front(_inputBuilder.buildLine(false));
        _inputBuilder.clear();
    }

    finalizeCallback _callback;
    tr::TextRenderer _renderer;
    std::deque<std::string> _buffer;
    internal::InputBuilder _inputBuilder;
    std::deque<std::string> _story;
    sc::utils::Vec<float, 3> _terminalColor;
    int _maxLines = 10;
    int _maxStory = 10;
};

} // namespace tio