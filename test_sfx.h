#pragma once
#include "aux_unittest.h"
#include "sfx_trees_bst.h"
#include "sfx_trees_bbst_avl.h"
#include "sfx_trees_bbst_splay.h"
#include "sfx_trees_bbst_redblack.h"
using namespace std::literals::chrono_literals;
using namespace pix;
using gui::widget;

struct TestSfx:
widget<TestSfx>
{
};

struct TestSfxTrees:
widget<TestSfxTrees>
{
    gui::canvas canvas;
    gui::radio::group buttons;
    gui::button zoom , speed ;
    gui::button zoom_, speed_;
    gui::button x_0_99;
    gui::button x_0_999;
    gui::button uniform;
    gui::button gaussian;
    gui::button clear;
    gui::image Image;
    pix::image<rgba> image;
    sfx::trees::binary::bst dynamic;
    sfx::trees::binary::static_bst tree;
    sfx::trees::binary::unbalanced unbalanced;
    sfx::trees::binary::redblack redblack;
    sfx::trees::binary::splay splay;
    sfx::trees::binary::avl avl;
    sfx::trees::binary::bst* bst = nullptr;

    void on_change (void* what) override
    {
        if (what == &coord and
            coord.was.size !=
            coord.now.size)
        {
            int W = coord.now.w; if (W <= 0) return;
            int H = coord.now.h; if (H <= 0) return;
            int w = gui::metrics::text::height*10;
            int h = gui::metrics::text::height*12/7;

            if (buttons.empty())
            {
                int i = 0;
                buttons(i++).text.text = "tree";
                buttons(i++).text.text ="+tree+";
                buttons(i++).text.text = "dynamic";
                buttons(i++).text.text = "unbalanced";
                buttons(i++).text.text = "red-black";
                buttons(i++).text.text = "splay";
                buttons(i++).text.text = "AVL";
                buttons(i++).text.text = "pause";
                buttons.front().on = true;
                canvas.color =
                rgba::black;
            }

            int y = 0; for (auto& button: buttons) {
            button.coord = xywh(0,y,w,h); y += h; }
            buttons.coord = xywh(W-w,0,w,H);
            canvas.coord = xywh(0,0,W,H);
            auto r = xywh(0,0,W-w,H);
            tree.coord = r;
            dynamic.coord = r;
            unbalanced.coord = r;
            redblack.coord = r;
            splay.coord = r;
            avl.coord = r;

            clear   .coord = xywh(W-w,H-9*h,w,h);
            uniform .coord = xywh(W-w,H-8*h,w,h);
            gaussian.coord = xywh(W-w,H-7*h,w,h);
            x_0_99  .coord = xywh(W-w,H-6*h,w,h);
            x_0_999 .coord = xywh(W-w,H-5*h,w,h);
            zoom    .coord = xywh(W-w,H-4*h,w,h);
            zoom_   .coord = xywh(W-w,H-3*h,w,h);
            speed   .coord = xywh(W-w,H-2*h,w,h);
            speed_  .coord = xywh(W-w,H-1*h,w,h);
            clear   .text.text = "clear";
            uniform .text.text = "uniform";
            gaussian.text.text = "gaussian";
            x_0_99  .text.text = "0 - 99";
            x_0_999 .text.text = "0 - 999";
            zoom    .text.text = "zoom+";
            zoom_   .text.text = "zoom-";
            speed   .text.text = "speed+";
            speed_  .text.text = "speed-";
            zoom    .repeat_lapse = 0ms;
            zoom_   .repeat_lapse = 0ms;
            speed   .repeat_lapse = 0ms;
            speed_  .repeat_lapse = 0ms;
            zoom    .repeating = true;
            zoom_   .repeating = true;
            speed   .repeating = true;
            speed_  .repeating = true;
        }

        if (what == &buttons)
        {
            str text = buttons.notifier->text.text;
            if (text == "tree")
            {
                tree.clear();
                for (int i=0; i<500; i++)
                tree.insert(aux::random(0,99));
                tree.place();

            }
            if (text == "+tree+") if (image.size == xy{})
            {
                xywh r = tree.coord.now;
                r.h /= 2; r.y += r.h;
                image.resize(r.size);
                Image.coord = r;
                Image.source = image.crop();
                auto frame = image.crop();
                frame.fill(rgba::black);

                for (auto& node: tree.post_order())
                {
                    frame.blend(pix::line{
                        {node->edge.x1.now,
                         node->edge.y1.now},
                        {node->edge.x2.now,
                         node->edge.y2.now}},
                        rgba::white, 2.0);
                    frame.crop(node->coord.now +
                    node->value.coord.now.origin).
                    blend_from(pix::util::node(
                        node->value.text,
                        rgba::red,
                        rgba::white,
                        rgba::navy,
                        node->value.coord.now.w)
                        .crop());
                }
            }

            Image.show(text == "tree" or text == "+tree+");
            tree .show(text == "tree" or text == "+tree+");

            if (text == "tree"      ) bst = nullptr;
            if (text == "+tree+"    ) bst = nullptr;
            if (text == "dynamic"   ) bst = &dynamic;
            if (text == "unbalanced") bst = &unbalanced;
            if (text == "red-black" ) bst = &redblack;
            if (text == "splay"     ) bst = &splay;
            if (text == "AVL"       ) bst = &avl;

            dynamic   .show(bst == &dynamic);
            unbalanced.show(bst == &unbalanced);
            redblack  .show(bst == &redblack);
            splay     .show(bst == &splay);
            avl       .show(bst == &avl);

            if (text == "dynamic"
            or  text == "unbalanced"
            or  text == "red-black"
            or  text == "splay"
            or  text == "AVL")
            {
                bst->pause = false;
            }
            if (text == "pause" and bst)
            {
                bst->pause = true;
            }
        }

        if (bst)
        {
            int d = gui::metrics::text::height;
            if (what == &zoom    ) bst->side  = min(bst->side+1, d*2);
            if (what == &zoom_   ) bst->side  = max(bst->side-1, d/3);
            if (what == &speed   ) bst->speed = min(bst->speed*1.1, 10.0);
            if (what == &speed_  ) bst->speed = max(bst->speed*0.9, 0.01);
            if (what == &uniform ) bst->queue.distribution = "random";
            if (what == &gaussian) bst->queue.distribution = "normal";
            if (what == &x_0_99  ) bst->queue.upper = 99;
            if (what == &x_0_999 ) bst->queue.upper = 999;
            if (what == &clear   ) bst->clear();
        }
    }
};