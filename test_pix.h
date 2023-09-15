#pragma once
#include <format>
#include "aux_unittest.h"
#include "gui_widget_console.h"
#include "sys_io_in.h"
using namespace pix;
using gui::widget;

namespace aux::unittest
{
    void test_pix1 () try
    {
        test("pix.hsv");
        {
            auto p = [](double x){ return std::format("{:5.2f}, ", x); };
            auto q = [](double x){ return std::format("{:5.1f}, ", x); };
            array<array<double>> rgb_hsv =
            {
	            {1.000, 1.000, 1.000,   0.0, 0.000, 1.000}, // #FFFFFF (H n/a)
	            {0.500, 0.500, 0.500,   0.0, 0.000, 0.500}, // #808080 (H n/a)
	            {0.000, 0.000, 0.000,   0.0, 0.000, 0.000}, // #000000 (H n/a)
	            {1.000, 0.000, 0.000,   0.0, 1.000, 1.000}, // #FF0000
	            {0.750, 0.750, 0.000,  60.0, 1.000, 0.750}, // #BFBF00
	            {0.000, 0.500, 0.000, 120.0, 1.000, 0.500}, // #008000
	            {0.500, 1.000, 1.000, 180.0, 0.500, 1.000}, // #80FFFF
	            {0.500, 0.500, 1.000, 240.0, 0.500, 1.000}, // #8080FF
	            {0.750, 0.250, 0.750, 300.0, 0.667, 0.750}, // #BF40BF
	            {0.628, 0.643, 0.142,  61.8, 0.779, 0.643}, // #A0A424
	            {0.255, 0.104, 0.918, 251.1, 0.887, 0.918}, // #411BEA
	            {0.116, 0.675, 0.255, 134.9, 0.828, 0.675}, // #1EAC41
	            {0.941, 0.785, 0.053,  49.5, 0.944, 0.941}, // #F0C80E
	            {0.704, 0.187, 0.897, 283.7, 0.792, 0.897}, // #B430E5
	            {0.931, 0.463, 0.316,  14.3, 0.661, 0.931}, // #ED7651
	            {0.998, 0.974, 0.532,  56.9, 0.467, 0.998}, // #FEF888
	            {0.099, 0.795, 0.591, 162.4, 0.875, 0.795}, // #19CB97
	            {0.211, 0.149, 0.597, 248.3, 0.750, 0.597}, // #362698
	            {0.495, 0.493, 0.721, 240.5, 0.316, 0.721}, // #7E7EB8
            };
            for (auto& d: rgb_hsv)
            {
                double h,s,v; rgb2hsv(d[0],d[1],d[2],h,s,v);
                double r,g,b; hsv2rgb(d[3],d[4],d[5],r,g,b);
                oops(out(p(r)+p(g)+p(b))) { p(d[0])+p(d[1])+p(d[2]) };
                oops(out(q(h)+p(s)+p(v))) { q(d[3])+p(d[4])+p(d[5]) };
            }
        }
        test("pix.hsl");
        {
            auto p = [](double x){ return std::format("{:5.2f}, ", x); };
            auto q = [](double x){ return std::format("{:5.1f}, ", x); };
            array<array<double>> rgb_hsl =
            {
	            {1.000, 1.000, 1.000,   0.0, 0.000, 1.000}, // #FFFFFF (H n/a)
	            {0.500, 0.500, 0.500,   0.0, 0.000, 0.500}, // #808080 (H n/a)
	            {0.000, 0.000, 0.000,   0.0, 0.000, 0.000}, // #000000 (H n/a)
	            {1.000, 0.000, 0.000,   0.0, 1.000, 0.500}, // #FF0000
	            {0.750, 0.750, 0.000,  60.0, 1.000, 0.375}, // #BFBF00
	            {0.000, 0.500, 0.000, 120.0, 1.000, 0.250}, // #008000
	            {0.500, 1.000, 1.000, 180.0, 1.000, 0.750}, // #80FFFF
	            {0.500, 0.500, 1.000, 240.0, 1.000, 0.750}, // #8080FF
	            {0.750, 0.250, 0.750, 300.0, 0.500, 0.500}, // #BF40BF
	            {0.628, 0.643, 0.142,  61.8, 0.638, 0.393}, // #A0A424
	            {0.254, 0.104, 0.918, 251.1, 0.832, 0.511}, // #411BEA
	            {0.116, 0.675, 0.255, 134.9, 0.707, 0.396}, // #1EAC41
	            {0.941, 0.786, 0.053,  49.5, 0.893, 0.497}, // #F0C80E
	            {0.704, 0.187, 0.897, 283.7, 0.775, 0.542}, // #B430E5
	            {0.931, 0.462, 0.316,  14.2, 0.817, 0.624}, // #ED7651
	            {0.998, 0.974, 0.532,  56.9, 0.991, 0.765}, // #FEF888
	            {0.099, 0.795, 0.591, 162.4, 0.779, 0.447}, // #19CB97
	            {0.211, 0.149, 0.597, 248.3, 0.601, 0.373}, // #362698
	            {0.495, 0.493, 0.721, 240.5, 0.290, 0.607}, // #7E7EB8
            };
            for (auto& d: rgb_hsl)
            {
                double h,s,v; rgb2hsl(d[0],d[1],d[2],h,s,v);
                double r,g,b; hsl2rgb(d[3],d[4],d[5],r,g,b);
                oops(out(p(r)+p(g)+p(b))) { p(d[0])+p(d[1])+p(d[2]) };
                oops(out(q(h)+p(s)+p(v))) { q(d[3])+p(d[4])+p(d[5]) };
            }
            auto navy = forecorrected(rgba::navy, rgba::black);
            oops(out(navy.r)) { "127" };
            oops(out(navy.g)) { "127" };
            oops(out(navy.b)) { "255" };
        }
        test("pix.lab");
        {
            auto p = [](double x){ return std::format("{:5.3f}, ", x); };
            auto q = [](double x){ return std::format("{:5.1f}, ", x); };
            array<array<double>> rgb_lab =
            {
	            {1.000, 1.000, 1.000, 1.000, 0.000, 0.000}, // #FFFFFF
	            {0.500, 0.500, 0.500, 0.794, 0.000, 0.000}, // #808080
	            {0.000, 0.000, 0.000, 0.000, 0.000, 0.000}, // #000000
	            {1.000, 0.000, 0.000, 0.628, 0.255, 0.126}, // #FF0000
	            {0.750, 0.750, 0.000, 0.879,-0.065, 0.180}, // #BFBF00
	            {0.000, 0.500, 0.000, 0.688,-0.186, 0.142}, // #008000
	            {0.500, 1.000, 1.000, 0.955,-0.064,-0.019}, // #80FFFF
	            {0.500, 0.500, 1.000, 0.818, 0.023,-0.092}, // #8080FF
	            {0.750, 0.250, 0.750, 0.751, 0.129,-0.084}, // #BF40BF
	            {0.628, 0.643, 0.142, 0.840,-0.044, 0.120}, // #A0A424
	            {0.254, 0.104, 0.918, 0.603, 0.085,-0.203}, // #411BEA
	            {0.116, 0.675, 0.255, 0.790,-0.137, 0.070}, // #1EAC41
	            {0.941, 0.786, 0.053, 0.911,-0.037, 0.169}, // #F0C80E
	            {0.704, 0.187, 0.897, 0.727, 0.149,-0.128}, // #B430E5
	            {0.931, 0.462, 0.316, 0.828, 0.062, 0.061}, // #ED7651
	            {0.998, 0.974, 0.532, 0.979,-0.020, 0.073}, // #FEF888
	            {0.099, 0.795, 0.591, 0.842,-0.129, 0.006}, // #19CB97
	            {0.211, 0.149, 0.597, 0.591, 0.050,-0.133}, // #362698
	            {0.495, 0.493, 0.721, 0.801, 0.013,-0.046}, // #7E7EB8
            };
            for (auto& d: rgb_lab)
            {
                double h,s,v; rgb2lab(d[0],d[1],d[2],h,s,v);
                double r,g,b; lab2rgb(d[3],d[4],d[5],r,g,b);
                oops(out(p(r)+p(g)+p(b))) { p(d[0])+p(d[1])+p(d[2]) };
                oops(out(p(h)+p(s)+p(v))) { p(d[3])+p(d[4])+p(d[5]) };
            }
            auto navy = forecorrected(rgba::navy, rgba::black);
            oops(out(navy.r)) { "127" };
            oops(out(navy.g)) { "127" };
            oops(out(navy.b)) { "255" };
        }
        test("pix.hcl");
        {
            auto p = [](double x){ return std::format("{:5.3f}, ", x); };
            auto q = [](double x){ return std::format("{:5.1f}, ", x); };
            array<array<double>> rgb_hcl =
            {
//	            {1.000, 1.000, 1.000,   0.0, 0.000, 1.000}, // #FFFFFF
//	            {0.500, 0.500, 0.500,   0.0, 0.000, 0.500}, // #808080
//	            {0.000, 0.000, 0.000,   0.0, 0.000, 0.000}, // #000000
//	            {1.000, 0.000, 0.000,   0.0, 1.000, 0.500}, // #FF0000
//	            {0.750, 0.750, 0.000,  60.0, 1.000, 0.375}, // #BFBF00
//	            {0.000, 0.500, 0.000, 120.0, 1.000, 0.250}, // #008000
//	            {0.500, 1.000, 1.000, 180.0, 1.000, 0.750}, // #80FFFF
//	            {0.500, 0.500, 1.000, 240.0, 1.000, 0.750}, // #8080FF
//	            {0.750, 0.250, 0.750, 300.0, 0.500, 0.500}, // #BF40BF
//	            {0.628, 0.643, 0.142,  61.8, 0.638, 0.393}, // #A0A424
//	            {0.254, 0.104, 0.918, 251.1, 0.832, 0.511}, // #411BEA
//	            {0.116, 0.675, 0.255, 134.9, 0.707, 0.396}, // #1EAC41
//	            {0.941, 0.786, 0.053,  49.5, 0.893, 0.497}, // #F0C80E
//	            {0.704, 0.187, 0.897, 283.7, 0.775, 0.542}, // #B430E5
//	            {0.931, 0.462, 0.316,  14.2, 0.817, 0.624}, // #ED7651
//	            {0.998, 0.974, 0.532,  56.9, 0.991, 0.765}, // #FEF888
//	            {0.099, 0.795, 0.591, 162.4, 0.779, 0.447}, // #19CB97
//	            {0.211, 0.149, 0.597, 248.3, 0.601, 0.373}, // #362698
//	            {0.495, 0.493, 0.721, 240.5, 0.290, 0.607}, // #7E7EB8

	            {1.000, 1.000, 1.000,  53.2, 0.000, 1.000}, // #FFFFFF
	            {0.500, 0.500, 0.500,  53.2, 0.000, 0.794}, // #808080
	            {0.000, 0.000, 0.000,   0.0, 0.000, 0.000}, // #000000
	            {1.000, 0.000, 0.000,  29.2, 0.258, 0.628}, // #FF0000
	            {0.750, 0.750, 0.000, 109.8, 0.192, 0.879}, // #BFBF00
	            {0.000, 0.500, 0.000, 142.5, 0.234, 0.688}, // #008000
	            {0.500, 1.000, 1.000,-163.9, 0.067, 0.955}, // #80FFFF
	            {0.500, 0.500, 1.000, -75.9, 0.095, 0.818}, // #8080FF
	            {0.750, 0.250, 0.750, -33.0, 0.154, 0.751}, // #BF40BF
	            {0.628, 0.643, 0.142, 109.9, 0.128, 0.840}, // #A0A424
	            {0.254, 0.104, 0.918, -67.4, 0.220, 0.603}, // #411BEA
	            {0.116, 0.675, 0.255, 153.0, 0.153, 0.790}, // #1EAC41
	            {0.941, 0.786, 0.053, 102.5, 0.173, 0.911}, // #F0C80E
	            {0.704, 0.187, 0.897, -40.7, 0.196, 0.727}, // #B430E5
	            {0.931, 0.462, 0.316,  44.3, 0.087, 0.828}, // #ED7651
	            {0.998, 0.974, 0.532, 105.5, 0.076, 0.979}, // #FEF888
	            {0.099, 0.795, 0.591, 177.3, 0.129, 0.842}, // #19CB97
	            {0.211, 0.149, 0.597, -69.5, 0.142, 0.591}, // #362698
	            {0.495, 0.493, 0.721, -74.3, 0.048, 0.801}, // #7E7EB8


            };
            for (auto& d: rgb_hcl)
            {
                double h,s,v; rgb2hcl(d[0],d[1],d[2],h,s,v);
                double r,g,b; hcl2rgb(d[3],d[4],d[5],r,g,b);
                oops(out(p(r)+p(g)+p(b))) { p(d[0])+p(d[1])+p(d[2]) };
                oops(out(q(h)+p(s)+p(v))) { q(d[3])+p(d[4])+p(d[5]) };
            }
            auto navy = forecorrected(rgba::navy, rgba::black);
            oops(out(navy.r)) { "127" };
            oops(out(navy.g)) { "127" };
            oops(out(navy.b)) { "255" };
        }
    }
    catch(assertion_failed){}

