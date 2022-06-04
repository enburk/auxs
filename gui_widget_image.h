#pragma once
#include <fstream>
#include "sys_aux.h"
#include "pix_sampling.h"
#include "gui_widget_aux.h"
#include "gui_widgetarium.h"
namespace gui
{
    struct image:
    widget<image>
    {
        unary_property<pix::frame<rgba>> source;
        enum fitness { none, fill, contain, cover, scale_down };
        binary_property<fitness> fit = fill;

        pix::image<rgba> resized_image;
        pix::frame<rgba> resized_frame;
        xy shift;

        void on_render (sys::window& window, xywh r, xy offset, uint8_t alpha) override
        {
            auto frame = resized_frame
                .crop(xywh(
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
                    resized_image.resize(xy());
                    resized_frame = source.now;
                }
                else if (fit.now == fill)
                {
                    resized_image = pix::resized(source.now, coord.now.size);
                    resized_frame = resized_image.crop();
                }
                else if (fit.now == cover)
                {
                    xy size = coord.now.size;
                    xy maxsize = source.now.size;
                    size = size.x/maxsize.x > size.y/maxsize.y ?
                    xy(maxsize.x * size.y / maxsize.y, size.y) :
                    xy(size.x, maxsize.y * size.x / maxsize.x);
                    resized_image = pix::resized(source.now, size);
                    resized_frame = resized_image.crop();
                }
                else // (fit.now == contain) // or scale_down and less
                {
                    xy size = coord.now.size;
                    xy maxsize = source.now.size;
                    size = size.x/maxsize.x < size.y/maxsize.y ?
                    xy(maxsize.x * size.y / maxsize.y, size.y) :
                    xy(size.x, maxsize.y * size.x / maxsize.x);
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
        pix::image<rgba> tinted;
        pix::image<rgba> original;
        binary_property<xyxy> padding;
        property<rgba> color;

        void load (pix::frame<rgba> frame)
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
                rgba src = original(x,y);
                rgba & dst = tinted(x,y);
                dst = src;

                if (src.r == 0 and src.g == 0 and src.b == 0) {
                    dst.r = color.now.r;
                    dst.g = color.now.g;
                    dst.b = color.now.b;
                }
            }

            tinted.updates += xywh(0,0,
            tinted.size.x, tinted.size.y);

            image.source = pix::frame<rgba>{};
            image.source = tinted.crop();
        }
    };
}
