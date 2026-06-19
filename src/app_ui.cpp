// src/app_ui.cpp
#include "app_ui.h"
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp> // Ensure this is present
#include <ftxui/component/loop.hpp>
#include <thread>
#include <vector>
#include <string>

using namespace ftxui;

AppUi::AppUi(TimerEngine &engine) : timer(engine) {
}

void AppUi::addLogMessage(const std::string &message) {
    // Keep only the last 4 log entries so it doesn't overflow our split panel
    if (system_logs.size() >= 4) {
        system_logs.erase(system_logs.begin());
    }
    system_logs.push_back("» " + message);
}

void AppUi::run() {
    auto screen = ScreenInteractive::Fullscreen();

    int slider_focus = timer.getFocusMins();
    int slider_short = timer.getShortBreakMins();
    int slider_long = timer.getLongBreakMins();

    int selected_preset = 0;
    std::vector<std::string> presets = {
        " Classic Pomodoro (25m / 5m / 15m) ",
        " Short Sprint     (15m / 3m / 10m) ",
        " Extended Deep    (50m / 10m / 30m) ",
        " Custom Layout    (Manual Sliders) "
    };

    auto preset_radiobox = Radiobox(&presets, &selected_preset);

    // Clean factory function syntax to resolve the compilation error
    auto focus_slider = Slider("Focus (min): ", &slider_focus, 1, 60, 1);
    auto short_slider = Slider("Short (min): ", &slider_short, 1, 30, 1);
    auto long_slider = Slider("Long (min):  ", &slider_long, 1, 45, 1);

    auto settings_container = Container::Vertical({
        preset_radiobox,
        focus_slider,
        short_slider,
        long_slider,
    });

    auto catch_keys = CatchEvent(settings_container, [&](Event event) {
        if (event == Event::Character('q') || event == Event::Escape) {
            running = false;
            screen.ExitLoopClosure()();
            return true;
        }
        if (event == Event::Character(' ')) {
            timer.togglePause();
            addLogMessage("Toggled session pause state");
            screen.PostEvent(Event::Custom); // <-- CRITICAL: Forces FTXUI to draw the new log instantly!
            return true;
        }
        if (event == Event::Character('r')) {
            timer.reset();
            addLogMessage("Manual system timer reset issued");
            screen.PostEvent(Event::Custom); // <-- CRITICAL
            return true;
        }
        if (event == Event::Character('t')) {
            timer.skipToTrigger();
            addLogMessage("Fast-forward skip test triggered");
            screen.PostEvent(Event::Custom); // <-- CRITICAL
            return true;
        }
        return false;
    });

    auto ui_render = Renderer(catch_keys, [&] {
        if (selected_preset == 0) {
            slider_focus = 25;
            slider_short = 5;
            slider_long = 15;
        } else if (selected_preset == 1) {
            slider_focus = 15;
            slider_short = 3;
            slider_long = 10;
        } else if (selected_preset == 2) {
            slider_focus = 50;
            slider_short = 10;
            slider_long = 30;
        }

        timer.setFocusMins(slider_focus);
        timer.setShortBreakMins(slider_short);
        timer.setLongBreakMins(slider_long);

        // --- LEFT PANEL: Workspace Timer Countdown & State ---
        auto left_title = text(" WORKSPACE ") | bold | center | color(Color::Cyan);
        auto time_display = text(timer.formatTime()) | size(HEIGHT, EQUAL, 3) | center | bold | color(Color::White);
        auto status_text = text(timer.stateToString()) | center | dim;
        auto progress_bar = gauge(timer.progressPercentage()) | border | color(Color::Green);

        int completed_today = timer.getTodayCompletedCount();
        std::string progress_blocks = "";
        for (int i = 0; i < 8; ++i) {
            progress_blocks += (i < completed_today) ? "█ " : "░ ";
        }

        auto analytics_box = vbox({
                                 text("TODAY'S PROGRESS:") | bold | color(Color::Yellow),
                                 hbox({
                                     text(progress_blocks) | color(Color::Green),
                                     text(" (" + std::to_string(completed_today) + " completed)") | dim
                                 })
                             }) | center | border | size(WIDTH, EQUAL, 36);

        auto left_panel = vbox({
                              left_title,
                              separator(),
                              vbox({
                                  status_text,
                                  separatorEmpty(),
                                  time_display,
                                  separatorEmpty(),
                                  progress_bar,
                                  separatorEmpty(),
                                  analytics_box
                              }) | size(WIDTH, EQUAL, 40) | center
                          }) | flex;

        // --- RIGHT PANEL: Custom Duration Settings & Presets ---
        auto right_title = text(" ADJUST RUNTIME ") | bold | center | color(Color::Cyan);

        auto slider_view = vbox({
            focus_slider->Render() | (selected_preset != 3 ? dim : nothing),
            separatorEmpty(),
            short_slider->Render() | (selected_preset != 3 ? dim : nothing),
            separatorEmpty(),
            long_slider->Render() | (selected_preset != 3 ? dim : nothing),
        });

        // Generate the scrolling log output components reactively
        Elements log_elements;
        if (system_logs.empty()) {
            log_elements.push_back(text("System idling. Awaiting initialization...") | dim);
        } else {
            for (const auto &log: system_logs) {
                log_elements.push_back(text(log) | color(Color::DarkCyan));
            }
        }

        auto live_log_box = vbox(std::move(log_elements))
                            | frame
                            | border
                            | color(Color::Cyan)
                            | size(HEIGHT, EQUAL, 6);

        // FIXED: Changed separatorLine() down below to a standard separator()
        auto right_panel = vbox({
                               right_title,
                               separator(),
                               vbox({
                                   text("Select a session profile preset:") | dim,
                                   preset_radiobox->Render() | border | color(Color::DarkCyan),
                                   separator(),
                                   text("Manual Interval Customization:") | dim,
                                   separatorEmpty(),
                                   slider_view
                               }) | size(WIDTH, EQUAL, 45) | center
                           }) | flex;

        // --- UTILITY FOOTER ---
        auto controls_footer = hbox({
                                   text(" [SPACE] Toggle ") | border,
                                   text(" [R] Reset ") | border,
                                   text(" [T] Trigger End ") | border | color(Color::Yellow),
                                   text(" [Q] Quit ") | border
                               }) | center;

        // Returns a clean, uniform top-level ftxui::Element
        return vbox({
                   text(" HOURGLASS // POMODORO ") | bold | center | color(Color::DarkCyan),
                   separatorDouble(),
                   hbox({
                       left_panel,
                       separator(),
                       right_panel
                   }) | flex,
                   separatorDouble(),
                   controls_footer
               }) | border;
    });

    Loop loop(&screen, ui_render);

    std::thread timer_thread([&] {
        while (running) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            if (!running) break;
            timer.update();
            screen.PostEvent(Event::Custom);
        }
    });

    loop.Run();

    running = false;
    if (timer_thread.joinable()) {
        timer_thread.join();
    }
}
