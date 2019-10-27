#include "timer.h"

#include "gtest/gtest.h"

// Constructor puts timer in good state
  // _running is false
TEST(TimerTest, ConstructorHappyPath) {
    Timer t;
    // some amount of time on the clock
    ASSERT_GT(t.Remaining(), std::chrono::seconds(0));
    // TODO - assert not running
}

// Start() puts timer in good state
TEST(TimerTest, StartHappyPath) {
    Timer t;

    t.Start();

    // TODO: check timeRemaining vs default
    //std::chrono::seconds timediff = std::chrono::duration_cast<std::chrono::seconds>(
    //    <default time value> - t.Remaining());
    //ASSERT_GT(timediff, std::chrono::seconds(-1));
    //ASSERT_LT(timediff, std::chrono::seconds(1));
    // TODO
    // _finishTime = now + 10m
    // _timeRemaining = 10m
    // _running = true
}

// Start() is idempotent - I can hit it a lot and nothing happens
TEST(TimerTest, StartIdempotence) {
    Timer t;

    t.Start();
    t.Start();
}

// Pause() puts timer in good state
TEST(TimerTest, PauseHappyPath) {
    Timer t;

    t.Start();
    t.Pause();
}

TEST(TimerTest, PauseAlreadyPausedDoesntCrash) {
    Timer t;

    t.Pause();
    t.Pause();
}

// Reset is happy with running timer
TEST(TimerTest, ResetRunningTimer) {
    Timer t;

    t.Start();
    t.Reset();
    // TODO: test that timer is running afterwards
}

TEST(TimerTest, ResetStoppedTimer) {
    Timer t;

    t.Pause();
    t.Reset();
}

// CheckTimer() doesn't crash?
// TODO: check timer when it's expired.
// This is waiting for runtime-timer-length-config to be useful.
TEST(TimerTest, CheckTimerHappyPath) {
    Timer t;

    t.CheckTimer();
}

// CheckTimer() won't trigger even if it's just paused.
TEST(TimerTest, CheckTimerWontMisfireOnPause) {
    Timer t;

    t.Reset();
    t.Pause();

    ASSERT_FALSE(t.CheckTimer());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
