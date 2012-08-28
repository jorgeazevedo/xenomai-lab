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

#include "block.h"

QString Block::d_textEditor="vim";

Block::Block(QString type, QString name, bool RT) :
    QObject(0)
{
    d_type=type;
    d_name=name;
    d_workspace="";
   // d_arg=arg;
    d_rt=RT;
    //RT Blocks default to execute in terminal window
    if(RT)
        d_execInTerm=false;
    else
        d_execInTerm=true;
    d_execAsSudo=false;

    d_execProcess = new QProcess();
    connect(d_execProcess, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(processError(QProcess::ProcessError)) );
    connect(d_execProcess, SIGNAL(finished(int,QProcess::ExitStatus)),
            this, SLOT(processFinished(int,QProcess::ExitStatus)));

    d_killer = new ProcessKiller(d_execProcess);
    connect(this, SIGNAL(killProcess()),
            d_killer, SLOT(start()),Qt::QueuedConnection);

    d_makeProcess = new QProcess();
    d_cleanProcess = new QProcess();
    d_editProcess = new QProcess();
    d_settingsProcess = new QProcess();

    updatePath();
}

Block::~Block()
{
    delete d_makeProcess;
    delete d_execProcess;
    delete d_cleanProcess;
    delete d_editProcess;
    delete d_settingsProcess;
    delete d_killer;

}

void Block::processError(QProcess::ProcessError err)
{

    QByteArray data;
    data.append(d_execProcess->readAll());



    QDEBUG << "Error! Value is " << err;
    QDEBUG << "This is the last data:" << QString::fromLocal8Bit(data.data());
    emit procFailed(d_name,err);
}

void Block::setType(const QString& type)
{
    d_type=type;

    updatePath();
}

QString Block::type(void) const
{
    return d_type;
}

void Block::setName(const QString& blockname)
{
    d_name=blockname;
}

QString Block::name(void) const
{
    return d_name;
}

void Block::setWorkspace(const QString& workspace)
{
    d_workspace=workspace;
}

QString Block::workspace(void) const
{
    return d_workspace;
}

void Block::clearArg(void)
{
    d_arg.clear();
}

void Block::setArg(const QStringList& arg)
{
    //QStringList cleanArg=arg;
    //cleanArg.replace(QString(" "),QString("\ "));
    d_arg=arg;
}

void Block::appendArg(QString arg)
{
    //QString cleanArg=arg;
    //cleanArg.replace(QString(" "),QString("\ "));

    d_arg << arg;
}

QStringList Block::arg(void) const
{   
    return d_arg;
}

void Block::setRT(bool RT)
{
    d_rt=RT;
}

bool Block::isRealTime() const
{
    return d_rt;
}

void Block::setTextEditor(const QString& editor)
{
    d_textEditor=editor;
}

QString Block::textEditor()
{
    return d_textEditor;
}

void Block::setExecInTerm(const bool& value)
{
    d_execInTerm=value;
}

bool Block::execInTerm() const
{
    return d_execInTerm;
}

void Block::setExecAsSudo(const bool& value)
{
    d_execAsSudo=value;
}

bool Block::execAsSudo() const
{
    return d_execAsSudo;
}

ProcessResult Block::clean(void)
{
    QString program = "make";
    QStringList arguments;
    ProcessResult result;

    result.name=name();

    arguments << "cleanall";

    qDebug() << "Executing" << program << arguments
             << "@" << d_path;

    d_cleanProcess->setWorkingDirectory(d_path);
    d_cleanProcess->start(program, arguments);


    if(!d_cleanProcess->waitForStarted()){
        result.processRet=1;
        return result;
    }

    d_cleanProcess->waitForFinished(1000);

    d_cleanProcess->setReadChannel(QProcess::StandardOutput);
    QByteArray data;
    data.append(d_cleanProcess->readAll());

    d_cleanProcess->setReadChannel(QProcess::StandardError);
    data.append(d_cleanProcess->readAll());

    //the end

    result.processOutput=QString::fromLocal8Bit(data.data());
    result.processRet=d_cleanProcess->exitCode();
    // Output the data
    //qDebug() << result.processOutput;

    return result;
}

ProcessResult Block::make(void)
{

    QString program = "make";
    QStringList arguments;
    ProcessResult result;

    result.name=name();

    arguments << "-j2";

    qDebug() << "Executing" << program << arguments
             << "@" << d_path;

    d_makeProcess->setWorkingDirectory(d_path);
    d_makeProcess->start(program, arguments);

    if(!d_makeProcess->waitForStarted()){
        result.processRet=1;
        return result;
    }

    d_makeProcess->waitForFinished(3000);

    d_makeProcess->setReadChannel(QProcess::StandardOutput);
    QByteArray data;
    data.append(d_makeProcess->readAll());

    d_makeProcess->setReadChannel(QProcess::StandardError);
    data.append(d_makeProcess->readAll());

    //the end

    result.processOutput=QString::fromLocal8Bit(data);
    result.processRet=d_makeProcess->exitCode();
    // Output the data
    //qDebug() << result.processOutput;

    return result;

}

ProcessResult Block::exec(void)
{
    if(execInTerm())
        return terminalExec();
    else
        return standaloneExec();
}

