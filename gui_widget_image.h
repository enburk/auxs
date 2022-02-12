#pragma once
#include <fstream>
#include "sys_aux.h"
#include "pix_sampling.h"
#include "gui_widget_canvas.h"
#include "gui_widgetarium.h"
namespace gui
{
    struct image:
    widget<image>
    {
        unary_property<pix::frame<RGBA>> source;
        enum fitness { none, fill, contain, cover, scale_down };
        binary_property<fitness> fit = fill;

        pix::image<RGBA> resized_image;
        pix::frame<RGBA> resized_frame;
        XY shift;

        void on_render (sys::window& window, XYWH r, XY offset, uint8_t alpha) override
        {
            auto frame = resized_frame
                .crop(XYWH(
                offset.x + shift.x,
                offset.y + shift.y,
                r.size.x, r.size.y));

            if (frame.size.x > 0
            &&  frame.size.y > 0)
                window.render(r, alpha, frame);
        }

        void on_change (void* what) override
        {
            if((what == &coord && coord.was.size != coord.now.size)
            ||  what == &source
            ||  what == &fit)
            {
                if (source.now.size.x == 0
                ||  source.now.size.y == 0
                ||  source.now.size == coord.now.size
                ||  fit.now == none || (fit.now == scale_down
                &&  source.now.size.x <= coord.now.size.x
                &&  source.now.size.y <= coord.now.size.y))
                {
                    resized_image.resize(XY());
                    resized_frame = source.now;
                }
                else if (fit.now == fill)
                {
                    resized_image = pix::resized(source.now, coord.now.size);
                    resized_frame = resized_image.crop();
                }
                else if (fit.now == cover)
                {
                    XY size = coord.now.size;
                    XY maxsize = source.now.size;
                    size = size.x/maxsize.x > size.y/maxsize.y ?
                    XY(maxsize.x * size.y / maxsize.y, size.y) :
                    XY(size.x, maxsize.y * size.x / maxsize.x);
                    resized_image = pix::resized(source.now, size);
                    resized_frame = resized_image.crop();
                }
                else // (fit.now == contain) // or scale_down and less
                {
                    XY size = coord.now.size;
                    XY maxsize = source.now.size;
                    size = size.x/maxsize.x < size.y/maxsize.y ?
                    XY(maxsize.x * size.y / maxsize.y, size.y) :
                    XY(size.x, maxsize.y * size.x / maxsize.x);
                    resized_image = pix::resized(source.now, size);
                    resized_frame = resized_image.crop();
                }

                shift = coord.now.size/2 - resized_frame.size/2;

                update();
            }
        }
    };

    struct icon:
    widget<icon>
    {
        image image;
        pix::image<RGBA> tinted;
        pix::image<RGBA> original;
        binary_property<XYXY> padding;
        property<RGBA> color;

        void load (pix::frame<RGBA> frame)
        {
            original.resize(frame.size);
            original.crop().copy_from(frame);
            refresh();
        }
        void load (array<sys::byte>::range_type range)
        {
            original = pix::unpack(range).value();
            refresh();
        }
        void load (array<sys::byte> & array)
        {
            original = pix::unpack(array.from(0)).value();
            refresh();
        }

        void on_change (void* what) override
        {
            if (what == &coord or what == &padding)
            {
                image.coord = coord.now.local()
                    + padding;
            }
            if (what == &color)
            {
                refresh();
            }
        }

        void refresh()
        {
            tinted.resize(original.size);
            for (int y=0; y<tinted.size.y; y++)
            for (int x=0; x<tinted.size.x; x++)
            {
                RGBA src = original(x,y);
                RGBA & dst = tinted(x,y);
                dst = src;

                if (src.r == 0 and src.g == 0 and src.b == 0) {
                    dst.r = color.now.r;
                    dst.g = color.now.g;
                    dst.b = color.now.b;
                }
            }

            tinted.updates += XYWH(0,0,
            tinted.size.x, tinted.size.y);

            image.source = pix::frame<RGBA>{};
            image.source = tinted.crop();
        }
    };
}
