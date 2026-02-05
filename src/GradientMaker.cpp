#include "../include/GradientMaker.h"
#include "../include/ScreenPicker.h"
#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QComboBox>
#include <QCursor>
#include <QFileDialog>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QPushButton>
#include <QRandomGenerator>
#include <QResizeEvent>
#include <QSettings>
#include <QSpinBox>
#include <QTextStream>
#include <QTimer>
#include <QToolTip>
#include <QtMath>

// GradientSlider implementation
GradientSlider::GradientSlider(QWidget *parent)
    : QWidget(parent), m_selectedStop(-1), m_draggedStop(-1),
      m_isDragging(false) {
  setMinimumHeight(50);
  setMaximumHeight(50);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  setMouseTracking(true);

  // Initialize with default gradient
  m_stops.append(GradientStop(QColor("#07C8F9"), 0.0));
  m_stops.append(GradientStop(QColor("#0D41E1"), 1.0));
}

void GradientSlider::setStops(const QVector<GradientStop> &stops) {
  m_stops = stops;
  update();
}

void GradientSlider::setSelectedStop(int index) {
  if (index >= -1 && index < m_stops.size()) {
    m_selectedStop = index;
    update();
  }
}

void GradientSlider::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  const int trackHeight = 20;
  const int trackY = (height() - trackHeight) / 2;
  const int margin = 15;

  // Draw track with gradient
  QRect trackRect(margin, trackY, width() - 2 * margin, trackHeight);

  // Draw checkered background for transparency
  painter.fillRect(trackRect, Qt::white);
  int checkerSize = 5;
  for (int y = trackRect.top(); y < trackRect.bottom(); y += checkerSize) {
    for (int x = trackRect.left(); x < trackRect.right(); x += checkerSize) {
      if ((x / checkerSize + y / checkerSize) % 2 == 0) {
        painter.fillRect(x, y, checkerSize, checkerSize, QColor(204, 204, 204));
      }
    }
  }

  // Create gradient for track
  QLinearGradient gradient(trackRect.left(), 0, trackRect.right(), 0);
  for (const GradientStop &stop : m_stops) {
    gradient.setColorAt(stop.position, stop.color);
  }

  painter.fillRect(trackRect, gradient);
  painter.setPen(QPen(QColor(180, 180, 180), 1));
  painter.drawRect(trackRect);

  // Draw stop handles
  for (int i = 0; i < m_stops.size(); ++i) {
    QRect stopRect = getStopRect(i);

    // Draw handle
    bool isSelected = (i == m_selectedStop);
    painter.setPen(
        QPen(isSelected ? QColor(0, 120, 215) : QColor(100, 100, 100),
             isSelected ? 2 : 1));
    painter.setBrush(m_stops[i].color);
    painter.drawRoundedRect(stopRect, 3, 3);

    // Draw inner border
    painter.setPen(QPen(QColor(255, 255, 255, 150), 1));
    painter.drawRoundedRect(stopRect.adjusted(1, 1, -1, -1), 2, 2);
  }
}

void GradientSlider::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    int stopIndex = getStopAtPosition(event->pos());

    if (stopIndex >= 0) {
      m_selectedStop = stopIndex;
      m_draggedStop = stopIndex;
      m_isDragging = true;
      emit stopSelected(stopIndex);
      update();
    }
  } else if (event->button() == Qt::RightButton) {
    int stopIndex = getStopAtPosition(event->pos());
    if (stopIndex >= 0 && m_stops.size() > 2) {
      m_stops.removeAt(stopIndex);
      if (m_selectedStop >= m_stops.size()) {
        m_selectedStop = m_stops.size() - 1;
      }
      emit stopRemoved(stopIndex);
      update();
    }
  }
}

void GradientSlider::mouseMoveEvent(QMouseEvent *event) {
  if (m_isDragging && m_draggedStop >= 0) {
    const int margin = 15;
    int trackWidth = width() - 2 * margin;
    double newPos =
        qBound(0.0, (event->pos().x() - margin) / (double)trackWidth, 1.0);

    m_stops[m_draggedStop].position = newPos;
    emit stopMoved(m_draggedStop, newPos);
    update();
  }
}

void GradientSlider::mouseReleaseEvent(QMouseEvent *event) {
  Q_UNUSED(event);
  m_isDragging = false;
  m_draggedStop = -1;
}

