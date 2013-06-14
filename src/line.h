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

#ifndef LINE_H
#define LINE_H

#include <QString>
#include "mtrx.h"

class Line
{
public:
    Line(const QString& name);
    Line(const QString& name, QString value);
    Line(const QString& name, const Matrix& value);

    void setName(const QString& name);
    QString name() const;

    void setInitValue(const Matrix& mat);
    Matrix initValue() const;

    QString fullName() const;

    static bool isValidInitValue(QString value);
private:
    QString d_name;
    Matrix d_initValue;
};

#endif // LINE_H
