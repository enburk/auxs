#pragma once
#include "doc_text_model_a.h"
namespace doc::text::b
{
    struct model : a::model
    {
        using base = a::model;
        using base::base;

        int tab = 4;

        bool insert (str s)
        {
            if (selections.size() > 1 && s.contains("\n")) return false;

            if (s != "\t" && s != "shift+\t")
                return base::insert(s);

            array<replace> replaces;

            bool case2 = false;
            for (auto [from, upto] : selections)
                if (from.line != upto.line) case2 = true;

            if (not case2)
            {
                if (s == "\t")
                    for (auto [from, upto] : selections)
                        replaces += replace{range{from, from},
                            text(str(' ', tab - from.offset % tab))};
                else
                {
                    int nn = tab;

                    for (auto [from, upto] : selections)
                    {
                        auto [line, offset] = from;
                        if (line < 0 || line >= lines.size())
                        { nn = 0; break; }

                        int n = offset % tab;
                        if (n == 0) n = tab;
                        for (int i=0; i<n; i++)
                        {
                            if (offset-i <= 0) { n = i; break; }
                            if (lines[line].size() > offset-i-1
                            &&  lines[line][offset-i-1] != " ")
                            { n = i; break; }
                        }
                        nn = min(nn, n);
                    }

                    if (nn == 0) return false;

                    for (auto [from, upto] : selections) {
                        auto new_from = from; new_from.offset -= nn;
                        replaces += replace{range{new_from, from}, text{}};
                    }
                }

                return perform(replaces);
            }
            else
            {
                reundo undo {.selections=selections};

                for (auto & [from, upto] : selections)
                {
                    if (from.line < 0 || from.line >= lines.size()) continue;
                    if (upto.line < 0 || upto.line >= lines.size()) continue;

                    int nn = tab;

                    int upto_line = upto.offset == 0 ?
                        upto.line-1 :
                        upto.line;

                    for (int line = from.line; line <= upto_line; line++)
                        for (int offset = 0; offset < nn; offset++)
                            if (lines[line][offset] != " ") {
                                nn = offset; break; }

                    if (s == "\t") { nn = tab - nn; if (nn == 0) nn = tab; }

                    if (nn == 0) continue;

                    for (int line = from.line; line <= upto_line; line++)
                    {
                        auto o = place{line, 0};
                        auto e = o; e.offset += nn;
                        undo.replaces += s == "\t" ?
                        replace{range{o, e}, text{}}:
                        replace{range{o, o}, text{str(' ',nn)}};

                        for (int n=0; n<nn; n++) if (s == "\t")
                            lines[line].insert(0, " "); else
                            lines[line].upto(1).erase();
                    }

                    if (s != "\t") nn = -nn;
                    if (from.offset > 0) from.offset += nn;
                    if (upto.offset > 0) upto.offset += nn;
                }

                if (undo.replaces.size() == 0) return false;
                undoes += undo;
                redoes.clear();
                return true;
            }
        }
    };
}
#include "aux_unittest.h"
namespace aux::unittest
{
    void out (doc::text::b::model const& m)
    {
        for (auto & line : m.lines) out(doc::text::string(line));
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
