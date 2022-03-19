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
