#include "plotter.h"

#include <math.h>

#include "exexpressionerror.h"

// a small number avoids integer overflow when converted to an int
#define INVALID_NUMBER 1.0e-100


Plotter::Plotter(QObject *parent) :
    QObject(parent)
{
    bkColor = Qt::black;
    lineColor = Qt::white;
    minimumF = QPointF(-1.0, -1.0);
    maximumF = QPointF(1.0, 1.0);
    axisModeX = Linear;
    axisModeY = Linear;
    diagramStyle = LineDiagram;
    plotRect = QRect(0,0,400,300);
    tickLength = font.pixelSize() / 2;
    setupPlotRect(plotRect);
    nPoints = 101; //include last position
    dx = 0.0;
    border = 5;
    penWidth = 1;
    indicatorIndex = -1;
    font.setFamily("Helvetica");
}

void Plotter::setupPlotRect(const QRect & plotArea)
{
    plotRect = plotArea;
    plotRect.adjust(border, border, -border, -border);
    bool xAxisTop = originF.y() == maximumF.y();
    bool xAxisBottom = originF.y() == minimumF.y();
    bool yAxisLeft = minimumF.x() >= 0;
    bool yAxisRight = maximumF.x() <= 0;

    //First set floating point origin of logical coordinate system
    //set x-axis position
    int plotHeight = plotRect.height();
    if( xAxisTop || xAxisBottom)//x-axis is at top or bottom, we need extra space
        plotHeight = plotHeight - font.pixelSize() - tickLength; // make space for x-axis texts
    yScale =  plotHeight / (axisModeTransformation(axisModeY,maximumF.y()) - axisModeTransformation(axisModeY,minimumF.y()));

    //set y-axis position
    int plotWidth = plotRect.width();
    if( yAxisLeft )
        originF.setX(minimumF.x());
    else if(yAxisRight)
        originF.setX(maximumF.x());
    else originF.setX(0.0);
    if(yAxisLeft) //y-axis is at left side, we need extra space
        plotWidth = plotWidth - 3 * font.pixelSize() - tickLength; //make space for y-axis texts
    xScale = plotWidth / (axisModeTransformation(axisModeX,maximumF.x()) - axisModeTransformation(axisModeX, minimumF.x()));

    if(minimumF.y() >= 0)
        originF.setY(minimumF.y());
    else if( maximumF.y() <= 0)
        originF.setY(maximumF.y());
    else originF.setY(0.0);
    if(autoFontSize)
      font.setPixelSize(plotRect.height() / 30);

    //Now set the crossover point of the axes in pixel coordinates: originPix
    double xPos = (axisModeTransformation(axisModeX,originF.x()) - axisModeTransformation(axisModeX,minimumF.x())) /
                  (axisModeTransformation(axisModeX,maximumF.x()) - axisModeTransformation(axisModeX,minimumF.x()));
    double yPos = (axisModeTransformation(axisModeY,originF.y()) - axisModeTransformation(axisModeY,minimumF.y())) /
                  (axisModeTransformation(axisModeY,maximumF.y()) - axisModeTransformation(axisModeY,minimumF.y()));

    //calculate offsets for axis texts
    int offsetY = 0;
    if(xAxisTop)
        offsetY = plotRect.height() - plotHeight;
    else if(xAxisBottom)
        offsetY = plotHeight - plotRect.height();
    int offsetX = 0;
    if(yAxisLeft)
        offsetX = plotRect.width() - plotWidth;
    //set crossover point
    originPix.setX( plotRect.x() + xPos * plotRect.width() + offsetX);
    originPix.setY( plotRect.y() + plotRect.height() - yPos * plotRect.height() + offsetY);

    //create points in pixel coordinates
    pointsPix.clear();
    for(int nPoint = 0; nPoint < pointsF.size(); nPoint++)
    {
        QPoint point = toPixelCoordinates(pointsF[nPoint]);
        pointsPix.append(point);
    }
}


/**
  * Draw the plot using the painter.
  */
