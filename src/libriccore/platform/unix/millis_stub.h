#pragma once
#include <chrono>

inline thread_local auto threadStartTime = std::chrono::steady_clock::now();
inline thread_local double clockDriftMultiplier = 1.0;

inline void setClockDriftPPM(int ppm) {
    clockDriftMultiplier = 1.0 + static_cast<double>(ppm) / 1e6;
	threadStartTime = std::chrono::steady_clock::now();
}

inline uint32_t millis() {
    auto elapsed = std::chrono::steady_clock::now() - threadStartTime;
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
    return static_cast<uint32_t>(static_cast<double>(ms) * clockDriftMultiplier);
}