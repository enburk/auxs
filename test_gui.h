#pragma once
#include "aux_unittest.h"
#include "gui_widget_console.h"
#include "gui_widget_geometry.h"
#include "test_gui_edit.h"
using namespace std::literals::chrono_literals;
using namespace pix;
using gui::widget;

str Lorem =
"<b><a href=\"lorem\">Lorem</a> <a href=\"ipsum\">ipsum</a></b><br>"
"Lorem ipsum dolor sit amet, <font color=#008000>consectetur</font> "
"<font color=#000080>adipiscing</font> <i>elit</i>, sed do eiusmod tempor incididunt "
"<font color=#800000>ut labore et dolore</font> <b>magna <i>aliqua.</i></b> "
"Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip "
"<font color=#008000><i>ex ea commodo</i></font> consequat. Duis aute irure dolor "
"in reprehenderit <b><i><u>in</u></i></b> voluptate <font color=#000080>velit</font> "
"esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non "
"proident, sunt in culpa qui officia deserunt mollit anim id est "
"<b><font color=#008000>laborum.</font></b><br><br>";

str LOrem =
"<font color=#800000>maroon </font>"
"<font color=#008000>green </font>"
"<font color=#000080>navy </font>"
"<font color=#800080>purple </font>"
"<font color=#808000>olive </font>"
"<font color=#008080>teal </font>"
"<font color=#FF0000>red </font>"  
"<font color=#00FF00>lime </font>"  
"<font color=#0000FF>blue </font>"  
"<font color=#FF00FF>fuchsia </font>"  
"<font color=#FFFF00>yellow </font>"  
"<font color=#00FFFF>aqua </font>"  
"<font color=#FFBF00>amber </font>"   
"<font color=#B00020>error </font>"   
"<br><br>" + Lorem;

struct TestGui:
widget<TestGui>
{
    bool ok = true;
    bool done = false;
    gui::canvas canvas;
    gui::area<gui::console> console;
    gui::widgetarium<gui::canvas> bars[6*3];
    gui::text::box cols[6*3];

    void on_change(void* what) override
    {
        if (what == &coord)
        {
            int W = coord.now.w; if (W <= 0) return;
            int H = coord.now.h; if (H <= 0) return;
            int h = gui::metrics::text::height;
            int w = 7*h;
            int d = 5;

            console.coord = xywh(0, 0, W/3, H);

            for (int x=0; x<6; x++)
            for (int y=0; y<3; y++)
            {
                int i = x*3 + y;
                cols[i].coord = xywh(W/3+x*w, y*H/3+d, w, (H-d)/3);
                bars[i].coord = xywh(W/3+x*w, y*H/3+d, w, (H-d)/3);
                array<str> fonts = { "", "Consolas", "Tahoma"};
                array<int> sizes = { 0, -9, h*10/9, -10, h*8/9, -8 };
                str font = fonts[y];
                int size = sizes[x];

                cols[i].style = pix::text::style{font, size};

                str s;
                int hh = x == 0 and y == 0 ? h : size > 0 ? size :
                pix::metrics(pix::font(font, size)).ascent +
                pix::metrics(pix::font(font, size)).descent;

                int ll = (H-d)/3 / (h*10/9) - 2;

                for (int l=0; l<ll; l++)
                {
                    s += " " + font + " " + std::to_string(size) + "\n";
                    bars[i](l).color = l%2 == 0 ? rgba::white : rgba::red;
                    bars[i](l).coord = xywh(d, hh*l, w-d, hh);
                }

                bars[i].truncate(ll);
                cols[i].text = s;
            }

            if (done) return; done = true;
            using namespace aux::unittest;
            auto style = pix::text::style{
            pix::font{"Consolas"}, rgba::black };
            console.object.page.style = style;
            try
            {
                test("effect.transit");
                {
                    oops(out(gui::transit<uint8_t>(0, 255,   0,   0))) { "255" };
                    oops(out(gui::transit<uint8_t>(0, 255,   0, 255))) {   "0" };
                    oops(out(gui::transit<uint8_t>(0, 255,  64, 255))) {  "64" };
                    oops(out(gui::transit<uint8_t>(0, 255, 128, 255))) { "128" };
                    oops(out(gui::transit<uint8_t>(0, 255, 192, 255))) { "192" };
                    oops(out(gui::transit<uint8_t>(0, 255, 255, 255))) { "255" };
                    oops(out(gui::transit<uint8_t>(0, 255, 256, 255))) { "255" };

                    oops(out(gui::transit<bool>(false, true,   0,   0))) { "1" };
                    oops(out(gui::transit<bool>(false, true,   0, 255))) { "0" };
                    oops(out(gui::transit<bool>(false, true,  64, 255))) { "0" };
                    oops(out(gui::transit<bool>(false, true, 128, 255))) { "0" };
                    oops(out(gui::transit<bool>(false, true, 192, 255))) { "0" };
                    oops(out(gui::transit<bool>(false, true, 255, 255))) { "1" };
                    oops(out(gui::transit<bool>(false, true, 256, 255))) { "1" };
                }
                test("widgets.size");
                {
                    oops(out(coord.now.w)) { "3594" };
                    oops(out(coord.now.h)) { "1972" };
                    oops(out(gui::metrics::text::height)) { "24" };
                    oops(out(gui::metrics::line::width)) { "1" };
                    oops(out(sizeof gui::canvas)) { "" };
                    oops(out(sizeof gui::button)) { "" };
                    oops(out(sizeof gui::text::box)) { "" };
                    oops(out(sizeof gui::text::cell)) { "" };
                    oops(out(sizeof gui::text::view)) { "" };
                    oops(out(sizeof gui::text::page)) { "" };
                    oops(out(sizeof gui::text::editor)) { "" };
                }
            }
            catch (assertion_failed) {}
            aux::unittest::test("");
            console.object.page.html =
            aux::unittest::results; ok &=
            aux::unittest::all_ok;
            console.object.page.
            scroll.y.top =
            max<int>();
        }
    }
};

