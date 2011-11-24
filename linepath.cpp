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

#include "linepath.h"

LinePath::LinePath(DiagramItem *startItem, DiagramItem *endItem,
                   const QString& name,QGraphicsItem * parent)
    :QGraphicsPathItem(parent),d_name(name)
{
    //Definitions
    lineThickness=4;
    arrowWidth=15;
    arrowHeight=10;

    QFont helvetica15("Helvetica",15);

    d_startItem=startItem;
    d_endItem=endItem;

    d_color = Qt::white;

    setFlag(QGraphicsItem::ItemIsSelectable, true);

    setZValue(1000.0);

    d_nameTextItem=new QGraphicsTextItem(d_name,this);
    d_nameTextItem->setFont(helvetica15);
    //textLabel->setZValue(1000.0);
    d_nameTextItem->setParentItem(this);
    d_nameTextItem->setPos(0,0);

    setPath(d_path);


}

LinePath::~LinePath()
{

}

void LinePath::updatePosition()
{
    //Clear current path
    d_path = QPainterPath();

    if(isZone2()){
        QPointF textPos=zone2();
        d_nameTextItem->setPos(textPos);
        d_nameTextItem->setX(d_nameTextItem->x()+8);
    }
    else{
        QPointF textPos=zone1();
        d_nameTextItem->setPos(textPos);
    }


    //textLabel->setPos(((line.x2()-line.x1())/2)+line.x1(),((line.y2()-line.y1())/2)+line.y1());
    //d_nameTextItem->setPos(getStartPoint());
    setPath(d_path);
}

QRectF LinePath::boundingRect() const
{

    return QGraphicsPathItem::boundingRect();
    //return QRectF(0, 0, 1000, 700);
}

void LinePath::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
          QWidget *)
{
    if (d_startItem->collidesWithItem(d_endItem))
        return;

    //Clear current path
    d_path = QPainterPath();
    if(isZone2())
        zone2();
    else
        zone1();

    QColor fillColor;
    if(isSelected())
        fillColor = Qt::black;
    else
        fillColor = d_color;

    painter->setBrush(fillColor);

    painter->setPen(pen());
    painter->drawPath(d_path);

}

QPointF LinePath::getStartPoint() const
{
    QPointF startPoint=d_startItem->pos();
    startPoint.setX(startPoint.x()+d_startItem->boundingRect().width()/2);

    return startPoint;
}



