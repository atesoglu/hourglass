#pragma once
#include "timer_engine.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <atomic>
#include <vector>  // Ensure this is included
#include <string>  // Ensure this is included

class AppUi {
public:
    explicit AppUi(TimerEngine& engine);
    void run();

private:
    TimerEngine& timer;
    std::atomic<bool> running{true};

    // --- New Logger Cache ---
    std::vector<std::string> system_logs;
    void addLogMessage(const std::string& message);

    ftxui::Element renderLayout();
};