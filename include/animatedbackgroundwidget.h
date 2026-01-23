#pragma once

#include <QWidget>
#include <QColor>
#include <QPaintEvent>

class AnimatedBackgroundWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY(double progress READ progress WRITE setProgress)

public:
    explicit AnimatedBackgroundWidget(QWidget *parent = nullptr);
    
    double progress() const { return m_progress; }
    void setProgress(double progress);
    void startNewRun();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void generateRandomColor();

    double m_progress;
    QColor m_fillColor;
};
