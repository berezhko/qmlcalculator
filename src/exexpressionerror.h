#ifndef EXEXPRESSIONERROR_H
#define EXEXPRESSIONERROR_H

#include <exception>
#include <QString>

/**
  * An exception class for internal use in class Calculator.
  */
class ExExpressionError: public std::exception
{
public:
    ExExpressionError(QString text = "Expression Error.");
    virtual ~ExExpressionError()throw(){}
    /** Used to show the cause of the exception
      */
    virtual const char* what() const throw();
 private:
  QString m_Text;
};

#endif // EXEXPRESSIONERROR_H
