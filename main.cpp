#include <QApplication>
#include "mainwindow.h"
#include "systemtraymanager.h"
#include "soundplayer.h"

int main(int argc, char *argv[]) {
    try {
        QApplication app(argc, argv);
        app.setQuitOnLastWindowClosed(false); // Keep app running when window is closed (system tray)

        MainWindow mainWindow;
        SystemTrayManager trayManager(&mainWindow);
        
        // Connect system tray quit to application exit
        QObject::connect(&trayManager, &SystemTrayManager::quitRequested, 
                        &app, &QApplication::quit);
        
        // Sound preset changes are handled through QSettings - MainWindow's SoundPlayer reads from QSettings
        
        if (trayManager.isSystemTrayAvailable()) {
            trayManager.show();
            mainWindow.show();
        } else {
            // If system tray is not available, show window and allow normal closing
            mainWindow.show();
        }

        return app.exec();
    } catch (const std::exception &e) {
        //logException(e.what());
        throw;
    } catch (...) {
        //logException();
        throw;
    }
}
