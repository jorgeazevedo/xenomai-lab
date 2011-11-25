#include "mainwindow.h"


MainWindow::MainWindow(const QString &execName, const QString &name, QWidget *parent) :
    BlockBase(parent,execName,name)
{

    /*
     * Sets the text for the top half of the dialog.
     */
    fillDialog("Pid","A standard PID controller.");
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
    newEntry("Ki:",&gs->ki);
    newEntry("Kp:",&gs->kp);
    newEntry("Td:",&gs->td);

}


