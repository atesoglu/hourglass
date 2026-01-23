#include "timerengine.h"
#include <QDebug>

TimerEngine::TimerEngine(QObject *parent)
    : QObject(parent)
    , m_state(TimerState::Idle)
    , m_totalSeconds(0)
    , m_remainingSeconds(0)
{
    m_timer.setSingleShot(false);
    m_timer.setInterval(1000); // 1 second intervals
    connect(&m_timer, &QTimer::timeout, this, &TimerEngine::onTimerTimeout);
}

void TimerEngine::start(int minutes)
{
    if (m_state == TimerState::Idle || m_state == TimerState::Completed) {
        m_totalSeconds = minutes * 60;
        m_remainingSeconds = m_totalSeconds;
        setState(TimerState::Running);
        updateProgress();
        m_timer.start();
    } else if (m_state == TimerState::Paused) {
        setState(TimerState::Running);
        m_timer.start();
    }
}

void TimerEngine::stop()
{
    m_timer.stop();
    setState(TimerState::Idle);
    m_remainingSeconds = 0;
    m_totalSeconds = 0;
    updateProgress();
}

void TimerEngine::pause()
{
    if (m_state == TimerState::Running) {
        m_timer.stop();
        setState(TimerState::Paused);
    }
}

void TimerEngine::resume()
{
    if (m_state == TimerState::Paused) {
        setState(TimerState::Running);
        m_timer.start();
    }
}

double TimerEngine::progress() const
{
    if (m_totalSeconds == 0) {
        return 0.0;
    }
    return 1.0 - (static_cast<double>(m_remainingSeconds) / static_cast<double>(m_totalSeconds));
}

void TimerEngine::onTimerTimeout()
{
    if (m_remainingSeconds > 0) {
        m_remainingSeconds--;
        emit timeRemainingChanged(m_remainingSeconds);
        updateProgress();
        
        if (m_remainingSeconds == 0) {
            m_timer.stop();
            setState(TimerState::Completed);
            emit timerCompleted();
        }
    }
}

void TimerEngine::setState(TimerState newState)
{
    if (m_state != newState) {
        m_state = newState;
        emit stateChanged(m_state);
    }
}

void TimerEngine::updateProgress()
{
    emit progressChanged(progress());
}
