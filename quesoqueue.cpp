#include "quesoqueue.h"

#include "../keys.h"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <tuple>

QuesoQueue::QuesoQueue(const Twitch &twitch) : _twitch(twitch) {
    // ...
}

std::string QuesoQueue::Add(Level level) {
    if (_levels.size() >= maxSize) {
        return std::string("Sorry, the level queue is full!");
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
        ss << "Your level has been added to the queue behind ";
        ss << std::get<0>(List()).size() - 1;
        ss << " viewers who are online right now (there are ";
        ss << _levels.size() << " total levels in queue, including yours).";
        SaveState();
        return ss.str();
    }
    else {
        return std::string("Sorry, viewers are limited to one submission at a time.");
    }
}

std::string QuesoQueue::ModRemove(std::string username) {
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
    }

    return std::string("Ok, I removed " + username + "'s level from the queue.");
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
    }
    return std::string("Ok " + username + ", your level was removed from the queue.");
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
