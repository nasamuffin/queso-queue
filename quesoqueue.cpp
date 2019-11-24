#include "quesoqueue.h"

#include "../keys.h"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <random>
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

    if (Current()->submitter == level.submitter && level.submitter != Auth::channel) {
        return std::string("Wait til your level has been completed before you submit again.");
    }

    // Does the viewer already have a level in queue?
    auto result = std::find_if(std::begin(_levels),
                               std::end(_levels),
                               [level] (Level l) {
                                   return l.submitter==level.submitter;
                               });

    // Or, if the submitter is the channel name, then THEY OWN US.
    if (result == _levels.end() || level.submitter == Auth::channel) {
        auto online_levels = std::get<0>(List()).size();
        // push to the end of the queue
        _levels.push_back(level);
        online_levels++;
        std::stringstream ss;
        // Since they JUST added it, we can pretty safely assume they're online.
        ss << level.submitter;
        ss << ", ";
        ss << level.levelCode;
        ss << " has been added to the queue. Currently in position #";
        ss << online_levels + (_current ? 1 : 0);
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
    if (Current()->submitter == username) {
        return std::string("We're playing that now! Don't take this away from us!");
    } else if (toRemove != _levels.end()) {
      _levels.erase(toRemove);
        SaveState();
        return std::string("Ok " + username + ", your level was removed from the queue.");
    } else {
        return std::string("Ok " + username + ", your level isn't in the queue " +
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
    } else if (Current()->submitter == username) {
        _current->levelCode = newLevelCode;
        SaveState();
        return std::string("Ok " + username + ", you are now in queue with level "
                           + newLevelCode + ".");
    } else {
        return std::string("I didn't find a level for " + username
                           + ". Try !add.");
    }
}

int QuesoQueue::Position(std::string username) {
    if (Current()->submitter == username) {
        return 0;
    }
    if (_levels.empty() || std::get<0>(List()).empty() ) {
        return -1;
    }

    int position = 0;
    auto list = List();
    auto both = std::get<0>(list);
    for (Level l : std::get<1>(list)) {
        both.push_back(l);
    }

    for (Level l : both) {
        position++;
        if (l.submitter == username) {
            return position + (_current ? 1 : 0);
        }
    }
    // not in queue
    return -1;
}

std::optional<Level> QuesoQueue::Punt() {
    if (_levels.empty()) {
        return std::nullopt;
    }

    auto top = Current();
    if (!top) {
        return std::nullopt;
    }
    auto next = Next();
    std::cout << Add(top.value()) << std::endl;
    return next;
}

std::optional<Level> QuesoQueue::Next() {
    auto list = List();

    // Concatenate both lists
    std::deque<Level> both(std::get<0>(list));
    for (Level l : std::get<1>(list)) {
        both.push_back(l);
    }

    if (both.empty()) {
        _current = std::nullopt;
    } else {
        _current = std::make_optional(both.front());
    }

    // Remove current (it's in a special current place now)
    for (auto pos = _levels.begin(); pos != _levels.end(); pos++) {
        if (pos->submitter == _current->submitter &&
            pos->levelCode == _current ->levelCode) {
            _levels.erase(pos);
            break;
        }
    }
    SaveState();
    return _current;
}

std::optional<Level> QuesoQueue::Current() {
    //// This is the case when we haven't started yet?
    //if (!_current && !_levels.empty()) {
    //    _current = Next();
    //}
    return _current;
}

PriorityQueso QuesoQueue::List() {
    std::deque<Level> online, offline;
    std::set<std::string> online_users = _twitch.getOnlineUsers(Auth::channel);
    for (Level l : _levels) {
        if (online_users.find(l.submitter) != online_users.end()) {
            online.push_back(l);
        } else {
            offline.push_back(l);
        }
    }
    return std::make_tuple(online, offline);
}

void QuesoQueue::SaveState() {
    std::ofstream savefile("queso.save", std::ios_base::out | std::ios_base::trunc);
    if (_current && !(_current->submitter.empty() || _current->levelCode.empty())) {
        savefile << _current->submitter << " " << _current->levelCode << std::endl;
    }
    for (Level l : _levels) {
        savefile << l.submitter << " " << l.levelCode << std::endl;
    }
}

void QuesoQueue::LoadLastState() {
    std::ifstream savefile("queso.save");
    _levels.clear();
    if (savefile) {
        std::string submitter;
        if (savefile >> submitter) {
            _current = std::make_optional(Level());
            _current->submitter = submitter;
            std::getline(savefile >> std::ws, _current->levelCode);
        }
    }
    while(savefile) {
        Level l;
        savefile >> l.submitter;
        std::getline(savefile >> std::ws, l.levelCode);
        if (l.submitter.empty() || l.levelCode.empty()) {
            continue;
        }
        _levels.push_back(l);
    }
}
