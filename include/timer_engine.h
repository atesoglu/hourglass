#pragma once
#include <chrono>
#include <string>

enum class PomodoroState { Focus, ShortBreak, LongBreak, Paused };

class TimerEngine {
public:
    TimerEngine();

    void update();

    void togglePause();

    void reset();

    void skipToTrigger();

    // Getters and Setters for custom durations (stored as minutes)
    [[nodiscard]] int getFocusMins() const { return focus_mins; }
    [[nodiscard]] int getShortBreakMins() const { return short_break_mins; }
    [[nodiscard]] int getLongBreakMins() const { return long_break_mins; }

    void setFocusMins(int mins);
    void setShortBreakMins(int mins);
    void setLongBreakMins(int mins);

    [[nodiscard]] std::string formatTime() const;
    [[nodiscard]] float progressPercentage() const;
    [[nodiscard]] std::string stateToString() const;

    // --- Analytics Getters ---
    [[nodiscard]] int getTodayCompletedCount() const; // Reads history from disk

private:
    std::chrono::seconds remaining_time;
    PomodoroState current_state;
    PomodoroState cached_state;
    int completed_cycles;

    // Configurable time blocks (default settings)
    int focus_mins = 25;
    int short_break_mins = 5;
    int long_break_mins = 15;

    void switchState();
    void logSessionToHistory(); // Appends completion timestamp to CSV
};
