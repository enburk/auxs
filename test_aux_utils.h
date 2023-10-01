#pragma once
#include "aux_abc.h"
#include "aux_unittest.h"
namespace aux::unittest
{
    template<typename T, typename... Ts>
    constexpr inline bool are_same_v = 
    std::conjunction_v<std::is_same<T,Ts>...>;

    template<typename... Ts>
    auto sum_all(Ts&&... args)
    requires are_same_v<Ts...> {
    return (... + std::forward<Ts>(args)); }

    void test_utils () try
    {
        test("packs");
        {
            oops(out(sum_all(1, 2, 3, 4))) { "10" };

            struct node
            {
                node* left = nullptr;
                node* right = nullptr;
                int data = 0;
            };
            node t[5];
            t[0].left = &t[1];
            t[1].left = &t[2];
            t[2].right = &t[3];
            t[3].data = 5;

            auto get = [](node* top, auto... args)
            {
                return (top ->* ... ->* args);
            };
            auto left = &node::left;
            auto right = &node::right;

            oops(out(get(&t[0], left, left, right)->data)) { "5" };
        }

        test("exceptions");
        {
            try { throw 0; } catch(int x)
            {
                oops(out(x)) { "0" };
                oops(out(std::uncaught_exceptions())) { "0" };
            }

            try { throw 1; } catch(...)
            {
                try
                {
                    try { throw 2; } catch(int x)
                    {
                        oops(out(x)) { "2" };
                    }
                    throw; // may cause terminate() ?
                }
                catch(int y)
                {
                    oops(out(y)) { "1" };
                }
            }

            try { throw 1; } catch(...)
            {
                try
                {
                    try
                    {
                        auto e = std::current_exception();
                        if (e) oops(out("e")) { "e" };
                        if (e) std::rethrow_exception(e);
                    }
                    catch(int x)
                    {
                        oops(out(x)) { "1" };
                    }
                    throw; // may cause terminate() ?
                }
                catch(int y)
                {
                    oops(out(y)) { "1" };
                }
            }
        }
    }
    catch(assertion_failed){}
}
