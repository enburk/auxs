#pragma once
#include "pix_coord.h"
namespace pix
{
    template<class T> struct view
    {
        typedef T type;
        type * ptr = nullptr;
        XY size; int pitch = 0;

        view () = default;
        view (type * ptr, XY size, int pitch) : ptr (ptr), size(size), pitch(pitch) {}
        const type & operator () (int x, int y) const { return *(ptr + y*pitch + x); }
        /***/ type & operator () (int x, int y) /***/ { return *(ptr + y*pitch + x); }

        view from (XY p) const { return from(p.x, p.y); }
        view from (int x, int y) const
        {
            int w = size.x - x; if (w <= 0) return view();
            int h = size.y - y; if (h <= 0) return view();
            return view (ptr + y*pitch + x, XY(w,h), pitch);
        }

        XY  fill (const type & c)
        {
            for (int y=0; y<size.y; y++)
            for (int x=0; x<size.x; x++) (*this)(x,y) = c;
            return size;
        }
        XY  blend (const type & c, uint8_t alpha = 255)
        {
            if (alpha == 0) return XY();
            for (int y=0; y<size.y; y++)
            for (int x=0; x<size.x; x++) (*this)(x,y).blend(c, alpha);
            return size;
        }

        XY  copy_to   (view v) const { return v.copy_from(*this); }
        XY  copy_from (view v)
        {
            int w = min(size.x, v.size.x); if (w <= 0) return XY();
            int h = min(size.y, v.size.y); if (h <= 0) return XY();
            for (int y=0; y<h; y++)
            for (int x=0; x<w; x++) (*this)(x,y) = v(x,y);
            return XY(w,h);
        }

        XY  blend_to   (view v, uint8_t alpha=255) const { return v.blend_from(*this, alpha); }
        XY  blend_from (view v, uint8_t alpha=255)
        {
            if (alpha == 0) return XY();
            int w = min(size.x, v.size.x); if (w <= 0) return XY();
            int h = min(size.y, v.size.y); if (h <= 0) return XY();
            for (int y=0; y<h; y++)
            for (int x=0; x<w; x++) (*this)(x,y).blend(v(x,y), alpha);
            return XY(w,h);
        }
    };

    template<class T> struct frame;
    template<class T> struct image
    {
        typedef T type; std::vector<type> data; XY size; rectifier updates;

	    explicit image (               ) : data (                  ), size (0, 0) { updates = XYXY(*this); }
	    explicit image (XY size        ) : data (size.x * size.y   ), size (size) { updates = XYXY(*this); }
	    explicit image (XY size, type c) : data (size.x * size.y, c), size (size) { updates = XYXY(*this); }

        template<typename U> explicit image (frame<U> f) : image (f.size)
        {
            for (int y=0; y<size.y; y++)
            for (int x=0; x<size.x; x++) (*this)(x,y) = type (f(x,y));
        }
        const type& operator () (int x, int y) const { return data[y*size.x + x]; }
        /***/ type& operator () (int x, int y) /***/ { return data[y*size.x + x]; }

        explicit operator XYWH () const { return XYWH (0, 0, size.x, size.y); }
        explicit operator XYXY () const { return XYXY (0, 0, size.x, size.y); }

        frame<type> crop (      ) { return frame<type>(*this); }
        frame<type> crop (XYWH r) { return crop().crop(r); }

	    void resize (XY Size) { data.resize (Size.x * Size.y); size = Size; updates = XYXY(*this); }

        void fill (const type & c) { for (auto & v : data) v = c; updates = XYXY(*this); }
    };

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
