#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QString>
#include <QColor>

namespace Settings {

// Setting keys as constants
namespace Keys {
    constexpr const char* LAST_COLOR = "last-color";
    constexpr const char* OUTPUT_MODE = "output-mode";
    constexpr const char* WINDOW_GEOMETRY = "window-geometry";
    constexpr const char* WINDOW_STATE = "window-state";
    constexpr const char* PALETTE_COLORS = "palette-colors";
    constexpr const char* CURRENT_PALETTE_ID = "current-palette-id";
    constexpr const char* SPLITTER_STATE = "splitter-state";
}

// Settings manager class
class Manager {
public:
    static Manager& instance();

    // Color settings
    QString getLastColor() const;
    void setLastColor(const QString& color);

    // Output mode settings
    QString getOutputMode() const;
    void setOutputMode(const QString& mode);

    // Palette settings
    QStringList getPaletteColors() const;
    void setPaletteColors(const QStringList& colors);

    QString getCurrentPaletteId() const;
    void setCurrentPaletteId(const QString& id);


    // Window settings
    QByteArray getWindowGeometry() const;
    void setWindowGeometry(const QByteArray& geometry);

    QByteArray getWindowState() const;
    void setWindowState(const QByteArray& state);

    QByteArray getSplitterState() const;
    void setSplitterState(const QByteArray& state);

private:
    Manager();
    ~Manager() = default;

    // Delete copy constructor and assignment operator
    Manager(const Manager&) = delete;
    Manager& operator=(const Manager&) = delete;

    QSettings m_settings;
};

} // namespace Settings

#endif // SETTINGS_H
