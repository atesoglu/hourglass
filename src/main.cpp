#include "timer_engine.h"
#include "app_ui.h"

int main() {
    TimerEngine engine;
    AppUi app(engine);
    app.run();
    return 0;
}