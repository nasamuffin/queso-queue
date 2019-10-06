#pragma once

#include <string>

class Twitch {
  public:
    bool isOnline(std::string username);
    void placeStreamMarker();

  private:
    // IDK twitch API keys??????
};
