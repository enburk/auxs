#pragma once
#include "gui_widget_text_box.h"
namespace gui::text
{
    struct cell:
    widget<cell>
    {
        using color_range = std::pair<range, rgba>;
        using color_bars = widgetarium<canvas>;

        color_bars highlight_bars;
        color_bars selection_bars; box box;
        widgetarium<caret> carets;

        unary_property<array<range>> highlights;
        unary_property<array<range>> selections;

        property<bool> read_only = true;
        property<bool> insert_mode = true;
        property<bool> virtual_space = false;

#define using(x) decltype(box.x)& x = box.x;
        using(text)
        using(html)
        using(model)
        using(color)
        using(font)
        using(style)
        using(format)
        using(update_text)
        using(update_colors)
        using(update_layout)
        #undef using

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                xywh r = coord.now.local();
                highlight_bars.coord = r;
                selection_bars.coord = r;
                carets.coord = r;
            }

            if (what == &update_text
            or  what == &update_layout
            or  what == &read_only)
            {
                xy size = box.coord.now.size;
                if (not read_only.now) size.x += // for caret
                3*gui::metrics::line::width;
                size.x = max(size.x,
                format.now.width);
                resize(size);
            }

            if (what == &update_text
            or  what == &update_layout)
            {
                highlights = array<range>{};
                if (selections.now != model.now->selections)
                    selections = model.now->selections; else
                    on_change(&selections);
            }

            if (what == &highlights)
            {
                int n = 0;
                for (auto range: highlights.now)
                for (xywh r: bars(range)) {
                    auto& bar = highlight_bars(n++);
                    bar.color = skins[skin.now].highlight.first;
                    bar.coord = r; }

                highlight_bars.truncate(n);
            }

            if (what == &selections)
            {
                model.now->selections = selections.now;

                int n = 0;
                for (auto range: selections.now)
                for (xywh r: bars(range)) {
                    auto& bar = selection_bars(n++);
                    bar.color = skins[skin.now].selection.first;
                    bar.coord = r; }

                selection_bars.truncate(n);
            }

            if (what == &selections
            or  what == &insert_mode
            or  what == &read_only
            or  what == &focus_on)
            {
                int n = 0;
                for (auto range: selections.now)
                    carets(n++).coord = model.now->block.bar(
                        range.upto); // could be after the end of line

                carets.truncate(n);

                for (auto& caret: carets) {
                caret.insert_mode = insert_mode.now;
                caret.show(not read_only
                    and focus_on.now); }
            }

            notify(what);
        }

        str selected () const
        {
            str s; auto& lines = model.now->block.lines;

            for (auto [from, upto] : selections.now)
            {
                if (from > upto) std::swap (from, upto);

                for (;
                from.line <= upto.line;
                from.line++, from.offset = 0, s += "\n")
                {
                    if (from.line >= lines.size()) break;
                    int from_offset = from.offset;
                    int upto_offset = from.line == upto.line ?
                        upto.offset : lines[from.line].length;
                    if (from_offset >= upto_offset) continue;

                    int offset = 0;

                    for (auto & token : lines[from.line].tokens)
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

            if (s != "") s.pop_back(); // last \n
            if (s.contains_only(one_of("\n"))) s = "";
            return s;
        }

        generator<xywh> bars(range range) {
            for (xywh bar: model.now->block.
                bars(range, virtual_space))
                co_yield bar; }

        place pointed (xy p) { return model.now->block.
              pointed (p, virtual_space.now); }

        auto token_placed (place p) { return model.now->block.token_placed(p); }

        auto rows() { return model.now->block.rows(); }
        auto row(int n) { return model.now->block.row(n); }
        place lines2rows(place p) { return model.now->block.lines2rows(p); }
        place rows2lines(place p) { return model.now->block.rows2lines(p); }
    };
}
