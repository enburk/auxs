#pragma once
#include <unordered_map>
#include "doc_text_text.h"
namespace doc::cpp::syntax
{
    using doc::text::token;
    using doc::text::report;

    struct preprocessor
    {
        report & log;

        preprocessor (report & log) : log(log) {}

        deque<token*> proceed (deque<token*> & input)
        {
            deque<token*> output;

            struct define { array<str> args; array<token*> text; };

            std::unordered_map<str, define> defines;

            auto read = [&](token*& token, str what)
            {
                if (input.size() == 0) log.error(token, what); else {
                    token = input.front();
                    input.pop_front();
                }
            };

            auto todo = [&](token* token)
            {
                token->kind = "pragma";
                bool backslash = false;
                while (true)
                {
                    read(token, "unexpected end of file");

                    if (token->text.starts_with("\"") and
                        token->text.ends_with("\\")) {
                        token->text.truncate();
                        backslash = true;
                    }
                    if (token->text == "\\") {
                        backslash = true;
                        continue;
                    }
                    if (token->text == "\n") {
                        if (backslash) {
                            backslash = false;
                            continue;
                        }
                        return;
                    }
                    token->kind = "macros";
                }
            };

            auto parse = [&](token* token)
            {
                read(token, "preprocessor directive expected");
            
                str directive = token->text;

                if (directive == "if") todo(token); else
                if (directive == "elif") todo(token); else
                if (directive == "else") todo(token); else
                if (directive == "endif") todo(token); else
                if (directive == "ifdef") todo(token); else
                if (directive == "ifndef") todo(token); else
                if (directive == "define") todo(token); else
                if (directive == "undef") todo(token); else
                if (directive == "include") todo(token); else
                if (directive == "line") todo(token); else
                if (directive == "error") todo(token); else
                if (directive == "pragma") todo(token); else
                if (directive == "defined") todo(token); else

                log.error(token, "unrecognized preprocessing directive");
            };

            auto process = [&](token* t)
            {
                output += t;
            };

            bool newline = true;

            while (input.size() > 0)
            {
                auto token = input.front(); input.pop_front();

                if (token->text == "\n") {
                    newline = true;
                }
                else
                if (not newline) process(token); else
                {
                    if (token->text == "#") {
                        token->kind = "pragma";
                        parse(token);
                    }
                    else {
                        newline = false;
                        process(token);
                    }
                }
            }
            return output;
        }
    };
}

