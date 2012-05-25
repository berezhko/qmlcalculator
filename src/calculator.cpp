#include "calculator.h"
#include <QFile>
#include <QDebug>
#include <math.h>

#include "exexpressionerror.h"

QString Calculator::m_TextSqrt = QChar(0x221A);
QString Calculator::m_TextPi = QChar(0x03C0);
QString Calculator::m_DecimalSeparator = ".";
QString Calculator::m_TextRad = "rad";
QString Calculator::m_TextDeg = "deg";



Calculator::Calculator(QObject *parent) :
    QObject(parent)
{
    m_NextAngleMode = m_TextRad;
    m_AngleMode = m_TextDeg;
    m_x = 0.0;
    //whatever it may be good for. Does not affect GUI, if GUI is not yet ready
    emit logTextChanged(m_LogText);
    emit expressionTextChanged(m_ExpressionText);
    emit angleModeChanged(m_AngleMode, m_NextAngleMode);
    connect(&plotter, SIGNAL(plotSetupChanged(QString)),
            this, SLOT(slot_Plotter_plotSetupChanged(QString)));
    connect(&plotter, SIGNAL(yValueRequest(double,double*)),
            this, SLOT(slot_Plotter_yValueRequest(double,double*)));
}

const QString & Calculator::expressionText()
{
    return m_ExpressionText;
}

/** Append text to Expression. Overwrites a trailing operator with the new one, if a sequence of
  * two operators is input.
  * Recognizes and executes C, AC and = commands.
  */
void Calculator::addToExpressionText(const QString & buttonText)
{
    // QStringList  expressionParts;
    QString text = buttonText;
    try{
        if(text == "C")
            clearLast();
        else if(text == "AC")
            clearAll();
        else if(text == "+/-")
            changeSign();
        else if(text == m_TextRad)
            setAngleMode(text);
        else if(text == m_TextDeg)
            setAngleMode(text);
        else if(text == "=")
        {
            if(isPlotExpression())
                createPlot();
            else parseExpression();
        }
        else if(text=="x")
            addVariable(text);
        else if(text.startsWith("Plot"))
                plotter.processPlotCommand(text);
        else{ //we have a digit, operator or function
            //no clear or evaluate command
            if(text == "x^y") //convert to operator
                text = "^";
            if(isNumber(text)){ //No parenthesis, we have a digit or a decimal separator
                addDigit(text);
                emit guiStateSuggested(STATE_STANDARD); //return to std page, if on Fn Page for entering E
            }
            else if(isOperator(text)) //operator or digit
            {
                addOperator(text);
            }
            else if(isParenthesis(text)) //we have a parenthesis
            {
                addParenthesis(text);
            }
            else if(isFunction(text)) //we have a function
            {
                addFunctionCall(text);
            }
            else if(isVariable(text)) //we have a variable
            {
                addVariable(text);
            }
            else throw ExExpressionError(tr("Unknown command:") + text);

        } //end digit operator or function

        m_ExpressionText = m_ExpressionParts.join("");
        if(isPlotExpression())
            emit expressionTextChanged("y=" + m_ExpressionText);
        else emit expressionTextChanged(m_ExpressionText);
    } //end try
    catch(ExExpressionError e)
    {
        emit guiStateSuggested(STATE_STANDARD);
        emit errorMessage(tr("Input Error"),e.what());
    }
    catch(std::exception e)
    {
        emit guiStateSuggested(STATE_STANDARD);
        emit errorMessage(tr("Unknown Input Error"),e.what());
    }
}

/**
  * Add a function call to expression
  */
