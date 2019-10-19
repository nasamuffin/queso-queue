#pragma once

#include <chrono>

class Timer {
  public:
    Timer();
    void Start();
    void Pause();
    void Reset();

    bool CheckTimer();
  private:
    std::chrono::system_clock::time_point _finishTime;
    std::chrono::seconds _timeRemaining;
    bool _running;

    std::chrono::minutes _defaultTimeout = std::chrono::minutes(10);
};
