#ifndef PARSERLIB_OPTIONAL_HPP
#define PARSERLIB_OPTIONAL_HPP


#include "Expression.hpp"
#include "UnaryOperatorsBase.hpp"


namespace parserlib
{


    /**
        An optional operator.
        The given expression can parse once optionally.
        @param ExpressionType expression type to parse optionally.
     */
    template <class ExpressionType> class Optional :
        public Expression,
        public UnaryOperatorsBase<Optional<ExpressionType>>
    {
    public:
        /**
            constructor.
            @param expression expression to parse.
         */
        Optional(const ExpressionType& expression) :
            m_expression(expression)
        {
        }

    private:
        //expression to parse
        const ExpressionType m_expression;
    };


} //namespace parserlib


#endif //PARSERLIB_OPTIONAL_HPP
