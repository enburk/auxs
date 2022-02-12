#pragma once
#include "gui_widget_console.h"
#include "aux_unittest_coro.h"
#include "aux_unittest_array.h"
#include "aux_unittest_string.h"
#include "doc_text_model_a.h"
#include "doc_text_model_b.h"
using namespace pix;
using gui::widget;

struct TestTexts:
widget<TestTexts>
{
    gui::button doubling;
    gui::area<gui::console> console1;
    gui::area<gui::console> console2;
    gui::splitter splitter; int x = 40'00; int xx = 80;
    TestTexts () { on_change(&doubling); }
    str text = "<b>Lorem ipsum</b> dolor sit amet, consectetur adipiscing <i>elit</i>, "
    "sed do eiusmod tempor incididunt ut labore et dolore <b>magna <i>aliqua.</i></b> "
    "Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip "
    "<font color=#008000><i>ex ea commodo</i></font> consequat. Duis aute irure dolor "
    "in reprehenderit <b><i><u>in</u></i></b> voluptate <font color=#000080>velit</font> "
    "esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non "
    "proident, sunt in culpa qui officia deserunt mollit anim id est "
    "<b><font color=#008000>laborum.</font></b><br><br>";
    void on_change (void* what) override
    {
        if (what == &skin)
        {
            console1.object.page.view.canvas.color = RGBA::white;
            console2.object.page.view.canvas.color = RGBA::white;
        }
        if (what == &coord && coord.was.size != coord.now.size)
        {
            int W = coord.now.w; if (W <= 0) return;
            int H = coord.now.h; if (H <= 0) return;
            int w = gui::metrics::text::height*xx;
            int h = gui::metrics::text::height*12/7;
            int d = gui::metrics::line::width * 6;
            int m = w * x / 100'00;
            doubling.coord = XYWH(w-0, 0, w/9, h);
            console1.coord = XYXY(0-0, 0, m+0, H);
            console2.coord = XYXY(m-0, 0, w+0, H);
            splitter.coord = XYXY(m-d, 0, m+d, H);
            splitter.lower = 20'00 * w / 100'00;
            splitter.upper = 80'00 * w / 100'00;
        }
        if (what == &doubling)
        {
            doubling.text.text = "double text";
            console1.object.page.html = text;
            console2.object.page.html = text;
            text = text + text;
        }
        if (what == &splitter)
        {
            int w = gui::metrics::text::height*xx;
            x = 100'00 * splitter.middle / w;
            on_change(&coord);
        }
    }
};


struct TestWideo:
widget<TestWideo>
{
    bool ok = true;
    bool done = false;
    gui::canvas canvas;
    gui::area<gui::console> console;
    void on_change(void* what) override
    {
        if (what == &coord && coord.was.size != coord.now.size)
        {
            int h = gui::metrics::text::height * 12 / 7;
            int W = coord.now.w; if (W <= 0) return; int w = W / 3;
            int H = coord.now.h; if (H <= 0) return;
            console.coord = XYWH(w * 0, 0, w, H);

            if (done) return; done = true;

            auto style = pix::text::style{
            sys::font{"Consolas"}, RGBA::black };
            console.object.page.style = style;

            using namespace aux::unittest;
            try
            {
                test("widgets.size");
                {
                    auto canvas = gui::canvas{};
                    auto glyph1 = gui::text::glyph{};
                    auto token1 = gui::text::token{};
                    auto token2 = gui::text::token{};
                    token2 = doc::view::token{ "0123456789", pix::text::style_index{}, "", ""};
                    oops(out(sizeof canvas)) { "432" };
                    oops(out(sizeof glyph1)) { "440" };
                    oops(out(sizeof token1)) { "520" };
                    oops(out(sizeof token2)) { "520" };
                    oops(out(gui::metrics::text::height)) { "24" };
                    oops(out(gui::metrics::line::width)) { "1" };
                    oops(out(token2.coord.now.w/10)) { "13" };
                    oops(out(token2.coord.now.h)) { "32" };
                    oops(out(coord.now.w)) { "3594" };
                    oops(out(coord.now.h)) { "1972" };
                }
            }
            catch (assertion_failed) {}

            aux::unittest::test("");
            console.object.page.html =
                aux::unittest::results; ok &=
                aux::unittest::all_ok;

            console.object.page.scroll.y.top = max<int>();
        }
    }
};

