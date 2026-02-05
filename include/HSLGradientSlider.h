#ifndef GRADIENTSLIDER_H
#define GRADIENTSLIDER_H

#include <QSlider>
#include <QColor>

class HSLGradientSlider : public QSlider {
    Q_OBJECT

public:
    enum GradientType {
        Hue,
        Saturation,
        Lightness
    };

    explicit HSLGradientSlider(Qt::Orientation orientation, GradientType type, QWidget *parent = nullptr);

    void setGradientType(GradientType type);
    void setBaseColor(const QColor &color); // For saturation and lightness gradients

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    GradientType m_gradientType;
    QColor m_baseColor;

    QLinearGradient createHueGradient(const QRect &rect) const;
    QLinearGradient createSaturationGradient(const QRect &rect) const;
    QLinearGradient createLightnessGradient(const QRect &rect) const;
};

#endif // GRADIENTSLIDER_H
