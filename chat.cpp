#include "chat.h"
#include "../keys.h"

#include <cstring>
#include <iostream>
#include <poll.h>
#include <regex>
#include <sstream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

Chat::Chat(const QuesoQueue &qq, const Timer &timer) : _qq(qq), _timer(timer) {
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
            WriteMessage("The timer has expired for this level! Yikes! Let's "
                         "roll for retries.");
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

std::string Chat::LevelListMessage(PriorityQueso list) {
    std::stringstream ss;
    auto online = std::get<0>(list);
    auto offline = std::get<1>(list);

    for(Level l : online)
        ss << l.submitter << " (online), ";
    for(Level l : offline)
        ss << l.submitter << " (offline), ";

    // TODO clean up trailing ", "
    if (ss.str().empty()) {
        ss << "There are no levels in the queue :C";
    }
    return ss.str();
}

std::string Chat::NextLevelMessage(Level l) {
    std::stringstream ss;
    ss << "Next up in queue is " << l.levelCode << ", submitted by "
       << l.submitter;
    return ss.str();
}

std::string Chat::CurrentLevelMessage(Level l) {
    std::stringstream ss;
    ss << "Currently playing " << l.levelCode << ", submitted by "
       << l.submitter;
    return ss.str();
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
            std::string levelCode = message.str();
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
    } else if (command == "next" && sender == Auth::channel) {
        _timer.Reset();
        _timer.Start();
        Level l = _qq.Next();
        WriteMessage(std::string("/marker " + l.levelCode + ", submitted by "
                                 + l.submitter));
        WriteMessage(NextLevelMessage(l));
    } else if (command == "current") {
        WriteMessage(CurrentLevelMessage(_qq.Current()));
    } else if (command == "list") {
        WriteMessage(LevelListMessage(_qq.List()));
    } else if (command == "position") {
        //WriteMessage(PositionMessage(_qq.Position(sender)));
    } else if ((command == "resume" || command == "start") && sender == Auth::channel) {
        _timer.Start();
        WriteMessage("Timer resumed! Get going!");
    } else if (command == "pause" && sender == Auth::channel) {
        _timer.Pause();
        WriteMessage("Timer paused... drink some water.");
    } else if (command == "restart" && sender == Auth::channel) {
        _timer.Reset();
        _timer.Start();
        WriteMessage("Starting the clock over! Don't mess up this time.");
    } else if (command == "restore" && sender == Auth::channel) {
        _qq.LoadLastState();
    }
}
