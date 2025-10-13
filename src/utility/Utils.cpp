#include <filesystem>
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

// Check for illegal characters (on Windows)
bool hasInvalidCharacters(const std::string& path) {
    static const std::string invalidChars = "<>:\"|?*";
    for (char c : path)
        if (invalidChars.find(c) != std::string::npos)
            return true;
    return false;
}

bool isValidRelativePath(const std::string& pathStr, size_t maxDepth) {
namespace fs = std::filesystem;
    try {
        fs::path path(pathStr);
        if (path.is_absolute()) return false; // relative paths only
        if (path.empty()) return false;
        if (hasInvalidCharacters(pathStr)) return false;

        // Count components before the filename
        size_t depth = 0;
        for (auto it = path.begin(); it != path.end(); ++it) {
            if (std::next(it) != path.end()) // not the last element (filename)
                ++depth;
        }

        if (depth > maxDepth) return false;

        return true;
    }
    catch (const fs::filesystem_error&) {
        return false;
    }
}