#ifndef PALETTE_H
#define PALETTE_H

#include <QColor>
#include <QString>
#include <QVector>
#include <QUuid>

class Palette {
public:
    explicit Palette(const QString &name = "Untitled Palette", const QString &id = QString(), bool isReadOnly = false);

    QString id() const { return m_id; }
    QString name() const { return m_name; }
    void setName(const QString &name) { m_name = name; }

    bool isReadOnly() const { return m_isReadOnly; }
    void setReadOnly(bool readOnly) { m_isReadOnly = readOnly; }

    QVector<QColor> colors() const { return m_colors; }
    void setColors(const QVector<QColor> &colors);
    void addColor(const QColor &color, const QString &colorName = QString());
    void removeColor(int index);
    void clearColors();
    int colorCount() const { return m_colors.size(); }
    QString colorName(int index) const;

private:
    QString m_id;
    QString m_name;
    QVector<QColor> m_colors;
    QVector<QString> m_colorNames;
    bool m_isReadOnly;
};

#endif // PALETTE_H
