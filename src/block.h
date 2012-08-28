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

#ifndef BLOCK_H
#define BLOCK_H

#include <QThread>
#include <QObject>
#include <QProcess>
#include "macros.h"
#include "processresult.h"

class Block : public QObject
{
    Q_OBJECT

private:
    class ProcessKiller : public QThread {
    public:
        ProcessKiller(QProcess* p) : process(p){}
        void run();
    private:
        QProcess* process;
    };

public:

    enum Process
    {
        Clean,
        Make,
        Exec
    };

    explicit Block(QString type="block", QString name="block", bool RT=1);

    void setType(const QString& type);
    QString type(void) const;

    void setName(const QString& blockname);
    QString name(void) const;

    void setWorkspace(const QString& workspace);
    QString workspace(void) const;

    void clearArg(void);
    void setArg(const QStringList& arg);
    void appendArg(QString arg);
    QStringList arg(void) const;

    void setRT(bool RT);
    bool isRealTime() const;

    static void setTextEditor(const QString& editor);
    static QString textEditor();


    void setExecInTerm(const bool&);
    bool execInTerm(void) const;

    void setExecAsSudo(const bool&);
    bool execAsSudo(void) const;

    ProcessResult clean(void);
    ProcessResult make(void);
    ProcessResult exec(void);
    ProcessResult kill();
    ProcessResult edit();
    ProcessResult settings();

    ~Block();

public slots:
       void processFinished(int,QProcess::ExitStatus);
       void processError(QProcess::ProcessError);
signals:
       void procFinished(int,QProcess::ExitStatus, enum Process);
       void procFailed(QString,QProcess::ProcessError);

       void killProcess();
private:

    static QString d_textEditor;

    void updatePath();
    ProcessResult standaloneExec();
    ProcessResult terminalExec();

    QString d_type;
    QString d_name;
    QString d_path;
    QString d_workspace;
    QStringList d_arg;
    bool d_rt;
    bool d_execInTerm;
    bool d_execAsSudo;
    ProcessKiller* d_killer;
    QProcess *d_execProcess;
    QProcess *d_makeProcess;
    QProcess *d_cleanProcess;
    QProcess *d_editProcess;
    QProcess *d_settingsProcess;


};

#endif // BLOCK_H