void Calculator::addFunctionCall(const QString & functionName)
{
    //function call only in position 1 or behind an operator or opening parenthesis
    if(!((m_ExpressionParts.size() == 0)
            || isOperator(m_ExpressionParts.last())
            || isFunctionPrefix(m_ExpressionParts.last())
            || (m_ExpressionParts.last() == "(")
            || (functionName == "1/x"))) // 1/x is appliccable to a number
     return;

    //operatorlike functions
    if(functionName == "1/x")
    {
        bool isFirst = m_ExpressionParts.isEmpty();
        m_ExpressionParts.prepend("(");
        m_ExpressionParts.prepend("/");
        m_ExpressionParts.prepend("1");
        if(!isFirst)
           m_ExpressionParts.append(")");
    }
    else //normal functions
    {
        if((m_ExpressionParts.size() > 0) && m_ExpressionParts.last() == "arc" && !isFunctionTrigonometric(functionName))
           return; //allow arc only as prefix for trigonometric functions
        if((m_ExpressionParts.size() > 0) && m_ExpressionParts.last() == "ar" && !isFunctionHyperbolic(functionName))
           return; //allow ar only as prefix for hyperbolicic functions

        if((m_ExpressionParts.size() > 0) && isNumber(m_ExpressionParts.last()))
            m_ExpressionParts.append("*"); // add multiplication automatically
        if((m_ExpressionParts.size() > 0) && isFunctionPrefix(m_ExpressionParts.last()))
            m_ExpressionParts.last() += functionName; //combine prefix and name
        else m_ExpressionParts.append(functionName); //add function name
        if(! isFunctionPrefix(functionName))
        {
            m_ExpressionParts.append("("); //and a (
            emit guiStateSuggested(STATE_STANDARD);
        }
    }
}

/**
  * Add an operator call to expression
  */
void Calculator::addOperator(const QString & operatorName)
{
    if((m_ExpressionParts.size() > 0) && isFunctionPrefix(m_ExpressionParts.last()))
        return; //no digits or operators behind a function prefix
    if(isOperator(operatorName))
    {
        if(m_ExpressionParts.size() > 0)
        { //we have parts
            if( isOperator(m_ExpressionParts.last()) )//replace trailing operator
            {
                m_ExpressionParts.removeLast(); // remove the trailing operator or dot in expression
            }
            m_ExpressionParts.append(operatorName);
        }
        else return; //this is the very first input, do not allow an operator here
    }
}

/**
  * Add a digit or decimal separator to expression
  */
void Calculator::addDigit(const QString & digit)
{
    if(digit.length() > 1)
        throw ExExpressionError("Digit text too long:" + digit);
    if(m_ExpressionParts.isEmpty())
        m_ExpressionParts.append(""); //put in an empty string, so that we have a last part
    if((digit ==m_DecimalSeparator ) &&  (m_ExpressionParts.last() == m_DecimalSeparator)) //no double dots
        m_ExpressionParts.last() = "";
    if(isNumber(m_ExpressionParts.last()) )
        m_ExpressionParts.last() += digit; //apppend digit to number
    else if(m_ExpressionParts.last().isEmpty())
    {
        if(digit.toLower() =="e")
            m_ExpressionParts.last() = "1.0e"; //start an E format number
        else m_ExpressionParts.last() += digit; //apppend digit to number
    }
    else if(isOperator(m_ExpressionParts.last()) || m_ExpressionParts.last() == "(")
        m_ExpressionParts.append(digit); //start a new number after operator or openening parenthesis
    //else do nothing!
}

/**
  * Add a variable or constant to expression
  */
void Calculator::addVariable(const QString & variable)
{
    if(m_ExpressionParts.isEmpty())
        m_ExpressionParts.append(variable);
    else if(isOperator(m_ExpressionParts.last())
            || m_ExpressionParts.last() == "(" ) //variables follow operators or functions
          m_ExpressionParts.append(variable);
    else return; //do nothing
    emit guiStateSuggested(STATE_STANDARD);
}

/**
  * Add a parenthesis to expression
  */
