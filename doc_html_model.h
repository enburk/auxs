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

        str  brief    () override { return untagged(source.upto(200)); }
        str  get_text () override { return untagged(source); }
        str  get_html () override { return source; }

        void set_text (str text) override { set_html(encoded(text)); }
        void set_html (str text) override 
        {
            source = std::move(text);
            entities = html::entities(source);
            if (false) { // debug
                auto tokens = print(entities);
                entities.clear();
                entities += entity{"", "text"};
                entities.back().head = tokens;
            }
        }

        void add_text (str text) override { set_html(source + encoded(text)); }
        void add_html (str html) override { set_html(source + html); }

        void set (style s) override
        {
            lines.clear();
            for (auto entity : entities)
                proceed(entity, s, padding{}, "");
        }

        void proceed (entity const& entity, style style, padding padding, str link)
        {
            std::map<str,str> attr_style;

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
                            attr_style[key] = value;
                    }
                }
            }

            int height = pix::metrics(style.font).height;

            auto heights = [height](str val)
            {
                double h = 0;
                if (val.ends_with("em")) h = height; else
                if (val.ends_with("ex")) h = height * 0.39; else
                return int(h);
                val.truncate();
                val.truncate();
                return int(h * std::round(std::stof(val)));
            };

            if (entity.kind == "text")
            {
                if (lines.size() == 0 or
                    lines.back().padding != padding)
                    lines += pix::text::line{
                    style_index(style), padding};

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
                lines += pix::text::line{style_index(style), padding};
                lines.back().style = style_index(style); // could be empty
                lines += pix::text::line{style_index(style), padding};
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
                style.font.size = height * 85/100;
            }
            else
            if (entity.name == "big") {
                style.font.size = height * 125/100;
            }
            else
            if (entity.name == "sub") {
                style.font.size = height * 77/100;
                style.offset.y  = height * 30/100;
                for (auto [key, val] : attr_style)
                    if (key == "margin-left")
                        style.offset.x = heights(val);
            }
            else
            if (entity.name == "sup") {
                style.font.size = height * 77/100;
                style.offset.y  = -height * 30/100;
                for (auto [key, val] : attr_style)
                    if (key == "margin-left")
                        style.offset.x = heights(val);
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
                            int x = std::atoi(value.c_str());
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
                        style.color.a = 255;
                    }
                }
            }
            else
            if (entity.name == "blockquote")
            {
                padding.left += 3*height;
            }
            else
            if (entity.name == "div")
            {
                for (auto [key, val] : attr_style)
                {
                    if (key == "margin-left") {
                        padding.left = heights(val);
                    }

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