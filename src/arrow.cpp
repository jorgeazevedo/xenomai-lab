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

#include <QtGui>

#include "arrow.h"
#include <math.h>

const qreal Pi = 3.14;

//! [0]
Arrow::Arrow(DiagramItem *startItem, DiagramItem *endItem, QString name,
         QGraphicsItem *parent, QGraphicsScene *scene)
    : QGraphicsLineItem(parent, scene),myName(name)
{
    myStartItem = startItem;
    myEndItem = endItem;
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    myColor = Qt::black;
    setPen(QPen(myColor, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    textLabel=new QGraphicsTextItem(myName,parent,scene);
    textLabel->setFont(*new QFont("Helvetica", 15));
    //textLabel->setZValue(1000.0);
    textLabel->setParentItem(this);
    textLabel->setPos(0,0);

    for(int i=0;i<5;i++)
        part.insert(i,new QLineF());
}



QRectF Arrow::boundingRect() const
{
    qreal extra = (pen().width() + 400) / 2.0;

    qreal leftMost=part[0]->x1();
    qreal rightMost=part[0]->x1();
    qreal bottomMost=part[0]->y1();
    qreal topMost=part[0]->y1();
    for(int i=0;i<part.size();++i){
        if(part[i]->x1()<leftMost){
            //qDebug() << "leftMost came from part" << i;
            leftMost=part[i]->x1();
        }
        if(part[i]->x2()<leftMost){
            //qDebug() << "leftMost came from part" << i;
            leftMost=part[i]->x2();
        }

        if(part[i]->x1()>rightMost){
            //qDebug() << "rightMost came from part" << i;
            rightMost=part[i]->x1();
        }
        if(part[i]->x2()>rightMost){
            //qDebug() << "rightMost came from part" << i;
            rightMost=part[i]->x2();
        }

        if(part[i]->y1()>bottomMost){
            //qDebug() << "bottomMost came from part" << i;
            bottomMost=part[i]->y1();
        }
        if(part[i]->y2()>bottomMost){
            //qDebug() << "bottomMost came from part" << i;
            bottomMost=part[i]->y2();
        }

        if(part[i]->y1()<topMost){
            //qDebug() << "topMost came from part" << i;
            topMost=part[i]->y1();
        }
        if(part[i]->y2()<topMost){
            //qDebug() << "topMost came from part" << i;
            topMost=part[i]->y2();
        }

    }
    //qDebug() << "************";
    //qDebug() << "P4-X4" << part[4]->x1() << "P0X1" << part[0]->x1();

    qreal width=rightMost-leftMost;
    qreal height=topMost-bottomMost;
/*
    qDebug() << "Numbers are fun!";
    qDebug() << "line.p1" << line().p1();
    qDebug() << "line.p2" << line().p2();
    qDebug() << "Part 0 p1 is" << part[0]->p1();
    qDebug() << "Part 0 p2 is" << part[0]->p2();
    qDebug() << "mine is" << QPointF(leftMost,topMost);
    qDebug() << "mine width is" << QPointF(width,height);
*/
    //qDebug() << width <<  height << "Pretty sure this is negative" << QSizeF(line().p2().x() - line().p1().x(),
    //                                                     line().p2().y() - line().p1().y());


    /*
    return QRect(QPoint(leftMost,topMost),
                 QSize(width,height))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
        */
    //-normalized: does module of width/height
    //-adjusted: adds extra do the corresponding coordinates
    //top left corner plus width/height
    return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(),
                                      line().p2().y() - line().p1().y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);

    //return QRect(QPoint(leftMost,topMost),
    //             QPoint(rightMost,bottomMost));


}



QPainterPath Arrow::shape() const
{
    QPainterPath path = QGraphicsLineItem::shape();
    path.addPolygon(arrowHead);
    return path;
}



void Arrow::updatePosition()
{
    qDebug() << "Arrow: Update Position";

    //-myStartItem->boundingRect().width()/2,-myEndItem->boundingRect().width()/2
    //QLineF line(mapFromItem(myStartItem, -myStartItem->boundingRect().width()/2, 0), mapFromItem(myEndItem, -myEndItem->boundingRect().width()/2, 0));
    QLineF line(mapFromItem(myStartItem, 0, 0), mapFromItem(myEndItem, 0, 0));
    setLine(line);

    //textLabel->setPos(((line.x2()-line.x1())/2)+line.x1(),((line.y2()-line.y1())/2)+line.y1());

    if(isZone2()){
        textLabel->setPos(part[2]->p1());
        textLabel->setX(textLabel->x()+part[2]->dx()/2);
        textLabel->setY(textLabel->y()-35);
    }
    else{
        textLabel->setPos(((line.x2()-line.x1())/2)+line.x1(),((line.y2()-line.y1())/2)+line.y1());
    }
}

void Arrow::highlight()
{
    myColor = Qt::red;
    pen().setColor(QColor("#ff0000"));
}


void Arrow::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
          QWidget *)
{
    qDebug() << "Arrow: paint";

    if (myStartItem->collidesWithItem(myEndItem))
        return;
/*
    QPen myPen = pen();
    myPen.setColor(myColor);
    qreal arrowSize = 20;
    painter->setPen(myPen);
    painter->setBrush(myColor);


    QLineF centerLine(myStartItem->pos(), myEndItem->pos());
    QPolygonF endPolygon = myEndItem->polygon();
    //p1 = coordinates of first pointer of enditem in
    //scene coordinates
    QPointF p1 = endPolygon.first() + myEndItem->pos();
    QPointF p2;
    QPointF intersectPoint;
    QLineF polyLine;
    //.count()=.size()
    for (int i = 1; i < endPolygon.count(); ++i) {
        p2 = endPolygon.at(i) + myEndItem->pos();
        polyLine = QLineF(p1, p2);
        QLineF::IntersectType intersectType =
            polyLine.intersect(centerLine, &intersectPoint);
        if (intersectType == QLineF::BoundedIntersection)
            break;
            p1 = p2;
    }
    */

    //qreal comp1=intersectPoint.x();
    //qreal comp2;

    //TODO:
    //This is my patch to define input and output points for lines
    //All code above this point is rendered useless and should be removed

    QPointF intersectPoint;
    QPen myPen = pen();
    myPen.setColor(myColor);
    qreal arrowSize = 20;
    painter->setPen(myPen);
    painter->setBrush(myColor);

    intersectPoint=myEndItem->pos();
    intersectPoint.setX(intersectPoint.x()-myEndItem->boundingRect().width()/2);

    QPointF otherPoint;
    otherPoint=myStartItem->pos();
    otherPoint.setX(otherPoint.x()+myStartItem->boundingRect().width()/2);
    //original
    setLine(QLineF(otherPoint, intersectPoint));

    if(isZone2()){
        zone2(otherPoint);
    }
    else{
        zone1(otherPoint);
    }

    //double angle = ::acos(line().dx() / line().length());
    double angle=Pi;
    if (line().dy() >= 0)
        angle = (Pi * 2) - angle;


    QPointF arrowP1 = line().p2() + QPointF(sin(angle + Pi / 3) * arrowSize,
                                    cos(angle + Pi / 3) * arrowSize);
    QPointF arrowP2 = line().p2() + QPointF(sin(angle + Pi - Pi / 3) * arrowSize,
                                    cos(angle + Pi - Pi / 3) * arrowSize);

    arrowHead.clear();
    arrowHead << line().p2() << arrowP1 << arrowP2;

    for(int i=0;i<part.size();++i)
        painter->drawLine(*part[i]);

    //setLine(*part[0]);
    //painter->drawLine(line());
    painter->drawPolygon(arrowHead);
    if (isSelected()) {
        painter->setPen(QPen(myColor, 1, Qt::DashLine));

        QLineF myLine = line();
        myLine.translate(0, 4.0);
        painter->drawLine(myLine);
        myLine.translate(0,-8.0);
        painter->drawLine(myLine);
    }

    //qDebug() << "PAINT ORDER FINISH";
}

