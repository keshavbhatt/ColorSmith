#include "../include/ScreenPicker.h"
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusPendingReply>
#include <QtDBus/QDBusArgument>
#include <QtDBus/QDBusMetaType>
#include <QDebug>

// Helper to register the custom DBus type for the struct (ddd)
struct ColorStruct {
    double r;
    double g;
    double b;
};
Q_DECLARE_METATYPE(ColorStruct)

QDBusArgument &operator<<(QDBusArgument &argument, const ColorStruct &myStruct)
{
    argument.beginStructure();
    argument << myStruct.r << myStruct.g << myStruct.b;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, ColorStruct &myStruct)
{
    argument.beginStructure();
    argument >> myStruct.r >> myStruct.g >> myStruct.b;
    argument.endStructure();
    return argument;
}

ScreenPicker::ScreenPicker(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<ColorStruct>("ColorStruct");
    qDBusRegisterMetaType<ColorStruct>();
}

void ScreenPicker::pickColor()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        "org.freedesktop.portal.Desktop",
        "/org/freedesktop/portal/desktop",
        "org.freedesktop.portal.Screenshot",
        "PickColor"
    );

    message << "";
    QVariantMap options;
    message << options;

    QDBusPendingCall call = QDBusConnection::sessionBus().asyncCall(message);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, &ScreenPicker::onPortalRequestFinished);
}

void ScreenPicker::onPortalRequestFinished(QDBusPendingCallWatcher *watcher)
{
    QDBusPendingReply<QDBusObjectPath> reply = *watcher;
    if (reply.isError()) {
        emit errorOccurred(QString("Portal error: %1").arg(reply.error().message()));
    } else {
        QDBusObjectPath path = reply.value();
        m_requestPath = path.path();

        QDBusConnection::sessionBus().connect(
            "org.freedesktop.portal.Desktop",
            m_requestPath,
            "org.freedesktop.portal.Request",
            "Response",
            this,
            SLOT(onPortalResponse(uint, QVariantMap))
        );
    }
    watcher->deleteLater();
}

void ScreenPicker::onPortalResponse(uint response, const QVariantMap &results)
{
    if (response == 0 && results.contains("color")) {
        QDBusArgument arg = results["color"].value<QDBusArgument>();
        ColorStruct color;
        arg >> color;

        QColor qtColor;
        qtColor.setRgbF(color.r, color.g, color.b);
        emit colorPicked(qtColor);
    }

    QDBusConnection::sessionBus().disconnect(
        "org.freedesktop.portal.Desktop",
        m_requestPath,
        "org.freedesktop.portal.Request",
        "Response",
        this,
        SLOT(onPortalResponse(uint, QVariantMap))
    );
}
