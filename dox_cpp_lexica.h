#pragma once
#include <set>
#include "doc_text_text.h"
namespace doc::cpp::lexica
{
	using text::token;

    inline const std::set<str> keywords = 
    {
        "alignas", "alignof", "and", "and_eq", "asm", "atomic_cancel",
        "atomic_commit", "atomic_noexcept", "auto", "bitand", "bitor", "bool",
        "break", "case", "catch", "char", "char8_t", "char16_t", "char32_t",
        "class", "compl", "concept", "const", "consteval", "constexpr",
        "constinit", "const_cast", "continue", "co_await", "co_return",
        "co_yield", "decltype", "default", "delete", "do", "double",
        "dynamic_cast", "else", "enum", "explicit", "export", "extern",
        "false", "float", "for", "friend", "goto", "if", "inline", "int",
        "long", "mutable", "namespace", "new", "noexcept", "not", "not_eq",
        "nullptr", "operator", "or", "or_eq", "private", "protected", "public",
        "reflexpr", "register", "reinterpret_cast", "requires", "return",
        "short", "signed", "sizeof", "static", "static_assert", "static_cast",
        "struct", "switch", "synchronized", "template", "this", "thread_local",
        "throw", "true", "try", "typedef", "typeid", "typename", "union",
        "unsigned", "using", "virtual", "void", "volatile", "wchar_t", "while",
        "xor", "xor_eq",
    };

    inline constexpr bool ascii (char c) { return (c >= ' ' && c <= '~'); }
    inline constexpr bool space (char c) { return (c == ' ' || c =='\t'); }
    inline constexpr bool digit (char c) { return (c >= '0' && c <= '9'); }
    inline constexpr bool alpha (char c) { return (c >= 'A' && c <= 'Z') ||
                                                  (c >= 'a' && c <= 'z'); }
 
    inline bool ascii (str c) { return c.size() == 1 && ascii(c[0]); }
    inline bool space (str c) { return c.size() == 1 && space(c[0]); }
    inline bool digit (str c) { return c.size() == 1 && digit(c[0]); }
    inline bool alpha (str c) { return c.size() == 1 && alpha(c[0]); }

    inline bool unclosed_literal (const str & text, str symbol = "\"")
    {
        return text == symbol ||
            !text.ends_with(symbol) ||
            text.ends_with(str("\\"+symbol));
    }

    inline array<token> parse (const text::text & text)
    {
        array<token> tokens; token t; str comment;

        for (auto [n, line] : enumerate(text.lines))
        {
            if (comment == "//") comment = "";
            if (comment == "/**") comment = "/*";

            for (auto [offset, c] : enumerate(line))
            {
                if (comment != "") { t.kind = "comment"; t.text += c; t.range.upto.offset++; }

                if (comment == "/*"  && c != "*") {                  continue; }
                if (comment == "/*"  && c == "*") { comment = "/**"; continue; }
                if (comment == "/**" && c == "*") { comment = "/**"; continue; }
                if (comment == "/**" && c != "/") { comment = "/*";  continue; }
                if (comment == "/**" && c == "/") { comment = "";    continue; }

                if (comment == "//") continue;

                bool same = false;

                if (t.kind == "space"  ) same = space (c); else
                if (t.kind == "name"   ) same = alpha (c) || digit (c) || c == "_"; else
                if (t.kind == "number" ) same = alpha (c) || digit (c) || c == "."; else
                if (t.kind == "literal") same = unclosed_literal(t.text, "\""); else
                if (t.kind == "char"   ) same = unclosed_literal(t.text, "\'"); else
                if (t.kind == "symbol" ) same = false
                
                || (t.text == "+" && (c == "=" || c == "+"))
                || (t.text == "-" && (c == "=" || c == "-" || c == ">"))
                || (t.text == "/" && (c == "=" || c == "*" || c == "/"))
                || (t.text == "*" && (c == "="))

                || (t.text == "<" && (c == "=" || c == "<"))
                || (t.text == ">" && (c == "=" || c == ">"))
                || (t.text == "&" && (c == "=" || c == "&"))
                || (t.text == "|" && (c == "=" || c == "|"))
                || (t.text == "!" && (c == "="))
                || (t.text == "=" && (c == "="))
                || (t.text == ":" && (c == ":"))

                || (t.text =="<<" && (c == "="))
                || (t.text ==">>" && (c == "="))

                || (t.text == "." && (c == "."))
                || (t.text ==".." && (c == "."))
                ;
                if (same)
                {
                    t.text += c;
                    t.range.upto.offset++;

                    if (t.text == "//" ||
                        t.text == "/*") {
                        t.kind = "comment";
                        comment = t.text;
                    }
                }
                else
                {
                    if (t.text != "" &&
                        t.kind == "name" &&
                        keywords.find(t.text) !=
                        keywords.end()) t.kind = "keyword";

                    if (t.text != "") tokens += t;

                    t = token {c, "", "", range{
                        {n, offset},
                        {n, offset}}
                    };
                    t.kind =
                        alpha (c) ? "name"    :
                        digit (c) ? "number"  :
                        space (c) ? "space"   : 
                        c == "\"" ? "literal" : 
                        c == "\'" ? "char"    : 
                                    "symbol"  ;
                }
            }

            if (t.text != "" &&
                t.kind == "name" &&
                keywords.find(t.text) !=
                keywords.end()) t.kind = "keyword";

            if (t.text != "") tokens += t;

            tokens += token {"\n", "", "", range{
                {n, line.size()},
                {n, line.size()}}
            };

            t = token{};
        }

        return tokens;
    }
}

