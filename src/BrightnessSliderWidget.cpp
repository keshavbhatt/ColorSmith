#include "../include/BrightnessSliderWidget.h"
#include <QPainter>
#include <QMouseEvent>

BrightnessSliderWidget::BrightnessSliderWidget(QWidget *parent)
    : QWidget(parent),
      m_hue(0),
      m_saturation(255),
      m_value(255),
      m_dragging(false)
{
    setMinimumWidth(12);
    setMouseTracking(true);
}

QSize BrightnessSliderWidget::sizeHint() const {
    return QSize(12, 200);
}

void BrightnessSliderWidget::setColor(const QColor &color) {
    m_color = color;

    int h, s, v;
    color.getHsv(&h, &s, &v);

    m_hue = (h < 0) ? 0 : h;
    m_saturation = s;
    m_value = v;

    update();
}

void BrightnessSliderWidget::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, false);

    // Draw brightness gradient
    QLinearGradient grad(0, 0, 0, height());
    grad.setColorAt(0.0, QColor::fromHsv(m_hue, m_saturation, 255));
    grad.setColorAt(1.0, QColor::fromHsv(m_hue, m_saturation, 0));

    p.fillRect(rect(), grad);



    // Draw indicator - triangle pointing to the current brightness level
    int y = (1.0 - (m_value / 255.0)) * height();

    p.setRenderHint(QPainter::Antialiasing, true);

    // Draw triangle pointing left
    int barWidth = width();
    QPolygon triangle;
    triangle << QPoint(barWidth - 8, y)       // Point to the left
             << QPoint(barWidth, y - 5)       // Top right
             << QPoint(barWidth, y + 5);      // Bottom right

    p.setBrush(Qt::white);
    p.setPen(QPen(Qt::black, 1));
    p.drawPolygon(triangle);

    p.setRenderHint(QPainter::Antialiasing, false);
}

void BrightnessSliderWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        updateFromPosition(event->pos().y());
    }
}

void BrightnessSliderWidget::mouseMoveEvent(QMouseEvent *event) {
    if (m_dragging) {
        updateFromPosition(event->pos().y());
    }
}

void BrightnessSliderWidget::mouseReleaseEvent(QMouseEvent *) {
    m_dragging = false;
}

void BrightnessSliderWidget::updateFromPosition(int y) {
    y = qBound(0, y, height());

    m_value = 255 - (y * 255 / height());

    m_color.setHsv(m_hue, m_saturation, m_value);

    emit valueChanged(m_value);
    emit colorChanged(m_color);

    update();
}