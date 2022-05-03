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
                c.format.ellipsis = false;
                c.format.lwrap.clear();
                c.format.rwrap.clear();
                c.format.width = w;
                c.offset.x = size.x;
                size.x += w + g;
            }
            size.x -= g;
            columns.back ().format.ellipsis = format.ellipsis;
            columns.front().format.lwrap = format.lwrap;
            columns.back ().format.rwrap = format.rwrap;

            for (auto& line: lines)
            for (auto row: line.ptrrows(
                columns.front().format))
                columns.front().rows += row;

            int R = columns.front().rows.size();
            int r = (R + n - 1) / n;
            for (int i=1; i<n; i++)
            for (int j=0; j<r; j++)
            {
                int k = i*r + j;
                if (k >= R) break;
                columns[i].rows += 
                columns.front().rows[k];
            }
            columns.front().rows.resize(r);

            for (auto& c: columns)
            {
                size.y = max(
                size.y, c.size.y);
                c.align();
            }
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
