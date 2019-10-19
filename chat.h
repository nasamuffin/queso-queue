#pragma once

// Maybe redundant to Twitch API.
#include "level.h"
#include "quesoqueue.h"
#include "timer.h"

#include <string>
#include <sstream>

class Chat {
  public:
    Chat(const QuesoQueue &qq, const Timer &timer);
    void HandleMessage(std::stringstream message, std::string sender);
    void WriteMessage(std::string message);
    void Write(std::string command);
    void Connect();
    void Listen();

  private:
    std::string NextLevelMessage(std::optional<Level> l);
    std::string CurrentLevelMessage(std::optional<Level> l);
    std::string PositionMessage(int position);
    std::string LevelListMessage(PriorityQueso list);
    static std::string GetRemainder(std::stringstream &message);

    bool _canAddToQueue;
    QuesoQueue _qq;
    Timer _timer;
    std::string _priorityText;

    std::string _server = "irc.chat.twitch.tv";
    unsigned short _port = 6667;
    std::string _nick = "quesoqueue";

    int _sockHandle;

    std::string _help =
        "Try these: !add ABC-DEF-GHI | !remove | !current | !list | !position";
};