void GradientSlider::mouseDoubleClickEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    const int margin = 15;
    int trackWidth = width() - 2 * margin;
    const int trackHeight = 20;
    const int trackY = (height() - trackHeight) / 2;

    QRect trackRect(margin, trackY, trackWidth, trackHeight);

    if (trackRect.contains(event->pos())) {
      double position = (event->pos().x() - margin) / (double)trackWidth;
      position = qBound(0.0, position, 1.0);

      // Interpolate color at position
      QColor newColor = Qt::white;
      for (int i = 0; i < m_stops.size() - 1; ++i) {
        if (position >= m_stops[i].position &&
            position <= m_stops[i + 1].position) {
          double t = (position - m_stops[i].position) /
                     (m_stops[i + 1].position - m_stops[i].position);
          int r = m_stops[i].color.red() +
                  t * (m_stops[i + 1].color.red() - m_stops[i].color.red());
          int g = m_stops[i].color.green() +
                  t * (m_stops[i + 1].color.green() - m_stops[i].color.green());
          int b = m_stops[i].color.blue() +
                  t * (m_stops[i + 1].color.blue() - m_stops[i].color.blue());
          int a = m_stops[i].color.alpha() +
                  t * (m_stops[i + 1].color.alpha() - m_stops[i].color.alpha());
          newColor = QColor(r, g, b, a);
          break;
        }
      }

      m_stops.append(GradientStop(newColor, position));

      // Sort stops by position
      std::sort(m_stops.begin(), m_stops.end(),
                [](const GradientStop &a, const GradientStop &b) {
                  return a.position < b.position;
                });

      // Find new stop index
      int newIndex = 0;
      for (int i = 0; i < m_stops.size(); ++i) {
        if (qAbs(m_stops[i].position - position) < 0.001) {
          newIndex = i;
          break;
        }
      }

      m_selectedStop = newIndex;
      emit stopAdded(newIndex, position);
      update();
    }
  }
}

int GradientSlider::getStopAtPosition(const QPoint &pos) const {
  for (int i = 0; i < m_stops.size(); ++i) {
    if (getStopRect(i).contains(pos)) {
      return i;
    }
  }
  return -1;
}

QRect GradientSlider::getStopRect(int index) const {
  if (index < 0 || index >= m_stops.size()) {
    return QRect();
  }

  const int margin = 15;
  int trackWidth = width() - 2 * margin;
  int x = margin + m_stops[index].position * trackWidth;

  const int handleWidth = 12;
  const int handleHeight = 20;
  const int trackHeight = 20;
  const int trackY = (height() - trackHeight) / 2;

  return QRect(x - handleWidth / 2, trackY + trackHeight - 5, handleWidth,
               handleHeight);
}

// GradientPreviewWidget implementation
GradientPreviewWidget::GradientPreviewWidget(QWidget *parent)
    : QFrame(parent), m_rotation(0), m_isRadial(false) {
  setMinimumHeight(200);
  setAutoFillBackground(false);

  // Create fullscreen button
  m_fullscreenBtn = new QPushButton(this);
  m_fullscreenBtn->setText("⛶");
  m_fullscreenBtn->setToolTip(tr("Fullscreen Preview"));
  m_fullscreenBtn->setFixedSize(32, 32);
  m_fullscreenBtn->setStyleSheet("QPushButton {"
                                 "  background-color: rgba(0, 0, 0, 100);"
                                 "  color: white;"
                                 "  border: 0;"
                                 "  border-radius: 4px;"
                                 "  font-size: 16px;"
                                 "}"
                                 "QPushButton:hover {"
                                 "  background-color: rgba(0, 0, 0, 150);"
                                 "  border: 0;"
                                 "}"
                                 "QPushButton:pressed {"
                                 "  background-color: rgba(0, 0, 0, 200);"
                                 "}");
  connect(m_fullscreenBtn, &QPushButton::clicked, this,
          &GradientPreviewWidget::onFullscreenClicked);
}

void GradientPreviewWidget::setGradient(const QVector<GradientStop> &stops,
                                        int rotation, bool isRadial) {
  m_stops = stops;
  m_rotation = rotation;
  m_isRadial = isRadial;
  update();
}

void GradientPreviewWidget::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  if (m_stops.isEmpty()) {
    painter.fillRect(rect(), Qt::white);
    return;
  }

  if (m_isRadial) {
    // Create radial gradient
    QRadialGradient gradient(width() / 2.0, height() / 2.0,
                             qMin(width(), height()) / 2.0);
    for (const GradientStop &stop : m_stops) {
      gradient.setColorAt(stop.position, stop.color);
    }
    painter.fillRect(rect(), gradient);
  } else {
    // Create linear gradient with rotation
    double radians = qDegreesToRadians((double)m_rotation);

    // Calculate gradient line based on rotation
    double centerX = width() / 2.0;
    double centerY = height() / 2.0;
    double length = qSqrt(width() * width() + height() * height()) / 2.0;

    double x1 = centerX - length * qCos(radians);
    double y1 = centerY - length * qSin(radians);
    double x2 = centerX + length * qCos(radians);
    double y2 = centerY + length * qSin(radians);

    QLinearGradient gradient(x1, y1, x2, y2);
    for (const GradientStop &stop : m_stops) {
      gradient.setColorAt(stop.position, stop.color);
    }
    painter.fillRect(rect(), gradient);
  }
}

void GradientPreviewWidget::resizeEvent(QResizeEvent *event) {
  QFrame::resizeEvent(event);

  // Position fullscreen button at bottom right with 10px margin
  if (m_fullscreenBtn) {
    m_fullscreenBtn->move(width() - m_fullscreenBtn->width() - 10,
                          height() - m_fullscreenBtn->height() - 10);
  }
}

