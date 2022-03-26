#pragma once
#include "gui_widget_console.h"
using namespace pix;
using gui::widget;

struct TestPix:
widget<TestPix>
{
};

struct TestPixDraw:
widget<TestPixDraw>
{
    gui::image Image;
    pix::image<rgba> image;

    void on_change (void* what) override
    {
        if (what != &alpha or alpha.to == 0 or
            coord.now.size == image.size or
            coord.now.size == xy())
            return;

        image.resize(coord.now.size);
        Image.coord = coord.now.local();
        Image.source = image.crop();

        int w = image.size.x;
        int h = image.size.y;

        pix::image<rgba> plane;
        plane.resize(image.size);
        image.fill(rgba::black);
        plane.fill(rgba{});

        auto frame1 = image.crop(xyxy(0,0,w,h/2));
        auto frame2 = plane.crop(xyxy(0,h/2,w,h));

        int n = 32;
        int r = h/4;
        for (double a = 0; a < 2*pi-0.001; a += 2*pi/n)
        {
            frame1.blend(pix::line{{r,r},
                pix::vector{r,0}.rotated(-a) +
                pix::vector{r,r}}, rgba::white,
                0.1 + 1.9*a/2/pi);
            frame2.copy(pix::line{{r,r},
                pix::vector{r,0}.rotated(-a) +
                pix::vector{r,r}}, rgba::white,
                2.0 - 1.9*a/2/pi);

            frame1.blend(pix::circle{{3*r,r},
                r - r*a/2/pi}, rgba::white,
                2.0 - 1.9*a/2/pi);
            frame2.copy(pix::circle{{3*r,r},
                r - r*a/2/pi}, rgba::white,
                2.0 - 1.9*a/2/pi);
        }

        image.crop().blend_from(
        plane.crop());
    }
};

struct TestPixFonts:
widget<TestPixFonts>
{
    gui::image gui_image;
    pix::image<rgba> image;

    void on_change (void* what) override
    {
        if (what != &alpha or alpha.to == 0 or
            coord.now.size == image.size or
            coord.now.size == xy())
            return;

        image.resize(coord.now.size); image.fill(rgba::red);
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
            style.color = rgba::black;
            style.color.a = 230;
            style.font = font;
            style.font.bold   = r == 2 || r == 3;
            style.font.italic = r == 1 || r == 3;

            for (char c : alnum) {
                auto glyph = pix::glyph(str(c), style);
                auto w = glyph.width;
                auto h = glyph.ascent + glyph.descent;
                auto frame = image.crop(xywh(x, y, w, h));
                frame.blend(rgba::white, 200);
                glyph.render(frame, xy(), 200, x);
                x += glyph.advance + gap;
            }
        }
    }
};

