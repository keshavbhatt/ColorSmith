#include "../include/PaletteWidget.h"
#include "../include/ColorExtractor.h"
#include "../include/Palette.h"
#include "../include/PaletteManager.h"

#include <QComboBox>
#include <QFileDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QImage>
#include <QInputDialog>
#include <QLabel>
#include <QMainWindow>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QScrollArea>
#include <QStatusBar>
#include <QTextStream>
#include <QTimer>
#include <QToolButton>

// ColorSwatch implementations
ColorSwatch::ColorSwatch(const QColor &color, QWidget *parent, bool readOnly,
                         const QString &colorName)
    : QWidget(parent), m_color(color), m_colorName(colorName),
      m_readOnly(readOnly) {
  setFixedSize(SWATCH_SIZE, SWATCH_SIZE);
  setCursor(Qt::PointingHandCursor);

  // Create tooltip with color name and hex value
  QString tooltip;
  if (!m_colorName.isEmpty()) {
    tooltip = QString("%1\n%2").arg(m_colorName, color.name(QColor::HexArgb));
  } else {
    tooltip = color.name(QColor::HexArgb);
  }
  setToolTip(tooltip);
}

void ColorSwatch::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  // Draw checkered background for transparency
  painter.fillRect(rect(), Qt::white);
  int checkerSize = 5;
  for (int y = 0; y < height(); y += checkerSize) {
    for (int x = 0; x < width(); x += checkerSize) {
      if ((x / checkerSize + y / checkerSize) % 2 == 0) {
        painter.fillRect(x, y, checkerSize, checkerSize, QColor(204, 204, 204));
      }
    }
  }

  // Draw color
  painter.fillRect(rect(), m_color);

  // Draw border
  painter.setPen(QPen(QColor(136, 136, 136), 1));
  painter.drawRect(rect().adjusted(0, 0, -1, -1));
}

void ColorSwatch::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    emit clicked();
  }
  QWidget::mousePressEvent(event);
}

void ColorSwatch::contextMenuEvent(QContextMenuEvent *event) {
  if (m_readOnly) {
    // No context menu for read-only swatches
    return;
  }

  QMenu menu(this);
  QAction *removeAction = menu.addAction(tr("Remove"));
  QAction *selected = menu.exec(event->globalPos());
  if (selected == removeAction) {
    emit removeRequested();
  }
}

// AddColorSwatch implementation
AddColorSwatch::AddColorSwatch(QWidget *parent)
    : QWidget(parent), m_enabled(true), m_hovered(false) {
  setFixedSize(SWATCH_SIZE, SWATCH_SIZE);
  setCursor(Qt::PointingHandCursor);
  setToolTip(tr("Add current color to palette"));
}

void AddColorSwatch::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  // Background
  QColor bgColor =
      m_hovered && m_enabled ? QColor(230, 230, 230) : QColor(245, 245, 245);
  painter.fillRect(rect(), bgColor);

  // Border
  painter.setPen(QPen(m_enabled ? QColor(136, 136, 136) : QColor(200, 200, 200),
                      1, Qt::DashLine));
  painter.drawRect(rect().adjusted(0, 0, -1, -1));

  // Plus icon
  painter.setPen(
      QPen(m_enabled ? QColor(100, 100, 100) : QColor(180, 180, 180), 2));
  int center = SWATCH_SIZE / 2;
  int size = 12;
  painter.drawLine(center - size / 2, center, center + size / 2, center);
  painter.drawLine(center, center - size / 2, center, center + size / 2);
}

void AddColorSwatch::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton && m_enabled) {
    emit clicked();
  }
  QWidget::mousePressEvent(event);
}

void AddColorSwatch::enterEvent(QEnterEvent *event) {
  Q_UNUSED(event);
  m_hovered = true;
  update();
}

void AddColorSwatch::leaveEvent(QEvent *event) {
  Q_UNUSED(event);
  m_hovered = false;
  update();
}

// PaletteWidget implementation
PaletteWidget::PaletteWidget(QWidget *parent)
    : QWidget(parent), m_currentPalette(nullptr), m_newPaletteAction(nullptr),
      m_renamePaletteAction(nullptr), m_deletePaletteAction(nullptr),
      m_generateFromImageAction(nullptr), m_clearPaletteAction(nullptr),
      m_exportPaletteAction(nullptr), m_importPaletteAction(nullptr) {
  setupUI();
}

