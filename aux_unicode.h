#pragma once
#include "aux_coro.h"
#include "aux_string.h"
namespace aux::unicode
{
    struct glyph
    {
        char cc[4]{};

        glyph() = default;
        glyph(char c) { cc[0] = c; }
        glyph(char8_t u[4]) {
            cc[0] = u[0];
            cc[1] = u[1];
            cc[2] = u[2];
            cc[3] = u[3];
        }

        template<int N> glyph(const char(&CC)[N]) { // includes '\0'
            static_assert(0 <= N and N <= 5, "wrong glyph size");
            int i = 0; for (auto C : CC) if (i<4) cc[i++] = C;
        }
        template<int N> glyph(const char8_t(&CC)[N]) { // includes '\0'
            static_assert(0 <= N and N <= 5, "wrong glyph size");
            int i = 0; for (auto C : CC) if (i<4) cc[i++] = (char)(C);
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

        static char ascii_tolower (char c) { return 'A' <= c && c <= 'Z' ? c - 'A' + 'a' : c; }
        static char ascii_toupper (char c) { return 'a' <= c && c <= 'z' ? c - 'a' + 'A' : c; }

        glyph ascii_lowercased () const { glyph g = *this; g.cc[0] = ascii_tolower(g.cc[0]); return g; }
        glyph ascii_uppercased () const { glyph g = *this; g.cc[0] = ascii_toupper(g.cc[0]); return g; }

        bool is_ascii_uppercased () const { return 'A' <= cc[0] and cc[0] <= 'Z'; }
        bool is_ascii_lowercased () const { return 'a' <= cc[0] and cc[0] <= 'z'; }
    };

    generator<glyph> glyphs (str const& s)
    {
        auto check = [&s](auto i){ if (i == s.end() or
            (static_cast<uint8_t>(*i) & 0b11000000) != 0b10000000)
            throw std::runtime_error("unicode: broken UTF-8");
        };

        for (auto i = s.begin(); i != s.end(); )
        {
            glyph g; int n = 0;
            char c = *i++; g.cc[n++] = c;
            auto u = static_cast<uint8_t>(c);
            if ((u & 0b11000000) == 0b11000000) { check(i); g.cc[n++] = *i++; // 110xxxxx 10xxxxxx
            if ((u & 0b11100000) == 0b11100000) { check(i); g.cc[n++] = *i++; // 1110xxxx 10xxxxxx 10xxxxxx
            if ((u & 0b11110000) == 0b11110000) { check(i); g.cc[n++] = *i++; // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
            }}}

            if (n <= 2 and i != s.end())
            if (static_cast<uint8_t>(*i) == 0xCC  // combining accents etc...
            or  static_cast<uint8_t>(*i) == 0xCD) // combining accents etc...
            {
                g.cc[n++] = *i++; check(i);
                g.cc[n++] = *i++;
            }

            co_yield g;
        }
    }

    bool ok (str const& s)
    {
        try { for (auto g: s) ; }
        catch (...) { return false; }
        return true;
    }

    std::vector<glyph> array (str s)
    {
        std::vector<glyph> gg;
        gg.reserve(s.size());
        for (auto g: glyphs(s))
        gg.push_back(g);
        return gg;
    }

    int length (str s)
    {
        int  n = 0;
        auto i = s.begin();
        auto e = s.end();
        while (i != e)
        {
            auto c = [&i](){ return static_cast<uint8_t>(*i); };
            auto u = static_cast<uint8_t>(c());

            if ((c() == 0xCC)) { i++; // Combining Diacritical Marks
            if ((c() >= 0x80)) { i++; continue; } else i--; }
            if ((c() == 0xCD)) { i++; // Combining Diacritical Marks
            if ((c() <= 0xAF)) { i++; continue; } else i--; }

            if ((u & 0b11000000) == 0b11000000) { i++; if (i == e) break;
            if ((u & 0b11100000) == 0b11100000) { i++; if (i == e) break;
            if ((u & 0b11110000) == 0b11110000) { i++; if (i == e) break;
            }}}
            i++;
            n++;
        }
        return n;
    }

    str string (auto glyphs)
    {
        str s; int n = 0;
        for (auto g : glyphs) n += g.size(); s.reserve(n);
        for (auto g : glyphs) s += g.string();
        return s;
    }

    str what (str); // std::exception::what to UTF-8
}
namespace aux
{
    void str::align_left (int n, char padding)
    {
        int m = unicode::length(*this);
        if (m < n) *this += str(padding, n-m);
    }
    void str::align_right (int n, char padding)
    {
        int m = unicode::length(*this);
        if (m < n) *this = str(padding, n-m) + *this;
    }
}