struct TestGuiColors:
widget<TestGuiColors>
{
    gui::canvas canvas;
    gui::widgetarium<gui::text::view> samples;

    void on_change (void* what) override
    {
        if (what == &coord)
        {
            int W = coord.now.w; if (W <= 0) return;
            int H = coord.now.h; if (H <= 0) return;
            int h = gui::metrics::text::height*2;

            canvas.color = rgba::black;
            canvas.coord = coord.now.local();
            samples.coord = coord.now.local();

            if (samples.size() == 0)
            {
                int nn = gui::palettes_names.size()/2;
                int nny = H / h / 10; if (nny == 0) nny = 1;
                int nnx = (nn+nny-1) / nny;
                int w = W / nnx;
                int n = 0;

                for (str name: gui::palettes_names)
                {
                    if (name.ends_with("+")) continue;
                    auto& palette = gui::palettes[name];
                    int nx = n / nny;
                    int ny = n % nny;

                    for (int j = 0; j < 10; j++) {
                        int i = n * 10 + j;
                        samples(i).coord = xywh(nx*w, ny*h*10 + j*h, w, h);
                        samples(i).color = palette[j].first;
                        samples(i).font = pix::font("Tahoma", h);
                        samples(i).text = name + " " +
                            std::to_string(j);
                    }
                    n++;
                }
            }
        }
    }
};