void PaletteWidget::setupUI() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);

  // Header
  m_headerFrame = new QFrame(this);

  QHBoxLayout *headerLayout = new QHBoxLayout(m_headerFrame);
  headerLayout->setContentsMargins(0, 0, 0, 0);
  headerLayout->setSpacing(8);

  // Title
  m_titleLabel = new QLabel(tr("Palette"), m_headerFrame);
  QFont font = m_titleLabel->font();
  font.setBold(true);
  m_titleLabel->setFont(font);
  headerLayout->addWidget(m_titleLabel);

  // Palette selector combo box
  m_paletteCombo = new QComboBox(m_headerFrame);
  m_paletteCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
  m_paletteCombo->setMinimumWidth(100);
  connect(m_paletteCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, &PaletteWidget::onPaletteSelected);
  headerLayout->addWidget(m_paletteCombo);

  headerLayout->addStretch();

  // Add Color button
  m_addButton = new QToolButton(m_headerFrame);
  m_addButton->setText("+");
  m_addButton->setToolTip(tr("Add current color to palette"));
  m_addButton->setAutoRaise(true);
  m_addButton->setFixedSize(28, 28);
  QFont btnFont = m_addButton->font();
  btnFont.setPointSize(14);
  m_addButton->setFont(btnFont);
  connect(m_addButton, &QToolButton::clicked, this,
          &PaletteWidget::onAddColorClicked);
  headerLayout->addWidget(m_addButton);

  // Menu button
  m_menuButton = new QToolButton(m_headerFrame);
  m_menuButton->setText("⋮");
  m_menuButton->setAutoRaise(true);
  m_menuButton->setPopupMode(QToolButton::InstantPopup);
  QFont menuFont = m_menuButton->font();
  menuFont.setPointSize(14);
  m_menuButton->setFont(menuFont);

  QMenu *menu = new QMenu(m_menuButton);
  m_newPaletteAction =
      menu->addAction(tr("New Palette..."), this, &PaletteWidget::onNewPalette);
  m_renamePaletteAction = menu->addAction(tr("Rename Palette..."), this,
                                          &PaletteWidget::onRenamePalette);
  m_deletePaletteAction = menu->addAction(tr("Delete Palette..."), this,
                                          &PaletteWidget::onDeletePalette);
  menu->addSeparator();
  m_generateFromImageAction = menu->addAction(
      tr("Generate from Image..."), this, &PaletteWidget::onGenerateFromImage);
  menu->addSeparator();
  m_clearPaletteAction = menu->addAction(tr("Clear Palette"), this,
                                         &PaletteWidget::onClearPalette);
  menu->addSeparator();
  m_exportPaletteAction = menu->addAction(tr("Export Palette..."), this,
                                          &PaletteWidget::onExportPalette);
  m_importPaletteAction = menu->addAction(tr("Import Palette..."), this,
                                          &PaletteWidget::onImportPalette);
  m_menuButton->setMenu(menu);

  headerLayout->addWidget(m_menuButton);

  mainLayout->addWidget(m_headerFrame);

  // Content widget with scroll area
  m_contentWidget = new QWidget(this);
  QVBoxLayout *contentLayout = new QVBoxLayout(m_contentWidget);
  contentLayout->setContentsMargins(0, 4, 0, 4); // No left/right margins

  m_scrollArea = new QScrollArea(m_contentWidget);
  m_scrollArea->setWidgetResizable(true);
  m_scrollArea->setFrameShape(QFrame::NoFrame);
  m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

  m_gridWidget = new QWidget();
  m_gridLayout = new QGridLayout(m_gridWidget);
  m_gridLayout->setContentsMargins(0, SPACING, 0, SPACING);
  m_gridLayout->setSpacing(SPACING);

  m_scrollArea->setWidget(m_gridWidget);
  contentLayout->addWidget(m_scrollArea);

  // Create empty state message widget (hidden by default)
  m_emptyStateLabel = new QLabel(m_contentWidget);
  m_emptyStateLabel->setText(
      tr("No colors in this palette.\nClick the + button to add colors."));
  m_emptyStateLabel->setAlignment(Qt::AlignCenter);
  m_emptyStateLabel->setSizePolicy(QSizePolicy::Expanding,
                                   QSizePolicy::Expanding);
  m_emptyStateLabel->setWordWrap(true);
  m_emptyStateLabel->setStyleSheet("QLabel { color: gray; padding: 20px; }");
  m_emptyStateLabel->hide();
  contentLayout->addWidget(m_emptyStateLabel);

  mainLayout->addWidget(m_contentWidget);

  updateLayout();
}

