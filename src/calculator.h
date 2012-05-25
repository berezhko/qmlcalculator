/******************************************************************************
 *
 *  The logic for a scientific calculator. Compatible with QWidget or QML GUIs.
 *  Author: Walter Roth
 *  Copyright by Walter Roth 2011. License: GPL
 *
 ******************************************************************************/

#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QObject>
#include <QStringList>

#include "plotter.h"

/**
  * The controller logic for a scientific calculator.
  * Communicates with a GUI via text input and signals containing texts to be displayed.
  *
  * The expression to be calculated is input with the addToExpressionText slot.
  * When an input was processed, a signal expressionTextChanged is emitted.
  * Errors are reported with signal errorMessage.
  * Function names may be upper- mixed- or lowercase.
  * The following function command strings from GUI are supported:
  * sin, cos, tan, arcsin, arccos, arctan
  * sinh, cosh, tanh, arsinh, arcosh, artanh
  * exp, ln, log, x^y
  * sqrt, 1/x
  * Unicode strings for Pi, deg, rad, sqrt are also supported.
  * Use the textXXX functions to set the Pi, deg, rad, sqrt button texts in GUI.
  * These will show nice symbols.
  * Parenthesis () and +,-,*,/ operator commands are recognized.
  * Operator priority from highest priority group to lowest: ^ ( * / ) ( + - )
  * Only negative sign (-) is indicated. No sign means positive.
  *
  *
  * Plotter commands must begin with PlotCommand: followed by a blank and the command.
  * The following plotter commands are supported:
  * Scaling of the Y-axis: Y=f(x)    Y=ln(f(x))     Y=1/f(x)
  * Scaling of the X axis: X=x       X=ln(x)        X=1/x
  * X-Range:               From x=   To x=          Dx=
  * where Dx is the distance between two x-values
  * Diagram styles:        Line      Dots           Bars
  */

#define STATE_FUNCTIONS "Functions"
#define STATE_STANDARD "Standard"
#define STATE_PLOT "Plot"
#define STATE_PLOT_ERROR "PlotError"

class Calculator : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString plotFontFamily READ getPlotFontFamily WRITE setPlotFontFamily)
    Q_PROPERTY(int plotFontSize READ getPlotFontSize WRITE setPlotFontSize)
    Q_PROPERTY(int plotPenWidth READ getPlotPenWidth WRITE setPlotPenWidth)

public:
    explicit Calculator(QObject *parent = 0);

    /**
      * Plotter setup properties
      */
    int getPlotPenWidth(){return plotter.getPenWidth();}
    void setPlotPenWidth(int newWidth){plotter.setPenWidth(newWidth);}
    QString getPlotFontFamily(){return plotter.getFontFamily();}
    void setPlotFontFamily(const QString & newFont){plotter.setFontFamily( newFont);}
    int getPlotFontSize(){return plotter.getFontSize();}
    void setPlotFontSize(int newSize){plotter.setFontSize(newSize);}

public slots:
    /**getter
      */
    const QString & expressionText();
    /**
      * Returns the parameters of the plot setup as a readable string.
      */
    QString plotSetup(){return plotter.plotSetup();}
    /**
      * To be called on setup changes. Creates a new plot and emits plotSetupChanged
      */
    void slot_Plotter_plotSetupChanged(QString plotSetup);
    /**
      * plot with plotter, using the painter.
      * To be called from a QWidget.
      */
    void plot(QPainter * painter, const QRect& plotArea){plotter.plot(painter, plotArea);}
    /**
      * Plot with plotter, using the painter.
      * To be called from QML. QML can only supply QVariant types as response to a signal.
      */
    void plot(QVariant painter, const QVariant plotArea);
    /**
      *  calculate a value for the plot.
      */
    void slot_Plotter_yValueRequest(double x, double *y);

    /** Append text to expression. Overwrites a trailing operator with the new one, if a sequence of
      * two operators is input.
      * Recognizes and executes C, AC, +/-, function and = commands.
      */
    void addToExpressionText(const QString & text);
    /**
      * Loads the protocol file.
      */
    bool openProtocol(QString fileName);
    /**
      * Returns unicode char for a squareroot.
      */
    QString textSqrt(){return m_TextSqrt;}
    /**
      * Returns unicode char for a squareroot.
      */
    QString textPi(){return m_TextPi;}
    /**
      * Returns unicode char for a squareroot.
      */
    QString textRad(){return m_TextRad;}
    /**
      * Returns unicode char for a squareroot.
      */
    QString textDeg(){return m_TextDeg;}
    /**
      * Returns decimal separator used by Calculator.
      */
    QString textDecimalSeparator(){return m_DecimalSeparator;}
signals:
    /**
      * GUI should display logText in a logwindow
      */
    void logTextChanged(QString logText);
    /**
      * The currently active expression text. To be displayed by GUI.
      */
    void expressionTextChanged(QString expressionText);
    /**
      * The currently active plotter setup. To be displayed by GUI.
      */
    void plotSetupChanged(QString plotSetup);
    /**
      * Caption and msg to be displayed by GUI in a modal Dialog.
      */
    void errorMessage(QString caption, QString msg);
    /**
      * GUI should display angleMode in a status indicator window.
      * nextAngleMode should be displayed on the anglemode button.
      */
    void angleModeChanged(QString angleMode, QString nextAngleMode);
    /**
      * GUI should switch to state.
      * Values for mode: "Functions" "Standard" "Plot"
      */
    void guiStateSuggested(QString state);