struct TestGuiColorsX:
widget<TestGuiColorsX>
{
    struct sample:
    widget<sample>
    {
        str name;
        gui::schema schema;
        std::pair<rgba,rgba> colors[10];
        gui::text::view palette[10];
        gui::text::view title;
        gui::text::view error1;
        gui::text::view error2;
        gui::canvas toolbar;
        gui::radio::group buttons;
        gui::area<gui::text::page> page;

        void on_change (void* what) override
        {
            if (what == &coord)
            {
                int W = coord.now.w; if (W <= 0) return;
                int H = coord.now.h; if (H <= 0) return;
                int h = gui::metrics::text::height*13/10;
                int q = W/10;

                title.coord = xywh(0,0, W,h);
                title.color = rgba::white;
                title.html = "<b>"+name+"</b>";

                for (int i=0; i<10; i++) {
                    palette[i].coord = xywh(q*i, h, q, q);
                    palette[i].canvas.color = colors[i].first;
                    palette[i].color = colors[i].second;
                    palette[i].text = std::to_string(i);
                }

                buttons.coord = xywh(0,h+q, W,h);
                toolbar.coord = xywh(0,h+q, W,h);
                toolbar.color = schema.light.first;

                int n = 3;
                for (int i=0; i<n; i++) {
                    buttons(i).text.text = "button";
                    buttons(i).tooltip.text.text = "tooltip";
                    buttons(i).coord = xywh(i*W/n, 0, W/n, h);
                    buttons(i).skin = name;
                    buttons(i).on = i == 0;
                }

                error1.coord = xywh(0,   h+q+h, W/2, h);
                error2.coord = xywh(W/2, h+q+h, W/2, h);
                error1.canvas.color = schema.error.first;
                error2.canvas.color = schema.error.second;
                error1.color = schema.error.second;
                error2.color = schema.error.first;
                error1.text = "error";
                error2.text = "error";

                page.skin = name;
                page.coord = xyxy(0,h+q+h+h, W, H);
                page.object.html = LOrem;
                page.object.alignment = xy{pix::left, pix::top};
                page.object.canvas.color = schema.ultralight.first;
                page.object.style = pix::text::style{
                    pix::font{"Segoe UI"},
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
                samples(i).coord = xywh(
                x*side, y*side, side, side);
            }
        }
    }
};

struct TestGuiFormat:
widget<TestGuiFormat>
{
    gui::widgetarium<gui::area<gui::text::page>> pages;
    gui::widgetarium<gui::canvas> wraps;

    void on_change (void* what) override
    {
        const int n = 3;

        if (what == &alpha
        and alpha.now == 255
        and pages.empty())
        {
            for (int i=0; i<2; i++)
            for (int j=0; j<n; j++) {
            pages.emplace_back().object.alignment = xy{pix::left,         pix::top};
            pages.emplace_back().object.alignment = xy{pix::center,       pix::top};
            pages.emplace_back().object.alignment = xy{pix::justify_left, pix::top};
            pages.emplace_back().object.alignment = xy{pix::right,        pix::top};
            }
            auto schema = gui::skins[skin];

            for (auto& page : pages)
            {
                page.object.html = Lorem;
                page.object.canvas.color = schema.ultralight.first;
                page.object.style = pix::text::style{
                    pix::font{"Segoe UI",
                    gui::metrics::text::height},
                    schema.dark.first};
            }
            what = &coord;
        }
        if (what == &coord and not pages.empty())
        {
            int W = coord.now.w; if (W <= 0) return;
            int H = coord.now.h; if (H <= 0) return;
            int w = W / 4;
            int h = H / 2 / n;

            for (int j=0; j<n; j++) { int v = w/(j+1);
            pages(1*4*n+j*4+0).object.lwrap = array<xy>{xy(0, h/2), xy(v/4, h/4)};
            pages(1*4*n+j*4+1).object.lwrap = array<xy>{xy(0, h/2), xy(v/4, h/4)};
            pages(1*4*n+j*4+2).object.lwrap = array<xy>{xy(0, h/2), xy(v/4, h/4)};
            pages(1*4*n+j*4+3).object.lwrap = array<xy>{xy(0, h/2), xy(v/4, h/4)};
            pages(1*4*n+j*4+0).object.rwrap = array<xy>{xy(v/4, h/4)};
            pages(1*4*n+j*4+1).object.rwrap = array<xy>{xy(v/4, h/4)};
            pages(1*4*n+j*4+2).object.rwrap = array<xy>{xy(v/4, h/4)};
            pages(1*4*n+j*4+3).object.rwrap = array<xy>{xy(v/4, h/4)};
            }
            for (int i=0; i<2; i++)
            for (int j=0; j<n; j++) { int v = w/(j+1);
            pages(i*4*n+j*4+0).coord = xywh(w*0, i*h*n+j*h, v, h);
            pages(i*4*n+j*4+1).coord = xywh(w*1, i*h*n+j*h, v, h);
            pages(i*4*n+j*4+2).coord = xywh(w*2, i*h*n+j*h, v, h);
            pages(i*4*n+j*4+3).coord = xywh(w*3, i*h*n+j*h, v, h);
            }
            pages.coord = xywh(0,0,W,H);
            wraps.coord = xywh(0,0,W,H);
            wraps.clear();

            for (auto& page : pages)
            {
                int y = 0;
                for (auto d : page.object.lwrap.now) {
                wraps.emplace_back().color = rgba(255, 0, 0, 128);
                wraps.back().coord = xywh(0, y, d.x, d.y) +
                    page.object.coord.now.origin +
                    page.coord.now.origin;
                y += d.y; }
                y = 0;
                int v = page.object.view.coord.now.w;
                for (auto d : page.object.rwrap.now) {
                wraps.emplace_back().color = rgba(0, 0, 255, 128);
                wraps.back().coord = xywh(v-d.x, y, d.x, d.y) +
                    page.object.coord.now.origin +
                    page.coord.now.origin;
                y += d.y; }
            }
        }
    }
};

struct TestGuiConsole:
widget<TestGuiConsole>
{
    str text = Lorem;
    gui::button doubling;
    gui::area<gui::console> console1;
    gui::area<gui::console> console2;
    gui::splitter splitter;
    int x = 40'00;
    int xx = 80;

    TestGuiConsole () { on_change(&doubling); }

    void on_change (void* what) override
    {
        if (what == &skin)
        {
            console1.object.canvas.color = rgba::white;
            console2.object.canvas.color = rgba::white;
        }
        if (what == &coord)
        {
            int W = coord.now.w; if (W <= 0) return;
            int H = coord.now.h; if (H <= 0) return;
            int w = gui::metrics::text::height*xx;
            int h = gui::metrics::text::height*13/10;
            int d = gui::metrics::line::width * 6;
            int m = w * x / 100'00;
            doubling.coord = xywh(w-0, 0, w/9, h);
            console1.coord = xyxy(0-0, 0, m+0, H);
            console2.coord = xyxy(m-0, 0, w+0, H);
            splitter.coord = xyxy(m-d, 0, m+d, H);
            splitter.lower = 20'00 * w / 100'00;
            splitter.upper = 80'00 * w / 100'00;
        }
        if (what == &splitter)
        {
            int w = gui::metrics::text::height*xx;
            x = 100'00 * splitter.middle / w;
            on_change(&coord);
        }
        if (what == &doubling)
        {
            doubling.text.text = "double text";
            console1.object.page.html += text;
            console2.object.page.html += text;
            text += text;
        }
    }
};

struct TestGuiAnimat:
widget<TestGuiAnimat>
{
    struct quad: gui::canvas
    {
        array<int> xs;
        void on_change (void* what) override {
        if (what == &coord) xs += coord.now.x;
        gui::canvas::on_change(what); }
    };

    quad quad;
    gui::text::view view, text;
    gui::line line, lin1;
    gui::oval oval;
    gui::button go1, go2, go3, go4, go5, go6; 
    gui::property<gui::time> timer;
    gui::console log;
    gui::time lapse;
    int turn = 0;

    void on_change (void* what) override
    {
        if (what == &coord and
            coord.was.size !=
            coord.now.size)
        {
            int W = coord.now.w; if (W <= 0) return;
            int H = coord.now.h; if (H <= 0) return;
            int w = gui::metrics::text::height*10;
            int h = gui::metrics::text::height*13/10;
            int d = gui::metrics::line::width * 6;

            view.text = "text";
            view.style = pix::text::style{
                pix::font{"Segoe UI",
                gui::metrics::text::height*2},
                rgba::black};

            text.html = Lorem;
            text.style = pix::text::style{
                pix::font{"Segoe UI",
                gui::metrics::text::height},
                rgba::black};

            go1.text.text = "go!";
            go2.text.text = "go!!";
            go3.text.text = "go!!!";
            go4.text.text = "go!!!!";
            go5.text.text = "go!!!!!";
            go6.text.text = "go!!!!!!";
            go1.coord = xywh(W-w, h*0, w, h);
            go2.coord = xywh(W-w, h*1, w, h);
            go3.coord = xywh(W-w, h*2, w, h);
            go4.coord = xywh(W-w, h*3, w, h);
            go5.coord = xywh(W-w, h*4, w, h);
            go6.coord = xywh(W-w, h*5, w, h);
            log.coord = xywh(W-w, h*6, w, H-h*6);
            on_change(&go2);
        }

        bool go = false;

        if (what == &go1) { go = true; lapse =18s; }
        if (what == &go2) { go = true; lapse = 6s; }
        if (what == &go3) { go = true; lapse = 3s; }
        if (what == &go4) { go = true; lapse = 1s; }
        if (what == &go5) { go = true; lapse = 500ms; }
        if (what == &go6) { go = true; lapse = 200ms; }

        if (go)
        {
            int W = coord.now.w; if (W <= 0) return;
            int H = coord.now.h; if (H <= 0) return;
            int w = gui::metrics::text::height*10;
            int h = gui::metrics::text::height*12/7;

            log.clear();
            quad.xs.clear();
            timer.go(lapse+200ms, lapse+200ms);

            switch (turn) {
            break; case 0:
                quad.color.go(rgba::black, lapse);
                view.color.go(rgba::black, lapse);
                text.color.go(rgba::green, lapse);
                view.canvas.color.go(rgba::white,  lapse);
                text.canvas.color.go(rgba::silver, lapse);
                quad.coord.go(xywh(0, 0*h, 1*w, 1*h), lapse);
                view.coord.go(xywh(0, 1*h, 2*w, 2*h), lapse);
                text.coord.go(xywh(0, 3*h, 3*w, 7*h), lapse);
                line.color.go(rgba::black, lapse);
                lin1.color.go(rgba::black, lapse);
                line.width.go(1.0, lapse);
                line.x1.go( 0*w, lapse);
                lin1.x2.go( 0*w, lapse);
                line.y1.go( 3*h, lapse);
                lin1.y1.go( 3*h, lapse);
                line.x2.go( 3*w, lapse);
                lin1.x1.go( 3*w, lapse);
                line.y2.go(10*h, lapse);
                lin1.y2.go(10*h, lapse);
                oval.color.go(rgba::black, lapse);
                oval.x.go( 3*h, lapse);
                oval.y.go( 2*h + 10*h, lapse);
                oval.rx.go( 2*h, lapse);
                oval.ry.go( 1*h, lapse);
                oval.rx2.go( 3*h, lapse);
                oval.ry2.go( 2*h, lapse);
            break; default:
                quad.color.go(rgba::olive,  lapse);
                view.color.go(rgba::white,  lapse);
                text.color.go(rgba::yellow, lapse);
                view.canvas.color.go(rgba::black, lapse);
                text.canvas.color.go(rgba::green, lapse);
                quad.coord.go(xywh(W-3*w, 0*h, 2*w, 2*h), lapse);
                view.coord.go(xywh(W-5*w, 2*h, 4*w, 4*h), lapse);
                text.coord.go(xywh(W-3*w, 6*h, 2*w,11*h), lapse);
                line.color.go(rgba::white, lapse);
                lin1.color.go(rgba::white, lapse);
                line.width.go(5.0, lapse);
                line.x1.go(W-3*w, lapse);
                lin1.x2.go(W-3*w, lapse);
                line.y1.go(  6*h, lapse);
                lin1.y1.go(  6*h, lapse);
                line.x2.go(W-1*w, lapse);
                lin1.x1.go(W-1*w, lapse);
                line.y2.go( 17*h, lapse);
                lin1.y2.go( 17*h, lapse);
                oval.color.go(rgba::white, lapse);
                oval.x.go(W-w-2*h, lapse);
                oval.y.go(3*h + 17*h, lapse);
                oval.rx.go(1*h, lapse);
                oval.ry.go(2*h, lapse);
                oval.rx2.go(2*h, lapse);
                oval.ry2.go(3*h, lapse);
            }

            turn = (turn + 1) % 2;
        }

        if (what == &timer and
            timer.now == timer.to)
        {
            int n = quad.xs.size();
            if (n <= 0) return;

            int ms = int(lapse.ms);
            double sec = ms/1000.0;
            int fps = int(n/sec);

            for (int x: quad.xs)
            log << std::to_string(x);
            log << "<br>";
            log << std::to_string(n) + " times";
            log << std::to_string(ms) + " ms";
            log << std::to_string(sec) + " sec";
            log << std::to_string(fps) + " fps";
        }
    }
};
