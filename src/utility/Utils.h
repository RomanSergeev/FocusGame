#pragma once
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>
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
template<typename Container>
void moveAndAppend(Container& src, Container& dest, typename Container::iterator srcFrom, typename Container::iterator srcTo) {
    dest.insert(dest.end(), std::make_move_iterator(srcFrom), std::make_move_iterator(srcTo));
    src.erase(srcFrom, srcTo);
}
template <typename Ptr>
bool isNull(const Ptr& p, const std::string& nullMessage) {
    if (p == nullptr) {
        std::cerr << nullMessage << std::endl;
        return true;
    }
    return false;
}

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

template<typename T>
std::string toString(const T& value, int precision = 2) {
    std::ostringstream oss;
    if constexpr (std::is_floating_point_v<T>) { // constexpr - evaluating the condition at compile time
        oss << std::fixed << std::setprecision(precision) << value;
    } else {
        oss << value; // ints, bools, etc.
    }

    return oss.str();
}
std::string toLowerCase(std::string s);