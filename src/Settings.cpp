#include "../include/Settings.h"
#include "../include/version.h"

namespace Settings {

Manager::Manager()
    : m_settings(COLORSMITH_ORGANIZATION_NAME, COLORSMITH_APP_NAME)
{
}

Manager& Manager::instance() {
    static Manager instance;
    return instance;
}

QString Manager::getLastColor() const {
    return m_settings.value(Keys::LAST_COLOR).toString();
}

void Manager::setLastColor(const QString& color) {
    m_settings.setValue(Keys::LAST_COLOR, color);
}

QString Manager::getOutputMode() const {
    return m_settings.value(Keys::OUTPUT_MODE, "hex").toString();
}

void Manager::setOutputMode(const QString& mode) {
    m_settings.setValue(Keys::OUTPUT_MODE, mode);
}

QStringList Manager::getPaletteColors() const {
    return m_settings.value(Keys::PALETTE_COLORS).toStringList();
}

void Manager::setPaletteColors(const QStringList& colors) {
    m_settings.setValue(Keys::PALETTE_COLORS, colors);
}

QString Manager::getCurrentPaletteId() const {
    return m_settings.value(Keys::CURRENT_PALETTE_ID).toString();
}

void Manager::setCurrentPaletteId(const QString& id) {
    m_settings.setValue(Keys::CURRENT_PALETTE_ID, id);
}


QByteArray Manager::getWindowGeometry() const {
    return m_settings.value(Keys::WINDOW_GEOMETRY).toByteArray();
}

void Manager::setWindowGeometry(const QByteArray& geometry) {
    m_settings.setValue(Keys::WINDOW_GEOMETRY, geometry);
}

QByteArray Manager::getWindowState() const {
    return m_settings.value(Keys::WINDOW_STATE).toByteArray();
}

void Manager::setWindowState(const QByteArray& state) {
    m_settings.setValue(Keys::WINDOW_STATE, state);
}

QByteArray Manager::getSplitterState() const {
    return m_settings.value(Keys::SPLITTER_STATE).toByteArray();
}

void Manager::setSplitterState(const QByteArray& state) {
    m_settings.setValue(Keys::SPLITTER_STATE, state);
}

} // namespace Settings
