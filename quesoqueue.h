#pragma once

#include "level.h"
#include "twitch.h"

#include <deque>
#include <optional>
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
     * remove the level with the specified username
     */
    std::string Remove(std::string username);
    std::string ModRemove(std::string username);

    /**
     * Replaces the level a user currently has in queue with a new code.
     */
    std::string Replace(std::string username, std::string newLevelCode);

    /**
     * Pop the top of the queue and return the new top (subject to priority queue split)
     */
    std::optional<Level> Next();

    /**
     * Report the level currently being played (not subject to priority queue split)
     */
    std::optional<Level> Current();

    /**
     * Takes the front level, and pushes it to the back.
     */
    std::optional<Level> Punt();

    /**
     * Split the stored level queue into online and offline for printing
     */
    PriorityQueso List();

    /**
     * Report the requester's level's position in the JIT pqueue.
     */
    int Position(std::string username);

    void SaveState();
    void LoadLastState();

  private:
    bool isValidLevelCode(std::string levelCode);
    bool isOnline(Level l);
    std::deque<Level> _levels;
    std::optional<Level> _current;
    Twitch _twitch;  // query online state
    const size_t maxSize = 15;
};
