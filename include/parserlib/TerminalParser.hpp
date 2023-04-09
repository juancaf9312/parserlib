#ifndef PARSERLIB_TERMINALPARSER_HPP
#define PARSERLIB_TERMINALPARSER_HPP


#include "ParserNode.hpp"


namespace parserlib {


    /**
     * A parser that parses a terminal.
     * @param TerminalValueType value type of the terminal.
     */
    template <class TerminalValueType> class TerminalParser 
        : public ParserNode<TerminalParser<TerminalValueType>> {
    public:
        /**
         * Constructor.
         * @param terminalValue terminal value.
         */
        TerminalParser(const TerminalValueType& terminalValue)
            : m_terminalValue(terminalValue) {
        }

        /**
         * Returns the terminal value.
         * @return the terminal value.
         */
        const TerminalValueType& terminalValue() const {
            return m_terminalValue;
        }

        /**
         * Compares the current token, if there is one, to the terminal value.
         * If equal, then it advances the source position by one.
         * @param pc parse context.
         * @return true if parsing succeeds, false otherwise.
         */
        template <class ParseContextType> bool operator ()(ParseContextType& pc) const {
            if (pc.sourcePosition() != pc.sourceEndPosition()) {
                if (*pc.sourcePosition() == m_terminalValue) {
                    pc.incrementSourcePosition();
                    return true;
                }
            }
            return false;
        }

        /**
         * Parses a terminal under left recursion.
         * @param pc parse context.
         * @return true if a terminal was parsed, false otherwise.
         */
        template <class ParseContextType> bool parseLeftRecursionTerminal(ParseContextType& pc) const {
            return operator ()(pc);
        }

        /**
         * Parses a left recursion continuation.
         * @param pc parse context.
         * @return true on success, false otherwise.
         */
        template <class ParseContextType> bool parseLeftRecursionContinuation(ParseContextType& pc) const {
            return false;
        }

    private:
        //the terminal value.
        const TerminalValueType m_terminalValue;
    };


    /**
     * Helper function for creating a terminal parser.
     * @param terminalValue the terminal value.
     * @return a terminal parser.
     */
    template <class TerminalValueType> 
    TerminalParser<TerminalValueType> terminal(const TerminalValueType& terminalValue) {
        return TerminalParser<TerminalValueType>(terminalValue);
    }


} //namespace parserlib


#endif //PARSERLIB_TERMINALPARSER_HPP
