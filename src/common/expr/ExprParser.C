// Copyright (c) Lawrence Livermore National Security, LLC and other VisIt
// Project developers.  See the top-level LICENSE file for dates and other
// details.  No copyright assignment is required to contribute to VisIt.

#include <ExprParser.h>
#include <ExprNode.h>
#include <VisItToken.h>
#include <SymbolSet.h>
#include <Sequence.h>
#include <Rule.h>
#include <ConfiguratingItem.h>
#include <ConfiguratingSet.h>
#include <Colors.h>
#include <ExprGrammar.h>
#include <VisItInit.h>
#include <InvalidExpressionException.h>
#include <ParseException.h>
#include <string>
#include <vector>
using std::vector;
using std::string;

ExprParser::ErrorMessageTarget ExprParser::errorMessageTarget = EMT_EXCEPTION;

class DummyNode : public ExprParseTreeNode
{
  public:
    DummyNode(const Pos &p) : ExprParseTreeNode(p) { }
    virtual void PrintNode(ostream &o) { o << "DummyNode\n"; }
};

// ****************************************************************************
//  Constructor:  ExprParser::ExprParser
//
//  Programmer:  Jeremy Meredith
//  Creation:    April  5, 2002
//
// ****************************************************************************
ExprParser::ExprParser(ExprNodeFactory *f) : Parser(), factory(f)
{
    Grammar *G = new ExprGrammar;
    if (!G->Initialize())
    {
        cerr << "Error in initializion of Expression Grammar!\n";
        exit(-1); // HOOKS_IGNORE
    }

    SetGrammar(G);
}

// ****************************************************************************
//  Destructor:  ExprParser::~ExprParser
//
//  Programmer:  David Camp
//  Creation:    Tue Jan 11 11:09:24 PST 2011
//
// ****************************************************************************
ExprParser::~ExprParser()
{
    if (factory)
        delete factory;
}

// ****************************************************************************
//  Method:  ExprParser::ApplyRule
//
//  Purpose:
//    This is the implementation of a Grammar's pure virtual function when
//    applying reductions based on grammar rules.  We group them by 
//    the rule's LHS for readability, then simply key off the identifying
//    integer we set for the rule when creating it in ExprGrammar::ExprGrammar.
//
//  Arguments:
//    sym        the LHS of the rule
//    rule       the rule to apply
//    E          an array of ExprParseTreeNodes as long as the length
//               of the rule's RHS
//    T          an array of Tokens as long as the length of the rule's RHS
//               note that many of these may be NULL
//    p          the position encompassing the sequence in the expression
//
//  Note:
//    All ExprParseTreeNodes must be downcasted here.  One must be careful that they
//    create the right types of nodes for what they are expecting in later
//    reductions.
//
//  Programmer:  Jeremy Meredith
//  Creation:    April  5, 2002
//
//  Modifications:
//    Sean Ahern, Wed Oct 16 14:11:22 PDT 2002
//    Changed to use an ExprNodeFactory.
//
//    Jeremy Meredith, Mon Jul 28 16:03:49 PDT 2003
//    Added more info to the error message in the TimeSpec.
//
//    Jeremy Meredith, Tue Nov 23 14:51:51 PST 2004
//    Refactored.  Removed the dynamic cast.
//
//    Jeremy Meredith, Wed Nov 24 15:36:34 PST 2004
//    Removed Expr => List and added Arg => Expr.  Lists are not
//    derived from ExprNode in the chain, and this seems more correct
//    since no filter can yet support expressions anyway.  We can
//    change it back later if we ever support it.
//
//    Jeremy Meredith, Tue Dec 28 11:22:13 PST 2004
//    Added the original text for an argument to ArgExpr because it is
//    useful for implementing macros.
//
//    Hank Childs, Fri Jan 28 14:07:18 PST 2005
//    Use exception macros.
//
//    Jeremy Meredith, Mon Jun 13 15:46:22 PDT 2005
//    Split ConstExpr into multiple concrete base classes.
//    Made FunctionExpr, MachExpr, and ArgsExpr use names
//    instead of Identifier tokens.  These two changes were to
//    remove Token references from the parse tree node classes.
//
//    Hank Childs, Thu Sep  1 11:47:47 PDT 2005
//    Add support for specifying times with a delta using 'd'.
//
//    Jeremy Meredith, Fri Sep  2 16:57:48 PDT 2005
//    Improved error message to TimeSpec slightly.
//
//    Brad Whitlock, Fri Oct 6 11:49:02 PDT 2006
//    I made the symbols be static in the ExprGrammar class so there are no
//    conflicts with Python 2.5.
//
//    Mark C. Miller, Mon Apr 14 15:41:21 PDT 2008
//    Changed C-stype commented-out code block for case 14 to conditionally
//    compiled code block.
//
//    Jeremy Meredith, Wed Jul 23 11:20:17 EDT 2008
//    Allow backslashes in paths for better windows support.
//
//    Jeremy Meredith, Mon Dec 15 14:30:57 EST 2008
//    Allow unary minus to get integrated directly into float and int
//    constants.
//
//    Kathleen Biagas, Thu May  1 17:17:32 PDT 2014
//    Changed FloatConstExpr to double.
//
// ****************************************************************************

