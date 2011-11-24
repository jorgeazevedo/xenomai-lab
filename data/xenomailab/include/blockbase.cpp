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

#include <QtGui>

#include "blockbase.h"

RT_TASK rt_task;
void BlockBase::blockSettingsInit(int argc, char *argv[],size_t gssize)
{
    mlockall(MCL_CURRENT|MCL_FUTURE);

    io.block_name=0;
    if(argc!=2){
        QString name=argv[0];
        name=name.right(name.size()-name.lastIndexOf("/")-1);
        qDebug() << QString("Usage: ")+name+" CONFIG_FILE";

        errorMessage("blockSettingsInit","No configurations file specified.\n"+
                     QString("Usage: ")+name+" CONFIG_FILE");

        exit(1);
    }
    rt_task_shadow(&rt_task,NULL,0,0);

    if(load_settings(argv[1],gssize)){
        QMessageBox::critical(0,"Error",QString("Could not open settings at<BR>")+argv[1]+"<BR>Does file exist?",
                                       QMessageBox::Ok);
        exit(1);
    }
}

void BlockBase::errorMessage(QString Origin, QString message, QWidget *parent)
{
    QMessageBox::critical(parent,"Error",message,QMessageBox::Ok);
    qDebug() << Origin << " - " << message;
}

BlockBase::BlockBase(QWidget *parent, const QString& execName, const QString& name) :
    QMainWindow(parent),d_execName(execName),d_name(name),d_entryNum(0)
{
    QWidget *holder = new QWidget(this);

    d_verticalLayout = new QVBoxLayout();
    holder->setLayout(d_verticalLayout);


    d_descriptionGroup = new QGroupBox(this);
    d_descriptionLayout = new QVBoxLayout();
    d_descriptionGroup->setLayout(d_descriptionLayout);
    //setDescription()

    d_settingsGroup = new QGroupBox("Settings",this);

    d_settingsLayout = new QGridLayout();
    d_settingsGroup->setLayout(d_settingsLayout);
    //setSettings();

    d_verticalLayout->addWidget(d_descriptionGroup);
    d_verticalLayout->addWidget(d_settingsGroup);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel | QDialogButtonBox::Apply);
    connect(buttonBox, SIGNAL(clicked(QAbstractButton*)),
            this, SLOT(pressedApply(QAbstractButton*)) );
    connect(buttonBox, SIGNAL(accepted()),
            this, SLOT(pressedOk()));
    connect(buttonBox, SIGNAL(rejected()),
            this, SLOT(cancel()));

    d_verticalLayout->addWidget(buttonBox);

    setCentralWidget(holder);

    setFocusPolicy(Qt::TabFocus);

}

BlockBase::~BlockBase(){

}

/*
 * Sets the text for the top half of the dialog.
 * New paragraphs are added by adding new QLabel widgets
 * to d_descriptionLayout
 */

