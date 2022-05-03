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

        void align ()
        {
            size = xy{};
            for (auto row: rows)
            {
                row->align();
                row->offset.y = size.y;
                size.y += row->Height();
                size.x = max(
                size.x, row->Width());
            }
        }

        void render (frame<rgba> frame, xy shift=xy{}, uint8_t alpha=255)
        {
            for (auto& r: rows)
            {
                r->render(frame, shift + offset, alpha);
            }
        }
    };
}
