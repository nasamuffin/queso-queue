#pragma once

#include <set>
#include <string>

class Twitch {
  public:
    std::set<std::string> getOnlineUsers(const std::string& channel);
    void placeStreamMarker();

  private:
    // IDK twitch API keys??????
};
