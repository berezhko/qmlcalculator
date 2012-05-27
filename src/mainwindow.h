/*
 *  Author: Walter Roth, University of Applied Science Suedwestfalen, Germany
 *  Copyright by Walter Roth 2012
 *  License: GPL
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "calculator.h"

namespace Ui {
    class MainWindow;
}

/**
  * The Main window for a QWidget based Calculator GUI. Uses mainwindow.ui as GUI.
  * NOT recommended for use on mobile devices. Use the QML GUI on mobile devices.
  *
  * This class was created to demonstrate the QWidget and QML capabilities of the
  * calculator and plotter classes.
  */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
 public slots:
    /** Slots for digit button clicks. Add digit to expression.
      */

    //Std page
    void on_pushButtonFn_clicked();
    void on_pushButtonClear_clicked();
    void on_pushButtonAllClear_clicked();

    void on_pushButton0_clicked();
    void on_pushButton1_clicked();
    void on_pushButton2_clicked();
    void on_pushButton3_clicked();
    void on_pushButton4_clicked();
    void on_pushButton5_clicked();
    void on_pushButton6_clicked();
    void on_pushButton7_clicked();
    void on_pushButton8_clicked();
    void on_pushButton9_clicked();

    // Operator buttons on Std page. Add operator to expression
    void on_pushButtonDivide_clicked();
    void on_pushButtonMultiply_clicked();
    void on_pushButtonSubtract_clicked();
    void on_pushButtonAdd_clicked();

    void on_pushButtonEqual_clicked();
    void on_pushButtonDot_clicked();
    void on_pushButtonPlusMinus_clicked();

    //Function page
    void on_pushButtonStd_clicked();
    void on_pushButtonPlot_clicked();
    void on_pushButtonParenthesisOpen_clicked();
    void on_pushButtonParenthesisClose_clicked();
    void on_pushButtonE_clicked();
    void on_pushButtonSin_clicked();
    void on_pushButtonCos_clicked();
    void on_pushButtonTan_clicked();
    void on_pushButtonArc_clicked();
    void on_pushButtonSinh_clicked();
    void on_pushButtonCosh_clicked();
    void on_pushButtonTanh_clicked();
    void on_pushButtonAr_clicked();
    void on_pushButtonExp_clicked();
    void on_pushButtonLn_clicked();
    void on_pushButtonLog_clicked();
    void on_pushButtonSqrt_clicked();
    void on_pushButton1OverX_clicked();
    void on_pushButtonAngleMode_clicked();
    void on_pushButtonPower_clicked();


   //PlotPage
    void on_pushButtonPlotStd_clicked();
    void on_pushButtonPlotFn_clicked();
    void on_pushButtonPlotHelp_clicked();
    void on_pushButtonY_clicked();
    void on_pushButtonY1overfx_clicked();
    void on_pushButtonYlnfx_clicked();
    void on_pushButtonXx_clicked();
    void on_pushButtonX1overx_clicked();
    void on_pushButtonXlnx_clicked();
    void on_pushButtonFromX_clicked();
    void on_pushButtonToX_clicked();
    void on_pushButtonDx_clicked();
    void on_pushButtonX_clicked();

   //PlotPage in input mode
    void on_pushButtonPlotDescription_clicked();
    void on_pushButtonPlotC_clicked();
    void on_pushButtonPlotOk_clicked();


//Menu related stuff
    /**
      * Exit program.
      */
    void on_actionExit_triggered();
    /**
      * Open log file
      */
    void on_actionOpen_triggered();

   //Calculator interface

    /**Process the calculator's response
      */
    void slot_Calculator_expressionTextChanged(const QString & text);
    /**Process the calculator's response
      */
    void slot_Calculator_logTextChanged(const QString & text);

    /**Process the calculator's response
      */
    void slot_Calculator_plotSetupChanged(const QString & text);
    /**
      * Show the error message.
      */
    void slot_Calculator_errorMessage(QString caption, QString msg);

    /**
      * Change the state of the GUI.
      */
    void slot_Caculator_guiStateSuggested(QString state);

private:
    /**
      * Send command to plotter.
      */
    void sendPlotCommand(const QString & command);
    /**
      * Show the numerical input elements for plot parameters.
      */
    void activatePlotInput(const QString & text);

    Ui::MainWindow *ui;
    Calculator calculator;
};

#endif // MAINWINDOW_H