    void test_pix2 () try
    {
        test("gif");
        {
            {
                array<byte> bytes;
                for (str s: array<str>(str("47 49 46 38 39 61 "
                "0A 00 0A 00 91 00 00 FF FF FF FF 00 00 00 00 FF 00 00 00 "
                "21 F9 04 00 00 00 00 00 2C 00 00 00 00 0A 00 0A 00 00 02 "
                "16 8C 2D 99 87 2A 1C DC 33 A0 02 75 EC 95 FA A8 DE 60 8C "
                "04 91 4C 01 00 3B").split_by(" ")))
                    bytes += (byte)std::strtoul(
                        s.c_str(), nullptr, 16);

                gif::decoder x(bytes.whole());

                ouch(out(x.Images.size())) { "1" };

                x.Images[0].unpack(x.gct);

                oops(out("ok")) { "ok" };
            }
            {
                auto bytes =
                sys::in::bytes("../data/xtest.gif");
                if (not bytes.empty())
                {
                    gif::decoder x(bytes.whole());
                }
                oops(out("ok")) { "ok" };
            }
        }
    }
    catch(assertion_failed){}
}

struct TestPix:
widget<TestPix>
{
    bool ok = true;
    bool done = false;
    gui::canvas canvas;
    gui::area<gui::console> console1;
    gui::area<gui::console> console2;
    void on_change (void* what) override
    {
        if (what == &coord and
            coord.was.size !=
            coord.now.size)
        {
            int h = gui::metrics::text::height*12/7;
            int W = coord.now.w; if (W <= 0) return; int w = W/3;
            int H = coord.now.h; if (H <= 0) return;
            console1.coord = xywh(w*0, 0, w, H);
            console2.coord = xywh(w*1, 0, w, H);

            if (done) return; done = true;

            auto style = pix::text::style{
            pix::font{"Consolas"}, rgba::black};
            console1.object.page.style = style;
            console2.object.page.style = style;

            aux::unittest::test_pix1();
            aux::unittest::test("");
            console1.object.page.html = 
            aux::unittest::results; ok &= 
            aux::unittest::all_ok;

            aux::unittest::test_pix2();
            aux::unittest::test("");
            console2.object.page.html = 
            aux::unittest::results; ok &= 
            aux::unittest::all_ok;

            console1.object.page.scroll.y.top = max<int>();
            console2.object.page.scroll.y.top = max<int>();
        }
    }
};

