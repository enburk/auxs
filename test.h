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
    TestAuxCoro    aux_coro;
    TestAuxHask    aux_hask;
    TestDoc        doc;
    TestDocHtml    doc_html;
    TestPix        pix;
    TestPixFonts   pix_fonts;
    TestGui        gui;
    TestGuiColors  gui_colors;
    TestGuiColorsX gui_colorsx;
    TestGuiFormat  gui_format;
    TestGuiConsole gui_console;
    TestGuiEditor  gui_editor;
    TestGuiAnimat  gui_animat;

    Test ()
    {
        skin = "gray";

        tests += {&aux,         "aux"};
        tests += {&aux_coro,    "coroutines"};
        tests += {&aux_hask,    "haskell"};

        tests += {&doc,         "doc"};
        tests += {&doc_html,    "html"};

        tests += {&pix,         "pix"};
        tests += {&pix_fonts,   "fonts"};

        tests += {&gui,         "gui"};
        tests += {&gui_colors,  "colors"};
        tests += {&gui_colorsx, "colors+"};
        tests += {&gui_format,  "format"};
        tests += {&gui_animat,  "animation"};
        tests += {&gui_console, "console"};
        tests += {&gui_editor,  "editor"};

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
            canvas.color = RGBA::red;
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

            canvas.coord = XYWH(0, 0, W, H);
            buttons.coord = XYWH(W-w, 0, w, H);

            for (auto & button : buttons) {
                str s = button.text.text;
                if (s == "doc"
                or  s == "pix"
                or  s == "gui")
                y += h;
                button.coord = XYWH(0, y, w, h);
                y += h; }

            for (auto test: tests)
                test.first->coord =
                XYWH(0, 0, W-w, H);
        }

        if (what == &buttons)
            for (int i=0; i<tests.size(); i++)
                tests[i].first->show(
                    buttons(i).on.now);
    }
};


