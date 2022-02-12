#pragma once
#include "pix_text.h"
#include "doc_text_text.h"
#include "doc.h"
namespace doc::html
{
    struct entity
    {
        str name, kind, info;
        std::map<str,str> attr;
        using token = text::token;
        array<token> head;
        array<entity> body;
        array<token> tail;
        array<str> debug;

        explicit entity() = default;
        explicit entity(str s) { head += token{s}; }
        explicit entity(array<token> tt) : head(tt) {}
        explicit entity(str name, str kind) : name(name), kind(kind) {}
    };
}


  