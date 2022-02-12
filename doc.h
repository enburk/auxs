#pragma once
#include "pix_text.h"
namespace doc
{
    struct place
    {
        int line = 0;
        int offset = 0;
        auto operator <=> (const place & p) const = default;
    };

    struct range
    {
        place from, upto;
        bool empty () const { return from == upto; }
        bool operator == (const range & r) const = default;
        bool operator != (const range & r) const = default;
    };
}
  