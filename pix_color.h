#pragma once
#include <cstdint>
#include <functional>
#include "aux_aux.h"
#include "aux_math.h"
namespace pix
{
    struct RGBA
    {
        union
        {
            struct {
            uint8_t b, g, r, a; };
            uint8_t channels [4];
            uint32_t value; // byte-order-dependent
        };

        RGBA () : value (0) {}
        RGBA (int r, int g, int b, int a = 255) : b(b), g(g), r(r), a(a) {}

        bool operator == (const RGBA & c) const { return value == c.value; }
        bool operator != (const RGBA & c) const { return value != c.value; }
        bool operator <  (const RGBA & c) const { return value <  c.value; }

        static const int color_channels = 3;
        static const int total_channels = 4;

        void blend (RGBA c, uint8_t alpha = 255) {
            uint8_t
                A = alpha == 255 ? c.a : c.a * alpha >> 8;
            r = A == 255 ? c.r : r + ( A * (c.r - r) >> 8 );
            g = A == 255 ? c.g : g + ( A * (c.g - g) >> 8 );
            b = A == 255 ? c.b : b + ( A * (c.b - b) >> 8 );
        }

        static RGBA random(uint8_t l = 64, uint8_t u = 255) { return RGBA(
               aux::random<int>(l, u),
               aux::random<int>(l, u),
               aux::random<int>(l, u)); }

        static const RGBA
        black,  silver, gray,   white,
        maroon, red,    purple, fuchsia,
        green,  lime,   olive,  yellow,
        navy,   blue,   teal,   aqua,
            
        error,
        amber;
    };

    inline RGBA ARGB (uint32_t value)
    {
        RGBA c;
        c.a = (value >> 24) & 0xFF;
        c.r = (value >> 16) & 0xFF;
        c.g = (value >>  8) & 0xFF;
        c.b = (value      ) & 0xFF;
        return c;
    }
    inline RGBA XRGB (uint32_t value)
    {
        RGBA c;
        c.a = 0xFF;
        c.r = (value >> 16) & 0xFF;
        c.g = (value >>  8) & 0xFF;
        c.b = (value      ) & 0xFF;
        return c;
    }

    inline const RGBA
    RGBA::black  = XRGB(0x000000), RGBA::silver  = XRGB(0xC0C0C0),
    RGBA::maroon = XRGB(0x800000), RGBA::red     = XRGB(0xFF0000),
    RGBA::green  = XRGB(0x008000), RGBA::lime    = XRGB(0x00FF00),
    RGBA::navy   = XRGB(0x000080), RGBA::blue    = XRGB(0x0000FF),
    RGBA::purple = XRGB(0x800080), RGBA::fuchsia = XRGB(0xFF00FF),
    RGBA::olive  = XRGB(0x808000), RGBA::yellow  = XRGB(0xFFFF00),
    RGBA::teal   = XRGB(0x008080), RGBA::aqua    = XRGB(0x00FFFF),
    RGBA::gray   = XRGB(0x808080), RGBA::white   = XRGB(0xFFFFFF),

    RGBA::amber  = XRGB(0xFFBF00),
    RGBA::error  = XRGB(0xB00020);

    struct  MONO
    {
        union { uint8_t value; uint8_t channels [1]; };

        MONO () : value (0) {}
        MONO (int value) : value(value) {}
        MONO (RGBA c) { value = aux::clamp
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
    template <> struct hash<pix::RGBA>
    {
        std::size_t operator()(const pix::RGBA& c) const
        {
            return std::hash<std::uint32_t>()(c.value);
        }
    };
}

