#pragma once
#include "aux_unittest.h"
#include "gui_widget_console.h"
#include "test_doc_text_a.h"
#include "test_doc_text_b.h"
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
            console1.coord = xywh(w*0, 0, w, H);
            console2.coord = xywh(w*1, 0, w, H);
            console3.coord = xywh(w*2, 0, w, H);

            if (done) return; done = true;

            auto style = pix::text::style{
            pix::font{"Consolas"}, rgba::black};
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
    static inline array<str> htmls = 
    {
        "normal<br>\n"
        "<b>bold</b><br>\n"
        "<i>italic</i><br>\n"
        "<b><i>bold-italic</i></b><br>\n"
        ,
        "<div style=\"line-height: 110%\">\n div style=\"line-height: 110%\"<br></div>\n"
        "<div style=\"line-height: 100%\">\n div style=\"line-height: 100%\"<br></div>\n"
        "<div style=\"line-height:  90%\">\n div style=\"line-height:  90%\"<br></div>\n"
        "<div style=\"line-height:  80%\">\n div style=\"line-height:  80%\"<br></div>\n"
        " line gap 110%<br>\n<div style=\"line-height: 110%\"><br></div>\n"
        " line gap 100%<br>\n<div style=\"line-height: 100%\"><br></div>\n"
        " line gap  90%<br>\n<div style=\"line-height:  90%\"><br></div>\n"
        " line gap  80%<br>\n<div style=\"line-height:  80%\"><br></div>\n"
        " line gap  70%<br>\n<div style=\"line-height:  70%\"><br></div>\n"
        " line gap  60%<br>\n<div style=\"line-height:  60%\"><br></div>\n"
        " line gap  50%<br>\n<div style=\"line-height:  50%\"><br></div>\n"
        " line gap  end<br>\n"
    };
    enum { n = 3 };
    gui::radio::group buttons;
    gui::area<gui::text::page> source[n];
    gui::area<gui::text::page> pretty[n];
    gui::area<gui::text::page> entity[n];
    gui::area<gui::text::page> result[n];

    void on_change(void* what) override
    {

        if (what == &coord and
            coord.was.size !=
            coord.now.size)
        {
            int W = coord.now.w; if (W <= 0) return;
            int H = coord.now.h; if (H <= 0) return;
            int bw = gui::metrics::text::height*10;
            int bh = gui::metrics::text::height*13/10;
            int w = W / 4;
            int h = (H-bh) / n;

            auto schema = gui::skins[skin];
            auto style = pix::text::style{
                pix::font{"Consolas"},
                schema.dark.first};

            for (int i=0; i<n; i++)
            {
                source[i].coord = xywh(0*w, i*h+bh, w, h);
                pretty[i].coord = xywh(1*w, i*h+bh, w, h);
                entity[i].coord = xywh(2*w, i*h+bh, w, h);
                result[i].coord = xywh(3*w, i*h+bh, w, h);
                auto& src = source[i].object;
                auto& pre = pretty[i].object;
                auto& ent = entity[i].object;
                auto& res = result[i].object;
                src.alignment = xy{pix::left, pix::top};
                pre.alignment = xy{pix::left, pix::top};
                ent.alignment = xy{pix::left, pix::top};
                res.alignment = xy{pix::left, pix::top};
                src.canvas.color = schema.white;
                pre.canvas.color = schema.ultralight.first;
                ent.canvas.color = schema.ultralight.first;
                res.canvas.color = schema.ultralight.first;
                src.style = style;
                pre.style = style;
                ent.style = style;
            }
            if (buttons.empty())
            {
                for (int i=0; i*3 < htmls.size(); i++)
                buttons.emplace_back().text.text = std::to_string(i+1);
                buttons(0).on = true;
                on_change(&buttons);
            }
            int x = 0;
            for (auto& button: buttons) {
            button.coord = xywh(x, 0, bw, bh); x += bw; }
            buttons.coord = xywh(0, 0, W, bh);
        }
        if (what == &buttons)
        {
            int i = buttons.notifier_index;

            for (int j=0; j<n; j++)
            {
                int k = i*3 + j;
                str html = k < htmls.size() ? htmls[k] : "";

                source[j].object.text = html;
                pretty[j].object.text = doc::html::print(html);
                result[j].object.html = html;

                array<str> ll; auto& lines = 
                result[j].object.view.cell.box.model->block.lines;

                int l = 0;
                for (auto& line: lines) {
                    ll += "line " + std::to_string(l++) + ":" + " padding = "
                     + std::to_string(line.padding.left ) + ","
                     + std::to_string(line.padding.right) + ","
                     + std::to_string(line.padding.first) + " " + " font.size = "
                     + std::to_string(line.style.style().font.size);
                    int r = 0;
                    for (auto& row: line.rows) {
                        ll += "  row " +
                            std::to_string(r) + " (" +
                            std::to_string(row.offset.x) + "," +
                            std::to_string(row.offset.y) + ")-[" +
                            std::to_string(row.Width ()) + "," +
                            std::to_string(row.Height()) + "]: ";
                        r++;
                        for (auto& solid: row.solids)
                        for (auto& token: solid.tokens)
                        ll.back() += token.text;
                    }
                }

                entity[j].object.text = str(ll);
            }
        }
    }
};
