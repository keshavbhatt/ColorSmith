#ifndef CONTRASTCHECKER_H
#define CONTRASTCHECKER_H

#include <QDialog>
#include <QColor>

class QLineEdit;
class QLabel;
class QSpinBox;
class QPushButton;
class QFrame;
class HSLGradientSlider;
class ScreenPicker;

class ContrastChecker : public QDialog {
    Q_OBJECT

public:
    explicit ContrastChecker(QWidget *parent = nullptr, const QColor &initialColor = Qt::black);
    ~ContrastChecker() override = default;

    void setTextColor(const QColor &color);
    void setBackgroundColor(const QColor &color);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onTextColorChanged();
    void onBackgroundColorChanged();
    void onTextHslChanged();
    void onBackgroundHslChanged();
    void onPickTextColor();
    void onPickBackgroundColor();
    void onSwapColors();
    void onTextColorPicked(const QColor &color);
    void onBackgroundColorPicked(const QColor &color);
    void onPickerError(const QString &message);
    void updateContrastRatio();

private:
    void setupUI();
    void updateTextPreview();
    void updateBackgroundPreview();
    void updateTextColorFromHex(const QString &hex);
    void updateBackgroundColorFromHex(const QString &hex);
    void updateTextHslSliders();
    void updateBackgroundHslSliders();

    double calculateContrastRatio(const QColor &foreground, const QColor &background);
    double calculateRelativeLuminance(const QColor &color);

    void saveGeometry();
    void loadGeometry();

    // UI Components
    QLineEdit *m_textColorInput;
    QLineEdit *m_backgroundColorInput;
    QFrame *m_textColorPreview;
    QFrame *m_backgroundColorPreview;
    QFrame *m_contrastPreview;

    // Text color HSL sliders
    HSLGradientSlider *m_textHueSlider;
    HSLGradientSlider *m_textSatSlider;
    HSLGradientSlider *m_textLightSlider;
    QSpinBox *m_textHueSpin;
    QSpinBox *m_textSatSpin;
    QSpinBox *m_textLightSpin;

    // Background color HSL sliders
    HSLGradientSlider *m_bgHueSlider;
    HSLGradientSlider *m_bgSatSlider;
    HSLGradientSlider *m_bgLightSlider;
    QSpinBox *m_bgHueSpin;
    QSpinBox *m_bgSatSpin;
    QSpinBox *m_bgLightSpin;

    QPushButton *m_pickTextColorBtn;
    QPushButton *m_pickBgColorBtn;
    QPushButton *m_swapColorsBtn;

    ScreenPicker *m_textScreenPicker;
    ScreenPicker *m_bgScreenPicker;

    // Contrast ratio display
    QLabel *m_ratioLabel;
    QLabel *m_normalTextLabel;
    QLabel *m_largeTextLabel;
    QLabel *m_aaaNormalLabel;
    QLabel *m_aaaLargeLabel;

    // State
    QColor m_textColor;
    QColor m_backgroundColor;
    bool m_programmaticChange;
};

#endif // CONTRASTCHECKER_H
