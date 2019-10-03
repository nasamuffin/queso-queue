// Maybe redundant to Twitch API.
#include <string>
#include <stringstream>

class Chat {
  public:
    Chat(QuesoQueue qq);
    void HandleMessage(std::stringstream message, std::string sender);
    void WriteMessage(std::string message);

  private:
    std::string NextLevelMessage(Level l);

    bool _canAddToQueue;
    QuesoQueue _qq;
    std::string _helpText;
    std::string _priorityText;
}
