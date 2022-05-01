#pragma once
#include "pix_text_row.h"
namespace pix::text
{
    struct column
    {
        array<row*> rows;
        format format;
        xy offset;
        xy size;

        void clear ()
        {
            rows.clear();
            offset = xy{};
            size = xy{};
        }

        bool add (row row, bool force = false)
        {
        }

        void align ()
        {
        }

        void render (frame<rgba> frame, xy shift=xy{}, uint8_t alpha=255)
        {
            for (auto& x: rows)
                x->render(frame, shift + x->offset, alpha);
        }
    };
}
