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

#include "projectfile.h"

ProjectFile::ProjectFile()
{
}

ProjectFile::ProjectFile(const QString &fileName)
{
    open(fileName);
}

ProjectFile::~ProjectFile()
{
}

void ProjectFile::open(const QString &fileName)
{
    QString program = "tar";
    QStringList arguments;
    d_filename=fileName;
    d_project=QFileInfo(d_filename).fileName();
    d_project.chop(4);
    d_workspace=QDir::homePath()+"/.xenomailab/workspace/";

    arguments << "-xvf" << d_filename;

    d_process.setWorkingDirectory(d_workspace);
    d_process.start(program, arguments);

    if(!d_process.waitForStarted())
        throw("Failed to open file!");

    d_process.waitForFinished(3000);

    d_process.setReadChannel(QProcess::StandardOutput);
    QByteArray data;
    data.append(d_process.readAll());

    d_process.setReadChannel(QProcess::StandardError);
    data.append(d_process.readAll());

    //the end

    //result.processOutput=QString::fromAscii(data.data());
    //result.processRet=d_process.exitCode();


}

QString ProjectFile::configFile()
{
    return d_workspace+d_project+"/diagram.conf";
}

QString ProjectFile::projectName()
{
    return d_project;
}
