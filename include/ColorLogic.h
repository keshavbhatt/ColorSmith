#ifndef COLORLOGIC_H
#define COLORLOGIC_H

#include <QString>
#include <QColor>

class ColorLogic {
public:
    // HEX format
    static QString colorToHex(const QColor &color);
    static QColor hexToColor(const QString &hex);

    // RGB format
    static QString colorToRgbString(const QColor &color);
    static QColor rgbStringToColor(const QString &rgbString);

    // RGBA format
    static QString colorToRgbaString(const QColor &color);
    static QColor rgbaStringToColor(const QString &rgbaString);

    // HSL format
    static QString colorToHslString(const QColor &color);
    static QColor hslStringToColor(const QString &hslString);

    // HSLA format
    static QString colorToHslaString(const QColor &color);
    static QColor hslaStringToColor(const QString &hslaString);

    // HSV/HSB format (same thing, different names)
    static QString colorToHsvString(const QColor &color);
    static QColor hsvStringToColor(const QString &hsvString);

    // CMYK format
    static QString colorToCmykString(const QColor &color);
    static QColor cmykStringToColor(const QString &cmykString);

    // Utility
    static QColor randomColor();
};

#endif // COLORLOGIC_H
