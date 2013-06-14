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

#ifndef SUPERBLOCK_H
#define SUPERBLOCK_H

#include "macros.h"
#include <QString>

class SuperBlock
{
public:
    SuperBlock();
    SuperBlock(const QString& type, const QString& name,
               const qreal& X, const qreal& Y);

    void setType(const QString& type);
    QString type() const;
    void setName(const QString& name);
    QString name() const;
    void setX(qreal X);
    qreal X() const;
    void setY(qreal Y);
    qreal Y() const;

    friend QDebug operator<< (QDebug d, const SuperBlock &model);
    bool operator==(const SuperBlock &other) const;

private:
    QString d_type;
    QString d_name;
    qreal x;
    qreal y;
};

#endif // SUPERBLOCK_H
