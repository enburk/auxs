#pragma once
#include "pix_text.h"
namespace doc::view
{
    using pix::XY;
    using pix::text::style;
    using pix::text::style_index;

    struct format
    {
        int width = max<int>();
        int height = max<int>();

        int columns = 1;
        int gutter = 0;

        XY alignment = XY{ pix::center, pix::center };

        int lpadding = 0;
        int rpadding = 0;

        array<XY> lwrap;
        array<XY> rwrap;

        bool wordwrap = true;
        bool ellipsis = false;

        bool operator == (format const&) const = default;
        bool operator != (format const&) const = default;
    };

    struct token
    {
        str text;
        style_index style;
        str info, link;

        bool operator != (token const&) const = default;
        bool operator == (token const&) const = default;
    };

    struct line
    {
        format format;
        int indent = 0;
        int number = -1;
        bool modified = true;
        array<token> tokens;

        bool operator != (line const&) const = default;
        bool operator == (line const&) const = default;
    };
}

namespace doc
{
   struct model : polymorphic
    {
        //virtual generator<line> lines (style s, format f) = 0;

        array<range> selections;
        array<view::line> view_lines;
        static inline std::map<str,
            view::style_index>
            styles;

        virtual void set_text (str) {}
        virtual void set_html (str) {}
        virtual void add_text (str) {}
        virtual void add_html (str) {}
        virtual str  get_text () { return ""; }
        virtual str  get_html () { return ""; }

        virtual void set (view::style s, view::format f) {}

        virtual bool undo        () { return false; }
        virtual bool redo        () { return false; }
        virtual bool erase       () { return false; }
        virtual bool backspace   () { return false; }
        virtual bool insert (str s) { return false; }
        virtual bool ready       () { return false; }

        virtual place front () const { return place{}; }
        virtual place back  () const { return place{}; }


    };
}


  