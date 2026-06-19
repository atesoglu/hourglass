#include "timer_engine.h"
#include <iomanip>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <fstream> // For file I/O operations
#include <ctime>   // For pulling calendar dates

TimerEngine::TimerEngine()
    : remaining_time(1500), current_state(PomodoroState::Paused), cached_state(PomodoroState::Focus), completed_cycles(0) {
}

void TimerEngine::update() {
    if (current_state == PomodoroState::Paused) return;

    if (remaining_time.count() > 0) {
        remaining_time -= std::chrono::seconds(1);
    } else {
        switchState();
    }
}

void TimerEngine::togglePause() {
    if (current_state == PomodoroState::Paused) {
        current_state = cached_state;
    } else {
        cached_state = current_state;
        current_state = PomodoroState::Paused;
    }
}

void TimerEngine::reset() {
    current_state = PomodoroState::Paused; // Freeze the clock on reset
    cached_state = PomodoroState::Focus; // Prep it to start fresh into Focus
    remaining_time = std::chrono::minutes(focus_mins); // Reset back to 25 mins
}

void TimerEngine::switchState() {
    std::string notification_msg;

    if (current_state == PomodoroState::Focus) {
        completed_cycles++;

        // Log this milestone to disk before updating states
        logSessionToHistory();

        if (completed_cycles % 4 == 0) {
            current_state = PomodoroState::LongBreak;
            remaining_time = std::chrono::minutes(long_break_mins);
            notification_msg = "Focus session complete! Take a long break.";
        } else {
            current_state = PomodoroState::ShortBreak;
            remaining_time = std::chrono::minutes(short_break_mins);
            notification_msg = "Focus session complete! Take a short break.";
        }
    } else {
        current_state = PomodoroState::Focus;
        remaining_time = std::chrono::minutes(focus_mins);
        notification_msg = "Break is over! Time to focus.";
    }

    std::cout << "\a" << std::flush;
    std::string command = "notify-send '⏳ Hourglass' '" + notification_msg + "' --icon=alarm-clock &";
    std::system(command.c_str());
}

std::string TimerEngine::formatTime() const {
    const auto minutes = std::chrono::duration_cast<std::chrono::minutes>(remaining_time);
    const auto seconds = remaining_time - minutes;
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << minutes.count() << ":"
            << std::setw(2) << std::setfill('0') << seconds.count();
    return oss.str();
}

float TimerEngine::progressPercentage() const {
    float total = 1500.0f; // backup fallback

    if (cached_state == PomodoroState::Focus) {
        total = static_cast<float>(focus_mins * 60);
    } else if (cached_state == PomodoroState::ShortBreak) {
        total = static_cast<float>(short_break_mins * 60);
    } else if (cached_state == PomodoroState::LongBreak) {
        total = static_cast<float>(long_break_mins * 60);
    }

    if (total <= 0.0f) return 0.0f;
    return (total - static_cast<float>(remaining_time.count())) / total;
}

std::string TimerEngine::stateToString() const {
    switch (current_state) {
        case PomodoroState::Focus: return "FOCUS SESSION";
        case PomodoroState::ShortBreak: return "SHORT BREAK";
        case PomodoroState::LongBreak: return "LONG BREAK";
        case PomodoroState::Paused:
            // If we are paused at the very beginning of a cycle
            if (progressPercentage() == 0.0f) {
                return "READY - PRESS [SPACE] TO START";
            }
            return "PAUSED";
    }
    return "UNKNOWN";
}

void TimerEngine::skipToTrigger() {
    // If it's paused, unpause it first so it processes the transition cleanly
    if (current_state == PomodoroState::Paused) {
        current_state = cached_state;
    }
    remaining_time = std::chrono::seconds(0);
    update(); // Run the update immediately to fire notifications and switch states
}

void TimerEngine::setFocusMins(int mins) {
    if (mins < 1) return;
    focus_mins = mins;
    if (current_state == PomodoroState::Paused && progressPercentage() == 0.0f && cached_state == PomodoroState::Focus) {
        remaining_time = std::chrono::minutes(focus_mins);
    }
}

void TimerEngine::setShortBreakMins(int mins) {
    if (mins < 1) return;
    short_break_mins = mins;
    if (current_state == PomodoroState::Paused && progressPercentage() == 0.0f && cached_state == PomodoroState::ShortBreak) {
        remaining_time = std::chrono::minutes(short_break_mins);
    }
}

void TimerEngine::setLongBreakMins(int mins) {
    if (mins < 1) return;
    long_break_mins = mins;
    if (current_state == PomodoroState::Paused && progressPercentage() == 0.0f && cached_state == PomodoroState::LongBreak) {
        remaining_time = std::chrono::minutes(long_break_mins);
    }
}

static std::string getTodayDateString() {
    std::time_t t = std::time(nullptr);
    std::tm *now = std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(now, "%Y-%m-%d");
    return oss.str();
}

void TimerEngine::logSessionToHistory() {
    std::ofstream file("hourglass_history.csv", std::ios::app);
    if (file.is_open()) {
        file << getTodayDateString() << ",focus_completed\n";
    }
}

int TimerEngine::getTodayCompletedCount() const {
    std::ifstream file("hourglass_history.csv");
    if (!file.is_open()) return 0;

    std::string today = getTodayDateString();
    std::string line;
    int count = 0;

    while (std::getline(file, line)) {
        // Simple fast CSV line matching: check if line starts with today's date string
        if (line.rfind(today, 0) == 0) {
            count++;
        }
    }
    return count;
}
