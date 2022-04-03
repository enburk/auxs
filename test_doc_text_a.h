#pragma once
#include "doc_text_model_a.h"
#include "aux_unittest.h"
namespace aux::unittest
{
    void out (doc::text::a::model const& m)
    {
        for (auto & line : m.lines) out(aux::unicode::string(line));
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
