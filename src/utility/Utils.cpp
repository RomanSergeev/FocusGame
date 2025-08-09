#include <algorithm>
#include <stdexcept>
#include "Utils.h"
#include "Constants.h"

void clampValue(float& value, float from, float to) { value = std::clamp(value, from, to); }
void clampValue(float& value, const std::pair<float, float>& pair) { value = std::clamp(value, pair.first, pair.second); }

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

Color getDefaultColor(unsigned int index) {
    if (index < 0 || index >= MAX_PLAYERS) throw std::invalid_argument("getDefaultColor: argument index is out of range.");
    return Color(DEFAULT_COLORS[index*3], DEFAULT_COLORS[index*3 + 1], DEFAULT_COLORS[index*3 + 2]);
}

int getPlayerOrdinal(PlayerPlaceholderID id) {
    switch (id) {
        case PlayerPlaceholderID::Player1  : return 0;
        case PlayerPlaceholderID::Player2  : return 1;
        case PlayerPlaceholderID::Player3  : return 2;
        case PlayerPlaceholderID::Player4  : return 3;
        case PlayerPlaceholderID::Player5  : return 4;
        case PlayerPlaceholderID::Player6  : return 5;
        case PlayerPlaceholderID::Player7  : return 6;
        case PlayerPlaceholderID::Player8  : return 7;
        case PlayerPlaceholderID::Spectator: return -1;
    }
}