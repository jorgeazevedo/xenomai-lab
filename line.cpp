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

#include "line.h"
#include "macros.h"

Line::Line(const QString& name):
    d_name(name)
{
    d_initValue=empty_matrix(1,1);
}

Line::Line(const QString& name, QString value):
    d_name(name)
{
    char buf[100];
    strcpy(buf,value.toStdString().c_str());
    d_initValue=new_matrix(buf);
    /*
    qDebug() << "Line::Line d_initValue columns" << d_initValue.columns;
    qDebug() << "Line::Line d_initValue rows" << d_initValue.rows;
    qDebug() << "Line::Line d_initValue 0,0" << d_initValue.matrix[0][0];
    qDebug() << "Line::Line d_initValue 0,1" << d_initValue.matrix[0][1];

    char buf2[100];
    Matrix temp=d_initValue;
    matrix_string(&temp,buf2);
    qDebug() << "Line::Line d_initValue string represent" << buf2;
    qDebug() << "Line::Line string was" << value;
    */
}

Line::Line(const QString& name, const Matrix& value):
    d_name(name)
{
    d_initValue=value;
}

void Line::setName(const QString &name)
{
    d_name=name;
}

QString Line::name() const
{
    return d_name;
}

void Line::setInitValue(const Matrix &mat)
{
    d_initValue=mat;
}

Matrix Line::initValue() const
{
    return d_initValue;
}

QString Line::fullName() const
{
    char buf[100];
    Matrix temp=d_initValue;
    matrix_string(&temp,buf);

    //qDebug() << "Line::fullName: Initvalue:" << d_initValue.rows << d_initValue.columns;
    //qDebug() << "Line::fullName: temp:" << temp.rows << temp.columns;
    return d_name+"="+buf;
}

bool Line::isValidInitValue(QString value)
{
    //const QString xis= value;
    //xis.toStdString().c_str();
    //TODO:
    //if(matrix_valid(value.toStdStr.c_str)
    return true;
}
