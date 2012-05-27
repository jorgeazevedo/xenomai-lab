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

#include "blockregistry.h"

//void anotherTest(const char *key, const char *value, const void *obj)
//{
//    qDebug("****2TEST:");
//}

BlockRegistry::BlockRegistry()
{


}

void BlockRegistry::open(){
    d_file.open(QDir::homePath()+"/.xenomailab/blocks/blocks.conf");

    d_file.iterOver("Real-Time",registerRT,this);
    d_file.iterOver("Non Real-Time",registerNRT,this);
}

bool BlockRegistry::isRealTime(const QString &type)
{
    return blockRTTable[type];
}

QString BlockRegistry::typeOfId(const int &id)
{
    return blockIndexTable.key(id);
}

int BlockRegistry::idOf(const QString &type)
{
    return blockIndexTable[type];
}

DiagramItem::DiagramType BlockRegistry::diagOf(const QString &type)
{
    return blockDiagTable[type];
}

QStringList BlockRegistry::getAllRT()
{
    return blockRTTable.keys(true);
}

QStringList BlockRegistry::getAllNRT()
{
    return blockRTTable.keys(false);
}

void BlockRegistry::dumpTables()
{
    qDebug() << blockRTTable;
    qDebug() << blockIndexTable;
    qDebug() << blockDiagTable;
}


void BlockRegistry::registerBlock(const QString &type, const bool &RT,
                                  DiagramItem::DiagramType diagType)
{
    static int index=0;

    blockRTTable[type]=RT;
    blockIndexTable[type]=index;
    blockDiagTable[type]=diagType;

    index++;
}

void BlockRegistry::registerRT(const char *key, const char *value, const void *obj)
{

    BlockRegistry* blockRegistry= BlockRegistry::instance();

    switch(*value){
    case 't': case 'T':
        blockRegistry->registerBlock(key,true,DiagramItem::Triangle);
        break;
    case 's': case'S':
        blockRegistry->registerBlock(key,true,DiagramItem::Square);
        break;
    }

}

void BlockRegistry::registerNRT(const char *key, const char *value, const void *obj)
{

    BlockRegistry* blockRegistry= BlockRegistry::instance();

    switch(*value){
    case 't': case 'T':
        blockRegistry->registerBlock(key,false,DiagramItem::Triangle);
        break;
    case 's': case'S':
        blockRegistry->registerBlock(key,false,DiagramItem::Square);
        break;
    }

}

BlockRegistry* BlockRegistry::instance()
{
    static BlockRegistry b;
    return &b;
}
