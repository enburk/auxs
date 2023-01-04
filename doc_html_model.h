#pragma once
#include "doc_html_utils.h"
namespace doc::html
{
    struct model : doc::model
    {
        str source;
        array<entity> entities;
        array<pix::text::line>& lines = block.lines;
        typedef struct pix::text::line::padding padding;

        str brief () override
        {
            str s;
            for (auto g: aux::unicode::glyphs(source)) {
                s += g.string();
                if (s.size() >= 200) {
                    s += "...\n";
                    break; } }

            return untagged(s);
        }

        str get_text () override { return untagged(source); }
        str get_html () override { return source; }

        bool set_text (str text) override { return set_html(encoded(text)); }
        bool set_html (str text) override 
        {
            if (source == text) return false;
            source = std::move(text);
            entities = html::entities(source);
            if (false) { // debug
                auto tokens = print(entities);
                entities.clear();
                entities += entity{"", "text"};
                entities.back().head = tokens;
            }
            return true;
        }

        bool add_text (str text) override { return add_html(encoded(text)); }
        bool add_html (str html) override
        {
            if (html == "") return false;
            entities += html::entities(html);
            source += html;
            return true;
        }

        void set (style s) override
        {
            lines.clear();
            for (auto entity : entities)
                proceed(entity, s, padding{}, "");

            while (
            not lines.empty()
            and lines.back().tokens.empty())
                lines.truncate();
        }

        void proceed (entity const& entity, style style, padding padding, str link)
        {
            std::multimap<str,str> attr_style;
            for (auto [attr, value] : entity.attr)
            {
                if (attr == "hidden")
                    return;

                if (attr == "style")
                {
                    value.strip("\"");
                    for (str s : value.split_by(";"))
                    {
                        str key, value;
                        if (s.split_by(":", key, value))
                        {
                            key.strip();
                            value.strip();
                            attr_style.emplace
                            (key, value);
                        }
                    }
                }
            }

            auto metrics = pix::metrics(style.font);
            auto measure = [metrics](str value)
            {
                double coeff = 1.0;

                if (value.ends_with("em")) { coeff = metrics.height;
                    value.truncate();
                    value.truncate();
                }
                else
                if (value.ends_with("ex")) { coeff = metrics.height * 0.39;
                    value.truncate();
                    value.truncate();
                }
                else
                if (value.ends_with("ch")) { coeff = metrics.average_char_width;
                    value.truncate();
                    value.truncate();
                }
                return int(std::round(coeff * std::stof(value)));
            };

            if (entity.kind == "text")
            {
                if (lines.size() == 0)
                    lines += pix::text::line{
                    style_index(style), padding};

                lines.back().padding = padding;

                for (auto token : entity.head)
                    lines.back().tokens +=
                        pix::text::token{token.text,
                            style_index(style),
                            link, token.info};
            }
            else
            if (entity.name == "br")
            {
                if (lines.size() == 0)
                    lines += pix::text::line{
                    style_index(style), padding};

                lines.back().style = // line could be empty
                style_index(style);  // apply current font height
                
                lines += pix::text::line{
                style_index(style), padding};
            }
            else
            if (entity.name == "h4") {
                style.font.bold = true;
            }
            else
            if (entity.name == "b") {
                style.font.bold = true;
            }
            else
            if (entity.name == "i") {
                style.font.italic = true;
            }
            else
            if (entity.name == "small") {
                style.font.size = metrics.height * 85/100;
            }
            else
            if (entity.name == "big") {
                style.font.size = metrics.height * 125/100;
            }
            else
            if (entity.name == "sub") {
                style.font.size = metrics.height * 77/100;
                style.offset.y  = metrics.height * 30/100;
                for (auto [key, val] : attr_style)
                    if (key == "margin-left")
                        style.offset.x =
                            measure(val);
            }
            else
            if (entity.name == "sup") {
                style.font.size = metrics.height * 77/100;
                style.offset.y  =-metrics.height * 30/100;
                for (auto [key, val] : attr_style)
                    if (key == "margin-left")
                        style.offset.x =
                            measure(val);
            }
            else
            if (entity.name == "code") {
                style.font.face = "monospace";
            }
            else
            if (entity.name == "a")
            {
                for (auto [attr, value] : entity.attr)
                    if (attr == "href") {
                        value.strip("\"");
                        link = value; }
            }
            else
            if (entity.name == "font")
            {
                for (auto [attr, value] : entity.attr)
                {
                    if (attr == "face")
                    {
                        value.strip("\"");
                        style.font.face = value;
                    }
                    if (attr == "size")
                    {
                        value.strip("\"");
                        if (value.ends_with("%")) {
                            value.truncate();
                            int x = std::stoi(value);
                            int size = gui::metrics::text::height;
                            style.font.size = size * x/100;
                        }
                    }
                    if (attr == "color" && // <font color=#008000>
                        value.starts_with("#") &&
                        value.size() == 1+6)
                    {
                        str r = value.from(1).upto(3);
                        str g = value.from(3).upto(5);
                        str b = value.from(5).upto(7);
                        style.color.r = (uint8_t) std::strtoul(r.c_str(), nullptr, 16);
                        style.color.g = (uint8_t) std::strtoul(g.c_str(), nullptr, 16);
                        style.color.b = (uint8_t) std::strtoul(b.c_str(), nullptr, 16);
                        style.color.a = 255; if (background != rgba{})
                        style.color = forecorrected(
                        style.color, background);
                    }
                }
            }
            else
            if (entity.name == "blockquote")
            {
                padding.left += 3 * metrics.height;
            }
            else
            if (entity.name == "div")
            {
                for (auto [key, val] : attr_style)
                {
                    if (key == "margin-left")
                        padding.left +=
                        measure(val);

                    if (key == "text-indent")
                        padding.first +=
                        measure(val);

                    if (key == "line-height")
                    {
                        if (val.ends_with("%")) {
                            val.truncate();
                            int x = std::atoi(val.c_str());
                            int size = gui::metrics::text::height;
                            style.font.size = size * x/100;
                        }
                    }
                }
            }
            else
            if (entity.name == "span")
            {
                for (auto [key, val] : attr_style)
                {
                    if (key == "font-variant") {
                        style.font.face = val;
                    }
                    if (key == "font-family") {
                        style.font.face = val;
                    }
                    if (key == "font-style" and
                        val == "normal") {
                        style.font.bold = false;
                        style.font.italic = false;
                    }
                }
            }

            for (auto e : entity.body)
                proceed(e, style, padding, link);

            if (entity.name == "h4") {
                if (lines.size() > 0 and
                    lines.back().tokens.size() > 0)
                    lines += pix::text::line{
                    style_index(style), padding};
            }
        }
    };
} 