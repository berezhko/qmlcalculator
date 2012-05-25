#ifndef PLOTDECLARATIVEITEM_H
#define PLOTDECLARATIVEITEM_H

#include <QDeclarativeItem>
#include "plotter.h"


/**
  * A class that supplies a painting area for a plotter.
  * Emits plotRequest when the plot needs to be painted.
  * painter is a uint based QVariant. To retrieve the QPainter * from the painter QVariant proceed as follows:
  * QPainter * thePainter = (QPainter*)painter.toUint();
  * painter is already activated and ready to paint on the PlotDeclarativeItem items window area.
  */
class PlotDeclarativeItem : public QDeclarativeItem
{
    Q_OBJECT
public:
    explicit PlotDeclarativeItem(QDeclarativeItem *parent = 0);
    /** Returns the size of the plot
      */
    QRectF boundingRect () const;

    /**
      * Called, whenever the plot needs to be painted.
      */
    void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );


signals:
    /**
      * This signal is for use with QML components. Therefore it contains only QVariant types.
      */
    void plotRequest(QVariant painter, QVariant plotRect);

public slots:
   void updatePlot(){update();}

 private:
   QRect plotRect;
};

#endif // PLOTDECLARATIVEITEM_H
