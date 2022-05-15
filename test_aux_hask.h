#pragma once
#include "aux_hask.h"
#include "aux_timing.h"
#include "aux_unittest.h"
namespace aux::unittest
{
    using namespace hask;

    //auto operator >  (auto l, auto f) { return fmap(f)(l); }
    //auto operator >= (auto l, auto f) { return flatmap(f)(l); }

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
        test("haskell.list.concat");
        {
            auto xs = list(1, 2);
            auto ys = list(3, 4);
            auto zs = concat(xs,ys);
            oops(out(head(zs))) { "1" };
            oops(out(head(tail(zs)))) { "2" };
            oops(out(head(tail(tail(zs))))) { "3" };
            oops(out(head(tail(tail(tail(zs)))))) { "4" };
            oops(out(length(zs))) { "4" };
        }
        test("haskell.list.fmap");
        {
            auto xs = list(1, 2, 3);
            auto sqr = [](auto n){ return n*n; };
            auto prn = [](auto x){ out(x); return x; };
            auto ys = fmap(sqr)(xs);
            oops(out(head(ys))) { "1" };
            oops(out(head(tail(ys)))) { "4" };
            oops(out(head(tail(tail(ys))))) { "9" };
            oops(out(length(ys))) { "3" };
            oops(fmap(prn)(xs)) { "1", "2", "3" };
            oops(fmap(prn)(xs)) { "3", "2", "1" };
            //oops(xs > prn) { "1", "2", "3" };
        }
        test("haskell.list.flatmap");
        {
            auto xs = list(1, 2, 3);
            auto pair = [](auto x) { return list(-x, x); };
            auto prn = [](auto x){ out(x); return x; };
            oops(fmap(prn)(flatmap(pair)(xs))) { "-1", "1", "-2", "2", "-3", "3" };
            //oops(flatmap(pair)(xs) > prn) { "-1", "1", "-2", "2", "-3", "3" };
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
