#pragma once
#include <coroutine>
#include <experimental/generator>
#include <future>
#include "aux_aux.h"

using std::ranges::input_range;
using std::ranges::random_access_range;
using std::experimental::generator;

namespace aux
{
    template<
    typename X>
    generator<X> operator_plus (generator<X>&& g, X ending)
    {
        for (auto x: g) co_yield x; co_yield ending;
    }

    template<
    input_range R>
    auto enumerate (R& r) -> generator<
    std::pair<int, typename R::value_type>>
    {
        int n = 0;
        for (auto& x : r)
            co_yield {n++, x};
    }

    template<
    input_range R>
    auto enumerate (R& r, typename R::value_type end) -> generator<
    std::pair<int, typename R::value_type>>
    {
        int n = 0;
        for (auto& x : r)
            co_yield {n++, x};
        co_yield {n, end};
    }

    template<class T=nothing>
    struct [[nodiscard]] task
    {
        struct promise_type {
            using handle_type = std::coroutine_handle<promise_type>;
            std::variant<std::monostate, T, std::exception_ptr> result;
            std::coroutine_handle<> previous; // who waits on this coroutine
            auto get_return_object  () { return task{handle_type::from_promise(*this)}; }
            void return_value(T value) { result.template emplace<1>(std::move(value)); }
            void unhandled_exception() { result.template emplace<2>(std::current_exception()); }
            auto initial_suspend() { return std::suspend_always{}; }
            auto final_suspend() noexcept {
                struct final_awaiter {
                    void await_resume() noexcept {}
                    bool await_ready () noexcept { return false; }
                    std::coroutine_handle<> await_suspend(handle_type me) noexcept {
                    // final_awaiter::await_suspend is called when the execution of the
                    // current coroutine (referred to by 'me') is about to finish.
                    // If the current coroutine was resumed by another coroutine via
                    // co_await get_task(), a handle to that coroutine has been stored
                    // as h.promise().previous. In that case, return the handle to resume
                    // the previous coroutine. Otherwise, return noop_coroutine(),
                    // whose resumption does nothing.
                        auto previous = me.promise().previous;
                        if (previous) return previous;
                        return std::noop_coroutine();
                    }
                };
                return final_awaiter{};
            }
        };

        using handle_type = promise_type::handle_type;
        explicit task(handle_type h) : handle(h) {}
        task(task&& rhs) : handle(rhs.handle) { rhs.handle = nullptr; }
       ~task() { if (handle) handle.destroy(); }
        handle_type handle;

        auto operator co_await() {
            struct awaiter {
                handle_type handle;
                bool await_ready() { return false; }
                auto await_resume() -> T {  
                    auto& r = handle.promise().result;
                    if (r.index() == 1) return std::get<1>(r);
                    std::rethrow_exception(std::get<2>(r));
                }
                auto await_suspend(std::coroutine_handle<> h) {
                    handle.promise().previous = h;
                    return handle;
                }
            };
            return awaiter{handle};
        }

        T operator()() const {
            handle.resume();
            auto& r = handle.promise().result;
            if (r.index() == 1) return std::get<1>(r);
            std::rethrow_exception(std::get<2>(r));
        }
    };
}
