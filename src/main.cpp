#include "MainWindow.h"

#include <QApplication>
#include <QFont>

int main(int argc, char* argv[]) {
    QApplication::setOrganizationName("CookieClicker");
    QApplication::setApplicationName("Cookie Clicker");
    QApplication::setApplicationDisplayName("Cookie Clicker");
    QApplication::setApplicationVersion("1.0.0");

    QApplication app(argc, argv);

    QFont base = app.font();
    if (base.pointSize() < 10) base.setPointSize(10);
    app.setFont(base);

    cookie::MainWindow w;
    w.show();
    return app.exec();
}
