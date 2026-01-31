#include "../include/ColorExtractor.h"
#include <algorithm>
#include <climits>

QVector<QColor> ColorExtractor::extractDominantColors(const QImage &image, int colorCount) {
    if (image.isNull() || colorCount <= 0)
        return QVector<QColor>();

    // Scale down image for faster processing
    QImage scaledImage = image;
    const int maxDimension = 200;
    if (image.width() > maxDimension || image.height() > maxDimension) {
        scaledImage = image.scaled(maxDimension, maxDimension,
                                    Qt::KeepAspectRatio, Qt::FastTransformation);
    }

    // Convert to RGB32 format for easier processing
    scaledImage = scaledImage.convertToFormat(QImage::Format_RGB32);

    // Collect all pixels
    QVector<QRgb> pixels;
    pixels.reserve(scaledImage.width() * scaledImage.height());

    for (int y = 0; y < scaledImage.height(); ++y) {
        const QRgb *line = reinterpret_cast<const QRgb *>(scaledImage.constScanLine(y));
        for (int x = 0; x < scaledImage.width(); ++x) {
            pixels.append(line[x]);
        }
    }

    // Perform K-means clustering
    QVector<QRgb> centroids = performKMeansClustering(pixels, colorCount);

    // Sort colors by frequency
    return sortColorsByFrequency(centroids, pixels);
}

QVector<QRgb> ColorExtractor::performKMeansClustering(const QVector<QRgb> &pixels, int clusterCount) {
    QVector<QRgb> centroids;

    // Initialize centroids with evenly spaced colors from the image
    int step = pixels.size() / clusterCount;
    for (int i = 0; i < clusterCount && i * step < pixels.size(); ++i) {
        centroids.append(pixels[i * step]);
    }

    // K-means iterations
    const int maxIterations = 10;
    for (int iter = 0; iter < maxIterations; ++iter) {
        // Assign pixels to nearest centroid
        QVector<QVector<QRgb>> clusters(clusterCount);

        for (const QRgb &pixel : pixels) {
            int nearestCentroid = 0;
            int minDistance = INT_MAX;

            for (int i = 0; i < centroids.size(); ++i) {
                int distance = calculateColorDistance(pixel, centroids[i]);

                if (distance < minDistance) {
                    minDistance = distance;
                    nearestCentroid = i;
                }
            }

            clusters[nearestCentroid].append(pixel);
        }

        // Update centroids
        bool changed = false;
        for (int i = 0; i < clusterCount; ++i) {
            if (clusters[i].isEmpty())
                continue;

            long long sumR = 0, sumG = 0, sumB = 0;
            for (const QRgb &pixel : clusters[i]) {
                sumR += qRed(pixel);
                sumG += qGreen(pixel);
                sumB += qBlue(pixel);
            }

            int count = clusters[i].size();
            QRgb newCentroid = qRgb(sumR / count, sumG / count, sumB / count);

            if (newCentroid != centroids[i]) {
                centroids[i] = newCentroid;
                changed = true;
            }
        }

        if (!changed)
            break;
    }

    return centroids;
}

int ColorExtractor::calculateColorDistance(QRgb color1, QRgb color2) {
    int dr = qRed(color1) - qRed(color2);
    int dg = qGreen(color1) - qGreen(color2);
    int db = qBlue(color1) - qBlue(color2);
    return dr * dr + dg * dg + db * db;
}

QVector<QColor> ColorExtractor::sortColorsByFrequency(const QVector<QRgb> &centroids,
                                                       const QVector<QRgb> &pixels) {
    QVector<ColorFrequency> colorFreqs;

    for (const QRgb &centroid : centroids) {
        ColorFrequency cf;
        cf.color = QColor(centroid);
        cf.frequency = 0;

        // Count how many pixels are closest to this centroid
        for (const QRgb &pixel : pixels) {
            int distance = calculateColorDistance(pixel, centroid);

            bool isClosest = true;
            for (const QRgb &otherCentroid : centroids) {
                if (otherCentroid == centroid)
                    continue;

                int distance2 = calculateColorDistance(pixel, otherCentroid);

                if (distance2 < distance) {
                    isClosest = false;
                    break;
                }
            }

            if (isClosest)
                cf.frequency++;
        }

        colorFreqs.append(cf);
    }

    // Sort by frequency (most common first)
    std::sort(colorFreqs.begin(), colorFreqs.end(),
              [](const ColorFrequency &a, const ColorFrequency &b) {
                  return a.frequency > b.frequency;
              });

    // Extract colors
    QVector<QColor> result;
    for (const ColorFrequency &cf : colorFreqs) {
        if (cf.frequency > 0) {
            result.append(cf.color);
        }
    }

    return result;
}
