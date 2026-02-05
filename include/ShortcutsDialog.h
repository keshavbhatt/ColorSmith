#ifndef SHORTCUTSDIALOG_H
#define SHORTCUTSDIALOG_H

#include <QDialog>

class ShortcutsDialog : public QDialog {
    Q_OBJECT

public:
    explicit ShortcutsDialog(QWidget *parent = nullptr);
    ~ShortcutsDialog() override = default;

private:
    void setupUI();
    void addShortcutSection(class QVBoxLayout *layout, const QString &title, const QList<QPair<QString, QString>> &shortcuts);
};

#endif // SHORTCUTSDIALOG_H
