#pragma once

#include <string>
#include <vector>

#include "oxygen/outcome.hpp"

namespace analyser
{

//! a valid char can be 2 "normal" char long.

enum class ValidCharType
{
    Letter,       //: a-zA-z
    DecimalDigit, //: 0-9

    Sym_QuoteFirst,  //: '
    Sym_QuoteSecond, //: "

    Sym_Concat,              //: ,
    Sym_Define,              //: =
    Sym_DefinitionSeparator, //: |, /, !
    Sym_Except,              //: -
    Sym_Repetition,          //: *

    Sym_CommentStart, //: (*
    Sym_CommentEnd,   //: *)

    Sym_GroupStart, //: (
    Sym_GroupEnd,   //: )

    Sym_OptionStart, //: [, (/
    Sym_OptionEnd,   //: ], /)

    Sym_RepeatStart, //: {, (:
    Sym_RepeatEnd,   //: }, :)

    Sym_SpecialSequence, //: ?

    Sym_Terminator, //: ;, .

    OtherChar, //: :, +, _, %, @, &, #, $, <, >, \, ^, `, ~

    SpaceChar,         //: ' ',
    HorizontalTabChar, //: '\t',
    NewLineChar,       //: '\n',
    VerticalTabChar,   //: '\v',
    FormFeed,          //: '\f',
};

struct ValidChar
{
    ValidChar(ValidCharType type, std::string value) : type(type), value(value)
    {
    }
    ValidChar(ValidCharType type, char value)
        : type(type), value(std::string() + value)
    {
    }
    ValidCharType type;
    std::string value;
};

class CharsAnalyser
{
public:
    xgn::OutcomeOr<std::vector<ValidChar>> analyse(std::string path);

private:
    xgn::Outcome start_analysis(std::string path);

    std::string get_current_pos();

    char peek(int depth = 0);
    char eat();

    size_t m_current_char;
    size_t m_current_ln;
    size_t m_current_lnc;
    std::string m_source;
    std::string m_path;
};

} // namespace analysers