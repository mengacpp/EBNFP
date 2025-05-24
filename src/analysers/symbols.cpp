// TODO improve error messages:
//  a) they are unclear
//  b) if we need to print a special char (eg. \n) it gets treated as is.
#include "analysers/symbols.hpp"

namespace analyser
{
xgn::OutcomeOr<std::vector<Symbol>>
SymbolsAnalyser::analyse(std::vector<ValidChar> source)
{
    std::vector<Symbol> symbols;

    // remove comments from source
    {
        xgn::OutcomeOr<std::vector<ValidChar>> oc = remove_comments(source);

        if (!oc.is_ok())
            return oc.outcome();

        m_source = *oc;
    }

    m_current_char = 0;

    while (peek().type != ValidCharType::NaC)
    {
        // make sure next char is a symbol
        remove_gap();

        if (peek().type == ValidCharType::NaC)
            break;

        // check if its a string
        {
            Symbol string;
            xgn::OutcomeOr<bool> oc = try_get_terminal_string(string);

            if (!oc.is_ok())
            {
                return oc.outcome();
            }

            bool is_string = *oc;
            if (is_string)
            {
                symbols.push_back(string);
                continue;
            }
        }

        // check if its another symbol
        {
            Symbol symbol;
            auto oc = try_get_symbol(symbol);

            if (!oc.is_ok())
            {
                return oc.outcome();
            }

            bool is_symbol = *oc;
            if (is_symbol)
            {
                symbols.push_back(symbol);
                continue;
            }
        }

        // ERROR it's not a string nor a symbol
        // this shouldn't really happen
        return xgn::Outcome(peek().file_pos + ": Invalid symbol '" +
                            peek().value + "'");
    }

    return symbols;
}

xgn::OutcomeOr<std::vector<ValidChar>>
SymbolsAnalyser::remove_comments(std::vector<ValidChar> source)
{
    std::vector<ValidChar> old_source = m_source;
    int old_current = m_current_char;

    m_current_char = 0;
    m_source = source;

    std::vector<ValidChar> out;
    while (peek().type != ValidCharType::NaC)
    {
        if (peek().type == ValidCharType::Sym_CommentStart)
        {
            auto oc = remove_comment();

            if (!oc.is_ok())
                return oc;
            continue;
        }
        else if (peek().type == ValidCharType::Sym_CommentEnd)
        {
            return xgn::Outcome(peek().file_pos + ": Invalid symbol '" +
                                peek().value + "'");
        }

        out.push_back(eat());
    }

    m_source = old_source;
    m_current_char = old_current;

    return out;
}

xgn::OutcomeOr<bool> SymbolsAnalyser::try_get_symbol(Symbol &out)
{
    if (is_terminal_char(peek()) &&
        (peek().type != ValidCharType::Sym_QuoteFirst &&
         peek().type != ValidCharType::Sym_QuoteSecond))
    {
        SymbolType t = chart_to_symbolt(peek().type);

        out = Symbol(t, eat().value);
        return true;
    }

    return false;
}
xgn::OutcomeOr<bool> SymbolsAnalyser::try_get_terminal_string(Symbol &out)
{
    if (peek().type != ValidCharType::Sym_QuoteFirst &&
        peek().type != ValidCharType::Sym_QuoteSecond)
        return false;

    ValidCharType delimiter_t = eat().type;

    std::string val;
    while (peek().type != ValidCharType::NaC)
    {
        if (peek().type == delimiter_t)
        {
            break;
        }

        if (!is_terminal_char(peek()) &&
            peek().type != ValidCharType::SpaceChar)
        {
            return xgn::Outcome(peek().file_pos +
                                ": Invalid character in string '" +
                                peek().value + "'");
        }

        val.append(eat().value);
    }
    eat();

    out = Symbol(SymbolType::TerminalString, val);
    return true;
}

xgn::Outcome SymbolsAnalyser::remove_comment()
{
    if (peek().type != ValidCharType::Sym_CommentStart)
        return xgn::Outcome::ok();

    std::string comment_pos = peek().file_pos;

    while (peek().type != ValidCharType::NaC)
    {
        if (peek().type == ValidCharType::Sym_CommentEnd)
        {
            eat();
            return xgn::Outcome::ok();
        }
        eat();
    }

    return xgn::Outcome(comment_pos + ": comment does not close");
}

void SymbolsAnalyser::remove_gap()
{
    while (peek().type != ValidCharType::NaC)
    {
        if (peek().type == ValidCharType::SpaceChar ||
            peek().type == ValidCharType::HorizontalTabChar ||
            peek().type == ValidCharType::VerticalTabChar ||
            peek().type == ValidCharType::FormFeed ||
            peek().type == ValidCharType::NewLineChar)
        {
            eat();
            continue;
        }

        break;
    }
}

ValidChar SymbolsAnalyser::peek(int depth)
{
    int target = m_current_char + depth;

    if (target < 0 || target > m_source.size() - 1)
        return ValidChar(ValidCharType::NaC, "", '\0');

    return m_source.at(target);
}

ValidChar SymbolsAnalyser::eat()
{
    if (m_current_char > m_source.size() - 1)
        return ValidChar(ValidCharType::NaC, "", '\0');

    return m_source.at(m_current_char++);
}

bool SymbolsAnalyser::is_terminal_char(ValidChar c)
{
    if (c.type == ValidCharType::Letter ||
        c.type == ValidCharType::DecimalDigit ||
        c.type == ValidCharType::Sym_Concat ||
        c.type == ValidCharType::Sym_Define ||
        c.type == ValidCharType::Sym_DefinitionSeparator ||
        c.type == ValidCharType::Letter ||
        c.type == ValidCharType::DecimalDigit ||
        c.type == ValidCharType::Sym_Concat ||
        c.type == ValidCharType::Sym_Define ||
        c.type == ValidCharType::Sym_DefinitionSeparator ||
        c.type == ValidCharType::Sym_CommentEnd ||
        c.type == ValidCharType::Sym_GroupEnd ||
        c.type == ValidCharType::Sym_OptionEnd ||
        c.type == ValidCharType::Sym_RepeatEnd ||
        c.type == ValidCharType::Sym_Except ||
        c.type == ValidCharType::Sym_QuoteFirst ||
        c.type == ValidCharType::Sym_Repetition ||
        c.type == ValidCharType::Sym_QuoteSecond ||
        c.type == ValidCharType::Sym_SpecialSequence ||
        c.type == ValidCharType::Sym_CommentStart ||
        c.type == ValidCharType::Sym_GroupStart ||
        c.type == ValidCharType::Sym_OptionStart ||
        c.type == ValidCharType::Sym_RepeatStart ||
        c.type == ValidCharType::Sym_Terminator ||
        c.type == ValidCharType::OtherChar)
    {
        return true;
    }

    return false;
}

SymbolType SymbolsAnalyser::chart_to_symbolt(ValidCharType ct)
{
    switch (ct)
    {

    case ValidCharType::Letter:
        return SymbolType::Letter;
    case ValidCharType::DecimalDigit:
        return SymbolType::DecimalDigit;
    case ValidCharType::Sym_QuoteFirst:
        return SymbolType::NaS;
    case ValidCharType::Sym_QuoteSecond:
        return SymbolType::NaS;
    case ValidCharType::Sym_Concat:
        return SymbolType::Concat;
    case ValidCharType::Sym_Define:
        return SymbolType::Define;
    case ValidCharType::Sym_DefinitionSeparator:
        return SymbolType::DefinitionSeparator;
    case ValidCharType::Sym_Except:
        return SymbolType::Except;
    case ValidCharType::Sym_Repetition:
        return SymbolType::Repetition;
    case ValidCharType::Sym_CommentStart:
        return SymbolType::NaS;
    case ValidCharType::Sym_CommentEnd:
        return SymbolType::NaS;
    case ValidCharType::Sym_GroupStart:
        return SymbolType::GroupStart;
    case ValidCharType::Sym_GroupEnd:
        return SymbolType::GroupEnd;
    case ValidCharType::Sym_OptionStart:
        return SymbolType::OptionStart;
    case ValidCharType::Sym_OptionEnd:
        return SymbolType::OptionEnd;
    case ValidCharType::Sym_RepeatStart:
        return SymbolType::RepeatStart;
    case ValidCharType::Sym_RepeatEnd:
        return SymbolType::RepeatEnd;
    case ValidCharType::Sym_SpecialSequence:
        return SymbolType::SpecialSequence;
    case ValidCharType::Sym_Terminator:
        return SymbolType::Terminator;
    case ValidCharType::OtherChar:
        return SymbolType::OtherChar;
    case ValidCharType::SpaceChar:
        return SymbolType::NaS;
    case ValidCharType::HorizontalTabChar:
        return SymbolType::NaS;
    case ValidCharType::NewLineChar:
        return SymbolType::NaS;
    case ValidCharType::VerticalTabChar:
        return SymbolType::NaS;
    case ValidCharType::FormFeed:
        return SymbolType::NaS;
    case ValidCharType::NaC:
        return SymbolType::NaS;
    }
}

}