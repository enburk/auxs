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

    // HSV

    void rgb2hsv (auto r, auto g, auto b, auto& h, auto& s, auto& v)
    {
        auto M = max(r,g,b);
        auto m = min(r,g,b);
        auto chroma = M - m;

        h = 0; s = 0; v = M;

        if (chroma == 0) return;

        h = M == r ? 0 + (g-b)/chroma:
            M == g ? 2 + (b-r)/chroma:
            M == b ? 4 + (r-g)/chroma: 0;

        h *= 60;
        if (h < 0)
        h += 360;

        if (M > 0)
        s = chroma/M;
    }

    void hsv2rgb (auto h, auto s, auto v, auto& r, auto& g, auto& b)
    {
        r = v; g = v; b = v;

        if (s == 0) return;

        h /= 60;
        auto i = (int)(fmod(h,6));
        auto f = h - i;

        auto p = v * (1.0 - (s));
        auto q = v * (1.0 - (s*f));
        auto t = v * (1.0 - (s*(1.0-f)));

        switch (i) {
        break; case 0: g = t; b = p;
        break; case 1: r = q; b = p;
        break; case 2: r = p; b = t;
        break; case 3: r = p; g = q;
        break; case 4: r = t; g = p;
        break; case 5: g = p; b = q;
        }
    }

    // HSL

    void rgb2hsl (auto r, auto g, auto b, auto& h, auto& s, auto& l)
    {
        double v;
        rgb2hsv(r,g,b, h,s,v);

        l = v * (1 - s/2);
        s = 0 < l and l < 1 ?
        (v-l)/(min(l, 1-l)) : 0;
    }

    void hsl2rgb (auto h, auto s, auto l, auto& r, auto& g, auto& b)
    {
        double v = l + s * min(l, 1-l);

        if (v > 0) s = 2*(1 - l/v);

        hsv2rgb(h,s,v, r,g,b);
    }

    // OK Lab https://bottosson.github.io/posts/oklab/

    void rgb2lab (auto r, auto g, auto b, auto& L, auto& A, auto& B)
    {
        auto l = 0.4122214708f * r + 0.5363325363f * g + 0.0514459929f * b;
	    auto m = 0.2119034982f * r + 0.6806995451f * g + 0.1073969566f * b;
	    auto s = 0.0883024619f * r + 0.2817188376f * g + 0.6299787005f * b;

        auto l_ = cbrt(l);
        auto m_ = cbrt(m);
        auto s_ = cbrt(s);

        L = 0.2104542553f*l_ + 0.7936177850f*m_ - 0.0040720468f*s_;
        A = 1.9779984951f*l_ - 2.4285922050f*m_ + 0.4505937099f*s_;
        B = 0.0259040371f*l_ + 0.7827717662f*m_ - 0.8086757660f*s_;
    }

    void lab2rgb (auto L, auto A, auto B, auto& r, auto& g, auto& b)
    {
        auto l_ = L + 0.3963377774f * A + 0.2158037573f * B;
        auto m_ = L - 0.1055613458f * A - 0.0638541728f * B;
        auto s_ = L - 0.0894841775f * A - 1.2914855480f * B;

        auto l = l_*l_*l_;
        auto m = m_*m_*m_;
        auto s = s_*s_*s_;

		r = +4.0767416621f * l - 3.3077115913f * m + 0.2309699292f * s;
		g = -1.2684380046f * l + 2.6097574011f * m - 0.3413193965f * s;
		b = -0.0041960863f * l - 0.7034186147f * m + 1.7076147010f * s;
    }

    void rgb2hcl (auto r, auto g, auto b, auto& h, auto& c, auto& l)
    {
        double L,A,B;
        rgb2lab(r,g,b, L,A,B);
        h = atan2(B,A);
        c = hypot(A,B);
        l = L;
        h = 180*h/pi;
        if (h < 0)
        h += 360.0;
    }

    void hcl2rgb (auto h, auto c, auto l, auto& r, auto& g, auto& b)
    {
        h = pi*h/180;
        double L = l;
        double A = c*cos(h);
        double B = c*sin(h);
        lab2rgb(L,A,B, r,g,b);
    }

    rgba forecorrected (rgba fore, rgba back)
    {
        if (back == rgba{}) return fore;
        if (back == XRGB(0xFAFAFA)) return fore;
        if (back == XRGB(0x212121)) {
        if (fore == XRGB(0x505050)) return XRGB(0xB0B0B0); // dark
        if (fore == XRGB(0x808080)) return XRGB(0x909090); // gray
        if (fore == XRGB(0xA0A0A0)) return XRGB(0x707070); // light
        if (fore == XRGB(0xC0C0C0)) return XRGB(0x505050); // silver
        if (fore == XRGB(0x008000)) return XRGB(0x00C000); // green 
        if (fore == XRGB(0x000080)) return XRGB(0x29B6F6); // navy  
        if (fore == XRGB(0x800080)) return XRGB(0xBB86FC); // purple
        if (fore == XRGB(0x808000)) return XRGB(0xD0D000); // olive 
        if (fore == XRGB(0x008080)) return XRGB(0x00D0D0); // teal  
        if (fore == XRGB(0xFF0000)) return XRGB(0xFF0266); // red      
        if (fore == XRGB(0x00FF00)) return XRGB(0x00FF00); // lime     
        if (fore == XRGB(0x0000FF)) return XRGB(0x2196F3); // blue     
        if (fore == XRGB(0xFF00FF)) return XRGB(0xFF00FF); // fuchsia  
        if (fore == XRGB(0xFFFF00)) return XRGB(0xFFFF00); // yellow   
        if (fore == XRGB(0xBF360C)) return XRGB(0xFFB74D); // yellow   
        if (fore == XRGB(0x00FFFF)) return XRGB(0x00FFFF); // aqua     
        if (fore == XRGB(0xFFBF00)) return XRGB(0xFFD54F); // amber     
        if (fore == XRGB(0xB00020)) return XRGB(0xEF5350); // error     
        }

        double r1 = fore.r/255.0; double r2 = back.r/255.0;
        double g1 = fore.g/255.0; double g2 = back.g/255.0;
        double b1 = fore.b/255.0; double b2 = back.b/255.0;

        double h1,s1,v1; rgb2hsl(r1,g1,b1, h1,s1,v1);
        double h2,s2,v2; rgb2hsl(r2,g2,b2, h2,s2,v2);

        h1 = h1 + h2;
        if (h1 >= 360.0)
            h1 -= 360.0;

        v1 = 1.0 - v1;
        v1 = v2 < 0.5?
        v2 + v1 * (1.0 - v2):
        v2 - v1 * v2;
        
        hsl2rgb(h1,s1,v1, r1,g1,b1);

        fore.r = aux::clamp(255.0*r1, 0, 255);
        fore.g = aux::clamp(255.0*g1, 0, 255);
        fore.b = aux::clamp(255.0*b1, 0, 255);
        return fore;
    }
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

