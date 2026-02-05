#include "../include/ContrastChecker.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QFrame>
#include <QGroupBox>
#include <QSettings>
#include <QPainter>
#include <QtMath>

ContrastChecker::ContrastChecker(QWidget *parent, const QColor &initialColor)
    : QDialog(parent), m_textColor(Qt::black), m_backgroundColor(Qt::white),
      m_programmaticChange(false) {
    setWindowTitle(tr("Color Contrast Checker"));
    setMinimumSize(700, 650);
    resize(750, 700);

    setupUI();

    // Set initial colors
    if (initialColor.isValid()) {
        setTextColor(initialColor);
    }

    updateContrastRatio();
    loadGeometry();
}

void ContrastChecker::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Title
    QLabel *titleLabel = new QLabel(tr("Color Contrast Checker"), this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(titleFont.pointSize() + 4);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // Description
    QLabel *descLabel = new QLabel(
        tr("Check the contrast ratio between text and background colors according to WCAG guidelines."),
        this);
    descLabel->setWordWrap(true);
    descLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(descLabel);

    // Create horizontal layout for left (controls) and right (results)
    QHBoxLayout *contentLayout = new QHBoxLayout();

    // Left side - Color controls
    QVBoxLayout *leftLayout = new QVBoxLayout();

    // Text Color Section
    QGroupBox *textColorGroup = new QGroupBox(tr("Text Color"), this);
    QVBoxLayout *textColorLayout = new QVBoxLayout(textColorGroup);

    QHBoxLayout *textInputLayout = new QHBoxLayout();
    m_textColorInput = new QLineEdit(this);
    m_textColorInput->setPlaceholderText(tr("#000000"));
    connect(m_textColorInput, &QLineEdit::textChanged, this, &ContrastChecker::onTextColorChanged);

    m_textColorPreview = new QFrame(this);
    m_textColorPreview->setFixedSize(50, 30);
    m_textColorPreview->setFrameShape(QFrame::StyledPanel);

    m_pickTextColorBtn = new QPushButton(tr("Pick"), this);
    connect(m_pickTextColorBtn, &QPushButton::clicked, this, &ContrastChecker::onPickTextColor);

    textInputLayout->addWidget(new QLabel(tr("HEX:"), this));
    textInputLayout->addWidget(m_textColorInput);
    textInputLayout->addWidget(m_textColorPreview);
    textInputLayout->addWidget(m_pickTextColorBtn);
    textColorLayout->addLayout(textInputLayout);

    // Text color HSL sliders
    QGridLayout *textHslLayout = new QGridLayout();

    m_textHueSlider = new QSlider(Qt::Horizontal, this);
    m_textHueSlider->setRange(0, 359);
    m_textHueSpin = new QSpinBox(this);
    m_textHueSpin->setRange(0, 359);
    connect(m_textHueSlider, &QSlider::valueChanged, m_textHueSpin, &QSpinBox::setValue);
    connect(m_textHueSpin, QOverload<int>::of(&QSpinBox::valueChanged), m_textHueSlider, &QSlider::setValue);
    connect(m_textHueSlider, &QSlider::valueChanged, this, &ContrastChecker::onTextHslChanged);

    m_textSatSlider = new QSlider(Qt::Horizontal, this);
    m_textSatSlider->setRange(0, 100);
    m_textSatSpin = new QSpinBox(this);
    m_textSatSpin->setRange(0, 100);
    connect(m_textSatSlider, &QSlider::valueChanged, m_textSatSpin, &QSpinBox::setValue);
    connect(m_textSatSpin, QOverload<int>::of(&QSpinBox::valueChanged), m_textSatSlider, &QSlider::setValue);
    connect(m_textSatSlider, &QSlider::valueChanged, this, &ContrastChecker::onTextHslChanged);

    m_textLightSlider = new QSlider(Qt::Horizontal, this);
    m_textLightSlider->setRange(0, 100);
    m_textLightSpin = new QSpinBox(this);
    m_textLightSpin->setRange(0, 100);
    connect(m_textLightSlider, &QSlider::valueChanged, m_textLightSpin, &QSpinBox::setValue);
    connect(m_textLightSpin, QOverload<int>::of(&QSpinBox::valueChanged), m_textLightSlider, &QSlider::setValue);
    connect(m_textLightSlider, &QSlider::valueChanged, this, &ContrastChecker::onTextHslChanged);

    textHslLayout->addWidget(new QLabel(tr("Hue:"), this), 0, 0);
    textHslLayout->addWidget(m_textHueSlider, 0, 1);
    textHslLayout->addWidget(m_textHueSpin, 0, 2);
    textHslLayout->addWidget(new QLabel(tr("Saturation:"), this), 1, 0);
    textHslLayout->addWidget(m_textSatSlider, 1, 1);
    textHslLayout->addWidget(m_textSatSpin, 1, 2);
    textHslLayout->addWidget(new QLabel(tr("Lightness:"), this), 2, 0);
    textHslLayout->addWidget(m_textLightSlider, 2, 1);
    textHslLayout->addWidget(m_textLightSpin, 2, 2);

    textColorLayout->addLayout(textHslLayout);
    leftLayout->addWidget(textColorGroup);

    // Swap button
    m_swapColorsBtn = new QPushButton(tr("⇅ Swap Colors"), this);
    connect(m_swapColorsBtn, &QPushButton::clicked, this, &ContrastChecker::onSwapColors);
    leftLayout->addWidget(m_swapColorsBtn);

    // Background Color Section
    QGroupBox *bgColorGroup = new QGroupBox(tr("Background Color"), this);
    QVBoxLayout *bgColorLayout = new QVBoxLayout(bgColorGroup);

    QHBoxLayout *bgInputLayout = new QHBoxLayout();
    m_backgroundColorInput = new QLineEdit(this);
    m_backgroundColorInput->setPlaceholderText(tr("#FFFFFF"));
    connect(m_backgroundColorInput, &QLineEdit::textChanged, this, &ContrastChecker::onBackgroundColorChanged);

    m_backgroundColorPreview = new QFrame(this);
    m_backgroundColorPreview->setFixedSize(50, 30);
    m_backgroundColorPreview->setFrameShape(QFrame::StyledPanel);

    m_pickBgColorBtn = new QPushButton(tr("Pick"), this);
    connect(m_pickBgColorBtn, &QPushButton::clicked, this, &ContrastChecker::onPickBackgroundColor);

    bgInputLayout->addWidget(new QLabel(tr("HEX:"), this));
    bgInputLayout->addWidget(m_backgroundColorInput);
    bgInputLayout->addWidget(m_backgroundColorPreview);
    bgInputLayout->addWidget(m_pickBgColorBtn);
    bgColorLayout->addLayout(bgInputLayout);

    // Background color HSL sliders
    QGridLayout *bgHslLayout = new QGridLayout();

    m_bgHueSlider = new QSlider(Qt::Horizontal, this);
    m_bgHueSlider->setRange(0, 359);
    m_bgHueSpin = new QSpinBox(this);
    m_bgHueSpin->setRange(0, 359);
    connect(m_bgHueSlider, &QSlider::valueChanged, m_bgHueSpin, &QSpinBox::setValue);
    connect(m_bgHueSpin, QOverload<int>::of(&QSpinBox::valueChanged), m_bgHueSlider, &QSlider::setValue);
    connect(m_bgHueSlider, &QSlider::valueChanged, this, &ContrastChecker::onBackgroundHslChanged);

    m_bgSatSlider = new QSlider(Qt::Horizontal, this);
    m_bgSatSlider->setRange(0, 100);
    m_bgSatSpin = new QSpinBox(this);
    m_bgSatSpin->setRange(0, 100);
    connect(m_bgSatSlider, &QSlider::valueChanged, m_bgSatSpin, &QSpinBox::setValue);
    connect(m_bgSatSpin, QOverload<int>::of(&QSpinBox::valueChanged), m_bgSatSlider, &QSlider::setValue);
    connect(m_bgSatSlider, &QSlider::valueChanged, this, &ContrastChecker::onBackgroundHslChanged);

    m_bgLightSlider = new QSlider(Qt::Horizontal, this);
    m_bgLightSlider->setRange(0, 100);
    m_bgLightSpin = new QSpinBox(this);
    m_bgLightSpin->setRange(0, 100);
    connect(m_bgLightSlider, &QSlider::valueChanged, m_bgLightSpin, &QSpinBox::setValue);
    connect(m_bgLightSpin, QOverload<int>::of(&QSpinBox::valueChanged), m_bgLightSlider, &QSlider::setValue);
    connect(m_bgLightSlider, &QSlider::valueChanged, this, &ContrastChecker::onBackgroundHslChanged);

    bgHslLayout->addWidget(new QLabel(tr("Hue:"), this), 0, 0);
    bgHslLayout->addWidget(m_bgHueSlider, 0, 1);
    bgHslLayout->addWidget(m_bgHueSpin, 0, 2);
    bgHslLayout->addWidget(new QLabel(tr("Saturation:"), this), 1, 0);
    bgHslLayout->addWidget(m_bgSatSlider, 1, 1);
    bgHslLayout->addWidget(m_bgSatSpin, 1, 2);
    bgHslLayout->addWidget(new QLabel(tr("Lightness:"), this), 2, 0);
    bgHslLayout->addWidget(m_bgLightSlider, 2, 1);
    bgHslLayout->addWidget(m_bgLightSpin, 2, 2);

    bgColorLayout->addLayout(bgHslLayout);
    leftLayout->addWidget(bgColorGroup);
    leftLayout->addStretch();

    // Right side - Contrast Results
    QVBoxLayout *rightLayout = new QVBoxLayout();

    // Contrast Result Section
    QGroupBox *resultGroup = new QGroupBox(tr("Contrast Ratio"), this);
    resultGroup->setMinimumWidth(300);
    QVBoxLayout *resultLayout = new QVBoxLayout(resultGroup);

    m_ratioLabel = new QLabel(tr("0:1"), this);
    QFont ratioFont = m_ratioLabel->font();
    ratioFont.setPointSize(ratioFont.pointSize() + 8);
    ratioFont.setBold(true);
    m_ratioLabel->setFont(ratioFont);
    m_ratioLabel->setAlignment(Qt::AlignCenter);
    resultLayout->addWidget(m_ratioLabel);

    // Preview
    m_contrastPreview = new QFrame(this);
    m_contrastPreview->setMinimumHeight(80);
    m_contrastPreview->setFrameShape(QFrame::StyledPanel);
    resultLayout->addWidget(m_contrastPreview);

    // Compliance labels
    QGridLayout *complianceLayout = new QGridLayout();
    complianceLayout->addWidget(new QLabel(tr("<b>WCAG AA</b>"), this), 0, 0);
    complianceLayout->addWidget(new QLabel(tr("<b>WCAG AAA</b>"), this), 0, 1);

    complianceLayout->addWidget(new QLabel(tr("Normal Text (4.5:1):"), this), 1, 0);
    m_normalTextLabel = new QLabel(tr("❌ Fail"), this);
    complianceLayout->addWidget(m_normalTextLabel, 1, 1);

    complianceLayout->addWidget(new QLabel(tr("Large Text (3:1):"), this), 2, 0);
    m_largeTextLabel = new QLabel(tr("❌ Fail"), this);
    complianceLayout->addWidget(m_largeTextLabel, 2, 1);

    complianceLayout->addWidget(new QLabel(tr("Normal Text (7:1):"), this), 3, 0);
    m_aaaNormalLabel = new QLabel(tr("❌ Fail"), this);
    complianceLayout->addWidget(m_aaaNormalLabel, 3, 1);

    complianceLayout->addWidget(new QLabel(tr("Large Text (4.5:1):"), this), 4, 0);
    m_aaaLargeLabel = new QLabel(tr("❌ Fail"), this);
    complianceLayout->addWidget(m_aaaLargeLabel, 4, 1);

    resultLayout->addLayout(complianceLayout);
    rightLayout->addWidget(resultGroup);
    rightLayout->addStretch();

    // Add left and right layouts to content layout
    contentLayout->addLayout(leftLayout, 1);
    contentLayout->addLayout(rightLayout, 1);
    mainLayout->addLayout(contentLayout);

    // Close button
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    QPushButton *closeButton = new QPushButton(tr("Close"), this);
    closeButton->setMinimumWidth(100);
    closeButton->setDefault(true);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    buttonLayout->addWidget(closeButton);
    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
}

