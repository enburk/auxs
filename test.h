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
    TestAux00 test_aux_00;
    TestAux01 test_aux_01;
    TestAux02 test_aux_02;
    TestDoc00 test_doc_00;
    TestPix00 test_pix_00;
    TestPix01 test_pix_01;
    TestGui00 test_gui_00;
    TestGui01 test_gui_01;
    TestGui02 test_gui_02;
    TestGui03 test_gui_03;

    Test ()
    {
        skin = "gray";

        tests += {&test_aux_00, "aux"};
        tests += {&test_aux_01, "coroutines"};
        tests += {&test_aux_02, "haskell"};

        tests += {&test_doc_00, "doc"};

        tests += {&test_pix_00, "pix"};
        tests += {&test_pix_01, "fonts"};

        tests += {&test_gui_00, "gui"};
        tests += {&test_gui_01, "colors"};
        tests += {&test_gui_02, "console"};
        tests += {&test_gui_03, "editors"};

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

    void on_focus (bool on) override {
        for (auto [test, title] : tests)
            if (test->alpha.to > 0)
                test->on_focus(on); }

    void on_key_input (str symbol) override {
        for (auto [test, title] : tests)
            if (test->alpha.to > 0)
                test->on_key_input(symbol); }
    
    void on_key_pressed (str key, bool down) override {
        for (auto [test, title] : tests)
            if (test->alpha.to > 0)
                test->on_key_pressed(key,down); }
};