ParseTreeNode*
ExprParser::ApplyRule(const Symbol           &sym,
                      const Rule             *rule,
                      vector<ParseTreeNode*> &E,
                      vector<Token*>         &T,
                      Pos                     p)
{
    ParseTreeNode *node = NULL;
    if (sym == ExprGrammar::Expr)
    {
        switch (rule->GetID())
        {
        case 0:
            node = factory->CreateBinaryExpr(p,
                           ((Character*)T[1])->GetVal(),
                           (ExprNode*)(E[0]),
                           (ExprNode*)(E[2]));

            break;
        case 1:
            node = factory->CreateBinaryExpr(p,
                                          ((Character*)T[1])->GetVal(),
                                          (ExprNode*)(E[0]),
                                          (ExprNode*)(E[2]));

            break;
        case 2:
            node = factory->CreateBinaryExpr(p,
                                          ((Character*)T[1])->GetVal(),
                                          (ExprNode*)(E[0]),
                                          (ExprNode*)(E[2]));

            break;
        case 3:
            node = factory->CreateBinaryExpr(p,
                                             ((Character*)T[1])->GetVal(),
                                             (ExprNode*)(E[0]),
                                             (ExprNode*)(E[2]));
            break;
        case 4:
            node = factory->CreateBinaryExpr(p,
                                             ((Character*)T[1])->GetVal(),
                                             (ExprNode*)(E[0]),
                                             (ExprNode*)(E[2]));

            break;
        case 5:
            node = factory->CreateBinaryExpr(p,
                                             ((Character*)T[1])->GetVal(),
                                             (ExprNode*)(E[0]),
                                             (ExprNode*)(E[2]));

            break;
        case 6:
            // We know that the only token that can come from this
            // expansion is an int, so do the cast.
            node = factory->CreateIndexExpr(p,
                                            (ExprNode*)(E[0]),
                                            ((IntegerConst*)T[2])->GetValue());
            break;
        case 7:
            // We can be a little smart and condense "-int" or "-float"
            // into a single constant here.
            if (((Character*)T[0])->GetVal() == '-' &&
                E[1]->GetTypeName() == "IntegerConst")
            {
                ConstExpr *c = dynamic_cast<ConstExpr*>(E[1]);
                int v = dynamic_cast<IntegerConstExpr*>(c)->GetValue();
                node = factory->CreateIntegerConstExpr(p,-v);
            }
            else if (((Character*)T[0])->GetVal() == '-' &&
                     E[1]->GetTypeName() == "FloatConst")
            {
                ConstExpr *c = dynamic_cast<ConstExpr*>(E[1]);
                double v = dynamic_cast<FloatConstExpr*>(c)->GetValue();
                node = factory->CreateFloatConstExpr(p,-v);
            }
            else
            {
                node = factory->CreateUnaryExpr(p,
                                                ((Character*)T[0])->GetVal(),
                                                (ExprNode*)(E[1]));
            }

            break;
        case 8:
            node = E[1];
            break;
        case 9:
            node = E[0];
            break;
        case 10:
            node = E[0];
            break;
        case 11:
            node = E[0];
            break;
        case 12:
            node = E[0];
            break;
        case 13:
            node = E[0];
            break;
#if 0
           // The next rule (for Expr => List) is compiled out
           // because we cannot implement it right now.  If we
           // compile-in this expansion, we should remove Arg => List
           // because we could simply Arg => Expr => List.  This
           // will probably fail with a RR conflict anyway....
        case 14:
            node = E[0];
            break;
#endif
        case 15:
            node = factory->CreateBinaryExpr(p,
                                             ((Character*)T[1])->GetVal(),
                                             (ExprNode*)(E[0]),
                                             (ExprNode*)(E[2]));
            break;
        }
    } else if (sym == ExprGrammar::Constant)
    {
        switch (rule->GetID())
        {
        case 0:
            node = factory->CreateIntegerConstExpr(p,
                                         ((IntegerConst *)T[0])->GetValue());
            break;
        case 1:
            node = factory->CreateFloatConstExpr(p,
                                         ((FloatConst *)T[0])->GetValue());
            break;
        case 2:
            node = factory->CreateStringConstExpr(p,
                                         ((StringConst *)T[0])->GetValue());
            break;
        case 3:
            node = factory->CreateBooleanConstExpr(p,
                                         ((BoolConst *)T[0])->GetValue());
            break;
        }
    } else if (sym == ExprGrammar::Vector)
    {
        switch (rule->GetID())
        {
        case 0:
            node = factory->CreateVectorExpr(p,
                                             (ExprNode*)(E[1]),
                                             (ExprNode*)(E[3]));
            break;
        case 1:
            node = factory->CreateVectorExpr(p,
                                             (ExprNode*)(E[1]),
                                             (ExprNode*)(E[3]),
                                             (ExprNode*)(E[5]));
            break;
        }
    } else if (sym == ExprGrammar::List)
    {
        switch (rule->GetID())
        {
        case 0:
            node = E[1];
            break;
        }
    } else if (sym == ExprGrammar::ListElems)
    {
        switch (rule->GetID())
        {
        case 0:
            ((ListExpr *) (node = E[0]))->AddListElem((ListElemExpr*)(E[2]));
            break;
        case 1:
            node = new ListExpr(p, (ListElemExpr*)(E[0]));
            break;
        }
    } else if (sym == ExprGrammar::ListElem)
    {
        switch (rule->GetID())
        {
        case 0:
            node = new ListElemExpr(p, (ExprNode*)(E[0]));
            break;
        case 1:
            node = new ListElemExpr(p, (ExprNode*)(E[0]),
                                    (ExprNode*)(E[2]));
            break;
        case 2:
            node = new ListElemExpr(p, (ExprNode*)(E[0]),
                                    (ExprNode*)(E[2]),
                                    (ExprNode*)(E[4]));
            break;
        }
    } else if (sym == ExprGrammar::Function)
    {
        switch (rule->GetID())
        {
        case 0:
            node = factory->CreateFunctionExpr(p,
                                               ((Identifier*)T[0])->GetVal());
            break;
        case 1:
            node = factory->CreateFunctionExpr(p,
                                               ((Identifier*)T[0])->GetVal(),
                                               (ArgsExpr*)(E[2]));
            break;
        }
    } else if (sym == ExprGrammar::Args)
    {
        switch (rule->GetID())
        {
        case 0:
            ((ArgsExpr *) (node = E[0]))->AddArg((ArgExpr*)(E[2]));
            break;
        case 1:
            node = new ArgsExpr(p, (ArgExpr*)(E[0]));
            break;
        }
    } else if (sym == ExprGrammar::Arg)
    {
        switch (rule->GetID())
        {
        case 0:
            node = new ArgExpr(p, (ExprParseTreeNode*)E[0], p.GetText(text));
            break;
        case 1:
            node = new ArgExpr(p, ((Identifier*)T[0])->GetVal(),
                               (ExprParseTreeNode*)E[2],
                               p.GetText(text));
            break;
        case 2:
            node = new ArgExpr(p, (ExprParseTreeNode*)E[0], p.GetText(text));
            break;
        }
    } else if (sym == ExprGrammar::PathSpec)
    {
        switch (rule->GetID())
        {
        case 0:
            node = E[0];
            ((PathExpr *) node)->Append("/");
            ((PathExpr *) node)->Append(((Identifier*)T[2])->GetVal());

            delete E[1];
            break;
        case 1:
            node = new PathExpr(p, "/");
            ((PathExpr *) node)->Append(((Identifier*)T[1])->GetVal());
            delete E[0];
            break;
        case 2:
            node = new PathExpr(p, ((Identifier*)T[0])->GetVal());
            break;
        }
    } else if (sym == ExprGrammar::MultiSlash)
    {
        switch (rule->GetID())
        {
        case 0:
            node = new DummyNode(p);
            break;
        case 1:
            node = new DummyNode(p);
            break;
        case 2:
            node = new DummyNode(p);
            break;
        case 3:
            node = new DummyNode(p);
            break;
        }
    } else if (sym == ExprGrammar::Variable)
    {
        switch (rule->GetID())
        {
        case 0:
            node = factory->CreateVarExpr(p, NULL, new PathExpr(p,
                                          ((Identifier*)T[0])->GetVal()),
                                          true);
            break;
        case 1:
            node = factory->CreateVarExpr(p, NULL,
                                          (PathExpr*)(E[1]),
                                          false);
            break;
        case 2:
            node = factory->CreateVarExpr(p, (DBExpr*)(E[1]),
                                          (PathExpr*)(E[3]),
                                          false);
            break;
        }
    } else if (sym == ExprGrammar::Database)
    {
        switch (rule->GetID())
        {
        case 0:
            node = E[1];
            break;
        }
    } else if (sym == ExprGrammar::DBSpec)
    {
        switch (rule->GetID())
        {
        case 0:
            node = new DBExpr(p, (PathExpr*)(E[0]), NULL, NULL);
            break;
        case 1:
            node = new DBExpr(p, (PathExpr*)(E[0]),
                              (MachExpr*)(E[1]), NULL);
            break;
        case 2:
            node = new DBExpr(p, NULL, NULL, (TimeExpr*)(E[0]));
            break;
        case 3:
            node = new DBExpr(p, (PathExpr*)(E[0]), NULL,
                              (TimeExpr*)(E[1]));
            break;
        case 4:
            node =
                new DBExpr(p, (PathExpr*)(E[0]),
                           (MachExpr*)(E[1]),
                           (TimeExpr*)(E[2]));
            break;
        }
    } else if (sym == ExprGrammar::MachSpec)
    {
        switch (rule->GetID())
        {
        case 0:
            node = new MachExpr(p, ((Identifier*)T[1])->GetVal());
            break;
        }
    } else if (sym == ExprGrammar::TimeSpec)
    {
        switch (rule->GetID())
        {
        case 0:
            {
                string id = ((Identifier*)T[3])->GetVal();
                bool isDelta = false;
                if (id.length() == 2)
                {
                    if (id[0] == 'd' || id[0] == 'D')
                        id = id.substr(1, 1);
                    else if (id[1] == 'd' || id[1] == 'D')
                        id = id.substr(0, 1);
                    else
                        EXCEPTION2(SyntacticException, T[3]->GetPos(),
                                   "time format needs to be "
                                   "'i', 'c', or 't', with an optional 'd'");
                    isDelta = true;
                }
               
                if (id.length() != 1)
                    EXCEPTION2(SyntacticException, T[3]->GetPos(),
                               "time format needs to be "
                               "'i', 'c', or 't', with an optional 'd'");
                char    c = id[0];
                TimeExpr::Type t;
                if (c == 'c' || c == 'C')
                    t = TimeExpr::Cycle;
                else if (c == 't' || c == 'T')
                    t = TimeExpr::Time;
                else if (c == 'i' || c == 'I')
                    t = TimeExpr::Index;
                else
                    EXCEPTION2(SyntacticException, T[3]->GetPos(),
                               "time format needs to be "
                               "'i', 'c', or 't', with an optional 'd'");
                TimeExpr *t1 = new TimeExpr(p, (ListExpr*)(E[1]), t);
                t1->SetIsDelta(isDelta);
                node = t1;

                break;
            }
        case 1:
            node = new TimeExpr(p, (ListExpr*)(E[1]));
            break;
        case 2:
            node = new TimeExpr(p, (ListExpr*)(E[2]),
                                TimeExpr::Index);
            break;
        }
    } else if (sym == *G->GetStartSymbol())
    {
        node = E[0];
    }
    return node;
}