struct TestPixDraw:
widget<TestPixDraw>
{
    gui::image Image;
    pix::image<rgba> image;

    void on_change (void* what) override
    {
        if (coord.now.size == xy{} or
            image.size != xy{})
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
            vector{r,0}.rotated(-a) + vector{r,r}}, rgba::white, w1*5);
            frame2.copy (line{{r,r},
            vector{r,0}.rotated(-a) + vector{r,r}}, rgba::white, w2*5);

            threads += 
            std::jthread([&frame1,&frame2,r,rr,w1,w2](){
            frame1.blend(circle{{3*r,r}, rr}, rgba::white, w1);
            frame2.copy (circle{{3*r,r}, rr}, rgba::white, w2); });

            threads += 
            std::jthread([&frame1,&frame2,r,rr,w1,w2](){
            frame1.blend(circle{{5*r,r}, rr}, rgba::white, w1, w1*pi/2, w1*pi);
            frame2.copy (circle{{5*r,r}, rr}, rgba::white, w2, w2*pi/2, w2*pi); });

            frame1.blend(circle{{7*r,r}, rr}, rgba(cc,0,0));
            frame2.copy (circle{{7*r,r}, rr}, rgba(255 - cc,0,0));
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
        if (coord.now.size == xy{} or
            image.size != xy{})
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
        if (coord.now.size == xy{} or
            image.size != xy{})
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
        pix::font{"Consolas", gui::metrics::text::height*2},
        pix::font{"Arial",    gui::metrics::text::height*2},
        pix::font{"Segoe UI", gui::metrics::text::height*2},
        pix::font{"Tahoma",   gui::metrics::text::height*2}};

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

