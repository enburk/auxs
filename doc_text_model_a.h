#pragma once
#include "doc_text_text.h"
namespace doc::text::a
{
    struct model : text, doc::model
    {
        explicit model (text t) : text(t)
        {
            for (auto & line : lines) line.trimr(glyph(" "));
            if (lines.size() == 0) lines += array<glyph>{};
            selections = array<range>{range{}};
        }
        explicit model (str s = "") : model(text(s)) {}

        struct replace { range range; text text; };

        struct reundo
        {
            array<replace> replaces;
            array<range> selections;
        };
        array<reundo> undoes;
        array<reundo> redoes;

        place front () const { return place{}; }
        place back  () const { return place{
            lines.size()-1, lines.back().size()}; }

        bool perform (array<replace> ordered_replaces, int dir = 1 )
        {
            array<std::pair<replace, int>> replaces;
            replaces.reserve(ordered_replaces.size());
            for (int i=0; auto && r : ordered_replaces)
                replaces.emplace_back(r, i++);

            replaces.sort([](auto a, auto b){ return
                min(a.first.range.from, a.first.range.upto) >
                min(b.first.range.from, b.first.range.upto); });

            reundo undo;
            undo.selections = selections; selections.clear();
            array<std::pair<range, int>> ordered_selections;

            for (auto && [r, order] : replaces)
            {
                auto & [from, upto] = r.range;
                place last = back(); last.offset = max<int>();
                from = clamp(from, front(), last);
                upto = clamp(upto, front(), last);
                if (from > upto) std::swap(from, upto);
                if (from == upto && r.text == text{}) {
                    ordered_selections.emplace_back(range{from, upto}, order);
                    continue;
                }

                undo.replaces += replace{range{}, text{}};
                undo.replaces.back().text.lines.reserve(upto.line - from.line + 1);

                for (place p = from; p <= upto; p.offset = 0, p.line++) {
                    undo.replaces.back().text.lines += p.line == upto.line ?
                        lines[p.line].from(p.offset).upto(upto.offset) :
                        lines[p.line].from(p.offset);
                }

                if (from.line == upto.line)
                    lines[from.line].from(from.offset).upto(upto.offset).erase();
                else {
                    lines[from.line].from(from.offset).erase();
                    lines[upto.line].upto(upto.offset).erase();
                    lines[from.line] += lines[upto.line];
                    lines.from(from.line+1)
                         .upto(upto.line+1)
                         .erase();
                }

                upto = from;
                int n = from.offset - lines[from.line].size();
                if (n > 0) { from.offset -= n; while (n--) lines[from.line] += " "; }

                bool first_line = true;
                for (auto && line : r.text.lines)
                {
                    if (first_line) first_line = false; else
                    {
                        lines.insert(upto.line+1,
                        lines[upto.line].from(upto.offset));
                        lines[upto.line].from(upto.offset).erase();
                        upto.offset = 0;
                        upto.line++;
                    }
                    lines[upto.line].insert(upto.offset, line.from(0));
                    upto.offset += line.size();
                }

                undo.replaces.back().range = r.range;

                ordered_selections.emplace_back(range{upto, upto}, order);
            }

            ordered_selections.sort([](auto a, auto b){ return a.second < b.second; });
            for (auto selection : ordered_selections) selections += selection.first;

            if (undo.replaces.size() == 0) return false;

            if (dir == 1) undoes += undo;
            if (dir ==-1) redoes += undo;

            return true;
        }

        bool undo ()
        {
            if (undoes.empty()) return false;
            auto undo = undoes.back(); undoes.pop_back();
            perform(undo.replaces,-1);
            selections = undo.selections;
            return true;
        }
        bool redo ()
        {
            if (redoes.empty()) return false;
            auto redo = redoes.back(); redoes.pop_back();
            perform(redo.replaces, 1);
            selections = redo.selections;
            return true;
        }

        bool erase ()
        {
            redoes.clear();

            array<replace> replaces;
            
            for (auto [from, upto] : selections)
            {
                if (from == upto)
                {
                    auto & [line, offset] = upto;
                    if (offset < lines[line].size())
                        offset++; else
                    {
                        if (selections.size() == 1 &&
                            line < lines.size()-1) {
                            line++; offset = 0;
                        }
                    }
                }

                replaces += replace{range{from, upto}, text{}};
            }

            return perform(replaces);
        }

        bool backspace ()
        {
            redoes.clear();

            array<replace> replaces;
            
            for (auto [from, upto] : selections)
            {
                if (from == upto)
                {
                    auto & [line, offset] = upto;
                    if (offset > 0)
                        offset--; else
                    {
                        if (selections.size() == 1 &&
                            line > 0) {
                            line--; offset = lines[line].size();
                        }
                    }
                }

                replaces += replace{range{from, upto}, text{}};
            }

            return perform(replaces);
        }

        bool insert (str s)
        {
            redoes.clear();

            array<replace> replaces;

            for (range selection : selections)
            {
                replaces += replace{selection, text(s)};
            }

            return perform(replaces);
        }

        bool set (text text)
        {
            redoes.clear();

            if (*this == text) return false;

            if (*this == model{}) {
                *this =  model{text};
                return true;
            }

            selections.resize(1);
            selections[0].from = front();
            selections[0].upto = back();
            return insert(text.string());
        }
    };
}

#include "aux_unittest.h"
namespace aux::unittest
{
    void out (doc::text::a::model const& m)
    {
        for (auto & line : m.lines) out(doc::text::string(line));
    }
    void out (doc::range r)
    {
        out("("+std::to_string(r.from.line)+","+std::to_string(r.from.offset)+")-"
            "("+std::to_string(r.upto.line)+","+std::to_string(r.upto.offset)+")"
        );
    }
    void out (array<doc::range> ss)
    {
        for (auto s : ss) out(s);
    }

