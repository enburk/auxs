#pragma once
#include <filesystem>
#include "doc_view.h"
#include "doc_text_model_b.h"
namespace doc::text
{
    struct model : b::model
    {
        using base = b::model;

        std::filesystem::path path;

        array<token> tokens;

        virtual report log () { return report{}; }

        virtual bool ready () { return true; }

        virtual void tick  () {}

        virtual void prereanalyze () {}

        virtual void reanalyze () {}

        virtual void tokenize ()
        {
            tokens.clear(); token t;

            for (auto [n, line] : enumerate(lines))
            {
                for (auto [offset, glyph] : enumerate(line))
                {
				    if (!glyph.letter() && !glyph.digit())
                    {
                        if (t.text != "") tokens += t;

                        tokens += token {glyph, "", "", range{
                            {n, offset},
                            {n, offset+1}}
                        };

                        t = token {"", "", "", range{
                            {n, offset+1},
                            {n, offset+1}}
                        };
                    }
                    else
                    {
                        t += glyph;
                    }
                }

                if (t.text != "") tokens += t;

                if (n != lines.size()-1)
                {
                    tokens += token {"\n", "", "", range{
                        {n, line.size()},
                        {n, line.size()}}
                    };

                    t = token {"", "", "", range{
                        {n+1, 0},
                        {n+1, 0}}
                    };
                }
            }
        };

        virtual bool tokenize_if (bool updated)
        {
            if (updated) tokenize();
            return updated;
        };

        bool undo        () override { return tokenize_if(base::undo     ()); }
        bool redo        () override { return tokenize_if(base::redo     ()); }
        bool erase       () override { return tokenize_if(base::erase    ()); }
        bool backspace   () override { return tokenize_if(base::backspace()); }
        bool insert (str s) override { return tokenize_if(base::insert  (s)); }

        str  get_text ()      override { return string(); }
        str  get_html ()      override { throw std::runtime_error("unsupported"); }

        void set_text (str s) override { if (base::set(text(s))) tokenize(); }
        void set_html (str s) override { throw std::runtime_error("unsupported"); }

        void add_text (str s) override { lines += base{s}.lines; tokenize(); }
        void add_html (str s) override { throw std::runtime_error("unsupported"); }

        void set (doc::view::style s, doc::view::format f) override
        {
            view_lines = {doc::view::line{f}};

            auto i = pix::text::style_index(s);
        
            for (const auto & t : tokens)
            {
                if (t.text == "\n")
                {
                    view_lines += {doc::view::line{f}};
                }
                else
                {
                    auto style = i;
                    if (auto it = styles.find(t.kind);
                        it != styles.end())
                        style = it->second;
            
                    view_lines.back().tokens +=
                        doc::view::token{t.text, style, t.info};
                }
            }
        }
    };
}