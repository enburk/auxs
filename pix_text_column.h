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
                row->offset.x = offset.x; // !!!
                row->offset.y = size.y;
                size.y += row->Height();
                size.x = max(size.x,
                row->Width());
            }

            if (format.alignment.y == center
            and format.height > size.y ) { int d = 
                format.height - size.y;
                size.y = format.height;
                for (auto row: rows)
                row->offset.y += d/2;
            }
            else
            if (format.alignment.y == bottom
            and format.height > size.y ) { int d = 
                format.height - size.y;
                size.y = format.height;
                for (auto row: rows)
                row->offset.y += d;
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

            int first = 0;
            int len = rows.size();
            while (len > 0) {
                int half = len / 2;
                int middle = first + half;
                if (rows[middle]->offset.y +
                    rows[middle]->Height() < p.y) {
                    first = middle + 1;
                    len = len - half - 1;
                } else
                    len = half;
            }
            
            if (p.y < 0) p.x = 0;

            if (first == rows.size()) {
                first--; return {
                rows[first]->from.line,
                rows[first]->from.offset +
                rows[first]->length}; }

            return rows[first]->pointed(
                p.x, virtual_space);
        }
    };
}
