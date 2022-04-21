#pragma once
#include "doc_text_model.h"
#include "dox_cpp_lexica.h"
#include "dox_cpp_syntax.h"
namespace doc::cpp
{
    using doc::text::report;

    struct model : doc::text::model
    {
        using base = doc::text::model;
        
        report log_;
        report log () override { return log_; }
        
        void tokenize () override
        {
            log_.clear();

            tokens = lexica::parse(*this);

            syntax::parser(log_).proceed(tokens);
        }
    };
}