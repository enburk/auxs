#pragma once
#include "pix_image.h"
namespace pix
{
    template<class T> struct frame
    {
        typedef T type;
        typedef T color;
        image<type>* img = nullptr;
        xy offset, size;

        frame () = default;
        frame (image<type>& img) : img(&img), size(img.size) {}

        const type& operator () (int x, int y) const { return (*img)(offset.x+x, offset.y+y); }
        /***/ type& operator () (int x, int y) /***/ { return (*img)(offset.x+x, offset.y+y); }

        bool  operator == (frame f) const { return img == f.img && offset == f.offset && size == f.size; }
        bool  operator != (frame f) const { return img != f.img || offset != f.offset || size != f.size; }

        explicit operator xywh () const { return xywh(offset.x, offset.y, size.x, size.y); }
        explicit operator xyxy () const { return xywh(*this); }

        [[nodiscard]]
        frame crop (xywh r) const
        {
            r = xywh(*this) & (r + offset);
            frame f; f.img = img;
            f.offset = xy(r.x, r.y);
            f.size   = xy(r.w, r.h);
            return f;
        }

        frame from (xy p) const { return from(p.x, p.y); }
        frame from (int x, int y) const
        {
            frame f; f.img = img;
            f.offset = xy(offset.x + x, offset.y + y);
            f.size   = xy(size.x - x, size.y - y);
            if (f.size.x <= 0 or
                f.size.y <= 0)
                f.size = xy{};
            return f;
        }

        void  fill (const type & c)
        {
            for (int y=0; y<size.y; y++)
            for (int x=0; x<size.x; x++) (*this)(x,y) = c;
            img->updates += xywh(*this);
        }
        void  blend (const type & c, uint8_t alpha = 255)
        {
            if (alpha == 0) return;
            for (int y=0; y<size.y; y++)
            for (int x=0; x<size.x; x++) (*this)(x,y).blend (c, alpha);
            img->updates += xywh(*this);
        }

        void copy_to   (frame f) const { f.copy_from (*this); }
        void copy_from (frame f)
        {
            int w = min(size.x, f.size.x); if (w <= 0) return;
            int h = min(size.y, f.size.y); if (h <= 0) return;
            for (int y=0; y<h; y++)
            for (int x=0; x<w; x++) (*this)(x,y) = f(x,y);
            img->updates += xywh(offset.x, offset.y, w, h);
        }

        void blend_to   (frame f, uint8_t alpha = 255) const { f.blend_from(*this, alpha); }
        void blend_from (frame f, uint8_t alpha = 255)
        {
            if (alpha == 0) return;
            int w = min(size.x, f.size.x); if (w <= 0) return;
            int h = min(size.y, f.size.y); if (h <= 0) return;
            for (int y=0; y<h; y++)
            for (int x=0; x<w; x++)
                (*this)(x,y).blend(f(x,y), alpha);
            img->updates += xywh(offset.x, offset.y, w, h);
        }
        void copy_to    (image<type>& i) const { copy_to  (i.crop()); }
        void copy_from  (image<type>& i)       { copy_from(i.crop()); }
        void copy_from  (image<type>&&i)       { copy_from(i.crop()); }
        void blend_to   (image<type>& i, uint8_t alpha = 255) const { blend_to  (i.crop(), alpha); }
        void blend_from (image<type>& i, uint8_t alpha = 255)       { blend_from(i.crop(), alpha); }
        void blend_from (image<type>&&i, uint8_t alpha = 255)       { blend_from(i.crop(), alpha); }

        void copy_from (view<T> v) {
            int w = min(size.x, v.size.x); if (w <= 0) return;
            int h = min(size.y, v.size.y); if (h <= 0) return;
            for (int y=0; y<h; y++)
            for (int x=0; x<w; x++) (*this)(x,y) = v(x,y);
            img->updates += xywh(offset.x, offset.y, w, h);
        }
        void copy_to (view<T> v) const {
            int w = min(size.x, v.size.x); if (w <= 0) return;
            int h = min(size.y, v.size.y); if (h <= 0) return;
            for (int y=0; y<h; y++)
            for (int x=0; x<w; x++) v(x,y) = (*this)(x,y);
        }
        void blend_from (view<T> v, uint8_t alpha = 255) {
            if (alpha == 0) return;
            int w = min(size.x, v.size.x); if (w <= 0) return;
            int h = min(size.y, v.size.y); if (h <= 0) return;
            for (int y=0; y<h; y++)
            for (int x=0; x<w; x++) (*this)(x,y).blend(v(x,y), alpha);
            img->updates += xywh(offset.x, offset.y, w, h);
        }
        void blend_to (view<T> v, uint8_t alpha = 255) const {
            if (alpha == 0) return;
            int w = min(size.x, v.size.x); if (w <= 0) return;
            int h = min(size.y, v.size.y); if (h <= 0) return;
            for (int y=0; y<h; y++)
            for (int x=0; x<w; x++) v(x,y).blend((*this)(x,y), alpha);
        }

        frame& copy  (line, color, double width=1);
        frame& blend (line, color, double width=1);
        frame& copy  (segment s, color c) { return copy (line{s.p1, s.p2}, c); }
        frame& blend (segment s, color c) { return blend(line{s.p1, s.p2}, c); }
        frame& copy  (circle, color, double width);
        frame& blend (circle, color, double width);
        frame& copy  (circle, color);
        frame& blend (circle, color);
    };
} 
