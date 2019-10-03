class Timer {
  public:
    void StartTimer();
    void PauseTimer();
    void ResetTimer();

  private:
    // std::string _outOfTimeMessage;
    // Chat _chat;
    std::function<...> _callback;
}
