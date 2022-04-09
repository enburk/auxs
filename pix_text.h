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
            xy   shift;

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
            int Ascent   = 0; // pixels above the base line (font based)
            int ascent   = 0; // pixels above the base line (actual)
            int Descent  = 0; // pixels below the base line (font based)
            int descent  = 0; // pixels below the base line (actual)
            int bearing  = 0; // horizontal displacement
            int advance  = 0; // pen position increment
            int lpadding = 0; // from begin to first pixel
            int rpadding = 0; // from last pixel to advance (negative for italic)

            int Width  () const { return max(advance, advance - rpadding); }
            int width  () const { return advance - lpadding - rpadding; }
            int Height () const { return Ascent + Descent; }
            int height () const { return ascent + descent; }

            bool operator == (metrics const&) const = default;
            bool operator != (metrics const&) const = default;
        };
    }

    struct glyph : text::metrics
    {
        aux::unicode::glyph text;
        text::style_index style_index;

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
