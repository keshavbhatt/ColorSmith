#ifndef COLORPREVIEWWIDGET_H
#define COLORPREVIEWWIDGET_H

#include <QFrame>
#include <QColor>
#include <QTimer>

class ColorPreviewWidget : public QFrame {
    Q_OBJECT

public:
    explicit ColorPreviewWidget(QWidget* parent = nullptr);

    void setColor(const QColor& color);
    QColor color() const { return m_color; }

protected:
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

private slots:
    void showColorPreview();

private:
    void hideColorPreview();

    QColor m_color;
    QWidget* m_previewPopup;
    QTimer* m_hoverTimer;
};

#endif // COLORPREVIEWWIDGET_H
