#include "../include/ShortcutsDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QFrame>
#include <QFont>

ShortcutsDialog::ShortcutsDialog(QWidget *parent)
    : QDialog(parent) {
    setWindowTitle(tr("Keyboard Shortcuts"));
    setMinimumSize(500, 400);
    resize(550, 500);

    setupUI();
}

void ShortcutsDialog::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Title
    QLabel *titleLabel = new QLabel(tr("Keyboard Shortcuts"), this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(titleFont.pointSize() + 4);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // Scroll area for shortcuts
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QWidget *scrollWidget = new QWidget();
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollWidget);
    scrollLayout->setSpacing(20);
    scrollLayout->setContentsMargins(0, 0, 0, 0);

    // General Actions
    QList<QPair<QString, QString>> generalShortcuts = {
        {tr("Ctrl+P"), tr("Pick color from screen")},
        {tr("Ctrl+G"), tr("Open Gradient Maker")},
        {tr("Ctrl+C"), tr("Copy current color")},
        {tr("Ctrl+R"), tr("Generate random color")},
        {tr("Ctrl+K"), tr("Open this Keyboard Shortcuts dialog")}
    };
    addShortcutSection(scrollLayout, tr("General"), generalShortcuts);

    // RGB Slider Navigation
    QList<QPair<QString, QString>> sliderShortcuts = {
        {tr("Up Arrow"), tr("Increase value by 1")},
        {tr("Down Arrow"), tr("Decrease value by 1")},
        {tr("Alt+Up"), tr("Increase value by 5")},
        {tr("Alt+Down"), tr("Decrease value by 5")},
        {tr("Shift+Up"), tr("Increase value by 10")},
        {tr("Shift+Down"), tr("Decrease value by 10")},
        {tr("Page Up"), tr("Increase value by 25")},
        {tr("Page Down"), tr("Decrease value by 25")},
        {tr("Home"), tr("Set to minimum (0)")},
        {tr("End"), tr("Set to maximum (255)")}
    };
    addShortcutSection(scrollLayout, tr("RGB Slider Controls"), sliderShortcuts);

    scrollLayout->addStretch();
    scrollWidget->setLayout(scrollLayout);
    scrollArea->setWidget(scrollWidget);
    mainLayout->addWidget(scrollArea);

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

void ShortcutsDialog::addShortcutSection(QVBoxLayout *layout, const QString &title, const QList<QPair<QString, QString>> &shortcuts) {
    // Section title
    QLabel *sectionLabel = new QLabel(title, this);
    QFont sectionFont = sectionLabel->font();
    sectionFont.setPointSize(sectionFont.pointSize() + 2);
    sectionFont.setBold(true);
    sectionLabel->setFont(sectionFont);
    layout->addWidget(sectionLabel);

    // Shortcuts list
    for (const auto &shortcut : shortcuts) {
        QHBoxLayout *shortcutLayout = new QHBoxLayout();
        shortcutLayout->setContentsMargins(20, 2, 0, 2);

        // Shortcut key
        QLabel *keyLabel = new QLabel(shortcut.first, this);
        QFont keyFont = keyLabel->font();
        keyFont.setFamily("Monospace");
        keyLabel->setFont(keyFont);
        keyLabel->setMinimumWidth(150);
        keyLabel->setStyleSheet("QLabel { background-color: palette(button); padding: 4px 8px; border-radius: 3px; }");
        shortcutLayout->addWidget(keyLabel);

        // Description
        QLabel *descLabel = new QLabel(shortcut.second, this);
        descLabel->setWordWrap(true);
        shortcutLayout->addWidget(descLabel, 1);

        layout->addLayout(shortcutLayout);
    }
}
