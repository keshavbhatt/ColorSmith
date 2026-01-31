#ifndef COLORPREVIEWWIDGET_H
#define COLORPREVIEWWIDGET_H

#include <QFrame>
#include <QColor>

class ColorPreviewWidget : public QFrame {
    Q_OBJECT

public:
    explicit ColorPreviewWidget(QWidget* parent = nullptr);

    void setColor(const QColor& color);
    QColor color() const { return m_color; }

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QColor m_color;
};

#endif // COLORPREVIEWWIDGET_H
