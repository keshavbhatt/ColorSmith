#include "../include/MainWindow.h"
#include "../include/version.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QApplication::setApplicationName(COLORSMITH_APP_NAME);
    QApplication::setOrganizationName(COLORSMITH_ORGANIZATION_NAME);
    QApplication::setOrganizationDomain(COLORSMITH_ORGANIZATION_DOMAIN);
    QApplication::setApplicationVersion(COLORSMITH_VERSION_STRING);

    MainWindow window;
    window.show();

    return app.exec();
}
