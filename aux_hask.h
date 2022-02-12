#pragma once
namespace aux::hask
{
    auto list = [](auto... xs) {
        return [=](auto access) {
            return access(xs...); }; };

    auto head = [](auto alist) {
        return alist([](auto first, auto... rest) {
            return first; }); };

    auto tail = [](auto alist) {
        return alist([](auto first, auto... rest) {
            return list(rest...); }); };

    auto length = [](auto alist) {
        return alist([](auto... xs) {
            return sizeof...(xs); }); };

    auto fmap = [](auto f) {
        return [f](auto alist) {
            return alist([f](auto... xs){
                return list(f(xs)...); }); }; };

    //auto flatten = [](auto f, auto... xs) {
    //    return [f](auto alist) {
    //        return alist([f](auto... xs){
    //            return flatten(f, xs...);
    //        });
    //    };
    //};

    auto flatmap = [](auto f) {
        return [f](auto alist) {
            return alist([f](auto... xs){
                return flatten(f, xs...);
            });
        };
    };

}

#include "aux_timing.h"
#include "aux_unittest.h"
namespace aux::unittest
{
    using namespace hask;

    void test_monad1 () try
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

    void test_monad2 () try
    {
    }
    catch(assertion_failed){}

    void test_monad3 () try
    {
    }
    catch(assertion_failed){}
}
