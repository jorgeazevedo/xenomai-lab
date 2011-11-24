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

#include "blockdiagram.h"

/*
 * A BlockDiagram is never empty and
 * no blocks are ever orphan. This is why
 * a root block is added (along with a connection
 * to itself)
 */


BlockDiagram::BlockDiagram(QString workspace, QString defaultTextEditor,QObject *parent) :
    QObject(parent),vis(this),d_workspace(workspace)
{
    using namespace boost;

    Block::setTextEditor(defaultTextEditor);
    edgeName=get(edge_name, graph);

    //BlocklDiagram is never empty
    addBlock( new Block("tick","root"));

}

/*
 * QDir().rmDir() only removes empty dirs.
 * so we roll our own function.
 */

BlockDiagram::~BlockDiagram()
{
    //PROPOSED:
    //Remove root block
    //removeBlock("root");
/*
    removeDir(d_workspaceFullPath);
*/
}


/*
 * If we can find a vertex by that name, then return;
 * If we cannot (findVertex throws exception) and we
 * use it do add the new block to the graph
 */

void BlockDiagram::addBlock(Block *block)
{
    if(blockExists(block->name())){
        QStringList err;
        err << Q_FUNC_INFO << "There's already a block called " + block->name() + " in BlockDiagram!";
        throw(err);
    }


    DiagramVertex u0=add_vertex(graph);
    block->setWorkspace(d_workspace);
    graph[u0]=block;
    connect(block, SIGNAL(procFailed(QString,QProcess::ProcessError)),
            this, SLOT(receivedBlockError(QString,QProcess::ProcessError)) );
    QDEBUG << "Added a" << graph[u0]->type() << "block called" << graph[u0]->name();

    catchOrphans();

    QDEBUG << "***BlockDiagList***";
    listBlocks();
    listLines();
    QDEBUG << "*******************";
}

void BlockDiagram::removeBlock(QString name){
    using namespace boost;


    DiagramVertex vert;
    try{
        vert=findVertex(name);
    }
    catch(QString block){
        QDEBUG << "E: There is no block called" << block << "!";
        return;
    }

    InEdgeIterator firstInEdge,lastInEdge,nextInEdge;
    tie(firstInEdge,lastInEdge)=in_edges(vert,graph);
    for (nextInEdge = firstInEdge; firstInEdge != lastInEdge; firstInEdge = nextInEdge) {
      ++nextInEdge;

      remove_edge(*firstInEdge, graph);
    }

    OutEdgeIterator firstOutEdge,lastOutEdge,nextOutEdge;
    tie(firstOutEdge,lastOutEdge)=out_edges(vert,graph);
    for (nextOutEdge = firstOutEdge; firstOutEdge != lastOutEdge; firstOutEdge = nextOutEdge) {
      ++nextOutEdge;
      remove_edge(*firstOutEdge, graph);
    }


    //remember that vertices are pointers to Block
    delete graph[vert];

    remove_vertex(vert,graph);

    catchOrphans();

    QDEBUG << "***BlockDiagList***";
    listBlocks();
    listLines();
    QDEBUG << "*******************";
}

/*
 * get iterators for all vertices in graph.
 * access vertix by dereferencing iterator, indexing graph
 * and calling member functions of Block class.
 */

QStringList BlockDiagram::listBlocks(void){
    using namespace boost;

    QStringList list;
    VertexIterator vi,vend;

    for(tie(vi,vend) = vertices(graph); vi!=vend; ++vi){
        QDEBUG << "B: " << graph[*vi]->type() << " called " << graph[*vi]->name()
                 << "with" << graph[*vi]->isRealTime() << "RT restraints" ;
        list << graph[*vi]->name();
    }

    return list;

}

int BlockDiagram::numBlocks()
{
    NumEdges number;
    number=num_vertices(graph);

    return (int) number;
}

bool BlockDiagram::addLine(const SuperLine &line)
{
    addLine(line.origin(),line.destiny(),
            line.name(),line.value());
}
/*
 * Try to find vertices by given names.
 * If they exist, create the edge between
 * them.
 * If this isn't a root connection,
 * remove connection to root if it exists, otherwise does nothing.
 */

