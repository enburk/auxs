#pragma once
#include "doc_html_model.h"
#include "gui_widget_text_aux_lines.h"
namespace gui::text
{
    struct cell:
    widget<cell>
    {
        using color_range = std::pair<range, RGBA>;
        using color_bars = widgetarium<canvas>;

        color_bars highlight_bars;
        color_bars selection_bars; lines lines;
        widgetarium<caret> carets;

        unary_property<array<range>> highlights;
        unary_property<array<range>> selections;

        binary_property<bool> virtual_space = false;
        binary_property<bool> insert_mode = true;

        void fill(array<doc::view::line> data)
        {
            lines.fill(std::move(data));
            resize(XY(virtual_space.now? max<int>()/2:
                lines.coord.now.size.x,
                lines.coord.now.size.y));
        }

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                XYWH r = coord.now.local();
                highlight_bars.coord = r;
                selection_bars.coord = r;
                carets.coord = r;
            }
            if (what == &highlights)
            {
                int n = 0;
                for (auto range: highlights.now)
                for (XYWH r: lines.bars(range, virtual_space)) {
                    auto& bar = highlight_bars(n++);
                    bar.color = skins[skin.now].highlight.first;
                    bar.coord = r; }

                highlight_bars.truncate(n);
            }
            if (what == &selections)
            {
                int n = 0;
                for (auto range: selections.now)
                for (XYWH r: lines.bars(range, virtual_space)) {
                    auto& bar = selection_bars(n++);
                    bar.color = skins[skin.now].selection.first;
                    bar.coord = r; }

                selection_bars.truncate(n);
            }
            if (what == &selections
            or  what == &insert_mode
            or  what == &focus_on)
            {
                int n = 0;
                for (auto range: selections.now)
                    carets(n++).coord = lines.bar(
                        range.upto, true); // could be after end of line

                carets.truncate(n);

                for (auto& caret: carets) {
                    caret.insert_mode = insert_mode.now;
                    caret.show(focus_on.now);
                }
            }

            notify(what);
        }

        str selected () const
        {
            str s;

            for (auto [from, upto] : selections.now)
            {
                if (from > upto) std::swap (from, upto);

                for (; from.line <= upto.line; from.line++, from.offset = 0)
                {
                    if (from.line >= lines.size()) break;
                    int from_offset = from.offset;
                    int upto_offset = from.line == upto.line ?
                        upto.offset : lines(from.line).length;
                    if (from_offset >= upto_offset) continue;

                    int offset = 0;

                    for (auto & token : lines(from.line))
                    {
                        if (upto_offset <= offset) break;
                        if (from_offset <= offset + token.size() - 1)
                        {
                            int from_glyph = max(from_offset - offset, 0);
                            int upto_glyph = min(upto_offset - offset, token.size());

                            for (auto& g: token.glyphs
                                .from(from_glyph)
                                .upto(upto_glyph))
                                s += g.text;
                        }
                        offset += token.size();
                    }
                }
            }

            return s;
        }

        XYWH bar(place place) {
            return lines.bar(place,
                virtual_space.now); }

        generator<XYWH> bars(range range) {
            for (XYWH bar: lines.bars(range, virtual_space))
                co_yield bar; }

        place point (XY p) { return lines.
            point(p, virtual_space.now); }

        auto rows() { return lines.rows(); }
        auto row(int n) { return lines.row(n); }
        place lines2rows(place p) { return lines.lines2rows(p); }
        place rows2lines(place p) { return lines.rows2lines(p); }
    };
}
