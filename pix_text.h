#pragma once
#include "pix_color.h"
#include "pix_image.h"
namespace pix
{
    using namespace data;

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
            str style; int width = 0; RGBA color;
            bool operator == (line const&) const = default;
            bool operator != (line const&) const = default;
            };

            font font;
            RGBA color;
            line underline;  // "solid", "double", "dahsed", "dotted", "wavy"
            line strikeout;
            line outline;
            XY   shift;

            bool operator == (style const&) const = default;
            bool operator != (style const&) const = default;
        };

        struct style_index
        {
            int value = 0;

            static inline array<style> styles = {style{}};

            text::style const& style () const { return styles[value]; }
            text::style /***/& style () /***/ { return styles[value]; }

            auto operator <=> (style_index const&) const = default;

            explicit style_index () = default;
            explicit style_index (text::style const& style) :
                value ((int)(styles.find_or_emplace(style) -
                             styles.begin())) {}
        };

        struct metrics
        {
            int ascent   = 0; // pixels above the base line (font based)
            int ascent_  = 0; // pixels above the base line (actual)
            int descent  = 0; // pixels below the base line (font based)
            int descent_ = 0; // pixels below the base line (actual)
            int bearing  = 0; // horizontal displacement
            int advance  = 0; // pen position increment
            int lpadding = 0; // from begin to first pixel
            int rpadding = 0; // from last pixel to advance (negative for italic)
            int width    = 0; // max(advance, advance - rpadding)

            bool operator == (metrics const&) const = default;
            bool operator != (metrics const&) const = default;
        };
    }

    struct glyph : text::metrics
    {
        str text; text::style_index style_index;

        glyph () = default;
        glyph (str text, text::style_index);
        glyph (str text, text::style style):
            glyph (text, text::style_index(style)) {}

        text::style style () const { return style_index.style(); }

        bool operator == (glyph const&) const = default;
        bool operator != (glyph const&) const = default;

        void render (pix::frame<RGBA>, XY offset=XY(), uint8_t alpha=255, int x=0);
    };
}