void ContrastChecker::setTextColor(const QColor &color) {
    if (!color.isValid()) return;

    m_textColor = color;
    m_programmaticChange = true;
    m_textColorInput->setText(color.name().toUpper());
    updateTextPreview();
    updateTextHslSliders();
    m_programmaticChange = false;
    updateContrastRatio();
}

void ContrastChecker::setBackgroundColor(const QColor &color) {
    if (!color.isValid()) return;

    m_backgroundColor = color;
    m_programmaticChange = true;
    m_backgroundColorInput->setText(color.name().toUpper());
    updateBackgroundPreview();
    updateBackgroundHslSliders();
    m_programmaticChange = false;
    updateContrastRatio();
}

void ContrastChecker::onTextColorChanged() {
    if (m_programmaticChange) return;
    updateTextColorFromHex(m_textColorInput->text());
}

void ContrastChecker::onBackgroundColorChanged() {
    if (m_programmaticChange) return;
    updateBackgroundColorFromHex(m_backgroundColorInput->text());
}

void ContrastChecker::onTextHslChanged() {
    if (m_programmaticChange) return;

    int h = m_textHueSlider->value();
    int s = m_textSatSlider->value();
    int l = m_textLightSlider->value();

    m_textColor = QColor::fromHsl(h, (s * 255) / 100, (l * 255) / 100);

    m_programmaticChange = true;
    m_textColorInput->setText(m_textColor.name().toUpper());
    updateTextPreview();
    m_programmaticChange = false;

    updateContrastRatio();
}

