#include "quesoqueue.h"

#include "../keys.h"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <regex>
#include <sstream>
#include <tuple>

QuesoQueue::QuesoQueue(const Twitch &twitch) : _twitch(twitch) {
    // ...
}

bool QuesoQueue::isValidLevelCode(std::string levelCode) {
    std::string levelBit("["
        "A-Ha-h" // exclude I
        "J-Nj-n" // exclude O
        "P-Yp-y" // exclude Z
        "0-9"    // numbers
        "]");
    std::string delimBit("[-. ]");
    std::regex validLevelCode(
        " ?" + // GetRemainder sometimes gives us extra whitespace??
        levelBit + "{3}" + // the first chonk
        delimBit + "?" + // it's ok not to use a delimiter
        levelBit + "{3}" + // the second chonk
        delimBit + "?" + // still ok not to use a delimiter
        levelBit + "{3}", // the last chonk
        std::regex_constants::egrep
    );

    return std::regex_match(levelCode, validLevelCode);
}

std::string QuesoQueue::Add(Level level) {
    if (_levels.size() >= maxSize) {
        return std::string("Sorry, the level queue is full!");
    }

    if (!isValidLevelCode(level.levelCode)) {
        return std::string("I'm pretty sure '" + level.levelCode + "' isn't " +
                "valid. Try again!");
    }

    // Does the viewer already have a level in queue?
    auto result = std::find_if(std::begin(_levels),
                               std::end(_levels),
                               [level] (Level l) {
                                   return l.submitter==level.submitter;
                               });
    // Or, if the submitter is the channel name, then THEY OWN US.
    if (result == _levels.end() || level.submitter == Auth::channel) {
        // push to the end of the queue
        _levels.push_back(level);
        std::stringstream ss;
        // Since they JUST added it, we can pretty safely assume they're online.
        ss << level.submitter;
        ss << ", ";
        ss << level.levelCode;
        ss << " has been added to the queue. Currently in position #";
        ss << std::get<0>(List()).size();
        ss << ".";
        SaveState();
        return ss.str();
    }
    else {
        return std::string("Sorry, viewers are limited to one submission at a time.");
    }
}

std::string QuesoQueue::ModRemove(std::string username) {
    if (username.empty()) {
        return std::string("You can use !remove <username> to kick out someone "
                "else's level; if you want to skip the current one use !next.");
    }

    // remove the level with the matching code
    auto toRemove = std::find_if(_levels.begin(),
                            _levels.end(),
                            [username](Level l) {
                               return l.submitter == username;
                            });
    // delet
    if (toRemove != _levels.end()) {
        _levels.erase(toRemove);
        SaveState();
        return std::string("Ok, I removed " + username + "'s level from the queue.");
    } else {
        return std::string("The level by " + username + " isn't in the queue " +
            "now. It wasn't before, but it isn't now, too.");
    }
}

std::string QuesoQueue::Remove(std::string username) {
    auto toRemove = std::find_if(_levels.begin(),
                            _levels.end(),
                            [username](Level l) {
                               return l.submitter == username;
                            });
                        
    // delet
    if (toRemove != _levels.end()) {
      _levels.erase(toRemove);
        SaveState();
        return std::string("Ok " + username + ", your level was removed from the queue.");
    } else {
        return std::string("Ok " + username + ", your isn't in the queue " +
            "now. It wasn't before, but it isn't now, too.");
    }
}

std::string QuesoQueue::Replace(std::string username, std::string newLevelCode) {
    if (!isValidLevelCode(newLevelCode)) {
        return std::string("I'm pretty sure '" + newLevelCode + "' isn't " +
                "valid. Try again!");
    }

    auto toReplace = std::find_if(_levels.begin(),
                            _levels.end(),
                            [username](Level l) {
                               return l.submitter == username;
                            });

    if (toReplace != _levels.end()) {
        toReplace->levelCode = newLevelCode;
        SaveState();
        return std::string("Ok " + username + ", you are now in queue with level "
                           + newLevelCode + ".");
    } else {
        return std::string("I didn't find a level for " + username
                           + ". Try !add.");
    }
}

int QuesoQueue::Position(std::string username) {
    if (_levels.empty() || std::get<0>(List()).empty() ) {
        return -1;
    }

    int position = 0;
    auto online = std::get<0>(List());
    for (Level l : online) {
        position++;
        if (l.submitter == username) {
            return position;
        }
    }
    return -1;
}

std::optional<Level> QuesoQueue::Next() {
    if (_levels.empty()) {
        return std::nullopt;
    }

    _levels.pop_front();
    SaveState();

    if (_levels.empty()) {
        return std::nullopt;
    }

    return std::make_optional(std::get<0>(List()).front());
}
    
std::optional<Level> QuesoQueue::Current() {
    if (_levels.empty()) {
        return std::nullopt;
    }

    return std::make_optional(_levels.at(0));
}

PriorityQueso QuesoQueue::List() {
    std::deque<Level> online, offline;
    bool isFirst = true;
    for (Level l : _levels) {
        // The first person in queue is special.
        if (isFirst) {
            online.push_back(l);
            isFirst = false;
        } else if (_twitch.isOnline(l.submitter, Auth::channel)) {
            online.push_back(l);
        } else {
            offline.push_back(l);
        }
    }
    return std::make_tuple(online, offline);
}

void QuesoQueue::SaveState() {
    std::ofstream savefile("queso.save", std::ios_base::out | std::ios_base::trunc);
    for (Level l : _levels) {
        savefile << l.submitter << " " << l.levelCode << std::endl;
    }
}

void QuesoQueue::LoadLastState() {
    std::ifstream savefile("queso.save");
    _levels.clear();
    while(savefile) {
        Level l;
        savefile >> l.submitter;
        std::getline(savefile, l.levelCode);
        if (l.submitter.empty() || l.levelCode.empty()) {
            continue;
        }
        _levels.push_back(l);
    }   
}
