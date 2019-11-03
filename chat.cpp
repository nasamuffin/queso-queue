#include "chat.h"
#include "../keys.h"

#include <spdlog/spdlog.h>

#include <cstring>
#include <iostream>
#include <numeric>
#include <poll.h>
#include <regex>
#include <sstream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <experimental/filesystem>

Chat::Chat(const QuesoQueue &qq, const Timer &timer) : _qq(qq), _timer(timer) {
    namespace fs = std::experimental::filesystem;
    static const std::regex chipModuleRegex(".*\\.so", std::regex_constants::egrep);

    for(auto& p: fs::directory_iterator("chips")) {
        std::cmatch m;
        bool matched = std::regex_match(p.path().c_str(), m, chipModuleRegex, std::regex_constants::match_not_eol);
        if (matched) {
            auto cc = LoadChip(p.path().string());
            _commandMap[cc.chip->command] = std::move(cc);
        }
    }
}

void Chat::Connect() {
    hostent *host = gethostbyname(_server.c_str());
    sockaddr_in addr;
    std::memcpy(&addr.sin_addr, host->h_addr, host->h_length);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port);
    _sockHandle = socket(AF_INET, SOCK_STREAM, 0);

    connect (_sockHandle, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));

    // Auth
    Write(std::string("PASS ") + Auth::ircPass + '\n');
    Write(std::string("NICK ") + _nick + '\n');

    // Join the channel
    Write(std::string("JOIN #") + Auth::channel + '\n');

    // Block til I'm authenticated.
}

std::string Chat::GetRemainder(std::stringstream &ss) {
    std::string remainder, tmp;
    ss >> remainder;
    while (ss >> tmp) {
        remainder += " ";
        remainder += tmp;
    }
    return remainder;
}

void Chat::Listen() {
    char sockbuff[4096];

    // :derflergen!derflergen@derflergen.tmi.twitch.tv PRIVMSG #nasamuffin :!Bot do thing
    std::regex userMessage(
        "^" // beginning of string
        ":" // all the IRC stuff gets prepended with a literal :
        "([a-zA-Z0-9_]+)" // username (IRC handle)
        "!" // followed by IRC host(???)
        ".* PRIVMSG #" // dont care >:C
        "[a-zA-Z0-9_]+" // channel name
        " :" // IRC message body prepended by literal :
        "!(.*)" // message body - capture this!
        "\n", // all the IRC stuff ends in a newline
        std::regex_constants::egrep
    );
    for (;;) {

        struct pollfd fd;
        fd.fd = _sockHandle;
        fd.events = POLLIN;
        int ret = poll(&fd, 1, 1000);

        if (_timer.CheckTimer()) {
            WriteMessage("@" + std::string(Auth::channel) + " the timer has "
                         "expired for this level! Let's roll for retries.");
            WriteMessage("!roll d10");
        }

        if (ret == 0) {
            continue;
        }

        std::memset (&sockbuff, '\0', sizeof(sockbuff));
        recv(_sockHandle, sockbuff, 4096, 0);

        // Respond to ping (otherwise Twitch kills the connection)
        if (std::strncmp(sockbuff, "PING", 4) == 0) {
            Write("PONG :tmi.twitch.tv\n");
            continue;
        }

        std::cmatch m;
        if (std::regex_match(sockbuff, m, userMessage,
                std::regex_constants::match_not_eol))
        {
            // Get username
            std::string username = m[1];
            // Get message body
            std::string messageBody = m[2];

            HandleMessage(std::stringstream(messageBody), username);
            continue;
        }
    }
}

void Chat::Write(std::string command) {
    send(_sockHandle, command.c_str(), command.size(), 0);
}

void Chat::WriteMessage(std::string message) {
    std::stringstream ss;
    Write(std::string("PRIVMSG #") + Auth::channel + std::string(" :") + message + '\n');
}

std::string Chat::LevelListMessage(std::optional<Level> current, PriorityQueso list) {
    std::stringstream ss;
    auto online = std::get<0>(list);
    auto offline = std::get<1>(list);
    if (!current && online.empty() && offline.empty()) {
        return "There are no levels in the queue :C";
    }

    ss << online.size() + (current ? 1 : 0)
       << " online level(s) in the queue: ";

    ss << std::accumulate(online.begin(), online.end(),
            std::string(current->submitter + " (current)"),
            [](std::string acc, Level x){
                return acc + ", " + x.submitter;
            });
    ss << ". There are also " << offline.size() << " offline level(s) in the queue.";

    return ss.str();
}