void GradientPreviewWidget::onFullscreenClicked() {
  // Create fullscreen window
  QWidget *window = new QWidget(this, Qt::Window);
  window->setWindowTitle(tr("Gradient Preview"));
  window->setAttribute(Qt::WA_DeleteOnClose, true);
  window->setStyleSheet("background-color: black;");

  // Setup layout
  QVBoxLayout *layout = new QVBoxLayout(window);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);

  // Create preview widget for fullscreen
  GradientPreviewWidget *preview = createFullscreenPreview(window);
  layout->addWidget(preview);

  // Setup close button
  setupFullscreenCloseButton(preview, window);

  window->showFullScreen();
}

GradientPreviewWidget *
GradientPreviewWidget::createFullscreenPreview(QWidget *parent) {
  GradientPreviewWidget *preview = new GradientPreviewWidget(parent);
  preview->setGradient(m_stops, m_rotation, m_isRadial);
  preview->setFrameShape(QFrame::NoFrame);

  // Remove the default fullscreen button from nested preview
  preview->m_fullscreenBtn->deleteLater();
  preview->m_fullscreenBtn = nullptr;

  return preview;
}

void GradientPreviewWidget::setupFullscreenCloseButton(
    GradientPreviewWidget *preview, QWidget *window) {
  QPushButton *closeBtn = new QPushButton("✕", preview);
  closeBtn->setToolTip(tr("Exit Fullscreen (ESC)"));
  closeBtn->setFixedSize(32, 32);
  closeBtn->setStyleSheet("QPushButton {"
                          "  background-color: rgba(0, 0, 0, 150);"
                          "  color: white;"
                          "  border: 0px;"
                          "  border-radius: 4px;"
                          "  font-size: 16px;"
                          "}"
                          "QPushButton:hover {"
                          "  background-color: rgba(200, 0, 0, 180);"
                          "}"
                          "QPushButton:pressed {"
                          "  background-color: rgba(150, 0, 0, 200);"
                          "}");

  // Connect close button
  connect(closeBtn, &QPushButton::clicked, window, &QWidget::close);

  // Position button after window is shown
  QTimer::singleShot(100, [closeBtn, preview]() {
    if (closeBtn && preview) {
      closeBtn->move(preview->width() - closeBtn->width() - 20,
                     preview->height() - closeBtn->height() - 20);
      closeBtn->raise();
    }
  });
}

// GradientMaker implementation
GradientMaker::GradientMaker(QWidget *parent)
  : QDialog(parent), m_gradientSlider(nullptr), m_rotation(45), m_type(Linear),
    m_screenPicker(new ScreenPicker(this)) {
  setWindowTitle(tr("Gradient Maker"));
  resize(700, 600);

  // Connect ScreenPicker signals
  connect(m_screenPicker, &ScreenPicker::colorPicked, this,
          &GradientMaker::onScreenColorPicked);
  connect(m_screenPicker, &ScreenPicker::errorOccurred, this,
          &GradientMaker::onPickerError);

  setupUI();

  // Initialize with default gradient
  m_stops.append(GradientStop(QColor("#07C8F9"), 0.06));
  m_stops.append(GradientStop(QColor("#0D41E1"), 0.77));
  m_gradientSlider->setStops(m_stops);

  updatePreview();
  updateControls();

  // Load saved geometry
  loadGeometry();
}

void GradientMaker::setupUI() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setSpacing(15);
  mainLayout->setContentsMargins(20, 20, 20, 20);

  // Title
  QLabel *titleLabel = new QLabel(tr("Gradient Maker"), this);
  QFont titleFont = titleLabel->font();
  titleFont.setPointSize(16);
  titleFont.setBold(true);
  titleLabel->setFont(titleFont);
  mainLayout->addWidget(titleLabel);

  setupGradientSlider(mainLayout);
  setupControls(mainLayout);
  setupButtons(mainLayout);
  setupPreview(mainLayout);
}

void GradientMaker::setupGradientSlider(QVBoxLayout *mainLayout) {
  QLabel *sliderLabel = new QLabel(
      tr("Gradient Stops (Double-click to add, Right-click to remove)"), this);
  mainLayout->addWidget(sliderLabel);

  m_gradientSlider = new GradientSlider(this);
  connect(m_gradientSlider, &GradientSlider::stopSelected, this,
          &GradientMaker::onStopSelected);
  connect(m_gradientSlider, &GradientSlider::stopMoved, this,
          &GradientMaker::onStopMoved);
  connect(m_gradientSlider, &GradientSlider::stopAdded, this,
          &GradientMaker::onStopAdded);
  connect(m_gradientSlider, &GradientSlider::stopRemoved, this,
          &GradientMaker::onStopRemoved);
  mainLayout->addWidget(m_gradientSlider);
}

