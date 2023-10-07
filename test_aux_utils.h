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

    template<class...Xs>
    auto print (Xs...xs)
    {
        str s;
        ([&]
        {
            s += to_string(xs) + " ";
        }
        (), ...);
        s.truncate();
        return s;
    }

    void test_utils () try
    {
        test("packs");
        {
            oops(out(sum_all(1, 2, 3, 4))) { "10" };

            oops(out(print(1, 2.3, "4"))) { "1 2.300000 4" };

            struct node
            {
                int data = 0;
                node* left  = nullptr;
                node* right = nullptr;
            };
            node t[5];
            t[0].left  = &t[1];
            t[1].left  = &t[2];
            t[2].right = &t[3];
            t[3].data  = 5;

            auto get = [](node* top, auto... args)
            {
                return (top ->* ... ->* args);
            };
            auto left  = &node::left;
            auto right = &node::right;

            oops(out(get(&t[0], left, left, right)->data)) { "5" };
        }

        test("recursion");
        {
            auto factorial = []
            (this auto&& self, int i) -> int {
            return i == 1 ? 1 : i*self(i-1); };

            oops(out(factorial(1))) { "1" };
            oops(out(factorial(2))) { "2" };
            oops(out(factorial(3))) { "6" };

            struct leaf {}; struct node;
            using  tree = std::variant<leaf, node*>;
            struct node { tree left, right; };

            auto num_of_leaves =
                [](const tree& tree) {
                return std::visit(overloaded{
                    [](leaf const&){ return 1; },
                    [](this auto const& self, node* n){
                        return std::visit(self, n->left)
                            + std::visit(self, n->right);
                    }
                }, tree);
            };

            tree t;
            node root, l, r;
            oops(out(num_of_leaves(t))) { "1" }; t = &root;
            oops(out(num_of_leaves(t))) { "2" }; root.left = &l;
            oops(out(num_of_leaves(t))) { "3" }; root.right = &r;
            oops(out(num_of_leaves(t))) { "4" };
        }

        test("ranges");
        {
            namespace ranges = std::ranges;
            namespace views = std::ranges::views;
            auto tostr = ranges::to<std::string>;
            auto triml = views::drop_while(::isspace);
            auto trimr = views::reverse | triml | views::reverse;
            auto strip = triml | trimr;

            oops(out(str(" abc ") | triml | tostr())) { "abc " };
            oops(out(str(" abc ") | trimr | tostr())) { " abc" };
            oops(out(str(" abc ") | strip | tostr())) { "abc" };
        }

        test("lifetime");
        {
            auto f = []
            {
                struct B
                {
                    B(){ out( "B"); }
                   ~B(){ out("~B"); }
                    auto c () { return std::vector<element>{'1','2'}; }
                };
                struct A
                {
                    A(){ out( "A"); }
                   ~A(){ out("~A"); }
                    auto b () { return B{}; }
                };
                auto a = []{ return A{}; };

                for (auto const& x: a().b().c())
                    out("["+x.s+"]");
            };
            oops(f())
            {
                "A",
                "B",
                "ctor: 1",
                "ctor: 2",
                "copy: 1",
                "copy: 2",
                "dtor: 2",
                "dtor: 1",
                "~B",
                "~A",
                "[1]",
                "[2]",
                "dtor: 1",
                "dtor: 2"
            };
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
