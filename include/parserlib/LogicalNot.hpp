#ifndef PARSERLIB_LOGICALNOT_HPP
#define PARSERLIB_LOGICALNOT_HPP


#include "Expression.hpp"
#include "UnaryOperatorsBase.hpp"


namespace parserlib
{


    /**
        A logical NOT operator.
        The given expression can parse once, but it does not advance the input position.
        @param ExpressionType expression type to parse.
     */
    template <class ExpressionType> class LogicalNot :
        public Expression,
        public UnaryOperatorsBase<LogicalNot<ExpressionType>>
    {
    public:
        /**
            constructor.
            @param expression expression to parse.
         */
        LogicalNot(const ExpressionType& expression) :
            m_expression(expression)
        {
        }

    private:
        //expression to parse
        const ExpressionType m_expression;
    };


} //namespace parserlib


#endif //PARSERLIB_LOGICALNOT_HPP