void GradientMaker::setupControls(QVBoxLayout *mainLayout) {
  QGridLayout *controlsLayout = new QGridLayout();
  controlsLayout->setSpacing(10);
  int row = 0;

  // Color input
  QLabel *colorLabel = new QLabel(tr("Color:"), this);
  controlsLayout->addWidget(colorLabel, row, 0);

  QHBoxLayout *colorLayout = new QHBoxLayout();
  m_colorInput = new QLineEdit(this);
  m_colorInput->setPlaceholderText("#FFFFFF");
  connect(m_colorInput, &QLineEdit::textChanged, this,
          &GradientMaker::onColorChanged);
  colorLayout->addWidget(m_colorInput, 1);

  m_colorPickerBtn = new QPushButton(tr("Pick"), this);
  m_colorPickerBtn->setMaximumWidth(60);
  connect(m_colorPickerBtn, &QPushButton::clicked, this,
          &GradientMaker::onPickColor);
  colorLayout->addWidget(m_colorPickerBtn);

  m_removeColorBtn = new QPushButton(tr("Remove"), this);
  m_removeColorBtn->setMaximumWidth(80);
  connect(m_removeColorBtn, &QPushButton::clicked, this,
          &GradientMaker::onRemoveColorClicked);
  colorLayout->addWidget(m_removeColorBtn);

  controlsLayout->addLayout(colorLayout, row, 1);
  row++;

  // Position input
  QLabel *posLabel = new QLabel(tr("Position (%):"), this);
  controlsLayout->addWidget(posLabel, row, 0);

  m_positionInput = new QSpinBox(this);
  m_positionInput->setRange(0, 100);
  m_positionInput->setSuffix("%");
  connect(m_positionInput, QOverload<int>::of(&QSpinBox::valueChanged), this,
          &GradientMaker::onPositionChanged);
  controlsLayout->addWidget(m_positionInput, row, 1);
  row++;

  // Rotation input
  QLabel *rotLabel = new QLabel(tr("Rotation (°):"), this);
  controlsLayout->addWidget(rotLabel, row, 0);

  m_rotationInput = new QSpinBox(this);
  m_rotationInput->setRange(0, 360);
  m_rotationInput->setSuffix("°");
  m_rotationInput->setValue(45);
  connect(m_rotationInput, QOverload<int>::of(&QSpinBox::valueChanged), this,
          &GradientMaker::onRotationChanged);
  controlsLayout->addWidget(m_rotationInput, row, 1);
  row++;

  // Type selector
  QLabel *typeLabel = new QLabel(tr("Type:"), this);
  controlsLayout->addWidget(typeLabel, row, 0);

  m_typeCombo = new QComboBox(this);
  m_typeCombo->addItem(tr("Linear"), Linear);
  m_typeCombo->addItem(tr("Radial"), Radial);
  connect(m_typeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, &GradientMaker::onTypeChanged);
  controlsLayout->addWidget(m_typeCombo, row, 1);

  mainLayout->addLayout(controlsLayout);
}

void GradientMaker::setupButtons(QVBoxLayout *mainLayout) {
  QHBoxLayout *buttonsLayout = new QHBoxLayout();

  m_randomBtn = new QPushButton(tr("Random Gradient"), this);
  connect(m_randomBtn, &QPushButton::clicked, this,
          &GradientMaker::onRandomClicked);
  buttonsLayout->addWidget(m_randomBtn);

  m_exportBtn = new QPushButton(tr("Copy CSS"), this);
  connect(m_exportBtn, &QPushButton::clicked, this, &GradientMaker::onCopyCSS);
  buttonsLayout->addWidget(m_exportBtn);

  m_copySVGBtn = new QPushButton(tr("Copy SVG"), this);
  connect(m_copySVGBtn, &QPushButton::clicked, this,
          &GradientMaker::onCopySVG);
  buttonsLayout->addWidget(m_copySVGBtn);

  m_exportSVGBtn = new QPushButton(tr("Export SVG"), this);
  connect(m_exportSVGBtn, &QPushButton::clicked, this,
          &GradientMaker::onExportSVG);
  buttonsLayout->addWidget(m_exportSVGBtn);

  m_exportImageBtn = new QPushButton(tr("Export Image"), this);
  connect(m_exportImageBtn, &QPushButton::clicked, this,
          &GradientMaker::onExportImage);
  buttonsLayout->addWidget(m_exportImageBtn);

  mainLayout->addLayout(buttonsLayout);
}

void GradientMaker::setupPreview(QVBoxLayout *mainLayout) {
  QLabel *previewLabel = new QLabel(tr("Preview:"), this);
  mainLayout->addWidget(previewLabel);

  m_previewFrame = new GradientPreviewWidget(this);
  m_previewFrame->setFrameShape(QFrame::StyledPanel);
  m_previewFrame->setFrameShadow(QFrame::Sunken);
  m_previewFrame->setMinimumHeight(200);
  m_previewFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  mainLayout->addWidget(m_previewFrame, 1);
}

void GradientMaker::onStopSelected(int index) { updateControls(); }

void GradientMaker::onStopMoved(int index, double position) {
  if (index >= 0 && index < m_stops.size()) {
    m_stops[index].position = position;
    updatePreview();
    updateControls();
  }
}

void GradientMaker::onStopAdded(int index, double position) {
  m_stops = m_gradientSlider->stops();
  updatePreview();
  updateControls();
}

