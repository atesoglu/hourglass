// src/app_ui.cpp
#include "app_ui.h"
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/loop.hpp>
#include <thread>

using namespace ftxui;

AppUi::AppUi(TimerEngine& engine) : timer(engine) {}

void AppUi::run() {
    auto screen = ScreenInteractive::Fullscreen();

    // Configuration values tied directly to your FTXUI sliders
    int slider_focus = timer.getFocusMins();
    int slider_short = timer.getShortBreakMins();
    int slider_long = timer.getLongBreakMins();

    // --- NEW: Presets Setup ---
    int selected_preset = 0; // Tracks which profile is selected
    std::vector<std::string> presets = {
        " Classic Pomodoro (25m / 5m / 15m) ",
        " Short Sprint     (15m / 3m / 10m) ",
        " Extended Deep    (50m / 10m / 30m) ",
        " Custom Layout    (Manual Sliders) "
    };

    auto preset_radiobox = Radiobox(&presets, &selected_preset);
    // --------------------------

    // Create the interactive text sliders on the right panel
    auto focus_slider = Slider("Focus (min): ", &slider_focus, 1, 60, 1);
    auto short_slider = Slider("Short (min): ", &slider_short, 1, 30, 1);
    auto long_slider  = Slider("Long (min):  ", &slider_long,  1, 45, 1);

    // Group the sliders vertically into a layout container
    auto settings_container = Container::Vertical({
        preset_radiobox, // Put presets at the top of the controls
        focus_slider,
        short_slider,
        long_slider,
    });

    // Handle global keyboard shortcuts cleanly across the single viewport
    auto catch_keys = CatchEvent(settings_container, [&](Event event) {
        if (event == Event::Character('q') || event == Event::Escape) {
            running = false;
            screen.ExitLoopClosure()();
            return true;
        }
        if (event == Event::Character(' ')) {
            timer.togglePause();
            return true;
        }
        if (event == Event::Character('r')) {
            timer.reset();
            return true;
        }
        if (event == Event::Character('t')) {
            timer.skipToTrigger();
            return true;
        }
        return false;
    });

    auto ui_render = Renderer(catch_keys, [&] {

        // --- NEW: Process Preset Selection ---
        if (selected_preset == 0) { // Classic
            slider_focus = 25; slider_short = 5; slider_long = 15;
        } else if (selected_preset == 1) { // Short Sprint
            slider_focus = 15; slider_short = 3; slider_long = 10;
        } else if (selected_preset == 2) { // Extended Deep
            slider_focus = 50; slider_short = 10; slider_long = 30;
        }
        // If selected_preset == 3 (Custom), we do nothing and let the user slide manual values!
        // --------------------------------------

        // Automatically sync runtime changes from layout variables back to the core engine
        timer.setFocusMins(slider_focus);
        timer.setShortBreakMins(slider_short);
        timer.setLongBreakMins(slider_long);

        // --- LEFT PANEL: Workspace Timer Countdown & State ---
        auto left_title = text(" WORKSPACE ") | bold | center | color(Color::Cyan);
        auto time_display = text(timer.formatTime()) | size(HEIGHT, EQUAL, 3) | center | bold | color(Color::White);
        auto status_text = text(timer.stateToString()) | center | dim;
        auto progress_bar = gauge(timer.progressPercentage()) | border | color(Color::Green);

        // Fetch session counts from history file dynamically
        int completed_today = timer.getTodayCompletedCount();

        // Generate a cool retro visual grid indicator matrix (e.g., "████░░░░")
        std::string progress_blocks = "";
        for (int i = 0; i < 8; ++i) {
            progress_blocks += (i < completed_today) ? "█ " : "░ ";
        }

        auto analytics_box = vbox({
            text("TODAY'S PROGRESS:") | bold | color(Color::Yellow),
            hbox({
                text(progress_blocks) | color(Color::Green),
                text("(" + std::to_string(completed_today) + " completed)") | dim
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
                analytics_box // Added milestone metrics display to grid interface
            }) | size(WIDTH, EQUAL, 40) | center
        }) | flex;

        // --- RIGHT PANEL: Custom Duration Settings ---
        auto right_title = text(" ADJUST RUNTIME ") | bold | center | color(Color::Cyan);


        // Disable visual focus feedback on sliders if a fixed preset is active
        auto slider_view = vbox({
            focus_slider->Render() | (selected_preset != 3 ? dim : nothing),
            separatorEmpty(),
            short_slider->Render() | (selected_preset != 3 ? dim : nothing),
            separatorEmpty(),
            long_slider->Render()  | (selected_preset != 3 ? dim : nothing),
        });

        auto right_panel = vbox({
            right_title,
            separator(),
            vbox({
                text("Use Arrow Keys [↑/↓/←/→] to adjust lengths:") | dim,
                separatorEmpty(),
                focus_slider->Render(),
                separatorEmpty(),
                short_slider->Render(),
                separatorEmpty(),
                long_slider->Render(),
            }) | size(WIDTH, EQUAL, 45) | center
        }) | flex;

        // --- UTILITY FOOTER ---
        auto controls_footer = hbox({
            text(" [SPACE] Toggle ")  | border,
            text(" [R] Reset ")        | border,
            text(" [T] Trigger End ")  | border | color(Color::Yellow),
            text(" [Q] Quit ")         | border
        }) | center;

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