/*
 * Xenomai Lab
 * Copyright (C) 2013 Jorge Azevedo
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <QtGui>

#include "line.h"
#include "mainwindow.h"
#include "settings.h"
#include <QHash>
#include <QString>


//Useful Macros
//Commenting this define disables debug messages




int main(int argv, char *args[])
{

    //BlockDiagram xis("jorge_proje");

    Q_INIT_RESOURCE(xenomailab);

    /*
        QtSingleApplication app(argc, argv);

        if (app.isRunning())
            return 0;
        */

    QApplication app(argv, args);
    MainWindow mainWindow;
    mainWindow.setGeometry(100, 100, 1024, 768);
    mainWindow.show();
/*
    Block ticker("tick","tick",1);
    ticker.setWorkspace("execinterm");
    ticker.setArg(QStringList("-o tick1=[ 1 2 3],tick2"));
    ticker.setExecInTerm(false);
    ticker.exec();
*/
    return app.exec();

}