void GradientMaker::onStopRemoved(int index) {
  m_stops = m_gradientSlider->stops();
  updatePreview();
  updateControls();
}

void GradientMaker::onColorChanged() {
  int selectedIndex = m_gradientSlider->selectedStopIndex();
  if (selectedIndex >= 0 && selectedIndex < m_stops.size()) {
    QColor color(m_colorInput->text());
    if (color.isValid()) {
      m_stops[selectedIndex].color = color;
      m_gradientSlider->setStops(m_stops);
      updatePreview();
    }
  }
}

void GradientMaker::onPositionChanged(int value) {
  int selectedIndex = m_gradientSlider->selectedStopIndex();
  if (selectedIndex >= 0 && selectedIndex < m_stops.size()) {
    m_stops[selectedIndex].position = value / 100.0;
    m_gradientSlider->setStops(m_stops);
    updatePreview();
  }
}

void GradientMaker::onRotationChanged(int value) {
  m_rotation = value;
  updatePreview();
}

void GradientMaker::onTypeChanged(int index) {
  m_type = static_cast<GradientType>(m_typeCombo->itemData(index).toInt());
  m_rotationInput->setEnabled(m_type == Linear);
  updatePreview();
}

void GradientMaker::onRemoveColorClicked() {
  int selectedIndex = m_gradientSlider->selectedStopIndex();
  if (selectedIndex >= 0 && m_stops.size() > 2) {
    m_stops.removeAt(selectedIndex);
    m_gradientSlider->setStops(m_stops);
    m_gradientSlider->setSelectedStop(qMax(0, selectedIndex - 1));
    updatePreview();
    updateControls();
  }
}

void GradientMaker::onRandomClicked() { generateRandomGradient(); }

void GradientMaker::onCopyCSS() {
  QString css = generateGradientCSS();
  QClipboard *clipboard = QApplication::clipboard();
  clipboard->setText(css);

  // Show brief tooltip notification
  QToolTip::showText(QCursor::pos(), tr("CSS gradient copied!"), this, QRect(), 2000);
}

void GradientMaker::onCopySVG() {
  QString svg = generateSVGGradientDefinition();

  QClipboard *clipboard = QApplication::clipboard();
  clipboard->setText(svg);

  QToolTip::showText(QCursor::pos(), tr("SVG gradient code copied!"), this, QRect(), 2000);
}

QString GradientMaker::generateSVGGradientDefinition() const {
  QString svg;

  if (m_type == Linear) {
    double radians = qDegreesToRadians((double)m_rotation);

    double width = 100.0;
    double height = 100.0;

    double centerX = width / 2.0;
    double centerY = height / 2.0;

    double length = qSqrt(width * width + height * height) / 2.0;

    double x1 = centerX - length * qCos(radians);
    double y1 = centerY - length * qSin(radians);
    double x2 = centerX + length * qCos(radians);
    double y2 = centerY + length * qSin(radians);

    svg = QString("<linearGradient id=\"grad\" x1=\"%1%\" y1=\"%2%\" "
                  "x2=\"%3%\" y2=\"%4%\" "
                  "gradientUnits=\"objectBoundingBox\" "
                  "color-interpolation=\"sRGB\">\n")
              .arg(qRound(x1))
              .arg(qRound(y1))
              .arg(qRound(x2))
              .arg(qRound(y2));
  } else {
    svg = "<radialGradient id=\"grad\" cx=\"50%\" cy=\"50%\" r=\"50%\" "
          "gradientUnits=\"objectBoundingBox\" "
          "color-interpolation=\"sRGB\">\n";
  }

  for (const GradientStop &stop : m_stops) {
    svg += QString("  <stop offset=\"%1%\" stop-color=\"%2\" />\n")
               .arg(qRound(stop.position * 100))
               .arg(stop.color.name());
  }

  svg += m_type == Linear ? "</linearGradient>" : "</radialGradient>";

  return svg;
}

void GradientMaker::onExportSVG() {
  QString fileName = QFileDialog::getSaveFileName(
    this,
    tr("Export SVG Gradient"),
    QDir::homePath() + "/gradient.svg",
    tr("SVG Files (*.svg);;All Files (*)")
  );

  if (!fileName.isEmpty()) {
    QString svgContent = getGradientSVG();

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
      QTextStream out(&file);
      out << svgContent;
      file.close();

      QToolTip::showText(QCursor::pos(), tr("SVG file exported successfully!"), this, QRect(), 2000);
    } else {
      QToolTip::showText(QCursor::pos(), tr("Failed to save SVG file!"), this, QRect(), 3000);
    }
  }
}

