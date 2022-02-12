#pragma once
#include <set>
#include <cmath>
#include <chrono>
#include <cassert>
#include <algorithm>
#include "aux_registry.h"
#include "pix_color.h"
#include "pix_coord.h"
namespace gui
{
    using namespace pix;
    using namespace std::literals::chrono_literals;

    struct time
    {
        int64_t  ms = 0;
        explicit time() = default;
        explicit time(int64_t ms) : ms(ms) {}
        template<class R, class P> time
        (std::chrono::duration<R, P> duration) {
            ms = std::chrono::duration_cast<
                 std::chrono::milliseconds>
                 (duration).count(); }

        void operator += (time t) { ms += t.ms; }
        void operator -= (time t) { ms -= t.ms; }

        friend time operator + (time a, time b) { a += b; return a; }
        friend time operator - (time a, time b) { a -= b; return a; }

        auto operator <=> (time const&) const = default;

        static time was;
        static time now;
        static time pause;
        static time infinity;
        static void reset_after_pause() { pause = time(); }
        static void set_after_pause() { set(); pause = now - was; }
        static void set() { static
            auto t0 = std::chrono::high_resolution_clock::now();
            auto t1 = std::chrono::high_resolution_clock::now();
            was = now; now = t1-t0;
        }
    };
    inline time time::was;
    inline time time::now;
    inline time time::pause;
    inline time time::infinity { max<int64_t>() };
    inline time default_transition_time = std::chrono::milliseconds(0);

    template<class type> inline type transit
        (type from, type to, int64_t mul, int64_t div) {
        if (div == 0) return to; auto add = to - from;
        if (mul == div) return clamp<type>(from + add);
        if (add == div) return clamp<type>(from + mul);
        if((add > max<int32_t>() && mul > max<int32_t>())
        or (add < min<int32_t>() && mul > max<int32_t>()))
        throw std::out_of_range("transit: overflow");
        return clamp<type>(from + add*mul/div);
    }
    template<> time inline transit<time>
        (time from, time to, int64_t mul, int64_t div) {
        return time(transit(from.ms, to.ms, mul, div));
    }
    template<> bool inline transit<bool>
        (bool from, bool to, int64_t mul, int64_t div) {
        return div == 0 or mul >= div ? to : from;
    }
    template<> RGBA inline transit<RGBA>
        (RGBA from, RGBA to, int64_t mul, int64_t div) { return RGBA(
            transit<uint8_t>(from.r, to.r, mul, div),
            transit<uint8_t>(from.g, to.g, mul, div),
            transit<uint8_t>(from.b, to.b, mul, div),
            transit<uint8_t>(from.a, to.a, mul, div));
    }
    template<> XYWH inline transit<XYWH>
        (XYWH from, XYWH to, int64_t mul, int64_t div) { return XYWH(
            transit<int>(from.x, to.x, mul, div),
            transit<int>(from.y, to.y, mul, div),
            transit<int>(from.w, to.w, mul, div),
            transit<int>(from.h, to.h, mul, div));
    }
    template<> XY   inline transit<XY>
        (XY from, XY to, int64_t mul, int64_t div) { return XY(
            transit<int>(from.x, to.x, mul, div),
            transit<int>(from.y, to.y, mul, div));
    }

    struct base_property : polymorphic { virtual void tick () = 0; };
    inline data::registry<base_property*> active_properties;

    namespace base { struct widget; }
    extern std::set<base::widget*> widgets;
    extern void change (base::widget*, void*);
    extern base::widget* inholder (void*);

    template<class type> struct property : base_property
    {
        type now, was, from, to;
        time notch, lapse, transition_time = default_transition_time;
        base::widget* widget = nullptr; std::optional<size_t> receipt;

        property (type value = type()) : now(value), was(value), from(value), to(value){}
       ~property () { if (receipt) active_properties.erase(*receipt); }

        operator type const& () { return now; }

        template<typename X> void operator = (X value) { go(value, time(0)); }

        void go (type to_) { go(to_, transition_time); }

        void go (type to_, time lapse_) {
            from = now; to = to_;
            notch = time::now;
            lapse = lapse_;
            tick();
        }

        void tick () override { 
            was = now;
            notch += time::pause;
            if (now != to)
                now = transit (from, to,
                    std::min(time::now - notch, lapse).ms, lapse.ms);
            if (now == to and receipt) { active_properties.erase(*receipt); receipt.reset(); }
            if (now != to and not receipt) receipt = active_properties.append(this);
            if (now != was) {
                if (!widget) widget = inholder(this);
                if (!widget) throw std::runtime_error
                ("property: wrong inholder");
                change(widget, this);
            }
        }
    };

    template<class type> struct binary_property
    {
        type now, was; base::widget* widget = nullptr;

        binary_property(type value = type()) : now(std::move(value)) {}

        operator type const& () { return now; }

        template<typename X>
        void operator = (X value) {
            was = std::move(now);
            now = std::move(value);
            if (now != was) {
                if (!widget) widget = inholder(this);
                if (!widget) throw std::runtime_error
                ("property: wrong inholder");
                change(widget, this);
            }
        }
    };

    template<class type> struct unary_property
    {
        type now; base::widget* widget = nullptr;

        unary_property (type value = type()) : now(std::move(value)) {}

        operator type const& () { return now; }

        template<typename X>
        void operator = (X value) {
            if (now != value) {
                now = std::move(value);
                if (!widget) widget = inholder(this);
                if (!widget) throw std::runtime_error
                ("property: wrong inholder");
                change(widget, this);
            }
        }
    };
}