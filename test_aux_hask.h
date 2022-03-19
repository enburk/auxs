#pragma once
#include "aux_hask.h"
#include "aux_timing.h"
#include "aux_unittest.h"
namespace aux::unittest
{
    using namespace hask;

    void test_hask1 () try
    {
        test("haskell.list");
        {
            auto xs = list(1, '2', "3");
            oops(out(head(xs))) { "1" };
            oops(out(head(tail(xs)))) { std::to_string('2') };
            oops(out(head(tail(tail(xs))))) { "3" };
            oops(out(length(xs))) { "3" };
        }
        test("haskell.fmap");
        {
            auto xs = list(1, 2, 3);
            auto sqr = [](auto n){ return n*n; };
            auto ys = fmap(sqr)(xs);
            oops(out(head(ys))) { "1" };
            oops(out(head(tail(ys)))) { "4" };
            oops(out(head(tail(tail(ys))))) { "9" };
            oops(out(length(ys))) { "3" };
        }
    }
    catch(assertion_failed){}

    void test_hask2 () try
    {
         test("-");
    }
    catch(assertion_failed){}

    void test_hask3 () try
    {
         test("-");
    }
    catch(assertion_failed){}
}
