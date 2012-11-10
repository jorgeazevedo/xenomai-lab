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

#include <QtGui>
#include <QLabel>

#include "mainwindow.h"


const int InsertTextButton = 20;

MainWindow::~MainWindow()
{
    delete blockDiag;
    delete d_ws;
}


//! [0]
MainWindow::MainWindow():
    d_actionNum(-1),d_index(0),d_executing(false)
{

    try{
    //Load Application settings from QSettings
    d_blockRegistry = BlockRegistry::instance();
    d_blockRegistry->open();

    //This is here because newFile() is gonna try to update
    //recent files list, which references a pointer instanced
    //in createAction
    createActions();
    createToolBox();
    createMenus();
    createToolbars();

    readSettings();

    //blockDiag= new BlockDiagram("teste",go["Editor"],this);
    blockDiag= new BlockDiagram("Untitled",d_defaultEditor,this);

    //BD
    connect(this, SIGNAL(sendAction(int)),
            blockDiag, SLOT(act(int)),Qt::QueuedConnection );
    connect(blockDiag,SIGNAL(progress(ProcessResult)),
            this, SLOT(updateProgress(ProcessResult)) );
    connect(blockDiag, SIGNAL(finished(bool)),
            this, SLOT(backFromAction(bool)),Qt::QueuedConnection );
    connect(this, SIGNAL(sendEdit(QString)),
            blockDiag, SLOT(edit(QString)) );
    connect(this, SIGNAL(sendSettings(QString)),
            blockDiag, SLOT(settings(QString)) );
    connect(blockDiag, SIGNAL(blockError(QString)),
            this, SLOT(processBlockError(QString)) );

    scene = new DiagramScene(blockDiag, Qt::white, this);

    scene->setSceneRect(QRectF(0, 0, 2000, 2000));

    connect(scene, SIGNAL(insertLine(DiagramItem*,DiagramItem*)),
            this, SLOT(insertLine(DiagramItem*,DiagramItem*)) );
    connect(scene, SIGNAL(insertItem(qreal,qreal)),
            this, SLOT(insertBlock(qreal,qreal)) );

    connect(scene, SIGNAL(itemInserted(DiagramItem*)),
            this, SLOT(itemInserted(DiagramItem*)));
    connect(scene, SIGNAL(itemMoved(QGraphicsItem*)),
            this, SLOT(itemMoved(QGraphicsItem*)) );
    connect(scene, SIGNAL(textInserted(QGraphicsTextItem*)),
        this, SLOT(textInserted(QGraphicsTextItem*)));
    connect(scene, SIGNAL(itemSelected(QGraphicsItem*)),
        this, SLOT(itemSelected(QGraphicsItem*)));

    d_ws = new Workspace();

    newFile();

    setWindowIcon(QIcon(":/images/xenomailab.png"));

}catch(QString err){

        errorMessage("MainWindow::MainWindow",err);
    }

    //Collect window elements
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(toolBox);
    view = new QGraphicsView(scene);
    layout->addWidget(view);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    setCentralWidget(widget);
    //setWindowTitle(tr("XenomaiLab"));
    setUnifiedTitleAndToolBarOnMac(true);

    //POG
    //I want scene to be ay 75% zoom at the start
    //but this is all rather convoluted really.
    sceneScaleChanged("75%");

    //loadFile("/home/ceno/.xenomailab/workspace/static.xlb");

}

void MainWindow::loadBlock(const SuperBlock& newBlock)
{
    BlockRegistry* br=BlockRegistry::instance();
    DiagramItem::DiagramType diagType=br->diagOf(newBlock.type());
    bool RT=br->isRealTime(newBlock.type());

    DiagramItem* item = new DiagramItem(newBlock,diagType,newBlockContextMenu(RT),0,scene);
    Block* block = new Block(newBlock.type(), newBlock.name(), RT);

    try{

    blockDiag->addBlock(block);
    scene->addItem(item);

    itemInserted(item);
    }catch(QStringList err){
        //Possible exceptions:
        //blockDiag->addBlock already has a block by the same name
        //(which it _not_ happening is a condition of entry to this function
        //d_ws->addblock detects a block by the same or
        //cannot instatiate block.
        scene->removeItem(item);
        delete item;
        delete block;

        errorMessage(Q_FUNC_INFO,err);
    }
}


void MainWindow::insertBlock(QString blockType, QString name, qreal x, qreal y)
{
    d_blockType=blockType;
    d_name=name;
    d_RT=d_blockRegistry->isRealTime(blockType);
    d_type=d_blockRegistry->diagOf(blockType);

    d_menu=newBlockContextMenu(d_RT);
    insertBlock(x,y);

}


void MainWindow::insertBlock(const SuperBlock& newBlock)
{
    BlockRegistry* br=BlockRegistry::instance();
    DiagramItem::DiagramType diagType=br->diagOf(newBlock.type());
    bool RT=br->isRealTime(newBlock.type());

    DiagramItem* item = new DiagramItem(newBlock,diagType,newBlockContextMenu(RT),0,scene);
    Block* block = new Block(newBlock.type(), newBlock.name(), RT);

    try{

        d_ws->addBlock(newBlock);
        blockDiag->addBlock(block);
        qDebug() << "MW::insertBlock: adding item to scene. Will it fail?";
        scene->addItem(item);
        //I want mouse pointer to drop left top corner of block, not center.
        qreal x=newBlock.X()-item->boundingRect().x();
        qreal y=newBlock.Y()-item->boundingRect().y();
        item->setPos(QPointF(x,y));
        qDebug() << "MW::insertBlock: sucessfuly added item to scene.";

        itemInserted(item);
    }catch(QStringList err){
        //Possible exceptions:
        //blockDiag->addBlock already has a block by the same name
        //(which it _not_ happening is a condition of entry to this function
        //d_ws->addblock detects a block by the same or
        //cannot instatiate block.
        scene->removeItem(item);
        delete item;
        delete block;

        errorMessage(Q_FUNC_INFO,err);
    }



    //DiagramItem* item = new DiagramItem(d_blockType,d_name,d_type,d_menu);
/*    Block* block = new Block(d_blockType,d_name,d_RT);

    x=x-item->boundingRect().x();
    y=y-item->boundingRect().y();
    item->setPos(QPointF(x,y));

    try{
        blockDiag->addBlock(block);
        scene->addItem(item);

    }
    catch(QString err){
        delete block;
        delete item;

        errorMessage("MainWindow::insertBlock",err);
    }

    }catch(QStringList err){
        QApplication::restoreOverrideCursor();
        errorMessage(Q_FUNC_INFO,err);
    }
*/
}

