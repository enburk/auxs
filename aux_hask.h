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

    auto concat = [](auto l1, auto l2) {
        auto access1 = [=](auto... p) {
        auto access2 = [=](auto... q) {
        return list(p..., q...); };
        return l2(access2); };
        return l1(access1); };

    auto fmap = [](auto f) {
        return [f](auto alist) {
            return alist([f](auto... xs){
                return list(f(xs)...); }); }; };

    auto flatten (auto f) { return list(); }
    auto flatten (auto f, auto a, auto... b) {
        return concat(f(a),
        flatten(f, b...)); }

    auto flatmap = [](auto f) {
        return [f](auto alist) {
            return alist([f](auto... xs){
                return flatten(f, xs...);
            });
        };
    };
}
