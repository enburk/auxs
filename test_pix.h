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
        if (coord.now.size == image.size or
            coord.now.size == xy())
            return;

        image.resize(coord.now.size);
        Image.coord = coord.now.local();
        Image.source = image.crop();

        pix::image<rgba> plane;
        plane.resize(image.size);
        image.fill(rgba::black);
        plane.fill(rgba{});

        int w = image.size.x;
        int h = image.size.y/4;
        auto frame1 = image.crop(xywh(0,0,w,h));
        auto frame2 = plane.crop(xywh(0,h,w,h));

        int n = 32;
        int r = h/2;
        array<std::jthread> threads;
        for (double a = 0; a < 2*pi-0.001; a += 2*pi/n)
        {
            auto w1 = 0.1 + 1.9*a/2/pi;
            auto w2 = 2.0 - 1.9*a/2/pi;
            auto rr = r - r*a/2/pi;
            auto cc = int(255*a/2/pi);

            frame1.blend(line{{r,r},
                vector{r,0}.rotated(-a) +
                vector{r,r}}, rgba::white, w1);
            frame2.copy (line{{r,r},
                vector{r,0}.rotated(-a) +
                vector{r,r}}, rgba::white, w2);

            threads += std::jthread([&frame1,&frame2,r,rr,w1,w2](){
            frame1.blend(circle{{3*r,r}, rr},
                rgba::white, w1);
            frame2.copy (circle{{3*r,r}, rr},
                rgba::white, w2);
            });

            frame1.blend(circle{{5*r,r}, rr},
                rgba(cc,0,0));
            frame2.copy (circle{{5*r,r}, rr},
                rgba(255 - cc,0,0));
        }
        for (auto& t: threads) t.join();
        image.crop().blend_from(
        plane.crop());
    }
};

struct TestPixDrawX:
widget<TestPixDrawX>
{
    gui::image Image;
    pix::image<rgba> image;

    void on_change (void* what) override
    {
        if (coord.now.size == image.size or
            coord.now.size == xy())
            return;

        image.resize(coord.now.size);
        Image.coord = coord.now.local();
        Image.source = image.crop();

        pix::image<rgba> plane;
        plane.resize(image.size);
        image.fill(rgba::black);
        plane.fill(rgba{});

        int w = image.size.x;
        int h = image.size.y/4;
        int u = gui::metrics::text::height;

        auto frame1 = image.crop(xywh(0,0,w,h));
        auto frame2 = plane.crop(xywh(0,h,w,h));

        int q = h/2; int c = q/2;
        int r = q/3; int e = q-r;
        rgba cc[3] = {
        rgba(255,0,0,128),
        rgba(0,255,0,128),
        rgba(0,0,255,128)};
        aux::vector<2> oo[3] = {{c,r},{r,e},{e,e}};
        auto frame1a = frame1.crop(xywh(0,0,q,q));
        auto frame1b = frame1.crop(xywh(0,q,q,q));
        auto frame2a = frame2.crop(xywh(0,0,q,q));
        auto frame2b = frame2.crop(xywh(0,q,q,q));
        frame1a.fill(rgba::white);
        frame2a.fill(rgba::white);
        frame1a.blend(circle{oo[0],double(r)}, cc[0]);
        frame1b.blend(circle{oo[0],double(r)}, cc[0]);
        frame2a.copy (circle{oo[0],double(r)}, cc[0]);
        frame2b.copy (circle{oo[0],double(r)}, cc[0]);
        frame1a.blend(circle{oo[1],double(r)}, cc[1]);
        frame1b.blend(circle{oo[1],double(r)}, cc[1]);
        frame2a.copy (circle{oo[1],double(r)}, cc[1]);
        frame2b.copy (circle{oo[1],double(r)}, cc[1]);
        frame1a.blend(circle{oo[2],double(r)}, cc[2]);
        frame1b.blend(circle{oo[2],double(r)}, cc[2]);
        frame2a.copy (circle{oo[2],double(r)}, cc[2]);
        frame2b.copy (circle{oo[2],double(r)}, cc[2]);
        for (int k=0; k<3; k++)
        {
            array<std::jthread> threads;
            for (int i = 1; i <= 4; i++) {
            for (int j = 1; j <= 4; j++) {
                threads += std::jthread([frame1,frame2,i,j,k,q,r,oo,cc](){
                    auto f = 
                    j == 1 ? frame1.crop(xywh(i*q,0,q,q)):
                    j == 2 ? frame1.crop(xywh(i*q,q,q,q)):
                    j == 3 ? frame2.crop(xywh(i*q,0,q,q)):
                    j == 4 ? frame2.crop(xywh(i*q,q,q,q)):
                    frame1;
                    if (k == 0) if (j == 1 or j == 3) f.fill(rgba::white);
                    for (int R = r; R > 0; R -= 1 << (i-1)) if (j == 1 or j == 2)
                    f.blend(circle{oo[k],double(R)}, cc[k], 2.0); else
                    f.copy (circle{oo[k],double(R)}, cc[k], 2.0);
                });
            }}
            for (auto& t: threads) t.join();
        }
        image.crop().blend_from(
        plane.crop());
    }
};

struct TestPixFonts:
widget<TestPixFonts>
{
    gui::image Image;
    pix::image<rgba> image;

    void on_change (void* what) override
    {
        if (coord.now.size == image.size or
            coord.now.size == xy())
            return;

        image.resize(coord.now.size);
        image.fill(rgba::red);
        Image.coord = coord.now.local();
        Image.source = image.crop();

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
                auto glyph = pix::glyph(c, style);
                auto w = glyph.Width();
                auto h = glyph.Height();
                auto frame = image.crop(xywh(x, y, w, h));
                frame.blend(rgba::white, 200);
                glyph.render(frame, xy(), 200, x);
                x += glyph.advance + gap;
            }
        }
    }
};

struct TestPixText:
widget<TestPixText>
{
    gui::image Image;
    pix::image<rgba> image;

    void on_change (void* what) override
    {
        if (coord.now.size == image.size or
            coord.now.size == xy())
            return;

        image.resize(coord.now.size);
        Image.coord = coord.now.local();
        Image.source = image.crop();
        image.fill(rgba::white);
        auto frame = image.crop();

        int w = image.size.x;
        int h = image.size.y/2;
        int u = gui::metrics::text::height;

        pix::text::style style;
        style.color = rgba::black;
        pix::text::style_index s(style);
        auto frame1 = frame.crop(xywh(u,u,u*5,u*2));
        frame1.blend(rgba::red, 200);
        pix::text::token("abc", s).render(frame1, xy{}, 200);
    }
};

struct TestPixUtil:
widget<TestPixUtil>
{
    gui::image Image;
    pix::image<rgba> image;

    void on_change (void* what) override
    {
        if (coord.now.size == image.size or
            coord.now.size == xy())
            return;

        image.resize(coord.now.size);
        Image.coord = coord.now.local();
        Image.source = image.crop();
        auto frame = image.crop();

        int w = image.size.x;
        int h = image.size.y/2;
        int u = gui::metrics::text::height;

        auto frame1 = image.crop(xywh(0,0,w,h));
        auto frame2 = image.crop(xywh(0,h,w,h));
        frame1.fill(rgba::white);
        frame2.fill(rgba::black);

        int x = 10;
        for (int i=u/2; i<=u*4/3; i++)
        {
            auto node = pix::util::node("13",
            rgba::red, rgba::white, rgba::black, 0, font{"",i});
            frame1.from(x,10).blend_from(node);
            frame2.from(x,10).blend_from(node);
            x += node.size.x*3/2;
        }
    }
};