void Plotter::plot(QPainter * painter, const QRect & plotArea)
{
    setupPlotRect(plotArea);
    //first draw the background rect
    painter->setBrush(bkColor);
    painter->setPen(QPen(lineColor,penWidth));
    painter->setFont(font);

    painter->drawRect(plotArea); //draw the background

    int firstPoint = 0;
    int lastPoint = 0;
    while(lastPoint < pointsF.size())
    {
        while(isValidPoint(pointsF[lastPoint]) && lastPoint < pointsF.size() - 1)
            lastPoint ++;
        switch(diagramStyle)
        {
         case BarDiagram:
        {
            for(int nPoint= firstPoint; nPoint <= lastPoint; nPoint++ )
            {
                QPen oldPen = painter->pen();
                QPen greenPen = QPen(Qt::green, penWidth);
                QPen redPen = QPen(Qt::red, penWidth);
                if(pointsF[nPoint].y() > 0.0)
                    painter->setPen(greenPen);
                else painter->setPen(redPen);
                painter->drawLine(pointsPix[nPoint], QPoint(pointsPix[nPoint].x(), originPix.y() + penWidth ));
                painter->setPen(oldPen);
            }
        }; // no break, draw white points in addition
        case DotDiagram:
        {
            for(int nPoint= firstPoint; nPoint <= lastPoint; nPoint++ )
                painter->drawPoint(pointsPix[nPoint]);
        }    break;

        default:
            painter->drawPolyline(&pointsPix[firstPoint], lastPoint - firstPoint + 1);
        }
        lastPoint ++;
        firstPoint = lastPoint;
    }

    //draw indicator line
    if((indicatorIndex >= 0) && (indicatorIndex < pointsPix.size()) )
    {
        QPen oldPen = painter->pen();
        QPen bluePen = QPen(QColor(128, 128, 255), penWidth);
        painter->setPen(bluePen);
        painter->drawLine(pointsPix[indicatorIndex], QPoint(pointsPix[indicatorIndex].x(), originPix.y() + penWidth ));
        painter->setPen(oldPen);
    }

    //draw the axes on top
    drawXAxis(painter);
    drawYAxis(painter);
}

/**
  * Create a plot by sending an yValueRequest signal for every point required for the plot.
  * Number of points is caculated as: (maximum.x - minimum.x) / dx
  */
void Plotter::createPlot()
{
    dx = (maximumF.x() - minimumF.x()) / (nPoints - 1);
    double fx = 1.0;
    double minY = 1.0E99;
    double maxY = -1.0E99;
    double y = 0.0;
    double x = 0.0;
    QString lastError;
    int errorCount = 0;
    if(axisModeX == LogarithmicBaseE)
    {
        if(minimumF.x() <= 0.0)
            throw ExExpressionError(tr("Minimum x value must be > 0 for logarithmic scaling."));
        fx =  pow(maximumF.x() / minimumF.x(), 1.0 / nPoints);
    }
    for(int nPoint = 0; nPoint < nPoints; nPoint++)
    {
        if(axisModeX == LogarithmicBaseE)
            x =  pow(fx, nPoint);
        else x = minimumF.x() + nPoint * dx;
        y = 0.0; //make it zero, just in case if we do not get an answer

        try
        {
          emit yValueRequest(x, &y); //this may throw divide by zero or similar exceptions
        }
        catch(ExExpressionError &e)//catch Expressionerrors and mark point as invalid
        {
            errorCount++;
            if(errorCount > 5)
                lastError = e.what();
            y= INVALID_NUMBER; //mark this point as invalid
        }
        catch(...)//catch everything and mark point as invalid
        {
            lastError = "Unexpected exception when calculating.";
            y= INVALID_NUMBER; //mark this point as invalid
        };

        if( isValid(y) && (y < minY))
            minY = y;
        if(isValid(y) && (y > maxY))
            maxY = y;
        pointsF.append(QPointF(x,y));
    }

    double maxRoundError = ((maxY * 10.0) - round(maxY * 10.0)) / 10.0;
    double minRoundError = ((minY * 10.0) - round(minY * 10.0)) / 10.0;
    double range = yCeil(maxY - minY);


    //Set y limits to "reasonable" numbers (2 digit round)
    if(fabs(maxRoundError) < fabs(minRoundError))
    {
        maximumF.setY(yCeil(maxY));
        minimumF.setY(maximumF.y() - range);
    }
    else
    {
       // minimumF.setY(floor(minY));
        minimumF.setY(yFloor(minY));
        maximumF.setY(minimumF.y() + range);
    }
    if(!lastError.isEmpty())
        throw ExExpressionError(lastError);
}

/**
  * round to 2 significant digits.
  */
double Plotter::yCeil(double y)
{
    int exponent = 0;
    double result = y;
    if(result < 10.0)
    {
        while (fabs(result) < 10.0)
        {
            result *= 10.0;
            exponent ++;
        }
        result = ceil(result);
        for(int i= 0; i < exponent; i++)
            result /= 10.0;
    }
    else
    {
        while (fabs(result) < 10.0)
        {
            result /= 10.0;
            exponent ++;
        }
        result = ceil(result);
        for(int i= 0; i < exponent; i++)
            result *= 10.0;

    }
    return result;
}

