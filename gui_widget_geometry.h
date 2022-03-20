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

        void on_render (sys::window& window, XYWH r, XY offset, uint8_t alpha) override
        {
            // this widget origin is shifted by 'offset'
            // relative to the window frame origin (r.origin)
            XYXY rr = coord.now.local() + r.origin - offset;

            //aux::vector<2, double> p1, p2;
            //p1.x = rr.x1 + x1.now - coord.now.x;
            //p1.y = rr.y1 + y1.now - coord.now.y;
            //p2.x = rr.x2 + x2.now - coord.now.x - coord.now.w;
            //p2.y = rr.y2 + y2.now - coord.now.y - coord.now.h;


            std::array<double, 4> points;
            points[0] = rr.x1 + x1.now - coord.now.x;
            points[1] = rr.y1 + y1.now - coord.now.y;
            points[2] = rr.x2 + x2.now - coord.now.x - coord.now.w;
            points[3] = rr.y2 + y2.now - coord.now.y - coord.now.h;

            window.render(r, alpha, color.now,
                pix::geo::lines,
                points.data(), (int)
                points.size());
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
                coord = XYXY
                (
                    int(std::floor(min(x1,x2))),
                    int(std::floor(min(y1,y2))),
                    int(std::ceil (max(x1,x2))),
                    int(std::ceil (max(y1,y2)))
                );
                update();
            }
        }
    };

}
