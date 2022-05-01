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

        void format ()
        {
            if (format.width == max<int>() and
               (format.alignment.x != left or
                format.columns > 1)) throw
                std::out_of_range(
                "bad text format");

            int n = format.columns;
            int g = format.gutter;
            int w = (format.width - g*(n-1)) / n;

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
                size.x += w;
            }
            columns.back ().format.ellipsis = format.ellipsis;
            columns.front().format.lwrap = format.lwrap;
            columns.back ().format.rwrap = format.rwrap;

            //for (auto& line: lines)
            //for (auto row: line.rows(column->format))
            //
            //
            //auto column = columns.begin();
            //for (auto& line: lines)
            //{
            //    for (auto row: line.rows(column->format))
            //    {
            //        if (column->add(row)) continue;
            //        if (column == columns.end()-1) {
            //            column->add(row, true);
            //            break;
            //        }
            //        column++;
            //        column->add(row, true);
            //    }
            //}

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
                c.render(frame, shift + c.offset, alpha);
        }
    };
}