void GradientMaker::onExportImage() {
  // Create dialog for size selection
  QDialog sizeDialog(this);
  sizeDialog.setWindowTitle(tr("Export Gradient as Image"));
  sizeDialog.setModal(true);

  QVBoxLayout *dialogLayout = new QVBoxLayout(&sizeDialog);
  dialogLayout->setSpacing(15);

  // Size preset selection
  QLabel *presetLabel = new QLabel(tr("Image Size:"), &sizeDialog);
  dialogLayout->addWidget(presetLabel);

  QComboBox *sizeCombo = new QComboBox(&sizeDialog);
  sizeCombo->addItem(tr("Instagram Post (1080x1080)"), QSize(1080, 1080));
  sizeCombo->addItem(tr("Instagram Story (1080x1920)"), QSize(1080, 1920));
  sizeCombo->addItem(tr("Twitter Header (1500x500)"), QSize(1500, 500));
  sizeCombo->addItem(tr("Facebook Cover (820x312)"), QSize(820, 312));
  sizeCombo->addItem(tr("YouTube Thumbnail (1280x720)"), QSize(1280, 720));
  sizeCombo->addItem(tr("Full HD (1920x1080)"), QSize(1920, 1080));
  sizeCombo->addItem(tr("4K UHD (3840x2160)"), QSize(3840, 2160));
  sizeCombo->addItem(tr("Square 512x512"), QSize(512, 512));
  sizeCombo->addItem(tr("Square 1024x1024"), QSize(1024, 1024));
  sizeCombo->addItem(tr("Square 2048x2048"), QSize(2048, 2048));
  sizeCombo->addItem(tr("Custom"), QSize(0, 0));
  dialogLayout->addWidget(sizeCombo);

  // Custom size inputs
  QWidget *customWidget = new QWidget(&sizeDialog);
  QHBoxLayout *customLayout = new QHBoxLayout(customWidget);
  customLayout->setContentsMargins(0, 0, 0, 0);

  QLabel *widthLabel = new QLabel(tr("Width:"), customWidget);
  customLayout->addWidget(widthLabel);

  QSpinBox *widthSpin = new QSpinBox(customWidget);
  widthSpin->setRange(1, 10000);
  widthSpin->setValue(1920);
  widthSpin->setSuffix(" px");
  customLayout->addWidget(widthSpin);

  QLabel *heightLabel = new QLabel(tr("Height:"), customWidget);
  customLayout->addWidget(heightLabel);

  QSpinBox *heightSpin = new QSpinBox(customWidget);
  heightSpin->setRange(1, 10000);
  heightSpin->setValue(1080);
  heightSpin->setSuffix(" px");
  customLayout->addWidget(heightSpin);

  customWidget->setEnabled(false);
  dialogLayout->addWidget(customWidget);

  // Enable/disable custom inputs based on combo selection
  connect(sizeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
          [customWidget, sizeCombo, widthSpin, heightSpin](int index) {
    bool isCustom = (sizeCombo->itemData(index).toSize() == QSize(0, 0));
    customWidget->setEnabled(isCustom);
    if (!isCustom) {
      QSize size = sizeCombo->itemData(index).toSize();
      widthSpin->setValue(size.width());
      heightSpin->setValue(size.height());
    }
  });

  // Buttons
  QHBoxLayout *buttonLayout = new QHBoxLayout();
  QPushButton *exportBtn = new QPushButton(tr("Export"), &sizeDialog);
  QPushButton *cancelBtn = new QPushButton(tr("Cancel"), &sizeDialog);

  connect(exportBtn, &QPushButton::clicked, &sizeDialog, &QDialog::accept);
  connect(cancelBtn, &QPushButton::clicked, &sizeDialog, &QDialog::reject);

  buttonLayout->addStretch();
  buttonLayout->addWidget(exportBtn);
  buttonLayout->addWidget(cancelBtn);
  dialogLayout->addLayout(buttonLayout);

  // Show dialog and export if accepted
  if (sizeDialog.exec() == QDialog::Accepted) {
    QSize exportSize;
    if (sizeCombo->itemData(sizeCombo->currentIndex()).toSize() == QSize(0, 0)) {
      // Custom size
      exportSize = QSize(widthSpin->value(), heightSpin->value());
    } else {
      // Preset size
      exportSize = sizeCombo->itemData(sizeCombo->currentIndex()).toSize();
    }

    // Get save file path
    QString fileName = QFileDialog::getSaveFileName(
      this,
      tr("Save Gradient Image"),
      QDir::homePath() + "/gradient.png",
      tr("PNG Images (*.png);;JPEG Images (*.jpg *.jpeg);;All Files (*)")
    );

    if (!fileName.isEmpty()) {
      // Create image with gradient
      QImage image(exportSize, QImage::Format_ARGB32);
      QPainter painter(&image);
      painter.setRenderHint(QPainter::Antialiasing);

      if (m_type == Radial) {
        // Create radial gradient
        QRadialGradient gradient(exportSize.width() / 2.0, exportSize.height() / 2.0,
                                qMin(exportSize.width(), exportSize.height()) / 2.0);
        for (const GradientStop &stop : m_stops) {
          gradient.setColorAt(stop.position, stop.color);
        }
        painter.fillRect(image.rect(), gradient);
      } else {
        // Create linear gradient with rotation
        double radians = qDegreesToRadians((double)m_rotation);
        double centerX = exportSize.width() / 2.0;
        double centerY = exportSize.height() / 2.0;
        double length = qSqrt(exportSize.width() * exportSize.width() +
                            exportSize.height() * exportSize.height()) / 2.0;

        double x1 = centerX - length * qCos(radians);
        double y1 = centerY - length * qSin(radians);
        double x2 = centerX + length * qCos(radians);
        double y2 = centerY + length * qSin(radians);

        QLinearGradient gradient(x1, y1, x2, y2);
        for (const GradientStop &stop : m_stops) {
          gradient.setColorAt(stop.position, stop.color);
        }
        painter.fillRect(image.rect(), gradient);
      }

      painter.end();

      // Save image
      if (image.save(fileName)) {
        QToolTip::showText(QCursor::pos(), tr("Image exported successfully!"), this, QRect(), 2000);
      } else {
        QToolTip::showText(QCursor::pos(), tr("Failed to save image!"), this, QRect(), 3000);
      }
    }
  }
}

