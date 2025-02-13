// Copyright (c) Lawrence Livermore National Security, LLC and other VisIt
// Project developers.  See the top-level LICENSE file for dates and other
// details.  No copyright assignment is required to contribute to VisIt.

#include "VisItGrammar.h"
#include "Colors.h"
#include "VisItToken.h"
#include <stdio.h>
#include <set>

using     std::map;
using     std::set;
using     std::string;
using     std::vector;

// ****************************************************************************
//  Constructor:  Grammar::Grammar
//
//  Programmer:  Jeremy Meredith
//  Creation:    April  5, 2002
//
//  Modifications:
//    Jeremy Meredith, Wed Jun  8 17:05:56 PDT 2005
//    Added a symbol dictionary.
//
// ****************************************************************************
Grammar::Grammar(Dictionary &d)
    : dictionary(d),
      eof(dictionary, EOF_TOKEN_ID),  // TT_EOF==256 for all grammars
      start(dictionary, "START")
{
    out = NULL;
    rules.push_back(NULL);
}

// ****************************************************************************
//  Destructor:  Grammar::Grammar
//
//  Programmer:  Jeremy Meredith
//  Creation:    April  5, 2002
//
//  Modifications:
//    Jeremy Meredith, Mon Jun 13 15:52:32 PDT 2005
//    Free the rules.
//
// ****************************************************************************
Grammar::~Grammar()
{
    for (size_t i=1; i<rules.size(); i++)
        delete rules[i];
}

// ****************************************************************************
//  Method:  Grammar::SetPrinter
//
//  Purpose:
//    Sets the ostream where we will print debug output.
//
//  Programmer:  Jeremy Meredith
//  Creation:    April  5, 2002
//
// ****************************************************************************
void
Grammar::SetPrinter(ostream *o)
{
    out = o;
}

// ****************************************************************************
//  Method:  Grammar::Print
//
//  Purpose:
//    Print the rules and states.
//
//  Programmer:  Jeremy Meredith
//  Creation:    April  5, 2002
//
//  Modifications:
//    Jeremy Meredith, Wed Jul 23 13:15:57 EDT 2008
//    Add the rule number when printing for easier cross-reference.
//
// ****************************************************************************
void
Grammar::Print(ostream &o)
{
    o << endl << TermBold << TermBrown << "- - - ------------- rules ------------- - - -\n" << TermReset;
    for (size_t i=0; i<rules.size(); i++)
        o << i <<":" << *(rules[i]) << endl;
    o << endl << TermBold << TermBrown << "- - - ------------- states ------------- - - -\n" << TermReset;
    for (size_t i=0; i<sets.size(); i++)
    {
        if (sets[i].HasConflict())   o << TermRed;
        else                         o << TermBold << TermBlue;
        o << TermUnderline << "<< State #"<<i<<" >>" << TermReset << endl << sets[i] << endl;
    }
}

// ****************************************************************************
//  Method:  Grammar::SetAssoc
//
//  Purpose:
//    Sets the associativity of a symbol.
//
//  Programmer:  Jeremy Meredith
//  Creation:    April  5, 2002
//
// ****************************************************************************
void
Grammar::SetAssoc(const Symbol &s, Associativity a)
{
    assoc[&s] = a;
}

// ****************************************************************************
//  Method:  Grammar::SetPrec
//
//  Purpose:
//    Sets the precedence of a symbol.  ( higher number = higher precedence )
//
//  Programmer:  Jeremy Meredith
//  Creation:    April  5, 2002
//
// ****************************************************************************
void
Grammar::SetPrec(const Symbol &s,  int p)
{
    prec[&s] = p;
}

// ****************************************************************************
//  Method:  Grammar::AddRule
//
//  Purpose:
//    Add a rule to the list, possibly assigning a precision.
//
//  Programmer:  Jeremy Meredith
//  Creation:    April  5, 2002
//
// ****************************************************************************
void
Grammar::AddRule(const Rule &r_, int prec)
{
    Rule *r = new Rule(r_);
    r->SetIndex((int)rules.size());
    if (prec != -1)
        r->SetPrec(prec);
    rules.push_back(r);
}

// ****************************************************************************
//  Method:  Grammar::SetStartSymbol
//
//  Purpose:
//    Sets the start symbol.
//
//  Programmer:  Jeremy Meredith
//  Creation:    April  5, 2002
//
// ****************************************************************************
void
Grammar::SetStartSymbol(const Symbol &ss)
{
    startrule = Rule(0, start) >> ss;
    startrule.SetIndex(0);
    rules[0] = &startrule;
}