bool Arrow::isZone2()
{
    //These calculations really need an illustration to be properly understood
    qreal ydelta;
    qreal xdelta;
    //ydelta=myEndItem->pos().y()-myStartItem->pos().y()-myStartItem->boundingRect().height();
    ydelta=myStartItem->pos().y()-myEndItem->pos().y();
    if(ydelta<0)
        ydelta=-ydelta;
    ydelta=ydelta-myStartItem->boundingRect().height();

    xdelta=myEndItem->pos().x()-myStartItem->pos().x()+myStartItem->boundingRect().width()/2;
    return ((ydelta<(60*2)&&(xdelta<myStartItem->boundingRect().width()/2)));

}

void Arrow::zone1(QPointF otherPoint)
{

    qreal part3Width=0;
    //Part1
    QPointF part1Origin=otherPoint;
    qreal part1Delta=myEndItem->pos().x()-myStartItem->pos().x()
                    -myEndItem->boundingRect().width()/2    //100
                    -myStartItem->boundingRect().width()/2; //100

    part1Delta=part1Delta/2;
    if(part1Delta<30){
        part3Width=(30-part1Delta)*2;
        part1Delta=30;
    }
    QPointF part1Destiny=part1Origin;
    part1Destiny.setX(part1Destiny.x()+part1Delta);

    part[0]->setP1(part1Origin);
    part[0]->setP2(part1Destiny);

    //Part2
    QPointF part2Origin=part1Destiny;
    qreal part2Delta=myEndItem->pos().y()-myStartItem->pos().y();
    part2Delta=part2Delta/2;
    QPointF part2Destiny=part2Origin;
    part2Destiny.setY(part2Destiny.y()+part2Delta);

    part[1]->setP1(part2Origin);
    part[1]->setP2(part2Destiny);

    //qDebug() << "part2:" << part2Origin << part2Destiny;

    //Part3
    QPointF part3Origin=part2Destiny;
    QPointF part3Destiny=part3Origin;
    part3Destiny.setX(part3Destiny.x()-part3Width);

    part[2]->setP1(part3Origin);
    part[2]->setP2(part3Destiny);

    //qDebug() << "part3:" << part3Origin << part3Destiny;

    //Part4
    QPointF part4Origin=part3Destiny;
    QPointF part4Destiny=part4Origin;
    part4Destiny.setY(part4Destiny.y()+part2Delta);

    part[3]->setP1(part4Origin);
    part[3]->setP2(part4Destiny);

    //qDebug() << "Part4:" << part4Origin << part4Destiny;

    //Part5
    QPointF part5Origin=part4Destiny;
    QPointF part5Destiny=part5Origin;
    part5Destiny.setX(part5Destiny.x()+part1Delta);

    part[4]->setP1(part5Origin);
    part[4]->setP2(part5Destiny);
}

