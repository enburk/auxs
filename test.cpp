#pragma once
#include "test_aux.h"
#include "test_doc.h"
#include "test_gui.h"
#include "test_pix.h"

struct Test:
widget<Test>
{
    gui::canvas canvas;
    gui::canvas toolbar;
    gui::radio::group buttons;
    array<gui::base::widget*> tests;
    TestFirst test_first;
    TestModel test_model;
    TestFonts test_fonts;
    TestTexts test_texts;
    TestCoros test_coros;
    TestMonad test_monad;
    TestColor test_color;
    TestWideo test_wideo;

    Test ()
    {
        skin = "gray";

        tests += &test_first; buttons.emplace_back().text.text = "unit test";
        tests += &test_model; buttons.emplace_back().text.text = "text model";
        tests += &test_fonts; buttons.emplace_back().text.text = "test fonts";
        tests += &test_texts; buttons.emplace_back().text.text = "test texts";
        tests += &test_coros; buttons.emplace_back().text.text = "coroutines";
        tests += &test_monad; buttons.emplace_back().text.text = "haskell";
        tests += &test_wideo; buttons.emplace_back().text.text = "widgets";
        tests += &test_color; buttons.emplace_back().text.text = "colors";

        buttons(0).on = true;
        for (int i=1; i<tests.size(); i++)
            tests[i]->hide();
    }

    void on_change (void* what) override
    {
        if (what == &skin)
        {
            canvas.color = RGBA::red;
            toolbar.color = gui::skins[skin].light.first;
        }
        if (what == &coord and
            coord.was.size !=
            coord.now.size)
        {
            int W = coord.now.w; if (W <= 0) return;
            int H = coord.now.h; if (H <= 0) return;
            int w = gui::metrics::text::height*10;
            int h = gui::metrics::text::height*12/7;
            int x = 0;

            canvas .coord = XYWH(0, 0, W, H);
            toolbar.coord = XYWH(0, 0, W, h);
            buttons.coord = XYWH(0, 0, W, h);

            for (auto & button : buttons) {
                button.coord = XYWH(x, 0, w, h);
                x += w; }

            for (auto & test : tests)
                test->coord = XYXY(0, h, W, H);
        }

        if (what == &buttons)
            for (int i=0; i<tests.size(); i++)
                tests[i]->show(buttons(i).on.now);

        if (test_first.done) buttons[0].text.color =
            test_first.ok ? RGBA::green : RGBA::error;

    }
};

sys::app<Test> app("auxs");

#include "windows_fonts.h"
#include "windows_images.h"
#include "windows_system.h"
#include "windows_windows.h"


