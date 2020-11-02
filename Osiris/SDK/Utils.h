#pragma once

#include <numbers>
#include <tuple>
#include <stdint.h>

constexpr auto degreesToRadians = [](float degrees) constexpr noexcept { return degrees * std::numbers::pi_v<float> / 180.0f; };
constexpr auto radiansToDegrees = [](float radians) constexpr noexcept { return radians * 180.0f / std::numbers::pi_v<float>; };

std::tuple<float, float, float> rainbowColor(float speed) noexcept;
uint64_t FindSignature(const char* szModule, const char* szSignature) noexcept;
