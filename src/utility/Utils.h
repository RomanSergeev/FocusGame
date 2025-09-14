#pragma once
#include <algorithm>
#include <iostream>
#include <utility>
#include "Color.h"
#include "Constants.h"
#include "glew/glew.h"
#include "graphics/Shader.h"

void clampValue(float& value, float from, float to);
void clampValue(float& value, const std::pair<float, float>& pair);
template<typename T>
bool vectorContains(const std::vector<T>& v, const T& value) {
    return std::find(v.begin(), v.end(), value) != v.end();
}
/*template <typename K, typename V>
void fuseMap(const std::unordered_map<K, V>& source, std::unordered_map<K, V>& target) {
    for (const auto& [k, v] : source) {
        // insert only if key is not present in target
        target.emplace(k, v);  
    }
}*/

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

enum class JumpDirection {
    Up, Down, Left, Right
};
int getDeltaX(JumpDirection jd);
int getDeltaY(JumpDirection jd);
bool isHorizontal(JumpDirection jd);
bool isVertical(JumpDirection jd);
JumpDirection reverseDirection(JumpDirection jd);

struct Coord { // board coordinates wrapper
    idxtype x, y;
    Coord() = default;
    Coord(idxtype x, idxtype y) : x(x), y(y) {}
    Coord(const Coord& c) = default;
    Coord& operator = (const Coord& c) = default;
    bool operator == (const Coord& c) const { return x == c.x && y == c.y; }
    bool operator != (const Coord& c) const { return x != c.x || y != c.y; }
};

Color getDefaultColor(PlayerSlot slot);
Color getNamedColor(const std::string& str);
std::string toLowerCase(std::string s);
//int getPlayerOrdinal(PlayerSlot id);