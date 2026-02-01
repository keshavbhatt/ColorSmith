#include "../include/PaletteManager.h"
#include <QDir>
// ReSharper disable once CppUnusedIncludeDirective
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QStandardPaths>

PaletteManager::PaletteManager() : m_currentPalette(nullptr) {
  // Current palette will be determined during loadPalettes
}

PaletteManager::~PaletteManager() { qDeleteAll(m_palettes); }

PaletteManager &PaletteManager::instance() {
  static PaletteManager instance;
  return instance;
}

Palette *PaletteManager::createPalette(const QString &name) {
  Palette *palette = new Palette(name);
  palette->setReadOnly(false);
  m_palettes.append(palette);
  emit paletteAdded(palette);
  return palette;
}

Palette *PaletteManager::getPalette(const QString &id) const {
  for (Palette *palette : m_palettes) {
    if (palette->id() == id) {
      return palette;
    }
  }
  return nullptr;
}

void PaletteManager::setCurrentPalette(const QString &id) {
  Palette *palette = getPalette(id);
  if (palette && palette != m_currentPalette) {
    m_currentPalette = palette;
    emit currentPaletteChanged(palette);
  }
}

void PaletteManager::setCurrentPalette(Palette *palette) {
  if (palette && palette != m_currentPalette && m_palettes.contains(palette)) {
    m_currentPalette = palette;
    emit currentPaletteChanged(palette);
  }
}

bool PaletteManager::deletePalette(const QString &id) {
  for (int i = 0; i < m_palettes.size(); ++i) {
    if (m_palettes[i]->id() == id) {
      Palette *palette = m_palettes[i];

      // Don't delete read-only palettes
      if (palette->isReadOnly()) {
        return false;
      }

      bool wasCurrent = (palette == m_currentPalette);

      m_palettes.remove(i);
      emit paletteRemoved(id);
      delete palette;

      // Set new current if deleted was current
      if (wasCurrent) {
        m_currentPalette = nullptr;

        if (!m_palettes.isEmpty()) {
          // Find first non-readonly palette
          for (Palette *p : m_palettes) {
            if (!p->isReadOnly()) {
              m_currentPalette = p;
              break;
            }
          }

          // If no user palette found, use the first system palette
          if (!m_currentPalette) {
            m_currentPalette = m_palettes[0];
          }

          emit currentPaletteChanged(m_currentPalette);
        }
      }

      savePalettes();
      return true;
    }
  }
  return false;
}

bool PaletteManager::renamePalette(const QString &id, const QString &newName) {
  if (Palette *palette = getPalette(id)) {
    palette->setName(newName);
    emit paletteRenamed(id);
    return true;
  }
  return false;
}

void PaletteManager::savePalettes() {
  QJsonArray palettesArray;

  for (const Palette *palette : m_palettes) {
    // Skip read-only palettes (like standard HTML colors)
    if (palette->isReadOnly())
      continue;

    QJsonObject paletteObj;
    paletteObj["id"] = palette->id();
    paletteObj["name"] = palette->name();

    QJsonArray colorsArray;
    for (const QColor &color : palette->colors()) {
      colorsArray.append(color.name(QColor::HexArgb));
    }
    paletteObj["colors"] = colorsArray;

    palettesArray.append(paletteObj);
  }

  QJsonObject root;
  root["palettes"] = palettesArray;
  root["currentPaletteId"] =
      m_currentPalette ? m_currentPalette->id() : QString();

  const QJsonDocument doc(root);

  const QString dataPath =
      QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  QDir().mkpath(dataPath);

  if (QFile file(dataPath + "/palettes.json");
      file.open(QIODevice::WriteOnly)) {
    file.write(doc.toJson());
    file.close();
  }
}