void Arrow::zone2(QPointF otherPoint)
{

    //Vertical Deltas (lines part[1] and part[3])
    qreal part2Delta;
    qreal part4Delta;

    if(myStartItem->pos().y()>myEndItem->pos().y()){
        part2Delta=myEndItem->boundingRect().height()/2+30
                +(myStartItem->pos().y()-myEndItem->pos().y());
        part4Delta=myStartItem->boundingRect().height()/2+30;
    }
    else{
        part2Delta=myEndItem->boundingRect().height()/2+30
                +(myEndItem->pos().y()-myStartItem->pos().y());
        part2Delta=-part2Delta;
        part4Delta=myEndItem->boundingRect().height()/2+30;
        part4Delta=-part4Delta;
        //part4Delta=longOne;
    }


    //Part1
    QPointF part1Origin=otherPoint;
    QPointF part1Destiny=part1Origin;
    part1Destiny.setX(part1Destiny.x()+30);

    part[0]->setP1(part1Origin);
    part[0]->setP2(part1Destiny);

    //Part2
    QPointF part2Origin=part1Destiny;
    QPointF part2Destiny=part2Origin;
    part2Destiny.setY(part2Destiny.y()-part2Delta);

    part[1]->setP1(part2Origin);
    part[1]->setP2(part2Destiny);

    //Part3
    QPointF part3Origin=part2Destiny;
    QPointF part3Destiny=part3Origin;
    qreal part3Width=30+myStartItem->boundingRect().width()/2
                    +(myStartItem->pos().x()-myEndItem->pos().x())
                    +30+myEndItem->boundingRect().width()/2;
    part3Destiny.setX(part3Destiny.x()-part3Width);

    part[2]->setP1(part3Origin);
    part[2]->setP2(part3Destiny);

    //qDebug() << "part3:" << part3Origin << part3Destiny;

    //Part4
    QPointF part4Origin=part3Destiny;
    QPointF part4Destiny=part4Origin;
    part4Destiny.setY(part4Destiny.y()+part4Delta);

    part[3]->setP1(part4Origin);
    part[3]->setP2(part4Destiny);

    //qDebug() << "Part4:" << part4Origin << part4Destiny;

    //Part5
    QPointF part5Origin=part4Destiny;
    QPointF part5Destiny=part5Origin;
    part5Destiny.setX(part5Destiny.x()+30);

    part[4]->setP1(part5Origin);
    part[4]->setP2(part5Destiny);
}
