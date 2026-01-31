#include "../include/ColorPreviewWidget.h"
#include <QPainter>
#include <QPaintEvent>

ColorPreviewWidget::ColorPreviewWidget(QWidget* parent)
    : QFrame(parent), m_color(Qt::white)
{
    setFrameShape(StyledPanel);
    setFrameShadow(Raised);
    setAutoFillBackground(false); // We'll handle painting ourselves
}

void ColorPreviewWidget::setColor(const QColor& color) {
    m_color = color;
    update();
}

void ColorPreviewWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect contentRect = contentsRect();

    // Draw checkered background for transparency
    painter.fillRect(contentRect, Qt::white);
    int checkerSize = 10;
    for (int y = contentRect.top(); y < contentRect.bottom(); y += checkerSize) {
        for (int x = contentRect.left(); x < contentRect.right(); x += checkerSize) {
            if ((x / checkerSize + y / checkerSize) % 2 == 0) {
                painter.fillRect(x, y, checkerSize, checkerSize, QColor(204, 204, 204));
            }
        }
    }

    // Draw the actual color on top
    painter.fillRect(contentRect, m_color);

    // Draw the frame
    QFrame::paintEvent(event);
}
