#pragma once

#include "level.h"
#include "twitch.h"

#include <deque>
#include <string>
#include <vector>

typedef std::tuple<std::deque<Level>,std::deque<Level>> PriorityQueso;

class QuesoQueue {
  public:
    QuesoQueue(const Twitch &twitch);

    /**
     * Adds a level to queue; limit one per viewer.
     */
    std::string Add(Level level);

    /**
     * Removes the level submitted by the user. Or, if the user is admin,
     * remove the level with the specified code.
     */
    std::string Remove(std::string username, std::string levelCode = "");

    /**
     * Pop the top of the queue and return the new top (subject to priority queue split)
     */
    Level Next();

    /**
     * Report the level currently being played (not subject to priority queue split)
     */
    Level Current();

    /**
     * Split the stored level queue into online and offline for printing
     */
    PriorityQueso List();

    void SaveState();
    void LoadLastState();

  private:
    bool isOnline(Level l);
    std::deque<Level> _levels;
    Twitch _twitch;  // query online state
    std::string _modPlsDelete; // TODO moderator name
    const size_t maxSize = 15;
};
