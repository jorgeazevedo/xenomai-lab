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

#include "superblock.h"

SuperBlock::SuperBlock():
    d_type(""),d_name(""),x(0),y(0)
{
}

SuperBlock::SuperBlock(const QString &type, const QString &name,
                       const qreal &X, const qreal &Y)
{
    d_type=type;
    d_name=name;
    x=X;
    y=Y;
}

void SuperBlock::setType(const QString &type)
{
    d_type=type;
}

QString SuperBlock::type() const
{
    return d_type;
}

void SuperBlock::setName(const QString &name)
{
    d_name=name;
}

QString SuperBlock::name() const
{
    return d_name;
}

void SuperBlock::setX(qreal X)
{
    x=X;
}

qreal SuperBlock::X() const
{
    return x;
}

void SuperBlock::setY(qreal Y)
{
    y=Y;
}

qreal SuperBlock::Y() const
{
    return y;
}

QDebug operator<< (QDebug d, const SuperBlock &block)
{
    d << "Type: " << block.d_type << ", Name: " << block.d_name;
    d << " @ (" << block.x;
    d << "," << block.y << ")";

    return d;
}

bool SuperBlock::operator==(const SuperBlock &other) const
{
    if(d_type==other.d_type && d_name==other.d_name
            && x == other.x && y == other.y)
        return true;
    else
        return false;
}