void ContrastChecker::onBackgroundHslChanged() {
    if (m_programmaticChange) return;

    int h = m_bgHueSlider->value();
    int s = m_bgSatSlider->value();
    int l = m_bgLightSlider->value();

    m_backgroundColor = QColor::fromHsl(h, (s * 255) / 100, (l * 255) / 100);

    m_programmaticChange = true;
    m_backgroundColorInput->setText(m_backgroundColor.name().toUpper());
    updateBackgroundPreview();
    m_programmaticChange = false;

    updateContrastRatio();
}

void ContrastChecker::onPickTextColor() {
    // This would integrate with MainWindow's current color
    // For now, we'll just show a message
    // In full integration, we'd emit a signal or use a callback
}

void ContrastChecker::onPickBackgroundColor() {
    // This would integrate with MainWindow's current color
}

void ContrastChecker::onSwapColors() {
    QColor temp = m_textColor;
    setTextColor(m_backgroundColor);
    setBackgroundColor(temp);
}

void ContrastChecker::updateTextPreview() {
    QString style = QString("QFrame { background-color: %1; }").arg(m_textColor.name());
    m_textColorPreview->setStyleSheet(style);
}

void ContrastChecker::updateBackgroundPreview() {
    QString style = QString("QFrame { background-color: %1; }").arg(m_backgroundColor.name());
    m_backgroundColorPreview->setStyleSheet(style);
}

