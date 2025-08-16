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

int getPlayerOrdinal(PlayerSlot id) {
    switch (id) {
        case PlayerSlot::Player1  : return 0;
        case PlayerSlot::Player2  : return 1;
        case PlayerSlot::Player3  : return 2;
        case PlayerSlot::Player4  : return 3;
        case PlayerSlot::Player5  : return 4;
        case PlayerSlot::Player6  : return 5;
        case PlayerSlot::Player7  : return 6;
        case PlayerSlot::Player8  : return 7;
        case PlayerSlot::Spectator: return -1;
    }
}