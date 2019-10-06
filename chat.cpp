#include "chat.h"

Chat::Chat(const QuesoQueue &qq, const Timer &timer) : _qq(qq), _timer(timer) {
    // ...
}

void Chat::HandleMessage(std::stringstream message, std::string sender) {
    std::string command;
    // take just the first word (bot word goes in the front)
    message >> command;

    // does it look like a bot command?
    if (command[0] != '!') {
        return;
    }

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
