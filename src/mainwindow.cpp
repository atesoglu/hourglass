#include "mainwindow.h"
#include "timerengine.h"
#include "animatedbackgroundwidget.h"
#include "soundplayer.h"
#include "systemtraymanager.h"
#include <QApplication>
#include <QResizeEvent>
#include <QCloseEvent>
#include <QSettings>
#include <QShortcut>
#include <QKeySequence>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_centralWidget(nullptr)
    , m_mainLayout(nullptr)
    , m_controlLayout(nullptr)
    , m_decreaseButton(nullptr)
    , m_decrease5Button(nullptr)
    , m_minutesSpinBox(nullptr)
    , m_increase5Button(nullptr)
    , m_increaseButton(nullptr)
    , m_startButton(nullptr)
    , m_backgroundWidget(nullptr)
    , m_timerEngine(nullptr)
    , m_soundPlayer(nullptr)
{
    setupWindowFlags();
    setupUI();
    
    m_timerEngine = new TimerEngine(this);
    m_soundPlayer = new SoundPlayer(this);
    
    connect(m_timerEngine, &TimerEngine::timerCompleted, this, &MainWindow::onTimerCompleted);
    connect(m_timerEngine, &TimerEngine::timerCompleted, m_soundPlayer, &SoundPlayer::play);
    connect(m_timerEngine, &TimerEngine::progressChanged, this, &MainWindow::onProgressChanged);
    connect(m_timerEngine, &TimerEngine::timeRemainingChanged, this, &MainWindow::onTimeRemainingChanged);
    
    // Keyboard shortcuts
    QShortcut *spaceShortcut = new QShortcut(QKeySequence(Qt::Key_Space), this);
    connect(spaceShortcut, &QShortcut::activated, this, &MainWindow::onStartButtonClicked);
    
    QShortcut *plusShortcut = new QShortcut(QKeySequence(Qt::Key_Plus), this);
    connect(plusShortcut, &QShortcut::activated, this, &MainWindow::onIncreaseClicked);
    
    QShortcut *minusShortcut = new QShortcut(QKeySequence(Qt::Key_Minus), this);
    connect(minusShortcut, &QShortcut::activated, this, &MainWindow::onDecreaseClicked);
    
    // Load window position
    QSettings settings;
    QPoint pos = settings.value("windowPosition", QPoint(100, 100)).toPoint();
    move(pos);
}

MainWindow::~MainWindow() = default;

void MainWindow::setupWindowFlags()
{
    setWindowFlags(
        Qt::WindowStaysOnTopHint |
        Qt::Window |
        Qt::WindowTitleHint |
        Qt::WindowCloseButtonHint
    );
    setFixedSize(400, 300);
    setWindowTitle("Hourglass Timer");
    
    // Set window icon (same as system tray)
    setWindowIcon(SystemTrayManager::createHourglassIcon());
}

