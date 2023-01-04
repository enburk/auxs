#pragma once
#include <map>
#include <filesystem>
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
        rgba background;

        static inline
            std::map<str,
            style_index>
            styles;

        virtual str  brief    () { return ""; }
        virtual str  get_text () { return ""; }
        virtual str  get_html () { return ""; }
        virtual bool set_text (str) { return false; }
        virtual bool set_html (str) { return false; }
        virtual bool add_text (str) { return false; }
        virtual bool add_html (str) { return false; }

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
  