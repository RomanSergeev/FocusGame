#include <algorithm>
#include "Utils.h"
#include "Constants.h"

void clampValue(float& value, float from, float to) { value = std::clamp(value, from, to); }
void clampValue(float& value, const std::pair<float, float>& pair) { value = std::clamp(value, pair.first, pair.second); }
std::string toLowerCase(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return s;
}

std::ostream& operator << (std::ostream& out, const AABB& box) {
    out <<  box.min.x << ' ' << box.min.y << ' ' << box.min.z << '\n' <<
            box.max.x << ' ' << box.max.y << ' ' << box.max.z << "\n\n";
    return out;
}

bool AABB::contains(const glm::vec3& point) const {
    return glm::all(glm::lessThanEqual(min, point)) &&
           glm::all(glm::lessThanEqual(point, max));
}

bool AABB::intersects(const AABB& other) const {
    return (min.x <= other.max.x && max.x >= other.min.x) &&
           (min.y <= other.max.y && max.y >= other.min.y) &&
           (min.z <= other.max.z && max.z >= other.min.z);
}

int getDeltaX(JumpDirection jd) {
    switch (jd) {
        case JumpDirection::Up   : return -1;
        case JumpDirection::Down : return  1;
        case JumpDirection::Left : return  0;
        case JumpDirection::Right: return  0;
    }
    return 0;
}

int getDeltaY(JumpDirection jd) {
    switch (jd) {
        case JumpDirection::Up   : return  0;
        case JumpDirection::Down : return  0;
        case JumpDirection::Left : return -1;
        case JumpDirection::Right: return  1;
    }
    return 0;
}

bool isHorizontal(JumpDirection jd) {
    return jd == JumpDirection::Left || jd == JumpDirection::Right;
}

bool isVertical(JumpDirection jd) {
    return jd == JumpDirection::Down || jd == JumpDirection::Up;
}

JumpDirection reverseDirection(JumpDirection jd) {
    switch (jd) {
        case JumpDirection::Up   : return JumpDirection::Down ;
        case JumpDirection::Down : return JumpDirection::Up   ;
        case JumpDirection::Left : return JumpDirection::Right;
        case JumpDirection::Right: return JumpDirection::Left ;
    }
}

Color getDefaultColor(PlayerSlot slot) {
    if (slot == PlayerSlot::Spectator) return Color::BLACK;
    return defaultColors.at(slot);
}

Color getNamedColor(const std::string& str) {
    auto iter = Color::namedColors.find(toLowerCase(str));
    if (iter == Color::namedColors.end()) return Color::BLACK;
    return iter->second;
}