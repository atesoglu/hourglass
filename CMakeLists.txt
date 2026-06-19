cmake_minimum_required(VERSION 3.25)
project(hourglass LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
FetchContent_Declare(
        ftxui
        GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
        GIT_TAG        v5.0.0
)
FetchContent_MakeAvailable(ftxui)

add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -O3)

add_executable(hourglass
        src/main.cpp
        src/app_ui.cpp
        src/timer_engine.cpp
)

target_include_directories(hourglass PRIVATE include)
target_link_libraries(hourglass PRIVATE ftxui::screen ftxui::dom ftxui::component)