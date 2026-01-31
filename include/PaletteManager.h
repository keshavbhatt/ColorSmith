#ifndef PALETTEMANAGER_H
#define PALETTEMANAGER_H

#include "Palette.h"
#include <QObject>
#include <QVector>

class PaletteManager : public QObject {
    Q_OBJECT

public:
    // Palette ID constants
    static constexpr auto RECENTLY_PICKED_PALETTE_ID = "recently-picked-colors";

    static PaletteManager& instance();

    Palette* createPalette(const QString &name = "Untitled Palette");
    Palette* getPalette(const QString &id) const;
    Palette* currentPalette() const { return m_currentPalette; }
    void setCurrentPalette(const QString &id);
    void setCurrentPalette(Palette *palette);

    QVector<Palette*> allPalettes() const { return m_palettes; }
    bool deletePalette(const QString &id);
    bool renamePalette(const QString &id, const QString &newName);

    void loadPalettes();
    void savePalettes();

    void addToRecentColors(const QColor &color);

signals:
    void paletteAdded(Palette *palette);
    void paletteRemoved(const QString &id);
    void paletteRenamed(const QString &id);
    void currentPaletteChanged(Palette *palette);
    void paletteColorsChanged(Palette *palette);

private:
    PaletteManager();
    ~PaletteManager();
    PaletteManager(const PaletteManager&) = delete;
    PaletteManager& operator=(const PaletteManager&) = delete;

    void loadStandardHtmlColorsPalette();
    void loadRecentlyPickedColorsPalette();

    QVector<Palette*> m_palettes;
    Palette *m_currentPalette;
};

#endif // PALETTEMANAGER_H
