#include "timer.h"

#include <iostream>

void Timer::Start() {
    _finishTime = std::chrono::system_clock::now() + _timeRemaining;
    _running = true;
}

void Timer::Pause() {
    _running = false;
}

void Timer::Reset() {
    _running = false;
    _timeRemaining = _defaultTimeout;
}

bool Timer::CheckTimer() {
    if (!_running) {
        return false;
    }

    _timeRemaining = std::chrono::duration_cast<std::chrono::seconds>(
                _finishTime - std::chrono::system_clock::now());

    if (_timeRemaining <= std::chrono::seconds(0)) {
        _running = false;
        return true;
    }
    return false;
}
