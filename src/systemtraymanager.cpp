#include "systemtraymanager.h"
#include "mainwindow.h"
#include "soundplayer.h"
#include <QApplication>
#include <QSettings>
#include <QAction>
#include <QIcon>
#include <QPainter>
#include <QPolygon>

SystemTrayManager::SystemTrayManager(MainWindow *mainWindow, QObject *parent)
    : QObject(parent)
    , m_trayIcon(nullptr)
    , m_contextMenu(nullptr)
    , m_soundPresetGroup(nullptr)
    , m_mainWindow(mainWindow)
    , m_currentSoundPreset("Chime")
{
    if (isSystemTrayAvailable()) {
        setupTrayIcon();
        setupContextMenu();
        loadSoundPreset();
    }
}

SystemTrayManager::~SystemTrayManager()
{
    // Qt parent-child relationship handles cleanup
}

bool SystemTrayManager::isSystemTrayAvailable() const
{
    return QSystemTrayIcon::isSystemTrayAvailable();
}

void SystemTrayManager::show()
{
    if (m_trayIcon) {
        m_trayIcon->show();
    }
}

QIcon SystemTrayManager::createHourglassIcon()
{
    // Try to use system theme icon first
    QIcon icon = QIcon::fromTheme("hourglass");
    if (!icon.isNull()) {
        return icon;
    }
    
    // Create a simple hourglass icon with multiple sizes for better quality
    QIcon hourglassIcon;
    
    // Create icons at different sizes (16, 32, 48, 64, 128, 256)
    QList<int> sizes = {16, 32, 48, 64, 128, 256};
    
    for (int size : sizes) {
        QPixmap pixmap(size, size);
        pixmap.fill(Qt::transparent);
        
        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        
        // Scale the drawing based on size
        qreal scale = size / 32.0;
        qreal penWidth = 2.0 * scale;
        
        painter.setPen(QPen(Qt::black, penWidth));
        painter.setBrush(Qt::yellow);
        
        // Draw hourglass shape
        QPolygon topHalf;
        qreal centerX = size / 2.0;
        qreal topY = 4.0 * scale;
        qreal middleY = 12.0 * scale;
        qreal bottomY = size - 4.0 * scale;
        
        topHalf << QPoint(centerX, topY)
                << QPoint(centerX + 8.0 * scale, middleY)
                << QPoint(centerX - 8.0 * scale, middleY);
        
        QPolygon bottomHalf;
        bottomHalf << QPoint(centerX - 8.0 * scale, middleY)
                   << QPoint(centerX + 8.0 * scale, middleY)
                   << QPoint(centerX, bottomY);
        
        painter.drawPolygon(topHalf);
        painter.drawPolygon(bottomHalf);
        
        hourglassIcon.addPixmap(pixmap);
    }
    
    return hourglassIcon;
}

void SystemTrayManager::setupTrayIcon()
{
    m_trayIcon = new QSystemTrayIcon(this);
    
    QIcon icon = createHourglassIcon();
    m_trayIcon->setIcon(icon);
    m_trayIcon->setToolTip("Hourglass Timer");
    
    connect(m_trayIcon, &QSystemTrayIcon::activated, 
            this, &SystemTrayManager::onTrayIconActivated);
}

void SystemTrayManager::setupContextMenu()
{
    m_contextMenu = new QMenu();
    
    // Sound preset group
    m_soundPresetGroup = new QActionGroup(this);
    m_soundPresetGroup->setExclusive(true);
    
    QAction *chimeAction = new QAction("Sound: Chime", this);
    chimeAction->setCheckable(true);
    chimeAction->setData("Chime");
    m_soundPresetGroup->addAction(chimeAction);
    
    QAction *beepAction = new QAction("Sound: Beep", this);
    beepAction->setCheckable(true);
    beepAction->setData("Beep");
    m_soundPresetGroup->addAction(beepAction);
    
    QAction *bellAction = new QAction("Sound: Bell", this);
    bellAction->setCheckable(true);
    bellAction->setData("Bell");
    m_soundPresetGroup->addAction(bellAction);
    
    QAction *noneAction = new QAction("Sound: None", this);
    noneAction->setCheckable(true);
    noneAction->setData("None");
    m_soundPresetGroup->addAction(noneAction);
    
    m_contextMenu->addActions(m_soundPresetGroup->actions());
    m_contextMenu->addSeparator();
    
    QAction *showWindowAction = new QAction("Show Window", this);
    connect(showWindowAction, &QAction::triggered, [this]() {
        if (m_mainWindow) {
            m_mainWindow->show();
            m_mainWindow->raise();
            m_mainWindow->activateWindow();
        }
    });
    m_contextMenu->addAction(showWindowAction);
    
    QAction *quitAction = new QAction("Quit", this);
    connect(quitAction, &QAction::triggered, this, &SystemTrayManager::quitRequested);
    m_contextMenu->addAction(quitAction);
    
    connect(m_soundPresetGroup, &QActionGroup::triggered,
            this, &SystemTrayManager::onSoundPresetSelected);
    
    m_trayIcon->setContextMenu(m_contextMenu);
}

void SystemTrayManager::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger) {
        // Left-click: toggle window visibility
        if (m_mainWindow) {
            if (m_mainWindow->isVisible()) {
                m_mainWindow->hide();
            } else {
                m_mainWindow->show();
                m_mainWindow->raise();
                m_mainWindow->activateWindow();
            }
        }
    }
}

void SystemTrayManager::onSoundPresetSelected(QAction *action)
{
    if (action) {
        m_currentSoundPreset = action->data().toString();
        QSettings settings;
        settings.setValue("soundPreset", m_currentSoundPreset);
        
        // Notify MainWindow to update sound player (if needed)
        // The SoundPlayer in MainWindow will read from QSettings on next play
    }
}

void SystemTrayManager::loadSoundPreset()
{
    QSettings settings;
    QString preset = settings.value("soundPreset", "Chime").toString();
    m_currentSoundPreset = preset;
    
    // Set the checked state of the corresponding action
    for (QAction *action : m_soundPresetGroup->actions()) {
        if (action->data().toString() == preset) {
            action->setChecked(true);
            break;
        }
    }
}
