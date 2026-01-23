#pragma once

#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QActionGroup>

class MainWindow;

class SystemTrayManager : public QObject {
    Q_OBJECT

public:
    explicit SystemTrayManager(MainWindow *mainWindow, QObject *parent = nullptr);
    ~SystemTrayManager() override;
    static QIcon createHourglassIcon();

    bool isSystemTrayAvailable() const;
    void show();

signals:
    void quitRequested();

private slots:
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void onSoundPresetSelected(QAction *action);

private:
    void setupTrayIcon();
    void setupContextMenu();
    void loadSoundPreset();

    QSystemTrayIcon *m_trayIcon;
    QMenu *m_contextMenu;
    QActionGroup *m_soundPresetGroup;
    MainWindow *m_mainWindow;
    QString m_currentSoundPreset;
};
