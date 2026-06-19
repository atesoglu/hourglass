# ⏳ HOURGLASS // POMODORO TUI

> **"Pomodoro TUI, because why not? GUIs for wussies."**

A high-performance, responsive split-screen Pomodoro and session-tracking engine built completely from scratch in modern **C++20** utilizing the **FTXUI** reactive terminal framework. Optimized for low-overhead terminal desktop environments.

---

## 🚀 Key Features

*   **Reactive Split-Screen Architecture:** Real-time dual-panel view parsing deep-work performance blocks on the left and engine runtime controls on the right.
*   **Dynamic Session Profiles:** Fast-swapping presets (Classic Pomodoro, Short Sprint, Extended Deep) alongside manual slider overrides for pinpoint custom interval adjustments.
*   **Live Event Log Console:** An asynchronous, interactive logging box tracking manual overrides, session pauses, skips, and state mutations live.
*   **Visual Analytics Layer:** A persistent progress visualization component utilizing precise block character rendering arrays (`██░░`) to monitor completed loops at a glance.
*   **Asynchronous Engine Loop:** A non-blocking dual-threaded tracking system running on an independent temporal thread to shield timer accuracy from terminal UI rendering bottlenecks.

---

## ⌨️ Global Shortcuts

| Shortcut | Action Description |
| :--- | :--- |
| `[SPACE]` | Toggle Session Timer (Pause / Resume) |
| ``[R]` ` | Hard Reset Current Interval State |
| ``[T]` ` | Trigger Fast-Forward End (Skip/Fast-Test Session Block) |
| ``[Q]` / `[ESC]`` | Safely Exit Application & Join Active Threads |

---

## 🛠️ Build and Dependencies

### System Requirements
*   **Compiler:** G++ or Clang with full **C++20** language specification support.
*   **Build Automation:** CMake (>= 3.20) & Ninja Build Backend.
*   **Framework Layer:** FTXUI (Fetched seamlessly automatically via CMake `FetchContent`).

### Compilation Controls
Fire up your terminal or run targets directly via CLion using:

```bash
# Generate Build Cache
cmake -B build -G Ninja

# Compile Project Target Binary
cmake --build build

# Launch the Application Workspace
./build/hourglass