#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QColor>

class QSpinBox;
class QSplitter;

namespace Ui {
class MainWindow;
}

class ScreenPicker;
class PaletteWidget;
class ColorPreviewWidget;
class QtHsvRectPicker;
class BrightnessSliderWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onRgbInputChanged();
    void onOutputModeChanged() const;
    void onOutputTextChanged();
    void onCopyClicked() const;
    void onPickColorClicked() const;
    void onRandomColorClicked();
    void onAboutClicked();
    void onBrightnessColorChanged(const QColor &color);
    void onResetBrightness();
    void onGradientClicked();
    void onContrastCheckerClicked();
    void onKeyboardShortcutsClicked();

    // Connected to ScreenPicker
    void onColorPicked(const QColor &color);
    void onPickerError(const QString &message) const;

    // Connected to PaletteWidget
    void onPaletteColorSelected(const QColor &color);
    void onAddToPaletteClicked() const;

    // Connected to QtHsvRectPicker
    void onColorPlaneColorChanged(const QColor &color);

private:
    void updateColor(const QColor &color, bool updateOutput = true, bool skipBrightnessReset = false, bool addToRecent = false);
    void updateOutput() const;
    void loadSettings();
    void saveSettings() const;
    static bool handleSpinBoxKeyPress(QSpinBox* spinBox, const QKeyEvent* event);

    Ui::MainWindow *ui;
    ScreenPicker *m_screenPicker;
    PaletteWidget *m_paletteWidget;
    ColorPreviewWidget *m_colorPreview;
    ColorPreviewWidget *m_lastColorPreview;
    QtHsvRectPicker *m_colorPlane;
    BrightnessSliderWidget *m_brightnessSlider;
    QSplitter *m_splitter;

    QColor m_currentColor;
    QColor m_baseColorForBrightness;
    bool m_programmaticChange;
};

#endif // MAINWINDOW_H
