#pragma once
#include <iostream>
#include <utility>
#include "glm/glm.hpp"
#include "Constants.h"
#include "glew/glew.h"
#include "graphics/Shader.h"

void clampValue(float& value, float from, float to);
void clampValue(float& value, const std::pair<float, float>& pair);

struct AABB { // Axis-Aligned Bounding Box
    glm::vec3 min;
    glm::vec3 max;

    AABB() = default;
    AABB(const glm::vec3& min, const glm::vec3& max) : min(min), max(max) {}

    friend std::ostream& operator << (std::ostream& out, const AABB& box);

    glm::vec3 center() const { return (min + max) * 0.5f; }
    glm::vec3 size() const { return max - min; }

    bool contains(const glm::vec3& point) const;
    bool intersects(const AABB& other) const;
};

class ShaderCarousel {
    int currentIndex = 0;
    std::vector<Shader> shaders;
public:
    ShaderCarousel() = default;
    ShaderCarousel(std::vector<Shader> v)
        : shaders(std::move(v)) {
        if (!shaders.empty())
            shaders[0].use();
    }
    ShaderCarousel& operator = (const ShaderCarousel& sc) = default;

    void next() {
        ++currentIndex;
        if (currentIndex == shaders.size()) currentIndex = 0;
        shaders[currentIndex].use();
    }
    const Shader& get() const { return shaders.at(currentIndex); }
    const Shader& getNext() {
        next();
        return get();
    }
};

struct Color {
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

Color getDefaultColor(unsigned int index);
int getPlayerOrdinal(PlayerSlot id);