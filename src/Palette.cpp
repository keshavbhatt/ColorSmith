#include "../include/Palette.h"

Palette::Palette(const QString &name, const QString &id, bool isReadOnly)
    : m_id(id.isEmpty() ? QUuid::createUuid().toString(QUuid::WithoutBraces)
                        : id),
      m_name(name), m_isReadOnly(isReadOnly) {}

void Palette::addColor(const QColor &color, const QString &colorName) {
  if (color.isValid()) {
    m_colors.append(color);
    m_colorNames.append(colorName);
  }
}

void Palette::setColors(const QVector<QColor> &colors) {
  m_colors = colors;
  // Clear color names and resize to match colors (with empty names)
  m_colorNames.clear();
  m_colorNames.resize(colors.size());
}

void Palette::removeColor(int index) {
  if (index >= 0 && index < m_colors.size()) {
    m_colors.remove(index);
    // Only remove from color names if the index exists there
    if (index < m_colorNames.size()) {
      m_colorNames.remove(index);
    }
  }
}

void Palette::clearColors() {
  m_colors.clear();
  m_colorNames.clear();
}

QString Palette::colorName(int index) const {
  if (index >= 0 && index < m_colorNames.size()) {
    return m_colorNames[index];
  }
  return QString();
}
