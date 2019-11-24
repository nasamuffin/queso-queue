#pragma once

#include <chrono>
#include <ctime>
#include <map>
#include <set>
#include <string>

class Twitch {
  public:
    bool isOnline(std::string username, std::string channel);
    std::set<std::string> getOnlineUsers(const std::string& channel);
    void placeStreamMarker();

    void markAsOnline(std::string username);

  private:
    std::map<std::string, std::chrono::system_clock::time_point> _recent_chatters;
    // IDK twitch API keys??????
};
