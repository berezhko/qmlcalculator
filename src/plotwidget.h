/*
 *  Author: Walter Roth, University of Applied Science Suedwestfalen, Germany
 *  Copyright by Walter Roth 2012
 *  License: GPL
 */
#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <QFrame>

#include "plotter.h"

/**
  * A wrapper class that is used as a QWidget based paintdevice for a Plotter object.
  * Whenever the plot needs to be painted, a plotRequest signal is emitted.
  * Connect this signal to the Plotter's plot() slot.
  */
class PlotWidget : public QFrame
{
    Q_OBJECT
public:
    explicit PlotWidget(QWidget *parent = 0);
protected:
    /**
      * Called whenever the widget nees to be painted. Emits plotRequest.
      */
    void paintEvent ( QPaintEvent * pe );

signals:
    /**
      * Emitted whenever the plot needs to be painted.
      */
    void plotRequest(QPainter* painter, const QRect plotArea);
};

#endif // PLOTWIDGET_H