void ContrastChecker::updateTextColorFromHex(const QString &hex) {
    QColor color(hex);
    if (color.isValid()) {
        m_textColor = color;
        m_programmaticChange = true;
        updateTextPreview();
        updateTextHslSliders();
        m_programmaticChange = false;
        updateContrastRatio();
    }
}

void ContrastChecker::updateBackgroundColorFromHex(const QString &hex) {
    QColor color(hex);
    if (color.isValid()) {
        m_backgroundColor = color;
        m_programmaticChange = true;
        updateBackgroundPreview();
        updateBackgroundHslSliders();
        m_programmaticChange = false;
        updateContrastRatio();
    }
}

void ContrastChecker::updateTextHslSliders() {
    int h, s, l;
    m_textColor.getHsl(&h, &s, &l);

    m_programmaticChange = true;
    m_textHueSlider->setValue(h);
    m_textSatSlider->setValue((s * 100) / 255);
    m_textLightSlider->setValue((l * 100) / 255);
    m_programmaticChange = false;
}

void ContrastChecker::updateBackgroundHslSliders() {
    int h, s, l;
    m_backgroundColor.getHsl(&h, &s, &l);

    m_programmaticChange = true;
    m_bgHueSlider->setValue(h);
    m_bgSatSlider->setValue((s * 100) / 255);
    m_bgLightSlider->setValue((l * 100) / 255);
    m_programmaticChange = false;
}

double ContrastChecker::calculateRelativeLuminance(const QColor &color) {
    // Convert RGB to sRGB
    double r = color.redF();
    double g = color.greenF();
    double b = color.blueF();

    // Apply gamma correction
    auto toLinear = [](double val) {
        return (val <= 0.03928) ? val / 12.92 : qPow((val + 0.055) / 1.055, 2.4);
    };

    r = toLinear(r);
    g = toLinear(g);
    b = toLinear(b);

    // Calculate relative luminance
    return 0.2126 * r + 0.7152 * g + 0.0722 * b;
}