struct TestColor:
widget<TestColor>
{
    struct sample:
    widget<sample>
    {
        str name;
        gui::schema schema;
        std::pair<RGBA,RGBA> colors[10];
        gui::text::view palette[10];
        gui::text::view title;
        gui::text::view error1;
        gui::text::view error2;
        gui::canvas toolbar;
        gui::radio::group buttons;
        gui::area<gui::text::page> page;
        str lorem = "<b>Lorem ipsum</b><br>"
        "Lorem ipsum dolor sit amet, <font color=#008000>consectetur</font> "
        "<font color=#000080>adipiscing</font> <i>elit</i>, sed do eiusmod tempor incididunt "
        "<font color=#800000>ut labore et dolore</font> <b>magna <i>aliqua.</i></b> "
        "Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip "
        "<font color=#008000><i>ex ea commodo</i></font> consequat. Duis aute irure dolor "
        "in reprehenderit <b><i><u>in</u></i></b> voluptate <font color=#000080>velit</font> "
        "esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non "
        "proident, sunt in culpa qui officia deserunt mollit anim id est "
        "<b><font color=#008000>laborum.</font></b><br><br>";

        void on_change (void* what) override
        {
            if (what == &coord)
            {
                int h = gui::metrics::text::height*12/7;
                int W = coord.now.w; if (W <= 0) return;
                int H = coord.now.h; if (H <= 0) return;
                int q = W/10;

                title.coord = XYWH(0,0, W,h);
                title.color = RGBA::white;
                title.html = "<b>"+name+"</b>";

                for (int i=0; i<10; i++) {
                    palette[i].coord = XYWH(q*i, h, q, q);
                    palette[i].canvas.color = colors[i].first;
                    palette[i].color = colors[i].second;
                    palette[i].text = std::to_string(i);
                }

                buttons.coord = XYWH(0,h+q, W,h);
                toolbar.coord = XYWH(0,h+q, W,h);
                toolbar.color = schema.light.first;

                int n = 3;
                for (int i=0; i<n; i++) {
                    buttons(i).text.text = "button";
                    buttons(i).coord = XYWH(i*W/n, 0, W/n, h);
                    buttons(i).skin = name;
                    buttons(i).on = i == 0;
                }

                error1.coord = XYWH(0,   h+q+h, W/2, h);
                error2.coord = XYWH(W/2, h+q+h, W/2, h);
                error1.canvas.color = schema.error.first;
                error2.canvas.color = schema.error.second;
                error1.color = schema.error.second;
                error2.color = schema.error.first;
                error1.text = "error";
                error2.text = "error";

                page.skin = name;
                page.coord = XYXY(0,h+q+h+h, W, H);
                page.object.html = lorem;
                page.object.alignment = XY{pix::left, pix::top};
                page.object.view.canvas.color = schema.ultralight.first;
                page.object.style = pix::text::style{
                    sys::font{"Segoe UI", h*4/7},
                    schema.dark.first};
            }
        }
    };

    gui::widgetarium<sample> samples;

    void on_change (void* what) override
    {
        if (what == &coord)
        {
            int W = coord.now.w; if (W <= 0) return;
            int H = coord.now.h; if (H <= 0) return;

            samples.coord = coord.now.local();

            if (samples.size() == 0)
            {
                for (auto [i, name]: enumerate(gui::palettes_names)) {
                    auto& palette = gui::palettes[name];
                    for (int j=0; j<10; j++)
                    samples(i).colors[j] = palette[j];
                    samples(i).schema = gui::skins[name];
                    samples(i).name = name;
                }
            }

            int n = samples.size();
            int square = W*H/n;
            int side = (int)(std::sqrt(square));
            if (side < 10) return;
            int nx = W/side;
            int ny = H/side;
            while (nx*ny < n) {
                side--; if (side < 10) return;
                nx = W/side;
                ny = H/side;
            }

            for (int y=0; y<ny; y++)
            for (int x=0; x<nx; x++)
            {
                int i = y*nx + x;
                if (i >= n) break;
                samples(i).coord = XYWH(
                x*side, y*side, side, side);
            }
        }
    }
};

