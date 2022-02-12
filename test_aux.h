#pragma once
#include "gui_widget_console.h"
#include "aux_unittest_coro.h"
#include "aux_unittest_array.h"
#include "aux_unittest_string.h"
using namespace pix;
using gui::widget;

struct TestFirst:
widget<TestFirst>
{
    bool ok = true;
    bool done = false;
    gui::canvas canvas;
    gui::area<gui::console> console1;
    gui::area<gui::console> console2;
    gui::area<gui::console> console3;
    void on_change (void* what) override
    {
        if (what == &coord && coord.was.size != coord.now.size)
        {
            int W = coord.now.w; if (W <= 0) return; int w = W/3;
            int H = coord.now.h; if (H <= 0) return;
            console1.coord = XYWH(w*0, 0, w, H);
            console2.coord = XYWH(w*1, 0, w, H);
            console3.coord = XYWH(w*2, 0, w, H);

            if (done) return; done = true;

            auto style = pix::text::style{
            sys::font{"Consolas"}, RGBA::black};
            console1.object.page.style = style;
            console2.object.page.style = style;
            console3.object.page.style = style;

            aux::unittest::test_array();
            aux::unittest::test("");
            console1.object.page.html = 
            aux::unittest::results; ok &= 
            aux::unittest::all_ok;

            aux::unittest::test_string();
            aux::unittest::test("");
            console2.object.page.html = 
            aux::unittest::results; ok &= 
            aux::unittest::all_ok;

            aux::unittest::test("");
            console3.object.page.html = 
            aux::unittest::results; ok &= 
            aux::unittest::all_ok;

            console1.object.page.scroll.y.top = max<int>();
            console2.object.page.scroll.y.top = max<int>();
            console3.object.page.scroll.y.top = max<int>();
        }
    }
};


struct TestCoros:
widget<TestCoros>
{
    bool ok = true;
    bool done = false;
    gui::canvas canvas;
    gui::button asyncs;
    gui::area<gui::console> console1;
    gui::area<gui::console> console2;
    gui::area<gui::console> console3;
    void on_change (void* what) override
    {
        if (what == &coord && coord.was.size != coord.now.size)
        {
            int h = gui::metrics::text::height*12/7;
            int W = coord.now.w; if (W <= 0) return; int w = (W-5*h)/3;
            int H = coord.now.h; if (H <= 0) return;
            console1.coord = XYWH(w*0, 0, w, H);
            console2.coord = XYWH(w*1, 0, w, H);
            console3.coord = XYWH(w*2, 0, w, H);
            asyncs  .coord = XYWH(w*3, 0, 5*h, h);
            asyncs.text.text = "asyncs";

            if (done) return; done = true;

            auto style = pix::text::style{
            sys::font{"Consolas"}, RGBA::black};
            console1.object.page.style = style;
            console2.object.page.style = style;
            console3.object.page.style = style;

            aux::unittest::test_coro1();
            aux::unittest::test("");
            console1.object.page.html = 
            aux::unittest::results; ok &= 
            aux::unittest::all_ok;

            aux::unittest::test_coro2();
            aux::unittest::test("");
            console2.object.page.html = 
            aux::unittest::results; ok &= 
            aux::unittest::all_ok;

            console1.object.page.scroll.y.top = max<int>();
            console2.object.page.scroll.y.top = max<int>();
            console3.object.page.html = "press the button "
            "------------------------------------------->";
        }
        if (what == &asyncs)
        {
            aux::unittest::test_coro3();
            aux::unittest::test("");
            console3.object.page.html = 
            aux::unittest::results; ok &= 
            aux::unittest::all_ok;

            console3.object.page.scroll.y.top = max<int>();
        }
    }
};

struct TestMonad:
widget<TestMonad>
{
    bool ok = true;
    bool done = false;
    gui::canvas canvas;
    gui::area<gui::console> console1;
    gui::area<gui::console> console2;
    gui::area<gui::console> console3;
    void on_change (void* what) override
    {
        if (what == &coord && coord.was.size != coord.now.size)
        {
            int h = gui::metrics::text::height*12/7;
            int W = coord.now.w; if (W <= 0) return; int w = W/3;
            int H = coord.now.h; if (H <= 0) return;
            console1.coord = XYWH(w*0, 0, w, H);
            console2.coord = XYWH(w*1, 0, w, H);
            console3.coord = XYWH(w*2, 0, w, H);

            if (done) return; done = true;

            auto style = pix::text::style{
            sys::font{"Consolas"}, RGBA::black};
            console1.object.page.style = style;
            console2.object.page.style = style;
            console3.object.page.style = style;

            aux::unittest::test_monad1();
            aux::unittest::test("");
            console1.object.page.html = 
            aux::unittest::results; ok &= 
            aux::unittest::all_ok;

            aux::unittest::test_monad2();
            aux::unittest::test("");
            console2.object.page.html = 
            aux::unittest::results; ok &= 
            aux::unittest::all_ok;

            aux::unittest::test_monad3();
            aux::unittest::test("");
            console3.object.page.html = 
            aux::unittest::results; ok &= 
            aux::unittest::all_ok;

            console1.object.page.scroll.y.top = max<int>();
            console2.object.page.scroll.y.top = max<int>();
            console3.object.page.scroll.y.top = max<int>();
        }
    }
};