void PaletteManager::loadPalettes() {
  // Load recently picked colors palette first (read-only)
  loadRecentlyPickedColorsPalette();

  // Load standard HTML colors palette (read-only)
  loadStandardHtmlColorsPalette();

  const QString dataPath =
      QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  QFile file(dataPath + "/palettes.json");

  if (!file.open(QIODevice::ReadOnly)) {
    return;
  }

  const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
  file.close();

  if (!doc.isObject()) {
    return;
  }

  QJsonObject root = doc.object();
  QJsonArray palettesArray = root["palettes"].toArray();
  QString currentPaletteId = root["currentPaletteId"].toString();

  for (const QJsonValue &value : palettesArray) {
    QJsonObject paletteObj = value.toObject();

    QString id = paletteObj["id"].toString();
    QString name = paletteObj["name"].toString();

    Palette *palette = new Palette(name, id);

    QJsonArray colorsArray = paletteObj["colors"].toArray();
    for (const QJsonValue &colorValue : colorsArray) {
      QColor color(colorValue.toString());
      if (color.isValid()) {
        palette->addColor(color);
      }
    }

    m_palettes.append(palette);
  }

  // Restore the last used palette (including system palettes)
  if (!currentPaletteId.isEmpty()) {
    for (Palette *palette : m_palettes) {
      if (palette->id() == currentPaletteId) {
        m_currentPalette = palette;
        break;
      }
    }
  }

  // If no current palette was restored, set a default
  if (!m_currentPalette) {
    if (Palette *recentPalette = getPalette(RECENTLY_PICKED_PALETTE_ID)) {
      // Prefer FIFO recent palette when user previously had no selection
      m_currentPalette = recentPalette;
    } else {
      // Prefer the first non-read-only palette, fall back to the first
      // available
      for (Palette *palette : m_palettes) {
        if (!palette->isReadOnly()) {
          m_currentPalette = palette;
          break;
        }
      }

      if (!m_currentPalette && !m_palettes.isEmpty()) {
        m_currentPalette = m_palettes.first();
      }
    }
  }
}

void PaletteManager::loadStandardHtmlColorsPalette() {
  // Load standard HTML colors from JSON file
  QFile file(":/standard-html-colors.json");

  if (!file.open(QIODevice::ReadOnly)) {
    qWarning() << "Failed to open standard-html-colors.json from resources";
    return;
  }

  QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
  file.close();

  if (!doc.isArray()) {
    qWarning() << "Invalid format in standard-html-colors.json";
    return;
  }

  // Create a read-only palette with fixed ID
  Palette *standardPalette =
      new Palette(tr("Standard HTML Colors"), "standard-html-colors", true);

  QJsonArray colorsArray = doc.array();
  for (const QJsonValue &value : colorsArray) {
    QJsonObject colorObj = value.toObject();
    QString hexColor = colorObj["hex"].toString();
    QString colorName = colorObj["name"].toString();

    if (QColor color(hexColor); color.isValid()) {
      standardPalette->addColor(color, colorName);
    }
  }

  // first palette
  m_palettes.append(standardPalette);
}

void PaletteManager::loadRecentlyPickedColorsPalette() {
  // Create a read-only palette with fixed ID
  Palette *recentPalette =
      new Palette(tr("Recently Picked"), RECENTLY_PICKED_PALETTE_ID, true);

  // Load recent colors from settings
  QString dataPath =
      QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  QFile file(dataPath + "/recent-colors.json");

  if (file.open(QIODevice::ReadOnly)) {
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (doc.isArray()) {
      QJsonArray colorsArray = doc.array();
      for (const QJsonValue &colorValue : colorsArray) {
        QString hexColor = colorValue.toString();
        QColor color(hexColor);
        if (color.isValid()) {
          recentPalette->addColor(color);
        }
      }
    }
  }

  // first palette
  m_palettes.prepend(recentPalette);
}

void PaletteManager::addToRecentColors(const QColor &color) {
  if (!color.isValid())
    return;

  // Find the recently picked palette
  Palette *recentPalette = nullptr;
  for (Palette *palette : m_palettes) {
    if (palette->id() == RECENTLY_PICKED_PALETTE_ID) {
      recentPalette = palette;
      break;
    }
  }

  if (!recentPalette)
    return;

  // Remove the color if it already exists (to move it to the front)
  QVector<QColor> colors = recentPalette->colors();
  for (int i = 0; i < colors.size(); ++i) {
    if (colors[i] == color) {
      recentPalette->removeColor(i);
      break;
    }
  }

  // Get current colors
  colors = recentPalette->colors();

  // Add new color at the beginning
  QVector<QColor> newColors;
  newColors.append(color);
  for (const QColor &c : colors) {
    newColors.append(c);
  }

  // Limit to 24 colors (FIFO)
  const int maxRecentColors = 24;
  if (newColors.size() > maxRecentColors) {
    newColors.resize(maxRecentColors);
  }

  // Update palette
  recentPalette->setColors(newColors);

  // Save recent colors to file
  QString dataPath =
      QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  QDir().mkpath(dataPath);

  QJsonArray colorsArray;
  for (const QColor &c : newColors) {
    colorsArray.append(c.name(QColor::HexArgb));
  }

  QJsonDocument doc(colorsArray);
  QFile file(dataPath + "/recent-colors.json");
  if (file.open(QIODevice::WriteOnly)) {
    file.write(doc.toJson());
    file.close();
  }

  // Notify that colors changed
  emit paletteColorsChanged(recentPalette);
}
