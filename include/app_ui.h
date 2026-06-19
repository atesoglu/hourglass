#pragma once
#include "timer_engine.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <atomic>

class AppUi {
public:
    explicit AppUi(TimerEngine& engine);
    void run();

private:
    TimerEngine& timer;
    std::atomic<bool> running{true};
    ftxui::Element renderLayout();
};