#pragma once
#include <utility>

void clampValue(float& value, float from, float to);
void clampValue(float& value, const std::pair<float, float>& pair);