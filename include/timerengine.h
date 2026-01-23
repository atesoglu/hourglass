#pragma once

#include <QObject>
#include <QTimer>

enum class TimerState {
    Idle,
    Running,
    Paused,
    Completed
};

class TimerEngine : public QObject {
    Q_OBJECT

public:
    explicit TimerEngine(QObject *parent = nullptr);
    
    void start(int minutes);
    void stop();
    void pause();
    void resume();
    
    TimerState state() const { return m_state; }
    int remainingSeconds() const { return m_remainingSeconds; }
    double progress() const;

signals:
    void timeRemainingChanged(int seconds);
    void progressChanged(double progress);
    void timerCompleted();
    void stateChanged(TimerState state);

private slots:
    void onTimerTimeout();

private:
    void setState(TimerState newState);
    void updateProgress();

    QTimer m_timer;
    TimerState m_state;
    int m_totalSeconds;
    int m_remainingSeconds;
};
