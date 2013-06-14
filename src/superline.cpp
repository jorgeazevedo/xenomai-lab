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

#include "superline.h"

SuperLine::SuperLine():
    d_origin(""),d_destiny(""),d_name(""),d_value("")
{
}

SuperLine::SuperLine(const QString &origin, const QString &destiny,
                     const QString &name, const QString &value)
{
    d_origin=origin;
    d_destiny=destiny;
    d_name=name;
    d_value=value;
}

void SuperLine::setOrigin(const QString &origin)
{
    d_origin=origin;
}

QString SuperLine::origin() const
{
    return d_origin;
}

void SuperLine::setDestiny(const QString &destiny)
{
    d_destiny=destiny;
}

QString SuperLine::destiny() const
{
    return d_destiny;
}

void SuperLine::setName(const QString &name)
{
    d_name=name;
}

QString SuperLine::name() const
{
    return d_name;
}

void SuperLine::setValue(const QString &value)
{
    d_value=value;
}

QString SuperLine::value() const
{
    return d_value;
}

QDebug operator<< (QDebug d, const SuperLine &line)
{
    d << "Origin: " << line.d_origin << ", Destiny: " << line.d_destiny;
    d << "Name: " << line.d_name << ", Value: " << line.d_value;

    return d;
}

bool SuperLine::operator==(const SuperLine &other) const
{
    //PROPOSED:
    //isn't an equal name enough for this? Since name is a unique
    //identifier and all? And the same thing applies for SuperBlock?
    if(d_origin==other.d_origin && d_destiny==other.d_destiny
       && d_name==other.d_name && d_value==other.d_value)
        return true;
    else
        return false;
}
