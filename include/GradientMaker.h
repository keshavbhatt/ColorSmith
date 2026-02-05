#ifndef GRADIENTMAKER_H
#define GRADIENTMAKER_H

#include <QDialog>
#include <QFrame>
#include <QVector>

class QSlider;
class QSpinBox;
class QComboBox;
class QPushButton;
class QLineEdit;
class QLabel;
class QFrame;
class QVBoxLayout;
class ScreenPicker;

struct GradientStop {
  QColor color;
  double position; // 0.0 to 1.0

  GradientStop(const QColor &c = Qt::white, double pos = 0.0)
      : color(c), position(pos) {}
};

class GradientSlider : public QWidget {
  Q_OBJECT

public:
  explicit GradientSlider(QWidget *parent = nullptr);

  void setStops(const QVector<GradientStop> &stops);
  QVector<GradientStop> stops() const { return m_stops; }

  int selectedStopIndex() const { return m_selectedStop; }
  void setSelectedStop(int index);

signals:
  void stopSelected(int index);
  void stopMoved(int index, double position);
  void stopAdded(int index, double position);
  void stopRemoved(int index);

protected:
  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
  QVector<GradientStop> m_stops;
  int m_selectedStop;
  int m_draggedStop;
  bool m_isDragging;

  int getStopAtPosition(const QPoint &pos) const;
  QRect getStopRect(int index) const;
};

class GradientPreviewWidget : public QFrame {
  Q_OBJECT

public:
  explicit GradientPreviewWidget(QWidget *parent = nullptr);

  void setGradient(const QVector<GradientStop> &stops, int rotation,
                   bool isRadial);

protected:
  void paintEvent(QPaintEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;

private slots:
  void onFullscreenClicked();
  GradientPreviewWidget *createFullscreenPreview(QWidget *parent);
  void setupFullscreenCloseButton(GradientPreviewWidget *preview,
                                  QWidget *window);

private:
  QVector<GradientStop> m_stops;
  int m_rotation;
  bool m_isRadial;
  QPushButton *m_fullscreenBtn;
};

class GradientMaker : public QDialog {
  Q_OBJECT

public:
  explicit GradientMaker(QWidget *parent = nullptr);

  QString getGradientCSS() const;
  QString getGradientSVG() const;

protected:
  void closeEvent(QCloseEvent *event) override;

private slots:
  void onStopSelected(int index);
  void onStopMoved(int index, double position);
  void onStopAdded(int index, double position);
  void onStopRemoved(int index);
  void onColorChanged();
  void onPositionChanged(int value);
  void onRotationChanged(int value);
  void onTypeChanged(int index);
  void onRemoveColorClicked();
  void onRandomClicked();
  void onCopyCSS();
  void onCopySVG();
  void onExportSVG();
  void onExportImage();
  void onPickColor();
  void onScreenColorPicked(const QColor &color);
  void onPickerError(const QString &message);

private:
  void setupUI();
  void setupGradientSlider(QVBoxLayout *mainLayout);
  void setupControls(QVBoxLayout *mainLayout);
  void setupButtons(QVBoxLayout *mainLayout);
  void setupPreview(QVBoxLayout *mainLayout);

  void updatePreview();
  void updateControls();
  QString generateGradientCSS() const;
  static QString getDirectionForOldWebKit(int angle);
  void generateRandomGradient();

  QString generateSVGGradientDefinition() const;

  void saveGeometry();
  void loadGeometry();

  // UI Components
  GradientSlider *m_gradientSlider;
  GradientPreviewWidget *m_previewFrame;
  QLineEdit *m_colorInput;
  QPushButton *m_colorPickerBtn;
  QPushButton *m_removeColorBtn;
  QSpinBox *m_positionInput;
  QSpinBox *m_rotationInput;
  QComboBox *m_typeCombo;
  QPushButton *m_randomBtn;
  QPushButton *m_exportBtn;
  QPushButton *m_copySVGBtn;
  QPushButton *m_exportSVGBtn;
  QPushButton *m_exportImageBtn;
  ScreenPicker *m_screenPicker;

  // State
  QVector<GradientStop> m_stops;
  int m_rotation;
  enum GradientType { Linear, Radial };
  GradientType m_type;
};

#endif // GRADIENTMAKER_H
