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
