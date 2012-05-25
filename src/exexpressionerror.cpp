#include "exexpressionerror.h"

ExExpressionError::ExExpressionError(QString text)
{
    m_Text = text;
}

const char * ExExpressionError::what()const throw()
{
   qDebug("%s", qPrintable(m_Text));
   return qPrintable(m_Text);
}
