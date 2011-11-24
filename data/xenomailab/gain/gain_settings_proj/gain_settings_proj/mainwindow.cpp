/*
 * Xenomai Lab
 * Copyright (C) 2011 Jorge Azevedo
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

#include "mainwindow.h"

MainWindow::MainWindow(const QString &execName, const QString &name, QWidget *parent) :
    BlockBase(parent,execName,name)
{
    fillDialog("Gain","Multiplies all elements of an input matrix by a given gain.");
}

MainWindow::~MainWindow()
{

}

/*
 * Sets the bottom half of the dialog.
 * There are only two types of settings:
 * -ComboBoxes, which allow the user to
 *      choose between multiple strings.
 * -Entries, which allow the user to
 *      specifiy a number (either an int
 *      or a double).
 * These options are supposed to cover all elements
 * of the globalsettings struct.
 * The SettingsLock object ensures mutual exclusion
 * from the real-time task, which is also manipulating
 * the same data. It locks gs on construction and unlocks
 * on destruction (when the function exits).
 */


void MainWindow::setSettings()
{
    newEntry("Gain:",&gs->gain);
}