void MainWindow::insertBlock(qreal x, qreal y)
{
    QString name=askForBlockName(d_currentlySelectedType);
    if(name=="")
        return;

    SuperBlock newBlock(d_currentlySelectedType,name,x,y);

    qDebug() << newBlock;

    insertBlock(newBlock);
/*
    DiagramItem* item = new DiagramItem(d_blockType,d_name,d_type,d_menu);
    Block* block = new Block(d_blockType,d_name,d_RT);

    x=x-item->boundingRect().x();
    y=y-item->boundingRect().y();
    item->setPos(QPointF(x,y));

    try{
        blockDiag->addBlock(block);
        scene->addItem(item);

    }
    catch(QString err){
        delete block;
        delete item;

        errorMessage("MainWindow::insertBlock",err);
    }
*/

}

QString MainWindow::askForBlockName(const QString& type)
{
    //PROPOSED:
    //QString nameSuggestion=BlockRegistry::instance()->suggestionFor(type);

    bool ok=true;
    QString text;
    static int num=0;
    QString nameSuggestion=type+QString::number(num);

    while(ok && text.isEmpty()){

        text = QInputDialog::getText(this, tr("New Block"),
                                     tr("Block name:"), QLineEdit::Normal,
                                     nameSuggestion, &ok);
        if (ok && !text.isEmpty()){
            //PROPOSED:
            //if(text==nameSuggestion)
            //    BlockRegistry::instance()->suggestionAcceptedFor(type);
            if(text==nameSuggestion)
                num++;

            //Validate name
            if(onlyValidChars(text)){
                if(!elementAlreadyExists(text)){
                    break;
                }
                else{
                    errorMessage("askForBlockName","There's already an element by that name");
                    text="";
                }
            }
            else{
                errorMessage("askForBlockName","Names can only contain up tp 16 letters, numbers and the characters '-' and '_'");
                text="";
            }

        }
    }

    return text;
}

bool MainWindow::onlyValidChars(QString str){
    if(str.size()>17)
        return false;

    QString old=str;
    str.remove(QRegExp(QString::fromUtf8("[`~!@#$%^&*()—+=|:;<>«»,.?/{}\'\"\\\[\\\]\\\\]")));
    str.remove(" ");
    return (old==str);
}

void MainWindow::loadLine(const SuperLine& line)
{
    LinePath* linePath=0;

    try{
        //d_ws->addLine(line);
        blockDiag->addLine(line);

        QList<QGraphicsItem *> itemList=scene->items();
        QGraphicsItem* it;
        DiagramItem* startItem;
        DiagramItem* endItem;

        foreach(it,itemList){
            if (it->type() == DiagramItem::Type) {
                //startItem=qgraphicsitem_cast<DiagramItem *>(it);
                if(qgraphicsitem_cast<DiagramItem *>(it)->name()==line.origin())
                    startItem=qgraphicsitem_cast<DiagramItem *>(it);
                if(qgraphicsitem_cast<DiagramItem *>(it)->name()==line.destiny())
                    endItem=qgraphicsitem_cast<DiagramItem *>(it);
            }
        }

        linePath = new LinePath(startItem, endItem,line.name());

        startItem->addLinePathOut(linePath);
        endItem->addLinePathIn(linePath);
        linePath->setZValue(-1000.0);

        scene->addItem(linePath);

        linePath->updatePosition();


    }catch(QStringList err){

        if(linePath!=0)
            delete linePath;

        errorMessage(Q_FUNC_INFO,err);
        return;
    }

    return;
}

void MainWindow::insertLine(DiagramItem *startItem, DiagramItem *endItem)
{
    QString linePathName=askForLineName();
    QString linePathValue("[0]");

    //if user canceled
    if(linePathName.isEmpty())
        return;

    SuperLine newLine(startItem->name(),endItem->name(),linePathName,linePathValue);
    LinePath* linePath=0;

    try{
        d_ws->addLine(newLine);
        blockDiag->addLine(newLine.origin(),newLine.destiny(),
                           newLine.name(),newLine.value());

        linePath = new LinePath(startItem, endItem,linePathName);
        ///linePath = new LinePath(startItem, endItem,linePathName);
        ///scene->addItem(linePath);
        ///startItem->addLinePath(linePath);
        ///endItem->addLinePath(linePath);
        ///linePath->updatePosition();

        startItem->addLinePathOut(linePath);
        endItem->addLinePathIn(linePath);
        linePath->setZValue(-1000.0);

        scene->addItem(linePath);

        linePath->updatePosition();


    }catch(QStringList err){

        if(linePath!=0)
            delete linePath;

        errorMessage(Q_FUNC_INFO,err);
        return;
    }

}

QString MainWindow::askForLineName()
{
    bool ok=true;
    QString text;
    static int num=0;
    QString nameSuggestion="signal"+QString::number(num);

    while(ok && text.isEmpty()){

        text = QInputDialog::getText(this, tr("New Line"),
                                     tr("Line name:"), QLineEdit::Normal,
                                     nameSuggestion, &ok);

        if (ok && !text.isEmpty()){
            if(text==nameSuggestion)
                num++;

            if(onlyValidChars(text)){
                if(!elementAlreadyExists(text)){
                    break;
                }
                else{
                    errorMessage("askForLineName","There's already an element by that name");
                    text="";
                }
            }
            else{
                errorMessage("askForLineName","Names can only contain up tp 16 letters, numbers and the characters '-' and '_'");
                text="";
            }
        }
    }
    return text;
}

