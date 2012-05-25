#ifndef PLOTTER_H
#define PLOTTER_H

#include <QDeclarativeItem>
#include <QPolygon>
#include <QPainter>
#include <QFont>

/** A plotter to be used with an activated QPainter. May be run in a QML or a QWidget environment.
  * The following plotter commands are supported:
  * Scaling of the Y-axis: Y=f(x)    Y=ln(f(x))     Y=1/f(x)
  * Scaling of the X axis: X=x       X=ln(x)        X=1/x
  * X-Range:               From x=   To x=          Dx=
  * where Dx is the distance between two x-values
  * Diagram styles:        Line      Dots           Bars
  * Indicator position:    x-Index
  * x-Index is the index of the plot point, whose coordinates are to be displayed in status.
  * Valid values are from 0 to 99.
  */

class Plotter : public QObject
{
    Q_OBJECT
    typedef enum{
        Linear,
        LogarithmicBaseE,
        LogarithmicBase10,
        Reciprocal
    }AxisMode;

    typedef enum{
        LineDiagram,
        DotDiagram,
        BarDiagram
    }DiagramStyle;

    Q_PROPERTY(int penWidth READ getPenWidth WRITE setPenWidth)
    Q_PROPERTY(int fontSize READ getFontSize WRITE setFontSize)
    Q_PROPERTY(QString fontFamily READ getFontFamily WRITE setFontFamily)

public:
    explicit Plotter(QObject *parent = 0);

    /**
      * Draw the plot in plotArea using the painter. Painter must already be active.
      */
    void plot(QPainter* painter, const QRect & plotArea);

    void setPenWidth(int newWidth){penWidth = newWidth;}
    int getPenWidth(){return penWidth;}
    QString getFontFamily(){return font.family();}
    void setFontFamily(const QString & newFont){font.setFamily( newFont);}
    int getFontSize(){return font.pixelSize();}
    void setFontSize(int newSize){font.setPixelSize(newSize); autoFontSize = false;}
signals:
    /**
      * Sent whenever the setup was modified.
      */
    void plotSetupChanged(QString plotSetup);
    /**
      * Sent whenever a y-value is needed for the corresponding x value.
      * Receiver should calculate y and copy it to y*.
      */
    void yValueRequest(double x, double * y);

public slots:
    void processPlotCommand(QString command);

    /**
      * Returns the parameters of the plot setup as a readable string.
      */
    QString plotSetup();
      /**
      * Set the plot caption.
      */
    void setCaption(const QString & newCaption){caption = newCaption;}

    /**
      * Clear the points lists.
      */
    void clear(){pointsF.clear(); }

    /**
      * Create a plot by sending an yValueRequest signal for every point required for the plot.
      * Number of points is caculated as: (maximum.x - minimum.x) / dx
      */
    void createPlot();


private:
    double axisModeTransformation(AxisMode mode, double in);
    QPoint toPixelCoordinates(const QPointF & p);
    void drawXAxis(QPainter * painter);
    void drawYAxis(QPainter * painter);
    void setupPlotRect(const QRect & plotArea);
    /**
      * round to 2 significant digits.
      */
    double yCeil(double y);
    /**
      * round to 2 significant digits.
      */
    double yFloor(double y);
    /**
      * Returns false, if y is == InvalidNumber
      */
     bool isValid(double y);
     /**
       * Returns false, y is == InvalidNumber
       */
      bool isValidPoint(const QPointF & point);

    QString caption;
    QColor bkColor;
    QColor lineColor;
    //points in original double values
    QPolygonF pointsF;
    //transformed points in pixel coordinates
    QPolygon pointsPix;
    QPoint originPix;
    QPointF originF;
    QPointF maximumF;
    QPointF minimumF;
    int nPoints;
    double dx;
    QRect plotRect;
    bool autoFontSize;
    QFont font;
    int tickLength;
    int border;
    int penWidth;
    int indicatorIndex;
    double xScale;
    double yScale;
    AxisMode axisModeX;
    AxisMode axisModeY;
    DiagramStyle diagramStyle;
};

#endif // PLOTTER_H
