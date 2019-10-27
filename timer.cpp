#include "timer.h"

#include <iostream>

Timer::Timer() {
    _timeRemaining = _defaultTimeout;
}

void Timer::Start() {
    _finishTime = std::chrono::system_clock::now() + _timeRemaining;
    _running = true;
}

void Timer::Pause() {
    _running = false;
}

void Timer::Reset() {
    _timeRemaining = _defaultTimeout;
    _running = false;
}

std::chrono::seconds Timer::Remaining() {
    return _timeRemaining;
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