str lorem = 
"Lorem ipsum dolor sit amet, consectetur "
"adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\n\n"
"Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip "
"ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit "
"esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non "
"proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";

struct TestPixText:
widget<TestPixText>
{
    gui::image Image;
    pix::image<rgba> image;

    void on_change (void* what) override
    {
        if (coord.now.size == xy{} or
            image.size != xy{})
            return;

        image.resize(coord.now.size);
        Image.coord = coord.now.local();
        Image.source = image.crop();

        const int n = 3;
        int W = image.size.x;
        int H = image.size.y;
        int u = gui::metrics::text::height;
        int w = W / 5;
        int h = H / 2 / n;

        pix::text::style style {.color = rgba::black};
        pix::text::style_index index(style);
        pix::text::block text;
        auto& format = text.format;

        for (str line: lorem.split_by("\n")) {
            text.lines += pix::text::line{};
            for (str word: line.split_by(" ")) {
                text.lines.back().tokens +=
                pix::text::token(word, index);
                text.lines.back().tokens +=
                pix::text::token(" ", index);
            }
        }

        for (int i=0; i<2; i++)
        for (int j=0; j<n; j++)
        for (int k=0; k<5; k++)
        {
            xywh r(w*k, i*h*n+j*h, w, h); r.deflate(1);
            auto frame = image.crop(r);
            frame.fill(rgba::white);

            format.alignment =
            k == 0? xy{pix::left,           pix::top}:
            k == 1? xy{pix::center,         pix::center}:
            k == 2? xy{pix::justify_left-1, pix::top}:
            k == 3? xy{pix::justify_left,   pix::top}:
            k == 4? xy{pix::right,          pix::bottom}: xy{};
            format.ellipsis = true;
            format.width    = frame.size.x;
            format.height   = frame.size.y;
            format.columns  = j+1;
            format.gutter   = u;
            format.lwrap.clear();
            format.rwrap.clear();

            if (i == 1)
            {
                int v = w/(j+1);
                frame.crop(xywh(0, h/2,   v/4, h/4)).fill(rgba::red);
                frame.crop(xywh(w-v/4, 0, v/4, h/4)).fill(rgba::red);
                format.lwrap = array<xy>{{0, h/2}, {v/4, h/4}};
                format.rwrap = array<xy>{{v/4, h/4}};
            }

            text.layout();
            text.render(frame);

            aux::vector<2> p1, p2;
            p1.x = aux::random(0, frame.size.x-1);
            p1.y = aux::random(0, frame.size.y-1);
            p2.x = aux::random(0, frame.size.x-1);
            p2.y = aux::random(0, frame.size.y-1);
            auto t1 = text.pointed({int(p1.x),int(p1.y)}, false);
            auto t2 = text.pointed({int(p2.x),int(p2.y)}, false);
            for (xyxy r: text.bars({t1,t2}, false))
            frame.crop(r).blend(rgba(0,128,128,64));
            frame.blend(aux::circle(p1,2), rgba::blue);
            frame.blend(aux::circle(p2,2), rgba::blue);
            frame.blend(aux::circle(p1,3), rgba::blue, 2);
            frame.blend(aux::circle(p2,3), rgba::blue, 2);
        }
    }
};

