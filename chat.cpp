#include "chat.h"

void Chat::HandleMessage(std::stringstream message, std::string sender) {
    std::string command;
    command << message;

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
            std::string levelCode << message;
            Level l = {.levelCode = levelCode; .submitter = sender};
            _qq.Add(l);
        }
    } else if (command =="!remove [ABC-DEF-GHI]") {
        // TODO - this might be optional
        std::string levelCode << message;
        _qq.Remove(sender, levelCode);
    } else if (command =="!next") {
        _timer.Reset();
        WriteMessage(NextLevelMessage(_qq.Next()));
    } else if (command =="!current") {
        WriteMessage(CurrentLevelMessage(_qq.Current()));
    } else if (command =="!list") {
        WriteMessage(LevelListMessage(_qq.List()));
    } else if (command =="!position") {
        WriteMessage(PositionMessage(_qq.Position(sender)));
    } else if (command =="!resume") {
        _timer.Resume();
    } else if (command =="!pause") {
        _timer.Pause();
    } else if (command =="!restore") {
        _qq.LoadLastState();
    // if it's not a command, print the usage
    } else {    // "!help"
        WriteMessage(_helpText);
    }
}
