#include "CookieClickerWindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    CookieClickerWindow window;
    window.show();

    return app.exec();
}