bool BlockDiagram::addLine(QString origin, QString destiny, QString name,QString initValue){
    using namespace boost;

    DiagramVertex o;
    DiagramVertex d;
    try{
        o=findVertex(origin);
        d=findVertex(destiny);
    }
    catch(QString block){
        QDEBUG << "E: There is no block called" << block << "!";
        return false;
    }

    //Ok, I found it out. boost::edge(u,v,g) returns pair<edge_descriptor, bool> where the bool is whether the edge exists. So in my case I know it does, so I use the expression:
    bool repeat=edge(o,d,graph).second;
    if(repeat){
        QDEBUG << "E: There's already a line between" << origin << "and" << destiny;
        return false;
    }

    //this is the default value
    if(initValue==""){
        initValue="[0]";
        QDEBUG << "Line" << name << "'s init value set to default" << initValue;
    }

    if(Line::isValidInitValue(initValue)){
        DiagramEdge e0 = (add_edge(o, d, graph)).first;
        edgeName[e0] = new Line(name,initValue);
    }
    else{
        QDEBUG << "E: this isn't a valid string:" << initValue << "!";
        return false;
    }

    //This is a BlockDiagram specificity
    if(origin!="root"){
        QDEBUG << "Removing connection to root from" << destiny;
        removeConnectionFromRoot(d);
    }

    QDEBUG << "Added line from" << origin << "to" << destiny << "called" << name;

    QDEBUG << "***BlockDiagList***";
    listBlocks();
    listLines();
    QDEBUG << "*******************";
    return true;
}

void BlockDiagram::removeLine(QString name)
{
    using namespace boost;


    DiagramVertex test1;
    DiagramVertex test2;

    EdgeIterator firstEdge,lastEdge,nextEdge;
    tie(firstEdge,lastEdge)= edges(graph);
    for (nextEdge = firstEdge; firstEdge != lastEdge; firstEdge = nextEdge) {
      ++nextEdge;

        if(edgeName[*firstEdge]->name() == name){


        test1=source(*firstEdge, graph);
        test2=target(*firstEdge, graph);
        QDEBUG << "Removing edge between" << graph[test1]->name()
                 << "and" << graph[test2]->name();

        delete edgeName[*firstEdge];
        remove_edge(*firstEdge, graph);

      }

    }

    catchOrphans();


    //POG version: iterate thru all edges
    //SANE version: find destiny vertex
    //if hasNoInEdges(vertex)
    // addLine(root)

    //boost::remove_edge(pipeList[name],graph);

    //pipeList.remove(name);

    QDEBUG << "***BlockDiagList***";
    listBlocks();
    listLines();
    QDEBUG << "*******************";
}

void BlockDiagram::listLines(void)
{
    using namespace boost;

    EdgeIterator firstInput,lastInput,nextInput;
    tie(firstInput,lastInput)= edges(graph);
    for (nextInput = firstInput; firstInput != lastInput; firstInput = nextInput) {
      ++nextInput;

        QDEBUG << "L: " << edgeName[*firstInput]->fullName();

    }
}

bool BlockDiagram::clear()
{
    QStringList list=listBlocks();
    list.removeAll("root");

    for(int i=0;i<list.size();++i){
        removeBlock(list.at(i));
    }

    return true;
}

void BlockDiagram::setWorkspace(const QString &workspace)
{
    VertexIterator vi,vend;

    for(tie(vi,vend) = vertices(graph); vi!=vend; ++vi)
        graph[*vi]->setWorkspace(workspace);


    d_workspace=workspace;
}

QString BlockDiagram::workspace() const
{
    return d_workspace;
}

void BlockDiagram::setExecInTerm(const QString &name, const bool &value)
{
    DiagramVertex v;
    try{
        v=findVertex(name);
    }
    catch(QString blockName){
        QDEBUG << "E: there is no block called" << name;
        return;
    }

    graph[v]->setExecInTerm(value);
    QDEBUG << graph[v]->name() << "'s execInTerm is now" << graph[v]->execInTerm();
}

bool BlockDiagram::getExecInTerm(const QString &name)
{
    DiagramVertex v;
    try{
        v=findVertex(name);
    }
    catch(QString blockName){
        QDEBUG << "E: there is no block called" << name;
        return false;
    }

    return graph[v]->execInTerm();

}

void BlockDiagram::setExecAsSudo(const QString &name, const bool &value)
{
    DiagramVertex v;
    try{
        v=findVertex(name);
    }
    catch(QString blockName){
        QDEBUG << "E: there is no block called" << name;
        return;
    }

    graph[v]->setExecAsSudo(value);
    QDEBUG << graph[v]->name() << "'s execInSudo is now" << graph[v]->execAsSudo();
}

