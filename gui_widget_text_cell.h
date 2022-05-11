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

        binary_property<bool> virtual_space = false;
        binary_property<bool> insert_mode = true;

        box::text_type& text = box.text;
        box::html_type& html = box.html;
        property<rgba>& color = box.color;
        binary_property<font>& font = box.font;
        binary_property<style>& style = box.style;
        binary_property<format>& format = box.format;
        property<bool>& update_text   = box.update_text;
        property<bool>& update_colors = box.update_colors;
        property<bool>& update_layout = box.update_layout;

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
            or  what == &update_layout)
            {
                resize(xy(virtual_space.now? max<int>()/2:
                box.coord.now.size.x,
                box.coord.now.size.y));

                highlights = array<range>{};
                selections = box.model->selections;
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
            or  what == &focus_on)
            {
                box.model->selections = selections.now;

                int n = 0;
                for (auto range: selections.now)
                    carets(n++).coord = box.model->block.bar(
                        range.upto); // could be after the end of line

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
            str s; auto& lines = box.model->block.lines;

            for (auto [from, upto] : selections.now)
            {
                if (from > upto) std::swap (from, upto);

                for (; from.line <= upto.line; from.line++, from.offset = 0)
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

            return s;
        }

        generator<xywh> bars(range range) {
            for (xywh bar: box.model->block.
                bars(range, virtual_space))
                co_yield bar; }

        place pointed (xy p) { return box.model->block.
            pointed(p, virtual_space.now); }

        auto rows() { return box.model->block.rows(); }
        auto row(int n) { return box.model->block.row(n); }
        place lines2rows(place p) { return box.model->block.lines2rows(p); }
        place rows2lines(place p) { return box.model->block.rows2lines(p); }
    };
}
