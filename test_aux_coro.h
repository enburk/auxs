#pragma once
#include "aux_abc.h"
#include "aux_timing.h"
#include "aux_unittest.h"
namespace aux::unittest
{
    void test_coro1 () try
    {
        test("coro.generator");
        {
			auto fibs = []() -> generator<int> {
				int a = 0, b = 1; while (true) {
					co_yield a; b += std::
						exchange(a, b); } } ();

            auto i = fibs.begin();
            oops(out(*++i)) { "1" };
            oops(out(*++i)) { "1" };
            oops(out(*++i)) { "2" };
            oops(out(*++i)) { "3" };
            oops(out(*++i)) { "5" };
            oops(out(*++i)) { "8" };

			auto nats = []() -> generator<int> {
				int n = 0; while (true)
					co_yield n++; } ();

            int N = 100;
            int sum = 0;
            for (int n : nats) {
                if (n > N) break;
                sum += n; }
			
            oops(out(sum)) { std::to_string(N*(N+1)/2) };
        }

        test("coro.generator.plus");
        {
            auto abc = []() -> generator<char> {
                co_yield 'a';
                co_yield 'b';
                co_yield 'c';
            };

            string s1; for (char c: abc()) s1 += c;
            string s2; for (auto z = abc(); char c: operator_plus(std::move(z), ' ')) s2 += c;
            oops(out(s1)) { "abc"  };
            oops(out(s2)) { "abc " };
        }

        test("coro.enumerate.1");
        {
            string s;
            string cc = "abc";
            for (auto [i, c] : enumerate(cc))
            s += std::to_string(i) + c + ' ';
            oops(out(s)) { "0a 1b 2c " };
        }

        test("coro.enumerate.2");
        {
            string s;
            array<int> nn = {0, 1, 2};
            for (auto [i, n] : enumerate(nn))
            s += std::to_string(i) + std::to_string(n) + ' ';
            oops(out(s)) { "00 11 22 " };
        }

        test("coro.enumerate.3");
        {
            string s;
            array<string> lines = {"", "a", "bc"};
            for (auto [i, line] : enumerate(lines))
                for (auto [j, c] : enumerate(line))
                    s += std::to_string(i) + ":" +
                         std::to_string(j) + " " +
                         c + " ";
            oops(out(s)) { "1:0 a 2:0 b 2:1 c " };
        }

        test("coro.enumerate.4");
        {
            string s;
            array<string> lines = {"", "a", "bc"};
            for (auto [i, line] : enumerate(lines, "d"))
                for (auto [j, c] : enumerate(line, '-'))
                    s += std::to_string(i) + ":" +
                         std::to_string(j) + " " +
                         c + " ";
            oops(out(s)) { "0:0 - 1:0 a 1:1 - 2:0 b 2:1 c 2:2 - 3:0 d 3:1 - " };
        }
    }
    catch(assertion_failed){}

    task<int> four () { co_return 4; }
    task<int> five () { co_return 5; }
    task<int> boom () { throw std::runtime_error("boom"); co_return 0; }
    task<int> sum (task<int>&& a, task<int>&& b) {
        co_return (co_await a + co_await b); }

    string safe (auto&& task)
    try { return std::to_string(task()); }
    catch(std::exception const& e) { return e.what(); }

    void test_coro2 () try
    {
        test("coro.task");
        {
            oops(out(four()())) { "4" };
            oops(out(five()())) { "5" };
            oops(out(sum(four(), four())())) { "8" };
            oops(out(sum(four(), five())())) { "9" };

            oops(out(safe(four()))) { "4" };
            oops(out(safe(boom()))) { "boom" };
            oops(out(safe(sum(boom(), four())))) { "boom" };
            oops(out(safe(sum(four(), boom())))) { "boom" };
        }
    }
    catch(assertion_failed){}

    std::mutex mutex;

    task<> sleep (string s, int ms) {
        std::this_thread::sleep_for(
        std::chrono::milliseconds(ms));
        std::lock_guard lock{mutex};
        out(s + " awaken after " +
        std::to_string(ms) + " ms");
        co_return nothing{}; }

    task<> await (task<>&& a, task<>&& b) {
        auto future1 = launch(std::move(a));
        auto future2 = launch(std::move(b));
        timing t0;
        future1.get();
        future2.get();
        timing t1;
        auto ms = std::to_string(
        std::chrono::duration_cast<
        std::chrono::milliseconds>(t1-t0 +
        std::chrono::microseconds(500)).count());
        std::lock_guard lock{mutex};
        out("waited for " + ms + " ms");
        co_return nothing{}; }

    void test_coro3 () try
    {
        test("coro.async");
        {
            oops(sleep("0", 0)()) { "0 awaken after 0 ms" };
            oops(sleep("1", 1)()) { "1 awaken after 1 ms" };

            oops(await(
                sleep("A", 100),
                sleep("B", 200))()) {
                "A awaken after 100 ms",
                "B awaken after 200 ms",
                "waited for 200 ms" };

            array<task<>> t;
            t.emplace_back(sleep("A", 100));
            t.emplace_back(sleep("B", 200));
            t.emplace_back(sleep("a", 150));
            t.emplace_back(sleep("b", 250));
            using std::move; oops(await(
            await(move(t[0]), move(t[1])),
            await(move(t[2]), move(t[3])))()) {
                "A awaken after 100 ms",
                "a awaken after 150 ms",
                "B awaken after 200 ms",
                "waited for 200 ms" ,
                "b awaken after 250 ms",
                "waited for 250 ms" ,
                "waited for 250 ms" };
        }
    }
    catch(assertion_failed){}
}
