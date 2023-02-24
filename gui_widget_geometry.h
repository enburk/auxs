#pragma once
#include "gui_widget.h"
namespace gui
{
    struct line:
    widget<line>
    {
        property<rgba> color;
        property<double> x1 = 0.0;
        property<double> y1 = 0.0;
        property<double> x2 = 0.0;
        property<double> y2 = 0.0;
        property<double> width = 1.0;
        pix::geo geo = pix::geo::none;
        double points[8];

        void on_render (sys::window& window, xywh r, xy offset, uint8_t alpha) override
        {
            if (alpha > 0 and color.now.a > 0)
            window.render(r, alpha, color.now,
                offset, geo, points,
                geo == pix::geo::lines?
                4 : 8);
        }

        void on_change (void* what) override
        {
            if (what == &color)
                if (color.was.a != 0 or
                    color.now.a != 0 )
                    update();

            if (what == &x1 or what == &x2 or
                what == &y1 or what == &y2 or
                what == &width)
            {
                if (width.now < 1.1)
                {
                    xyxy r (
                    int(std::floor(min(x1.now, x2.now))),
                    int(std::floor(min(y1.now, y2.now))),
                    int(std::ceil (max(x1.now, x2.now))),
                    int(std::ceil (max(y1.now, y2.now))));
                    coord = r;
                    geo = pix::geo::lines;
                    points[0] = x1.now - r.l;
                    points[1] = y1.now - r.t;
                    points[2] = x2.now - r.l;
                    points[3] = y2.now - r.t;
                }
                else
                {
                    aux::vector<2> p1 {x1.now, y1.now};
                    aux::vector<2> p2 {x2.now, y2.now};

                    aux::vector<2> a = p2 - p1;
                    a = width.now * a.normalized().rotated(pi/2);

                    aux::vector<2> v1 = p1 - a;
                    aux::vector<2> v2 = p1 + a;
                    aux::vector<2> v3 = p2 - a;
                    aux::vector<2> v4 = p2 + a;

                    xyxy r (
                    int(std::floor(min(v1.x, v2.x, v3.x, v4.x))),
                    int(std::floor(min(v1.y, v2.y, v3.y, v4.y))),
                    int(std::ceil (max(v1.x, v2.x, v3.x, v4.x))),
                    int(std::ceil (max(v1.y, v2.y, v3.y, v4.y))));
                    coord = r;

                    geo = pix::geo::triangle_strip;
                    points[0] = v1.x - r.l; points[1] = v1.y - r.t;
                    points[2] = v2.x - r.l; points[3] = v2.y - r.t;
                    points[4] = v3.x - r.l; points[5] = v3.y - r.t;
                    points[6] = v4.x - r.l; points[7] = v4.y - r.t;
                }
                update();
            }
        }
    };

    struct oval:
    widget<oval>
    {
        property<rgba> color;
        property<double> x = 0.0;
        property<double> y = 0.0;
        property<double> rx = 0.0;
        property<double> ry = 0.0;
        property<double> rx2 = 0.0;
        property<double> ry2 = 0.0;
        pix::geo geo = pix::geo::none;
        array<double> points;

        void on_render (sys::window& window, xywh r, xy offset, uint8_t alpha) override
        {
            if (alpha > 0 and color.now.a > 0)
            window.render(r, alpha, color.now,
                offset, geo,
                points.data(),(int)
                points.size());
        }

        void on_change (void* what) override
        {
            if (what == &color)
            if (color.was.a != 0 or
                color.now.a != 0 )
                update();

            if (what == &x or what == &rx or what == &rx2 or
                what == &y or what == &ry or what == &ry2)
            {
                points.clear();
                aux::vector<2> r1 {rx.now, ry.now};
                aux::vector<2> r2 {rx2.now, ry2.now};
                if (r1.x > r2.x) std::swap(r1.x, r2.x); 
                if (r1.y > r2.y) std::swap(r1.y, r2.y);
                double rmax = max(r2.x,r2.y); if (rmax < 0.5) return;
                double delta = 1/(rmax+2);

                xyxy r (
                int(std::floor(x.now - r2.x)),
                int(std::floor(y.now - r2.y)),
                int(std::ceil (x.now + r2.x)),
                int(std::ceil (y.now + r2.y)));
                coord = r;

                double dx = x.now - r.l;
                double dy = y.now - r.t;

                if (r1.x < 0.5 or r1.y < 0.5)
                {
                    geo = pix::geo::triangle_fan;
                    points.reserve(int(2*2*pi/delta + 2 + 2));
                    points += dx;
                    points += dy;
                    for (double a = 0.0;
                        a < 2*pi + delta/2;
                        a += delta) {
                        points += dx + r2.x*cos(a);
                        points += dy + r2.y*sin(a);
                    }
                }
                else
                if (aux::distance(r1, r2) < 1.1)
                {
                    geo = pix::geo::lines;
                    points.reserve(int(2*2*pi/delta + 2));
                    for (double a = 0.0;
                        a < 2*pi + delta/2;
                        a += delta) {
                        points += dx + r2.x*cos(a);
                        points += dy + r2.y*sin(a);
                    }
                }
                else
                {
                    geo = pix::geo::triangle_strip;
                    points.reserve(int(4*2*pi/delta + 4));
                    for (double a = 0.0;
                        a < 2*pi + delta/2;
                        a += delta) {
                        points += dx + r1.x*cos(a);
                        points += dy + r1.y*sin(a);
                        points += dx + r2.x*cos(a);
                        points += dy + r2.y*sin(a);
                    }
                }
                update();
            }
        }
    };
}
