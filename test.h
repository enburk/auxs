#pragma once
#include "test_aux.h"
#include "test_doc.h"
#include "test_gui.h"
#include "test_pix.h"
#include "test_sfx.h"

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
    TestPix        pix;
    TestPixDraw    pix_draw;
    TestPixDrawX   pix_drawx;
    TestPixFonts   pix_fonts;
    TestPixText    pix_text;
    TestPixTextX   pix_textx;
    TestPixUtil    pix_util;
    TestDoc        doc;
    TestDocHtml    doc_html;
    TestGui        gui;
    TestGuiColors  gui_colors;
    TestGuiColorsX gui_colorsx;
    TestGuiFormat  gui_format;
    TestGuiAnimat  gui_animat;
    TestGuiConsole gui_console;
    TestGuiEditor  gui_editor;
    TestSfx        sfx;
    TestSfxTrees   sfx_trees;

    Test ()
    {
        skin = "gray";

        tests += {&aux,         "aux"};
        tests += {&aux_math,    "math"};
        tests += {&aux_coro,    "coroutines"};
        tests += {&aux_hask,    "haskell"};

        tests += {&pix,         "pix"};
        tests += {&pix_draw,    "draw"};
        tests += {&pix_drawx,  "+draw+"};
        tests += {&pix_fonts,   "fonts"};
        tests += {&pix_text,    "text"};
        tests += {&pix_textx,  "+text+"};
        tests += {&pix_util,    "util"};

        tests += {&doc,         "doc"};
        tests += {&doc_html,    "html"};

        tests += {&gui,         "gui"};
        tests += {&gui_colors,  "colors"};
        tests += {&gui_colorsx,"+colors+"};
        tests += {&gui_format,  "format"};
        tests += {&gui_animat,  "animation"};
        tests += {&gui_console, "console"};
        tests += {&gui_editor,  "editor"};

        tests += {&sfx,         "sfx"};
        tests += {&sfx_trees,   "trees"};

        for (auto [ptr, title] : tests)
            buttons.emplace_back()
            .text.text = title;

        buttons(0).on = true;
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
            int w = gui::metrics::text::height*7;
            int h = gui::metrics::text::height*13/10;
            int y = 0;

            canvas.coord = xywh(0, 0, W, H);
            buttons.coord = xywh(W-w, 0, w, H);

            for (auto & button: buttons) {
                str s = button.text.text;
                if (s == "doc"
                or  s == "pix"
                or  s == "gui"
                or  s == "sfx")
                y += h;
                button.coord = xywh(0, y, w, h);
                y += h;
            }

            for (int i=0; i<tests.size(); i++)
            {
                if (buttons(i).on.now)
                tests[i].first->coord = xyxy(0,0,
                buttons.coord.now.x, coord.now.h);
            }
        }

        if (what == &buttons)
        {
            for (int i=0; i<tests.size(); i++)
            {
                bool on = buttons(i).on.now;
                tests[i].first->show(on); if (on)
                tests[i].first->coord = xyxy(0,0,
                buttons.coord.now.x, coord.now.h);
            }
        }
    }
};


