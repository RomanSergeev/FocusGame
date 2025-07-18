#pragma once
#include <iostream>
#include <utility>
#include "glm/glm.hpp"
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