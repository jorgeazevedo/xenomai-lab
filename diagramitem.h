#ifndef DIAGRAMITEM_H
#define DIAGRAMITEM_H

//#include "blockregistry.h"
#include "superblock.h"
#include <QGraphicsPixmapItem>
#include <QList>
//#include "linepath.h"

QT_BEGIN_NAMESPACE
class QPixmap;
class QGraphicsItem;
class QGraphicsScene;
class QTextEdit;
class QGraphicsSceneMouseEvent;
class QMenu;
class QGraphicsSceneContextMenuEvent;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;
class QPolygonF;
QT_END_NAMESPACE

class LinePath;

class DiagramItem : public QGraphicsPolygonItem
{

public:
    enum { Type = UserType + 15 };
    enum DiagramType { Step, Conditional, StartEnd, Io, Triangle, Square };

    DiagramItem(const SuperBlock& block, DiagramType diagramType, QMenu *contextMenu,
                QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);
    DiagramItem(QString blockType,QString Name, DiagramType diagramType, QMenu *contextMenu=0,
        QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);

    void drawPoligon(DiagramType type);

    void paintLinePaths(const QColor& color,bool highlight);
    void removeLinePath(LinePath *linePath);
    void removeLinePaths();
    DiagramType diagramType() const
        { return myDiagramType; }
    QPolygonF polygon() const
        { return myPolygon; }
    void addLinePathIn(LinePath *linePath);
    void addLinePathOut(LinePath *linePath);
    QPixmap image() const;
    QString blockType() const;
    int type() const
        { return Type;}
    QString name();

    qreal getEntryHeight(LinePath* linePath);

public slots:



protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:

    QGraphicsScene *myScene;
    QGraphicsTextItem* nameLabel;
    QGraphicsTextItem* blockTypeLabel;
    DiagramType myDiagramType;
    QPolygonF myPolygon;
    QMenu *myContextMenu;
    QList<LinePath *> linePathsIn;
    QList<LinePath *> linePathsOut;
    QString myName;
    QString myBlockType;

};

#endif
