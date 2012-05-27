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

#include "workspace.h"

Workspace::Workspace(void)
{
    d_fileSettings=0;
    d_workspaceFullPath="";
    d_workspace="";
}

Workspace::Workspace(const QString& name)
{
    newWorkspace(name);

}

Workspace::~Workspace()
{
    clearCurrentWorkspace();
}

void Workspace::newWorkspace(const QString &name)
{

    //Here, we try to recover from a previous crash before giving up.
    QString workspaceFullPath= QDir::homePath()+"/.xenomailab/workspace/"+name+"/";
    if(!QDir().mkdir(workspaceFullPath)){
        removeDir(workspaceFullPath);
        if(!QDir().mkdir(workspaceFullPath)){
            QStringList err;
            err << Q_FUNC_INFO << "Failed to prepare a new workspace! Do we have full permissions on workspace folder?";
            throw(err);
        }
    }

    setWorkspace(name);

    SuperBlock root("tick","root",0,0);

    instatiateBlock(root);
}

void Workspace::setWorkspace(const QString &name)
{
    d_workspace=name;
    d_workspaceFullPath= QDir::homePath()+"/.xenomailab/workspace/"+d_workspace+"/";
    d_fileSettings = new FileSettings(d_workspaceFullPath+"diagram.conf");


    //is file isn't empty, we are opening a workspace
    if(!d_fileSettings->isEmpty()){
        int numBlocks=d_fileSettings->getInt("Diagram","Blocks");

        qDebug() << "Numblocks is" << numBlocks;

        QString block="Block%1";
        for(int i=0;i<numBlocks;++i){
            d_blockList << d_fileSettings->getBlock(block.arg(i));
            qDebug() << d_fileSettings->getBlock(block.arg(i));
        }

        int numLines=d_fileSettings->getInt("Diagram","Lines");
        QString line="Line%1";
        for(int i=0;i<numLines;++i){
            d_lineList << d_fileSettings->getLine(line.arg(i));
            qDebug() << d_fileSettings->getLine(line.arg(i));
        }

    }


}

void Workspace::clearCurrentWorkspace()
{
    if(d_fileSettings!=0)
        delete d_fileSettings;
    if(d_workspace!="")
        removeDir(d_workspaceFullPath);

    d_blockList.clear();
    d_lineList.clear();
}

/*
 * Untars a .xlb unto .xenomailab/workspace
 * and (should) check integrity of file
 * so that a valid project is ready to be loaded
 * after this function exits.
 * This is important because if the file is invalid
 * and we detect it, then we're able to leave the current
 * user project intact.
 * The next step after openProject
 * is usually a call to clear, that deletes everything.
 * Then, we try to load the workspace. If that load fails,
 * the program leaves the user with an incomplete diagram
 * that does nothig. This is why integrity checking
 * is so important.
 */

QString Workspace::openProject(const QString& fileName)
{
    //untar project file
    QString program = "tar";
    QStringList arguments;

    arguments << "-xvf" << fileName;

    QProcess tarProcess;
    tarProcess.setWorkingDirectory(QDir::homePath()+"/.xenomailab/workspace/");
    tarProcess.start(program, arguments);

    if(!tarProcess.waitForStarted()){
        QStringList err;
        err << Q_FUNC_INFO << "Failed to open project file" + fileName + " .";
        throw(err);

    }

    tarProcess.waitForFinished(3000);

    tarProcess.setReadChannel(QProcess::StandardOutput);
    QByteArray data;
    data.append(tarProcess.readAll());

    tarProcess.setReadChannel(QProcess::StandardError);
    data.append(tarProcess.readAll());

    //TODO: check integrity

    //Open diagram.conf

    QString ret=QFileInfo(fileName).fileName();
    ret.chop(4);
    return ret;
}

/*
 * saves current project to fileName(.xlb)
 * if the current workspace is of a diferent name
 * we must first create a copy of the current folder
 * with the appropriate name, tar it, then delete it
 * This is because the name of .xlb must match the name
 * of the folder inside.
 */

