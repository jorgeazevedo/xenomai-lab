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

#ifndef BLOCKDIAGRAM_H
#define BLOCKDIAGRAM_H

#include "superline.h"
#include "line.h"
#include "block.h"
#include "processresult.h"
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
//#include <iostream>

#include <QObject>
#include <QProcess>
#include "macros.h"
#include <QDir>

class BlockDiagram : public QObject
{
    Q_OBJECT
public:
   static const int numActions=4;

   enum Actions
   {
       Clean,
       Make,
       Exec,
       Kill
   };

private:
    //Boost library types
    //typedef boost::property<boost::edge_name_t, std::string,
    //        boost::property<boost::edge_weight_t, int> > EdgeProperties;
    typedef boost::property<boost::edge_name_t, Line*> EdgeProperties;

    typedef boost::adjacency_list<boost::listS, boost::vecS, boost::bidirectionalS,
                                  Block*,EdgeProperties> Graph;

    typedef boost::graph_traits<Graph>::vertex_descriptor DiagramVertex;
    typedef boost::graph_traits<Graph>::edge_descriptor DiagramEdge;

    typedef boost::graph_traits<Graph>::vertices_size_type NumEdges;

    typedef boost::graph_traits<Graph>::vertex_iterator VertexIterator;
    typedef boost::graph_traits<Graph>::edge_iterator EdgeIterator;
    typedef boost::graph_traits<Graph>::out_edge_iterator OutEdgeIterator;
    typedef boost::graph_traits<Graph>::in_edge_iterator InEdgeIterator;
    typedef boost::graph_traits<Graph>::adjacency_iterator AdjIterator;

    class VisitorClean : public boost::default_bfs_visitor
    {
    public:
       VisitorClean(BlockDiagram* owner);
       void discover_vertex(DiagramVertex v, const Graph& gz);
       static bool d_errors;
       BlockDiagram* d_owner;
    };

    class VisitorMake : public boost::default_bfs_visitor
    {
    public:
        VisitorMake(BlockDiagram* owner);

        void discover_vertex(DiagramVertex v, const Graph& gz);
        static bool d_errors;
    private:
        BlockDiagram* d_owner;
    };

    class VisitorKill : public boost::default_bfs_visitor
    {
    public:
        VisitorKill(BlockDiagram* owner);

        void discover_vertex(DiagramVertex v, const Graph& gz);
        static bool d_errors;
    private:
        BlockDiagram* d_owner;
    };

    class VisitorArg : public boost::default_bfs_visitor
    {
    public:
        void discover_vertex(DiagramVertex v, const Graph& gz) const;
    private:
        void add_arg(Block* block,QStringList list, const QString& prefix) const;
    };

    class VisitorRTExec : public boost::default_bfs_visitor
    {
    public:
        VisitorRTExec(BlockDiagram* owner);

        void discover_vertex(DiagramVertex v, const Graph& gz);
        static bool d_errors;
    private:
        bool d_continue;
        BlockDiagram* d_owner;
    };

    class VisitorNRTExec : public boost::default_bfs_visitor
    {
    public:
        VisitorNRTExec(BlockDiagram* owner);

        void discover_vertex(DiagramVertex v, const Graph& gz) const;
        static bool d_errors;
    private:
        BlockDiagram* d_owner;
    };

    class Visitors
    {
    public:
        Visitors(BlockDiagram* owner):
            clean(owner),make(owner),
            kill(owner),RTexec(owner),
            NRTexec(owner)
        {
            d_owner=owner;
        }

        VisitorClean clean;
        VisitorMake make;
        VisitorKill kill;
        VisitorRTExec RTexec;
        VisitorNRTExec NRTexec;
        VisitorArg argGenerator;

        BlockDiagram* d_owner;
    };

public:
    explicit BlockDiagram(QString workspace="Untitled",QString defaultTextEditor="vim",QObject *parent = 0);
    ~ BlockDiagram();


    void removeBlock(QString name);
    QStringList listBlocks(void);
    int numBlocks(void);

    void removeLine(QString name);
    void listLines(void);

    void setWorkspace(const QString& workspace);
    QString workspace(void) const;

    void setExecInTerm(const QString& name, const bool& value);
    bool getExecInTerm(const QString& name);

    void setExecAsSudo(const QString& name, const bool& value);
    bool getExecAsSudo(const QString& name);

signals:
    void finished(bool);
    void progress(ProcessResult);
    void blockError(QString);

public slots:
    void receivedBlockError(QString,QProcess::ProcessError);

    void act(int);

    bool clean();
    bool make();
    bool exec();
    bool kill();
    bool edit(QString name);
    bool settings(QString name);

    void addBlock(Block* block);
    bool addLine(const SuperLine& line);
    bool addLine(QString origin, QString destiny, QString name,QString initValue="");

    bool clear();

    bool blockExists(QString name);
private:
    //void instatiateBlock(Block* block);
    //void deinstatiateBlock(QString name);
    DiagramVertex findVertex(QString name);

    bool hasNoInEdges(DiagramVertex vertex);
    void catchOrphans();
    void removeConnectionFromRoot(DiagramVertex vertex);
    QString generateRootName();
    void demo();
    //bool removeDir(const QString& dirName);

    //Graph that holds the block diagram
    Graph graph;
    boost::property_map<Graph, boost::edge_name_t>::type edgeName;
    Visitors vis;
    QString d_workspace;
    QString d_workspaceFullPath;

};

#endif // BLOCKDIAGRAM_H