bool BlockDiagram::getExecAsSudo(const QString &name)
{
    DiagramVertex v;
    try{
        v=findVertex(name);
    }
    catch(QString blockName){
        QDEBUG << "E: there is no block called" << name;
        return false;
    }

    return graph[v]->execAsSudo();
}

void BlockDiagram::receivedBlockError(QString name,QProcess::ProcessError err)
{
    QDEBUG << "Error! " + name + "has just crashed with value " << err;
    //TODO:
    if(err!=0)
        emit blockError(name);
}

void BlockDiagram::act(int action)
{
    switch(action){
    case Clean:
        clean();
        break;
    case Make:
        make();
        break;
    case Exec:
        exec();
        break;
    case Kill:
        kill();
        break;
    }
}

bool BlockDiagram::clean()
{


    breadth_first_search(graph, findVertex("root"), visitor(vis.clean));

    //reset errors
    bool errors=vis.clean.d_errors;
    vis.clean.d_errors=false;

    emit finished(errors);

    return errors;
}

BlockDiagram::VisitorClean::VisitorClean(BlockDiagram* owner)
{
    d_errors=false;
    d_owner=owner;

}

bool BlockDiagram::VisitorClean::d_errors=false;

void BlockDiagram::VisitorClean::discover_vertex(DiagramVertex v, const Graph &gz)
{
    Graph graphCopy=gz;

    if(!d_errors){


        ProcessResult result=graphCopy[v]->clean();
        if(result.processRet){
            d_errors=true;
            QDEBUG << "VisitorClean: A clean error has occured";
        }

        //QDEBUG << "VisitorClean: d_errors=" << d_errors;

        emit d_owner->progress(result);
    }

    /*

    if(graphCopy[v]->type()!="root"){
        if(!d_errors){


            ProcessResult result=graphCopy[v]->clean();
            if(result.processRet){
                d_errors=true;
                QDEBUG << "VisitorClean: A clean error has occured";
            }

            //QDEBUG << "VisitorClean: d_errors=" << d_errors;

            emit d_owner->progress(result);
        }

    }
*/
}

bool BlockDiagram::make()
{
    breadth_first_search(graph, findVertex("root"), visitor(vis.make));

    //reset errors
    bool errors=vis.make.d_errors;
    vis.make.d_errors=false;

    emit finished(errors);
    return errors;
}

BlockDiagram::VisitorMake::VisitorMake(BlockDiagram* owner)
{
    d_errors=false;
    d_owner=owner;

}

bool BlockDiagram::VisitorMake::d_errors=false;

void BlockDiagram::VisitorMake::discover_vertex(DiagramVertex v, const Graph &gz)
{
    Graph graphCopy=gz;


    if(!d_errors){

        ProcessResult result=graphCopy[v]->make();
        if(result.processRet){
            d_errors=true;
            QDEBUG << "VisitorMake: A make error has occured";
        }

        //QDEBUG << "VisitorMake: d_errors=" << d_errors;

        emit d_owner->progress(result);
    }

    /*
    if(graphCopy[v]->type()!="root"){
        if(!d_errors){

            ProcessResult result=graphCopy[v]->make();
            if(result.processRet){
                d_errors=true;
                QDEBUG << "VisitorMake: A make error has occured";
            }

            //QDEBUG << "VisitorMake: d_errors=" << d_errors;

            emit d_owner->progress(result);
        }

    }*/
}

/*
 * Execution, one would think, should be as simple
 * as generating the arguments for the block and then
 * executing it. The problem is that non-real-time blocks
 * need to be executed _after_ the real-time counterparts.
 * This is because of the non-blocking open() call
 * NRT blocks use to open their input pipes. Because the
 * directory where the pipes are does not exit, making the
 * open() call blocking is a non-trivial exercise.
 * The solution is then to split the process into 3 parts:
 * -Generate the arguments
 * -Execute the Real-Time components
 * -If sucessfull, execute NRT components
 */

