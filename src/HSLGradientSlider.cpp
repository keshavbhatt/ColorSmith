#include "../include/HSLGradientSlider.h"
#include <QPainter>
#include <QStyleOptionSlider>
#include <QLinearGradient>

HSLGradientSlider::HSLGradientSlider(Qt::Orientation orientation, GradientType type, QWidget *parent)
    : QSlider(orientation, parent), m_gradientType(type), m_baseColor(Qt::red) {
    setMinimumHeight(25);
}

void HSLGradientSlider::setGradientType(GradientType type) {
    m_gradientType = type;
    update();
}

void HSLGradientSlider::setBaseColor(const QColor &color) {
    m_baseColor = color;
    update();
}

void HSLGradientSlider::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Get the groove rect
    QStyleOptionSlider opt;
    initStyleOption(&opt);
    QRect grooveRect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderGroove, this);

    // Adjust groove rect for better appearance
    grooveRect.adjust(2, 5, -2, -5);

    // Draw gradient background
    QLinearGradient gradient;
    switch (m_gradientType) {
        case Hue:
            gradient = createHueGradient(grooveRect);
            break;
        case Saturation:
            gradient = createSaturationGradient(grooveRect);
            break;
        case Lightness:
            gradient = createLightnessGradient(grooveRect);
            break;
    }

    // Draw the gradient
    painter.setPen(Qt::NoPen);
    painter.setBrush(gradient);
    painter.drawRoundedRect(grooveRect, 3, 3);

    // Draw border
    painter.setPen(QPen(palette().dark().color(), 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(grooveRect, 3, 3);

    // Draw the handle
    QRect handleRect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);

    // Custom handle drawing
    painter.setPen(QPen(Qt::black, 2));
    painter.setBrush(Qt::white);

    if (orientation() == Qt::Horizontal) {
        // Draw a vertical line as handle
        int centerX = handleRect.center().x();
        painter.drawLine(centerX, grooveRect.top(), centerX, grooveRect.bottom());

        // Draw circle at the center
        painter.drawEllipse(QPoint(centerX, grooveRect.center().y()), 4, 4);
    } else {
        // Draw a horizontal line as handle
        int centerY = handleRect.center().y();
        painter.drawLine(grooveRect.left(), centerY, grooveRect.right(), centerY);

        // Draw circle at the center
        painter.drawEllipse(QPoint(grooveRect.center().x(), centerY), 4, 4);
    }
}

QLinearGradient HSLGradientSlider::createHueGradient(const QRect &rect) const {
    QLinearGradient gradient;

    if (orientation() == Qt::Horizontal) {
        gradient = QLinearGradient(rect.left(), 0, rect.right(), 0);
    } else {
        gradient = QLinearGradient(0, rect.top(), 0, rect.bottom());
    }

    // Add color stops for full hue spectrum
    gradient.setColorAt(0.0, QColor::fromHsv(0, 255, 255));     // Red
    gradient.setColorAt(0.17, QColor::fromHsv(60, 255, 255));   // Yellow
    gradient.setColorAt(0.33, QColor::fromHsv(120, 255, 255));  // Green
    gradient.setColorAt(0.5, QColor::fromHsv(180, 255, 255));   // Cyan
    gradient.setColorAt(0.67, QColor::fromHsv(240, 255, 255));  // Blue
    gradient.setColorAt(0.83, QColor::fromHsv(300, 255, 255));  // Magenta
    gradient.setColorAt(1.0, QColor::fromHsv(359, 255, 255));   // Red

    return gradient;
}

QLinearGradient HSLGradientSlider::createSaturationGradient(const QRect &rect) const {
    QLinearGradient gradient;

    if (orientation() == Qt::Horizontal) {
        gradient = QLinearGradient(rect.left(), 0, rect.right(), 0);
    } else {
        gradient = QLinearGradient(0, rect.top(), 0, rect.bottom());
    }

    // Get current hue and lightness from base color
    int h, s, l;
    m_baseColor.getHsl(&h, &s, &l);

    // From gray (no saturation) to full color (full saturation)
    gradient.setColorAt(0.0, QColor::fromHsl(h, 0, l));
    gradient.setColorAt(1.0, QColor::fromHsl(h, 255, l));

    return gradient;
}

QLinearGradient HSLGradientSlider::createLightnessGradient(const QRect &rect) const {
    QLinearGradient gradient;

    if (orientation() == Qt::Horizontal) {
        gradient = QLinearGradient(rect.left(), 0, rect.right(), 0);
    } else {
        gradient = QLinearGradient(0, rect.top(), 0, rect.bottom());
    }

    // Get current hue and saturation from base color
    int h, s, l;
    m_baseColor.getHsl(&h, &s, &l);

    // From black (0% lightness) to white (100% lightness)
    gradient.setColorAt(0.0, QColor::fromHsl(h, s, 0));    // Black
    gradient.setColorAt(0.5, QColor::fromHsl(h, s, 127));  // Mid tone
    gradient.setColorAt(1.0, QColor::fromHsl(h, s, 255));  // White

    return gradient;
}