QPointF LinePath::zone1()
{

    //Essential points and distances
    QPointF startPoint=getStartPoint();
    qreal startx=startPoint.x();
    qreal starty=startPoint.y()-lineThickness/2;
    qreal endx=startx;
    qreal endy=startPoint.y()+lineThickness/2;

    qreal distanceBetweenBlockEdgesX=d_endItem->pos().x()-d_startItem->pos().x()
                                -d_endItem->boundingRect().width()/2
                                -d_startItem->boundingRect().width()/2;

    qreal distanceBetweenBlockCenterY=d_startItem->pos().y()
                                    -d_endItem->getEntryHeight(this);

    //Each LinePath in zone 1 is divided into 5 lines
    //              _____5______
    //            4|
    //             |_____3______
    //                          |2
    //                  ____1___|
    //Each line is actually 2 lines apart lineThickness.
    //The alghorithm for drawing the LinePath is always the same,
    //what varies is the value of the line width. That's why we
    //calculate the values first, depending on distanceBetweenBlockCenterY

    d_path.moveTo(startx,starty);

    qreal line1Width1,line2Width1,line4Width1,line5Width1;
    qreal line1Width2,line2Width2,line4Width2,line5Width2;
    qreal line3Width=0;

    qreal line1Width=distanceBetweenBlockEdgesX/2;
    if(line1Width<30){
        line3Width=(30-line1Width)*2;
        line1Width=30;
    }

    if(distanceBetweenBlockCenterY>=0){
        line1Width1=line1Width;
        line2Width1=distanceBetweenBlockCenterY/2-lineThickness;
        //Bellow zero line is drawn inside the arrow, so we kill it
        if(line2Width1<0)
            line2Width1=0;
        line4Width1=distanceBetweenBlockCenterY/2+lineThickness;

        //Bellow this value (two arrow bodies stacked)
        //line should be equal to heigh difference
        if(line4Width1 <= (2*lineThickness) )
            line4Width1=distanceBetweenBlockCenterY;
        line5Width1=line1Width;

        line1Width2=line1Width+lineThickness;
        line2Width2=distanceBetweenBlockCenterY/2+lineThickness;

        if(line2Width2 <= (2*lineThickness) )
            line2Width2=distanceBetweenBlockCenterY;
        line4Width2=distanceBetweenBlockCenterY/2-lineThickness;

        if(line4Width2<0)
            line4Width2=0;
        line5Width2=line1Width;

    }
    else{
        line1Width1=line1Width+lineThickness;
        line2Width1=distanceBetweenBlockCenterY/2-lineThickness;
        if(line2Width1 > (-2*lineThickness) )
            line2Width1=distanceBetweenBlockCenterY;

        line4Width1=distanceBetweenBlockCenterY/2+lineThickness;
        if(line4Width1>=0)
            line4Width1=0;

        line5Width1=line1Width-lineThickness;

        line1Width2=line1Width;
        line2Width2=distanceBetweenBlockCenterY/2+lineThickness;
        if(line2Width2>=0)
            line2Width2=0;

        line4Width2=distanceBetweenBlockCenterY/2-lineThickness;
        if(line4Width2 > (-2*lineThickness) )
            line4Width2=distanceBetweenBlockCenterY;

        line5Width2=line1Width+lineThickness;
    }

    //Start line1
    d_path.lineTo(startx+line1Width1,
                  starty);

    //Start Line2
    d_path.lineTo(d_path.currentPosition().x(),
                  d_path.currentPosition().y()-line2Width1);


    //Start Line3
    d_path.lineTo(d_path.currentPosition().x()-line3Width,
                  d_path.currentPosition().y());

    QPointF ret=d_path.currentPosition();

    //Start Line4
    d_path.lineTo(d_path.currentPosition().x(),
                  d_path.currentPosition().y()-line4Width1);

    //Start Line5 (with arrowhead)
    d_path.lineTo(d_path.currentPosition().x()+line5Width1-arrowWidth,
                  d_path.currentPosition().y());
    d_path.lineTo(d_path.currentPosition().x(),
                  d_path.currentPosition().y()-arrowHeight);
    d_path.lineTo(d_path.currentPosition().x()+arrowWidth,
                  d_path.currentPosition().y()+arrowHeight+lineThickness/2);

    //Reverse Line5
    d_path.lineTo(d_path.currentPosition().x()-arrowWidth,
                  d_path.currentPosition().y()+arrowHeight+lineThickness/2);
    d_path.lineTo(d_path.currentPosition().x(),
                  d_path.currentPosition().y()-arrowHeight);
    d_path.lineTo(d_path.currentPosition().x()-(line5Width2-arrowWidth-lineThickness),
                  d_path.currentPosition().y());

    //Reverse Line4
    d_path.lineTo(d_path.currentPosition().x(),
                  d_path.currentPosition().y()+line4Width2);

    //Reverse Line3
    d_path.lineTo(d_path.currentPosition().x()+line3Width,
                  d_path.currentPosition().y());

    //Reverse Line2
    d_path.lineTo(d_path.currentPosition().x(),
                  d_path.currentPosition().y()+line2Width2);

    //Reverse Line1
    d_path.lineTo(endx,endy);

    //close it
    d_path.closeSubpath();

    return ret;
}


