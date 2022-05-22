#pragma once
#include <map>
#include "pix_abc.h"
namespace doc
{
    using pix::font;
    using pix::text::place;
    using pix::text::range;
    using pix::text::style;
    using pix::text::style_index;

    struct model : polymorphic
    {
        pix::text::block block;
        //array<pix::text::block> blocks;
        array<range> selections;

        static inline
            std::map<str,
            style_index>
            styles;

        virtual void set_text (str) {}
        virtual void set_html (str) {}
        virtual void add_text (str) {}
        virtual void add_html (str) {}
        virtual str  get_text () { return ""; }
        virtual str  get_html () { return ""; }
        virtual str  brief    () { return ""; }

        virtual place front () const { return place{}; }
        virtual place back  () const { return place{}; }

        virtual void set (pix::text::style) {}

        virtual bool undo        () { return false; }
        virtual bool redo        () { return false; }
        virtual bool erase       () { return false; }
        virtual bool backspace   () { return false; }
        virtual bool insert (str s) { return false; }
        virtual bool ready       () { return false; }
    };
}
  