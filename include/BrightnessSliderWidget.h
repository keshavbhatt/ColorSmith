#ifndef COLORPICKER_BRIGHTNESSSLIDERWIDGET_H
#define COLORPICKER_BRIGHTNESSSLIDERWIDGET_H

#include <QWidget>
#include <QColor>

class BrightnessSliderWidget : public QWidget {
  Q_OBJECT

public:
    explicit BrightnessSliderWidget(QWidget *parent = nullptr);

    void setColor(const QColor &color);
    int value() const { return m_value; }

    signals:
        void valueChanged(int value);
    void colorChanged(const QColor &color);

  protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    QSize sizeHint() const override;

  private:
    void updateFromPosition(int y);

  private:
    QColor m_color;
    int m_hue;
    int m_saturation;
    int m_value;
    bool m_dragging;
};

#endif //COLORPICKER_BRIGHTNESSSLIDERWIDGET_H