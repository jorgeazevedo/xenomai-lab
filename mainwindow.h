/*
 * Xenomai Lab
 * Copyright (C) 2011  Jorge Azevedo
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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "superline.h"
#include "workspace.h"
#include "blockregistry.h"
#include "projectfile.h"
#include "diagramitem.h"
#include "processresult.h"
#include "blockdiagram.h"
#include "diagramitem.h"
#include "diagramscene.h"
#include "diagramtextitem.h"
#include "linepath.h"
#include "settings.h"
#include "filesettings.h"
#include "linepath.h"
#include <QList>
#include <QProgressDialog>
#include "macros.h"

class DiagramScene;

QT_BEGIN_NAMESPACE
class QAction;
class QToolBox;
class QSpinBox;
class QComboBox;
class QFontComboBox;
class QButtonGroup;
class QLineEdit;
class QGraphicsTextItem;
class QFont;
class QToolButton;
class QAbstractButton;
class QGraphicsView;
QT_END_NAMESPACE

//! [0]
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
   MainWindow();
   ~MainWindow();
signals:
   void sendAction(int action);
   void sendEdit(QString name);
   void sendSettings(QString name);

private slots:
    //void backgroundButtonGroupClicked(QAbstractButton *button);
    void blockButtonGroupClicked(int id);
    void deleteItem();
    void pointerGroupClicked(int id);
    void itemInserted(DiagramItem *item);
    void itemMoved(QGraphicsItem *item);
    void textInserted(QGraphicsTextItem *item);
    void sceneScaleChanged(const QString &scale);
    void itemSelected(QGraphicsItem *item);
    void about();

    void insertBlock(QString blockType, QString name, qreal x, qreal y);
    void insertBlock(qreal x,qreal y);
    void insertBlock(const SuperBlock& newBlock);
    void insertLine(DiagramItem* startItem, DiagramItem* endItem);
    void loadLine(const SuperLine& line);
    void loadBlock(const SuperBlock& newBlock);
    QString askForLineName();
    QString askForLineValue();
    bool elementAlreadyExists(const QString& line);

    void selectedItemChanged(DiagramItem*,bool);


    void prepareAction();
    void prepareEdit();
    void prepareSettings();
    void prepareGlobalSettings();

    void setExecTerm(bool);
    void setExecSudo(bool);

    bool readFile(const QString& fileName);
    bool loadFile(const QString& fileName);
    bool saveFile(const QString& fileName);


    void clear();
    void newFile();
    void openFile(const QString& fileName);
    void openFile();
    bool save();
    bool saveAs();
    void openRecentFile();

    void errorMessage(QString Catcher, QStringList err);
    void errorMessage(QString Origin, QString message);
public slots:
    void backFromAction(bool);
    void updateProgress(ProcessResult result);
    void processBlockError(QString);

private:
    QString askForBlockName(const QString& type);
    bool invalidName(const QString& name) const;
    bool onlyValidChars(QString str);

    bool okToContinue();
    void setCurrentFile(const QString& fileName);
    void updateRecentFileActions(void);

    void closeEvent(QCloseEvent * event);

    void createToolBox();
    void createActions();
    void createMenus();
    void createToolbars();

    void readSettings();
    void writeSettings();


    QMenu* newBlockContextMenu(bool RT);
    QWidget *newBlockButton(const QString &text,
                              DiagramItem::DiagramType type, int index=-1);

    QMenu *createColorMenu(const char *slot, QColor defaultColor);
    QIcon createColorToolButtonIcon(const QString &image, QColor color);
    QIcon createColorIcon(QColor color);

    static const int MaxRecentFiles=5;

    QStringList d_recentFiles;
    QString d_currentFile;

    QAction* recentFileActions[MaxRecentFiles];
    QAction* newAction;
    QAction* openAction;
    QAction* saveAction;
    QAction* saveAsAction;
    QAction* separatorAction;

    DiagramScene *scene;
    QGraphicsView *view;

    QAction *exitAction;
    QAction *addAction;
    QAction *deleteAction;


    QAction *aboutAction;

    QMenu *fileMenu;
    QMenu *toolsMenu;
    QMenu *itemMenu;
    QMenu *aboutMenu;

    QToolBar *textToolBar;
    QToolBar *editToolBar;
    QToolBar *colorToolBar;
    QToolBar *pointerToolbar;
    QToolBar *diagramToolbar;

    QComboBox *sceneScaleCombo;
    QComboBox *itemColorCombo;
    QComboBox *textColorCombo;

    QToolBox *toolBox;
    QButtonGroup *blockButtonGroup;
    QButtonGroup *pointerTypeGroup;

    QAction *textAction;
    QAction *fillAction;
    QAction *lineAction;

    Workspace* d_ws;
    BlockDiagram* blockDiag;
    //QAction * execInTermAction;
    QAction * editAction;
    QAction * settingsAction;
    QAction *cleanAction;
    QAction *makeAction;
    QAction *execAction;
    QAction *killAction;
    QAction *globalOptionsAction;
    int d_actionNum;

    QList<ProcessResult> d_results;
    QProgressDialog* d_progress;
    int d_index;
    Settings *d_settings;

    QString d_currentlySelectedType;
    //Block for scene->setItem
    QString d_blockType;
    QString d_name;
    bool d_RT;
    DiagramItem::DiagramType d_type;
    QMenu* d_menu;

    BlockRegistry* d_blockRegistry;

    QString d_defaultEditor;

    bool d_executing;
};


#endif
