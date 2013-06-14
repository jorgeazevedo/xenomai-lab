/*
 * Xenomai Lab
 * Copyright (C) 2013  Jorge Azevedo
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

#include "diagramscene.h"


//#define ROUND(x) ((x)>=0?(int)((x)+0.5):(int)((x)-0.5))

//! [0]
DiagramScene::DiagramScene(BlockDiagram* bd, enum Qt::GlobalColor BackgroundColor, QObject *parent)
    : QGraphicsScene(parent)
{
    setBackgroundBrush(BackgroundColor);
    d_blockDiag=bd;
    //myItemMenu = itemMenu;
    myMode = MoveItem;
    myItemType = DiagramItem::Step;
    line = 0;
    textItem = 0;
    //d_delta=0;
    //d_centerPoint=0;
    d_lock=false;
    myItemColor = Qt::white;
    myTextColor = Qt::black;
    myLineColor = Qt::black;

}
//! [0]

//! [1]
void DiagramScene::setLineColor(const QColor &color)
{
    myLineColor = color;
    if (isItemChange(LinePath::Type)) {
        LinePath *item =
            qgraphicsitem_cast<LinePath *>(selectedItems().first());
        item->setColor(myLineColor);
        update();
    }
}
//! [1]

//! [2]
void DiagramScene::setTextColor(const QColor &color)
{
    myTextColor = color;
    if (isItemChange(DiagramTextItem::Type)) {
        DiagramTextItem *item =
            qgraphicsitem_cast<DiagramTextItem *>(selectedItems().first());
        item->setDefaultTextColor(myTextColor);
    }
}
//! [2]

//! [3]
void DiagramScene::setItemColor(const QColor &color)
{
    myItemColor = color;
    if (isItemChange(DiagramItem::Type)) {
        DiagramItem *item =
            qgraphicsitem_cast<DiagramItem *>(selectedItems().first());
        item->setBrush(myItemColor);
    }
}
//! [3]

//! [4]
void DiagramScene::setFont(const QFont &font)
{
    myFont = font;

    if (isItemChange(DiagramTextItem::Type)) {
        QGraphicsTextItem *item =
            qgraphicsitem_cast<DiagramTextItem *>(selectedItems().first());
        //At this point the selection can change so the first selected item might not be a DiagramTextItem
        if (item)
            item->setFont(myFont);
    }
}
//! [4]

void DiagramScene::lock(void)
{
    setBackgroundBrush(Qt::lightGray);
    d_lock=true;
}


void DiagramScene::unlock(void)
{
    setBackgroundBrush(Qt::white);
    d_lock=false;
}

bool DiagramScene::isLocked()
{
    return d_lock;
}

void DiagramScene::setMode(Mode mode)
{
    myMode = mode;
}

void DiagramScene::setItem(QString blockType, QString name, bool RT, DiagramItem::DiagramType type, QMenu* menu)
{
    qDebug() << "Block is of type" << blockType << ", called" << name << ", with" << RT << "rt, and diagramtype" << type;
    myItemBlockType=blockType;
    myItemRT=RT;
    myItemType = type;
    myItemName= name;
    myItemMenu=menu;
}

//! [5]
void DiagramScene::editorLostFocus(DiagramTextItem *item)
{
    QTextCursor cursor = item->textCursor();
    cursor.clearSelection();
    item->setTextCursor(cursor);

    if (item->toPlainText().isEmpty()) {
        removeItem(item);
        item->deleteLater();
    }
}



void DiagramScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
  if(!isLocked()){
    if (mouseEvent->button() != Qt::LeftButton)
        return;

    qreal x=mouseEvent->scenePos().x();
    qreal y=mouseEvent->scenePos().y();

    switch (myMode) {
        case InsertItem:

            emit insertItem(x,y);

            break;

        case InsertLine:
            line = new QGraphicsLineItem(QLineF(mouseEvent->scenePos(),
                                        mouseEvent->scenePos()));
            line->setPen(QPen(myLineColor, 2));
            addItem(line);
            break;

        case InsertText:
            textItem = new DiagramTextItem();
            textItem->setFont(myFont);
            textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
            textItem->setZValue(1000.0);
            connect(textItem, SIGNAL(lostFocus(DiagramTextItem*)),
                    this, SLOT(editorLostFocus(DiagramTextItem*)));
            connect(textItem, SIGNAL(selectedChange(QGraphicsItem*)),
                    this, SIGNAL(itemSelected(QGraphicsItem*)));
            addItem(textItem);
            textItem->setDefaultTextColor(myTextColor);
            textItem->setPos(mouseEvent->scenePos());
            emit textInserted(textItem);
    case MoveItem:
        break;
    default:
        break;
    }



  }
  QGraphicsScene::mousePressEvent(mouseEvent);
}

void DiagramScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
  if(!isLocked()){
      QGraphicsItem* it=mouseGrabberItem();

      if(it!=0){
          //If GraphicsItem is a DiagramItem
          if(it->type()== (it->UserType+15) )
            emit itemMoved(it);
      }

    if (myMode == InsertLine && line != 0) {
        QLineF newLine(line->line().p1(), mouseEvent->scenePos());
        line->setLine(newLine);
    } else if (myMode == MoveItem) {

           QGraphicsScene::mouseMoveEvent(mouseEvent);
    }
  }
}
//! [10]

//! [11]
void DiagramScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
  if(!isLocked()){
    //Strip lines and possible labels from selection
    if (line != 0 && myMode == InsertLine) {
        QList<QGraphicsItem *> startItems = items(line->line().p1());
        foreach(QGraphicsItem* item,startItems){
            //if QGraphicsTextItem (the name of the block) or Line
            if(item->type()==8 || item->type()==6)
                startItems.removeAll(item);
        }
        QList<QGraphicsItem *> endItems = items(line->line().p2());
        foreach(QGraphicsItem* item,endItems){
            //if QGraphicsTextItem
            if(item->type()==8 || item->type()==6)
                endItems.removeAll(item);
        }

        removeItem(line);
        delete line;

        if (startItems.count() > 0 && endItems.count() > 0 &&
            startItems.first()->type() == DiagramItem::Type &&
            endItems.first()->type() == DiagramItem::Type &&
            startItems.first() != endItems.first()) {
            DiagramItem *startItem =
                qgraphicsitem_cast<DiagramItem *>(startItems.first());
            DiagramItem *endItem =
                qgraphicsitem_cast<DiagramItem *>(endItems.first());
            //insert LinePath

            emit insertLine(startItem, endItem);

        }
    }
//! [12] //! [13]
    line = 0;

  }
  QGraphicsScene::mouseReleaseEvent(mouseEvent);
}
//! [13]

//! [14]
bool DiagramScene::isItemChange(int type)
{
    foreach (QGraphicsItem *item, selectedItems()) {
        if (item->type() == type)
            return true;
    }
    return false;
}
