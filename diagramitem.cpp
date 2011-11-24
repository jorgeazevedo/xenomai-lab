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

#include "diagramitem.h"
#include "linepath.h"

DiagramItem::DiagramItem(const SuperBlock &block, DiagramType diagramType, QMenu *contextMenu,
                         QGraphicsItem *parent, QGraphicsScene *scene)
: QGraphicsPolygonItem(parent, scene)
{
    myScene=scene;
    myBlockType = block.type();
    myName = block.name();
    myDiagramType = diagramType;
    myContextMenu = contextMenu;
    //Type should be written with first letter up
    QString blockTypeText= block.type().toUpper().at(0)+
                        block.type().right(block.type().size()-1);
    QString blockNameText=block.name();

    drawPoligon(myDiagramType);

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    QFont textFont("Helvetica", 15);

    blockTypeLabel = new QGraphicsTextItem(blockTypeText,parent,scene);
    blockTypeLabel->setFont(textFont);
    blockTypeLabel->setParentItem(this);

    nameLabel=new QGraphicsTextItem(blockNameText,parent,scene);
    nameLabel->setFont(textFont);
    nameLabel->setParentItem(this);

    blockTypeLabel->setPos(-blockTypeLabel->boundingRect().width()/2,
                      -blockTypeLabel->boundingRect().height()/2);
    nameLabel->setPos(-nameLabel->boundingRect().width()/2,
                      this->boundingRect().height()/2);

    //We want mouse position to be top left corner of block,
    //not center!
    //qreal x=block.X()-boundingRect().x();
    //qreal y=block.Y()-boundingRect().y();
    setPos(QPointF(block.X(),block.Y()));

    QColor backgroundColor=Qt::white;
    setBrush(backgroundColor);

}

DiagramItem::DiagramItem(QString blockType, QString name, DiagramType diagramType, QMenu *contextMenu,
             QGraphicsItem *parent, QGraphicsScene *scene)
    : QGraphicsPolygonItem(parent, scene), myName(name), myBlockType(blockType)
{
    myDiagramType = diagramType;

    myContextMenu = contextMenu;
    //myContextMenu = contextMenu;

    //contextMenu->actions();
/*
    //set execInTerm action to value of block
    QList<QAction*> actions=myContextMenu->actions();
    int i;
    for (i = 0; i < actions.size(); ++i) {
            if(actions.at(i)->statusTip() == "Block executes within a terminal window")
                break;

    }
    if(i!=actions.size())
        myContextMenu->removeAction(actions.at(i));
*/
    //QString final=

    blockTypeLabel = new QGraphicsTextItem(myBlockType.toUpper().at(0)+myBlockType.right(myBlockType.size()-1),parent,scene);
    blockTypeLabel->setFont(*new QFont("Helvetica", 15));
    blockTypeLabel->setParentItem(this);

    nameLabel=new QGraphicsTextItem(myName,parent,scene);
    nameLabel->setFont(*new QFont("Helvetica", 15));
    //nameLabel->setZValue(1000.0);
    nameLabel->setParentItem(this);
    //nameLabel->setPos(-nameLabel->boundingRect().width()/2,
    //                  -nameLabel->boundingRect().height()/2);


    drawPoligon(myDiagramType);

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    blockTypeLabel->setPos(-blockTypeLabel->boundingRect().width()/2,
                      -blockTypeLabel->boundingRect().height()/2);
    nameLabel->setPos(-nameLabel->boundingRect().width()/2,
                      this->boundingRect().height()/2);
}

void DiagramItem::drawPoligon(DiagramType type)
{
    QPainterPath path;
    switch (type) {
        case StartEnd:
            path.moveTo(200, 50);
            path.arcTo(150, 0, 50, 50, 0, 90);
            path.arcTo(50, 0, 50, 50, 90, 90);
            path.arcTo(50, 50, 50, 50, 180, 90);
            path.arcTo(150, 50, 50, 50, 270, 90);
            path.lineTo(200, 25);
            myPolygon = path.toFillPolygon();
            break;
        case Conditional: case Triangle:
            myPolygon << QPointF(-40, -50) << QPointF(-40, 50)
                      << QPointF(40, 0) << QPointF(-40, -50);
            break;
        case Step: case Square:
            myPolygon << QPointF(-80, -40) << QPointF(80, -40)
                      << QPointF(80, 40) << QPointF(-80, 40)
                      << QPointF(-80, -40);
            break;
        default:
            myPolygon << QPointF(-120, -80) << QPointF(-70, 80)
                      << QPointF(120, 80) << QPointF(70, -80)
                      << QPointF(-120, -80);
            break;
    }
    setPolygon(myPolygon);
}