/**
  * round to 2 significant digits.
  */
double Plotter::yFloor(double y)
{
    int exponent = 0;
    double result = y;
    if(result < 10.0)
    {
        while (fabs(result) < 10.0)
        {
            result *= 10.0;
            exponent ++;
        }
        result = floor(result);
        for(int i= 0; i < exponent; i++)
            result /= 10.0;
    }
    else
    {
        while (fabs(result) < 10.0)
        {
            result /= 10.0;
            exponent ++;
        }
        result = floor(result);
        for(int i= 0; i < exponent; i++)
            result *= 10.0;

    }
    return result;
}

/**
  * Returns false, if y is == InvalidNumber
  */
 bool Plotter::isValid(double y)
 {
     return y != INVALID_NUMBER;
 }

 /**
   * Returns false, y is == InvalidNumber
   */
  bool Plotter::isValidPoint(const QPointF & point)
  {
      return point.y() != INVALID_NUMBER;
  }


/**
  *Parse a plot command. Commands are expected to begin with "PlotCommand: "
  */
void Plotter::processPlotCommand(QString command)
{
    qDebug("Plotter::processPlotCommand: %s", qPrintable(command));
    QStringList commandParts = command.split(" "); //command may have 2 to 4 parts
    QString cmd = commandParts[1];
    bool ok =true;
    double val = 0.0;
    int index = 0;

    //Axis scaling modes
    if(cmd == "Y=f(x)")
        axisModeY = Linear;
    else if(cmd == "Y=ln(f(x))")
        axisModeY = LogarithmicBaseE;
    else if(cmd == "Y=1/f(x)")
        axisModeY = Reciprocal;
    if(cmd == "X=x")
        axisModeX = Linear;
    else if(cmd == "X=ln(x)")
        axisModeX = LogarithmicBaseE;
    else if(cmd == "X=1/x")
        axisModeX = Reciprocal;

    //Range settings
    if((cmd == "From") || (cmd == "To"))
    {
        if(commandParts.size() != 4)
            throw ExExpressionError("Plot command from/to syntax error:" + command);
        if(commandParts[3].trimmed().isEmpty()) //no range given, skip command
            return;
        val = commandParts[3].toDouble(&ok);
        if(!ok)
            throw ExExpressionError(tr("Range must be given as numbers"));
        if(cmd == "From")
            minimumF.setX(val);
        else  if(cmd == "To")
            maximumF.setX(val);

    }
    //Position of indicator
    if(cmd == "x-Index"){
        if(commandParts.size() != 3)
            throw ExExpressionError("Plot command x-Index syntax error:" + command);
        val = commandParts[2].toDouble(&ok);
        if(!ok)
            throw ExExpressionError("Syntax error in plot indicator position! Command was: " + command);
        else index = (floor(val)); //take the lower number
        if(indicatorIndex <= pointsF.size())
           indicatorIndex = index;
        else indicatorIndex = pointsF.size() -1;
    }

    //automatic correction of range for reciprocal or logarithmic x-axis
    if((axisModeX == Reciprocal) || (axisModeX == LogarithmicBaseE)
        && ((minimumF.x() <= 1e-30) ||(maximumF.x() <= 2e-30)))
    {
        if(minimumF.x() < 0.0)
            minimumF.setX(0.01);
        if(maximumF.x() < 1.0)
            maximumF.setX(1.0);
    }

    //automatic corretion of min > max
    if(minimumF.x() >= maximumF.x())
        maximumF.setX(minimumF.x() + 1.0);

    //Diagram types
    if(cmd == "Line")
        diagramStyle = LineDiagram;
    else if(cmd =="Dots")
        diagramStyle = DotDiagram;
    else if(cmd =="Bars")
        diagramStyle = BarDiagram;

    emit plotSetupChanged(plotSetup());
}


/**
  * Transforms float point to pixel coordinates.
  */
QPoint Plotter::toPixelCoordinates(const QPointF & p)
{
    double xF = axisModeTransformation( axisModeX, p.x());
    double yF = axisModeTransformation( axisModeY, p.y());
    int x = (xF -originF.x()) * xScale;
    int y = -(yF - originF.y())* yScale; //y values from bottom to top
    return QPoint(x,y) + originPix;
}

