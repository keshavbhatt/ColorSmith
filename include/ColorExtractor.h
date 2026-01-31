#ifndef COLOREXTRACTOR_H
#define COLOREXTRACTOR_H

#include <QColor>
#include <QImage>
#include <QVector>

class ColorExtractor {
public:
    // Extract dominant colors from an image using K-means clustering
    static QVector<QColor> extractDominantColors(const QImage &image, int colorCount);

private:
    // Helper struct for color frequency tracking
    struct ColorFrequency {
        QColor color;
        int frequency;
    };

    // K-means clustering implementation
    static QVector<QRgb> performKMeansClustering(const QVector<QRgb> &pixels, int clusterCount);

    // Calculate Euclidean distance in RGB space
    static int calculateColorDistance(QRgb color1, QRgb color2);

    // Sort colors by frequency
    static QVector<QColor> sortColorsByFrequency(const QVector<QRgb> &centroids,
                                                   const QVector<QRgb> &pixels);
};

#endif // COLOREXTRACTOR_H
