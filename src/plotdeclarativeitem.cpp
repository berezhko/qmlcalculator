#include "plotdeclarativeitem.h"
#include <QStyleOptionGraphicsItem>
#include <QRegion>

PlotDeclarativeItem::PlotDeclarativeItem(QDeclarativeItem *parent) :
    QDeclarativeItem(parent)
{
    // Important, otherwise the paint method is never called
    setFlag(QGraphicsItem::ItemHasNoContents, false);
}

QRectF PlotDeclarativeItem::boundingRect () const
{
    qDebug("boundingRect called");
    return QRectF(0,0,width(),height());
}

/**
  * Called, whenever the plotter needs to be painted.
  *
  */
void PlotDeclarativeItem::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    if(! option->exposedRect.intersects(boundingRect()))
        return; //no need to plot
    setClip(true); //activate clipping
   // qDebug("PlotDeclarativeItem::paint called");
    plotRect = QRect(0,0, width(),height());
    QRegion oldClipRegion= painter->clipRegion();
    painter->setClipRect(boundingRect()); //clip to boundingRect
    painter->setClipping(true);
    emit plotRequest(QVariant ((uint) painter), QVariant(plotRect));
    painter->setClipping(false);//paint the rest of the screen
    painter->setClipRegion(oldClipRegion);
}
