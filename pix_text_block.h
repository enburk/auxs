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
            auto f = columns.front().format;
            f.height = max<int>();
            int n = 0;

            for (auto& line: lines)
            {
                for (auto row: line.ptrrows(f))
                {
                    row->from.line = n;
                    columns.front().rows += row;
                    int h = row->Height();
                    line::skip(f.lwrap, h);
                    line::skip(f.rwrap, h);
                    H += h;
                }
                n++;
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

        auto bars (range range, bool virtual_space)
        {
            array<xywh> bars;
 
            auto[from, upto] = range;
            if  (from> upto) std::swap
                (from, upto);

            if (from.line < 0) from = place{};
            if (upto.line >= lines.size()) {
                upto.line  = lines.size()-1;
                upto.offset = max<int>(); }
        
            for (;
            from.line <= upto.line;
            from.line++, from.offset = 0)
            {
                for (xywh r: lines[from.line].bars(
                from.offset, from.line == upto.line ?
                upto.offset : max<int>(), virtual_space))
                {
                    if (bars.size() > 0
                    and bars.back().x == r.x
                    and bars.back().w == r.w)
                        bars.back() |= r; else
                        bars += r;
                }
            }
            return bars;
        }
        
        place pointed (xy p, bool virtual_space)
        {
            if (p.x < 0) p.x = 0;
        
            for (auto& c: reverse(columns))
                if (not c.rows.empty() and
                    p.x >= c.offset.x) return
                    c.pointed(p - c.offset,
                        virtual_space);
        
            return {};
        }

        struct row_data
        {
            int length = 0;
            int indent = 0;
        };

        auto rows()
        {
            int n = 0;
            for (auto& line: lines)
            n += line.rows.size();
            return n;
        }

        auto row(int n)
        {
            for (auto& line: lines)
            if (n >= line.rows.size())
                n -= line.rows.size();
            else return row_data{
                line.rows[n].length,
                line.rows[n].indent};
            return row_data{};
        }

        place lines2rows(place p)
        {
            int r = 0;
            int l = p.line;
            int o = p.offset;

            if (l > lines.size()-1)
                l = lines.size()-1;
            if (l < 0) return place{};

            for (int i=0; i<l; i++)
            r += lines[i].rows.size();

            for (auto& row: lines[l].rows)
                if (not row.last and
                    o >= row.length) {
                    o -= row.length;
                    r++; }
                else break;

            return {r,o};
        }

        place rows2lines(place p)
        {
            int l = 0;
            int r = p.line;
            int o = p.offset;
            for (auto& line: lines)
            if (r >= line.rows.size()) {
                r -= line.rows.size();
                l++; }
            else
            {
                for (int i=0; i<r; i++)
                o += line.rows[i].length;
                break;
            }
            return {l,o};
        }
    };
}
