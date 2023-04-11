#ifndef PARSERLIB_SEQUENCEPARSER_HPP
#define PARSERLIB_SEQUENCEPARSER_HPP


#include "ParserNode.hpp"
#include "TerminalParser.hpp"
#include "TerminalStringParser.hpp"


namespace parserlib {


    /**
     * Sequence of parsers.
     * All parsers must parse successfully in order to parse the sequence successfully.
     * @param Children children parser nodes.
     */
    template <class ...Children> class SequenceParser : public ParserNode<SequenceParser<Children...>> {
    public:
        /**
         * Constructor.
         * @param children children nodes.
         */
        SequenceParser(const std::tuple<Children...>& children) 
            : m_children(children) {
        }

        /**
         * Returns the children nodes.
         * @return the children nodes.
         */
        const std::tuple<Children...>& children() const {
            return m_children;
        }

        /**
         * Invokes all child parsers, one by one, until one returns false.
         * @param pc parse context.
         * @return true if parsing succeeds, false otherwise.
         */
        template <class ParseContextType> bool operator ()(ParseContextType& pc) const {
            //keep initial state in order to later restore it if subsequence parser fails
            const auto state = pc.state();

            //parse children; all children must parse
            const bool ok = parse<0>(pc);

            //if parsing fails, restore the initial state
            if (!ok) {
                pc.setState(state);
            }

            return ok;
        }

    private:
        std::tuple<Children...> m_children;

        template <size_t Index, class ParseContextType> bool parse(ParseContextType& pc) const {
            if constexpr (Index < sizeof...(Children)) {
                if (std::get<Index>(m_children)(pc)) {
                    return parse<Index + 1>(pc);
                }
                return false;
            }
            else {
                return true;
            }
        }
    };


    /**
     * Creates a sequence of parsers out of two parsers.
     * @param node1 1st node.
     * @param node2 2nd node.
     * @return a sequence of parsers.
     */
    template <class ParserNodeType1, class ParserNodeType2>
    SequenceParser<ParserNodeType1, ParserNodeType2>
    operator >> (const ParserNode<ParserNodeType1>& node1, const ParserNode<ParserNodeType2>& node2) {
        return SequenceParser<ParserNodeType1, ParserNodeType2>(
            std::make_tuple(
                static_cast<const ParserNodeType1&>(node1), 
                static_cast<const ParserNodeType2&>(node2)));
    }


    /**
     * Creates a sequence of parsers out of two sequence parsers.
     * It flattens the structures into one sequence.
     * @param node1 1st node.
     * @param node2 2nd node.
     * @return a sequence of parsers.
     */
    template <class ...Children1, class ...Children2>
    SequenceParser<Children1..., Children2...>
    operator >> (const ParserNode<SequenceParser<Children1...>>& node1, const ParserNode<SequenceParser<Children2...>>& node2) {
        return SequenceParser<Children1..., Children2...>(
            std::tuple_cat(
                static_cast<const SequenceParser<Children1...>&>(node1).children(), 
                static_cast<const SequenceParser<Children2...>&>(node2).children()));
    }


    /**
     * Creates a sequence of parsers out of a sequence parser and a non-sequence parser.
     * @param node1 1st node.
     * @param node2 2nd node.
     * @return a sequence of parsers.
     */
    template <class ...Children1, class ParserNodeType2>
    SequenceParser<Children1..., ParserNodeType2>
    operator >> (const ParserNode<SequenceParser<Children1...>>& node1, const ParserNode<ParserNodeType2>& node2) {
        return SequenceParser<Children1..., ParserNodeType2>(
            std::tuple_cat(
                static_cast<const SequenceParser<Children1...>&>(node1).children(), 
                std::make_tuple(static_cast<const ParserNodeType2&>(node2))));
    }


    /**
     * Creates a sequence of parsers out of a non-sequence parser and a sequence parser.
     * It flattens the structures into one sequence.
     * @param node1 1st node.
     * @param node2 2nd node.
     * @return a sequence of parsers.
     */
    template <class ParserNodeType1, class ...Children2>
    SequenceParser<ParserNodeType1, Children2...>
        operator >> (const ParserNode<ParserNodeType1>& node1, const ParserNode<SequenceParser<Children2...>>& node2) {
        return SequenceParser<ParserNodeType1, Children2...>(
            std::tuple_cat(
                std::make_tuple(static_cast<const ParserNodeType1&>(node1)), 
                static_cast<const SequenceParser<Children2...>&>(node2).children()));
    }


    /**
     * Creates a sequence of parsers out of a parser and a char.
     * @param node parser node.
     * @param ch char.
     * @return a sequence of parsers.
     */
    template <class ParserNodeType> 
    auto operator >> (const ParserNode<ParserNodeType>& node, char ch) {
        return node >> TerminalParser<char>(ch);
    }


    /**
     * Creates a sequence of parsers out of a parser and a char string.
     * @param node parser node.
     * @param str char string.
     * @return a sequence of parsers.
     */
    template <class ParserNodeType>
    auto operator >> (const ParserNode<ParserNodeType>& node, const char* str) {
        return node >> TerminalStringParser<char>(str);
    }


