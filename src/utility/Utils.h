#pragma once
#include <algorithm>
#include <iomanip>
#include <iterator>
#include <list>
#include <utility>
#include "Logger.h"
#include "graphics/Shader.h"

void clampValue(float& value, float from, float to);
void clampValue(float& value, const std::pair<float, float>& pair);
std::string toLowerCase(std::string s);

template<typename T>
bool vectorContains(const std::vector<T>& v, const T& value) {
    return std::find(v.begin(), v.end(), value) != v.end();
}
template<typename Container>
void moveAndAppend(Container& src, Container& dest, typename Container::iterator srcFrom, typename Container::iterator srcTo) {
    dest.insert(dest.end(), std::make_move_iterator(srcFrom), std::make_move_iterator(srcTo));
    src.erase(srcFrom, srcTo);
}
template<typename T>
void moveAndAppend(std::list<T>& src, std::list<T>& dest, typename std::list<T>::iterator srcFrom, typename std::list<T>::iterator srcTo) {
    dest.splice(dest.end(), src, srcFrom, srcTo);
}
template<typename T>
void moveAndAppend(std::list<T>& src, std::list<T>& dest, typename std::list<T>::iterator srcIter) {
    moveAndAppend<T>(src, dest, srcIter, std::next(srcIter));
}

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

template <typename Ptr>
bool isNull(const Ptr& p, const std::string& nullMessage) {
    if (p == nullptr) {
        Logger::log(nullMessage);
        return true;
    }
    return false;
}

// allow up to N subfolders (e.g., N = 1 means at most "dir/file.txt")
bool isValidRelativePath(const std::string& pathStr, size_t maxDepth = 1);