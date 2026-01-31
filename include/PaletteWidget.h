#ifndef PALETTEWIDGET_H
#define PALETTEWIDGET_H

#include <QWidget>
#include <QColor>
#include <QVector>

class QGridLayout;
class QAction;
class QToolButton;
class QLabel;
class QFrame;
class QScrollArea;
class QComboBox;
class QStatusBar;
class Palette;

class ColorSwatch : public QWidget {
    Q_OBJECT

public:
    explicit ColorSwatch(const QColor& color, QWidget* parent = nullptr, bool readOnly = false, const QString& colorName = QString());
    QColor color() const { return m_color; }
    bool isReadOnly() const { return m_readOnly; }

signals:
    void clicked();
    void removeRequested();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

private:
    QColor m_color;
    QString m_colorName;
    bool m_readOnly;
    static constexpr int SWATCH_SIZE = 40;
};

class AddColorSwatch : public QWidget {
    Q_OBJECT

public:
    explicit AddColorSwatch(QWidget* parent = nullptr);

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    bool m_enabled;
    bool m_hovered;
    static constexpr int SWATCH_SIZE = 40;
};

class PaletteWidget : public QWidget {
    Q_OBJECT

public:
    explicit PaletteWidget(QWidget* parent = nullptr);

    void setPalette(Palette* palette);
    Palette* currentPalette() const { return m_currentPalette; }
    void refreshColors();

signals:
    void colorSelected(const QColor& color);
    void addColorRequested();

private slots:
    void onClearPalette();
    void onExportPalette();
    void onImportPalette();
    void onAddColorClicked();
    void onSwatchClicked();
    void onSwatchRemoveRequested();
    void onPaletteSelected(int index);
    void onNewPalette();
    void onRenamePalette();
    void onDeletePalette();
    void onGenerateFromImage();

private:
    void setupUI();
    void updateLayout();
    void addColorToUI(const QColor& color, const QString& colorName = QString());
    void updatePaletteCombo();
    QStatusBar* statusBar();

    QFrame* m_headerFrame;
    QLabel* m_titleLabel;
    QComboBox* m_paletteCombo;
    QToolButton* m_addButton;
    QToolButton* m_menuButton;
    QWidget* m_contentWidget;
    QScrollArea* m_scrollArea;
    QWidget* m_gridWidget;
    QGridLayout* m_gridLayout;
    QLabel* m_emptyStateLabel;

    Palette* m_currentPalette;
    QVector<ColorSwatch*> m_swatches;

    QAction* m_newPaletteAction;
    QAction* m_renamePaletteAction;
    QAction* m_deletePaletteAction;
    QAction* m_generateFromImageAction;
    QAction* m_clearPaletteAction;
    QAction* m_exportPaletteAction;
    QAction* m_importPaletteAction;

    static constexpr int MAX_COLUMNS = 6;
    static constexpr int SWATCH_SIZE = 40;
    static constexpr int SPACING = 8;
};

#endif // PALETTEWIDGET_H