    void text_model_a () try
    {
        using doc::text::a::model;
        using doc::range;
        using doc::place;

        test("text.model.ctor");
        {
            oops( model m; out(m) ) { "" };
            oops( model m(" "); out(m) ) { "" };
            oops( model m("a"); out(m) ) { "a" };
            oops( model m("a "); out(m) ) { "a" };
            oops( model m(" a"); out(m) ) { " a" };
            oops( model m("\n"); out(m) ) { "", "" };
            oops( model m("\n "); out(m) ) { "", "" };
            oops( model m(" \n"); out(m) ) { "", "" };
            oops( model m(" \n "); out(m) ) { "", "" };
            oops( model m("a\nb"); out(m) ) { "a", "b" };
            oops( model m("a \nb"); out(m) ) { "a", "b" };
            oops( model m("a\n b"); out(m) ) { "a", " b" };
        }
        test("text.model.char");
        {
            model m;
            oops( m.insert(" "); out(m) ) { " " };
            oops( m.insert("a"); out(m) ) { " a" };
            oops( m.insert(" "); out(m) ) { " a " };
            oops( m.backspace(); out(m) ) { " a" };
            oops( m.erase    (); out(m) ) { " a" };
            oops( m.undo     (); out(m) ) { " a " };
            oops( m.undo     (); out(m) ) { " a" };
            oops( m.undo     (); out(m) ) { " " };
            oops( m.undo     (); out(m) ) { "" };
            oops( m.undo     (); out(m) ) { "" };
            oops( m.redo     (); out(m) ) { " " };
            oops( m.redo     (); out(m) ) { " a" };
            oops( m.redo     (); out(m) ) { " a " };
            oops( m.redo     (); out(m) ) { " a" };
            oops( m.redo     (); out(m) ) { " a" };
        }
        test("text.model.line");
        {
            model m;
            oops( m.insert("a"); out(m) ) { "a" };
            oops( m.insert("b"); out(m) ) { "ab" };
            oops( m.insert("c"); out(m) ) { "abc" };
            m.selections[0].from.offset--;
            m.selections[0].upto.offset--;
            oops( m.insert("d"); out(m) ) { "abdc" };
            oops( m.backspace(); out(m) ) { "abc" };
            oops( m.erase    (); out(m) ) { "ab" };
            oops( m.erase    (); out(m) ) { "ab" };
            m.selections[0].from.offset--;
            m.selections[0].upto.offset--;
            oops( m.backspace(); out(m) ) { "b" };
            oops( m.backspace(); out(m) ) { "b" };
            oops( m.insert("a"); out(m) ) { "ab" };
            oops( m.undo     (); out(m) ) { "b" };
            oops( m.undo     (); out(m) ) { "ab" };
            oops( m.undo     (); out(m) ) { "abc" };
            oops( m.undo     (); out(m) ) { "abdc" };
            oops( m.undo     (); out(m) ) { "abc" };
            oops( m.undo     (); out(m) ) { "ab" };
            oops( m.undo     (); out(m) ) { "a" };
            oops( m.undo     (); out(m) ) { "" };
            oops( m.undo     (); out(m) ) { "" };
        }
        test("text.model.redo");
        {
            model m;
            oops( m.redo     (); out(m) ) { "" };
            oops( m.undo     (); out(m) ) { "" };
            oops( m.insert("a"); out(m) ) { "a" };
            oops( m.redo     (); out(m) ) { "a" };
            oops( m.undo     (); out(m) ) { "" };
            oops( m.redo     (); out(m) ) { "a" };
            oops( m.backspace(); out(m) ) { "" };
            oops( m.redo     (); out(m) ) { "" };
            oops( m.undo     (); out(m) ) { "a" };
            oops( m.undo     (); out(m) ) { "" };
            oops( m.erase    (); out(m) ) { "" };
            oops( m.redo     (); out(m) ) { "" };
        }
        test("text.model.erase");
        {
            model m
            (
                "abc"  "\n"
                "defg" "\n"
                "h"
            );
            m.selections.resize(2);
            m.selections[0] = range{place{0,1}, place{0,2}};
            m.selections[1] = range{place{1,1}, place{1,2}};

            oops( m.backspace(); out(m) ) {
                "ac",
                "dfg",
                "h"
            };
            oops( out(m.selections) ) {
                "(0,1)-(0,1)",
                "(1,1)-(1,1)"
            };
            oops( m.backspace(); out(m) ) {
                "c",
                "fg",
                "h"
            };
            oops( m.backspace(); out(m) ) {
                "c",
                "fg",
                "h"
            };
            oops( m.erase(); out(m) ) {
                "",
                "g",
                "h"
            };
            oops( m.erase(); out(m) ) {
                "",
                "",
                "h"
            };
            oops( out(m.selections) ) {
                "(0,0)-(0,0)",
                "(1,0)-(1,0)"
            };
            oops( m.erase(); out(m) ) {
                "",
                "",
                "h"
            };
            oops( out(m.selections) ) {
                "(0,0)-(0,0)",
                "(1,0)-(1,0)"
            };
            oops( m.undo(); out(m) ) {
                "",
                "g",
                "h"
            };
            oops( m.undo(); out(m) ) {
                "c",
                "fg",
                "h"
            };
            oops( m.undo(); out(m) ) {
                "ac",
                "dfg",
                "h"
            };
            oops( m.undo(); out(m) ) {
                "abc",
                "defg",
                "h"
            };
            oops( out(m.selections) ) {
                "(0,1)-(0,2)",
                "(1,1)-(1,2)"
            };
        }
    }
    catch(assertion_failed){}
}
