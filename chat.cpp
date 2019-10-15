#include "chat.h"

#include "../keys.h"

#include <cstring>
#include <iostream>
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
        "([a-zA-Z0-9]+)" // username (IRC handle)
        "!" // followed by IRC host(???)
        ".* PRIVMSG #" // dont care >:C
        "[a-zA-Z0-9]+" // channel name
        " :" // IRC message body prepended by literal :
        "!(.*)" // message body - capture this!
        "\n", // all the IRC stuff ends in a newline
        std::regex_constants::egrep
    );
    for (;;) {
        std::memset (&sockbuff, '\0', sizeof(sockbuff));
        recv(_sockHandle, sockbuff, 4096, 0);
        
        std::cmatch m;
        if (std::regex_match(sockbuff, m, userMessage,
                std::regex_constants::match_not_eol))
        {
            // Get username
            std::string username = m[1];
            // Get message body
            std::string messageBody = m[2];

            HandleMessage(std::stringstream(messageBody), username);
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

void Chat::HandleMessage(std::stringstream message, std::string sender) {
    std::string command;
    // take just the first word (bot word goes in the front)
    message >> command;

    // check the command word
    if (command == "!open") {
        _canAddToQueue = true;
    } else if (command =="!close") {
        _canAddToQueue = false;
    } else if (command =="!add ABC-DEF-GHI") {
        if (_canAddToQueue) {
            std::string levelCode;
            message >> levelCode;
            Level l;
            l.levelCode = levelCode;
            l.submitter = sender;
            _qq.Add(l);
        }
    } else if (command =="!remove [ABC-DEF-GHI]") {
        // TODO - this might be optional
        std::string levelCode;
        message >> levelCode;
        _qq.Remove(sender, levelCode);
    } else if (command =="!next") {
        _timer.Reset();
        //WriteMessage(NextLevelMessage(_qq.Next()));
    } else if (command =="!current") {
        //WriteMessage(CurrentLevelMessage(_qq.Current()));
    } else if (command =="!list") {
        //WriteMessage(LevelListMessage(_qq.List()));
    } else if (command =="!position") {
        //WriteMessage(PositionMessage(_qq.Position(sender)));
    } else if (command =="!resume") {
        _timer.Start();
    } else if (command =="!pause") {
        _timer.Pause();
    } else if (command =="!restore") {
        _qq.LoadLastState();
    // if it's not a command, print the usage
    } else {    // "!help"
        //WriteMessage(_helpText);
    }
}
