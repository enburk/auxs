#pragma once
#include <any>
#include <set>
#include "gui_widget.h"
namespace gui
{
    template<class T>
    struct widgetarium :
    widget<widgetarium<T>>
    {
        using widget = widget<widgetarium<T>>;
        using widget::children;
        using widget::skin;

        array<int> holes;
        array<int> indices;
        std::deque<std::optional<T>> deque;

        T* notifier = nullptr;
        int notifier_index = 0;

        int size () const { return indices.size(); }
        bool empty() const { return size() == 0; }
        void reserve (int n)
        {
            holes.reserve(n);
            indices.reserve(n);
            children.reserve(n);
        }

        void on_change (void* w) override
        {
            for (int i = 0; i < size(); i++)
                if (&(*this)(i) == w)
                {
                    notifier = &(*this)(i);
                    notifier_index = i;
                    widget::notify();
                }
        }

        const T& at (int pos) const {
            if (pos >= size()) throw std::out_of_range
            ("widgetarium size = " + std::to_string(size())
                + ", accessed at " + std::to_string(pos));
            return *deque[indices[pos]];
        }
        /***/ T& at (int pos) /***/ {
            if (pos > size()) throw std::out_of_range
            ("widgetarium size = " + std::to_string(size())
                + ", accessed at " + std::to_string(pos));
            if (pos == size()) emplace_back();
            return *deque[indices[pos]];
        }

        const T& operator [] (int pos) const { return at(pos); }
        /***/ T& operator [] (int pos) /***/ { return at(pos); }

        const T& operator () (int pos) const { return at(pos); }
        /***/ T& operator () (int pos) /***/ { return at(pos); }

        const T & front () const { return *deque[indices.front()]; }
        /***/ T & front () /***/ { return *deque[indices.front()]; }

        const T & back  () const { return *deque[indices.back()]; }
        /***/ T & back  () /***/ { return *deque[indices.back()]; }

        template<class... Args>
        T & emplace_back (Args&&... args)
        {
            if (holes.empty()) { auto & t =
                deque.emplace_back(std::in_place, std::forward<Args>(args)...);
                indices.emplace_back(size());
                children += &t.value();
                t.value().parent = this;
                if (t.value().skin.now == "")
                    t.value().skin = skin.now;
                return t.value();
            } else {
                int index = holes.back(); holes.pop_back(); auto & t =
                deque[index].emplace(std::forward<Args>(args)...);
                indices += index;
                children += &t;
                t.parent = this;
                if (t.skin.now == "")
                    t.skin = skin.now;
                return t;
            }
        }

        int rotate (int f, int m, int l) {
            assert(children.size() == indices.size());
            std::rotate(
            children.begin() + f,
            children.begin() + m,
            children.begin() + l);
            return (int)(
            std::rotate(
            indices.begin() + f,
            indices.begin() + m,
            indices.begin() + l) -
            indices.begin());
        }

        void erase (int pos)
        {
            deque[indices[pos]].reset();
            holes += indices[pos];
            indices.erase(indices.begin()+pos);
        }
        void truncate (int pos) { while (size() > pos) erase(size()-1); }
        void truncate () { if (size() > 0) truncate(size()-1); }
        void clear () { truncate (0); }

        struct iterator
        {
            widgetarium& that; int index;
            void operator ++ () { ++index; }
            void operator -- () { --index; }
            void operator += (int n) { index += n; }
            void operator -= (int n) { index -= n; }
            bool operator == (iterator i) { return index == i.index; }
            bool operator != (iterator i) { return index != i.index; }
            const T & operator * () const { return that(index); }
            /***/ T & operator * () /***/ { return that(index); }
            friend iterator operator + (iterator i, int n) { i.index += n; return i; }
            friend iterator operator - (iterator i, int n) { i.index -= n; return i; }
            friend int operator - (iterator i, iterator j) { return i.index - j.index; }
            friend int operator < (iterator i, iterator j) { return i.index < j.index; }
        };
        iterator begin () { return iterator{*this, 0}; }
        iterator end   () { return iterator{*this, size()}; }
        const iterator begin () const { return iterator{*const_cast<widgetarium*>(this), 0}; }
        const iterator end   () const { return iterator{*const_cast<widgetarium*>(this), size()}; }

        struct range_type
        {
            widgetarium& host; int begin_, end_;
            auto range_(iterator i, iterator j) -> range_type {
                return range_type {host,
                    int(i-host.begin()),
                    int(j-host.begin())};
            }

            iterator begin () { return host.begin() + begin_; }
            iterator end   () { return host.begin() + end_; }

            auto size   () { return (int)(end() - begin()); }
            auto offset () { return (int)(begin() - host.begin()); }
            bool empty  () { return size() == 0; }
            explicit operator bool () {
                return not empty(); }

            auto clip (iterator i) { return std::min(std::max(begin(),i), end()); }
            auto from (iterator i) { return range_(clip(i), end  ()); }
            auto upto (iterator i) { return range_(begin(), clip(i)); }

            auto upto (int n) { return range_(begin(), host.clip(host.begin() + n)); }
            auto span (int n) { return range_(begin(), host.clip(     begin() + n)); }

            auto operator () (iterator i, iterator j) { return from(i).upto(j); }
            auto operator () (int b, int e) { return from(begin()+b).upto(begin()+e); }
            auto operator () (int i) -> T& { return *(begin() + i); }

            auto& front() { return host.at(begin_); }
            auto& back () { return host.at(end_-1); }

            int rotate (int f, int m, int l) {
                return host.rotate(
                    begin_+ f,
                    begin_+ m,
                    begin_+ l) -
                    begin_; }

            void erase(int pos) { host.erase(begin_ + pos); end_--; }
            void truncate (int pos) { while (size() > pos) erase(size()-1); }
            void truncate () { if (size() > 0) truncate(size()-1); }
            void clear () { truncate (0); }
        };

        auto range_(range_type r) { return from(r.offset()).span(r.size()); }
        auto range_(iterator  i, iterator  j) -> range_type {
            return range_type {*this,
                int(i-begin()),
                int(j-begin())}; }

        auto clip (iterator i) { return std::min(std::max(begin(),i), end()); }
        auto from (iterator i) { return range_(clip(i), end  ()); }
        auto upto (iterator i) { return range_(begin(), clip(i)); }
        auto from (int n) { return from(begin() + n); }
        auto upto (int n) { return upto(begin() + n); }
        auto whole () { return range_(begin(), end()); }

        auto operator () (iterator i, iterator j) { return from(i).upto(j); }
        auto operator () (int b, int e) { return from(begin()+b).upto(begin()+e); }
    };
}