void Workspace::saveProject(const QString &fileName)
{
    //Although diagram.conf always contains the latest
    //existing blocks and lines, it doesn't update
    //the coordinates on every single block move.
    //So when we want to save a project, the first thing we do
    //is delete diagram.conf and write it all over again.
    resetDiagramFile();

    QString projectName=QFileInfo(fileName).fileName();
    projectName.chop(4);
    QString destFolderFullPath;

    QString folderToTar=d_workspace;

    if(d_workspace!=projectName){

        destFolderFullPath=QDir::homePath()+"/.xenomailab/workspace/"
                +projectName+"/";

        copyDir(d_workspaceFullPath,destFolderFullPath);

        folderToTar=projectName;
    }

    //qDebug() << "New project file is" << projectName;
    //qDebug() << "Current workspace is" << d_workspace;
    //qDebug() << "Whose fullpath is" << d_workspaceFullPath;
    //qDebug() << "We need a new folder at" << destFolderFullPath;


    //tar project file
    QString program = "tar";
    QStringList arguments;

    arguments << "-c" << "--bzip2" << "-f" << fileName << folderToTar;

    //qDebug() << "*************";
    //qDebug() << program << arguments;
    QProcess tarProcess;
    tarProcess.setWorkingDirectory(QDir::homePath()+"/.xenomailab/workspace/");
    tarProcess.start(program, arguments);

    if(!tarProcess.waitForStarted()){
        QStringList err;
        err << Q_FUNC_INFO << "Failed to save project file" + fileName + " .";
        removeDir(destFolderFullPath);
        throw(err);

    }

    tarProcess.waitForFinished(3000);

    tarProcess.setReadChannel(QProcess::StandardOutput);
    QByteArray data;
    data.append(tarProcess.readAll());

    tarProcess.setReadChannel(QProcess::StandardError);
    data.append(tarProcess.readAll());
    //TODO: We should do something with this output

    //we could just remove it, but this is makes it more
    //readable
    if(d_workspace!=projectName)
        removeDir(destFolderFullPath);
}

/*
void Workspace::loadBlock(const SuperBlock& block)
{
}
*/

void Workspace::addBlock(const SuperBlock& block)
{
    if(blockExists(block.name())){
        QStringList err;
        err << Q_FUNC_INFO << "There's already a block called \"" + block.name() + "\" in diagram!";
        throw(err);

    }

    instatiateBlock(block);
    registerBlock(block);

}

void Workspace::removeBlock(const QString& blockName)
{
    if(!blockExists(blockName)){
        QStringList err;
        err << Q_FUNC_INFO << "There isn't any block called \"" + blockName + "\" in diagram!";
        throw(err);

    }

    deinstatiateBlock(blockName);
    unregisterBlock(blockName);

    removeOrphanLinesOf(blockName);
}

void Workspace::removeOrphanLinesOf(const QString &blockName)
{
    SuperLine line;
    foreach(line, d_lineList){
        if((line.destiny()==blockName) ||
           (line.origin()==blockName))
            d_lineList.removeAll(line);
    }

    resetDiagramFile();
}

void Workspace::setBlockPosition(const QString &name, const QPointF &pos)
{

    for(int i=0;i<d_blockList.size();i++){
        if(d_blockList[i].name()==name){
            d_blockList[i].setX(pos.x());
            d_blockList[i].setY(pos.y());
        }
    }

}

QList<SuperBlock> Workspace::getAllBlocks()
{
    return d_blockList;
}

void Workspace::addLine(const SuperLine &line)
{
    if(lineExists(line)){
        QStringList err;
        err << Q_FUNC_INFO << "No two lines can have the same name or connect the same blocks!";
        throw(err);

    }

    //instatiateLine(line);
    registerLine(line);
}

void Workspace::removeLine(const QString &lineName)
{
    if(!lineExists(lineName)){
        QStringList err;
        err << Q_FUNC_INFO << "There isn't any line called \"" + lineName + "\" in diagram!";
        throw(err);

    }

    unregisterLine(lineName);

}

QList<SuperLine> Workspace::getAllLines()
{
    return d_lineList;
}

bool Workspace::lineExists(const SuperLine& line)
{
    SuperLine lineInList;

    foreach(lineInList,d_lineList){
        if(lineInList.name()==line.name())
            return true;
        if((lineInList.origin()==line.origin()) &&
            (lineInList.destiny()==line.destiny()))
                return true;

    }

    return false;
}

bool Workspace::lineExists(const QString&name)
{
    SuperLine line;

    foreach(line,d_lineList){
        if(line.name()==name)
            return true;
    }

    return false;
}

void Workspace::registerLine(const SuperLine &line)
{
    d_lineList.append(line);

    QString sectionName=QString("Line%1").arg(d_lineList.indexOf(line));

    d_fileSettings->storeLine(sectionName,line);
    d_fileSettings->commit();
    updateLineCount();

}

bool Workspace::blockExists(const QString& name)
{
    SuperBlock block;

    foreach(block,d_blockList){
        if(block.name()==name)
            return true;
    }

    return false;
}

void Workspace::registerBlock(const SuperBlock &block)
{
    //PROPOSED:
    //Add to ProjectFile

    //testeSet.storeInt("Diagram","Lines",0);
    //testeSet.commit();
    d_blockList.append(block);

    QString sectionName=QString("Block%1").arg(d_blockList.indexOf(block));

    d_fileSettings->storeBlock(sectionName,block);
    d_fileSettings->commit();
    updateBlockCount();

}