void Calculator::addParenthesis(const QString & text)
{
    if(text == "("){
        if((m_ExpressionParts.size() > 0) && isNumber(m_ExpressionParts.last()))
            m_ExpressionParts.append("*"); // add multiplication automatically
        if(m_ExpressionParts.size() >= 2
           && m_ExpressionParts.last()== "-"
           && isOperator(m_ExpressionParts[m_ExpressionParts.size()-2]))
        {
            m_ExpressionParts.removeLast(); //delete trailing "-"
            m_ExpressionParts.append("-("); //and replace it with "-("
        }
        else m_ExpressionParts.append(text);
    }
    else if(text == ")"){
        if((m_ExpressionParts.size() > 0) &&( m_ExpressionParts.last() != "(")) //no () pair
            m_ExpressionParts.append(text);
    }
    else throw ExExpressionError("Parenthesis expected. Found: " + text);
    emit guiStateSuggested(STATE_STANDARD);
}

/** Parse the expression in m_ExpressionText
  * and calculate result.
  * Return the result as a double.
  * If quiet ist true, no signals will be emitted and m_Expressiontext will not be modified.
  * If quiet is false, Log will be updated and m_Expressiontext will be set to the result of the calculation.
  * A logTextChanged and an expressionTextChanged signal will be emitted.
  */
double Calculator::parseExpression(bool quiet)
{
    QStringList expressionParts = m_ExpressionParts; //work on a copy, keep the original
    int partCount = expressionParts.size();
    double result = 0.0;
    try{
        //Parse the expression
        while(partCount >  1)
        {
            parseSubexpression(expressionParts);
            partCount = expressionParts.size();
        }
        if(expressionParts.size() == 0)
            throw ExExpressionError(tr("Expression could not be evaluated."));
        if( ! quiet)
        {
            m_ExpressionText = m_ExpressionParts.join("") + ( "= " + expressionParts[0]);
            //Log the expression and its result
            m_LogText += m_ExpressionText + "\n";
            emit logTextChanged(m_LogText);
            //replace the expression with its result
            m_ExpressionParts.clear();
            m_ExpressionParts.append(expressionParts[0]);
            m_ExpressionText = expressionParts[0];
            emit expressionTextChanged(m_ExpressionText);
            result = m_ExpressionText.toDouble();
        }
        else //keep m_ExpressionParts and m_Expressiontext as they are
        {
            if(isVariable(expressionParts[0]))
                result = getVariableValue(expressionParts[0]);
           else result = expressionParts[0].toDouble();
        }
    }    
    catch(ExExpressionError & e)
    {
        qDebug("Calculator::parseExpression caught ExExpressionError: %s", e.what());
        if(quiet)
            throw e;
        else emit errorMessage(tr("Error"), e.what());
    }
    catch(std::exception &e)
    {
        qDebug("Calculator::parseExpression caught std::exception: %s", e.what());
        if(quiet)
            throw e;
        else emit errorMessage(tr("Error"), e.what());
    }
    catch(...)
    {
        qDebug("Calculator::parseExpression caught unknown exception.");
        if(quiet)
            throw std::exception();
        else emit errorMessage(tr("Error"),"Calculator::parseExpression caught unknown exception.");
    }
    return result;
}

/**
  * Parses and calculates outer parenthesis pair.
  * May be called recursively.
  * Returns the result as a QString.
  */
