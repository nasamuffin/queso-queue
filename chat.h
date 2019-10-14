#pragma once

// Maybe redundant to Twitch API.
#include "level.h"
#include "quesoqueue.h"
#include "timer.h"
#include "../keys.h"

#include <string>
#include <sstream>

class Chat {
  public:
    Chat(const QuesoQueue &qq, const Timer &timer);
    void HandleMessage(std::stringstream message, std::string sender);
    void WriteMessage(std::string message);

  private:
    std::string NextLevelMessage(Level l);

    bool _canAddToQueue;
    QuesoQueue _qq;
    Timer _timer;
    std::string _helpText;
    std::string _priorityText;

    std::string _server = "irc://irc.chat.twitch.tv"
    std::string _port = 6667;
    std::string _pass = Auth::ircPass;
    std::string _nick = "NICK QuesoQueue";
};