void PaletteWidget::updateLayout() {
  // Clear grid
  while (m_gridLayout->count() > 0) {
    QLayoutItem *item = m_gridLayout->takeAt(0);
    if (item->widget()) {
      item->widget()->setParent(nullptr);
    }
    delete item;
  }

  // Check if palette is empty
  bool isEmpty = m_swatches.isEmpty();

  // Show/hide empty state message
  m_emptyStateLabel->setVisible(isEmpty);
  m_scrollArea->setVisible(!isEmpty);

  if (!isEmpty) {
    // Add swatches to grid
    int col = 0;
    int row = 0;
    for (ColorSwatch *swatch : std::as_const(m_swatches)) {
      m_gridLayout->addWidget(swatch, row, col);
      col++;
      if (col >= MAX_COLUMNS) {
        col = 0;
        row++;
      }
    }
  }

  m_gridWidget->adjustSize();
}

void PaletteWidget::setPalette(Palette *palette) {
  m_currentPalette = palette;
  updatePaletteCombo();
  refreshColors();

  // Disable edit operations for read-only palettes (except for "Recent" palette)
  bool isReadOnly = palette && palette->isReadOnly();
  // Enable + button for "Recent" palette even though it's read-only
  const bool enableAddButton = !isReadOnly ||
      (palette && palette->id() == PaletteManager::RECENTLY_PICKED_PALETTE_ID);
  m_addButton->setEnabled(enableAddButton);

  auto setActionEnabled = [isReadOnly](QAction *action,
                                       bool disableForReadOnly) {
    if (!action)
      return;
    action->setEnabled(disableForReadOnly ? !isReadOnly : true);
  };

  setActionEnabled(m_renamePaletteAction, true);
  setActionEnabled(m_deletePaletteAction, true);
  setActionEnabled(m_clearPaletteAction, true);
  setActionEnabled(m_importPaletteAction, true);
  setActionEnabled(m_generateFromImageAction, false);
  setActionEnabled(m_newPaletteAction, false);
  setActionEnabled(m_exportPaletteAction, false);
}

void PaletteWidget::refreshColors() {
  // Clear existing swatches
  for (ColorSwatch *swatch : std::as_const(m_swatches)) {
    swatch->deleteLater();
  }
  m_swatches.clear();

  // Add swatches for current palette
  if (m_currentPalette) {
    const QVector<QColor> colors = m_currentPalette->colors();
    for (int i = 0; i < colors.size(); ++i) {
      QString colorName = m_currentPalette->colorName(i);
      addColorToUI(colors[i], colorName);
    }
  }

  updateLayout();
}

void PaletteWidget::addColorToUI(const QColor &color,
                                 const QString &colorName) {
  if (!color.isValid())
    return;

  bool isReadOnly = m_currentPalette && m_currentPalette->isReadOnly();
  ColorSwatch *swatch =
      new ColorSwatch(color, m_gridWidget, isReadOnly, colorName);
  connect(swatch, &ColorSwatch::clicked, this, &PaletteWidget::onSwatchClicked);

  if (!isReadOnly) {
    connect(swatch, &ColorSwatch::removeRequested, this,
            &PaletteWidget::onSwatchRemoveRequested);
  }

  m_swatches.append(swatch);
}

void PaletteWidget::onClearPalette() {
  if (!m_currentPalette || m_currentPalette->colorCount() == 0)
    return;

  if (m_currentPalette->isReadOnly()) {
    QMessageBox::information(this, tr("Clear Palette"),
                             tr("Cannot modify read-only palettes."));
    return;
  }

  QMessageBox::StandardButton reply = QMessageBox::question(
      this, tr("Clear Palette"),
      tr("Are you sure you want to clear all colors from the palette?"),
      QMessageBox::Yes | QMessageBox::No);

  if (reply == QMessageBox::Yes) {
    m_currentPalette->clearColors();
    refreshColors();
    PaletteManager::instance().savePalettes();
  }
}