void DiagramItem::removeLinePath(LinePath *linePath)
{
    /*
    int index = linePaths.indexOf(linePath);

    if (index != -1)
        linePaths.removeAt(index);
    */

    linePathsIn.removeAll(linePath);
    linePathsOut.removeAll(linePath);
}

void DiagramItem::removeLinePaths()
{
    foreach (LinePath *linePath, linePathsIn) {
        linePath->startItem()->removeLinePath(linePath);
        linePath->endItem()->removeLinePath(linePath);
        scene()->removeItem(linePath);
        delete linePath;
    }
    foreach (LinePath *linePath, linePathsOut) {
        linePath->startItem()->removeLinePath(linePath);
        linePath->endItem()->removeLinePath(linePath);
        scene()->removeItem(linePath);
        delete linePath;
    }
}



void DiagramItem::addLinePathIn(LinePath *linePath)
{
    linePathsIn.append(linePath);
}

void DiagramItem::addLinePathOut(LinePath *linePath)
{
    linePathsOut.append(linePath);
}

//! [3]

//! [4]
QPixmap DiagramItem::image() const
{
    QPixmap pixmap(250, 250);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setPen(QPen(Qt::black, 8));
    painter.translate(125, 125);
    painter.drawPolyline(myPolygon);

    return pixmap;
}

QString DiagramItem::blockType() const
{
    return myBlockType;
}

//! [4]

//! [5]
void DiagramItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    static bool executesInTerm=false;
    scene()->clearSelection();
    setSelected(true);
/*
    //set execInTerm action to value of block
    QList<QAction*> actions=myContextMenu->actions();
    int i;
    for (i = 0; i < actions.size(); ++i) {
            if(actions.at(i)->statusTip() == "Block executes within a terminal window")
                break;

    }

    bool old=actions.at(i)->isChecked();

    actions.at(i)->setChecked(executesInTerm);

*/
    myContextMenu->exec(event->screenPos());
    qDebug() << "contextMenuEvent: returned";
  //  actions.at(i)->setChecked(old);
}
//! [5]

void DiagramItem::paintLinePaths(const QColor &color, bool highlight)
{
    LinePath* linePath;
    foreach(linePath,linePathsIn){
        linePath->setColor(color);
        if(highlight)
            linePath->setZValue(-999.0);
        else
            linePath->setZValue(-1000.0);
    }
    foreach(linePath,linePathsOut){
        linePath->setColor(color);
        if(highlight)
            linePath->setZValue(-999.0);
        else
            linePath->setZValue(-1000.0);
    }
}

QVariant DiagramItem::itemChange(GraphicsItemChange change,
                     const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionChange) {

        foreach (LinePath *linePath, linePathsIn) {
            linePath->updatePosition();
        }
        foreach (LinePath *linePath, linePathsOut) {
            linePath->updatePosition();
        }
    }

    if (change == QGraphicsItem::ItemSelectedHasChanged){
        if(value==true){
            paintLinePaths(QColor(255,0,0),true);
            myScene->update();
            //qDebug() << "Selected" << name();
        }
        else{
            paintLinePaths(QColor(255,255,255),false);
            myScene->update();
            //qDebug() << "Deselected" << name();
        }

    }


    return value;
}
//! [6]

QString DiagramItem::name()
{
    return myName;
}

qreal DiagramItem::getEntryHeight(LinePath *linePath)
{
    qreal entryHeight;
    qreal base;
    qDebug() << myName << "::getEntryHeight for line called" << linePath->name();
    qDebug() << "***LIST***";

    foreach(LinePath* lp,linePathsIn){
        qDebug() << lp->name();
    }
    qDebug() << "**/LIST***";
    qDebug() << "index is " << linePathsIn.indexOf(linePath);

    switch(linePathsIn.size()){
        case 1:
            entryHeight=pos().y();
            break;
        case 2:
            base=pos().y()+15;
            entryHeight=base-30*linePathsIn.indexOf(linePath);
            break;
        case 3:
            base=pos().y()+25;
            entryHeight=base-25*linePathsIn.indexOf(linePath);
            break;
        case 4:
            base=pos().y()+30;
            entryHeight=base-20*linePathsIn.indexOf(linePath);
            break;
        case 5:
            base=pos().y()+40;
            entryHeight=base-20*linePathsIn.indexOf(linePath);
            break;
        default:
            base=pos().y()+40;
            entryHeight=base-20*linePathsIn.indexOf(linePath);
            if(entryHeight<(base-80))
                entryHeight=(base-80);
            break;
    }

    return entryHeight;
}