bool BlockDiagram::exec()
{
    using namespace boost;
    DiagramVertex theRoot=findVertex("root");

    breadth_first_search(graph, theRoot , visitor(vis.argGenerator));
    breadth_first_search(graph, theRoot, visitor(vis.RTexec));

    //reset errors
    bool errors=vis.RTexec.d_errors;
    vis.RTexec.d_errors=false;

    //if no errors
    if(!errors){
        breadth_first_search(graph, theRoot, visitor(vis.NRTexec));
        errors=vis.NRTexec.d_errors;
        vis.NRTexec.d_errors=false;
    }
    else{
        breadth_first_search(graph, theRoot, visitor(vis.kill));
        vis.kill.d_errors=false;
    }

    emit finished(errors);
    return errors;

}

/*
 * This function consists in four iterations. Two for in_edges,
 * the input edges, and two for out_edges.
 * First of we fork the execution path into RT and NRT. This is
 * because RT Blocks can only have RT inputs (-i) and NRT can only
 * have NRT inputs (-u)
 * Then we search once for real-time outputs (-o), once for NRT outputs (-p)
 * Frankly, this whole thing is suboptimal and an example of spaghetti code.
 * Gonna mark this as TODO:
 * TODO: only one cycle for in_dges and another for out_edges, please.
 */

 void BlockDiagram::VisitorArg::add_arg(Block* block, QStringList list, const QString &prefix) const
 {
    //sort alphabeticaly
    list.sort();

    //insert a comma between the names
    int size=list.size()-1;
    for(int i=0,j=1;i<size;i++,j=j+2)
        list.insert(j,",");

    //append flag accordingly
    list.prepend(prefix);

    if(list.size()>1)
        block->appendArg(list.join(""));


    QDEBUG << block->name() << ": " << list.join("");

    //empty QStringList for next round
    list.clear();

}

void BlockDiagram::VisitorArg::discover_vertex(DiagramVertex v, const Graph& gz) const
  {
    using namespace boost;

    Graph graphCopy=gz;
    boost::property_map<Graph, boost::edge_name_t>::type
        lineName=get(edge_name, graphCopy);
    QStringList rtHolder;
    QStringList nrtHolder;

    //First clear argument list;
    graphCopy[v]->clearArg();

    //append all in_edge names to rtHolder
    InEdgeIterator firstInput,lastInput;
    for(tie(firstInput,lastInput)=in_edges(v,graphCopy); firstInput != lastInput; ++firstInput)
           rtHolder << lineName[*firstInput]->fullName();

    //If I am Real-Time, my inputs are real time
    //if(graphCopy[v]->isRealTime())
    //    add_arg(graphCopy[v],rtHolder,"-i");
    //else
    add_arg(graphCopy[v],rtHolder,"-i");

    //empty QStringList for next round
    rtHolder.clear();

    OutEdgeIterator firstEdge,lastEdge;
    for(tie(firstEdge,lastEdge)=out_edges(v,graphCopy); firstEdge != lastEdge; ++firstEdge){

        if(graphCopy[target(*firstEdge,graphCopy)]->isRealTime())
            rtHolder << lineName[*firstEdge]->fullName();
        else
            nrtHolder << lineName[*firstEdge]->fullName();

    }
    add_arg(graphCopy[v],rtHolder,"-o");
    add_arg(graphCopy[v],nrtHolder,"-p");

  }

BlockDiagram::VisitorRTExec::VisitorRTExec(BlockDiagram* owner)
{
    d_continue=true;
    d_owner=owner;
}

bool BlockDiagram::VisitorRTExec::d_errors=false;

void BlockDiagram::VisitorRTExec::discover_vertex(DiagramVertex v, const Graph& gz)
{
    using namespace boost;
    Graph graphCopy=gz;

    if(!d_errors){
        if(graphCopy[v]->isRealTime()){

            ProcessResult result=graphCopy[v]->exec();
            if(result.processRet){
                d_errors=true;
                QDEBUG << "VisitorRTExec: A make error has occured";
            }

            //QDEBUG << "VisitorRTExec: d_errors=" << d_errors;

            emit d_owner->progress(result);
        }

    }

}

BlockDiagram::VisitorNRTExec::VisitorNRTExec(BlockDiagram* owner)
{
    d_owner=owner;
}

bool BlockDiagram::VisitorNRTExec::d_errors=false;

/*
 * We don't need a continue flag in NRT execution because
 * if RT has failed, NRT will also fail. But this is a bit crap
 * for the osciloscope which does not fail silently.
 */