bool MainWindow::elementAlreadyExists(const QString& name)
{
   if(d_ws->blockExists(name))
       return true;
   if(d_ws->lineExists(name))
       return true;

    return false;
}

void MainWindow::selectedItemChanged(DiagramItem *, bool)
{
    scene->update();
    return;
}

//readFile throws exceptions, need to catch them!
bool MainWindow::readFile(const QString& fileName)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    //PROPOSED:
    //by enforcing the "name".xlb has a "name"
    //folder rule, we can deduce the folder name
    //by chopping up fileName and openProject
    //can return void like all its peers.
    QString name=d_ws->openProject(fileName);

    clear();

    d_ws->setWorkspace(name);

    blockDiag->setWorkspace(name);

    QList<SuperBlock> superBlockList=d_ws->getAllBlocks();
    SuperBlock superBlock;
    foreach(superBlock, superBlockList){
        loadBlock(superBlock);
    }

    QList<SuperLine> superLineList=d_ws->getAllLines();
    SuperLine superLine;
    foreach(superLine, superLineList){
        qDebug() << "About to load" << superLine;
        loadLine(superLine);
    }


    QApplication::restoreOverrideCursor();
    return true;
}

bool MainWindow::loadFile(const QString& fileName)
{
    try{
        readFile(fileName);

    }catch(QStringList err){
        errorMessage(Q_FUNC_INFO,err);
        return false;
    }

    setCurrentFile(fileName);

    return true;

}

bool MainWindow::saveFile(const QString& fileName)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    //Maybe encapsulate this behind a writeFile
    try{

        d_ws->saveProject(fileName);

    }catch(QString err){
        QApplication::restoreOverrideCursor();
        errorMessage(Q_FUNC_INFO,err);
        return false;
    }

    setCurrentFile(fileName);

    QApplication::restoreOverrideCursor();
    return true;
}


void MainWindow::newFile()
{
    if(okToContinue()){
        try{
            clear();

            d_ws->newWorkspace("Untitled");

            blockDiag->setWorkspace("Untitled");

            setCurrentFile("Untitled");

        }catch(QStringList err){
            errorMessage(Q_FUNC_INFO,err);
        }
    }

}

void MainWindow::clear()
{

    blockDiag->clear();
    scene->clear();
    d_ws->clearCurrentWorkspace();


}


bool MainWindow::okToContinue()
{
    if(isWindowModified()){
        int ret = QMessageBox::warning
                (this,"XenomaiLab",
                  "The project has been modified.\n"
                  "Do you want to save your changes?",
                  QMessageBox::Yes,QMessageBox::No,QMessageBox::Cancel);

        switch(ret){
            case QMessageBox::Yes :
            qDebug() << "MainWindow::okToContinue: save file!";
            return save();
            case QMessageBox::No :
            qDebug() << "MainWindow::okToContinue: don't save file!";
            return true;
            case QMessageBox::Cancel :
            qDebug() << "MainWindow::okToContinue: canceled";
            return false;

        }
    }
    return true;
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    d_currentFile=fileName;
    setWindowModified(false);


    QString shownName=tr("Untitled");
    if(!d_currentFile.isEmpty()){

        shownName= QFileInfo(d_currentFile).fileName();
        d_recentFiles.removeAll(d_currentFile);
        d_recentFiles.prepend(d_currentFile);
        updateRecentFileActions();
    }

    setWindowTitle("XenomaiLab - "+shownName+"[*]");
}


void MainWindow::updateRecentFileActions()
{
    QMutableStringListIterator i(d_recentFiles);
    while(i.hasNext()) {
        if(!QFile::exists(i.next()))
            i.remove();
    }


    for(int j=0; j< MaxRecentFiles; ++j){
        if(j < d_recentFiles.size()){
            QString text = QString("&%1 %2").
                    arg(j+1).arg(QFileInfo(d_recentFiles[j]).fileName());

            recentFileActions[j]->setText(text);

            recentFileActions[j]->setData(d_recentFiles[j]);
            recentFileActions[j]->setVisible(true);
        }
        else
            recentFileActions[j]->setVisible(false);
    }

    separatorAction->setVisible(!d_recentFiles.isEmpty());
}

void MainWindow::openFile(const QString& fileName)
{
    if(okToContinue()){
        //if we have a filename
        if(!fileName.isEmpty())
            loadFile(fileName);

    }
}

void MainWindow::openFile()
{
    if(okToContinue()){
        QString fileName = QFileDialog::getOpenFileName(
                            this,tr("Open Project"),QDir::homePath(),
                            tr("Project files .xlb (*.xlb)"));
        //if we have a filename
        if(!fileName.isEmpty())
            loadFile(fileName);

    }
}

bool MainWindow::save()
{

    if(d_currentFile=="Untitled"){
        //PROPOSED:
        //Open file after saving so Untitled dissapears
        //Alghorithm is this, now we need to change these functions
        //to efficientely accomodate this
        QString fileName = QFileDialog::getSaveFileName(
                            this,tr("Save Project As"),getenv("HOME"),
                            tr("Project files .xlb (*.xlb)"));

        if(fileName.isEmpty())
            return false;

        if(!fileName.contains(".xlb"))
            fileName.append(".xlb");

        /*
         * we don't call saveFile(fileName)
         * because saveFile changes the current
         * file. I don't agree with this behavior.
         * Save as should merely create a copy
         * of the project somewhere and leave
         * the program otherwise unchanged
         */
        QApplication::setOverrideCursor(Qt::WaitCursor);
        try{
            d_ws->saveProject(fileName);
            setWindowModified(false);
            openFile(fileName);
        }catch(QString err){
            QApplication::restoreOverrideCursor();
            errorMessage(Q_FUNC_INFO,err);
            return false;
        }

        QApplication::restoreOverrideCursor();



        return true;


        //return saveAs();
    }
    else
        return saveFile(d_currentFile);
}