void PaletteWidget::onExportPalette() {
  if (!m_currentPalette || m_currentPalette->colorCount() == 0) {
    QMessageBox::information(this, tr("Export Palette"),
                             tr("Palette is empty."));
    return;
  }

  QString fileName =
      QFileDialog::getSaveFileName(this, tr("Export Palette"), QString(),
                                   tr("Palette Files (*.txt);;All Files (*)"));

  if (fileName.isEmpty())
    return;

  QFile file(fileName);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::warning(this, tr("Export Error"),
                         tr("Could not open file for writing."));
    return;
  }

  QTextStream out(&file);
  for (const QColor &color : m_currentPalette->colors()) {
    out << color.name(QColor::HexArgb) << "\n";
  }
  file.close();

  QMessageBox::information(this, tr("Export Palette"),
                           tr("Palette exported successfully."));
}

void PaletteWidget::onImportPalette() {
  if (!m_currentPalette)
    return;

  if (m_currentPalette->isReadOnly()) {
    QMessageBox::information(this, tr("Import Palette"),
                             tr("Cannot import into read-only palettes."));
    return;
  }

  QString fileName =
      QFileDialog::getOpenFileName(this, tr("Import Palette"), QString(),
                                   tr("Palette Files (*.txt);;All Files (*)"));

  if (fileName.isEmpty())
    return;

  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::warning(this, tr("Import Error"),
                         tr("Could not open file for reading."));
    return;
  }

  m_currentPalette->clearColors();

  QTextStream in(&file);
  while (!in.atEnd()) {
    QString line = in.readLine().trimmed();
    if (!line.isEmpty()) {
      QColor color(line);
      if (color.isValid()) {
        m_currentPalette->addColor(color);
      }
    }
  }
  file.close();

  refreshColors();
  PaletteManager::instance().savePalettes();

  QMessageBox::information(this, tr("Import Palette"),
                           tr("Palette imported successfully."));
}

void PaletteWidget::onAddColorClicked() {
  // Emit signal for MainWindow to add current color
  emit addColorRequested();
}

void PaletteWidget::onSwatchClicked() {
  ColorSwatch *swatch = qobject_cast<ColorSwatch *>(sender());
  if (swatch) {
    emit colorSelected(swatch->color());
  }
}

void PaletteWidget::onSwatchRemoveRequested() {
  if (!m_currentPalette)
    return;

  ColorSwatch *swatch = qobject_cast<ColorSwatch *>(sender());
  if (swatch) {
    int index = m_swatches.indexOf(swatch);
    if (index >= 0) {
      m_currentPalette->removeColor(index);
      m_swatches.removeAt(index);
      swatch->deleteLater();
      updateLayout();
      PaletteManager::instance().savePalettes();
    }
  }
}

void PaletteWidget::updatePaletteCombo() {
  m_paletteCombo->blockSignals(true);
  m_paletteCombo->clear();

  QVector<Palette *> palettes = PaletteManager::instance().allPalettes();
  int currentIndex = -1;
  int insertedCount = 0;

  for (int i = 0; i < palettes.size(); ++i) {
    Palette *palette = palettes[i];

    // Add separator after system palettes (first 2: Standard HTML Colors and
    // Recently Picked)
    if (i == 2 && palettes.size() > 2) {
      m_paletteCombo->insertSeparator(insertedCount);
      insertedCount++;
    }

    m_paletteCombo->addItem(palette->name(), palette->id());

    if (palette == m_currentPalette) {
      currentIndex = insertedCount;
    }

    insertedCount++;
  }

  if (currentIndex >= 0) {
    m_paletteCombo->setCurrentIndex(currentIndex);
  }

  m_paletteCombo->blockSignals(false);
}

void PaletteWidget::onPaletteSelected(int index) {
  if (index < 0)
    return;

  QString paletteId = m_paletteCombo->itemData(index).toString();
  Palette *palette = PaletteManager::instance().getPalette(paletteId);

  if (palette && palette != m_currentPalette) {
    PaletteManager::instance().setCurrentPalette(palette);
    setPalette(palette);
    PaletteManager::instance().savePalettes();
  }
}

