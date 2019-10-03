#include "level.h"

#include <deque>

class QuesoQueue {
  public:
    QuesoQueue(Twitch twitch);

    /**
     * Adds a level to queue; limit one per viewer.
     */
    void Add(Level level);

    /**
     * Removes the level submitted by the user. Or, if the user is admin,
     * remove the level with the specified code.
     */
    void Remove(std::string username, std::string levelCode = "");

    /**
     * Split the stored level queue into online and offline for printing
     */
    std::tuple<std::queue<Level>, std::queue<Level>> List();

    void SaveState();
    void LoadLastState();

  private:
    bool isOnline(Level l);
    std::deque<Level> _levels;
    Twitch _twitch;  // query online state
}