bool MainWindow::saveAs()
{
    //PROPOSED:
    //encapsulate this under private member askForFileName();
    QString fileName = QFileDialog::getSaveFileName(
                        this,tr("Save Project As"),getenv("HOME"),
                        tr("Project files .xlb (*.xlb)"));

    if(fileName.isEmpty())
        return false;

    if(!fileName.contains(".xlb"))
        fileName.append(".xlb");

    /*
     * we don't call saveFile(fileName)
     * because saveFile changes the current
     * file. I don't agree with this behavior.
     * Save as should merely create a copy
     * of the project somewhere and leave
     * the program otherwise unchanged
     */
    QApplication::setOverrideCursor(Qt::WaitCursor);
    try{
        d_ws->saveProject(fileName);
    }catch(QString err){
        QApplication::restoreOverrideCursor();
        errorMessage(Q_FUNC_INFO,err);
        return false;
    }

    QApplication::restoreOverrideCursor();
    return true;
}
void MainWindow::openRecentFile()
{
    if(okToContinue()){
        QAction* action = qobject_cast<QAction *>(sender());
        if(action)
            loadFile(action->data().toString());
    }

}

void MainWindow::blockButtonGroupClicked(int id)
{
    QList<QAbstractButton *> buttons = blockButtonGroup->buttons();
    foreach (QAbstractButton *button, buttons) {
        if (blockButtonGroup->button(id) != button)
            button->setChecked(false);
    }

    d_currentlySelectedType=d_blockRegistry->typeOfId(id);
    scene->setMode(DiagramScene::InsertItem);

    qDebug() << "Currently selected block type:" <<d_currentlySelectedType;

        //after foreach
        //PROPOSED:
        //d_currentlySelectedType=d_blockRegistry.typeOfID(id);
        //scene->setMode(DiagramScene::InsertItem);
        //Notes:
        //Name is asked on click. Suggestion is similar to
        //QString fullName=i.key()+QString::number(i.value().instance(),10);
        //RT and diag type come from d_blockRegistry
        //d_menu is called on insertBlock
        //PROPOSED:
        //if(d_blockRegstry->isRealTime(Type)
        //  newBlockContextMenu(false);
        //else
        // newBlockContextMenu(true);

}
//! [2]

QMenu* MainWindow::newBlockContextMenu(bool RT)
{
    QMenu* newContextMenu= new QMenu();
    newContextMenu->addActions(itemMenu->actions());

    QAction* execInTermAction = new QAction(QIcon::fromTheme("accessories-text-editor"),"E&xecute in Terminal",0);
    execInTermAction->setStatusTip("Execute block within a terminal window");
    execInTermAction->setCheckable(true);
    //NRT blocks default to terminal execution
    execInTermAction->setChecked(!RT);

    connect(execInTermAction, SIGNAL(toggled(bool)),
            this,SLOT(setExecTerm(bool)) );

    QAction* execAsSudoAction = new QAction(QIcon::fromTheme("accessories-text-editor"),"E&xecute as sudo",0);
    execAsSudoAction->setStatusTip("Execute block with super user privileges");
    execAsSudoAction->setCheckable(true);
    execInTermAction->setChecked(false);

    connect(execAsSudoAction, SIGNAL(toggled(bool)),
            this,SLOT(setExecSudo(bool)) );


    newContextMenu->addSeparator();
    newContextMenu->addAction(execInTermAction);
    newContextMenu->addAction(execAsSudoAction);

    return newContextMenu;

}

//! [3]
void MainWindow::deleteItem()
{
 if(!scene->isLocked()){
    foreach (QGraphicsItem *item, scene->selectedItems()) {
        if (item->type() == LinePath::Type) {

            scene->removeItem(item);
            LinePath *linePath = qgraphicsitem_cast<LinePath *>(item);
            linePath->startItem()->removeLinePath(linePath);
            linePath->endItem()->removeLinePath(linePath);

            //MODPOINT
            setWindowModified(true);
            blockDiag->removeLine(linePath->name());
            //PROPOSED:
            d_ws->removeLine(linePath->name());

            delete item;
        }
    }

    foreach (QGraphicsItem *item, scene->selectedItems()) {
        /*
         if (item->type() == DiagramItem::Type) {
             qgraphicsitem_cast<DiagramItem *>(item)->removeLinePaths();
             QString name=qgraphicsitem_cast<DiagramItem *>(item)->name();
             //MODPOINT
             setWindowModified(true);
             blockDiag->removeBlock(name);
         }
         //this is outside the if clause in case the item isn't of
         //type DiagramItem. We dont' need it
         scene->removeItem(item);
         delete item;
         */
         //PROPOSED:
         if (item->type() == DiagramItem::Type) {
             qgraphicsitem_cast<DiagramItem *>(item)->removeLinePaths();
             QString name=qgraphicsitem_cast<DiagramItem *>(item)->name();
             //MODPOINT


             d_ws->removeBlock(name);
             blockDiag->removeBlock(name);
             scene->removeItem(item);
             delete item;

             setWindowModified(true);
         }
     }
 }
 else{
     errorMessage("deleteItem","Please stop execution before deleting");
 }

}
//! [3]

//! [4]
void MainWindow::pointerGroupClicked(int)
{
    scene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));
}

