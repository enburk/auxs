#pragma once
#include "pix_text_line.h"
#include "pix_text_column.h"
namespace pix::text
{
    struct block
    {
        array<line> lines;
        array<column> columns;
        format format;
        xy offset;
        xy size;

        void layout ()
        {
            if (format.width == max<int>() and
               (format.alignment.x != left or
                format.columns > 1)) throw
                std::out_of_range(
                "bad text format");

            int n = format.columns;
            int g = format.gutter;
            int w = (format.width - g*(n-1)) / n;

            size = xy{};
            columns.resize(n);
            for (auto& c: columns)
            {
                c.clear();
                c.format = format;
                c.format.lwrap.clear();
                c.format.rwrap.clear();
                c.format.width = w;
                c.offset.x = size.x;
                size.x += w + g;
            }
            size.x -= g;
            columns.front().format.lwrap = format.lwrap;
            columns.back ().format.rwrap = format.rwrap;

            if (format.columns > 1 and
               (format.lwrap.size() > 0 or
                format.rwrap.size() > 0))
                format_dynamic(); else
                format_eager();

            for (auto& c: columns)
            {
                size.y = max(
                size.y, c.size.y);
                c.align();
            }
        }

        void format_eager ()
        {
            int H = 0;
            for (auto& line: lines)
            for (auto row: line.ptrrows(
                columns.front().format)) {
                columns.front().rows += row;
                H += row->Height();
            }

            int cc = format.columns;
            H = min(H/cc, format.height);

            for (int c=0; c<cc; c++)
            {
                int h = 0;
                for (int r=0; r<columns[c].rows.size(); r++)
                {
                    h += columns[c].rows[r]->Height();
                    if (r == 0 or h <= H) continue;

                    if (c+1 < cc) 
                    columns[c+1].rows = 
                    columns[c].rows.from(r);
                    columns[c].rows.resize(r);

                    if (c+1 == cc and format.ellipsis)
                    columns[c].rows.back()->ellipt();
                    break;
                }
            }
        }

        void format_dynamic ()
        {
            format_eager();
        }

        void render (frame<rgba> frame, xy shift=xy{}, uint8_t alpha=255)
        {
            for (auto& c: columns)
            {
                c.render(frame, shift + offset, alpha);
            }
        }
    };
}