void Calculator::parseParenthesis(QStringList & expressionParts, int openingParenthesisPos)
{
    int parenthesisCount = 1;
    int nextParenthesisPos = openingParenthesisPos;
    QStringList innerParenthesisExpression; //the expression inside the parenthesis without ()
    if(!isParenthesis(expressionParts[openingParenthesisPos]))
        throw ExExpressionError("Calculator::parseParenthesis: Parenthesis expected! Found:"
                                + expressionParts[openingParenthesisPos]);
    bool isNegative = expressionParts[openingParenthesisPos] == "-(";

    while(parenthesisCount > 0 && nextParenthesisPos < expressionParts.count() - 1) //find matching closing parenthesis
    {
        nextParenthesisPos++;
        if(expressionParts[nextParenthesisPos] == "(")
            parenthesisCount++;
        else if(expressionParts[nextParenthesisPos] == ")")
            parenthesisCount--;

    }
    //Not enough closing parenthesises until end of list
    if(parenthesisCount > 0)
         throw ExExpressionError(tr("Closing parenthesis missing!"));
    if(nextParenthesisPos == openingParenthesisPos + 1)
        throw ExExpressionError(tr("Empty parenthesis pair."));
    //make the inner expression
    for(int part = openingParenthesisPos + 1; part < nextParenthesisPos; part++)
        innerParenthesisExpression+= expressionParts[part];
    //parse it. This loop will calculate the result and put it into parenthesisExpression[0]
    while(innerParenthesisExpression.count() > 1)
       parseSubexpression(innerParenthesisExpression);
    //finally replace the whole parenthesis expression with its result
    //remove parts backwards from closing parenthesis to opening parenthesis + 1
    for(int toRemove = nextParenthesisPos; toRemove > openingParenthesisPos; toRemove--)
        expressionParts.removeAt(toRemove);
    //replace opening parenthesis with the result
    QString result = innerParenthesisExpression[0];
    if(isNegative) //take care of sign
    {
        if(result[0] != '-')
           result = "-" + result; // add -
        else result = result.right(result.length() -1); //delete -
    }
    expressionParts[openingParenthesisPos] = result;
}

/**
  * Parse a subexpression and replace it with its result.
  */
void Calculator::parseSubexpression(QStringList & expressionParts)
{
  int operatorLine = 0;
  int prioLine = 0;
  int priority = 0;
  int parenthesisPos = 0;
  int functionLine = 0;
  for(parenthesisPos = 0; parenthesisPos < expressionParts.size(); parenthesisPos++)
      if((expressionParts[parenthesisPos]== "(") || (expressionParts[parenthesisPos]=="-("))
          parseParenthesis(expressionParts, parenthesisPos);
      else if(expressionParts[parenthesisPos]== ")"){
          throw ExExpressionError(tr("Parenthesis syntax error."));
      }
  //now we have all function arguments directly behind the function name
  if(!(isNumber(expressionParts.last()) || isVariable(expressionParts.last())))
          throw ExExpressionError(tr("Last term of expression must be a number."));
  //evaluate function values from right to left
  for(functionLine = expressionParts.size() - 1; functionLine > -1; functionLine--)
      if(isFunction(expressionParts[functionLine]))
          evaluateFunction(expressionParts, functionLine);
  while(operatorLine < expressionParts.size() &&! isOperator(expressionParts[operatorLine]))
        operatorLine ++;
  if(!operatorLine >= expressionParts.size() - 1) //no operator, invalid expression or nothing to be done
     return; //do nothing

  //we found an operator, now search for the first operator with highest priority
  prioLine = operatorLine;
  priority = operatorPriority(expressionParts[operatorLine]);
  while( prioLine < expressionParts.size() - 1 )
  {
    prioLine ++;
    if(operatorPriority(expressionParts[prioLine]) > priority)
    {
        operatorLine = prioLine;
        priority = operatorPriority(expressionParts[prioLine]);
    }
  }
  //Now lets calculate
  if(operatorLine < 1)
      throw ExExpressionError(tr("No operator allowed in first position."));
  calculateSubExpression(expressionParts, operatorLine);
}