void MainWindow::setupUI()
{
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    
    m_mainLayout = new QVBoxLayout(m_centralWidget);
    m_mainLayout->setContentsMargins(30, 30, 30, 30);
    m_mainLayout->setSpacing(20);
    
    // Create animated background widget (bottom layer)
    m_backgroundWidget = new AnimatedBackgroundWidget(m_centralWidget);
    m_backgroundWidget->lower(); // Keep behind other widgets
    m_backgroundWidget->setGeometry(0, 0, width(), height());
    
    // Control panel with spinbox and buttons
    m_controlLayout = new QHBoxLayout();
    m_controlLayout->setSpacing(15);
    
    m_decreaseButton = new QPushButton("-", this);
    m_decreaseButton->setFixedSize(40, 40);
    m_decreaseButton->setFont(QFont("Arial", 18, QFont::Bold));
    
    m_minutesSpinBox = new QSpinBox(this);
    m_minutesSpinBox->setRange(1, 120);
    m_minutesSpinBox->setValue(5);
    m_minutesSpinBox->setSuffix(" min");
    m_minutesSpinBox->setFont(QFont("Arial", 20));
    m_minutesSpinBox->setAlignment(Qt::AlignCenter);
    m_minutesSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
    
    m_decrease5Button = new QPushButton("-5", this);
    m_decrease5Button->setFixedSize(40, 40);
    m_decrease5Button->setFont(QFont("Arial", 14, QFont::Bold));
    
    m_increase5Button = new QPushButton("+5", this);
    m_increase5Button->setFixedSize(40, 40);
    m_increase5Button->setFont(QFont("Arial", 14, QFont::Bold));
    
    m_increaseButton = new QPushButton("+", this);
    m_increaseButton->setFixedSize(40, 40);
    m_increaseButton->setFont(QFont("Arial", 18, QFont::Bold));
    
    m_controlLayout->addWidget(m_decreaseButton);
    m_controlLayout->addWidget(m_decrease5Button);
    m_controlLayout->addWidget(m_minutesSpinBox);
    m_controlLayout->addWidget(m_increase5Button);
    m_controlLayout->addWidget(m_increaseButton);
    m_controlLayout->setAlignment(Qt::AlignCenter);
    
    m_mainLayout->addLayout(m_controlLayout);
    m_mainLayout->addStretch();
    
    // Start button
    m_startButton = new QPushButton("Start", this);
    m_startButton->setFont(QFont("Arial", 14, QFont::Bold));
    m_startButton->setMinimumHeight(50);
    m_mainLayout->addWidget(m_startButton);
    
    // Connect signals
    connect(m_startButton, &QPushButton::clicked, this, &MainWindow::onStartButtonClicked);
    connect(m_decreaseButton, &QPushButton::clicked, this, &MainWindow::onDecreaseClicked);
    connect(m_decrease5Button, &QPushButton::clicked, this, &MainWindow::onDecrease5Clicked);
    connect(m_increase5Button, &QPushButton::clicked, this, &MainWindow::onIncrease5Clicked);
    connect(m_increaseButton, &QPushButton::clicked, this, &MainWindow::onIncreaseClicked);
    
    updateUIForState(false);
}

void MainWindow::onStartButtonClicked()
{
    TimerState currentState = m_timerEngine->state();
    if (currentState == TimerState::Idle || currentState == TimerState::Completed) {
        int minutes = m_minutesSpinBox->value();
        m_timerEngine->start(minutes);
        m_backgroundWidget->startNewRun();
        updateUIForState(true);
    } else {
        m_timerEngine->stop();
        updateUIForState(false);
    }
}

void MainWindow::onIncreaseClicked()
{
    if (m_timerEngine->state() == TimerState::Idle) {
        m_minutesSpinBox->stepUp();
    }
}

void MainWindow::onIncrease5Clicked()
{
    if (m_timerEngine->state() == TimerState::Idle) {
        int newValue = m_minutesSpinBox->value() + 5;
        m_minutesSpinBox->setValue(qMin(newValue, m_minutesSpinBox->maximum()));
    }
}

void MainWindow::onDecreaseClicked()
{
    if (m_timerEngine->state() == TimerState::Idle) {
        m_minutesSpinBox->stepDown();
    }
}

void MainWindow::onDecrease5Clicked()
{
    if (m_timerEngine->state() == TimerState::Idle) {
        int newValue = m_minutesSpinBox->value() - 5;
        m_minutesSpinBox->setValue(qMax(newValue, m_minutesSpinBox->minimum()));
    }
}

void MainWindow::onTimerCompleted()
{
    updateUIForState(false);
}

void MainWindow::onProgressChanged(double progress)
{
    m_backgroundWidget->setProgress(progress);
}

void MainWindow::onTimeRemainingChanged(int seconds)
{
    // Could display remaining time here if needed
    Q_UNUSED(seconds);
}

void MainWindow::updateUIForState(bool isRunning)
{
    m_minutesSpinBox->setEnabled(!isRunning);
    m_decreaseButton->setEnabled(!isRunning);
    m_decrease5Button->setEnabled(!isRunning);
    m_increase5Button->setEnabled(!isRunning);
    m_increaseButton->setEnabled(!isRunning);
    
    if (isRunning) {
        m_startButton->setText("Stop");
    } else {
        m_startButton->setText("Start");
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    if (m_backgroundWidget) {
        m_backgroundWidget->setGeometry(0, 0, width(), height());
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Save window position
    QSettings settings;
    settings.setValue("windowPosition", pos());
    
    QMainWindow::closeEvent(event);
}
