#pragma once
#include "aux_abc.h"
namespace doc::text
{
    using namespace data;

    struct glyph
    {
        char cc[4] = {};

        glyph () = default;

        template<int N> glyph(const char(&CC)[N]) {
            static_assert(0 <= N && N <= 4, "wrong glyph size");
            int i = 0; for (auto C : CC) cc[i++] = C;
        }
        template<int N> glyph(const char8_t(&CC)[N]) {
            static_assert(0 <= N && N <= 4, "wrong glyph size");
            int i = 0; for (auto C : CC) cc[i++] = (char)(C);
        }

        bool operator ==  (glyph const&) const = default;
        bool operator !=  (glyph const&) const = default;
        auto operator <=> (glyph const&) const = default;

        operator str () const { return string(); }

        int size () const
        {
            return
            cc[0] == '\0' ? 0 :
            cc[1] == '\0' ? 1 :
            cc[2] == '\0' ? 2 :
            cc[3] == '\0' ? 3 : 4;
        }

        str string () const
        {
            str s; for (char c : cc)
            if (c) s += c; else break;
            return s;
        }

        bool letter () const
        {
            if (size() == 1) { char c = cc[0];
                return (c >= 'A' && c <= 'Z')
                    || (c >= 'a' && c <= 'z');
            }
            else
            if (size() == 2) {
                auto x = static_cast<uint8_t>(cc[0]);
                auto y = static_cast<uint8_t>(cc[1]);
                if (x == 0xC3
                and y >= 0x80 // Latin-1 Supplement
                and y <= 0xBF // Latin-1 Supplement
                and y != 0x97 // multiplication sign
                and y != 0xB7 // division sign
                ) return true;
                if (x >= 0xC4 // Latin Extended-A
                and x <= 0xDD // Arabic
                ) return true;
            }
            return false;
        }

        bool digit () const
        {
            if (size() == 1) { char c = cc[0];
                return (c >= '0' && c <= '9');
            }
            return false;
        }

        bool space () const
        {
            if (size() == 1) { char c = cc[0];
                return (c == ' ' || c =='\t');
            }
            return false;
        }

        bool ascii () const
        {
            if (size() == 1) { char c = cc[0];
                return (c >= ' ' && c <= '~');
            }
            return false;
        }

        static array<glyph> parse (const str & s)
        {
            array<glyph> glyphs; glyphs.reserve(s.size());

            auto check = [&s](auto i){ if (i == s.end() ||
                (static_cast<uint8_t>(*i) & 0b11000000) != 0b10000000 )
                    throw std::runtime_error("unicode: broken UTF-8"); };

            for (auto i = s.begin(); i != s.end(); )
            {
                glyph g; int n = 0;
                char c = *i++; g.cc[n++] = c;
                auto u = static_cast<uint8_t>(c);

                if ((u & 0b11000000) == 0b11000000) { check(i); g.cc[n++] = *i++; // 110xxxxx 10xxxxxx
                if ((u & 0b11100000) == 0b11100000) { check(i); g.cc[n++] = *i++; // 1110xxxx 10xxxxxx 10xxxxxx
                if ((u & 0b11110000) == 0b11110000) { check(i); g.cc[n++] = *i++; // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
                }}}
                
                glyphs += g;
            }

            return glyphs;
        }
    };

    template<class X>
    inline str string (X glyphs)
    {
        str s; int n = 0;
        for (auto g : glyphs) n += g.size(); s.reserve(n);
        for (auto g : glyphs) s += g.string();
        return s;
    }
}