double Plotter::axisModeTransformation(AxisMode mode, double in)
{
    switch (mode)
    {
    case LogarithmicBaseE:
    {
        if( in <= 0.0)
            return 0.0;
        else return log(in);
    }break;
    case Reciprocal:
    {
        if(abs(in) < 1e-20)
        {
            if(in < 0.0)
                return -1e20;
            else return 1e20;
        }
        return 1.0 / in;
    }break;
    default: return in;break;
    }
}

void Plotter::drawXAxis(QPainter * painter)
{
    int left = plotRect.x();
    int right = left + plotRect.width();
    tickLength = font.pixelSize() / 2;
    painter->drawLine(left, originPix.y(), right, originPix.y());

    QPoint tick = QPoint(0, tickLength);
    QPoint textSize = QPoint(0, painter->fontMetrics().height());
    int nTicks = 2;
    if(axisModeX == Linear)
    {
        double plusRange = maximumF.x() - originF.x();
        double minusRange = originF.x() - minimumF.x();
        double tickDistance = plusRange / nTicks;
        if(minusRange > plusRange)
            tickDistance = minusRange / nTicks;
        for(int i = -nTicks; i < nTicks; i++)
        {
            QPointF pointF = QPointF(originF.x() + i* tickDistance, originF.y());
            QPoint pointTick = toPixelCoordinates(pointF);
            QPoint pointTick2 = pointTick + tick;
            painter->drawLine(pointTick, pointTick2);
            QString text = QString::number(pointF.x());
            QPoint pointText = pointTick2 + textSize;
            painter->drawText(pointText, text);
        }
    }
}

void Plotter::drawYAxis(QPainter * painter)
{
    QPoint top = toPixelCoordinates(QPointF(originF.x(), maximumF.y()));
    QPoint bottom = toPixelCoordinates(QPointF(originF.x(), minimumF.y()));
    painter->drawLine(top, bottom);

    int tickLength = font.pixelSize() / 2;
    QPoint tick = QPoint(tickLength, 0);
    QPoint textSize;
    int nTicks = 2;
    if(axisModeY == Linear)
    {
        double plusRange = maximumF.y() - originF.y();
        double minusRange = originF.y() - minimumF.y();
        double tickDistance;
        if(minusRange > plusRange)
            tickDistance = minusRange / nTicks;
        else tickDistance = plusRange / nTicks;
        for(int i = -nTicks; i < nTicks; i++)
        {
            QPointF pointF = QPointF(originF.x(), originF.y() + i* tickDistance);
            QPoint pointTick = toPixelCoordinates(pointF);
            QPoint pointTick2 = pointTick - tick;
            painter->drawLine(pointTick, pointTick2);
            QString text = QString::number(pointF.y());
            textSize = QPoint(painter->fontMetrics().boundingRect(text).width(), 0);
            QPoint pointText = pointTick2 - textSize;
            painter->drawText(pointText, text);
        }
    }
}

/**
  * Returns the parameters of the plot setup as a readable string.
  */
QString Plotter::plotSetup()
{
    QString result;
 /*   switch (axisModeY){
    case Linear: result += "Y=f(x)"; break;
    case LogarithmicBaseE:
    case LogarithmicBase10: result += "Y=ln(f(x))"; break;
    case Reciprocal: result += "Y=1/f(x)"; break;
    default: result +="yMode?";break;
    }
    result+=" ";
    switch (axisModeX){
    case Linear: result += "X=x"; break;
    case LogarithmicBaseE:
    case LogarithmicBase10: result += "X=ln(x)"; break;
    case Reciprocal: result += "X=1/x"; break;
    default: result +="xMode?";break;
    }
    result+=" ";
*/
    result += tr("From x=") + QString::number(minimumF.x());
    result+=" ";
    result += tr("To x=") + QString::number(maximumF.x());
    result += "\n";

    if( (indicatorIndex >= 0) && (indicatorIndex < pointsF.size()) )
    {
        result +=
                tr("Cursor at x=") + QString::number(pointsF[indicatorIndex].x())
                + tr(" y=") + QString::number(pointsF[indicatorIndex].y()) + "   ";
    }

    if(diagramStyle == BarDiagram) //calculate area
    {
        double area = 0.0;
        for(int nPoint = 0; nPoint < pointsF.size(); nPoint++)
        {
            if(isValidPoint(pointsF[nPoint]))
                area += dx * pointsF[nPoint].y();
        }
        result += " ";
        result += tr("Total Area: ")+ QString::number(area, 'g', 3);
    }
    return result;
}