double ContrastChecker::calculateContrastRatio(const QColor &foreground, const QColor &background) {
    double l1 = calculateRelativeLuminance(foreground);
    double l2 = calculateRelativeLuminance(background);

    // Ensure l1 is the lighter color
    if (l1 < l2) {
        qSwap(l1, l2);
    }

    return (l1 + 0.05) / (l2 + 0.05);
}

void ContrastChecker::updateContrastRatio() {
    double ratio = calculateContrastRatio(m_textColor, m_backgroundColor);

    // Update ratio label
    m_ratioLabel->setText(QString::number(ratio, 'f', 2) + ":1");

    // Update preview
    QString previewStyle = QString(
        "QFrame { background-color: %1; color: %2; padding: 10px; }"
    ).arg(m_backgroundColor.name(), m_textColor.name());
    m_contrastPreview->setStyleSheet(previewStyle);

    // Create a custom widget to draw text in the preview
    class PreviewLabel : public QLabel {
    public:
        PreviewLabel(const QString &text, QWidget *parent = nullptr) : QLabel(text, parent) {
            setAlignment(Qt::AlignCenter);
            setWordWrap(true);
        }
    };

    // Clear existing preview content
    QLayoutItem *item;
    if (m_contrastPreview->layout()) {
        while ((item = m_contrastPreview->layout()->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        delete m_contrastPreview->layout();
    }

    QVBoxLayout *previewLayout = new QVBoxLayout(m_contrastPreview);
    PreviewLabel *normalLabel = new PreviewLabel(tr("Normal Text (12pt)"), m_contrastPreview);
    QFont normalFont = normalLabel->font();
    normalFont.setPointSize(12);
    normalLabel->setFont(normalFont);
    normalLabel->setStyleSheet(QString("color: %1;").arg(m_textColor.name()));
    previewLayout->addWidget(normalLabel);

    PreviewLabel *largeLabel = new PreviewLabel(tr("Large Text (18pt)"), m_contrastPreview);
    QFont largeFont = largeLabel->font();
    largeFont.setPointSize(18);
    largeLabel->setFont(largeFont);
    largeLabel->setStyleSheet(QString("color: %1;").arg(m_textColor.name()));
    previewLayout->addWidget(largeLabel);

    // Check WCAG AA compliance
    bool normalAA = ratio >= 4.5;
    bool largeAA = ratio >= 3.0;

    m_normalTextLabel->setText(normalAA ? tr("✓ Pass") : tr("❌ Fail"));
    m_normalTextLabel->setStyleSheet(normalAA ? "color: green; font-weight: bold;" : "color: red;");

    m_largeTextLabel->setText(largeAA ? tr("✓ Pass") : tr("❌ Fail"));
    m_largeTextLabel->setStyleSheet(largeAA ? "color: green; font-weight: bold;" : "color: red;");

    // Check WCAG AAA compliance
    bool normalAAA = ratio >= 7.0;
    bool largeAAA = ratio >= 4.5;

    m_aaaNormalLabel->setText(normalAAA ? tr("✓ Pass") : tr("❌ Fail"));
    m_aaaNormalLabel->setStyleSheet(normalAAA ? "color: green; font-weight: bold;" : "color: red;");

    m_aaaLargeLabel->setText(largeAAA ? tr("✓ Pass") : tr("❌ Fail"));
    m_aaaLargeLabel->setStyleSheet(largeAAA ? "color: green; font-weight: bold;" : "color: red;");
}

void ContrastChecker::saveGeometry() {
    QSettings settings("KTechpit", "ColorSmith");
    settings.setValue("ContrastChecker/geometry", QDialog::saveGeometry());
}

void ContrastChecker::loadGeometry() {
    QSettings settings("KTechpit", "ColorSmith");
    const QByteArray geometry = settings.value("ContrastChecker/geometry").toByteArray();
    if (!geometry.isEmpty()) {
        restoreGeometry(geometry);
    }
}

void ContrastChecker::closeEvent(QCloseEvent *event) {
    saveGeometry();
    QDialog::closeEvent(event);
}