/** Calculates a number-operator-number sequence.
  * operatorLine is the line in the middle.
  * Replaces the three lines with a single Line containing the result of the calculation.
  */
 void Calculator::calculateSubExpression(QStringList & expressionParts, int operatorLine)
 {
    double number1 = 0.0;
    double number2 = 0.0;
    double result = 0.0;
    bool ok = true;

    //check expression syntax
    if(operatorLine == 0 || operatorLine >= expressionParts.size())
        throw ExExpressionError(tr("Invalid expression. Wrong operator position."));
    QString op = expressionParts[operatorLine];

    // check numbers
    QString operand1 = expressionParts[operatorLine -1];
    if(isVariable(operand1))
        number1 = getVariableValue(operand1);
    else number1 = operand1.toDouble( &ok );
    if(!ok)
        throw ExExpressionError(tr("Invalid number format:") + " " + expressionParts[operatorLine - 1]);

    QString operand2 = expressionParts[operatorLine + 1];
    if(isVariable(operand2))
        number2 = getVariableValue(operand2);
    else number2 = operand2.toDouble( &ok );
    if(!ok)
        throw ExExpressionError(tr("Invalid number format:") + " " + expressionParts[operatorLine + 1]);

    //perform calculation
    if(op == "+")
        result = number1 + number2;
    else if(op == "-")
        result = number1 - number2;
    else if(op == "-")
        result = number1 - number2;
    else if(op == "*")
        result = number1 * number2;
    else if(op == "/")
    {
        if(number2 == 0.0)
           throw ExExpressionError(tr("Can not divide by zero."));
        else result = number1 / number2;
        if(abs(result) > 1e12)
            throw ExExpressionError(tr("Can not divide by zero."));
    }
    else if(op == "^")
       result = pow(number1, number2);
    //convert to string, copy to first line and remove line 2 and 3
    QString sResult = QString::number(result, 'G');
    expressionParts[operatorLine - 1] = sResult;
    expressionParts.removeAt(operatorLine);
    expressionParts.removeAt(operatorLine);
 }

 /** Evaluates a function-number sequence.
   * Replaces the two lines with a single line containing the result of the evaluation.
   */
  void Calculator::evaluateFunction(QStringList & expressionParts, int functionLine)
  {
      double result =0.0;
      bool ok = true;
      QString sArgument = expressionParts[functionLine + 1];
      double argument = 0.0;

      if(isVariable(sArgument))
          argument = getVariableValue(sArgument);
      else argument = sArgument.toDouble(&ok);

      if(!ok)
          throw ExExpressionError(tr("Function name must be followed by a number."));
      QString function = expressionParts[functionLine].toLower();
      if(!isFunction(function))
          throw ExExpressionError(tr("Unknown function:") + function);

      if(function == "sin")
          result = sin(toRad(argument));
      else if(function == "arcsin")
      {
          if(argument < -1.0 || argument > 1.0)
              throw ExExpressionError(tr("Argument of arcsin must be in the range from -1 to 1. Found:") + " "
                                      + expressionParts[functionLine + 1] );
          result = fromRad(asin(argument));
      }
      else if(function == "cos")
          result = cos(toRad(argument));
      else if(function == "arccos")
      {
          if(argument < -1.0 || argument > 1.0)
              throw ExExpressionError(tr("Argument of arccos must be in the range from -1 to 1. Found:") + " "
                                      + expressionParts[functionLine+1] );
          result = fromRad(acos(argument));
      }
      else if(function == "tan")
          result = tan(toRad(argument));
      else if(function == "arctan")
      {
          result = fromRad(atan(argument));
      }
      else if(function == "sinh")
          result = sinh(argument);
      else if(function == "arsinh")
      {
          result = asinh(argument);
      }
      else if(function == "cosh")
          result = cosh(argument);
      else if(function == "arcosh")
      {
          if(argument < 1.0 )
              throw ExExpressionError(tr("Argument of arcosh must be >= 1. Found:") + " "
                                      + QString::number(argument) );
          result = acosh(argument);
      }
      else if(function == "tanh")
          result = tanh(argument);
      else if(function == "artanh")
      {
          if(argument < -1.0 || argument > 1.0 )
              throw ExExpressionError(tr("Argument of artanh must be in the range from -1 to 1. Found:") + " "
                                      + QString::number(argument) );
          result = atanh(argument);
      }
      else if(function == "exp")
          result = exp(argument);
      else if(function == "ln")
      {
          if(argument <= 0.0)
              throw ExExpressionError(tr("Argument of ln must be > 0. Found:") + " "
                                      + QString::number(argument) );
          result = log(argument);
      }
      else if(function == "log")
      {
          if(argument < 0.0)
              throw ExExpressionError(tr("Argument of log must be > 0. Found:") + " "
                                      + QString::number(argument) );
          result = log10(argument);
      }
      else if(function == "sqrt" || function == textSqrt())
      {
          if(argument < 0.0)
              throw ExExpressionError(tr("Argument of sqrt must be > 0. Found:") + " "
                                      + QString::number(argument) );
          result = sqrt(argument);
      }
      if(function != textPi()) //Pi has no argument
         expressionParts.removeAt(functionLine +1);
      expressionParts[functionLine] = QString::number(result, 'G');
  }

  /**
    * Calculates plot points and adds them to plotter. Shows plot.
    */
  void Calculator::createPlot()
  {
      bool ok = true;
      if(m_ExpressionText.isEmpty())
          return;
      plotter.clear();
      try{
        plotter.createPlot();
      }
      catch (ExExpressionError & e){
          errorMessage(tr("Error when creating plot"), e.what());
          ok = false;
      }
      catch (...){
          errorMessage(tr("Error"),tr("Unexpected error when creating plot"));
          ok = false;
      }

      emit plotSetupChanged("y=" + m_ExpressionText + "   " + plotter.plotSetup());
      if(ok)
        emit guiStateSuggested(STATE_PLOT);
      else emit guiStateSuggested(STATE_PLOT_ERROR);
  }

  /**
    * To be called on setup changes. Creates a new plot and emits plotSetupChanged
    */
  void  Calculator::slot_Plotter_plotSetupChanged(QString plotSetup)
  {
      createPlot();
      emit plotSetupChanged("y=" + m_ExpressionText + " " + plotSetup);
  }

  /**
    *  calculate a value for the plot.
    */
  void Calculator::slot_Plotter_yValueRequest(double x, double *y)
  {
    m_x = x;
    double result = parseExpression(true); //parse it quietly
    *y = result; // return the result
  }

  /**
   *Returns value of variable.
   */
  double Calculator::getVariableValue(const QString & variable)
  {
      double result = 0.0;
      if(variable.indexOf(textPi()) >= 0)
          result = M_PI;
      else if(variable.indexOf("x") >= 0)
          result =  m_x;
      else throw ExExpressionError("Variable expected. Found: " + variable);
      if(variable[0] == '-')
              return -result;
      else return result;
  }