void GradientMaker::onPickColor() {
  int selectedIndex = m_gradientSlider->selectedStopIndex();
  if (selectedIndex >= 0 && selectedIndex < m_stops.size()) {
    m_screenPicker->pickColor();
  }
}

void GradientMaker::onScreenColorPicked(const QColor &color) {
  int selectedIndex = m_gradientSlider->selectedStopIndex();
  if (selectedIndex >= 0 && selectedIndex < m_stops.size() && color.isValid()) {
    m_stops[selectedIndex].color = color;
    m_colorInput->setText(color.name());
    m_gradientSlider->setStops(m_stops);
    updatePreview();
  }
}

void GradientMaker::onPickerError(const QString &message) {
  qWarning() << "Screen picker error:" << message;
}

void GradientMaker::updatePreview() {
  m_previewFrame->setGradient(m_stops, m_rotation, m_type == Radial);
}

void GradientMaker::updateControls() {
  int selectedIndex = m_gradientSlider->selectedStopIndex();
  bool hasSelection = selectedIndex >= 0 && selectedIndex < m_stops.size();

  m_colorInput->setEnabled(hasSelection);
  m_colorPickerBtn->setEnabled(hasSelection);
  m_positionInput->setEnabled(hasSelection);
  m_removeColorBtn->setEnabled(hasSelection && m_stops.size() > 2);

  if (hasSelection) {
    m_colorInput->blockSignals(true);
    m_positionInput->blockSignals(true);

    m_colorInput->setText(m_stops[selectedIndex].color.name());
    m_positionInput->setValue(qRound(m_stops[selectedIndex].position * 100));

    m_colorInput->blockSignals(false);
    m_positionInput->blockSignals(false);
  } else {
    m_colorInput->clear();
    m_positionInput->setValue(0);
  }
}

QString GradientMaker::generateGradientCSS() const {
    QString result;

    if (m_type == Linear) {
        // Convert preview angle to CSS angle
        int cssAngle = (m_rotation + 90) % 360;

        // Build the stops string
        QString stopsStr;
        for (const GradientStop &stop : m_stops) {
            stopsStr += QString(", %1 %2%")
                       .arg(stop.color.name())
                       .arg(qRound(stop.position * 100));
        }

        // Generate all vendor prefixes
        QStringList gradients;

        // Old WebKit syntax (Safari 4+, Chrome 1-9)
        gradients << QString("background: -webkit-gradient(linear, "
                           "left top, "
                           "%1 %2%3)")
                         .arg(getDirectionForOldWebKit(cssAngle))
                         .arg(cssAngle >= 180 ? "right" : "left")
                         .arg(cssAngle % 180 >= 90 ? " bottom" : " top")
                         .arg(stopsStr);

        // Newer WebKit syntax (Chrome 10+, Safari 5.1+)
        gradients << QString("background: -webkit-linear-gradient(%1deg%2);")
                         .arg(cssAngle)
                         .arg(stopsStr);

        // Firefox 3.6+
        gradients << QString("background: -moz-linear-gradient(%1deg%2);")
                         .arg(cssAngle)
                         .arg(stopsStr);

        // Opera 11.10+
        gradients << QString("background: -o-linear-gradient(%1deg%2);")
                         .arg(cssAngle)
                         .arg(stopsStr);

        // IE 10+
        gradients << QString("background: -ms-linear-gradient(%1deg%2);")
                         .arg(cssAngle)
                         .arg(stopsStr);

        // Standard
        gradients << QString("background: linear-gradient(%1deg%2);")
                         .arg(cssAngle)
                         .arg(stopsStr);

        result = gradients.join("\n");

    } else {
      // Radial gradient - FIXED for proper sizing
      QString stopsStr;
      for (const GradientStop &stop : m_stops) {
        stopsStr += QString(", %1 %2%")
                   .arg(stop.color.name())
                   .arg(qRound(stop.position * 100));
      }

      QStringList gradients;

      // Old WebKit syntax
      gradients << QString("background: -webkit-gradient(radial, "
                         "center center, 0, "
                         "center center, 50%"
                         "%1);")
                   .arg(stopsStr);

      // Newer WebKit syntax - use closest-side
      gradients << QString("background: -webkit-radial-gradient(center, circle closest-side%1);")
                   .arg(stopsStr);

      // Firefox
      gradients << QString("background: -moz-radial-gradient(center, circle closest-side%1);")
                   .arg(stopsStr);

      // Opera
      gradients << QString("background: -o-radial-gradient(circle closest-side at center%1);")
                   .arg(stopsStr);

      // IE 10+
      gradients << QString("background: -ms-radial-gradient(circle closest-side at center%1);")
                   .arg(stopsStr);

      // Standard syntax
      gradients << QString("background: radial-gradient(circle closest-side at center%1);")
                   .arg(stopsStr);

      result = gradients.join("\n");
    }

    return result;
}

