#pragma once
#include <string>
#include <unordered_map>
#include "glm/glm.hpp"

struct Color {
    static const std::unordered_map<std::string, Color> namedColors;
    static const Color BLACK;

    uint8_t r, g, b;

    Color() = default;
    Color(uint8_t r_, uint8_t g_, uint8_t b_) : r(r_), g(g_), b(b_) {}

    glm::vec3 toVec3() const {
        return glm::vec3(r / 255.0f, g / 255.0f, b / 255.0f);
    }

    bool operator == (const Color& other) const {
        return r == other.r && g == other.g && b == other.b;
    }
};