void Workspace::updateBlockCount()
{
    d_fileSettings->storeInt("Diagram","Blocks",d_blockList.size());
    d_fileSettings->commit();
}

void Workspace::updateLineCount()
{
    d_fileSettings->storeInt("Diagram","Lines",d_lineList.size());
    d_fileSettings->commit();
}

void Workspace::unregisterBlock(const QString& blockName)
{
    SuperBlock block;

    foreach(block,d_blockList){
        if(block.name()==blockName)
            break;
    }


    /* To unregister a block, we have to create a new diagram.conf
     * this is unfortunate. TWo solutions:
     * -Only commit diagram.conf on save. This isn't very good
     * because the whole diagram dissapears on a crash.
     * -Patch settings backend to support the operation we need.
     * Since patching is too much work, this is gonna be inneficient
     * like this for the time being.
     */

    d_blockList.removeAll(block);

    resetDiagramFile();

}

void Workspace::resetDiagramFile()
{
    delete d_fileSettings;
    d_fileSettings = new FileSettings(d_workspaceFullPath+"diagram.conf");

    int numBlocks=d_blockList.size();
    d_fileSettings->storeInt("Diagram","Blocks",d_blockList.size());

    QString blockSection="Block%1";

    for(int i=0;i<numBlocks;++i){
        d_fileSettings->storeBlock(blockSection.arg(i),d_blockList.at(i));
        qDebug() << "InsertBlock:";
        qDebug() << d_blockList.at(i);
    }


    int numLines=d_lineList.size();
    d_fileSettings->storeInt("Diagram","Lines",d_lineList.size());

    QString lineSection="Line%1";

    for(int i=0;i<numLines;++i){
        d_fileSettings->storeLine(lineSection.arg(i),d_lineList.at(i));
        qDebug() << "InsertLine:";
        qDebug() << d_lineList.at(i);
    }


    d_fileSettings->commit();
}

void Workspace::unregisterLine(const QString &lineName)
{
    SuperLine line;

    foreach(line,d_lineList){
        if(line.name()==lineName)
            break;
    }

    d_lineList.removeAll(line);

    resetDiagramFile();
}


void Workspace::instatiateBlock(const SuperBlock& block){

    QString originPath=QDir::homePath()+"/.xenomailab/blocks/"+
                    block.type()+"/"+block.type()+".conf";
    QString destinyPath=d_workspaceFullPath+block.name()+".conf";

    qDebug() << "Will copy" << originPath
             << "to" << destinyPath;


    if(!QFile::copy(originPath,destinyPath)){
        QStringList err;
        err << Q_FUNC_INFO << "Could not instatiate "
               +block.name()+"! Does "+originPath+" exist?";
        throw(err);
    }
}

void Workspace::deinstatiateBlock(const QString& blockName)
{
    QString confFile=d_workspaceFullPath+blockName+".conf";
    qDebug() << "Will remove" << confFile;


    if(!QFile::remove(confFile)){
        QStringList err;
        err << Q_FUNC_INFO << "Could not deinstatiate "+blockName+" ! This is a sign of workspace corruption."
               "Does "+confFile+" exist?";
        throw(err);
    }
}

bool Workspace::copyDir(QString srcPath, QString dstPath)
{
    if(srcPath.at(srcPath.size()-1)=='/')
        srcPath.chop(1);
    if(dstPath.at(dstPath.size()-1)=='/')
        dstPath.chop(1);

    removeDir(dstPath);
    QDir parentDstDir(QFileInfo(dstPath).path());
    if (!parentDstDir.mkdir(QFileInfo(dstPath).fileName()))
        return false;

    QDir srcDir(srcPath);
    foreach(const QFileInfo &info, srcDir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot)) {
        QString srcItemPath = srcPath + "/" + info.fileName();
        QString dstItemPath = dstPath + "/" + info.fileName();
        if (info.isDir()) {
            if (!copyDir(srcItemPath, dstItemPath)) {
                return false;
            }
        } else if (info.isFile()) {
            if (!QFile::copy(srcItemPath, dstItemPath)) {
                return false;
            }
        } else {
            qDebug() << "Unhandled item" << info.filePath() << "in cpDir";
        }
    }
    return true;
}
/*
bool Workspace::removeDir(const QString &dirPath)
{
    QDir dir(dirPath);
    if (!dir.exists())
        return true;
    foreach(const QFileInfo &info, dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot)) {
        if (info.isDir()) {
            if (!removeDir(info.filePath()))
                return false;
        } else {
            if (!dir.remove(info.fileName()))
                return false;
        }
    }
    QDir parentDir(QFileInfo(dirPath).path());
    return parentDir.rmdir(QFileInfo(dirPath).fileName());
}
*/

bool Workspace::removeDir(const QString &dirName)
{


    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = removeDir(info.absoluteFilePath());
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(dirName);
    }

    return result;
}
