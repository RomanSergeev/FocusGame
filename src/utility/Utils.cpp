#include "Utils.h"
#include "Color.h"

void clampValue(float& value, float from, float to) { value = std::clamp(value, from, to); }
void clampValue(float& value, const std::pair<float, float>& pair) { value = std::clamp(value, pair.first, pair.second); }
std::string toLowerCase(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return s;
}

Color getNamedColor(const std::string& str) {
    auto iter = Color::namedColors.find(toLowerCase(str));
    if (iter == Color::namedColors.end()) return Color::BLACK;
    return iter->second;
}