/** Deletes last character.
  */
void Calculator::clearLast()
{
    if(m_ExpressionParts.size() == 0)
      return;

    m_ExpressionParts.removeLast();

    if(m_ExpressionParts.size() > 0 && isFunction(m_ExpressionParts.last())) //remove ( and function name
        m_ExpressionParts.removeLast();

    m_ExpressionText = m_ExpressionParts.join("");
    emit expressionTextChanged(m_ExpressionText);
}

/** Deletes expresssion text. Deletes Log, when expression is empty.
  */
void Calculator::clearAll()
{
    if(m_ExpressionText.isEmpty())
    {
        m_LogText = "";
        emit logTextChanged(m_LogText);
    }
    m_ExpressionParts.clear();
    m_ExpressionText = "";
    emit expressionTextChanged(m_ExpressionText);
}

/**
  * Changes sign of an operator expression group
  */
void Calculator::changeGroupSign(int operatorPos, QString * expressionString)
{
    if(operatorPos >= 0)
    {
        if( m_ExpressionParts[operatorPos] == "+") // replace + with -
            m_ExpressionParts[operatorPos] = "-";
        else if (m_ExpressionParts[operatorPos] == "-") //replace - with +
            m_ExpressionParts[operatorPos] = "+";
    }
    else if(expressionString->at(0) == '-') //remove sign of expression
        expressionString->remove(0,1); //remove "-"
    else *expressionString = "-" + (*expressionString); //add sign to expression
}

/**Changes sign of last term in expression
  */
