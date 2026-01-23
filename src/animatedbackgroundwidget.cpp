#include "animatedbackgroundwidget.h"
#include <QPainter>
#include <QRandomGenerator>
#include <QPaintEvent>

AnimatedBackgroundWidget::AnimatedBackgroundWidget(QWidget *parent)
    : QWidget(parent)
    , m_progress(0.0)
{
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    generateRandomColor();
}

void AnimatedBackgroundWidget::setProgress(double progress)
{
    m_progress = qBound(0.0, progress, 1.0);
    update();
}

void AnimatedBackgroundWidget::startNewRun()
{
    m_progress = 0.0;
    generateRandomColor();
    update();
}

void AnimatedBackgroundWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Fill from top to bottom
    int fillHeight = static_cast<int>(height() * m_progress);
    
    if (fillHeight > 0) {
        QRect fillRect(0, 0, width(), fillHeight);
        painter.fillRect(fillRect, m_fillColor);
    }
}

void AnimatedBackgroundWidget::generateRandomColor()
{
    // Generate a random hue (0-359), with fixed saturation and brightness for modern colors
    int hue = QRandomGenerator::global()->bounded(360);
    int saturation = 150 + QRandomGenerator::global()->bounded(106); // 150-255
    int brightness = 200 + QRandomGenerator::global()->bounded(56);  // 200-255
    
    m_fillColor = QColor::fromHsv(hue, saturation, brightness);
}
