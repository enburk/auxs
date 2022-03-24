#pragma once
#include "gui_widget_console.h"
using namespace pix;
using gui::widget;

struct TestPix:
widget<TestPix>
{
};

struct TestPixFonts:
widget<TestPixFonts>
{
    gui::image gui_image;
    pix::image<RGBA> image;

    void on_change (void* what) override
    {
        if (what != &alpha or alpha.to == 0 or
            coord.now.size == image.size or
            coord.now.size == XY())
            return;

        image.resize(coord.now.size); image.fill(RGBA::red);
        gui_image.coord = coord.now.local();
        gui_image.source = image.crop();

        str digit = "0123456789";
        str Latin = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        str latin = "abcdefghijklmnopqrstuvwxyz";
        str alnum = Latin + latin + digit;
        array<
        pix::font> fonts = {
        pix::font{"Consolas", gui::metrics::text::height*5/2},
        pix::font{"Arial",    gui::metrics::text::height*5/2},
        pix::font{"Segoe UI", gui::metrics::text::height*5/2},
        pix::font{"Tahoma",   gui::metrics::text::height*5/2}};

        int gap = 1; int x = gap; int y = gap;

        for (auto font : fonts)
        for (int r=0; r<4; r++, x = gap, y += gap +
            pix::metrics(font).ascent  +
            pix::metrics(font).descent +
            pix::metrics(font).linegap)
        {
            pix::text::style style;
            style.color = RGBA::black;
            style.color.a = 230;
            style.font = font;
            style.font.bold   = r == 2 || r == 3;
            style.font.italic = r == 1 || r == 3;

            for (char c : alnum) {
                auto glyph = pix::glyph(str(c), style);
                auto w = glyph.width;
                auto h = glyph.ascent + glyph.descent;
                auto frame = image.crop(XYWH(x, y, w, h));
                frame.blend(RGBA::white, 200);
                glyph.render(frame, XY(), 200, x);
                x += glyph.advance + gap;
            }
        }
    }
};

struct TestPixDraw:
widget<TestPixDraw>
{
    gui::image Image;
    pix::image<RGBA> image;

    void on_change (void* what) override
    {
        if (what != &alpha or alpha.to == 0 or
            coord.now.size == image.size or
            coord.now.size == XY())
            return;

        image.resize(coord.now.size);
        Image.coord = coord.now.local();
        Image.source = image.crop();
        image.fill(RGBA::black);

    }
};