QPointF LinePath::zone2(){

    //Essential points and distances
    QPointF startPoint=getStartPoint();
    qreal startx=startPoint.x();
    qreal starty=startPoint.y()-lineThickness/2;
    qreal endx=startx;
    qreal endy=startPoint.y()+lineThickness/2;

    qreal distanceBetweenBlockCenterY=d_startItem->pos().y()
                                    -d_endItem->getEntryHeight(this);


    //Each LinePath in zone 2 is divided into 5 lines
    //
    //    _________3____________
    //  4|                      |2
    //   |____5__       ____1___|
    //
    //Each line is actually 2 lines apart lineThickness.
    //The alghorithm for drawing the LinePath is always the same,
    //what varies is the value of the line width. That's why we
    //calculate the values first, depending on distanceBetweenBlockCenterY

    d_path.moveTo(startx,starty);

    qreal line1Width=30;
    qreal line2Width1,line2Width2=0;
    qreal line4Width1,line4Width2=0;

    qreal line5Width1=30;
    qreal line5Width2;

    qreal line3Width1=30+d_startItem->boundingRect().width()/2
                    +(d_startItem->pos().x()-d_endItem->pos().x())
                    +30+d_endItem->boundingRect().width()/2;

    qreal line3Width2;

    if(distanceBetweenBlockCenterY>=0){
        line2Width1=d_endItem->boundingRect().height()/2+50
                +(distanceBetweenBlockCenterY -lineThickness/2);
        line4Width1=d_startItem->boundingRect().height()/2+50;

        line5Width2=line5Width1+2*lineThickness;
        line4Width2=-(line4Width1+2*lineThickness);

        line3Width2=line3Width1+2*lineThickness;
        line2Width2=-(line2Width1+2*lineThickness);

    }
    else{
        line2Width1=d_endItem->boundingRect().height()/2+50
                -(distanceBetweenBlockCenterY -lineThickness/2);
        line2Width1=-line2Width1;
        line4Width1=d_startItem->boundingRect().height()/2+50;
        line4Width1=-line4Width1;

        line3Width2=line3Width1-2*lineThickness;
        line5Width2=30;
        line4Width2=-line4Width1-2*lineThickness;
        line2Width2=-line2Width1-2*lineThickness;

    }




    //Start line1
    d_path.lineTo(startx+line1Width,
                  starty);

    //Start Line2
    d_path.lineTo(d_path.currentPosition().x(),
                  d_path.currentPosition().y()-line2Width1);

    //Start Line3
    d_path.lineTo(d_path.currentPosition().x()-line3Width1,
                  d_path.currentPosition().y());

    QPointF ret=d_path.currentPosition();

    //Start Line4
    d_path.lineTo(d_path.currentPosition().x(),
                  d_path.currentPosition().y()+line4Width1);\

    //Start Line5 (with arrowhead)
    d_path.lineTo(d_path.currentPosition().x()+line5Width1-arrowWidth,
                  d_path.currentPosition().y());
    d_path.lineTo(d_path.currentPosition().x(),
                  d_path.currentPosition().y()-arrowHeight);
    d_path.lineTo(d_path.currentPosition().x()+arrowWidth,
                  d_path.currentPosition().y()+arrowHeight+lineThickness/2);

    //Reverse Line5
    d_path.lineTo(d_path.currentPosition().x()-arrowWidth,
                  d_path.currentPosition().y()+arrowHeight+lineThickness/2);
    d_path.lineTo(d_path.currentPosition().x(),
                  d_path.currentPosition().y()-arrowHeight);
    d_path.lineTo(d_path.currentPosition().x()-(line5Width2-arrowWidth-lineThickness),
                  d_path.currentPosition().y());



    //Reverse Line4
    d_path.lineTo(d_path.currentPosition().x(),
                  d_path.currentPosition().y()+line4Width2);

    //Reverse Line3
    d_path.lineTo(d_path.currentPosition().x()+line3Width2,
                  d_path.currentPosition().y());

    //Reverse Line2
    d_path.lineTo(d_path.currentPosition().x(),
                  d_path.currentPosition().y()-line2Width2);

    //Reverse Line1
    d_path.lineTo(endx,endy);

    //close it
    d_path.closeSubpath();

    return ret;
}

bool LinePath::isZone2()
{
    //These calculations really need an illustration to be properly understood
    qreal ydelta;
    qreal xdelta;

    ydelta=d_startItem->pos().y()-d_endItem->pos().y();
    if(ydelta<0)
        ydelta=-ydelta;
    ydelta=ydelta-d_startItem->boundingRect().height();

    xdelta=d_endItem->pos().x()-d_startItem->pos().x()+d_startItem->boundingRect().width()/2;
    return ((ydelta<(60*2)&&(xdelta<d_startItem->boundingRect().width()/2)));

}
