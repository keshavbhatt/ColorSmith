#include "../include/ColorPreviewWidget.h"
#include <QPainter>
#include <QPaintEvent>
#include <QApplication>
#include <QScreen>

ColorPreviewWidget::ColorPreviewWidget(QWidget* parent)
    : QFrame(parent), m_color(Qt::white), m_previewPopup(nullptr)
{
    setFrameShape(StyledPanel);
    setFrameShadow(Raised);
    setAutoFillBackground(false); // We'll handle painting ourselves
    setMouseTracking(true); // Enable mouse tracking for hover events

    // Create hover timer with delay
    m_hoverTimer = new QTimer(this);
    m_hoverTimer->setSingleShot(true);
    m_hoverTimer->setInterval(500); // 500ms delay
    connect(m_hoverTimer, &QTimer::timeout, this, &ColorPreviewWidget::showColorPreview);
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

void ColorPreviewWidget::enterEvent(QEnterEvent* event) {
    QFrame::enterEvent(event);
    // Start the timer - preview will show after delay
    m_hoverTimer->start();
}

void ColorPreviewWidget::leaveEvent(QEvent* event) {
    QFrame::leaveEvent(event);
    // Stop the timer if still waiting
    m_hoverTimer->stop();
    // Hide the preview if already shown
    hideColorPreview();
}

void ColorPreviewWidget::showColorPreview() {
    if (m_previewPopup) {
        m_previewPopup->deleteLater();
        m_previewPopup = nullptr;
    }

    // Create popup widget
    m_previewPopup = new QWidget(nullptr, Qt::ToolTip | Qt::FramelessWindowHint);
    m_previewPopup->setAttribute(Qt::WA_TranslucentBackground, false);
    m_previewPopup->setAttribute(Qt::WA_DeleteOnClose);
    m_previewPopup->setFixedSize(200, 200);

    // Create a frame for the preview
    QFrame* previewFrame = new QFrame(m_previewPopup);
    previewFrame->setFrameShape(QFrame::StyledPanel);
    previewFrame->setFrameShadow(QFrame::Raised);
    previewFrame->setLineWidth(2);
    previewFrame->setGeometry(0, 0, 200, 200);

    // Custom paint for the preview frame
    class PreviewFrame : public QFrame {
    public:
        QColor color;
        PreviewFrame(QWidget* parent) : QFrame(parent) {}

    protected:
        void paintEvent(QPaintEvent* event) override {
            QPainter painter(this);
            painter.setRenderHint(QPainter::Antialiasing);

            QRect contentRect = contentsRect();

            // Draw checkered background for transparency
            painter.fillRect(contentRect, Qt::white);
            int checkerSize = 15;
            for (int y = contentRect.top(); y < contentRect.bottom(); y += checkerSize) {
                for (int x = contentRect.left(); x < contentRect.right(); x += checkerSize) {
                    if ((x / checkerSize + y / checkerSize) % 2 == 0) {
                        painter.fillRect(x, y, checkerSize, checkerSize, QColor(204, 204, 204));
                    }
                }
            }

            // Draw the actual color on top
            painter.fillRect(contentRect, color);

            // Draw the frame
            QFrame::paintEvent(event);
        }
    };

    PreviewFrame* colorFrame = new PreviewFrame(m_previewPopup);
    colorFrame->color = m_color;
    colorFrame->setFrameShape(QFrame::StyledPanel);
    colorFrame->setFrameShadow(QFrame::Raised);
    colorFrame->setLineWidth(2);
    colorFrame->setGeometry(0, 0, 200, 200);

    // Position the popup near the cursor
    QPoint globalPos = mapToGlobal(rect().center());

    // Adjust position to show to the right and slightly below
    globalPos.setX(globalPos.x() + width() / 2 + 10);
    globalPos.setY(globalPos.y() - 100);

    // Make sure it stays on screen
    if (QScreen* screen = QApplication::screenAt(globalPos)) {
        QRect screenGeometry = screen->availableGeometry();

        // Adjust if goes off right edge
        if (globalPos.x() + 200 > screenGeometry.right()) {
            globalPos.setX(mapToGlobal(rect().center()).x() - width() / 2 - 210);
        }

        // Adjust if goes off bottom
        if (globalPos.y() + 200 > screenGeometry.bottom()) {
            globalPos.setY(screenGeometry.bottom() - 200);
        }

        // Adjust if goes off top
        if (globalPos.y() < screenGeometry.top()) {
            globalPos.setY(screenGeometry.top());
        }
    }

    m_previewPopup->move(globalPos);
    m_previewPopup->show();
}

void ColorPreviewWidget::hideColorPreview() {
    if (m_previewPopup) {
        m_previewPopup->deleteLater();
        m_previewPopup = nullptr;
    }
}