/*
void MainWindow::bringToFront()
{
    if (scene->selectedItems().isEmpty())
        return;

    QGraphicsItem *selectedItem = scene->selectedItems().first();
    QList<QGraphicsItem *> overlapItems = selectedItem->collidingItems();

    qreal zValue = 0;
    foreach (QGraphicsItem *item, overlapItems) {
        if (item->zValue() >= zValue &&
            item->type() == DiagramItem::Type)
            zValue = item->zValue() + 0.1;
    }
    selectedItem->setZValue(zValue);
}
*/
void MainWindow::itemInserted(DiagramItem *item)
{
    //MODPOINT
    //block was inserted
    setWindowModified(true);

    pointerTypeGroup->button(int(DiagramScene::MoveItem))->setChecked(true);

    scene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));

    int id=d_blockRegistry->idOf(item->blockType());
    blockButtonGroup->button(id)->setChecked(false);

}

void MainWindow::itemMoved(QGraphicsItem *item)
{
    QString name=qgraphicsitem_cast<DiagramItem *>(item)->name();
    QPointF position=item->pos();

    d_ws->setBlockPosition(name,position);

    if(!isWindowModified())
        setWindowModified(true);
}

void MainWindow::textInserted(QGraphicsTextItem *)
{
    blockButtonGroup->button(InsertTextButton)->setChecked(false);
    scene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));
}

void MainWindow::sceneScaleChanged(const QString &scale)
{
    double newScale = scale.left(scale.indexOf(tr("%"))).toDouble() / 100.0;
    QMatrix oldMatrix = view->matrix();
    view->resetMatrix();
    view->translate(oldMatrix.dx(), oldMatrix.dy());
    view->scale(newScale, newScale);
}

/*
void MainWindow::lineColorChanged()
{
    lineAction = qobject_cast<QAction *>(sender());
    lineColorToolButton->setIcon(createColorToolButtonIcon(
                 ":/images/linecolor.png",
                 qVariantValue<QColor>(lineAction->data())));
    lineButtonTriggered();
}


//! [15]
void MainWindow::textButtonTriggered()
{
    scene->setTextColor(qVariantValue<QColor>(textAction->data()));
}
//! [15]

//! [16]
void MainWindow::fillButtonTriggered()
{
    scene->setItemColor(qVariantValue<QColor>(fillAction->data()));
}
//! [16]

//! [17]
void MainWindow::lineButtonTriggered()
{
    scene->setLineColor(qVariantValue<QColor>(lineAction->data()));
}
*/
void MainWindow::itemSelected(QGraphicsItem *item)
{
    DiagramTextItem *textItem =
    qgraphicsitem_cast<DiagramTextItem *>(item);

    QFont font = textItem->font();
    QColor color = textItem->defaultTextColor();

    qDebug() << "Selected item!";

}
//! [19]

//! [20]
void MainWindow::about()
{
    QMessageBox::about(this, tr("About Xenomai Lab"),
                       tr("<b>Xenomai Lab</b> version 0.1 Alpha Squared<BR><BR>Jorge Azevedo (C) 2011<BR>Universidade de Aveiro"));
}
//! [20]

void MainWindow::prepareEdit()
{
    if (scene->selectedItems().isEmpty())
        return;

    foreach (QGraphicsItem *item, scene->selectedItems()) {
         if (item->type() == DiagramItem::Type) {
             emit sendEdit(qgraphicsitem_cast<DiagramItem *>(item)->name());
         }
     }
}

void MainWindow::prepareSettings()
{
    if (scene->selectedItems().isEmpty())
        return;

    foreach (QGraphicsItem *item, scene->selectedItems()) {
         if (item->type() == DiagramItem::Type) {
             emit sendSettings(qgraphicsitem_cast<DiagramItem *>(item)->name());
         }
     }
}

void MainWindow::prepareGlobalSettings()
{
    emit sendSettings("root");

}

void MainWindow::setExecTerm(bool state)
{
    QString name;
    foreach (QGraphicsItem *item, scene->selectedItems()) {
         if (item->type() == DiagramItem::Type) {
             name=qgraphicsitem_cast<DiagramItem *>(item)->name();
         }
    }

    blockDiag->setExecInTerm(name,state);
    //qDebug() << "setExecTerm: should set " << name << "exec in term status to" << state;

}

void MainWindow::setExecSudo(bool state)
{
    QString name;
    foreach (QGraphicsItem *item, scene->selectedItems()) {
         if (item->type() == DiagramItem::Type) {
             name=qgraphicsitem_cast<DiagramItem *>(item)->name();
         }
    }

    blockDiag->setExecAsSudo(name,state);
}

void MainWindow::prepareAction()
{
    if(blockDiag->numBlocks()==1){
        errorMessage("MainWindow::prepareAction","Project is empty");
        return;
    }

    QAction* action = qobject_cast<QAction *>(sender());

    qDebug() << "Action is" << action->text();
    //QApplication::processEvents();


    if(action->text()!=cleanAction->text())
        cleanAction->setEnabled(false);
    else
        d_actionNum=BlockDiagram::Clean;
    if(action->text()!=makeAction->text())
        makeAction->setEnabled(false);
    else
        d_actionNum=BlockDiagram::Make;
    if(action->text()!=execAction->text())
        execAction->setEnabled(false);
    else
        d_actionNum=BlockDiagram::Exec;
    if(action->text()!=killAction->text())
        killAction->setEnabled(false);
    else
        d_actionNum=BlockDiagram::Kill;

    //If exec, let kill, lock diag
    if(d_actionNum==BlockDiagram::Exec){
        execAction->setEnabled(false);
        killAction->setEnabled(true);

        scene->lock();
    }

    if(d_actionNum==BlockDiagram::Make||d_actionNum==BlockDiagram::Exec){

        QString textString;
        switch(d_actionNum){
        case BlockDiagram::Make:
            textString="Building project...";
            break;
        case BlockDiagram::Exec:
            textString="Executing project...";
            break;
        }

        d_progress=new QProgressDialog(textString, "Abort", 0,blockDiag->numBlocks(),this);

        d_progress->setWindowModality(Qt::WindowModal);
        d_progress->setMinimumDuration(0);
        d_progress->setValue(0);
        d_progress->setCancelButton(0);
        d_progress->setMinimumSize(450,100);
        d_progress->show();

        d_index=1;

    }
    /*
     * This next emit is a Queued connection, so this function
     * will exit before the respective slot is executed.
     * This is not enough, however to force the window to repaint.
     * by directly calling process events, we're forcing the
     * buttons to disable before the emit. notice that this doesn't
     * work with DirectConnection. This function needs to exit
     * (apparetenly) so that the repaint actually happens
     */
    QApplication::processEvents();

    emit sendAction(d_actionNum);


    /*
    for(int i=0;i<BlockDiagram::numActions;++i){
        if(QObject::sender()==d_buttonActions[i])`
            action=i;
        else
            d_buttonActions[i]->setEnabled(false);

    }
    */
}

