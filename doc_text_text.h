#pragma once
#include "doc.h"
#include "doc_text_glyph.h"
namespace doc::text
{
    struct text
    {
        array<array<glyph>> lines;

        bool operator == (const text & t) const = default;
        bool operator != (const text & t) const = default;

        text () = default;
        explicit text (str s) {
            lines.reserve(s.size()/80);
            for (str line : s.split_by("\n"))
                lines += glyph::parse(line);
        }
        str string () const {
            str s; for (const auto & line : lines)
                s += doc::text::string(line) + "\n";
            if (s != "") s.pop_back();
            return s;
        }
    };

    struct token
    {
        str text, kind, info; range range;
        void operator += (const glyph & g) { text += g.string(); range.upto.offset++; }
        bool operator != (const token & t) const = default;
        bool operator == (const token & t) const = default;
    };

    struct report
    {
        struct message
        {
            token* token = nullptr; str kind, what;
        };
        array<message> messages;
        array<message> errors;

        void debug (str what) { debug(nullptr, what); }
        void info  (str what) { info (nullptr, what); }
        void error (str what) { error(nullptr, what); }

        void debug (token* token, str what) {
            messages += message{token, "debug", what}; }

        void info  (token* token, str what) {
            messages += message{token, "info", what}; }

        void error (token* token, str what) {
            messages += message{token, "error", what};
            errors   += message{token, "error", what};
            if (token) { token->kind = "error";
                token->info = "<font color=#B00020>"
                    + what + "</font>"; } }

        void operator += (report report)
        {
            messages += report.messages;
            errors   += report.errors;
        }
        void clear ()
        {
            messages.clear();
            errors  .clear();
        }
        str operator () () const
        {
            str s;
            
            for (auto [token, kind, what] : messages)
            {
                if (kind == "debug") s += "<font color=#616161>";
                if (kind == "error") s += "<font color=#B00020>";

                if (token) s += "("
                    + std::to_string(token->range.from.line  +1) + ":"
                    + std::to_string(token->range.from.offset+1) + ") ";

                s += what;

                if (kind == "debug" or
                    kind == "error")
                    s += "</font>";

                s += "<br>";
            }

            return s;
        }
    };
}


  