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

std::string QuesoQueue::Remove(std::string username, std::string levelCode) {
    auto toRemove = _levels.end();
    // Check if admin
    if (username == _modPlsDelete) {
        // remove the level with the matching code
        toRemove = std::find_if(_levels.begin(),
                                _levels.end(),
                                [levelCode](Level l) {
                                   return l.levelCode == levelCode;
                                });
    // otherwise, remove the level with matching username
    } else {
        toRemove = std::find_if(_levels.begin(),
                                _levels.end(),
                                [username](Level l) {
                                   return l.submitter == username;
                                });
    }
                        
    // delet
    if (toRemove != _levels.end()) {
      _levels.erase(toRemove);
        SaveState();
    }
    return std::string("PLACEHOLDER");
}

Level QuesoQueue::Next() {
    _levels.pop_front();
    auto pq = this->List();
    SaveState();

    return std::get<0>(pq).front();
}
    
Level QuesoQueue::Current() {
    return _levels.at(0);
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
        savefile >> l.levelCode;
        if (l.submitter.empty() || l.levelCode.empty()) {
            continue;
        }
        _levels.push_back(l);
    }   
}
