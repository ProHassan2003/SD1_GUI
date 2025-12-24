#include "Timer.h"

void Timer::start() {
    startTime = std::chrono::high_resolution_clock::now();
}

void Timer::stop() {
    endTime = std::chrono::high_resolution_clock::now();
}

double Timer::getMilliseconds() const {
    return std::chrono::duration<double, std::milli>(endTime - startTime).count();
}