private:
    /**
      * Add a function call to expression
      */
    void addFunctionCall(const QString & functionName);
    /**
      * Add an operator call to expression
      */
    void addOperator(const QString & operatorName);
    /**
      * Add a digit or decimal separator to expression
      */
    void addDigit(const QString & digit);
     /**
      * Add a parenthesis to expression
      */
    void addParenthesis(const QString & text);
    /**
      * Add a variable or constant to expression
      */
    void addVariable(const QString & variable);
    /** Parse the expression in m_ExpressionText
      * and calculate result.
      * Returns the result as a double.
      * If quiet ist true, no signals will be emitted and m_Expressiontext will not be modified.
      * If quiet is false, Log will be updated and m_Expressiontext will be set to the result of the calculation.
      * A logTextChanged and an expressionTextChanged signal will be emitted.
      */
    double parseExpression(bool quiet = false);
    /**
      * Parse a subexpression and replace it with its result.
      */
    void parseSubexpression(QStringList & expressionParts);

    /**
      * Parses and calculates outer parenthesis pair in expression.
      * May be called recursively.
      * Replaces parenthesis expression with its result as a string.
      */
    void parseParenthesis(QStringList & expressionParts, int parenthesisPos);

    /**
      * Returns true, if op is an operator.
      */
    bool isOperator(const QString & op);
    /**
      * Returns true, if fn is one of the  arithmetic functions.
      */
    bool isFunction(const QString & fn);
    /**
      * Returns true, if fn is aprefix for one of the  arithmetic functions.
      */
    bool isFunctionPrefix(const QString & fn);
    /**
      * Returns true, if expression contains a variable (x).
      */
    bool isPlotExpression();
    /**
      * Returns true, if fn is one of the  trigonometric functions.
      */
    bool isFunctionTrigonometric(const QString & fn);
    /**
      * Returns true, if fn is one of the hyperbolicic functions.
      */
    bool isFunctionHyperbolic(const QString & fn);
    /**
      * Returns true, if n can be converted to a number.
      */
    bool isNumber(const QString & n);
    /**
      * Returns true, if name is the name of a variable or constant.
      */
    bool isVariable(const QString & name);    
    /**
      * Returns true, if name is the name of a constant variable like M_PI.
      */
    bool isConstant(const QString & name);
    /**
      * Returns true, if expression contains a variable.
      */
    bool expressionContainsVariable();
    /**
      * Returns true, if name is an opening "(", "-(" or closing ")" parenthesis.
      * "-(" is treated as an opening parenthesis
      */
    bool isParenthesis(const QString & name);
    /**
      * Returns true, if name is an opening "(" or "-(" parenthesis.
      * "-(" is treated as an opening parenthesis
      */
    bool isOpeningParenthesis(const QString & name);
    /**
      * Returns operator priority.
      */
    int operatorPriority(QString op);
    /** Calculates a number-operator-number sequence.
      * operatorLine is the line in the middle.
      * Replaces the three lines with a single Line containing the result of the calculation.
      */
     void calculateSubExpression(QStringList & expressionParts, int operatorLine);
     /** Evaluates a function-number sequence.
       * Replaces the two lines with a single line containing the result of the evaluation.
       */
      void evaluateFunction(QStringList & expressionParts, int functionLine);
      /**
        * Caculates plot points and adds them to plotter. Shows plot.
        */
      void createPlot();
      /**
       *Returns value of variable.
       */
      double getVariableValue(const QString & variable);
      /**
       * Setter for angle mode. Not for external use!
       * Emits angleModeChanged.
       */
     void setAngleMode(const QString & newMode);

     /**
       * Converts degree to radian, if m_AngleMode is Deg.
       */
     double toRad(double argument);
     /**
       * Converts radian to degree, if m_AngleMode is Deg.
       */
     double fromRad(double argument);

     /**Changes sign of last term in expression
       */
     void changeSign();
     /**
       * Changes sign of an operator expression group
       */
     void changeGroupSign(int operatorPos, QString * expressionString);

     /** Deletes last character.
       */
     void clearLast();
     /** Deletes expresssion text. Deletes Log, when expression is empty.
       */
     void clearAll();
    //The text to be displayed by GUI
    QString m_ExpressionText;
    //The complete expression as entered by user
    QStringList m_ExpressionParts;
    //The log to be displayed by GUI
    QString m_LogText;
    //The anglemode that shall be activated on next click on anglemode button
    QString m_NextAngleMode;
    //The anglemode that is presently active. To be displayed by GUI
    QString m_AngleMode;
    //The unicode for sqrt
    static QString m_TextSqrt;
    //The unicode for pi
    static QString m_TextPi;
    //The text for rad
    static QString m_TextRad;
    //The text for deg
    static QString m_TextDeg;
    //The decimal separator used
    static QString m_DecimalSeparator;
    //The plotter
    Plotter plotter;
    // The x variable
    double m_x;
};

#endif // CALCULATOR_H
