#include "quesoqueue.h"

#include <algorithm>
#include <iostream>
#include <tuple>

QuesoQueue::QuesoQueue(const Twitch &twitch) : _twitch(twitch) {
    // ...
}

void QuesoQueue::Add(Level level) {
    // Check if the viewer already submitted something
    auto result = std::find_if(std::begin(_levels),
                               std::end(_levels),
                               [level] (Level l) {
                                   return l.submitter==level.submitter;
                               });
    if (result == _levels.end()) {
        // TODO: exception? error? explain why it broekn?
        std::cout << "Couldn't find something with the matching submitter" << std::endl;
        // push to the end of the queue
        _levels.push_back(level);
    }
    else {
        std::cout << level.submitter << " already has a level!" << std::endl;
    }
    std::cout << "now the queue has " << _levels.size() << " entries." 
        << std::endl;

    // Report the placement in queue?
    // TODO: return List()
}

void QuesoQueue::Remove(std::string username, std::string levelCode) {
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
    }
}

Level QuesoQueue::Next() {
    _levels.pop_front();
    auto pq = this->List();
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
    //...
}

void QuesoQueue::LoadLastState() {
    //...
}
