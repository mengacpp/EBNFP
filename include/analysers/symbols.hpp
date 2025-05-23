#pragma once

#include "chars.hpp"

#include "oxygen/outcome.hpp"

#include <string>
#include <vector>

namespace analyser
{

enum class SymbolType
{
    TerminalString = 0, // "terminal string"
    Letter,             //: a-zA-z
    DecimalDigit,       //: 0-9

    Concat,              //: ,
    Define,              //: =
    DefinitionSeparator, //: |, /, !
    Except,              //: -
    Repetition,          //: *

    GroupStart, //: (
    GroupEnd,   //: )

    OptionStart, //: [, (/
    OptionEnd,   //: ], /)

    RepeatStart, //: {, (:
    RepeatEnd,   //: }, :)

    SpecialSequence, //: ?

    Terminator, //: ;, .

    OtherChar, //: :, +, _, %, @, &, #, $, <, >, \, ^, `, ~

    NaS = 100, // not a symbol
};

struct Symbol
{
    Symbol() : type(SymbolType::NaS), value("") {}
    Symbol(SymbolType type, std::string value = "") : type(type), value(value)
    {
    }
    SymbolType type;
    std::string value;
};

class SymbolsAnalyser
{
public:
    xgn::OutcomeOr<std::vector<Symbol>> analyse(std::vector<ValidChar> source);

private:
    ValidChar peek(int depth = 0);
    ValidChar eat();

    xgn::OutcomeOr<std::vector<ValidChar>>
    remove_comments(std::vector<ValidChar> source);

    xgn::Outcome remove_comment();
    void remove_gap();

    xgn::OutcomeOr<bool> try_get_symbol(Symbol &out);
    xgn::OutcomeOr<bool> try_get_terminal_string(Symbol &out);

    bool is_terminal_char(ValidChar c);

    SymbolType chart_to_symbolt(ValidCharType ct);

    std::vector<ValidChar> m_source;
    size_t m_current_char;
};
} // namespace analyser