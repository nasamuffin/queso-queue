#pragma once

#include <string>

class Twitch {
  public:
    bool isOnline(std::string username, std::string channel);
    void placeStreamMarker();

  private:
    // IDK twitch API keys??????
};