std::string Chat::NextLevelMessage(std::optional<Level> l) {
    if (!l) {
        return std::string("The queue is empty. Feed me levels!");
    }

    std::stringstream ss;
    ss << "Next up in queue is " << l->levelCode << ", submitted by "
       << l->submitter;
    return ss.str();
}

std::string Chat::CurrentLevelMessage(std::optional<Level> l) {
    if (!l) {
        return std::string("We're not playing a level right now! D:");
    }

    std::stringstream ss;
    ss << "Currently playing " << l->levelCode << ", submitted by "
       << l->submitter;
    return ss.str();
}

std::string Chat::PositionMessage(int position) {
    std::stringstream msg;
    switch (position) {
    case -1:
        msg << "Looks like you're not in the queue. Try !add AAA-AAA-AAA.";
        break;
    case 0:
        msg << "Your level is being played right now!";
        break;
    default:
        msg << "You are currently in position " << position+1;
        break;
    }
    return msg.str();
}

void Chat::HandleMessage(std::stringstream message, std::string sender) {
    std::string command;
    // take just the first word (bot word goes in the front)
    message >> command;

    // check the command word
    if (command == "open" && sender == Auth::channel) {
        _canAddToQueue = true;
        WriteMessage("The queue is now open!");
    } else if (command == "close" && sender == Auth::channel) {
        _canAddToQueue = false;
        WriteMessage("The queue is now closed!");
    } else if (command == "add") {
        if (_canAddToQueue || sender == Auth::channel) {
            std::string levelCode = Chat::GetRemainder(message);
            Level l;
            l.levelCode = levelCode;
            l.submitter = sender;
            WriteMessage(_qq.Add(l));
        } else {
            WriteMessage("Sorry, the queue is closed right now :C");
        }
    } else if (command == "remove") {
        // TODO - this might be optional
        if (sender == Auth::channel) {
            std::string toRemove;
            message >> toRemove;
            WriteMessage(_qq.ModRemove(toRemove));
        } else {
            WriteMessage(_qq.Remove(sender));
        }
    } else if (command == "replace") {
        WriteMessage(_qq.Replace(sender, GetRemainder(message)));
    } else if (command == "next" && sender == Auth::channel) {
        _timer.Reset();
        std::optional<Level> l = _qq.Next();
        if (l) {
            WriteMessage(std::string("/marker " + l->levelCode + ", submitted by "
                                     + l->submitter));
        }
        WriteMessage(NextLevelMessage(l));
    } else if (command == "punt" && sender == Auth::channel) {
        WriteMessage("Ok, I'll save that one for later...");
        _timer.Reset();
        std::optional<Level> l = _qq.Punt();
        if (l) {
            WriteMessage(std::string("/marker " + l->levelCode + ", submitted by "
                                     + l->submitter));
        }
        WriteMessage(NextLevelMessage(l));
    } else if (command == "current") {
        WriteMessage(CurrentLevelMessage(_qq.Current()));
    } else if (command == "list") {
        WriteMessage(LevelListMessage(_qq.Current(), _qq.List()));
    } else if (command == "position") {
        WriteMessage(PositionMessage(_qq.Position(sender)));
    } else if ((command == "resume" || command == "start") && sender == Auth::channel) {
        _timer.Start();
        WriteMessage("Timer started! Get going!");
    } else if (command == "pause" && sender == Auth::channel) {
        _timer.Pause();
        WriteMessage("Timer paused... drink some water.");
    } else if (command == "restart" && sender == Auth::channel) {
        _timer.Reset();
        _timer.Start();
        WriteMessage("Starting the clock over! CP Hype!");
    } else if (command == "restore" && sender == Auth::channel) {
        _qq.LoadLastState();
    } else {
        spdlog::info("Checking for matching chip command: {}", command);
        auto chip = _commandMap.find(command);
        if (chip != _commandMap.end()) {
            const Chip *chipModule = chip->second.chip;
            std::string moduleMsg = chipModule->chip(GetRemainder(message));
            spdlog::info("ChipModule {} returned: {}", chipModule->name, moduleMsg);
            WriteMessage(moduleMsg);
        }
    }
}
