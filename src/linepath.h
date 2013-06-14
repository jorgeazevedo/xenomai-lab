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

#ifndef LINEPATH_H
#define LINEPATH_H

#include <QGraphicsPathItem>
#include "macros.h"
#include <QString>
#include <QPen>
#include <QFont>
#include <QPainterPath>
#include <QPainter>

#include "diagramitem.h"

class LinePath : public QGraphicsPathItem
{
public:
    enum { Type = UserType + 4 };
    int type() const
        { return Type; }

    LinePath(DiagramItem *startItem, DiagramItem *endItem,
             const QString& name,QGraphicsItem * parent = 0);
    ~LinePath();

    void setColor(const QColor &color)
        { d_color = color; }
    DiagramItem *startItem() const
        { return d_startItem; }
    DiagramItem *endItem() const
        { return d_endItem; }
    QString name() const
        { return d_nameTextItem->toPlainText(); }


    void updatePosition();

protected:

    QRectF boundingRect() const;
    void paint(QPainter * painter,
               const QStyleOptionGraphicsItem * option,
               QWidget * widget = 0 );

private:
    QPointF getStartPoint() const;
    QPointF zone1();
    QPointF zone2();
    bool isZone2();

    DiagramItem* d_startItem;
    DiagramItem* d_endItem;
    QString d_name;
    QGraphicsTextItem* d_nameTextItem;
    QPainterPath d_path;

    QColor d_color;

    //Definitions
    qreal lineThickness;
    qreal arrowWidth;
    qreal arrowHeight;

};

#endif // LINEPATH_H
