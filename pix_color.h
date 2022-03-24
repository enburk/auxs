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

        void blend (rgba c, uint8_t alpha = 255) {
            uint8_t
                A = alpha == 255 ? c.a : c.a * alpha >> 8;
            r = A == 255 ? c.r : r + ( A * (c.r - r) >> 8 );
            g = A == 255 ? c.g : g + ( A * (c.g - g) >> 8 );
            b = A == 255 ? c.b : b + ( A * (c.b - b) >> 8 );
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

    struct  MONO
    {
        union { uint8_t value; uint8_t channels [1]; };

        MONO () : value (0) {}
        MONO (int value) : value(value) {}
        MONO (rgba c) { value = aux::clamp
            <uint8_t>(255* (
            0.212671 * c.r +
            0.715160 * c.g +
            0.072169 * c.b)); }

        bool operator == (const MONO & c) const { return value == c.value; }
        bool operator != (const MONO & c) const { return value != c.value; }
        bool operator <  (const MONO & c) const { return value <  c.value; }
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

