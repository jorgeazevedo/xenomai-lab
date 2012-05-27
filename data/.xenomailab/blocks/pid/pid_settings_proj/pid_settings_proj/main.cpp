#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    BlockBase::blockSettingsInit(argc,argv,sizeof(struct global_settings));

    MainWindow w(argv[0],argv[1]);
    w.show();

    return a.exec();
}
