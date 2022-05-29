#pragma once
#include "aux_abc.h"
#include "pix_color.h"
#include "pix_frame.h"
namespace pix
{
    struct font
    {
        str face; int size = 0; bool bold = false; bool italic = false;

        bool operator == (font const&) const = default;
        bool operator != (font const&) const = default;

        struct metrics
        {
             int height  = 0; // ascent + descent
             int ascent  = 0; // units above the base line
             int descent = 0; // units below the base line (positive value)
             int linegap = 0; // baseline-to-baseline distance = ascent + descent + linegap
             int chargap = 0; // extra space between characters
             int average_char_width = 0; // usually the width of 'x'
             int maximum_char_width = 0;
             int minimum_char_width = 0;
        };
    };

    font::metrics metrics (font);

    namespace text
    {
        struct style
        {
            struct line {
            str style; int width = 0; rgba color;
            bool operator == (line const&) const = default;
            bool operator != (line const&) const = default;
            };

            font font;
            rgba color;
            line underline;  // "solid", "double", "dahsed", "dotted", "wavy"
            line strikeout;
            line outline;
            xy   offset; // e.g. subscript/superscript

            bool operator == (style const&) const = default;
            bool operator != (style const&) const = default;
        };

        struct style_index
        {
            int value = 0;

            static inline array<style> styles = {style{}};

            style_index () = default;
            style_index (text::style const& style) :
                value ((int)(styles.find_or_emplace(style) -
                             styles.begin())) {}

            text::style const& style () const { return styles[value]; }
            text::style /***/& style () /***/ { return styles[value]; }

            auto operator <=> (style_index const&) const = default;
        };

        struct metrics
        {
            int Ascent  = 0; // pixels above the base line (font based)
            int ascent  = 0; // pixels above the base line (actual)
            int Descent = 0; // pixels below the base line (font based)
            int descent = 0; // pixels below the base line (actual)
            int advance = 0; // pen position increment
            int xoffset = 0; // horizontal displacement
            int lborder = 0; // from begin to first pixel
            int rborder = 0; // from begin to last pixel

            int Width  () const { return max(advance, rborder); }
            int width  () const { return rborder - lborder; }
            int Height () const { return Ascent + Descent; }
            int height () const { return ascent + descent; }

            bool operator == (metrics const&) const = default;
            bool operator != (metrics const&) const = default;
            void operator += (metrics const& m) {
            Ascent  = max(Ascent,  m.Ascent);
            ascent  = max(ascent,  m.ascent);
            Descent = max(Descent, m.Descent);
            descent = max(descent, m.descent);
            lborder = min(lborder, m.lborder + advance + m.xoffset); // starts from zero?
            rborder = max(rborder, m.rborder + advance + m.xoffset);
            advance += m.advance; }
        };

        struct format
        {
            int width  = max<int>();
            int height = max<int>();

            xy alignment = xy{left, top};

            array<xy> lwrap;
            array<xy> rwrap;

            int columns = 1;
            int gutter  = 1;

            bool wordwrap = true;
            bool ellipsis = false;

            bool operator == (format const&) const = default;
            bool operator != (format const&) const = default;
        };

        struct place
        {
            int line = 0;
            int offset = 0;
            auto operator <=> (const place & p) const = default;
        };

        struct range
        {
            place from, upto;
            bool empty () const { return from == upto; }
            bool operator == (const range & r) const = default;
            bool operator != (const range & r) const = default;
        };
    }

    struct glyph : text::metrics
    {
        aux::unicode::glyph text;
        text::style_index style_index;
        xy offset; // for external formatting

        glyph () = default;
        glyph (aux::unicode::glyph text, text::style_index);
        glyph (aux::unicode::glyph text, text::style style):
        glyph (text, text::style_index(style)) {}

        text::style style () const { return style_index.style(); }

        bool operator == (glyph const&) const = default;
        bool operator != (glyph const&) const = default;

        void render (frame<rgba>, xy offset=xy(), uint8_t alpha=255, int x=0);
    };
}