void Calculator::changeSign()
{
    if(m_ExpressionParts.size() == 0) //first input
    {
        m_ExpressionParts.append("-"); //that's all
        return;
    }

    //we have text, lets see what we have
    QString lastElement = m_ExpressionParts.last();
    m_ExpressionParts.removeLast();
    if(isOperator(lastElement)) //we have an operator
    {
        if(lastElement == "-")
            lastElement = "+";
        else {
           m_ExpressionParts.append(lastElement);
           m_ExpressionParts.append("-"); // * -, / - or + -
        }

    }
    else if(isParenthesis(lastElement)) //process a parenthesis expression as a unit
    {
        if(lastElement == "(" || lastElement == "-(") //opening, positive or negative
            changeGroupSign(m_ExpressionParts.size() - 1, &lastElement);
        else //closing parenthesis, make the whole expression negative
        {
            //search for corresponding opening parenthesis
            int parenthesiscount = 1;
            int pos= m_ExpressionParts.size();
            while (parenthesiscount > 0 && pos > 0)
            {
                pos--;
                if(m_ExpressionParts[pos] == ")")
                    parenthesiscount++;
                if(m_ExpressionParts[pos] == "(" || m_ExpressionParts[pos] == "-(")
                    parenthesiscount--;
            }
            changeGroupSign(pos - 1, &m_ExpressionParts[pos]);
        }
        m_ExpressionParts.append(lastElement); //finally add the parenthesis
    }
    else{ //no operator, no parenthesis: this leaves a number or a variable
        QString exponent;
        int ePos = lastElement.indexOf("E");
        if(ePos > 0) //we have a number with exponent, change sign of exponent
        {
            exponent = lastElement.right(lastElement.length() - ePos - 1);
            if(exponent[0] == '-')
                exponent.remove(0, 1); //remove sign
            else exponent = '-' + exponent; //add sign
            lastElement = lastElement.left(ePos + 1) + exponent;
        }
        else { //number without exponent or variable
            if(lastElement[0] == '-'){
                lastElement.remove(0, 1); //negative number, remove sign
            }
            else if(m_ExpressionParts.size() > 1 && m_ExpressionParts.last() =="-") // "-" operation
                m_ExpressionParts.last() = "+"; //change to "+"
            else if(m_ExpressionParts.size() > 1 && m_ExpressionParts.last() =="+") // "+" operation
                m_ExpressionParts.last() = "-"; //change to "-"
            else lastElement = '-' + lastElement; //add sign to number
        }
        m_ExpressionParts.append(lastElement);
    }
}

/**
  * Returns true, if c is an operator.
  */
bool Calculator::isOperator(const QString & op)
{
    return op == "+"
        || op == "-"
        || op == "*"
        || op == "/"
        || op == "^";
}

/**
  * Returns true, if fn is one of the  arithmetic functions.
  */
bool Calculator::isFunction(const QString & fn)
{
    QString fnlow = fn.toLower();
    return     fnlow =="sin"
            || fnlow =="cos"
            || fnlow =="tan"
            || fnlow =="arc"
            || fnlow =="arcsin"
            || fnlow =="arccos"
            || fnlow =="arctan"
            || fnlow =="exp"
            || fnlow =="ln"
            || fnlow =="log"
            || fnlow =="sinh"
            || fnlow =="cosh"
            || fnlow =="tanh"
            || fnlow =="ar"
            || fnlow =="arsinh"
            || fnlow =="arcosh"
            || fnlow =="artan"
            || fnlow =="1/x"
            || fnlow =="sqrt"
            || fn ==textSqrt();
}
/**
  * Returns true, if fn is a prefix for one of the  arithmetic functions.
  */
bool Calculator::isFunctionPrefix(const QString & fn)
{
    QString fnlow = fn.toLower();
    return
            fnlow =="arc"
            || fnlow =="ar";
}

/**
  * Returns true, if expression contains a variable (x).
  */
bool Calculator::isPlotExpression()
{
    return m_ExpressionText.indexOf("x") >= 0;
}


/**
  * Returns true, if fn is one of the  trigonometric functions.
  */
bool Calculator::isFunctionTrigonometric(const QString & fn)
{
    return fn == "sin"
            || fn == "cos"
            || fn == "tan";
}

/**
  * Returns true, if fn is one of the hyperbolicic functions.
  */
bool Calculator::isFunctionHyperbolic(const QString & fn)
{
    return fn == "sinh"
            || fn == "cosh"
            || fn == "tanh";
}

