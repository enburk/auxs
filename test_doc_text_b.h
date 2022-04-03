#pragma once
#include "doc_text_model_b.h"
#include "aux_unittest.h"
namespace aux::unittest
{
    void out (doc::text::b::model const& m)
    {
        for (auto & line : m.lines) out(aux::unicode::string(line));
    }

    void text_model_b () try
    {
        using doc::text::b::model;
        using doc::range;
        using doc::place;

        test("text.model.tab.case1.a");
        {
            model m;
            m.tab = 2;
            m.insert("abc"); m.insert("\n");
            m.insert(" abc"); m.insert("\n");
            m.insert("  abc");
            m.selections.resize(3);
            m.selections[0] = range{place{0,1}, place{0,1}};
            m.selections[1] = range{place{1,1}, place{1,1}};
            m.selections[2] = range{place{2,1}, place{2,1}};

            oops( m.insert("\t"); out(m) ) {
                "a bc",
                "  abc",
                "   abc"};
            oops( m.insert("\t"); out(m) ) {
                "a   bc",
                "    abc",
                "     abc"};
            oops( m.insert("shift+\t"); out(m) ) {
                "a bc",
                "  abc",
                "   abc"};
            oops( m.insert("shift+\t"); out(m) ) {
                "abc",
                " abc",
                "  abc"};
            oops( m.insert("shift+\t"); out(m) ) {
                "abc",
                " abc",
                "  abc"};
            oops( out(m.selections) ) {
                "(0,1)-(0,1)",
                "(1,1)-(1,1)",
                "(2,1)-(2,1)"};
            oops( m.undo(); out(m) ) {
                "a bc",
                "  abc",
                "   abc"};
            oops( m.undo(); out(m) ) {
                "a   bc",
                "    abc",
                "     abc"};
            oops( m.undo(); out(m) ) {
                "a bc",
                "  abc",
                "   abc"};
            oops( m.undo(); out(m) ) {
                "abc",
                " abc",
                "  abc"};
            oops( out(m.selections) ) {
                "(0,1)-(0,1)",
                "(1,1)-(1,1)",
                "(2,1)-(2,1)"};
        }

        test("text.model.tab.case1.b");
        {
            model m;
            m.tab = 2;
            m.insert("abc"); m.insert("\n");
            m.insert(" abc"); m.insert("\n");
            m.insert("  abc");
            m.selections.resize(3);
            m.selections[0] = range{place{0,1}, place{0,2}};
            m.selections[1] = range{place{1,1}, place{1,2}};
            m.selections[2] = range{place{2,1}, place{2,2}};

            oops( m.insert("\t"); out(m) ) {
                "a bc",
                "  abc",
                "   abc"};
            oops( out(m.selections) ) {
                "(0,2)-(0,2)",
                "(1,2)-(1,2)",
                "(2,2)-(2,2)"};
            oops( m.insert("\t"); out(m) ) {
                "a   bc",
                "    abc",
                "     abc"};
            oops( m.insert("shift+\t"); out(m) ) {
                "a bc",
                "  abc",
                "   abc"};
            oops( m.insert("shift+\t"); out(m) ) {
                "abc",
                " abc",
                "  abc"};
            oops( out(m.selections) ) {
                "(0,1)-(0,1)",
                "(1,1)-(1,1)",
                "(2,1)-(2,1)"};
        }

        test("text.model.tab.case2.a");
        {
            model m;
            m.tab = 2;
            m.insert(" oss"); m.insert("\n");
            m.insert("  ss"); m.insert("\n");
            m.insert(" sso");
            m.selections.resize(1);
            m.selections[0] = range{place{0,2}, place{2,3}};

            oops( m.insert("\t"); out(m) ) {
                "  oss",
                "   ss",
                "  sso"};
            oops( m.insert("shift+\t"); out(m) ) {
                "oss",
                " ss",
                "sso"};
            oops( m.insert("shift+\t"); out(m) ) {
                "oss",
                " ss",
                "sso"};
            oops( out(m.selections) ) { "(0,1)-(2,2)" };
            oops( m.undo(); out(m) ) {
                "  oss",
                "   ss",
                "  sso"};
            oops( m.undo(); out(m) ) {
                " oss",
                "  ss",
                " sso"};
            oops( m.redo(); out(m) ) {
                "  oss",
                "   ss",
                "  sso"};
            oops( m.redo(); out(m) ) {
                "oss",
                " ss",
                "sso"};
            oops( out(m.selections) ) { "(0,1)-(2,2)" };
        }

        test("text.model.tab.case2.b");
        {
            model m;
            m.tab = 2;
            m.insert(" oss"); m.insert("\n");
            m.insert("  ss"); m.insert("\n");
            m.insert(" sso");
            m.selections.resize(1);
            m.selections[0] = range{place{0,2}, place{2,3}};

            oops( m.insert("shift+\t"); out(m) ) {
                "oss",
                " ss",
                "sso"};
            oops( m.insert("\t"); out(m) ) {
                "  oss",
                "   ss",
                "  sso"};
            oops( out(m.selections) ) { "(0,3)-(2,4)" };
            oops( m.undo(); out(m) ) {
                "oss",
                " ss",
                "sso"};
            oops( m.undo(); out(m) ) {
                " oss",
                "  ss",
                " sso"};
            oops( out(m.selections) ) { "(0,2)-(2,3)" };
        }

        test("text.model.tab.case2.c");
        {
            model m;
            m.tab = 2;
            m.insert(" abc"); m.insert("\n");
            m.insert("  de"); m.insert("\n");
            m.insert(" fgh");
            m.selections.resize(1);
            m.selections[0] = range{place{0,0}, place{2,0}};

            oops( m.insert("shift+\t"); out(m) ) {
                "abc",
                " de",
                " fgh"};
            oops( out(m.selections) ) { "(0,0)-(2,0)" };
            oops( m.insert("\t"); out(m) ) {
                "  abc",
                "   de",
                " fgh"};
            oops( out(m.selections) ) { "(0,0)-(2,0)" };
            oops( m.insert("shift+\t"); out(m) ) {
                "abc",
                " de",
                " fgh"};
            oops( out(m.selections) ) { "(0,0)-(2,0)" };
            oops( m.undo(); out(m) ) {
                "  abc",
                "   de",
                " fgh"};
            oops( out(m.selections) ) { "(0,0)-(2,0)" };
            oops( m.undo(); out(m) ) {
                "abc",
                " de",
                " fgh"};
            oops( out(m.selections) ) { "(0,0)-(2,0)" };
            oops( m.undo(); out(m) ) {
                " abc",
                "  de",
                " fgh"};
            oops( out(m.selections) ) { "(0,0)-(2,0)" };
        }
    }
    catch(assertion_failed){}
}
