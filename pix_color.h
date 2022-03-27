#pragma once
#include <cstdint>
#include <functional>
#include "aux_aux.h"
#include "aux_math.h"
namespace pix
{
    struct rgba
    {
        union
        {
            struct {
            uint8_t b, g, r, a; };
            uint8_t channels [4];
            uint32_t value; // byte-order-dependent
        };

        rgba () : value (0) {}
        rgba (int r, int g, int b, int a = 255) : b(b), g(g), r(r), a(a) {}

        bool operator == (const rgba & c) const { return value == c.value; }
        bool operator != (const rgba & c) const { return value != c.value; }
        bool operator <  (const rgba & c) const { return value <  c.value; }

        static const int color_channels = 3;
        static const int total_channels = 4;

        // a*x + (1-a)*y = a*(x-y) + y
        static uint8_t blend (uint8_t dst, uint8_t src, uint8_t a)
        {
            return a == 255 ? src : (a*(src-dst) >> 8) + dst;
        }
        void blend (rgba c)
        {
            r = blend(r, c.r, c.a);
            g = blend(g, c.g, c.a);
            b = blend(b, c.b, c.a);
        }
        void blend (rgba c, uint8_t alpha)
        {
            uint8_t A = alpha == 255 ? c.a : (c.a+1)*(alpha+1) >> 8;
            r = blend(r, c.r, A);
            g = blend(g, c.g, A);
            b = blend(b, c.b, A);
        }
        // accumulating for 3 and more planes
        // x1 (below), x2 (middle), x3 (above)
        // blend(x1, accum(x2, x3)) == blend(blend(x1, x2), x3)
        // 
        // a2*x2 + (1-a2)*(a1*x1 + (1-a1)*x0) ==
        // a2*x2 + (1-a2)*a1*x1 + (1 - a1 - a2 + a1*a2)*x0 ==
        // [denote a1 + a2 - a1*a2 as A] == A*(a2*x2 + (1-a2)*a1*x1)/A + (1-A)*x0
        // A = a1 + a2 - a1*a2; X = (a2*x2 + (1-a2)*a1*x1) / A; (blending x2 to a1*x1)
        void accumulate (rgba c)
        {
            if (a == 0 or c.a == 255) *this = c; else if (a == 255) blend(c); else
            {
                int A = a;
                a = a + c.a + 1 - ((a+1)*(c.a+1) >> 8); // prevent 0x100 cast to 0x00
                r = (blend((r*A >> 8), c.r, c.a) << 8) / a;
                g = (blend((g*A >> 8), c.g, c.a) << 8) / a;
                b = (blend((b*A >> 8), c.b, c.a) << 8) / a;
            }
        }

        static rgba random(uint8_t l = 64, uint8_t u = 255) { return rgba(
               aux::random<int>(l, u),
               aux::random<int>(l, u),
               aux::random<int>(l, u)); }

        static const rgba
        black,  silver, gray,   white,
        maroon, red,    purple, fuchsia,
        green,  lime,   olive,  yellow,
        navy,   blue,   teal,   aqua,
            
        error,
        amber;
    };

    inline rgba ARGB (uint32_t value)
    {
        rgba c;
        c.a = (value >> 24) & 0xFF;
        c.r = (value >> 16) & 0xFF;
        c.g = (value >>  8) & 0xFF;
        c.b = (value      ) & 0xFF;
        return c;
    }
    inline rgba XRGB (uint32_t value)
    {
        rgba c;
        c.a = 0xFF;
        c.r = (value >> 16) & 0xFF;
        c.g = (value >>  8) & 0xFF;
        c.b = (value      ) & 0xFF;
        return c;
    }

    inline const rgba
    rgba::black  = XRGB(0x000000), rgba::silver  = XRGB(0xC0C0C0),
    rgba::maroon = XRGB(0x800000), rgba::red     = XRGB(0xFF0000),
    rgba::green  = XRGB(0x008000), rgba::lime    = XRGB(0x00FF00),
    rgba::navy   = XRGB(0x000080), rgba::blue    = XRGB(0x0000FF),
    rgba::purple = XRGB(0x800080), rgba::fuchsia = XRGB(0xFF00FF),
    rgba::olive  = XRGB(0x808000), rgba::yellow  = XRGB(0xFFFF00),
    rgba::teal   = XRGB(0x008080), rgba::aqua    = XRGB(0x00FFFF),
    rgba::gray   = XRGB(0x808080), rgba::white   = XRGB(0xFFFFFF),

    rgba::amber  = XRGB(0xFFBF00),
    rgba::error  = XRGB(0xB00020);

    struct  mono
    {
        union { uint8_t value; uint8_t channels [1]; };

        mono () : value (0) {}
        mono (int value) : value(value) {}
        mono (rgba c) { value = aux::clamp
            <uint8_t>(255* (
            0.212671 * c.r +
            0.715160 * c.g +
            0.072169 * c.b)); }

        bool operator == (const mono & c) const { return value == c.value; }
        bool operator != (const mono & c) const { return value != c.value; }
        bool operator <  (const mono & c) const { return value <  c.value; }
    };
}

namespace std
{
    template <> struct hash<pix::rgba>
    {
        std::size_t operator()(const pix::rgba& c) const
        {
            return std::hash<std::uint32_t>()(c.value);
        }
    };
}

