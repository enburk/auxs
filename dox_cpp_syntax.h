#pragma once
#include <filesystem>
#include "dox_cpp_syntax_preprocessor.h"
namespace doc::cpp::syntax
{
    struct parser
    {
        report & log;

        parser (report & log) : log(log) {}

        struct element
        {
            str name, kind;
            array<token*> head;
            array<element> body;
            array<token*> tail;
        };

        array<element> bracketing (deque<token*> & input, str closing)
        {
            auto close = [this, &input](element & e, str s)
            {
                if (input.size() == 0) {
                    log.error(e.head[0], "unclosed '" + e.head[0]->text + "'");
                    return;
                }
                auto token = input.front();
                if (token->text != s) std::terminate();
                input.pop_front();
                e.tail += token;
            };

            array<element> output; while (input.size() > 0)
            {
                auto token = input.front();
                if (token->text == closing) break;
                input.pop_front();

                if (token->text == "}") { log.error(token, "unexpected '}'"); } else
                if (token->text == ")") { log.error(token, "unexpected ')'"); } else
                if (token->text == "]") { log.error(token, "unexpected ']'"); }

                element e; e.head += token;

                if (token->text == "{") { e.kind = "{}"; e.body = bracketing(input, "}"); close(e, "}"); } else
                if (token->text == "(") { e.kind = "()"; e.body = bracketing(input, ")"); close(e, ")"); } else
                if (token->text == "[") { e.kind = "[]"; e.body = bracketing(input, "]"); close(e, "]"); }

                output += e;
            }
            return output;
        }
        array<element> bracketing (deque<token*> input) {
                return bracketing (input, "");
        }

        inline array<element> proceed (array<token> & input)
        {
            deque<token*> tokens;
            for (auto & token : input)
                if (token.kind != "space" &&
                    token.kind != "comment")
                    tokens += &token;

            return
                bracketing(
                preprocessor(log).proceed(
                tokens));
        }
    };

    namespace analysis
    {
        struct data { report log; };

        data proceed (std::filesystem::path, array<token> & tokens)
        {
            data d;
            parser(d.log).proceed(tokens);
            return d;
        }
    };
}