struct TestPixTextX:
widget<TestPixTextX>
{
    gui::image Image;
    pix::image<rgba> image;

    void on_change (void* what) override
    {
        if (coord.now.size == xy{} or
            image.size != xy{})
            return;

        image.resize(coord.now.size);
        Image.coord = coord.now.local();
        Image.source = image.crop();

        const int n = 3;
        int W = image.size.x;
        int H = image.size.y;
        int u = gui::metrics::text::height;
        int w = W / 8;
        int h = H / 2 / n;

        pix::text::style style {.color = rgba::black};
        pix::text::style_index index(style);
        pix::text::block text;
        auto& format = text.format;

        for (str line: lorem.split_by("\n")) {
            text.lines += pix::text::line{};
            for (str word: line.split_by(" ")) {
                text.lines.back().tokens +=
                pix::text::token(word, index);
                text.lines.back().tokens +=
                pix::text::token(" ", index);
            }
        }

        int x = 0;
        for (int a=0; a<4; a++, x += 4*w, w /= 2)
        for (int i=0; i<2; i++)
        for (int j=0; j<n; j++)
        for (int k=0; k<4; k++)
        {
            xywh r(x + w*k, i*h*n+j*h, w, h); r.deflate(1);
            auto frame = image.crop(r);
            frame.fill(rgba::white);

            format.alignment =
            k == 0? xy{pix::left,           pix::top}:
            k == 1? xy{pix::center,         pix::center}:
            k == 2? xy{pix::justify_left,   pix::top}:
            k == 3? xy{pix::right,          pix::bottom}: xy{};
            format.ellipsis = true;
            format.width    = frame.size.x;
            format.height   = frame.size.y;
            format.columns  = j+1;
            format.gutter   = u;
            format.lwrap.clear();
            format.rwrap.clear();

            if (i == 1)
            {
                int v = w/(j+1);
                frame.crop(xywh(0, h/2,   v/4, h/4)).fill(rgba::red);
                frame.crop(xywh(w-v/4, 0, v/4, h/4)).fill(rgba::red);
                format.lwrap = array<xy>{xy(0, h/2), xy(v/4, h/4)};
                format.rwrap = array<xy>{xy(v/4, h/4)};
            }

            text.layout();
            text.render(frame);

            auto t1 = pix::text::place{0, j+1};
            auto t2 = pix::text::place{0, j+1+j+1};
            for (xyxy r: text.bars({t1,t2}, false))
            frame.crop(r).blend(rgba(0,128,128,64));
            auto t3 = pix::text::place{2, j+1};
            auto t4 = pix::text::place{2, j+1+j+1};
            for (xyxy r: text.bars({t3,t4}, false))
            frame.crop(r).blend(rgba(0,128,128,64));
        }
    }
};