// ****************************************************************************
//  Method:  Grammar::GetStartSymbol
//
//  Purpose:
//    Gets the start symbol.
//
//  Programmer:  Jeremy Meredith
//  Creation:    April  5, 2002
//
// ****************************************************************************
const Symbol*
Grammar::GetStartSymbol()
{
    return &start;
}

// ****************************************************************************
//  Method:  Grammar::GetState
//
//  Purpose:
//    Returns the "i"th state.
//
//  Programmer:  Jeremy Meredith
//  Creation:    April  5, 2002
//
// ****************************************************************************
State&
Grammar::GetState(int i)
{
    return states[i];
}

// ****************************************************************************
//  Method:  Grammar::GetRule
//
//  Purpose:
//    Returns the "i"th rule.
//
//  Programmer:  Jeremy Meredith
//  Creation:    April  5, 2002
//
// ****************************************************************************
const Rule*
Grammar::GetRule(int i)
{
    return rules[i];
}

// ****************************************************************************
//  Method:  Grammar::GetDictionary
//
//  Purpose:
//    Returns the symbol dictionary.
//
//  Programmer:  Jeremy Meredith
//  Creation:    June  8, 2005
//
// ****************************************************************************
Dictionary&
Grammar::GetDictionary()
{
    return dictionary;
}

