#pragma once
#include "test_aux.h"
#include "test_doc.h"
#include "test_gui.h"
#include "test_pix.h"

struct Test:
widget<Test>
{
    gui::canvas canvas;
    gui::radio::group buttons;
    array<std::pair<gui::base::widget*, str>> tests;
    TestAux        aux;
    TestAuxMath    aux_math;
    TestAuxCoro    aux_coro;
    TestAuxHask    aux_hask;
    TestDoc        doc;
    TestDocHtml    doc_html;
    TestPix        pix;
    TestPixDraw    pix_draw;
    TestPixFonts   pix_fonts;
    TestGui        gui;
    TestGuiColors  gui_colors;
    TestGuiColorsX gui_colorsx;
    TestGuiFormat  gui_format;
    TestGuiAnimat  gui_animat;
    TestGuiConsole gui_console;
    TestGuiEditor  gui_editor;
    TestGuiGraph   gui_graph;

    Test ()
    {
        skin = "gray";

        tests += {&aux,         "aux"};
        tests += {&aux_math,    "math"};
        tests += {&aux_coro,    "coroutines"};
        tests += {&aux_hask,    "haskell"};

        tests += {&doc,         "doc"};
        tests += {&doc_html,    "html"};

        tests += {&pix,         "pix"};
        tests += {&pix_draw,    "draw"};
        tests += {&pix_fonts,   "fonts"};

        tests += {&gui,         "gui"};
        tests += {&gui_colors,  "colors"};
        tests += {&gui_colorsx, "colors+"};
        tests += {&gui_format,  "format"};
        tests += {&gui_animat,  "animation"};
        tests += {&gui_console, "console"};
        tests += {&gui_editor,  "editor"};
        tests += {&gui_graph,   "graph"};

        for (auto [ptr, title] : tests)
            buttons.emplace_back()
            .text.text = title;

        buttons(0).on = true;
        for (int i=1; i<tests.size(); i++)
            tests[i].first->hide();

    }

    void on_change (void* what) override
    {
        if (what == &skin)
        {
            canvas.color = rgba::red;
        }
        if (what == &coord and
            coord.was.size !=
            coord.now.size)
        {
            int W = coord.now.w; if (W <= 0) return;
            int H = coord.now.h; if (H <= 0) return;
            int w = gui::metrics::text::height*10;
            int h = gui::metrics::text::height*12/7;
            int y = 0;

            canvas.coord = xywh(0, 0, W, H);
            buttons.coord = xywh(W-w, 0, w, H);

            for (auto & button : buttons) {
                str s = button.text.text;
                if (s == "doc"
                or  s == "pix"
                or  s == "gui")
                y += h;
                button.coord = xywh(0, y, w, h);
                y += h; }

            for (auto test: tests)
                test.first->coord =
                xywh(0, 0, W-w, H);
        }

        if (what == &buttons)
            for (int i=0; i<tests.size(); i++)
                tests[i].first->show(
                    buttons(i).on.now);
    }
};