void BlockBase::fillDialog(const QString &blockName, const QString &description)
{
    setWindowTitle(blockName+" settings");

    d_descriptionGroup->setTitle(blockName);

    d_descriptionLayout->addWidget(new QLabel(description,this));
    d_descriptionLayout->addWidget(new QLabel("Current instance: <i>"+d_name+"<i>",this));

    setSettings();
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



/*
 * Notice that the pointer to int is saved in a
 * QHash<QSpinBox*,int*>.
 * In effect, a hash table is created between Spinboxes
 * and a pointer to the int they manipulate.
 * This pointer points to the int in the globalsettings
 * structure, which might be a position in shared memory
 * if the RT process is running, or to a normal struct
 * allocated my malloc.
 * When the spinbox changes value, we do:
 * *spinBoxes[origin]=value;
 * effectively changing the value inside the gs structure
 */

void BlockBase::newEntry(const QString& text, int* value)
{
    SettingsLock lock;
    QSpinBox *spinBox = new QSpinBox(this);
    spinBox->setMinimum(-1000000000);
    spinBox->setMaximum(1000000000);
    spinBox->setValue(*value);

    d_settingsLayout->addWidget(new QLabel(text,this),d_entryNum,0);
    d_settingsLayout->addWidget(spinBox,d_entryNum,1);


    connect(spinBox, SIGNAL(valueChanged(int)),
            this, SLOT(spinBoxChange(int)) );

    spinBoxes[spinBox]=value;
    originalInt[value]=*value;

    d_entryNum++;
}


void BlockBase::newEntry(const QString& text, double* value)
{
    SettingsLock lock;
    QDoubleSpinBox *spinBox = new QDoubleSpinBox(this);
    spinBox->setMinimum(-1000000000);
    spinBox->setMaximum(1000000000);
    spinBox->setValue(*value);

    d_settingsLayout->addWidget(new QLabel(text,this),d_entryNum,0);
    d_settingsLayout->addWidget(spinBox,d_entryNum,1);


    connect(spinBox, SIGNAL(valueChanged(double)),
            this, SLOT(doubleSpinBoxChange(double)) );

    doubleSpinBoxes[spinBox]=value;
    originalDouble[value]=*value;

    d_entryNum++;
}

void BlockBase::newComboBox(const QString& text, const QStringList& options,
                                 char* select)
{
    SettingsLock lock;
    QComboBox* box=new QComboBox(this);
    box->addItems(options);
    box->setCurrentIndex(options.indexOf(select));

    d_settingsLayout->addWidget(new QLabel(text,this),d_entryNum,0);
    d_settingsLayout->addWidget(box,d_entryNum,1);

    connect(box, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(comboBoxChange(QString)) );

    comboBoxes[box]=select;
    originalString[select]=QString(select);

    d_entryNum++;
}

void BlockBase::newMatrix(const QString& text, Matrix* M1)
{
    SettingsLock lock;
    char buf[50];
    matrix_string(M1,buf);

    QLineEdit* lineEdit = new QLineEdit(this);
    lineEdit->setText(buf);

    d_settingsLayout->addWidget(new QLabel(text,this),d_entryNum,0);
    d_settingsLayout->addWidget(lineEdit,d_entryNum,1);

    connect(lineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(lineEditChange(QString)) );

    lineEdits[lineEdit]=M1;
    originalMatrix[M1]=*M1;

    d_entryNum++;
}

void BlockBase::doubleSpinBoxChange(double value)
{
    SettingsLock lock;
    QDoubleSpinBox* origin = qobject_cast<QDoubleSpinBox *>(sender());
    *doubleSpinBoxes[origin]=value;


    //qDebug() << origin->value() << value;
}

void BlockBase::spinBoxChange(int value)
{
    SettingsLock lock;
    QSpinBox* origin = qobject_cast<QSpinBox *>(sender());
    *spinBoxes[origin]=value;

    //qDebug() << origin->value() << value;
}

void BlockBase::comboBoxChange(QString newText)
{
    SettingsLock lock;
    QComboBox* origin = qobject_cast<QComboBox *>(sender());

    strcpy(comboBoxes[origin],newText.toStdString().c_str());

    //qDebug() << origin->currentText() << newText;
}

void BlockBase::lineEditChange(QString text)
{
    SettingsLock lock;
    QLineEdit* origin = qobject_cast<QLineEdit *>(sender());

    Matrix M1;
    char buf[50];
    sprintf(buf,"%s",text.toStdString().c_str());

    //returns 1 on error
    if(new_matrix_safe(&M1,buf)){
        origin->setStyleSheet("background-color: red");
    }
    else{
        origin->setStyleSheet("background-color: white");
        *lineEdits[origin]=M1;
    }
    return;
}

void BlockBase::keyPressEvent(QKeyEvent * event)
{
    if(event->key()==Qt::Key_Escape)
        cancel();

    QWidget::keyPressEvent(event);
}

bool BlockBase::permissionToQuit(){
    char heapName[50];
    QString heapNameS=d_name;
    heapNameS.replace('/','-');
    sprintf(heapName,"%s",heapNameS.toStdString().c_str());

    if(settings_owner && !am_alone(heapName)){
        return false;
    }

    return true;
}

void BlockBase::pressedOk()
{
    char name[50];
    sprintf(name,"%s",d_name.toStdString().c_str());

    if(!permissionToQuit()){
        errorMessage("pressedOk","Can't quit because real-time block is running!<BR>Stop block and then quit.",this);
        return;
    }

    if(save_settings(name)){
        errorMessage("pressedOk","Unable to save settings.",this);
    }

    close();
}

void BlockBase::pressedApply(QAbstractButton *origin)
{
    if(origin->text()=="Apply"){
        char name[50];
        sprintf(name,"%s",d_name.toStdString().c_str());

        if(update_settings(name)){
            errorMessage("pressedOk","Unable to save settings.",this);
        }

    }
}

void BlockBase::cancel()
{    SettingsLock* lock= new SettingsLock;

    //Step thru old values and copy them to right places
    QHash<int*, int>::iterator i = originalInt.begin();
    while (i != originalInt.end()) {
        *i.key() = i.value();
        ++i;
    }
    QHash<double*, double>::iterator j = originalDouble.begin();
    while (j != originalDouble.end()) {
        *j.key() = j.value();
        ++j;
    }
    QHash<char*, QString>::iterator k = originalString.begin();
    while (k != originalString.end()) {
        strcpy(k.key(),k.value().toStdString().c_str());
        ++k;
    }

    QHash<Matrix*, Matrix>::iterator l = originalMatrix.begin();
    while (l != originalMatrix.end()) {
        *l.key() = l.value();
        ++l;
    }

    delete lock;
    pressedOk();
}
