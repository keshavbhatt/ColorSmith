#include "../include/ColorLogic.h"
#include <QRandomGenerator>
#include <QRegularExpression>
#include <QtMath>

// HEX format
QString ColorLogic::colorToHex(const QColor &color) {
    // Include alpha if not fully opaque
    if (color.alpha() < 255) {
        return color.name(QColor::HexArgb);
    }
    return color.name(QColor::HexRgb);
}

QColor ColorLogic::hexToColor(const QString &hex) {
    QColor color(hex);
    if (!hex.startsWith("#")) {
        color = QColor("#" + hex);
    }
    return color;
}

// RGB format
QString ColorLogic::colorToRgbString(const QColor &color) {
    return QString("rgb(%1, %2, %3)")
            .arg(color.red())
            .arg(color.green())
            .arg(color.blue());
}

QColor ColorLogic::rgbStringToColor(const QString &rgbString) {
    static QRegularExpression re("rgb\\(\\s*(\\d+)\\s*,\\s*(\\d+)\\s*,\\s*(\\d+)\\s*\\)");
    QRegularExpressionMatch match = re.match(rgbString);
    if (match.hasMatch()) {
        int r = match.captured(1).toInt();
        int g = match.captured(2).toInt();
        int b = match.captured(3).toInt();
        return QColor(r, g, b);
    }
    return QColor();
}

// RGBA format
QString ColorLogic::colorToRgbaString(const QColor &color) {
    return QString("rgba(%1, %2, %3, %4)")
            .arg(color.red())
            .arg(color.green())
            .arg(color.blue())
            .arg(color.alphaF(), 0, 'f', 2);
}

QColor ColorLogic::rgbaStringToColor(const QString &rgbaString) {
    static QRegularExpression re("rgba\\(\\s*(\\d+)\\s*,\\s*(\\d+)\\s*,\\s*(\\d+)\\s*,\\s*([0-9]*\\.?[0-9]+)\\s*\\)");
    QRegularExpressionMatch match = re.match(rgbaString);
    if (match.hasMatch()) {
        int r = match.captured(1).toInt();
        int g = match.captured(2).toInt();
        int b = match.captured(3).toInt();
        qreal a = match.captured(4).toDouble();
        return QColor::fromRgbF(r / 255.0, g / 255.0, b / 255.0, a);
    }
    return QColor();
}

// HSL format
QString ColorLogic::colorToHslString(const QColor &color) {
    return QString("hsl(%1, %2%, %3%)")
            .arg(color.hslHue() == -1 ? 0 : color.hslHue())
            .arg(qRound(color.hslSaturationF() * 100))
            .arg(qRound(color.lightnessF() * 100));
}

QColor ColorLogic::hslStringToColor(const QString &hslString) {
    static QRegularExpression re("hsl\\(\\s*(\\d+)\\s*,\\s*(\\d+)%?\\s*,\\s*(\\d+)%?\\s*\\)");
    QRegularExpressionMatch match = re.match(hslString);
    if (match.hasMatch()) {
        int h = match.captured(1).toInt();
        int s = match.captured(2).toInt();
        int l = match.captured(3).toInt();
        return QColor::fromHsl(h, qRound(s * 2.55), qRound(l * 2.55));
    }
    return QColor();
}

// HSLA format
QString ColorLogic::colorToHslaString(const QColor &color) {
    return QString("hsla(%1, %2%, %3%, %4)")
            .arg(color.hslHue() == -1 ? 0 : color.hslHue())
            .arg(qRound(color.hslSaturationF() * 100))
            .arg(qRound(color.lightnessF() * 100))
            .arg(color.alphaF(), 0, 'f', 2);
}

QColor ColorLogic::hslaStringToColor(const QString &hslaString) {
    static QRegularExpression re("hsla\\(\\s*(\\d+)\\s*,\\s*(\\d+)%?\\s*,\\s*(\\d+)%?\\s*,\\s*([0-9]*\\.?[0-9]+)\\s*\\)");
    QRegularExpressionMatch match = re.match(hslaString);
    if (match.hasMatch()) {
        int h = match.captured(1).toInt();
        int s = match.captured(2).toInt();
        int l = match.captured(3).toInt();
        qreal a = match.captured(4).toDouble();
        QColor color = QColor::fromHsl(h, qRound(s * 2.55), qRound(l * 2.55));
        color.setAlphaF(a);
        return color;
    }
    return QColor();
}

// HSV/HSB format
QString ColorLogic::colorToHsvString(const QColor &color) {
    return QString("hsv(%1, %2%, %3%)")
            .arg(color.hsvHue() == -1 ? 0 : color.hsvHue())
            .arg(qRound(color.hsvSaturationF() * 100))
            .arg(qRound(color.valueF() * 100));
}

QColor ColorLogic::hsvStringToColor(const QString &hsvString) {
    static QRegularExpression re("hsv\\(\\s*(\\d+)\\s*,\\s*(\\d+)%?\\s*,\\s*(\\d+)%?\\s*\\)");
    QRegularExpressionMatch match = re.match(hsvString);
    if (match.hasMatch()) {
        int h = match.captured(1).toInt();
        int s = match.captured(2).toInt();
        int v = match.captured(3).toInt();
        return QColor::fromHsv(h, qRound(s * 2.55), qRound(v * 2.55));
    }
    return QColor();
}

// CMYK format
QString ColorLogic::colorToCmykString(const QColor &color) {
    return QString("cmyk(%1%, %2%, %3%, %4%)")
            .arg(qRound(color.cyanF() * 100))
            .arg(qRound(color.magentaF() * 100))
            .arg(qRound(color.yellowF() * 100))
            .arg(qRound(color.blackF() * 100));
}

QColor ColorLogic::cmykStringToColor(const QString &cmykString) {
    static QRegularExpression re("cmyk\\(\\s*(\\d+)%?\\s*,\\s*(\\d+)%?\\s*,\\s*(\\d+)%?\\s*,\\s*(\\d+)%?\\s*\\)");
    QRegularExpressionMatch match = re.match(cmykString);
    if (match.hasMatch()) {
        int c = match.captured(1).toInt();
        int m = match.captured(2).toInt();
        int y = match.captured(3).toInt();
        int k = match.captured(4).toInt();
        return QColor::fromCmyk(qRound(c * 2.55), qRound(m * 2.55), qRound(y * 2.55), qRound(k * 2.55));
    }
    return QColor();
}

// Utility
QColor ColorLogic::randomColor() {
    return QColor(
        QRandomGenerator::global()->bounded(256),
        QRandomGenerator::global()->bounded(256),
        QRandomGenerator::global()->bounded(256)
    );
}
