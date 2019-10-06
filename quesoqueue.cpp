#include "quesoqueue.h"

#include <algorithm>
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
        return;
    }

    // push to the end of the queue
    _levels.push_back(level);

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

bool QuesoQueue::isOnline(Level l) {
    return _twitch.isOnline(l.submitter);
}

std::tuple<std::deque<Level>, std::deque<Level>> QuesoQueue::List() {
    std::deque<Level> online, offline;
    std::partition_copy(_levels.begin(), _levels.end(), online.begin(),
                        offline.begin(),
                        [this](Level l){
                            return this->_twitch.isOnline(l.submitter);
                        });
    return std::make_tuple(online, offline);
}

void QuesoQueue::SaveState() {
    //...
}

void QuesoQueue::LoadLastState() {
    //...
}
