#pragma once
#include "pix_text_block.h"
#include "doc_text_model_b.h"
namespace doc::text
{
    struct model : b::model
    {
        using base = b::model;

        array<token> tokens;

        std::filesystem::path path;

        virtual report log () { return report{}; }

        virtual bool ready () { return true; }

        virtual void tick  () {}

        virtual void preanalize () {}

        virtual void analyze () {}

        virtual void tokenize ()
        {
            tokens.clear(); token t;

            for (auto [n, line] : enumerate(lines))
            {
                for (auto [offset, glyph] : enumerate(line, ""))
                {
				    if (glyph.letter() or
					    glyph.digit() or
					    glyph == "_")
					    t += glyph;
                    else
                    {
                        if (t.text != "") tokens += t;

                        tokens += token {glyph, "", "", range{
                            {n, offset},
                            {n, offset+1}}};

                        t = token {"", "", "", range{
                            {n, offset+1},
                            {n, offset+1}}};
                    }
                }
            }
        };

        bool tokenize_if (bool updated) {
            if (updated) tokenize();
            return updated; };

        bool undo        () override { return tokenize_if(base::undo     ()); }
        bool redo        () override { return tokenize_if(base::redo     ()); }
        bool erase       () override { return tokenize_if(base::erase    ()); }
        bool backspace   () override { return tokenize_if(base::backspace()); }
        bool insert (str s) override { return tokenize_if(base::insert  (s)); }

        str  get_text ()      override { return string(); }
        str  get_html ()      override { throw std::runtime_error("unsupported"); }
        bool add_html (str s) override { throw std::runtime_error("unsupported"); }
        bool set_html (str s) override { throw std::runtime_error("unsupported"); }
        bool set_text (str s) override
        {
            bool changed = base::set(text(s));
            if (changed) tokenize();
            return changed;
        }
        bool add_text (str s) override
        {
            bool changed = s != "";
            if (not changed) return false;
            lines += base{s}.lines;
            tokenize();
            return true;
        }

        void set (style s) override
        {
            auto i = style_index(s);
        
            block.lines.clear();
            block.lines += pix::text::line{};
            block.lines.back().style = i;

            for (const auto & t : tokens)
            {
                if (t.text == "\n")
                {
                    block.lines += pix::text::line{};
                    block.lines.back().style = i;
                }
                else
                {
                    auto style = i;
                    auto it = styles.find(t.kind);
                    if (it != styles.end())
                        style = it->second;
            
                    block.lines.back().tokens +=
                    pix::text::token{t.text, style};
                    block.lines.back().tokens.back().info = t.info;
                }
            }
        }

        str brief () override
        {
            str s;
            for (const auto & line : lines) {
                s += aux::unicode::string(line) + "\n";
                if (s.size() > 100) {
                    s.resize(100);
                    s += "...\n";
                    break;
                }
            }
            if (s != "") s.pop_back();
            return s;
        }
    };
}