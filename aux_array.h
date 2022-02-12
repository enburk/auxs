#pragma once
#include <deque>
#include <vector>
#include "aux_aux.h"
#include "aux_math.h"
#include "aux_coro.h"
namespace aux
{
    template<class x> requires input_range<x> struct one_of     : x { using x::x; };
    template<class x> requires input_range<x> struct one_not_of : x { using x::x; };
    template<class x> one_of     (x) -> one_of     <x>;
    template<class x> one_not_of (x) -> one_not_of <x>;

    template<class Value, class Container = std::vector<Value>> struct array : Container
    {
        using type = Value;
        using base = Container;
        using container = Container;
        using value_type = type;
        using typename base::const_iterator;
        using typename base::iterator;
        using base::begin;
        using base::end;
        using base::rbegin;
        using base::rend;
        
        int  size    () const { return (int) base::size(); }
        void resize  (int n)  { base::resize (max(0,n)); }
        void reserve (int n)  { base::reserve(max(0,n)); }

        constexpr array (              ) = default;
        constexpr array (const array  &) = default;
        constexpr array (      array &&) = default;
        constexpr array (const base  &c) : base(c) {}
        constexpr array (      base &&c) : base(std::move(c)) {}
        constexpr array (std::initializer_list<type> list) : base(list) {}

        auto& operator =  (const array  & a) { base::operator = (a); return *this; }
        auto& operator =  (      array && a) { base::operator = (std::move(a)); return *this; }

        void  operator += (const type   & e) { base::push_back (e); }
        void  operator += (      type  && e) { base::push_back (std::move(e)); }

        void  operator += (const array  & a) { base::insert (end(), a.begin(), a.end()); }
        void  operator += (      array && a) { base::insert (end(),
            std::make_move_iterator(a.begin()),
            std::make_move_iterator(a.end())); }

        // template<class X>
        // void operator += (X r)
        //     requires input_range<X> {and
        //     std::convertible_to<X, value_type> {
        //     for (const auto & e : r)
        //         (*this) += std::forward(e);
        // }
        // template<class X>
        // void operator += (X r)
        //     requires random_access_range<X> and
        //     std::convertible_to<X, value_type> {
        //     reserve(int(size() + r.size()));
        //     for (const auto & e : r)
        //         (*this) += std::forward(e);
        // }

        friend array operator + (const array & a, const type  & b) { array r = a; r += b; return r; }
        friend array operator + (const type  & a, const array & b) { array r; r += a; r += b; return r; }

        ////////////////////////////////////////////////////////////////////////

        struct range_type
        {
            array& host; int begin_, end_;
            using iterator   = typename array::iterator;
            using iterator_  = typename array::const_iterator;
            using value_type = typename array::value_type;
            using range_type_= const range_type;
            auto range (iterator_ i, iterator_ j) const -> range_type_{ return range_type_{host, int(i-host.begin()), int(j-host.begin())}; }
            auto range (iterator  i, iterator  j) /***/ -> range_type { return range_type {host, int(i-host.begin()), int(j-host.begin())}; }
            
            iterator  begin () /***/ { return host.begin() + begin_; }
            iterator_ begin () const { return host.begin() + begin_; }
            iterator  end   () /***/ { return host.begin() + end_; }
            iterator_ end   () const { return host.begin() + end_; }

            auto size   () const { return (int)(end() - begin()); }
            auto offset () const { return (int)(begin() - host.begin()); }
            bool empty  () const { return size() == 0; }
            explicit operator bool () const {
                return not empty(); }

            auto clip (iterator i) /***/ { return std::min(std::max(begin(),i), end()); }
            auto clip (iterator i) const { return std::min(std::max(begin(),i), end()); }
            auto from (iterator i) /***/ { return range(clip(i), end  ()); }
            auto from (iterator i) const { return range(clip(i), end  ()); }
            auto upto (iterator i) /***/ { return range(begin(), clip(i)); }
            auto upto (iterator i) const { return range(begin(), clip(i)); }

            auto upto (int n) /***/ { return range(begin(), host.clip(host.begin() + n)); }
            auto upto (int n) const { return range(begin(), host.clip(host.begin() + n)); }
            auto span (int n) /***/ { return range(begin(), host.clip(     begin() + n)); }
            auto span (int n) const { return range(begin(), host.clip(     begin() + n)); }

            bool operator != (range_type r) { return !(*this == r); }
            bool operator == (range_type r) { return
                std::equal(begin(), end(),
                    r.begin(), r.end()); }
            auto operator <=> (range_type r) { return
                std::lexicographical_compare_three_way(begin(), end(),
                    r.begin(), r.end()); }
            template<int N>
            friend bool operator == (range_type l, const char(&r)[N])
                requires std::same_as<value_type, char> {
                std::string_view s(r);
                return std::equal(
                    l.begin(), l.end(),
                    s.begin(), s.end()); }
            template<int N>
            friend bool operator == (const char(&l)[N], range_type r)
                requires std::same_as<value_type, char> {
                std::string_view s(l);
                return std::equal(
                    r.begin(), r.end(),
                    s.begin(), s.end()); }
            template<int N>
            friend bool operator != (range_type l, const char(&r)[N])
                requires std::same_as<value_type, char> {
                return not (l == r); }
            template<int N>
            friend bool operator != (const char(&l)[N], range_type r)
                requires std::same_as<value_type, char> {
                return not (l == r); }

            auto operator () (iterator i, iterator j) const { return from(i).upto(j); }
            auto operator () (iterator i, iterator j) /***/ { return from(i).upto(j); }
            auto operator () (int b, int e) const { return from(begin()+b).upto(begin()+e); }
            auto operator () (int b, int e) /***/ { return from(begin()+b).upto(begin()+e); }
            auto operator [] (int i) /***/ -> type /***/& { return *(begin()+i); }
            auto operator [] (int i) const -> type const& { return *(begin()+i); }
            auto operator () (int i) { return *(begin()+i); }

            bool starts_with (random_access_range auto r) const
            {
                if (size() < r.size()) return false;
                return std::equal(begin(), begin() + r.size(),
                    r.begin(), r.end());
            }
            bool ends_with (random_access_range auto r) const
            {
                if (size() < r.size()) return false;
                return std::equal(end() - r.size(), end(),
                    r.begin(), r.end());
            }

            void insert (iterator i, range_type r) { host.insert(i, r); }
            void insert (iterator i, value_type e) { host.insert(i, std::move(e)); }
            void erase  () { host.erase(begin(), end()); end_ = begin_; }
            void replace_by (range_type r)
            {
                int n = min(size(), int(r.size()));
                for (int i=0; i<n; i++) (*this)[i] = r[i];
                if (size() > r.size()) from(begin()+n).erase();
                else insert(end(), r.from(r.begin()+n));
            }
            template<int N>
            void replace_by (const char(&r)[N])
                requires std::same_as<value_type, char> {
                int n = min(size(), N-1);
                for (int i=0; i<n; i++) (*this)[i] = r[i];
                if (size() > N-1) from(begin()+n).erase();
                else host.insert(end(), r+n, r+N-1);
            }
            void replace_by (const array& a) {
                replace_by(a.whole());
            }

            #include "aux_algo.h"
        };

        #include "aux_algo.h"

        ////////////////////////////////////////////////////////////////////////

        using iterator_ = const_iterator;
        using range_type_ = const range_type;
        auto range (range_type r) { return from(r.offset()).span(r.size()); }
        auto range (iterator_ i, iterator_ j) const -> range_type_{ return range_type_{*const_cast<array*>(this), int(i-begin()), int(j-begin())}; }
        auto range (iterator  i, iterator  j) /***/ -> range_type { return range_type {*this, int(i-begin()), int(j-begin())}; }

        array (range_type r) : base(r.begin(), r.end()) {}

        auto clip (iterator_ i) const { return std::min(std::max(begin(),i), end()); }
        auto clip (iterator  i) /***/ { return std::min(std::max(begin(),i), end()); }
        auto from (iterator_ i) const { return range(clip(i), end  ()); }
        auto from (iterator  i) /***/ { return range(clip(i), end  ()); }
        auto upto (iterator_ i) const { return range(begin(), clip(i)); }
        auto upto (iterator  i) /***/ { return range(begin(), clip(i)); }
        auto from (int n) const { return from(begin() + n); }
        auto from (int n) /***/ { return from(begin() + n); }
        auto upto (int n) const { return upto(begin() + n); }
        auto upto (int n) /***/ { return upto(begin() + n); }
        auto whole () const { return range(begin(), end()); }
        auto whole () /***/ { return range(begin(), end()); }

        auto operator () (iterator i, iterator j) const { return from(i).upto(j); }
        auto operator () (iterator i, iterator j) /***/ { return from(i).upto(j); }
        auto operator () (int b, int e) const { return from(begin()+b).upto(begin()+e); }
        auto operator () (int b, int e) /***/ { return from(begin()+b).upto(begin()+e); }

        friend bool operator == (range_type l, array const& r) { return l == r.whole(); }
        friend bool operator != (range_type l, array const& r) { return l != r.whole(); }
        friend auto operator <=>(range_type l, array const& r) { return l <=>r.whole(); }
        friend bool operator == (array const& l, range_type r) { return l.whole() == r; }
        friend bool operator != (array const& l, range_type r) { return l.whole() != r; }
        friend auto operator <=>(array const& l, range_type r) { return l.whole() <=>r; }

        //friend array operator + (const array & a, const range & b) { array r = a; r += b; return r; }
        //friend array operator + (const range & a, const array & b) { array r = a; r += b; return r; }

        ////////////////////////////////////////////////////////////////////////

        auto find         (const type & e) const { return std::find (begin(), end(), e); }
        bool binary_found (const type & e) const { return std::binary_search (begin(), end(), e); }

        template<class P> auto find_if     (P p) const { return std::find_if     (begin(), end(), p); }
        template<class P> auto find_if_not (P p) const { return std::find_if_not (begin(), end(), p); }
        template<class P> auto find_if     (P p) /***/ { return std::find_if     (begin(), end(), p); }
        template<class P> auto find_if_not (P p) /***/ { return std::find_if_not (begin(), end(), p); }

        ////////////////////////////////////////////////////////////////////////

        template<class F> void sort (F compare = std::less<>{}) { std::sort(begin(), end(), compare); }

        template<class P>
        void erase_if (P predicate) { base::erase(std::remove_if(begin(), end(), predicate), end()); }
        auto erase (const_iterator f, const_iterator l) { return base::erase(f, l); }
        auto erase (const_iterator i                  ) { return base::erase(i); }
        auto erase (int            i                  ) { return base::erase(begin()+i); }

        void erase_all(const type & e) { erase_if([e](const auto & a){ return a == e; }); }

        void insert(iterator_ i, range_type r) { container::insert(i, r.begin(), r.end()); }
        void insert(iterator_ i, value_type e) { container::insert(i, std::move(e)); }
        template<class I> void insert(iterator_ i, I b, I e) { container::insert(i, b, e); }

        void insert(int i, range_type r) { container::insert(begin()+i, r.begin(), r.end()); }
        void insert(int i, value_type e) { container::insert(begin()+i, std::move(e)); }

        void try_erase       (const type & e) { auto it = find(e); if (it != end()) erase(it); }
        void try_emplace     (const type & e) { auto it = find(e); if (it == end()) *this += e; }
        auto find_or_emplace (const type & e)
        {
            auto it = find(e); if (it != end()) return it;
            *this += e; it = end()-1; return it;
        }

        void replace_by (random_access_range auto r)
        {
            int n = min(size(), r.size());
            for (int i=0; i<n; i++) (*this)[i] = r[i];
            if (size() > r.size()) from(begin()+n).erase();
            else insert(end(), r.from(r.begin()+n));
        }

        int replace_all (
            random_access_range auto what,
            random_access_range auto with)
        {
            int nn = 0;
            int pos = 0;
            while (true) {
                auto range = from(begin()+pos).first(what); if (range.size() == 0) break;
                pos = int(range.begin() - begin() + with.size());
                range.replace_by(with);
                nn++;
            };
            return nn;
        }

        void triml (value_type const& e){
             upto(first(one_not_of(array{e}))
                .begin()).erase(); }

        void trimr (value_type const& e){
             from(last(one_not_of(array{e}))
                .end()).erase(); }

        void strip (value_type const& e){
             trimr(e); triml(e); }

        void truncate () { if (size() > 0)
             container::pop_back(); }

        void deduplicate () {
            sort(std::less{});
            erase(std::unique(
                begin(), end()),
                end()); }
    };

    template<class type> struct deque : public std::deque<type>
    {
        using base = std::deque<type>;

        int size () const { return (int)(base::size()); }

        deque (            ) = default;
        deque (deque const&) = default;
        deque (deque     &&) = default;
        deque (base const&c) : base(c) {}
        deque (base     &&c) : base(std::move(c)) {}

        explicit deque (array<type> const& a) : base(a.begin(), a.end()) {}
        explicit deque (array<type>     && a) : base(
            std::make_move_iterator(a.begin()),
            std::make_move_iterator(a.end())) {}

        auto& operator =  (deque const& a) { base::operator = (a); return *this; }
        auto& operator =  (deque     && a) { base::operator = (std::move(a)); return *this; }

        void  operator += (type  const& e) { base::push_back(e); }
        void  operator += (type      && e) { base::push_back(std::move(e)); }

        void  operator += (deque const& a) { base::insert(base::end(), a.begin(), a.end()); }
        void  operator += (deque     && a) { base::insert(base::end(),
            std::make_move_iterator(a.begin()),
            std::make_move_iterator(a.end())); }
    };
}

