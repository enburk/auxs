#pragma once
#include "gui_widget_text_aux_line.h"
namespace gui::text
{
    struct lines : widgetarium<line>
    {
        static void skip (array<XY>& bars, int height)
        {
            while (not bars.empty()) {
                auto& bar = bars.front();
                bar.y -= height;
                if (bar.y >= 0)
                    break;
                height = -bar.y;
                bars.erase(0);
            }
        }

        void fill (array<doc::view::line> datae)
        {
            int n = 0;
            int width = 0;
            int height = 0;

            reserve(datae.size());

            for (auto && data : datae)
            {
                auto refill = [&]()
                {
                    if (size() <= n) return true;

                    if ((*this)(n) == data) return false;

                    if (size() == datae.size()) return true;

                    // style could be changed by syntax highlighting
                    const auto & tt1 = (*this)(n).tokens;
                    const auto & tt2 = data.tokens;
                    bool same_text = tt1.size() == tt2.size();
                    if (same_text)
                        for (int i=0; i<tt1.size(); i++)
                            if (tt1[i].text != tt2[i].text)
                                { same_text = false; break; }

                    if (same_text) return true;

                    if (size() < datae.size())
                    { // lines were inserted
                        emplace_back() = std::move(data);
                        rotate(n, size()-1, size());
                        return false;
                    }
                    else
                    { // lines were removed
                        rotate(n, size()-datae.size()+n, size());
                        truncate(datae.size());
                        return (*this)(n) != data;
                    }
                };

                if (refill()) {
                    skip(data.format.lwrap, height);
                    skip(data.format.rwrap, height);
                    at(n) = std::move(data);
                }

                line & line = at(n);
                line.move_to(XY(0, height));
                width = max(width, line.coord.now.size.x);
                height += line.coord.now.size.y;
                n++;
            }

            truncate(n);

            if (n > 0) at(n-1).last = true;

            resize(XY(width, height));
        }        

        bool focusable_now () override { return false; }

        XYWH bar (place place, bool virtual_space)
        {
            if (place.line < 0) return XYWH{};
            if (place.line >= size()) return XYWH{};
            if (place.offset < 0)
                place.offset = 0;

            auto& line = at(place.line);

            if (place.offset >= line.length)
                place.offset -= line.length;
            else return
                line.bar(place.offset) +
                line.coord.now.origin;

            if (not virtual_space)
                place.offset = 0;

            pix::glyph space (" ", line.style);

            XYWH r = line.coord.now;
            r.x = r.x + r.w + place.offset * space.advance;
            r.w = space.advance;
            return r;
        }

        array<XYWH> bars (range range, bool virtual_space)
        {
            array<XYWH> bars;
            auto[from, upto] = range;
            if  (from> upto) std::swap
                (from, upto);
        
            for (; from.line <= upto.line;
                from.line++, from.offset = 0)
            {
                if (from.line >= size()) break;
                int from_offset = from.offset;
                int upto_offset = from.line == upto.line ?
                    upto.offset : max<int>();

                array<XYWH> rr =
                at (from.line).bars(
                    from_offset,
                    upto_offset,
                    virtual_space);
                for (auto& r: rr) r +=
                at (from.line).coord.now.origin;
                if (rr.empty()) continue;
                if (bars.size() > 0 and rr.size() == 1
                and bars.back().x == rr.back().x
                and bars.back().w == rr.back().w)
                    bars.back() |= rr.back(); else
                    bars += rr;
            }
            return bars;
        }

        place point (XY p, bool virtual_space)
        {
            int l = 0;

            for (auto& line : *this)
            {
                XYWH r = line.coord.now;
                if (r.y + r.h > p.y or line.last) return
                {
                    l, line.point(
                    p - r.origin,
                    virtual_space)
                };
                else l++;
            }

            return {};
        }

        struct row_type
        {
            int length = 0;
            int indent = 0;
        };

        auto rows()
        {
            int n = 0;
            for (auto& line : *this)
            n += line.rows.size();
            return n;
        }

        auto row(int n)
        {
            for (auto& line : *this)
            if (n >= line.rows.size())
                n -= line.rows.size();
            else return row_type{
                line.rows[n].length,
                line.rows[n].indent};
            return row_type{};
        }

        place lines2rows(place p)
        {
            int r = 0;
            int l = p.line;
            int o = p.offset;

            if (l > size()-1)
                l = size()-1;
            if (l < 0) return place{};

            for (int i=0; i<l; i++)
            r += at(i).rows.size();

            for (auto& row : at(l).rows)
                if (not row.the_last_row and
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
            for (auto& line : *this)
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