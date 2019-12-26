#pragma once

#include <chrono>
#include <string>

// Add `_path` as a text source in obs to get an onscreen queso-queue timer.
class ObsTextSource {
  public:
    ObsTextSource(std::string path);
    void UpdateTime(std::chrono::seconds seconds);

  private:
    void WriteMessage(std::string message);
    std::string _path;
};