void BlockDiagram::VisitorNRTExec::discover_vertex(DiagramVertex v, const Graph& gz) const
{
    using namespace boost;
    Graph graphCopy=gz;

/*
    if((graphCopy[v]->type()!="root")&&(!graphCopy[v]->isRealTime()))
        QDEBUG << "NRTVisitor execution says:" << graphCopy[v]->exec();

    return;
    */

    if(!d_errors){
        if(!graphCopy[v]->isRealTime()){
            QDEBUG << "fukc debuging" << graphCopy[v]->arg();

            ProcessResult result=graphCopy[v]->exec();
            if(result.processRet){
                d_errors=true;
                QDEBUG << "VisitorNRTExec: A make error has occured";
            }

            //QDEBUG << "VisitorNRTExec: d_errors=" << d_errors;

            emit d_owner->progress(result);
        }

    }
    /*
    if(graphCopy[v]->type()!="root"&&(!d_errors)){
        if(!graphCopy[v]->isRealTime()){
            QDEBUG << "fukc debuging" << graphCopy[v]->arg();

            ProcessResult result=graphCopy[v]->exec();
            if(result.processRet){
                d_errors=true;
                QDEBUG << "VisitorNRTExec: A make error has occured";
            }

            //QDEBUG << "VisitorNRTExec: d_errors=" << d_errors;

            emit d_owner->progress(result);
        }

    }*/

}



bool BlockDiagram::kill()
{
    breadth_first_search(graph, findVertex("root"), visitor(vis.kill));

    //reset errors
    bool errors=vis.kill.d_errors;
    vis.kill.d_errors=false;

    emit finished(errors);
    return errors;
}

BlockDiagram::VisitorKill::VisitorKill(BlockDiagram* owner)
{
    d_errors=false;
    d_owner=owner;

}

bool BlockDiagram::VisitorKill::d_errors=false;

void BlockDiagram::VisitorKill::discover_vertex(DiagramVertex v, const Graph &gz)
{
    Graph graphCopy=gz;


    if(!d_errors){

        ProcessResult result=graphCopy[v]->kill();
        if(result.processRet){
            d_errors=true;
            QDEBUG << "VisitorKill: A make error has occured";
        }

        //QDEBUG << "VisitorKill: d_errors=" << d_errors;

        //emit d_owner->progress(result);
    }

    /*
    if(graphCopy[v]->type()!="root"){
        if(!d_errors){

            ProcessResult result=graphCopy[v]->kill();
            if(result.processRet){
                d_errors=true;
                QDEBUG << "VisitorKill: A make error has occured";
            }

            //QDEBUG << "VisitorKill: d_errors=" << d_errors;

            //emit d_owner->progress(result);
        }

    }*/
}

bool BlockDiagram::edit(QString name)
{
    DiagramVertex vert;
    try{
        vert=findVertex(name);
    }
    catch(QString block){
        QDEBUG << "E: There is no block called" << block << "!";
        return false;
    }

//    QDEBUG << "like this";
//    DiagramVertex v=findVertex(name);
//    QDEBUG << "problem is in edit block side";
    ProcessResult a=graph[vert]->edit();

    return a.processRet;
}

bool BlockDiagram::settings(QString name)
{
    DiagramVertex vert;
    try{
        vert=findVertex(name);
    }
    catch(QString block){
        QDEBUG << "E: There is no block called" << block << "!";
        return false;
    }

//    QDEBUG << "like this";
//    DiagramVertex v=findVertex(name);
//    QDEBUG << "problem is in edit block side";
    //ProcessResult a=graph[vert]->edit();

    //return a.processRet;

    QDEBUG << "Settings for " << graph[vert]->name();
    graph[vert]->settings();
    QDEBUG << "Returned!";

    return true;
}

/*
 * get iterators for all vertices in graph.
 * find vertex with the name we're looking for
 */

BlockDiagram::DiagramVertex BlockDiagram::findVertex(QString name){
    using namespace boost;

    VertexIterator vi,vend;

    for(tie(vi,vend) = vertices(graph); vi!=vend; ++vi)
        if(graph[*vi]->name()==name)
          return *vi;

    //There is no vertex by that name, throw exception
    throw name;
}

bool BlockDiagram::blockExists(QString name)
{
    try{
        findVertex(name);
    }
    catch(QString name){
        return false;
    }

    return true;
}