/**
  * Returns true, if n can be converted to a number.
  */
bool Calculator::isNumber(const QString & n)
{
    if(n ==m_DecimalSeparator)
        return true;
    if(n.toLower() =="e")
        return true;
    bool result;
    n.toDouble(&result);
    return result;
}

/**
  * Returns true, if name is the name of a variable.
  */
bool Calculator::isVariable(const QString & name)
{
    return name == textPi()
            || name == "-"+textPi()
            || name == "x"
            || name == "-x";
}

/**
  * Returns true, if name is the name of a constant variable like M_PI.
  */
bool Calculator::isConstant(const QString & name)
{
    return name == textPi()
            || name == "-"+textPi();
}

/**
  * Returns true, if expression contains a variable.
  */
bool Calculator::expressionContainsVariable()
{
 bool result = false;

 for(int pos = 0; pos < m_ExpressionParts.size(); pos ++)
     if(isVariable(m_ExpressionParts[pos]) && !isConstant(m_ExpressionParts[pos]))
             result = true;

 return result;
}

/**
  * Returns true, if name is an opening "(" or closing ")" parenthesis.
  * "-(" is treated as an opening parenthesis
  */
bool Calculator::isParenthesis(const QString & name)
{
    return (name == "(") //opening
         || (name == ")"  //closing
         || (name == "-(")); //opening, negative
}

/**
  * Returns true, if name is an opening "(" or "-(" parenthesis.
  * "-(" is treated as an opening parenthesis
  */
bool Calculator::isOpeningParenthesis(const QString & name)
{
return (name == "(") //opening
     || (name == "-("); //opening, negative
}
/**
  * Returns operator priority.
  */
int Calculator::operatorPriority(QString op)
{
    if (op == "+" || op == "-")
      return 1;
    if (op == "*" || op == "/")
      return 2;
    if(op == "^")
      return 3;
  return 0; //no operator
}

/**
  * Loads the protocol file.
  */
bool Calculator::openProtocol(QString fileName)
  {
    QFile protocolFile(fileName);
    if(!protocolFile.exists())
      {
         emit errorMessage(tr("File Open Error"), tr("File does not exist:") + fileName );
         return false;
      }
    if(!protocolFile.open(QIODevice::ReadOnly))
      {
         emit errorMessage(tr("File Open Error"), tr("Can not open file:") + fileName );
         return false;
      }
   return true;
  }

/**
  * Setter for angle mode. Not for external use!
  * Emits angleModeChanged.
  */
void Calculator::setAngleMode(const QString & newMode)
{
    if(newMode == m_TextRad)
    {
        m_AngleMode = m_TextRad;
        m_NextAngleMode = m_TextDeg;
        emit angleModeChanged(m_AngleMode, m_NextAngleMode);
    }
    else if(newMode == m_TextDeg)
    {
        m_AngleMode = m_TextDeg;
        m_NextAngleMode = m_TextRad;
        emit angleModeChanged(m_AngleMode, m_NextAngleMode);
    }
    else errorMessage("Error", "Unknown angle mode: " + newMode);
}

/**
  * Converts degree to radian, if m_AngleMode is Deg.
  */
double Calculator::toRad(double argument)
{
    if(m_AngleMode == m_TextDeg)
        return argument = argument * M_PI / 180.0;
    else return argument;
}

/**
  * Converts radian to degree, if m_AngleMode is Deg.
  */
double Calculator::fromRad(double argument)
{
    if(m_AngleMode == m_TextDeg)
        return argument = argument / M_PI * 180.0;
    else return argument;
}

/**
  * Plot with plotter, using the painter.
  * To be called from QML
  */
void Calculator::plot(QVariant painter, const QVariant plotArea)
{
    QPainter * thePainter;
    uint uintPainter = painter.toUInt();
    thePainter = (QPainter *)uintPainter;
    QRect thePlotArea = plotArea.toRect();
    plotter.plot(thePainter, thePlotArea);
}
