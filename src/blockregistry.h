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

#ifndef BLOCKREGISTRY_H
#define BLOCKREGISTRY_H

#include "diagramitem.h"
#include "filesettings.h"
#include <QDir>
#include <QString>

class BlockRegistry
{
public:
    BlockRegistry();

    void open();
    static void registerRT(const char *key, const char *value, const void *obj);
    static void registerNRT(const char *key, const char *value, const void *obj);
    static BlockRegistry* instance();


    bool isRealTime(const QString& type);
    QString typeOfId(const int& id);
    int idOf(const QString& type);

    DiagramItem::DiagramType diagOf(const QString& type);
    QStringList getAllRT();
    QStringList getAllNRT();

    void registerBlock(const QString& type, const bool& RT,
                  DiagramItem::DiagramType diagType);
    void dumpTables();
private:
    FileSettings d_file;

    QHash<QString, bool> blockRTTable;
    QHash<QString, int> blockIndexTable;
    QHash<QString, DiagramItem::DiagramType> blockDiagTable;

};

#endif // BLOCKREGISTRY_H
