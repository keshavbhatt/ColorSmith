#ifndef SCREENPICKER_H
#define SCREENPICKER_H

#include <QObject>
#include <QColor>
#include <QtDBus/QDBusPendingCallWatcher>

class ScreenPicker : public QObject {
    Q_OBJECT
public:
    explicit ScreenPicker(QObject *parent = nullptr);
    void pickColor();

signals:
    void colorPicked(const QColor &color);
    void errorOccurred(const QString &message);

private slots:
    void onPortalRequestFinished(QDBusPendingCallWatcher *watcher);
    void onPortalResponse(uint response, const QVariantMap &results);

private:
    QString m_requestPath;
};

#endif // SCREENPICKER_H
