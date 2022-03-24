#pragma once
#include "pix_image.h"
namespace pix
{
    template<class T> struct frame
    {
        typedef T type; image<type> * image; XY offset, size;

        frame (                        ) : image (nullptr) {}
        frame (pix::image<type> & image) : image (&image), size(image.size) {}

        const type& operator () (int x, int y) const { return (*image)(offset.x+x, offset.y+y); }
        /***/ type& operator () (int x, int y) /***/ { return (*image)(offset.x+x, offset.y+y); }

        bool  operator == (frame f) const { return image == f.image && offset == f.offset && size == f.size; }
        bool  operator != (frame f) const { return image != f.image || offset != f.offset || size != f.size; }

        explicit operator XYWH () const { return XYWH (offset.x, offset.y, size.x, size.y); }
        explicit operator XYXY () const { return XYWH (*this); }

        [[nodiscard]] frame crop (XYWH r) const
        {
            r = XYWH(*this) & (r + offset);
            frame f; f.image = image;
            f.offset = XY(r.x, r.y);
            f.size   = XY(r.w, r.h);
            return f;
        }

        frame from (XY p) const { return from(p.x, p.y); }
        frame from (int x, int y) const
        {
            frame f; f.image = image;
            f.offset = XY(offset.x + x, offset.y + y);
            f.size   = XY(size.x - x, size.y - y);
            if (f.size.x <= 0 or
                f.size.y <= 0)
                f.size = XY{};
            return f;
        }

        void  fill (const type & c)
        {
            for (int y=0; y<size.y; y++)
            for (int x=0; x<size.x; x++) (*this)(x,y) = c;
            image->updates += XYWH(*this);
        }
        void  blend (const type & c, uint8_t alpha = 255)
        {
            if (alpha == 0) return;
            for (int y=0; y<size.y; y++)
            for (int x=0; x<size.x; x++) (*this)(x,y).blend (c, alpha);
            image->updates += XYWH(*this);
        }

        void copy_to   (frame f) const { f.copy_from (*this); }
        void copy_from (frame f)
        {
            int w = min(size.x, f.size.x); if (w <= 0) return;
            int h = min(size.y, f.size.y); if (h <= 0) return;
            for (int y=0; y<h; y++)
            for (int x=0; x<w; x++) (*this)(x,y) = f(x,y);
            image->updates += XYWH(offset.x, offset.y, w, h);
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
            image->updates += XYWH(offset.x, offset.y, w, h);
        }

        void copy_from (view<T> v) {
            int w = min(size.x, v.size.x); if (w <= 0) return;
            int h = min(size.y, v.size.y); if (h <= 0) return;
            for (int y=0; y<h; y++)
            for (int x=0; x<w; x++) (*this)(x,y) = v(x,y);
            image->updates += XYWH(offset.x, offset.y, w, h);
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
            image->updates += XYWH(offset.x, offset.y, w, h);
        }
        void blend_to (view<T> v, uint8_t alpha = 255) const {
            if (alpha == 0) return;
            int w = min(size.x, v.size.x); if (w <= 0) return;
            int h = min(size.y, v.size.y); if (h <= 0) return;
            for (int y=0; y<h; y++)
            for (int x=0; x<w; x++) v(x,y).blend((*this)(x,y), alpha);
        }
    };
} 