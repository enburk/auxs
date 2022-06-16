#pragma once
#include "aux_unittest.h"
#include "gui_widget_console.h"
#include "gui_widget_text_editline.h"
#include "gui_widget_text_editor.h"
using gui::widget;


struct TestGuiEditor:
widget<TestGuiEditor>
{
    gui::area<gui::console> console; 
    gui::area<gui::text::editor> editor1; 
    gui::area<gui::text::editor> editor2; 
    gui::area<gui::text::one_line_editor> edline1; 
    gui::area<gui::text::one_line_editor> edline2; 
    gui::splitter splitt1;
    gui::splitter splitt2;
    int x1 = 33'00;
    int x2 = 66'00;

    gui::text::editor ee;
    gui::text::one_line_editor el;
    bool once_flag = false;
    bool ok = true;

    void once ()
    {
        if (
        once_flag) return;
        once_flag = true;

        editor1.object.text = 
        "Lorem <b>ipsum</b>\n\n"
        "Lorem ipsum dolor sit amet, consectetur "
        "adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. "
        "Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip "
        "ex ea commodo consequat.\nDuis aute irure dolor in reprehenderit in voluptate velit "
        "esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non "
        "proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";

        editor2.object.wordwrap = false;
        editor2.object.virtual_space = true;
        editor2.object.text = 
        "template <class ForwardIterator, class T, class Distance>\n"
        "ForwardIterator __lower_bound(ForwardIterator first, ForwardIterator last,\n"
        "                              const T& value, Distance*,\n"
        "                              forward_iterator_tag) {\n"
        "    Distance len = 0;\n"
        "    distance(first, last, len);\n"
        "    Distance half;\n"
        "    ForwardIterator middle;\n"
        "\n"
        "    while (len > 0) {\n"
        "        half = len / 2;\n"
        "        middle = first;\n"
        "        advance(middle, half);\n"
        "        if (*middle < value) {\n"
        "            first = middle;\n"
        "            ++first;\n"
        "            len = len - half - 1;\n"
        "        } else\n"
        "            len = half;\n"
        "    }\n"
        "    return first;\n"
        "}\n";

        auto style = pix::text::style{
        pix::font{"Consolas"}, rgba::black };
        console.object.view.style = style;
        editor2.object.view.style = style;
        editor2.object.view.current_line_frame.color = rgba(64,64,64,64);

        ee.hide();
        el.hide();
        using namespace aux::unittest;
        int h = pix::metrics(style.font).height;
        int w = pix::metrics(style.font).average_char_width;
        str w0 = std::to_string(0*w); str h0 = std::to_string(0*h);
        str w1 = std::to_string(1*w); str h1 = std::to_string(1*h);
        str w2 = std::to_string(2*w); str h2 = std::to_string(2*h);
        str w3 = std::to_string(3*w); str h3 = std::to_string(3*h);
        str w4 = std::to_string(4*w); str h4 = std::to_string(4*h);
        str w5 = std::to_string(5*w); str h5 = std::to_string(5*h);
        try
        {
            xy point;
            auto& carets = ee.view.cell.carets;
            auto& bars = ee.view.cell.selection_bars;
            ee.scroll.x.mode = gui::scroll::mode::none;
            ee.scroll.y.mode = gui::scroll::mode::none;
            ee.coord = xywh(0,0, 5*w, 5*h);
            ee.style = style;
            
            auto input = [this](str key){
                ee.on_key(key, true, key.size() == 1);
                str s = ee.selected();
                s.replace_all("\n", "|");
                out(s); };
            
            test("editor.init");
            {
                oops(out(ee.coord.now.w/w)) { "5" };
                oops(out(ee.coord.now.h/h)) { "5" };
                oops(out(ee.selected()))    { "" };
                ouch(out(carets.size())) { "1" };
                oops(out(carets[0].coord.now.x)) { "0" };
                oops(input("shift+left" )) { "" };
                oops(input("shift+right")) { "" };
                oops(input("shift+home" )) { "" };
                oops(input("shift+end"  )) { "" };
            }
            test("editor.char");
            {
                oops(input("1")) { "" };
                oops(input("shift+left" )) { "1" };
                oops(input("shift+right")) { ""  };
                oops(input("shift+home" )) { "1" };
                oops(input("shift+end"  )) { ""  };
                oops(input("ctrl+home"  )) { ""  };
                oops(input("shift+left" )) { ""  };
                oops(input("shift+right")) { "1" };
                oops(input("shift+home" )) { ""  };
                oops(input("shift+end"  )) { "1" };
                oops(input("ctrl+end"   )) { ""  };
            }
            test("editor.word");
            {
                oops(input("2")) { "" };
                oops(input("shift+left"  )) { "2"  };
                oops(input("shift+left"  )) { "12" };
                oops(input("shift+left"  )) { "12" };
                oops(input("shift+right" )) { "2"  };
                oops(input("shift+right" )) { ""   };
                oops(input("shift+right" )) { ""   };
                oops(input("shift+up"    )) { ""   };
                oops(input("shift+down"  )) { ""   };
            }
            test("editor.word.wrap");
            {
                oops(input("3")) { "" };
                oops(input(" ")) { "" };
                oops(input("a")) { "" };
                oops(input("b")) { "" };
                oops(input("shift+up"    )) { "3 ab" };
                oops(input("3")) { "" };
                oops(input(" ")) { "" };
                oops(input("a")) { "" };
                oops(input("b")) { "" };
                oops(input("shift+up"    )) { "3 ab" };
                oops(input("shift+right" )) { " ab"   };
                oops(input("shift+right" )) { "ab"   };
                oops(input("shift+right" )) { "b"   };
                oops(input("shift+right" )) { ""   };
                oops(input("ctrl+shift+home" )) { "123 ab" };
                oops(input("ctrl+shift+end"  )) { "" };
            }
            test("editor.lines");
            {
                oops(input("enter")) { "" };
                oops(input("A"    )) { "" };
                oops(input("ctrl+shift+home" )) { "123 ab|A" };
                oops(input("ctrl+end"    )) { "" };
                oops(input("shift+up"    )) { "b|A" };
                oops(input("shift+up"    )) { "23 ab|A" };
                oops(input("left"        )) { "" };
                oops(input("shift+page down" )) { "23 ab|A" };
                oops(input("right")) { "" };
                oops(input("B"    )) { "" };
                oops(input("enter")) { "" };
                oops(input("enter")) { "" };
                oops(input("C"    )) { "" };
                oops(input("ctrl+shift+home" )) { "123 ab|AB||C" };
            }
            test("editor.bars");
            {
                ouch(out(bars.size())) { "3" };
                oops(out(bars[0].coord.now.x)) { w0 };
                oops(out(bars[0].coord.now.y)) { h0 };
                oops(out(bars[0].coord.now.w)) { w4 };
                oops(out(bars[0].coord.now.h)) { h1 };
                oops(out(bars[1].coord.now.x)) { w0 };
                oops(out(bars[1].coord.now.y)) { h1 };
                oops(out(bars[1].coord.now.w)) { w2 };
                oops(out(bars[1].coord.now.h)) { h2 };
                oops(out(bars[2].coord.now.x)) { w0 };
                oops(out(bars[2].coord.now.y)) { h4 };
                oops(out(bars[2].coord.now.w)) { w1 };
                oops(out(bars[2].coord.now.h)) { h1 };
            }
            test("editor.virtual.space.1");
            {
                ee.wordwrap = false;
                ee.virtual_space = true;
                oops(input("ctrl+home" )) { "" };
                oops(input("shift+down")) { "123 ab|" };
                oops(input("shift+down")) { "123 ab|AB|" };
                oops(input("shift+down")) { "123 ab|AB||" };
                oops(input("shift+down")) { "123 ab|AB||" };
                ouch(out(bars.size())) { "1" };
                oops(out(bars[0].coord.now.x)) { w0 };
                oops(out(bars[0].coord.now.y)) { h0 };
                oops(out(bars[0].coord.now.w > 10000)) { "1" };
                oops(out(bars[0].coord.now.h)) { h3 };
                oops(input("shift+right")) { "123 ab|AB||C" };
                oops(input("shift+right")) { "123 ab|AB||C" };
                ouch(out(bars.size())) { "3" };
                oops(out(bars[0].coord.now.x)) { w0 };
                oops(out(bars[0].coord.now.y)) { h0 };
                oops(out(bars[0].coord.now.w > 10000)) { "1" };
                oops(out(bars[0].coord.now.h)) { h3 };
                oops(out(bars[1].coord.now.x)) { w0 }; // C
                oops(out(bars[1].coord.now.y)) { h3 };
                oops(out(bars[1].coord.now.w)) { w1 };
                oops(out(bars[1].coord.now.h)) { h1 };
                oops(out(bars[2].coord.now.x)) { w1 }; // virtual space
                oops(out(bars[2].coord.now.y)) { h3 };
                oops(out(bars[2].coord.now.w)) { w1 };
                oops(out(bars[2].coord.now.h)) { h1 };
            }
            test("editor.virtual.space.2");
            {
                oops(input("right")) { "" };
                ouch(out(bars.size())) { "0" };
                ouch(out(carets.size())) { "1" };
                oops(out(carets[0].coord.now.x)) { w2 };
                oops(out(carets[0].coord.now.y)) { h3 };
                oops(out(carets[0].coord.now.w)) { w1 };
                oops(out(carets[0].coord.now.h)) { h1 };
                oops(input("D"              )) { "" };
                oops(input("ctrl+shift+home")) { "123 ab|AB||C D" };
                oops(input("ctrl+shift+end" )) { "" };
                oops(input("shift+up"       )) { "|C D" };
                ouch(out(bars.size())) { "2" };
                oops(out(bars[0].coord.now.x)) { w3 }; // virtual space
                oops(out(bars[0].coord.now.y)) { h2 };
                oops(out(bars[0].coord.now.w > 10000)) { "1" };
                oops(out(bars[0].coord.now.h)) { h1 };
                oops(out(bars[1].coord.now.x)) { w0 }; // C D
                oops(out(bars[1].coord.now.y)) { h3 };
                oops(out(bars[1].coord.now.w)) { w3 };
                oops(out(bars[1].coord.now.h)) { h1 };
                ouch(out(carets.size())) { "1" };
                oops(out(carets[0].coord.now.x)) { w3 };
                oops(out(carets[0].coord.now.y)) { h2 };
                oops(out(carets[0].coord.now.w)) { w1 };
                oops(out(carets[0].coord.now.h)) { h1 };
                oops(input("shift+up")) { "||C D" };
                ouch(out(bars.size())) { "3" };
                oops(out(bars[0].coord.now.x)) { w3 }; // virtual space
                oops(out(bars[0].coord.now.y)) { h1 };
                oops(out(bars[0].coord.now.w > 10000)) { "1" };
                oops(out(bars[0].coord.now.h)) { h1 };
                oops(out(bars[1].coord.now.x)) { w0 }; // virtual space
                oops(out(bars[1].coord.now.y)) { h2 };
                oops(out(bars[1].coord.now.w > 10000)) { "1" };
                oops(out(bars[1].coord.now.h)) { h1 };
                oops(out(bars[2].coord.now.x)) { w0 }; // C D
                oops(out(bars[2].coord.now.y)) { h3 };
                oops(out(bars[2].coord.now.w)) { w3 };
                oops(out(bars[2].coord.now.h)) { h1 };
                ouch(out(carets.size())) { "1" };
                oops(out(carets[0].coord.now.x)) { w3 };
                oops(out(carets[0].coord.now.y)) { h1 };
                oops(out(carets[0].coord.now.w)) { w1 };
                oops(out(carets[0].coord.now.h)) { h1 };
                oops(input("shift+left")) { "||C D" };
                oops(input("shift+left")) { "B||C D" };
            }
            test("editor.multicaret");
            {
                oops(input("ctrl+A")) { "123 ab|AB||C D" };
                oops(input("ctrl+home")) { "" };
                oops(input("alt+shift+down")) { "" };
                oops(input("alt+shift+down")) { "" };
                oops(input("alt+shift+down")) { "" };
                oops(input("alt+shift+down")) { "" };
                oops(input("alt+shift+down")) { "" };
                oops(out(carets.size())) { "4" };
                oops(input("shift+right")) { "1|A||C" };
                oops(input("shift+left" )) { "" };
                oops(input("up"         )) { "" };
                oops(out(carets.size())) { "4" };
                oops(input("shift+right")) { "1|A||C" };
                oops(input("shift+up"   )) { "1|A|" };
                oops(input("escape"     )) { "" };
                oops(out(carets.size())) { "1" };
                oops(out(bars.size())) { "0" };
            }
            test("editor.undo.redo");
            {
            }
            test("editor.clipboard");
            {
                oops(input("ctrl+end")) { "" };
            }
            test("editor.indent");
            {
            }
            test("editor.clear");
            {
                oops(input("ctrl+A")) { "123 ab|AB||C D" };
                oops(input("delete")) { "" };
                oops(input("ctrl+A")) { "" };
                ouch(out(bars.size())) { "0" };
                ouch(out(carets.size())) { "1" };
                oops(out(carets[0].coord.now.x)) { "0" };
                oops(out(carets[0].coord.now.y)) { "0" };
            }
        }
        catch (assertion_failed) {}
        try
        {
            el.coord = xywh(0,0, 3*w, 1*h);
            el.style = style;
            
            auto input = [this](str key){
                el.on_key(key, true, key.size() == 1);
                out(el.editor.selected()); };
            
            test("one_line_editor.init");
            {
                oops(out(el.coord.now.w/w)) { "3" };
                oops(out(el.coord.now.h/h)) { "1" };
                oops(input("shift+left" )) { "" };
                oops(input("shift+right")) { "" };
            }
            test("one_line_editor.fill");
            {
                oops(input("1" )) { "" };
                oops(input("2" )) { "" };
                oops(input("3" )) { "" };
                oops(input("4" )) { "" };
                oops(input("ctrl+home")) { "" };
                oops(input("ctrl+shift+end")) { "1234" };
            }
            test("one_line_editor.clear");
            {
                oops(input("ctrl+home")) { "" };
                oops(input("delete")) { "" };
                oops(input("delete")) { "" };
                oops(input("delete")) { "" };
                oops(input("delete")) { "" };
                oops(input("delete")) { "" };
                oops(input("ctrl+shift+end" )) { "" };
            }
        }
        catch (assertion_failed) {}
        aux::unittest::test("");
        console.object.page.html =
        aux::unittest::results; ok &=
        aux::unittest::all_ok;
        console.object.page.scroll.y.top = max<int>();
    }

    void on_change (void* what) override
    {
        if (what == &skin)
        {
            editor1.show_focus = true;
            editor2.show_focus = true;
            edline1.show_focus = true;
            edline2.show_focus = true;
            editor1.object.canvas.color = rgba::white;
            editor2.object.canvas.color = rgba::white;
            edline1.object.canvas.color = rgba::white;
            edline2.object.canvas.color = rgba::white;
        }
        if (what == &coord)
        {
            int W = coord.now.w; if (W <= 0) return;
            int H = coord.now.h; if (H <= 0) return;
            int h = gui::metrics::text::height*13/10;
            int d = gui::metrics::line::width*6;
            int l = W * x1 / 100'00;
            int r = W * x2 / 100'00;
            console.coord = xyxy(0-0, 0, l+0, H);
            editor1.coord = xyxy(l-0, 0, r+0, H-h);
            editor2.coord = xyxy(r-0, 0, W+0, H-h);
            edline1.coord = xyxy(l-0, H-h, r+0, H);
            edline2.coord = xyxy(r-0, H-h, W+0, H);
            splitt1.coord = xyxy(l-d, 0, l+d, H);
            splitt2.coord = xyxy(r-d, 0, r+d, H);
            splitt1.lower = 10'00 * W / 100'00;
            splitt1.upper = 40'00 * W / 100'00;
            splitt2.lower = 60'00 * W / 100'00;
            splitt2.upper = 90'00 * W / 100'00;
            once();
        }
        if (what == &splitt1)
        {
            x1 = 100'00 *
            splitt1.middle/coord.now.w;
            on_change(&coord);
        }
        if (what == &splitt2)
        {
            x2 = 100'00 *
            splitt2.middle/coord.now.w;
            on_change(&coord);
        }
        if (what == &edline1.object)
        {
            editor1.object.text += edline1.object.text;
            editor1.object.text += "\n";
            edline1.object.text = "";
        }
        if (what == &edline2.object)
        {
            editor2.object.text += edline2.object.text;
            editor2.object.text += "\n";
            edline2.object.text = "";
        }
    }
};
