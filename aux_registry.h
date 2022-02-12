#pragma once
#include <vector>
#include <optional>
namespace data
{
    // Sean Parent, CppCon 2019
    // Russian coat check algorithm
    template<class T> struct registry
    {
        std::vector<
        std::pair<size_t,
        std::optional<T>>> pool;

        size_t size = 0, next_id = 0;

        auto append(T element) -> size_t {
            pool.emplace_back(next_id, std::move(element));
            ++size; return next_id++;
        }

        void erase(size_t id) noexcept
        {
            auto it = std::lower_bound(
                pool.begin(), pool.end(), id,
                    [](const auto & a, const auto & b)
                        { return a.first < b; });

            if (it == pool.end() || it->first != id || !it->second) return;

            it->second.reset(); --size;
            if (size < pool.size() / 2)
                pool.erase(std::remove_if(pool.begin(), pool.end(),
                    [](const auto & e) { return !e.second; }), pool.end());
        }

        void clear() noexcept { pool.clear(); size = 0; }

        template<class F> void for_each(F f) {
            for (const auto & e : pool)
                if (e.second) f(*e.second);
        }
    };
}