void MainWindow::processBlockError(QString blockName)
{
    //PROPOSED:
    if(d_executing){
        errorMessage("processBlockError","<i>"+blockName+"</i> has crashed! Project execution will now be terminated.<BR><BR>You might want to re-start execution with <i>"+blockName+"</i> running in a terminal so you can read is output.");
        killAction->trigger();
    }else{
        //PROPOSED:
        //d_errorDuringStartup=true;
        errorMessage("processBlockError","<i>"+blockName+"</i> has crashed during execution startup!");
    }
}

/**
  * Sent from BlockDiagram::exec() (emit finished)
  * ret is true if errors occured
  */
void MainWindow::backFromAction(bool errorsOccured)
{
    //we do this just in case there was an error and d_progress
    //didn't reach 100%, and therefore didn't exit normaly
    if(d_actionNum==BlockDiagram::Make||d_actionNum==BlockDiagram::Exec)
        d_progress->setValue(blockDiag->numBlocks());



    //PROPOSED:
    if(errorsOccured){
    //if there was an error picked up during progress report
    //if((d_results.size()>0)&&(d_results.last().processRet!=0)){

        QString textString;
        QString informativeString="<i>"+d_results.last().name+"</i> failed to ";
        switch(d_actionNum){
        case BlockDiagram::Clean:
            textString="Clean failed!";
            informativeString+="clean.";
            break;
        case BlockDiagram::Make:
            textString="Build failed!";
            informativeString+="compile.";
            break;
        case BlockDiagram::Exec:
            textString="Failed to execute project!";
            informativeString+="execute.";
            break;
        case BlockDiagram::Kill:
            textString="This never happens.";
            informativeString+="kill. How did it survive a SIGKILL? It must be from outter space, surely.";
            break;
        }



        QString detailedString;
        for(int i=d_results.size()-1;i>=0;i--)
            detailedString+="***"+d_results.at(i).name+"\n"+d_results.at(i).processOutput;

        QMessageBox msgBox(QMessageBox::Critical,"Error",textString,QMessageBox::Ok,this);
        msgBox.setMinimumSize(450,100);
        msgBox.setMaximumSize(450,100);
        //msgBox.setText(textString);
        msgBox.setInformativeText(informativeString);
        //msgBox.setIcon(QMessageBox::Critical);
        msgBox.setDetailedText(detailedString);
        //msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);

        //Ugly hack to force width on QMessageBox
        QSpacerItem* horizontalSpacer = new QSpacerItem(450, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        QGridLayout* layout = (QGridLayout*)msgBox.layout();
        layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

        msgBox.exec();
    }

    qDebug() << "*******Progress report";

    for(int i=0;i<d_results.size();i++){
        qDebug() << "Report for:" << d_results.at(i).name;
        qDebug() << d_results.at(i).processOutput;
        qDebug() << "returned:" << d_results.at(i).processRet;
        qDebug() << "**************";
    }
    qDebug() << "*******Final result is" << errorsOccured << "for action number" << d_actionNum;
    d_results.clear();


    //If this was a call to execution
    //and no errors ocurred
    if((d_actionNum==BlockDiagram::Exec)&&(!errorsOccured)){
        qDebug() << "Execution start up was sucessfull. All blocks should be running.";
        d_executing=true;
        return;
    }
    else{
        d_executing=false;
        scene->unlock();
        cleanAction->setEnabled(true);
        makeAction->setEnabled(true);
        execAction->setEnabled(true);
        killAction->setEnabled(true);
    }
}

void MainWindow::updateProgress(ProcessResult result)
{
    if(d_actionNum==BlockDiagram::Make||d_actionNum==BlockDiagram::Exec){
        d_progress->setValue(d_index);
        d_index++;
    }

    //qDebug() << "Progress report:";
    //qDebug() << result.processOutput;
    //qDebug() << "Returned:" << result.processRet;
    d_results.append(result);
}


void MainWindow::createToolBox()
{

    blockButtonGroup = new QButtonGroup(this);
    blockButtonGroup->setExclusive(false);
    connect(blockButtonGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(blockButtonGroupClicked(int)));

    QGridLayout *rtLayout = new QGridLayout;


    //For every rt block type, create a newblock button and add it
    //to the rt layout. j and k produce grid coordinates (j,0/1) of each
    //block
    int j=0,k=0;
    QStringList RTBlocks=d_blockRegistry->getAllRT();
    QString type;
    foreach(type,RTBlocks){
        rtLayout->addWidget(
        newBlockButton(type,d_blockRegistry->diagOf(type),
                       d_blockRegistry->idOf(type)),j,k);
        k++;
        if(k>1)
            k=0;
        else
            j--;
        j++;
    }

    rtLayout->setRowStretch(j+1, 10);
    rtLayout->setColumnStretch(2, 10);

    QWidget *rtWidget = new QWidget;
    rtWidget->setLayout(rtLayout);

    //NON REAL-TIME
    QGridLayout *nrtLayout = new QGridLayout;

    j=0;
    k=0;
    QStringList NRTBlocks=d_blockRegistry->getAllNRT();
    //QString type;
    foreach(type,NRTBlocks){
        nrtLayout->addWidget(
                    newBlockButton(type,d_blockRegistry->diagOf(type),
                                   d_blockRegistry->idOf(type)),j,k);
        k++;
        if(k>1)
            k=0;
        else
            j--;
        j++;
    }

    nrtLayout->setRowStretch(j+1, 10);
    nrtLayout->setColumnStretch(2, 10);

    QWidget *nrtWidget = new QWidget;
    nrtWidget->setLayout(nrtLayout);


    toolBox = new QToolBox;
    toolBox->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));
    toolBox->setMinimumWidth(rtWidget->sizeHint().width());
    toolBox->addItem(rtWidget, tr("Real-Time"));
    toolBox->addItem(nrtWidget, tr("Non Real-Time"));
}