void PaletteWidget::onNewPalette() {
  bool ok;
  QString name =
      QInputDialog::getText(this, tr("New Palette"), tr("Palette name:"),
                            QLineEdit::Normal, tr("New Palette"), &ok);

  if (ok && !name.isEmpty()) {
    Palette *newPalette = PaletteManager::instance().createPalette(name);
    PaletteManager::instance().setCurrentPalette(newPalette);
    m_currentPalette = newPalette;
    updatePaletteCombo();
    refreshColors();
    PaletteManager::instance().savePalettes();
    statusBar()->showMessage(tr("Created palette: %1").arg(name), 2000);
  }
}

void PaletteWidget::onRenamePalette() {
  if (!m_currentPalette)
    return;

  bool ok;
  QString newName =
      QInputDialog::getText(this, tr("Rename Palette"), tr("New name:"),
                            QLineEdit::Normal, m_currentPalette->name(), &ok);

  if (ok && !newName.isEmpty()) {
    PaletteManager::instance().renamePalette(m_currentPalette->id(), newName);
    updatePaletteCombo();
    PaletteManager::instance().savePalettes();
  }
}

void PaletteWidget::onDeletePalette() {
  if (!m_currentPalette)
    return;

  if (m_currentPalette->isReadOnly()) {
    QMessageBox::information(this, tr("Delete Palette"),
                             tr("Cannot delete read-only palettes."));
    return;
  }

  QMessageBox::StandardButton reply = QMessageBox::question(
      this, tr("Delete Palette"),
      tr("Are you sure you want to delete the palette '%1'?")
          .arg(m_currentPalette->name()),
      QMessageBox::Yes | QMessageBox::No);

  if (reply == QMessageBox::Yes) {
    QString paletteId = m_currentPalette->id();
    PaletteManager::instance().deletePalette(paletteId);
    m_currentPalette = PaletteManager::instance().currentPalette();
    updatePaletteCombo();
    refreshColors();
    PaletteManager::instance().savePalettes();
  }
}

// Helper to get parent window's status bar
QStatusBar *PaletteWidget::statusBar() {
  QWidget *window = this->window();
  QMainWindow *mainWindow = qobject_cast<QMainWindow *>(window);
  if (mainWindow) {
    return mainWindow->statusBar();
  }
  return nullptr;
}

void PaletteWidget::onGenerateFromImage() {
  QString fileName = QFileDialog::getOpenFileName(
      this, tr("Select Image"), QString(),
      tr("Image Files (*.png *.jpg *.jpeg *.bmp *.gif *.webp);;All Files (*)"));

  if (fileName.isEmpty())
    return;

  QImage image(fileName);
  if (image.isNull()) {
    QMessageBox::warning(this, tr("Generate from Image"),
                         tr("Failed to load image."));
    return;
  }

  // Ask user how many colors to extract
  bool ok;
  int colorCount = QInputDialog::getInt(this, tr("Generate from Image"),
                                        tr("Number of colors to extract:"), 8,
                                        2, 20, 1, &ok);

  if (!ok)
    return;

  // Extract dominant colors using ColorExtractor utility
  QVector<QColor> colors =
      ColorExtractor::extractDominantColors(image, colorCount);

  if (colors.isEmpty()) {
    QMessageBox::warning(this, tr("Generate from Image"),
                         tr("Failed to extract colors from image."));
    return;
  }

  // Ask if user wants to create a new palette or add to current
  QMessageBox::StandardButton reply =
      QMessageBox::question(this, tr("Generate from Image"),
                            tr("Create a new palette with these colors?"),
                            QMessageBox::Yes | QMessageBox::Cancel);

  if (reply == QMessageBox::Cancel)
    return;

  if (reply == QMessageBox::Yes) {
    // Create new palette
    QString paletteName =
        QInputDialog::getText(this, tr("New Palette"), tr("Palette name:"),
                              QLineEdit::Normal, tr("Image Palette"), &ok);

    if (!ok || paletteName.isEmpty())
      return;

    Palette *newPalette = PaletteManager::instance().createPalette(paletteName);
    for (const QColor &color : colors) {
      newPalette->addColor(color);
    }

    PaletteManager::instance().setCurrentPalette(newPalette);
    m_currentPalette = newPalette;
    updatePaletteCombo();
    refreshColors();
    PaletteManager::instance().savePalettes();

    if (statusBar()) {
      statusBar()->showMessage(
          tr("Created palette '%1' with %2 colors from image")
              .arg(paletteName)
              .arg(colors.size()),
          3000);
    }
  }
}
