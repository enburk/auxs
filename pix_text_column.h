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
                row->offset.x += offset.x; // !!!
                row->offset.y = size.y;
                size.y += row->Height();
                size.x = max(size.x,
                row->offset.x +
                row->Width());
            }
        }

        void render (frame<rgba> frame, xy shift=xy{}, uint8_t alpha=255)
        {
            for (auto& r: rows)
            {
                r->render(frame, shift /*+ offset*/, alpha);
            }
        }

        place pointed (xy p, bool virtual_space)
        {
            if (rows.empty()) return {};

            row r{.offset=xy{0,p.y}};
            auto i = rows.lower_bound(&r,
                [](auto a, auto b){ return
                a->offset.y <
                b->offset.y; });

            if (i == rows.end()) {
                p.x = max<int>();
                i--; }

            return (*i)->pointed(p.x,
                virtual_space);
        }
    };
}