// ****************************************************************************
//  Method:  ExprParser::Parse
//
//  Purpose:
//    Main external routine for clients.
//    Re-initialize the parser and scanner, and process one at a time until
//    we get an error or acceptance.  Return the parse tree when finished.
//
//  Programmer:  Jeremy Meredith
//  Creation:    April  5, 2002
//
//  Modifications:
//    Jeremy Meredith, Mon Jul 28 16:13:35 PDT 2003
//    Made it report normal parse errors through the viewer window
//    mechanism if needed.  Unhandled reductions will only occur during
//    debugging and can still go to cerr.
//
//    Hank Childs, Fri Aug  8 08:13:21 PDT 2003
//    Have error messages be issued in a way that it is independent of 
//    component.
//
//    Jeremy Meredith, Fri Aug 15 12:49:01 PDT 2003
//    Added the EMT_EXCEPTION type, and renamed EMT_VIEWER to EMT_COMPONENT.
//
//    Jeremy Meredith, Wed Nov 24 11:51:59 PST 2004
//    Refactored.  There's a new base class for the ExprParser and the
//    return types became more general.
//
//    Jeremy Meredith, Tue Dec 28 11:18:37 PST 2004
//    Made the current text a data member to help with macro support.
//
//    Hank Childs, Tue Dec 28 16:14:47 PST 2004
//    Rename GetText and PrintText to GetErrorText and PrintErrorText.
//
//    Hank Childs, Fri Jan 28 14:07:18 PST 2005
//    Use exception macros.
//
//    Jeremy Meredith, Mon Jun 13 16:17:14 PDT 2005
//    Delete the tokens that have not taken part in a rule reduction -- in 
//    this case that means Space tokens and the final EOF token.
//
//    Mark C. Miller, Wed Mar 17 10:03:09 PDT 2010
//    Pass buffer length estimate to GetErrorText to handle truncation of
//    error messages too long to fit into buffer.
// ****************************************************************************
ParseTreeNode*
ExprParser::Parse(const std::string &s)
{
    text = s;

    // Change weird spacing (tabs, newlines) to normal ones.
    // This will make printing error messages later much easier.
    for (size_t i=0; i<text.length(); i++)
        if (text[i] == '\t' || text[i] == '\n') text[i] = ' ';

    TRY
    {
        Init();
        scanner.SetInput(text);
        Token *token = NULL;
        while (!Accept()) 
        {
            token = scanner.ScanOneToken();
            if (token->GetType() != TT_Space)
                ParseOneToken(token);
            else
                delete token;
        }
        delete token;
    }
    CATCH2(UnhandledReductionException, e)
    {
        // This should only occur during debugging; print to cerr anyway
        cerr << e.Message() << endl;
        cerr << "Rule = " << *(e.GetRule()) << endl;
        e.GetPos().PrintErrorText(cerr, text);
        CATCH_RETURN2(1, NULL);
    }
    CATCH2(ParseException, e)
    {
        char error[1024];
        size_t n = sizeof(error) - strlen(e.Message()) - 2;
        snprintf(error, sizeof(error), "%s\n%s",
                 e.Message(), e.GetPos().GetErrorText(text,n).c_str());

        if (errorMessageTarget == EMT_COMPONENT)
        {
            VisItInit::ComponentIssueError(error);
        }
        else if (errorMessageTarget == EMT_CONSOLE)
        {
            cerr << error;
        }
        else if (errorMessageTarget == EMT_EXCEPTION)
        {
            EXCEPTION1(InvalidExpressionException, error);
        }

        CATCH_RETURN2(1, NULL);
    }
    ENDTRY

    return GetParseTree();
}