// ****************************************************************************
//  Method:  Grammar::Configure
//
//  Purpose:
//    Makes all configurating sets/states to be used by the parser.
//    Resolves conflicts using precedence and associativity.
//
//  Programmer:  Jeremy Meredith
//  Creation:    April  5, 2002
//
//  Modifications:
//    Jeremy Meredith, Wed Jul 23 13:23:14 EDT 2008
//    Cleaned up the logic in the shift-reduce conflict resolution.
//    Using the last terminal in the reduce rule to find precedence
//    is a nice convenience, and it makes sense, but it was coded such
//    that there was an assumption that there *was* a terminal in
//    the reduce rule.  In fact, it's fine not to have one, it just
//    means we can only use a precedence attached to the rule itself.
//    Also, by cleaning it up, I was able to add extra info to the
//    error messages.
//
// ****************************************************************************
bool
Grammar::Configure()
{
    bool okay = true;

    ConfiguratingSet start;
    start.AddItem(ConfiguratingItem(rules[0], eof));
    start.Close(rules);
    sets.push_back(start);

    // For every state in the grammar
    for (size_t j=0; j<sets.size(); j++)
    {
        // Add new states to the grammar resulting from shifting a symbol
        // in the current state
        SymbolSet shift = sets[j].GetShiftSymbols();
        for (size_t i=0; i<shift.size(); i++)
        {
            const Symbol *ssym = shift.set[i];
            if (!ssym)
                continue;

            ConfiguratingSet cs = sets[j].GetShiftSet(ssym);
            cs.Close(rules);

            size_t match = sets.size();
            for (size_t k=0; k<sets.size(); k++)
            {
                if (cs == sets[k])
                {
                    match = k;
                    break;
                }
            }
            if (match == sets.size())
            {
                sets.push_back(cs);
            }
            sets[j].SetShiftTransition(ssym, (int)match);
        }

        // Add the reduce rules for the current state, looking for 
        // and reduce-reduce conflicts
        SymbolSet reduce = sets[j].GetReduceSymbols();
        for (size_t i=0; i<reduce.size(); i++)
        {
            const Symbol *rsym = reduce.set[i];
            if (!rsym)
                continue;

            vector<int> reducerules = sets[j].GetReduceRules(rsym);
            if (reducerules.size() < 1) (*out) << TermRed << "Internal error! 1\n";
            if (reducerules.size() > 1)
            {
                // We could probably resolve these using rule precedence, but
                // I never bothered since it never came up in our grammar
                (*out) << TermRed << "Reduce-reduce conflict in state " << j
                     << " for symbol " << *rsym << ":" << endl;
                for (size_t k=0; k<reducerules.size(); k++)
                {
                    (*out) << TermRed << "\tRule "<<reducerules[k]<<": " << TermReset;
                    rules[reducerules[k]]->Print((*out));
                    (*out) << endl;
                }
                (*out) << TermReset;
                sets[j].SetConflict(true);
                okay = false;
            }
            for (size_t k=0; k<reducerules.size(); k++)
                sets[j].SetReduceRule(rsym, reducerules[k]);
        }

        // Look for any shift-reduce conflicts
        for (size_t i=0; i<shift.size(); i++)
        {
            const Symbol *ssym = shift.set[i];
            if (!ssym || !reduce.set[i])
                continue;

            // We know there's only one reduce rule
            const Rule *reducerule = rules[sets[j].GetReduceRules(ssym)[0]];

            int sprec = prec.count(ssym) ? prec[ssym] : -1;
            int rprec = reducerule->GetPrec();
            bool haveprec  = (sprec >= 0 && rprec >= 0);

            // Check precedence against reduce rule first
            if (haveprec && sprec!=rprec)
            {
                if (sprec < rprec)
                {
                    sets[j].RemoveShiftTransition(ssym);
                    (*out) << TermGreen << "SR conflict in state " << j << " for " << *ssym << " and rule "<<reducerule->GetIndex()<<" resolved by higher precedence of "<< *ssym << TermReset << endl;
                }
                else
                {
                    sets[j].RemoveReduceRule(ssym);
                    (*out) << TermGreen << "SR conflict in state " << j << " for " << *ssym << " and rule "<<reducerule->GetIndex()<<" resolved by higher precedence of rule " << TermReset << endl;
                }
                continue;
            }

            // Okay, no dice.  Try using the last terminal in the rule
            const Symbol *rsym = reducerule->GetRHS().GetLastTerminal();
            rprec = prec.count(rsym) ? prec[rsym] : -1;
            haveprec  = (sprec >= 0 && rprec >= 0);

            // Try associativity first.
            bool same = (ssym == rsym);
            if (assoc.count(ssym) && (same || (haveprec && sprec==rprec)))
            {
                if (assoc[ssym] != Right)
                    sets[j].RemoveShiftTransition(ssym);
                if (assoc[ssym] != Left)
                    sets[j].RemoveReduceRule(ssym);
                (*out) << TermGreen << "SR conflict in state " << j  << " for " << *ssym << " and rule " << reducerule->GetIndex() << " resolved by " << (assoc[ssym] == Left ? "left" : "right") << " assoc "<< TermReset << endl;
                continue;
            }

            // Nope, so now try using it for precedence.
            if (haveprec && sprec!=rprec)
            {
                if (sprec < rprec)
                    sets[j].RemoveShiftTransition(ssym);
                else
                    sets[j].RemoveReduceRule(ssym);
                (*out) << TermGreen << "SR conflict in state " << j << " for " << *ssym << " and "<<*rsym<<" resolved by higher precedence of "<<(sprec < rprec ? *ssym : *rsym) << TermReset << endl;
                continue;
            }

            // failed to resolve... set the error flag
            sets[j].SetConflict(true);
            okay = false;
            (*out) << TermRed << "SR conflict in state " << j << " for " << *ssym << " and rule " << reducerule->GetIndex() << TermReset << endl;
        }
    }

    // Print all rules and states if needed
    if (out)
        Print(*out);

    // Create the states directly from the configurating sets
    if (okay)
    {
        for (size_t i=0; i<sets.size(); i++)
            states.push_back(State(sets[i]));
    }

    return okay;
}

// ****************************************************************************
//  Method:  Grammar::WriteStateInitialization
//
//  Purpose:
//    Write out C++ code to initialize the grammar states without
//    having to go through the slower configuration.  This allows
//    the yacc/bison mode of operation.
//
//  Arguments:
//    name       Grammar derived class name
//    o          output file derived class name
//
//  Programmer:  Jeremy Meredith
//  Creation:    April  5, 2002
//
//  Modifications:
//    Jeremy Meredith, Thu Apr 11 12:56:46 PDT 2002
//    Cleaned up output -- made it hundreds of 10-line functions instead of
//    one function with thousands of lines.  Also added some comments.
//
//    Brad Whitlock, Fri Jun 28 15:12:49 PST 2002
//    Made it work on Windows.
//
//    Jeremy Meredith, Wed Jun  8 17:06:40 PDT 2005
//    Added a symbol dictionary to remove static data.
//
//    Jeremy Meredith, Tue Jun  5 13:29:33 EDT 2007
//    Added copyright notice.  Added parentheses to function definition.
//
//    Jeremy Meredith, Wed Jul 23 13:25:32 EDT 2008
//    Since we now have backslash as a symbol, escape it properly
//    (with an extra backslash).
//
// ****************************************************************************

