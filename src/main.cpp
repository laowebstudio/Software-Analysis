#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("SAPUDOM Structural Analysis");
    QApplication::setOrganizationName("SAPUDOM");

    MainWindow window;
    window.show();

    return app.exec();
}
