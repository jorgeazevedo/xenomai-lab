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

#ifndef BLOCKBASE_H
#define BLOCKBASE_H

#include "rt_block_io.h"

#include <QMainWindow>
#include <QHash>
class QVBoxLayout;
class QHBoxLayout;
class QGroupBox;
class QLabel;
class QDialogButtonBox;
class QLineEdit;
class QSpinBox;
class QDoubleSpinBox;
class QComboBox;
class QDebug;
class QAbstractButton;
class QPushButton;
class QGridLayout;
class QMessageBox;
class QKeyEvent;

class BlockBase : public QMainWindow
{

    Q_OBJECT

    class SettingsLock{
    public:
        SettingsLock(){
            settings_lock(&gs_mtx);
        }
        ~SettingsLock(){
            settings_unlock(&gs_mtx);
        }
    };



public:
    explicit BlockBase(QWidget *parent = 0, const QString& execName="missing", const QString& name="missing");
    virtual ~BlockBase();

    static void blockSettingsInit(int argc,char* argv[],size_t gssize);
    static void errorMessage(QString Origin, QString message, QWidget* parent=0);

    void keyPressEvent(QKeyEvent *);
private Q_SLOTS:
    void pressedOk();
    void pressedApply(QAbstractButton*);
    void cancel();
    void spinBoxChange(int);
    void doubleSpinBoxChange(double);
    void comboBoxChange(QString);
    void lineEditChange(QString);
    bool permissionToQuit();


protected:
    void fillDialog(const QString& blockName, const QString& description);
    /**
     * Pure virtual function. Derived classes must implement
     * it with calls to newComboBox, newEntry, etc.
     */
    virtual void setSettings() = 0;

    void newEntry(const QString& text,int* value);
    void newEntry(const QString& text,double* value);
    void newComboBox(const QString& text, const QStringList& options,
                         char* select);
    void newMatrix(const QString& text,Matrix* M1);
private:


    QString d_execName;
    QString d_name;
    int d_entryNum;
    QVBoxLayout* d_verticalLayout;
    QGroupBox* d_descriptionGroup;
    QVBoxLayout* d_descriptionLayout;
    QGroupBox* d_settingsGroup;
    QGridLayout* d_settingsLayout;
    QLabel* d_temp;
    QHash<QDoubleSpinBox*,double*> doubleSpinBoxes;
    QHash<double*,double> originalDouble;
    QHash<QSpinBox*,int*> spinBoxes;
    QHash<int*,int> originalInt;
    QHash<QComboBox*,char*> comboBoxes;
    QHash<char*,QString> originalString;
    QHash<QLineEdit*,Matrix*> lineEdits;
    QHash<Matrix*, Matrix> originalMatrix;


};

#endif // BLOCKBASE_H