    /**
     * Creates a sequence of parsers out of a parser and a wchar_t.
     * @param node parser node.
     * @param ch wchar_t.
     * @return a sequence of parsers.
     */
    template <class ParserNodeType>
    auto operator >> (const ParserNode<ParserNodeType>& node, wchar_t ch) {
        return node >> TerminalParser<wchar_t>(ch);
    }


    /**
     * Creates a sequence of parsers out of a parser and a wchar_t string.
     * @param node parser node.
     * @param str wchar_t string.
     * @return a sequence of parsers.
     */
    template <class ParserNodeType>
    auto operator >> (const ParserNode<ParserNodeType>& node, const wchar_t* str) {
        return node >> TerminalStringParser<wchar_t>(str);
    }


    /**
     * Creates a sequence of parsers out of a parser and a char16_t.
     * @param node parser node.
     * @param ch char16_t.
     * @return a sequence of parsers.
     */
    template <class ParserNodeType>
    auto operator >> (const ParserNode<ParserNodeType>& node, char16_t ch) {
        return node >> TerminalParser<char16_t>(ch);
    }


    /**
     * Creates a sequence of parsers out of a parser and a char16_t string.
     * @param node parser node.
     * @param str char16_t string.
     * @return a sequence of parsers.
     */
    template <class ParserNodeType>
    auto operator >> (const ParserNode<ParserNodeType>& node, const char16_t* str) {
        return node >> TerminalStringParser<char16_t>(str);
    }


    /**
     * Creates a sequence of parsers out of a parser and a char32_t.
     * @param node parser node.
     * @param ch char32_t.
     * @return a sequence of parsers.
     */
    template <class ParserNodeType>
    auto operator >> (const ParserNode<ParserNodeType>& node, char32_t ch) {
        return node >> TerminalParser<char32_t>(ch);
    }


    /**
     * Creates a sequence of parsers out of a parser and a char32_t string.
     * @param node parser node.
     * @param str char32_t string.
     * @return a sequence of parsers.
     */
    template <class ParserNodeType>
    auto operator >> (const ParserNode<ParserNodeType>& node, const char32_t* str) {
        return node >> TerminalStringParser<char32_t>(str);
    }


    /**
     * Creates a sequence of parsers out of a char and a parser.
     * @param ch char.
     * @param node node.
     * @return a sequence of parsers.
     */
    template <class ParserNodeType>
    auto operator >> (char ch, const ParserNode<ParserNodeType>& node) {
        return TerminalParser<char>(ch) >> node;
    }


    /**
     * Creates a sequence of parsers out of a char string and a parser.
     * @param str char string.
     * @param node node.
     * @return a sequence of parsers.
     */
    template <class ParserNodeType>
    auto operator >> (const char* str, const ParserNode<ParserNodeType>& node) {
        return TerminalStringParser<char>(str) >> node;
    }


    /**
     * Creates a sequence of parsers out of a wchar_t and a parser.
     * @param ch wchar_t.
     * @param node node.
     * @return a sequence of parsers.
     */
    template <class ParserNodeType>
    auto operator >> (wchar_t ch, const ParserNode<ParserNodeType>& node) {
        return TerminalParser<wchar_t>(ch) >> node;
    }


    /**
     * Creates a sequence of parsers out of a wchar_t string and a parser.
     * @param str wchar_t string.
     * @param node node.
     * @return a sequence of parsers.
     */
    template <class ParserNodeType>
    auto operator >> (const wchar_t* str, const ParserNode<ParserNodeType>& node) {
        return TerminalStringParser<wchar_t>(str) >> node;
    }


    /**
     * Creates a sequence of parsers out of a char16_t and a parser.
     * @param ch char16_t.
     * @param node node.
     * @return a sequence of parsers.
     */
    template <class ParserNodeType>
    auto operator >> (char16_t ch, const ParserNode<ParserNodeType>& node) {
        return TerminalParser<char16_t>(ch) >> node;
    }


    /**
     * Creates a sequence of parsers out of a char16_t string and a parser.
     * @param str char16_t string.
     * @param node node.
     * @return a sequence of parsers.
     */
    template <class ParserNodeType>
    auto operator >> (const char16_t* str, const ParserNode<ParserNodeType>& node) {
        return TerminalStringParser<char16_t>(str) >> node;
    }


    /**
     * Creates a sequence of parsers out of a char32_t and a parser.
     * @param ch char32_t.
     * @param node node.
     * @return a sequence of parsers.
     */
    template <class ParserNodeType>
    auto operator >> (char32_t ch, const ParserNode<ParserNodeType>& node) {
        return TerminalParser<char32_t>(ch) >> node;
    }


    /**
     * Creates a sequence of parsers out of a char32_t string and a parser.
     * @param str char32_t string.
     * @param node node.
     * @return a sequence of parsers.
     */
    template <class ParserNodeType>
    auto operator >> (const char32_t* str, const ParserNode<ParserNodeType>& node) {
        return TerminalStringParser<char32_t>(str) >> node;
    }


} //namespace parserlib


#endif //PARSERLIB_SEQUENCEPARSER_HPP
