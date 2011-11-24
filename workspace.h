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

#ifndef WORKSPACE_H
#define WORKSPACE_H

#include "filesettings.h"
#include "superline.h"
#include "superblock.h"
#include "macros.h"
#include <QProcess>
#include <QString>
#include <QDir>
#include <QPointF>

class Workspace
{
public:
    Workspace();
    Workspace(const QString& name);
    ~Workspace();

    void newWorkspace(const QString& name);
    void clearCurrentWorkspace();
    void setWorkspace(const QString& name);

    QString openProject(const QString& fileName);
    void saveProject(const QString& fileName);

    QList<SuperBlock> getAllBlocks();
    //void loadBlock(const SuperBlock& block);
    void addBlock(const SuperBlock& block);
    void removeBlock(const QString& blockName);
    void setBlockPosition(const QString& name, const QPointF& pos);


    QList<SuperLine> getAllLines();
    //void loadLine(const SuperLine& line);
    void addLine(const SuperLine& line);
    void removeLine(const QString& lineName);

    //These are public so that we can valid new element names
    bool blockExists(const QString& block);
    bool lineExists(const SuperLine& line);
    bool lineExists(const QString& line);
private:
    void resetDiagramFile();

    void removeOrphanLinesOf(const QString& blockName);


    void registerLine(const SuperLine& line);
    void updateLineCount();
    void unregisterLine(const QString& lineName);



    void registerBlock(const SuperBlock& block);
    void updateBlockCount();
    void unregisterBlock(const QString& block);

    void instatiateBlock(const SuperBlock&);
    void deinstatiateBlock(const QString&);

    bool copyDir(QString srcPath, QString dstPath);
    bool removeDir(const QString& dirName);

    QString d_workspace;
    QString d_workspaceFullPath;
    QList<SuperBlock> d_blockList;
    QList<SuperLine> d_lineList;
    FileSettings* d_fileSettings;
};

#endif // WORKSPACE_H
