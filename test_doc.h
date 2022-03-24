#pragma once
#include "gui_widget_console.h"
#include "doc_text_model_a.h"
#include "doc_text_model_b.h"
using namespace pix;
using gui::widget;

struct TestDoc:
widget<TestDoc>
{
    bool ok = true;
    bool done = false;
    gui::canvas canvas;
    gui::area<gui::console> console1;
    gui::area<gui::console> console2;
    gui::area<gui::console> console3;
    void on_change (void* what) override
    {
        if (what == &coord and
            coord.was.size !=
            coord.now.size)
        {
            int W = coord.now.w; if (W <= 0) return; int w = W/3;
            int H = coord.now.h; if (H <= 0) return;
            console1.coord = XYWH(w*0, 0, w, H);
            console2.coord = XYWH(w*1, 0, w, H);
            console3.coord = XYWH(w*2, 0, w, H);

            if (done) return; done = true;

            auto style = pix::text::style{
            pix::font{"Consolas"}, RGBA::black};
            console1.object.page.style = style;
            console2.object.page.style = style;
            console3.object.page.style = style;

            aux::unittest::text_model_a();
            aux::unittest::test("");
            console1.object.page.html = 
            aux::unittest::results; ok &= 
            aux::unittest::all_ok;

            aux::unittest::text_model_b();
            aux::unittest::test("");
            console2.object.page.html = 
            aux::unittest::results; ok &= 
            aux::unittest::all_ok;

            aux::unittest::test("-");
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

struct TestDocHtml:
widget<TestDocHtml>
{
    gui::radio::group buttons;
    gui::widgetarium<gui::area<gui::text::page>> source;
    gui::widgetarium<gui::area<gui::text::page>> pretty;
    gui::widgetarium<gui::area<gui::text::page>> entity;
    gui::widgetarium<gui::area<gui::text::page>> result;

    void on_change(void* what) override
    {
        const int n = 3;

        if (what == &alpha
        and alpha.now == 255
        and buttons.empty())
        {
            auto schema = gui::skins[skin];
            auto style = pix::text::style{
                pix::font{"Consolas",
                gui::metrics::text::height},
                schema.dark.first};

            for (int i=0; i<n; i++)
            {
                auto& src = source[i].object;
                auto& pre = pretty[i].object;
                auto& ent = entity[i].object;
                auto& res = result[i].object;
                src.alignment = XY{pix::left, pix::top};
                pre.alignment = XY{pix::left, pix::top};
                ent.alignment = XY{pix::left, pix::top};
                res.alignment = XY{pix::left, pix::top};
                src.canvas.color = schema.white;
                pre.canvas.color = schema.ultralight.first;
                ent.canvas.color = schema.ultralight.first;
                res.canvas.color = schema.ultralight.first;
                src.style = style;
                pre.style = style;
                ent.style = style;
            }
            buttons.emplace_back().text.text = "1";
            buttons.emplace_back().text.text = "2";
            buttons(0).on = true;
            on_change(&coord);
            on_change(&buttons);
        }
        if (what == &coord and not buttons.empty())
        {
            int W = coord.now.w; if (W <= 0) return;
            int H = coord.now.h; if (H <= 0) return;
            int bw = gui::metrics::text::height*10;
            int bh = gui::metrics::text::height*12/7;
            int w = W / 4;
            int h = (H-bh) / n;

            source.coord = XYWH(0*w, bh, w, H);
            pretty.coord = XYWH(1*w, bh, w, H);
            entity.coord = XYWH(2*w, bh, w, H);
            result.coord = XYWH(3*w, bh, w, H);

            for (int i=0; i<n; i++) {
            source[i].coord = XYWH(0, i*h, w, h);
            pretty[i].coord = XYWH(0, i*h, w, h);
            entity[i].coord = XYWH(0, i*h, w, h);
            result[i].coord = XYWH(0, i*h, w, h);
            }
            int x = 0;
            for (auto& button: buttons) {
                button.coord = XYWH(x, 0, bw, bh);
                x += bw; }

            buttons.coord = XYWH(0, 0, W, bh);
        }
        if (what == &buttons)
        {
            str ss[n];
            auto& x = ss[0];
            auto& y = ss[1];
            auto& z = ss[2];
            switch (buttons.notifier_index) {
            break; case 0:
            x =
            "normal<br>\n"
            "<b>bold</b><br>\n"
            "<i>italic</i><br>\n"
            "<b><i>bold-italic</i></b><br>\n"
            ;
            y =
            "<div style=\"line-height: 110%\">\n div style=\"line-height: 110%\"<br></div>\n"
            "<div style=\"line-height: 100%\">\n div style=\"line-height: 100%\"<br></div>\n"
            "<div style=\"line-height:  90%\">\n div style=\"line-height:  90%\"<br></div>\n"
            "<div style=\"line-height:  80%\">\n div style=\"line-height:  80%\"<br></div>\n"
            "line gap 110%<br>\n<div style=\"line-height: 110%\"><br></div>\n"
            "line gap 100%<br>\n<div style=\"line-height: 100%\"><br></div>\n"
            "line gap  90%<br>\n<div style=\"line-height:  90%\"><br></div>\n"
            "line gap  80%<br>\n<div style=\"line-height:  80%\"><br></div>\n"
            "line gap  end<br>\n"
            ;
            z =
            "default 3";
            break; default: 
            x = "default 1";
            y = "default 2";
            z = "default 3";
            }
            for (int i = 0; i < n; i++)
            {
                source[i].object.text = ss[i];
                pretty[i].object.text = doc::html::print(ss[i]);
                result[i].object.html = ss[i];

                array<str> ll;
                auto& lines = 
                result[i].object.view.cell.lines;

                int l = 0;
                for (auto& line: lines) {
                    ll += "line " +
                        std::to_string(l) + "(" +
                        std::to_string(line.coord.now.x) + "," +
                        std::to_string(line.coord.now.y) + ")-[" +
                        std::to_string(line.coord.now.w) + "," +
                        std::to_string(line.coord.now.h) + "]:";
                    l++;
                    int r = 0;
                    for (auto& row: line.rows) {
                        ll += "  row " +
                            std::to_string(r) + "(" +
                            std::to_string(row.offset.x) + "," +
                            std::to_string(row.offset.y) + ")-[" +
                            std::to_string(row.width) + "," +
                            std::to_string(row.ascent + row.descent) + "]: ";
                        r++;
                        for (auto& solid: row.solids)
                            for (auto& token: solid.tokens)
                                ll.back() += token.text;
                    }
                }

                entity[i].object.text = str(ll);
            }
        }
    }
};