ProcessResult Block::standaloneExec()
{
    QString program="./"+d_type;
    ProcessResult result;

    result.name=name();

    d_arg << d_workspace+"/"+d_name;

    qDebug() << "Executing" << program << d_arg
             << "@" << d_path;

    d_execProcess->setWorkingDirectory(d_path);
    d_execProcess->setProcessChannelMode(QProcess::MergedChannels);
    d_execProcess->start(program, d_arg);

    //qDebug() << "Waiting for start";
    if(!d_execProcess->waitForStarted()){
        result.processOutput="Process failed to start. Has "+d_path+d_type+" been erased? Has it segfault?\n";
        result.processRet=1;
        return result;
    }
    //qDebug() << "Done Waiting";

    //instead of wait for finish, wait for a read
    //which may come from process finish or fflush(stdout)
    d_execProcess->waitForReadyRead();

    QByteArray data;
    data.append(d_execProcess->readAll());

    result.processOutput=QString::fromLocal8Bit(data.data());

    //error conditions
    if((d_execProcess->state()==QProcess::NotRunning)
       ||(data.contains("E:"))){
        result.processRet=1;
        return result;
    }

    //qDebug() << result.processOutput;
    return result;

}

ProcessResult Block::terminalExec()
{
    QString program = "xterm";
    QStringList arguments;
    ProcessResult result;

    result.name=name();

    QString command=d_path+d_type+" "+d_workspace+"/"+d_name+" ";

    //we want this:
    // \"-ofoo=[1 2 3],bar\"
    QStringList::Iterator iterator;
    for (iterator = d_arg.begin(); iterator != d_arg.end();
           ++iterator)
        command+=(*iterator).replace(QString(" "),QString("\\ "))+" ";


    command+=" & bash";

    arguments << "-e" << command;

    qDebug() << "Executing" << program << arguments;

    d_execProcess->setProcessChannelMode(QProcess::MergedChannels);
    d_execProcess->start(program, arguments);

   // qDebug() << "Wait for start";

    // If xterm does not start, an error occured
    if(!d_execProcess->waitForStarted()){
        result.processRet=1;
        return result;
    }

    // If xterm returns in less tha a second, an error occured
    if(d_execProcess->waitForFinished(1000)){
        result.processRet=1;
        return result;
    }

    return result;
}

/**
 * Send sigterm, give it a second then kill.
 * Note: we cannot call proces->waitForFinished
 * from this context.
 */

void Block::ProcessKiller::run()
{
    if(process->state()!=QProcess::NotRunning){
         process->terminate();

         sleep(2);

         if(process->state()!=QProcess::NotRunning)
            process->kill();
     }
}


/*
 * If process is starting or running
 * try to send SIGTERM by calling terminate
 * give it 5 seconds to finish, if SIGTERM didn't work
 * send SIGKILL and hope for the best
 */

ProcessResult Block::kill()
{
    ProcessResult result;

    result.name=name();

    qDebug() << "Kill" << d_name;

    //this differs d_killer->start() to a more convenient time
    //which then runs Block::ProcessKiller::run()
    emit killProcess();

    return result;
}

ProcessResult Block::edit()
{

    QString program = "gnome-terminal";
    QStringList arguments;
    ProcessResult result;

    result.name=name();

    QString command=d_path+d_type+".c";

    arguments << "-e" << d_textEditor+" "+command;

    qDebug() << "Executing" << program << arguments;

    d_editProcess->setWorkingDirectory(d_path);
    d_editProcess->setProcessChannelMode(QProcess::MergedChannels);
    d_editProcess->start(program, arguments);

    //qDebug() << "Wait for start";

    // If xterm does not start, an error occured
    if(!d_editProcess->waitForStarted()){
        result.processRet=1;
        return result;
    }

    // If xterm returns in less tha a second, an error occured
    if(d_editProcess->waitForFinished(1000)){
        result.processRet=1;
        return result;
    }

    return result;

}

ProcessResult Block::settings()
{

    QString program = "./"+d_type+"_settings";
    QStringList arguments;
    ProcessResult result;

    result.name=program;

    arguments << d_workspace+"/"+d_name;
    qDebug() << "Executing" << program << arguments
             << "@" << d_path;;

    d_settingsProcess->setWorkingDirectory(d_path);
    d_settingsProcess->setProcessChannelMode(QProcess::MergedChannels);
    d_settingsProcess->start(program, arguments);

    //qDebug() << "Wait for start";

    // If xterm does not start, an error occured
    if(!d_settingsProcess->waitForStarted()){
        result.processRet=1;
        return result;
    }

    // If _settings returns in less tha a second, an error occured
    if(d_settingsProcess->waitForFinished(1000)){
        result.processRet=1;
        return result;
    }

    return result;

}

void Block::processFinished(int ret,QProcess::ExitStatus status)
{
    enum Process p=Clean; //avoid warning

    if(QObject::sender()==d_cleanProcess)
        p=Clean;
    else if(QObject::sender()==d_makeProcess)
        p=Make;
    else if(QObject::sender()==d_execProcess)
        p=Exec;

    switch(p){
        case Clean:
            qDebug() << d_name << " -make  clean exited with no errors";
            break;
        case Make:
            qDebug() << d_name << " - make exited :";
            qDebug() << d_makeProcess->readAllStandardOutput();
            if(ret)
                qDebug() << d_makeProcess->readAllStandardError();
            break;
        case Exec:
            qDebug() << d_name << " - block process exited with code" << ret;
            break;
    }


    emit procFinished(ret,status,p);

}

void Block::updatePath(){
    d_path=QString(getenv("HOME"))+"/.xenomailab/blocks/"+d_type+"/";
}