struct TestPixUtil:
widget<TestPixUtil>
{
    gui::image Image;
    pix::image<rgba> image;

    void on_change (void* what) override
    {
        if (coord.now.size == xy{} or
            image.size != xy{})
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
        frame2.fill(gui::skins["gray+"].ultralight.first);

        int d = 9;
        int x = d;
        for (int i=u/2; i<=u*4/3; i++)
        {
            int y = d;
            auto node = pix::util::node("13",
            rgba::red, rgba::white, rgba::black, 0, font{"",i});
            frame1.from(x,y).blend_from(node);
            frame2.from(x,y).blend_from(node);
            int z = node.size.x;
            y += z + d;
            frame1.from(x,y).blend_from(pix::util::icon("play.play", xy{z,z}, rgba::black, z/4));
            frame2.from(x,y).blend_from(pix::util::icon("play.play", xy{z,z}, rgba::white, z/4));
            y += z + d;
            frame1.from(x,y).blend_from(pix::util::icon("play.stop", xy{z,z}, rgba::black, z/4));
            frame2.from(x,y).blend_from(pix::util::icon("play.stop", xy{z,z}, rgba::white, z/4));
            y += z + d;
            frame1.from(x,y).blend_from(pix::util::icon("", xy{z,z}, rgba::black));
            frame2.from(x,y).blend_from(pix::util::icon("", xy{z,z}, rgba::white));
            x += z + d;
        }
    }
};
