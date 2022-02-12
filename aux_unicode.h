#pragma once
#include "aux_abc.h"
namespace aux::unicode
{
    array<str> glyphs (str s)
    {
        array<str> glyphs;
        glyphs.reserve(s.size());

        auto check = [&s](auto i){ if (i == s.end())
        throw std::runtime_error("unicode: broken UTF-8"); };

        for (auto i = s.begin(); i != s.end(); )
        {
            char c = *i++; str g = c;
            uint8_t u = static_cast<uint8_t>(c);
            if ((u & 0b11000000) == 0b11000000) { check(i); g += *i++; // 110xxxxx 10xxxxxx
            if ((u & 0b11100000) == 0b11100000) { check(i); g += *i++; // 1110xxxx 10xxxxxx 10xxxxxx
            if ((u & 0b11110000) == 0b11110000) { check(i); g += *i++; // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
            }}}
            glyphs += g;
        }
        return glyphs;
    }

    int length (str s)
    {
        int n = 0;
        for (auto i = s.begin(); i != s.end(); )
        {
            char c = *i++; n++;
            uint8_t u = static_cast<uint8_t>(c);
            if ((u & 0b11000000) == 0b11000000) { if (i == s.end()) break; n++; i++;
            if ((u & 0b11100000) == 0b11100000) { if (i == s.end()) break; n++; i++;
            if ((u & 0b11110000) == 0b11110000) { if (i == s.end()) break; n++; i++;
            }}}
        }
        return n;
    }
}