// Helper function for old WebKit syntax
QString GradientMaker::getDirectionForOldWebKit(int angle) {
    // Old WebKit uses "left top, right bottom" syntax instead of angles
    // This is a simplified version - for production you'd want more precise conversion
    if (angle < 45 || angle >= 315) return "right top";
    else if (angle < 135) return "right bottom";
    else if (angle < 225) return "left bottom";
    else return "left top";
}

QString GradientMaker::getGradientCSS() const { return generateGradientCSS(); }

QString GradientMaker::getGradientSVG() const {
    int width = 800;
    int height = 400;

    QString svg = QString("<svg width=\"%1\" height=\"%2\" viewBox=\"0 0 %1 %2\" "
                         "xmlns=\"http://www.w3.org/2000/svg\">\n\n")
                         .arg(width).arg(height);

    svg += "  <defs>\n";

    if (m_type == Linear) {
        double radians = qDegreesToRadians((double)m_rotation);

        double centerX = width / 2.0;
        double centerY = height / 2.0;
        double length = qSqrt(width * width + height * height) / 2.0;

        double x1 = centerX - length * qCos(radians);
        double y1 = centerY - length * qSin(radians);
        double x2 = centerX + length * qCos(radians);
        double y2 = centerY + length * qSin(radians);

        svg += QString("    <linearGradient id=\"grad\" x1=\"%1\" y1=\"%2\" "
                      "x2=\"%3\" y2=\"%4\" "
                      "gradientUnits=\"userSpaceOnUse\" "
                      "color-interpolation=\"sRGB\">\n")
                      .arg(x1)
                      .arg(y1)
                      .arg(x2)
                      .arg(y2);
    } else {
        svg += QString("    <radialGradient id=\"grad\" cx=\"%1\" cy=\"%2\" r=\"%3\" "
                      "gradientUnits=\"userSpaceOnUse\" "
                      "color-interpolation=\"sRGB\">\n")
                      .arg(width / 2.0)
                      .arg(height / 2.0)
                      .arg(qMin(width, height) / 2.0);
    }

    for (const GradientStop &stop : m_stops) {
        svg += QString("      <stop offset=\"%1%\" stop-color=\"%2\" />\n")
                   .arg(qRound(stop.position * 100))
                   .arg(stop.color.name());
    }

    svg += m_type == Linear ? "    </linearGradient>\n" : "    </radialGradient>\n";
    svg += "  </defs>\n\n";

    svg += QString("  <rect x=\"0\" y=\"0\" width=\"%1\" height=\"%2\" fill=\"url(#grad)\"/>\n")
               .arg(width).arg(height);

    svg += "</svg>";

    return svg;
}

void GradientMaker::generateRandomGradient() {
  m_stops.clear();

  int numStops = QRandomGenerator::global()->bounded(2, 6);

  for (int i = 0; i < numStops; ++i) {
    QColor randomColor(QRandomGenerator::global()->bounded(256),
                       QRandomGenerator::global()->bounded(256),
                       QRandomGenerator::global()->bounded(256));

    double position = i / (double)(numStops - 1);
    m_stops.append(GradientStop(randomColor, position));
  }

  m_rotation = QRandomGenerator::global()->bounded(360);
  m_rotationInput->setValue(m_rotation);

  m_gradientSlider->setStops(m_stops);
  m_gradientSlider->setSelectedStop(0);

  updatePreview();
  updateControls();
}

void GradientMaker::closeEvent(QCloseEvent *event) {
  saveGeometry();
  QDialog::closeEvent(event);
}

void GradientMaker::saveGeometry() {
  QSettings settings;
  settings.beginGroup("GradientMaker");
  settings.setValue("geometry", QDialog::saveGeometry());
  settings.setValue("size", size());
  settings.setValue("pos", pos());
  settings.endGroup();
}

void GradientMaker::loadGeometry() {
  QSettings settings;
  settings.beginGroup("GradientMaker");

  // Try to restore saved geometry
  QByteArray geometry = settings.value("geometry").toByteArray();
  if (!geometry.isEmpty()) {
    QDialog::restoreGeometry(geometry);
  } else {
    // Fallback to size and position if geometry is not available
    QSize savedSize = settings.value("size", QSize(700, 600)).toSize();
    resize(savedSize);

    if (settings.contains("pos")) {
      QPoint savedPos = settings.value("pos").toPoint();
      move(savedPos);
    }
  }

  settings.endGroup();
}

