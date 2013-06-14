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

#ifndef SUPERLINE_H
#define SUPERLINE_H

#include "macros.h"

class SuperLine
{
public:
    SuperLine();
    SuperLine(const QString& origin, const QString& destiny,
              const QString& name, const QString& value);

    void setOrigin(const QString& origin);
    QString origin() const;
    void setDestiny(const QString& destiny);
    QString destiny() const;
    void setName(const QString& name);
    QString name() const;
    void setValue(const QString& value);
    QString value() const;

    friend QDebug operator<< (QDebug d, const SuperLine &model);
    bool operator==(const SuperLine &other) const;

private:
    QString d_origin;
    QString d_destiny;
    QString d_name;
    QString d_value;
};

#endif // SUPERLINE_H
