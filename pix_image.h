#pragma once
#include "pix_coord.h"
namespace pix
{
    template<class T> struct frame;
    template<class T> struct image
    {
        typedef T type;
        std::vector<type> data; xy size;
        coord::rectifier<int> updates;

        explicit image (               ) : data(                  ), size(0, 0) { updates = xyxy(*this); }
        explicit image (xy size        ) : data(size.x * size.y   ), size(size) { updates = xyxy(*this); }
        explicit image (xy size, type c) : data(size.x * size.y, c), size(size) { updates = xyxy(*this); }
        template<typename U>
        explicit image (frame<U> f) : image (f.size)
        {
            for (int y=0; y<size.y; y++)
            for (int x=0; x<size.x; x++)
            (*this)(x,y) = type(f(x,y));
        }
        const type& operator () (int x, int y) const { return data[y*size.x + x]; }
        /***/ type& operator () (int x, int y) /***/ { return data[y*size.x + x]; }

        explicit operator xywh () const { return xywh(0, 0, size.x, size.y); }
        explicit operator xyxy () const { return xyxy(0, 0, size.x, size.y); }

        const frame<type> crop (      ) const { return *this; }
        /***/ frame<type> crop (      )       { return *this; }
        const frame<type> crop (xywh r) const { return crop().crop(r); }
        /***/ frame<type> crop (xywh r)       { return crop().crop(r); }

        void resize (xy Size)
        {
            data.resize(Size.x * Size.y);
            size = Size;
            updates = xyxy(*this);
        }

        void fill (const type & c)
        {
            for (auto & v : data) v = c;
            updates = xyxy(*this);
        }
    };

    template<class T> struct view
    {
        typedef T type;
        type * ptr = nullptr;
        xy size; int pitch = 0;

        view () = default;
        view (type * ptr, xy size, int pitch) : ptr (ptr), size(size), pitch(pitch) {}
        const type & operator () (int x, int y) const { return *(ptr + y*pitch + x); }
        /***/ type & operator () (int x, int y) /***/ { return *(ptr + y*pitch + x); }

        view from (xy p) const { return from(p.x, p.y); }
        view from (int x, int y) const
        {
            int w = size.x - x; if (w <= 0) return view();
            int h = size.y - y; if (h <= 0) return view();
            return view (ptr + y*pitch + x, xy(w,h), pitch);
        }

        xy  fill (const type & c)
        {
            for (int y=0; y<size.y; y++)
            for (int x=0; x<size.x; x++) (*this)(x,y) = c;
            return size;
        }
        xy  blend (const type & c, uint8_t alpha = 255)
        {
            if (alpha == 0) return xy();
            for (int y=0; y<size.y; y++)
            for (int x=0; x<size.x; x++) (*this)(x,y).blend(c, alpha);
            return size;
        }

        xy  copy_to   (view v) const { return v.copy_from(*this); }
        xy  copy_from (view v)
        {
            int w = min(size.x, v.size.x); if (w <= 0) return xy();
            int h = min(size.y, v.size.y); if (h <= 0) return xy();
            for (int y=0; y<h; y++)
            for (int x=0; x<w; x++) (*this)(x,y) = v(x,y);
            return xy(w,h);
        }

        xy  blend_to   (view v, uint8_t alpha=255) const { return v.blend_from(*this, alpha); }
        xy  blend_from (view v, uint8_t alpha=255)
        {
            if (alpha == 0) return xy();
            int w = min(size.x, v.size.x); if (w <= 0) return xy();
            int h = min(size.y, v.size.y); if (h <= 0) return xy();
            for (int y=0; y<h; y++)
            for (int x=0; x<w; x++) (*this)(x,y).blend(v(x,y), alpha);
            return xy(w,h);
        }
    };
} 
