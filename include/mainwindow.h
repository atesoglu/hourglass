#pragma once

#include <QMainWindow>
#include <QSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QResizeEvent>
#include <QCloseEvent>
#include <QPoint>

class TimerEngine;
class AnimatedBackgroundWidget;
class SoundPlayer;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onStartButtonClicked();
    void onIncreaseClicked();
    void onIncrease5Clicked();
    void onDecreaseClicked();
    void onDecrease5Clicked();
    void onTimerCompleted();
    void onProgressChanged(double progress);
    void onTimeRemainingChanged(int seconds);

private:
    void setupUI();
    void setupWindowFlags();
    void updateUIForState(bool isRunning);

    QWidget *m_centralWidget;
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_controlLayout;
    
    QPushButton *m_decreaseButton;
    QPushButton *m_decrease5Button;
    QSpinBox *m_minutesSpinBox;
    QPushButton *m_increase5Button;
    QPushButton *m_increaseButton;
    QPushButton *m_startButton;
    
    AnimatedBackgroundWidget *m_backgroundWidget;
    TimerEngine *m_timerEngine;
    SoundPlayer *m_soundPlayer;
};