void MainWindow::createActions()
{

//    deleteAction = new QAction(QIcon(":/images/delete.png"),
//                               tr("&Delete"), this);
    deleteAction = new QAction(QIcon::fromTheme("edit-delete"),tr("&Delete"), this);
    deleteAction->setShortcut(tr("Delete"));
    deleteAction->setStatusTip(tr("Delete item from diagram"));
    connect(deleteAction, SIGNAL(triggered()),
        this, SLOT(deleteItem()));

    exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcuts(QKeySequence::Quit);
    exitAction->setStatusTip(tr("Quit Xenomai Lab"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    aboutAction = new QAction(tr("A&bout"), this);
    aboutAction->setShortcut(tr("Ctrl+B"));
    connect(aboutAction, SIGNAL(triggered()),
            this, SLOT(about()));


    editAction = new QAction(QIcon::fromTheme("accessories-text-editor"),tr("&Edit"), this);
    editAction->setShortcut(tr("Ctrl+D"));
    editAction->setStatusTip(tr("Edit block"));
    connect(editAction, SIGNAL(triggered()),
            this,SLOT(prepareEdit()) );

    settingsAction = new QAction(QIcon::fromTheme("preferences-system"),tr("&Settings"), this);
    settingsAction->setShortcut(tr("Ctrl+G"));
    settingsAction->setStatusTip(tr("Edit block settings"));
    connect(settingsAction, SIGNAL(triggered()),
            this,SLOT(prepareSettings()) );

    globalOptionsAction = new QAction(QIcon::fromTheme("x-office-calendar"),tr("Sampling &Period"), this);
    globalOptionsAction->setShortcut(tr("Ctrl+L"));
    globalOptionsAction->setStatusTip(tr("Edit Sampling Period"));
    connect(globalOptionsAction, SIGNAL(triggered()),
            this,SLOT(prepareGlobalSettings()) );



    cleanAction = new QAction(QIcon::fromTheme("edit-clear"),tr("&Clean"), this);
    cleanAction->setShortcut(tr("Ctrl+W"));
    cleanAction->setStatusTip(tr("Clean last build"));
    connect(cleanAction, SIGNAL(triggered()),
            this,SLOT(prepareAction()) );

    makeAction = new QAction(QIcon::fromTheme("system-run"),tr("&Make"), this);
    makeAction->setShortcut(tr("Ctrl+E"));
    makeAction->setStatusTip(tr("Build current project"));
    connect(makeAction, SIGNAL(triggered()),
            this,SLOT(prepareAction()) );

    execAction = new QAction(QIcon::fromTheme("media-playback-start"),tr("&Execute"), this);
    execAction->setShortcut(tr("Ctrl+R"));
    execAction->setStatusTip(tr("Execute current project in real-time"));
    connect(execAction, SIGNAL(triggered()),
            this,SLOT(prepareAction()) );

    killAction = new QAction(QIcon::fromTheme("media-playback-stop"),tr("&Kill"), this);
    killAction->setShortcut(tr("Ctrl+T"));
    killAction->setStatusTip(tr("Kill all blocks currently running"));
    connect(killAction, SIGNAL(triggered()),
            this,SLOT(prepareAction()) );




    //File
    newAction = new QAction(QIcon::fromTheme("document-new"),tr("&New"), this);
    newAction->setShortcut(QKeySequence::New);
    newAction->setStatusTip(tr("Create new project"));
    connect(newAction, SIGNAL(triggered()),
            this,SLOT(newFile()) );

    openAction = new QAction(QIcon::fromTheme("document-open"),tr("&Open"), this);
    openAction->setShortcut(QKeySequence::Open);
    openAction->setStatusTip(tr("Open project"));
    connect(openAction, SIGNAL(triggered()),
            this,SLOT(openFile()) );

    saveAction = new QAction(QIcon::fromTheme("document-save"),tr("&Save"), this);
    saveAction->setShortcut(QKeySequence::Save);
    saveAction->setStatusTip(tr("Save project"));
    connect(saveAction, SIGNAL(triggered()),
            this,SLOT(save()) );

    saveAsAction = new QAction(QIcon::fromTheme("document-save-as"),tr("&Save as"), this);
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    saveAsAction->setStatusTip(tr("Save project as a new file"));
    connect(saveAsAction, SIGNAL(triggered()),
            this,SLOT(saveAs()) );

    for(int i=0; i < MaxRecentFiles; ++i){
        recentFileActions[i] = new QAction(this);
        recentFileActions[i]->setVisible(false);
        connect(recentFileActions[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()) );
    }

}

//! [24]
void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);

    separatorAction = fileMenu->addSeparator();
    for(int i=0; i < MaxRecentFiles; i++)
        fileMenu->addAction(recentFileActions[i]);

    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    toolsMenu = menuBar()->addMenu(tr("&Tools"));
    toolsMenu->addAction(cleanAction);
    toolsMenu->addAction(makeAction);
    toolsMenu->addAction(execAction);
    toolsMenu->addAction(killAction);


    //itemMenu = menuBar()->addMenu(tr("&Item"));
    itemMenu = new QMenu();
    itemMenu->addAction(editAction);
    itemMenu->addAction(settingsAction);
    itemMenu->addSeparator();
    itemMenu->addAction(deleteAction);
    //itemMenu->addSeparator();
    //itemMenu->addAction(execInTermAction);

    aboutMenu = menuBar()->addMenu(tr("&Help"));
    aboutMenu->addAction(aboutAction);
}


