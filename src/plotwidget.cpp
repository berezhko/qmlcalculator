#include "plotwidget.h"
#include <QPainter>

PlotWidget::PlotWidget(QWidget *parent) :
    QFrame(parent)
{
}

/**
  * Called whenever the widget nees to be painted. Emits plotRequest.
  */
void PlotWidget::paintEvent ( QPaintEvent * pe )
{
    QPainter painter(this); //create and activate a painter
    QRect plotRect(0,0, width(),height()); //and give it to the plotter
    emit plotRequest(&painter, plotRect);
 }