void
Grammar::WriteStateInitialization(const string &name, ostream &o)
{
    o << "// Copyright (c) Lawrence Livermore National Security, LLC and other VisIt" << endl;
    o << "// Project developers.  See the top-level LICENSE file for dates and other" << endl;
    o << "// details.  No copyright assignment is required to contribute to VisIt." << endl;
    o << "" << endl;

    o << "//" << endl;
    o << "//   Automatically generated!!!  Use 'make init' to rebuild." << endl;
    o << "//" << endl;
    o << endl;
    o << "#include \""<<name.c_str()<<".h\"" << endl;
    o << endl;
    o << "" << endl;
    o << "static void SetShiftState(State &s, int next, const Symbol *sym)" << endl;
    o << "{" << endl;
    o << "    s.shift[sym] = next;" << endl;
    o << "}" << endl;
    o << "" << endl;
    o << "static void SetReduceRule(State &s, int rule, const Symbol *sym)" << endl;
    o << "{" << endl;
    o << "    s.reduce[sym] = rule;" << endl;
    o << "}" << endl;
    o << "" << endl;

    for (size_t i=0; i<states.size(); i++)
    {
        o << "static void InitState_"<<i<<"(Dictionary &d, State &s)" << endl;
        o << "{" << endl;

        // Write out the shift state symbol map
        if (!states[i].shift.empty())
            o << "    // shift transitions" << endl;
        std::map<const Symbol*,int>::iterator j;
        for (j = states[i].shift.begin();
             j != states[i].shift.end();
             j++)
        {
            const Symbol *s = j->first;
            char str[256];
            sprintf(str, "%4d", j->second);

            int tt = s->GetTerminalType();

            o << "    SetShiftState(s,"<<str<<", d.Get(";
            if (s->IsNonTerminal())
                o << "\"" << s->GetDisplayString().c_str() << "\"";
            else
                if (tt == '\\')
                    o << "'\\\\'";
                else if (tt < 256)
                    o << "'" << char(tt) << "'";
                else
                    o << tt;
            o << "));" << endl;
        }

        if (!states[i].shift.empty() && !states[i].reduce.empty())
            o << endl;

        // Write out the reduce rule symbol map
        set<int> reductions;
        for (j = states[i].reduce.begin();
             j != states[i].reduce.end();
             j++)
        {
            reductions.insert(j->second);
        }
        std::set<int>::iterator k;
        for (k = reductions.begin();
             k != reductions.end();
             k++)
        {
            o << "    // reduce rule " << *k << ": ";
            GetRule(*k)->PrintNoColor(o);
            o << endl;
        }
        for (j = states[i].reduce.begin();
             j != states[i].reduce.end();
             j++)
        {
            const Symbol *s = j->first;
            char str[256];
            sprintf(str, "%4d", j->second);

            int tt = s->GetTerminalType();

            o << "    SetReduceRule(s,"<<str<<", d.Get(";
            if (s->IsNonTerminal())
                o << "\"" << s->GetDisplayString().c_str() << "\"";
            else
                if (tt == '\\')
                    o << "'\\\\'";
                else if (tt < 256)
                    o << "'" << char(tt) << "'";
                else
                    o << tt;
            o << "));" << endl;
        }

        o << "}" << endl;
        o << endl;
    }

    int nfuncper = 50;    
    int nfuncs = states.size() / nfuncper;
    if(states.size() % nfuncper > 0)
        ++nfuncs;
    size_t f = 0;
    std::vector<std::string> funcnames;
    for(int i = 0; i < nfuncs; ++i)
    {
        char tmp[10];
        sprintf(tmp, "%03d", i*nfuncper);
        std::string name = std::string("ExprGrammar_Init") + std::string(tmp);
        funcnames.push_back(name);
        o << "static void " << name << "(Dictionary &dictionary," << endl;
        o << "    std::vector<State> &states)" << endl;
        o << "{" << endl;
        for(int j = 0; j < nfuncper; ++j)
        {
            if(f < states.size())
                o << "    InitState_"<<f<<"(dictionary, states[" << f << "]);" << endl;
            ++f;
        }
        o << "}" << endl << endl;
    }
    o << "bool " << name.c_str() << "::Initialize()" << endl;
    o << "{" << endl;
    o << "    states.resize(" << states.size() << ");" << endl;
    o << endl;
    for (size_t i=0; i<funcnames.size(); i++)
    {
        o << "    " << funcnames[i] <<"(dictionary, states);" << endl;
    }
    o << endl;
    o << "    return true;" << endl;
    o << "}" << endl;
    o << endl;
}
