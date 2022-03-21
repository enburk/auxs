#pragma once
#include "gui_widget.h"
namespace gui
{
    struct line:
    widget<line>
    {
        property<RGBA> color;
        property<double> x1 = 0.0;
        property<double> y1 = 0.0;
        property<double> x2 = 0.0;
        property<double> y2 = 0.0;
        property<double> width = 1.0;
        pix::geo geo = pix::geo::none;
        double points[8];

        void on_render (sys::window& window, XYWH r, XY offset, uint8_t alpha) override
        {
            window.render(r, alpha, color.now,
                offset, geo, points,
                geo == geo::lines?
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
                    XYXY r (
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

                    XYXY r (
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

}