void MainWindow::createToolbars()
{

    diagramToolbar = addToolBar(tr("Actions"));
    diagramToolbar->addAction(cleanAction);
    diagramToolbar->addAction(makeAction);
    diagramToolbar->addAction(execAction);
    diagramToolbar->addAction(killAction);

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(deleteAction);
    editToolBar->addAction(editAction);
    editToolBar->addAction(settingsAction);
    editToolBar->addAction(globalOptionsAction);

    QToolButton *pointerButton = new QToolButton;
    pointerButton->setCheckable(true);
    pointerButton->setChecked(true);
    pointerButton->setIcon(QIcon(":/images/pointer.png"));
    QToolButton *linePointerButton = new QToolButton;
    linePointerButton->setCheckable(true);
    linePointerButton->setIcon(QIcon(":/images/linepointer.png"));

    pointerTypeGroup = new QButtonGroup(this);
    pointerTypeGroup->addButton(pointerButton, int(DiagramScene::MoveItem));
    pointerTypeGroup->addButton(linePointerButton,
                                int(DiagramScene::InsertLine));
    connect(pointerTypeGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(pointerGroupClicked(int)));

    sceneScaleCombo = new QComboBox;
    QStringList scales;
    scales << tr("50%") << tr("75%") << tr("100%") << tr("125%") << tr("150%");
    sceneScaleCombo->addItems(scales);
    sceneScaleCombo->setCurrentIndex(1);
    connect(sceneScaleCombo, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(sceneScaleChanged(QString)));

    pointerToolbar = addToolBar(tr("Pointer type"));
    pointerToolbar->addWidget(pointerButton);
    pointerToolbar->addWidget(linePointerButton);
    pointerToolbar->addWidget(sceneScaleCombo);

}

QWidget *MainWindow::newBlockButton(const QString &type,
                      DiagramItem::DiagramType diagType,int index)
{

    DiagramItem item("irrelevant","name",diagType);
    QIcon icon(item.image());

    QToolButton *button = new QToolButton;
    button->setIcon(icon);
    button->setIconSize(QSize(50, 50));
    button->setCheckable(true);

    blockButtonGroup->addButton(button, index);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(button, 0, 0, Qt::AlignHCenter);
    layout->addWidget(new QLabel(type), 1, 0, Qt::AlignCenter);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    return widget;
}

//! [29]

//! [30]
QMenu *MainWindow::createColorMenu(const char *slot, QColor defaultColor)
{
    QList<QColor> colors;
    colors << Qt::black << Qt::white << Qt::red << Qt::blue << Qt::yellow;
    QStringList names;
    names << tr("black") << tr("white") << tr("red") << tr("blue")
          << tr("yellow");

    QMenu *colorMenu = new QMenu(this);
    for (int i = 0; i < colors.count(); ++i) {
        QAction *action = new QAction(names.at(i), this);
        action->setData(colors.at(i));
        action->setIcon(createColorIcon(colors.at(i)));
        connect(action, SIGNAL(triggered()),
                this, slot);
        colorMenu->addAction(action);
        if (colors.at(i) == defaultColor) {
            colorMenu->setDefaultAction(action);
        }
    }
    return colorMenu;
}
//! [30]

//! [31]
QIcon MainWindow::createColorToolButtonIcon(const QString &imageFile,
                        QColor color)
{
    QPixmap pixmap(50, 80);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    QPixmap image(imageFile);
    QRect target(0, 0, 50, 60);
    QRect source(0, 0, 42, 42);
    painter.fillRect(QRect(0, 60, 50, 80), color);
    painter.drawPixmap(target, image, source);

    return QIcon(pixmap);
}
//! [31]

//! [32]
QIcon MainWindow::createColorIcon(QColor color)
{
    QPixmap pixmap(20, 20);
    QPainter painter(&pixmap);
    painter.setPen(Qt::NoPen);
    painter.fillRect(QRect(0, 0, 20, 20), color);

    return QIcon(pixmap);
}
//! [32]

void MainWindow::errorMessage(QString Catcher, QStringList err)
{
    QMessageBox::critical(this,"Error",err[1],QMessageBox::Ok);
    qDebug() << "Run-time exception!";
    qDebug() << "Catcher: "+Catcher;
    qDebug() << "Thrower: "+err[0];
    qDebug() << "Message: "+err[1];
}

void MainWindow::errorMessage(QString Origin, QString message)
{
    QMessageBox::critical(this,"Error",message,QMessageBox::Ok);
    qDebug() << Origin << " - " << message;
}

void MainWindow::closeEvent(QCloseEvent * event)
{

    //TODO:
    //if running project
    if(okToContinue()){
        writeSettings();
        //write settings
        event->accept();
    }
    else
        event->ignore();
}

void MainWindow::readSettings()
{
    QSettings settings("Universidade de Aveiro","XenomaiLab");

    restoreGeometry(settings.value("geometry").toByteArray());
    d_recentFiles = settings.value("recentFiles").toStringList();
    d_defaultEditor = settings.value("editor","vim").toString();
    updateRecentFileActions();
}

void MainWindow::writeSettings()
{
    QSettings settings("Universidade de Aveiro","XenomaiLab");
    settings.setValue("recentFiles",d_recentFiles);
    settings.setValue("geometry",saveGeometry());

}