bool BlockDiagram::hasNoInEdges(DiagramVertex vertex){

    InEdgeIterator firstInEdge,lastInEdge;
    tie(firstInEdge,lastInEdge)=in_edges(vertex,graph);

    if(firstInEdge==lastInEdge)
        return true;
    else
        return false;

}

void BlockDiagram::catchOrphans(void){
    using namespace boost;

    VertexIterator vi,vend;

    for(tie(vi,vend) = vertices(graph); vi!=vend; ++vi)
        if(hasNoInEdges(*vi))
            addLine("root",graph[*vi]->name(),generateRootName());

}


/*
 * For every InEdge of vertex
 * if first 4 chars of edge_name == "root"
 * remove edge
 */

void BlockDiagram::removeConnectionFromRoot(DiagramVertex vertex){


    InEdgeIterator firstInEdge,lastInEdge,nextInEdge;
    tie(firstInEdge,lastInEdge)=in_edges(vertex,graph);
    for (nextInEdge = firstInEdge; firstInEdge != lastInEdge; firstInEdge = nextInEdge) {
      ++nextInEdge;

      if(edgeName[*firstInEdge]->name().left(4) == "root")
          remove_edge(*firstInEdge, graph);
    }
}

QString BlockDiagram::generateRootName(void){
    static int i=0;
    i++;

    return QString("root%1").arg(i);
}

void BlockDiagram::demo()
{
    this->listBlocks();
    this->addBlock(new Block("signal_generator","signal_generator1","-o xis"));
    this->addBlock(new Block("signal_generator","signal_generator1","-o xisasd"));
    this->listBlocks();
    this->listLines();
    this->removeBlock("signal_egnge");
    this->removeBlock("signal_generator1");
    this->listBlocks();
    this->listLines();

    this->addBlock(new Block("virtual_ad","virtual_ad1"));
    this->addBlock(new Block("gain","gain1"));
    this->addBlock(new Block("pid","pid1"));

    QDEBUG << "**We should have 3 blocks and 3 lines to root";

    this->listBlocks();
    this->listLines();

    this->addLine("virtual_ad1","gain1","xis");
    this->addLine("gain1","pid1","xis2");
        QDEBUG << "**We should have 3 blocks and 1 line to root";
    this->listBlocks();
    this->listLines();

    this->removeBlock("gain1");
    QDEBUG << "**Removed gain1. pid should now be orphan and connect to root";

    this->listBlocks();
    this->listLines();


    this->addBlock(new Block("signal_generator","signal_generator1"));
    this->addBlock(new Block("sum","sum1"));
    this->addLine("virtual_ad1","sum1","hctl");
    this->addLine("signal_generator1","sum1","setpoint");
    this->addLine("sum1","pid1","et");

    QDEBUG << "**Connected all blocks. Should have root1 and another root connection to sig gen";

    this->listBlocks();
    this->listLines();

    this->removeBlock("signal_generator");
    this->addLine("xis","ypselon","buh");

    QDEBUG << "**Removed signal_generator and added xis to ypselon line";

    this->listBlocks();
    this->listLines();

    this->addBlock(new Block("display","display1",0));
    this->addLine("signal_generator1","display1","HCTL");
    this->addLine("virtual_ad1","display1","SETPOINT");
    this->addLine("sum1","display1","ET");

    this->addBlock(new Block("display","display1",0));
    this->addBlock(new Block("display","display2",0));
    this->addLine("sum1","display2","ET2");

    this->listBlocks();
    this->listLines();

    this->removeBlock("virtual_ad1");
    this->removeBlock("pid1");
    this->removeBlock("display2");
    this->removeBlock("sum1");

    this->addBlock(new Block("gain","gain1"));
    this->addLine("signal_generator1","gain1","hctl");
    this->addLine("gain1","display1","gain1Output");

    this->addBlock(new Block("gain","gain2"));
    this->addLine("gain1","gain2","abcdef");
    this->addLine("gain2","display1","gain2Output");


    this->addBlock(new Block("pid","pid1",1));
    this->addLine("gain2","pid1","a",0);
    this->addLine("pid1","display1","pidOutput",0);
    //this->addLine("pid1","display1","someOtherName",0);


    this->listBlocks();
    this->listLines();

    QDEBUG << "Number of blocks:" << this->numBlocks();
}
/*
bool BlockDiagram::removeDir(const QString &dirName)
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
*/
