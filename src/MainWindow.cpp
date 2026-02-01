#include "../include/MainWindow.h"
#include "../include/AboutDialog.h"
#include "../include/BrightnessSliderWidget.h"
#include "../include/ColorLogic.h"
#include "../include/ColorPreviewWidget.h"
#include "../include/GradientMaker.h"
#include "../include/Palette.h"
#include "../include/PaletteManager.h"
#include "../include/PaletteWidget.h"
#include "../include/ScreenPicker.h"
#include "../include/Settings.h"
#include "../include/qthsvrectpicker.h"
#include "ui_MainWindow.h"

#include <QClipboard>
#include <QKeyEvent>
#include <QScrollArea>
#include <QScrollBar>
#include <QSpinBox>
#include <QStatusBar>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow),
      m_screenPicker(new ScreenPicker(this)), m_programmaticChange(false) {
  ui->setupUi(this);

  // Setup implicit data for comboBox
  ui->comboOutput->setItemData(0, "hex");
  ui->comboOutput->setItemData(1, "rgb");
  ui->comboOutput->setItemData(2, "rgba");
  ui->comboOutput->setItemData(3, "hsl");
  ui->comboOutput->setItemData(4, "hsla");
  ui->comboOutput->setItemData(5, "hsv");
  ui->comboOutput->setItemData(6, "cmyk");

  // Connect ScreenPicker
  connect(m_screenPicker, &ScreenPicker::colorPicked, this,
          &MainWindow::onColorPicked);
  connect(m_screenPicker, &ScreenPicker::errorOccurred, this,
          &MainWindow::onPickerError);

  // Connect Actions
  connect(ui->actionPick_Color, &QAction::triggered, this,
          &MainWindow::onPickColorClicked);
  connect(ui->actionRandom, &QAction::triggered, this,
          &MainWindow::onRandomColorClicked);
  connect(ui->actionGradient, &QAction::triggered, this,
          &MainWindow::onGradientClicked);
  connect(ui->actionAbout, &QAction::triggered, this,
          &MainWindow::onAboutClicked);

  // Install event filters on spinboxes for keyboard shortcuts
  ui->spinR->installEventFilter(this);
  ui->spinG->installEventFilter(this);
  ui->spinB->installEventFilter(this);
  ui->spinA->installEventFilter(this);

  // Connect Inputs
  auto connectRGB = [&](const QSpinBox *spin, const QSlider *slider) {
    connect(spin, QOverload<int>::of(&QSpinBox::valueChanged), this,
            &MainWindow::onRgbInputChanged);
    connect(slider, &QSlider::valueChanged, this,
            &MainWindow::onRgbInputChanged);
  };

  connectRGB(ui->spinR, ui->sliderR);
  connectRGB(ui->spinG, ui->sliderG);
  connectRGB(ui->spinB, ui->sliderB);
  connectRGB(ui->spinA, ui->sliderA);

  connect(ui->btnResetAlpha, &QPushButton::clicked, this,
          [this]() { ui->spinA->setValue(255); });

  // Create and add new brightness slider widget
  m_brightnessSlider = new BrightnessSliderWidget(this);
  ui->brightnessLayout->insertWidget(0, m_brightnessSlider);

  connect(m_brightnessSlider, &BrightnessSliderWidget::colorChanged, this,
          &MainWindow::onBrightnessColorChanged);
  connect(ui->btnResetBrightness, &QPushButton::clicked, this,
          &MainWindow::onResetBrightness);

  connect(ui->comboOutput, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, &MainWindow::onOutputModeChanged);
  connect(ui->outputEdit, &QLineEdit::textChanged, this,
          &MainWindow::onOutputTextChanged);

  connect(ui->btnCopyColor, &QPushButton::clicked, this,
          &MainWindow::onCopyClicked);

  // Add copy action to the line edit
  // QAction *copyAction = new QAction(this);
  // copyAction->setIcon(QIcon::fromTheme("edit-copy-symbolic"));
  // copyAction->setToolTip(tr("Copy color"));
  // copyAction->setShortcut(QKeySequence::Copy);
  // connect(copyAction, &QAction::triggered, this, &MainWindow::onCopyClicked);
  // ui->outputEdit->addAction(copyAction, QLineEdit::TrailingPosition);

  // Replace the placeholder colorPreview QFrame with our ColorPreviewWidget
  m_colorPreview = new ColorPreviewWidget(this);
  ui->colorPreviewFrame->layout()->addWidget(m_colorPreview);

  // Create and add QtHsvRectPicker
  m_colorPlane = new QtHsvRectPicker(this);
  ui->colorPlaneFrame->layout()->addWidget(m_colorPlane);
  ui->colorPlaneFrame->setFrameShape(QFrame::StyledPanel);
  ui->colorPlaneFrame->setFrameShadow(QFrame::Plain);

  // Connect color picker signals
  connect(m_colorPlane, &QtHsvRectPicker::colorChanged, this,
          &MainWindow::onColorPlaneColorChanged);

  // Create and add PaletteWidget below the sliders
  m_paletteWidget = new PaletteWidget(this);

  ui->paletteFrame->layout()->addWidget(m_paletteWidget);
  ui->paletteFrame->setFrameShape(QFrame::StyledPanel);
  ui->paletteFrame->setFrameShadow(QFrame::Plain);

  // Connect palette signals
  connect(m_paletteWidget, &PaletteWidget::colorSelected, this,
          &MainWindow::onPaletteColorSelected);
  connect(m_paletteWidget, &PaletteWidget::addColorRequested, this,
          &MainWindow::onAddToPaletteClicked);

  // Load palettes and set current
  PaletteManager::instance().loadPalettes();
  m_paletteWidget->setPalette(PaletteManager::instance().currentPalette());


  // Make statusbar always visible
  statusBar()->show();
  statusBar()->showMessage(tr("Ready"));

  loadSettings();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::updateColor(const QColor &color, bool updateOutputField,
                             bool skipBrightnessReset, bool addToRecent) {
  if (!color.isValid())
    return;

  m_programmaticChange = true;
  m_currentColor = color;

  // Update color preview widget
  m_colorPreview->setColor(color);

  // Update color picker widget
  m_colorPlane->blockSignals(true);
  m_colorPlane->setColor(color);
  m_colorPlane->blockSignals(false);

  // Update inputs including alpha
  QSpinBox *spins[] = {ui->spinR, ui->spinG, ui->spinB, ui->spinA};
  QSlider *sliders[] = {ui->sliderR, ui->sliderG, ui->sliderB, ui->sliderA};
  const int values[] = {color.red(), color.green(), color.blue(), color.alpha()};

  for (int i = 0; i < 4; i++) {
    spins[i]->blockSignals(true);
    sliders[i]->blockSignals(true);
    spins[i]->setValue(values[i]);
    sliders[i]->setValue(values[i]);
    spins[i]->blockSignals(false);
    sliders[i]->blockSignals(false);
  }

  // Only reset brightness slider if not skipped
  if (!skipBrightnessReset) {
    m_brightnessSlider->setColor(color);
    m_baseColorForBrightness = color;
  }

  // Add color to recently picked colors (only when explicitly requested)
  if (addToRecent) {
    PaletteManager::instance().addToRecentColors(color);
  }

  if (updateOutputField) {
    ui->outputEdit->blockSignals(true);
    updateOutput();
    ui->outputEdit->blockSignals(false);
  }

  m_programmaticChange = false;
}

void MainWindow::updateOutput() const {
  if (const QString mode = ui->comboOutput->currentData().toString();
      mode == "hex") {
    ui->outputEdit->setText(ColorLogic::colorToHex(m_currentColor));
  } else if (mode == "rgb") {
    ui->outputEdit->setText(ColorLogic::colorToRgbString(m_currentColor));
  } else if (mode == "rgba") {
    ui->outputEdit->setText(ColorLogic::colorToRgbaString(m_currentColor));
  } else if (mode == "hsl") {
    ui->outputEdit->setText(ColorLogic::colorToHslString(m_currentColor));
  } else if (mode == "hsla") {
    ui->outputEdit->setText(ColorLogic::colorToHslaString(m_currentColor));
  } else if (mode == "hsv") {
    ui->outputEdit->setText(ColorLogic::colorToHsvString(m_currentColor));
  } else if (mode == "cmyk") {
    ui->outputEdit->setText(ColorLogic::colorToCmykString(m_currentColor));
  }
}

void MainWindow::onRgbInputChanged() {
  if (m_programmaticChange)
    return;

  QObject *senderObj = sender();

  int r, g, b, a;

  // Helper to get value
  auto getValue = [&](const QSpinBox *s, const QSlider *l) {
    if (senderObj == s)
      return s->value();
    if (senderObj == l)
      return l->value();
    return s->value();
  };

  if (senderObj == ui->spinR || senderObj == ui->sliderR)
    r = getValue(ui->spinR, ui->sliderR);
  else
    r = ui->spinR->value();

  if (senderObj == ui->spinG || senderObj == ui->sliderG)
    g = getValue(ui->spinG, ui->sliderG);
  else
    g = ui->spinG->value();

  if (senderObj == ui->spinB || senderObj == ui->sliderB)
    b = getValue(ui->spinB, ui->sliderB);
  else
    b = ui->spinB->value();

  if (senderObj == ui->spinA || senderObj == ui->sliderA)
    a = getValue(ui->spinA, ui->sliderA);
  else
    a = ui->spinA->value();

  // RGB slider adjustments don't add to recent colors
  updateColor(QColor(r, g, b, a), true, false, false);
}

void MainWindow::onOutputModeChanged() const {
  // Block signals to prevent onOutputTextChanged from firing and changing the
  // color
  ui->outputEdit->blockSignals(true);
  updateOutput();
  ui->outputEdit->blockSignals(false);
}

void MainWindow::onOutputTextChanged() {
  if (m_programmaticChange)
    return;

  QString text = ui->outputEdit->text();
  QString mode = ui->comboOutput->currentData().toString();
  QColor newColor;

  if (mode == "hex") {
    newColor = ColorLogic::hexToColor(text);
  } else if (mode == "rgb") {
    newColor = ColorLogic::rgbStringToColor(text);
  } else if (mode == "rgba") {
    newColor = ColorLogic::rgbaStringToColor(text);
  } else if (mode == "hsl") {
    newColor = ColorLogic::hslStringToColor(text);
  } else if (mode == "hsla") {
    newColor = ColorLogic::hslaStringToColor(text);
  } else if (mode == "hsv") {
    newColor = ColorLogic::hsvStringToColor(text);
  } else if (mode == "cmyk") {
    newColor = ColorLogic::cmykStringToColor(text);
  }

  if (newColor.isValid()) {
    // Manual color entry should NOT add to recent colors
    updateColor(newColor, false, false, false);
  }
}

void MainWindow::onCopyClicked() const {
  QClipboard *clipboard = QApplication::clipboard();
  clipboard->setText(ui->outputEdit->text());
  statusBar()->showMessage(tr("%1 copied!").arg(ui->outputEdit->text()), 2000);
}

void MainWindow::onPickColorClicked() const { m_screenPicker->pickColor(); }

void MainWindow::onBrightnessColorChanged(const QColor &color) {
  if (m_programmaticChange)
    return;

  // Preserve current alpha
  QColor colorWithCurrentAlpha = color;
  colorWithCurrentAlpha.setAlpha(m_currentColor.alpha());
  // Update with the new brightness-adjusted color
  // Skip brightness reset and don't add to recent colors
  updateColor(colorWithCurrentAlpha, true, true, false);
}

void MainWindow::onResetBrightness() {
  // Restore the base color if it exists
  if (m_baseColorForBrightness.isValid()) {
    // Temporarily set programmatic change to prevent any side effects
    m_programmaticChange = true;

    // Update brightness slider widget with base color
    m_brightnessSlider->setColor(m_baseColorForBrightness);

    // Restore the base color without adding to recent colors (skip brightness
    // reset) Since we're restoring, not picking a new color, we skip the recent
    // colors update
    m_currentColor = m_baseColorForBrightness;

    // Update color preview widget
    m_colorPreview->setColor(m_baseColorForBrightness);

    // Update color picker widget
    m_colorPlane->blockSignals(true);
    m_colorPlane->setColor(m_baseColorForBrightness);
    m_colorPlane->blockSignals(false);

    // Update RGB sliders
    ui->spinR->setValue(m_baseColorForBrightness.red());
    ui->sliderR->setValue(m_baseColorForBrightness.red());
    ui->spinG->setValue(m_baseColorForBrightness.green());
    ui->sliderG->setValue(m_baseColorForBrightness.green());
    ui->spinB->setValue(m_baseColorForBrightness.blue());
    ui->sliderB->setValue(m_baseColorForBrightness.blue());
    ui->spinA->setValue(m_baseColorForBrightness.alpha());
    ui->sliderA->setValue(m_baseColorForBrightness.alpha());

    // Update output
    updateOutput();

    m_programmaticChange = false;
  }
}

void MainWindow::onColorPicked(const QColor &color) {
  // Screen picker should add to recent colors
  updateColor(color, true, false, true);
}

void MainWindow::onPickerError(const QString &message) const {
  statusBar()->showMessage(message, 3000);
}

void MainWindow::onRandomColorClicked() {
  QColor randomColor = ColorLogic::randomColor();
  // Random color should add to recent colors
  updateColor(randomColor, true, false, true);
}

void MainWindow::onAboutClicked() {
  AboutDialog aboutDialog(this);
  aboutDialog.exec();
}

void MainWindow::onGradientClicked() {
  GradientMaker *gradientMaker = new GradientMaker(this);
  gradientMaker->setAttribute(Qt::WA_DeleteOnClose);
  gradientMaker->show();
}

void MainWindow::onPaletteColorSelected(const QColor &color) {
  // Selecting from palette should NOT add to recent colors
  updateColor(color, true, false, false);
}

void MainWindow::onColorPlaneColorChanged(const QColor &color) {
  // Preserve current alpha
  QColor colorWithCurrentAlpha = color;
  colorWithCurrentAlpha.setAlpha(m_currentColor.alpha());
  // Color picker changes should update live but not add to recent
  updateColor(colorWithCurrentAlpha, true, false, false);
}

void MainWindow::onAddToPaletteClicked() const {
  if (Palette *currentPalette = PaletteManager::instance().currentPalette()) {
    // Check if palette is read-only (except for "Recent" palette)
    if (currentPalette->isReadOnly() &&
        currentPalette->id() != PaletteManager::RECENTLY_PICKED_PALETTE_ID) {
      statusBar()->showMessage(tr("Cannot add colors to read-only palettes"),
                               2000);
      return;
    }

    // Special handling for Recently Picked palette
    if (currentPalette->id() == PaletteManager::RECENTLY_PICKED_PALETTE_ID) {
      // Use the proper method that handles FIFO and prepending
      PaletteManager::instance().addToRecentColors(m_currentColor);
      m_paletteWidget->refreshColors();

      // Scroll to top to see the newly added color (it's prepended)
      QTimer::singleShot(50, [this]() {
        if (const QScrollArea *scrollArea =
                m_paletteWidget->findChild<QScrollArea *>()) {
          if (QScrollBar *vScrollBar = scrollArea->verticalScrollBar()) {
            vScrollBar->setValue(0); // Scroll to top
          }
        }
      });

      statusBar()->showMessage(tr("Color added to recent colors"), 2000);
      return;
    }

    // For regular palettes, check if color already exists
    bool exists = false;
    for (const QColor &existingColor : currentPalette->colors()) {
      if (existingColor == m_currentColor) {
        exists = true;
        break;
      }
    }

    if (!exists) {
      currentPalette->addColor(m_currentColor);
      m_paletteWidget->refreshColors();

      // Scroll to the newly added color (appended to bottom)
      QTimer::singleShot(50, [this]() {
        if (const QScrollArea *scrollArea =
                m_paletteWidget->findChild<QScrollArea *>()) {
          if (QScrollBar *vScrollBar = scrollArea->verticalScrollBar()) {
            vScrollBar->setValue(vScrollBar->maximum());
          }
        }
      });

      PaletteManager::instance().savePalettes();
      statusBar()->showMessage(tr("Color added to palette"), 2000);
    } else {
      statusBar()->showMessage(tr("Color already in palette"), 2000);
    }
  }
}

void MainWindow::loadSettings() {
  const Settings::Manager &settings = Settings::Manager::instance();

  // Restore window geometry and state
  const QByteArray geometry = settings.getWindowGeometry();
  const QByteArray state = settings.getWindowState();

  if (!geometry.isEmpty()) {
    restoreGeometry(geometry);
  }

  if (!state.isEmpty()) {
    restoreState(state);
  }

  // Restore last color
  if (const QString lastColor = settings.getLastColor(); !lastColor.isEmpty()) {
    // Restoring saved color should not add to recent
    updateColor(ColorLogic::hexToColor(lastColor), true, false, false);
  } else {
    // Initial random color should not add to recent
    updateColor(ColorLogic::randomColor(), true, false, false);
  }

  // Restore output mode (combobox selection)
  const QString outputMode = settings.getOutputMode();
  for (int i = 0; i < ui->comboOutput->count(); ++i) {
    if (ui->comboOutput->itemData(i).toString() == outputMode) {
      ui->comboOutput->setCurrentIndex(i);
      break;
    }
  }
}

void MainWindow::saveSettings() const {
  Settings::Manager &settings = Settings::Manager::instance();

  // Save window geometry and state
  settings.setWindowGeometry(saveGeometry());
  settings.setWindowState(saveState());

  // Save last color
  settings.setLastColor(ColorLogic::colorToHex(m_currentColor));

  // Save output mode (combobox selection)
  const QString outputMode = ui->comboOutput->currentData().toString();
  settings.setOutputMode(outputMode);

  // Save palettes
  PaletteManager::instance().savePalettes();
}

void MainWindow::closeEvent(QCloseEvent *event) {
  saveSettings();
  QMainWindow::closeEvent(event);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
  if (event->type() == QEvent::KeyPress) {
    const QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

    // Check if the object is one of our spinboxes
    if (QSpinBox *spinBox = qobject_cast<QSpinBox *>(obj);
        spinBox && (spinBox == ui->spinR || spinBox == ui->spinG ||
                    spinBox == ui->spinB || spinBox == ui->spinA)) {
      if (handleSpinBoxKeyPress(spinBox, keyEvent)) {
        return true; // Event handled
      }
    }
  }

  // Pass the event to the base class
  return QMainWindow::eventFilter(obj, event);
}

bool MainWindow::handleSpinBoxKeyPress(QSpinBox *spinBox,
                                       const QKeyEvent *event) {
  int delta = 0;
  bool handled = false;

  const Qt::KeyboardModifiers modifiers = event->modifiers();

  // Handle different key combinations
  if (const int key = event->key(); key == Qt::Key_Up) {
    if (modifiers & Qt::ShiftModifier) {
      delta = 10; // Shift + Up: +10
    } else if (modifiers & Qt::AltModifier) {
      delta = 5; // Alt + Up: +5
    } else if (modifiers == Qt::NoModifier) {
      delta = 1; // Up: +1
    }
    handled = true;
  } else if (key == Qt::Key_Down) {
    if (modifiers & Qt::ShiftModifier) {
      delta = -10; // Shift + Down: -10
    } else if (modifiers & Qt::AltModifier) {
      delta = -5; // Alt + Down: -5
    } else if (modifiers == Qt::NoModifier) {
      delta = -1; // Down: -1
    }
    handled = true;
  } else if (key == Qt::Key_PageUp) {
    delta = 25; // PageUp: +25
    handled = true;
  } else if (key == Qt::Key_PageDown) {
    delta = -25; // PageDown: -25
    handled = true;
  } else if (key == Qt::Key_Home) {
    spinBox->setValue(0); // Home: Min (0)
    return true;
  } else if (key == Qt::Key_End) {
    spinBox->setValue(255); // End: Max (255)
    return true;
  }

  if (handled && delta != 0) {
    int newValue = spinBox->value() + delta;
    newValue = qBound(0, newValue, 255);
    spinBox->setValue(newValue);
    return true;
  }

  return false;
}
