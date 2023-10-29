#pragma once
#include <set>
#include "sys_ui.h"
#include "gui_effect.h"
namespace gui::base
{
    struct widget : polymorphic
    {
        widget* parent = nullptr;
        array <widget*> children;

        property<xywh> coord;
        property<uint8_t> alpha = 255;
        unary_property<str> skin;
        unary_property<str> name;

        void hide    (bool off, time t=time()) { alpha.go(off? 0 : 255, t); }
        void show    (bool on , time t=time()) { alpha.go(on ? 255 : 0, t); }
        void hide    (          time t=time()) { hide(true, t); }
        void show    (          time t=time()) { show(true, t); }
        void move_to (xywh  r,  time t=time()) { coord.go(r, t); }
        void move_to (xy    p,  time t=time()) { coord.go(xywh(p.x, p.y, coord.to.w, coord.to.h), t); }
        void shift   (xy    d,  time t=time()) { coord.go(xywh(coord.to.x+d.x, coord.to.y+d.y, coord.to.w, coord.to.h), t); }
        void resize  (xy size,  time t=time()) { coord.go(xywh(coord.to.x, coord.to.y, size.x, size.y), t); }

        virtual void on_render (sys::window& window, xywh r, xy offset, uint8_t alpha) {}
        virtual void on_change (void* what) { on_change(); }
        virtual void on_change () {}

        enum Opacity { transparent, semitransparent, opaque };
        virtual Opacity opacity () { return transparent; }

        pix::coord::rectifier<int> updates;
        void update () { update(coord.now.local()); }
        void update (xywh r) {
            if (alpha.now == 0) return; r &= coord.now.local();
            if (parent) parent->update (r +  coord.now.origin);
        /// prevent multiple redraw with OpenGL
        /// (saving rectangles union time also):
            else updates += coord.now.local();
        /// else updates += r;
        }

        void change (void* what)
        {
            if (what == &coord && parent) parent->update(coord.was);
            if (what == &coord && parent) parent->update(coord.now);
            if (what == &alpha && parent) parent->update(coord.now);

            if (what == &focus and focus_on.now) {
            if (focus.was) focus.was->on_focus(false);
            if (focus.now) focus.now->on_focus(true); }

            if (what == &skin)
            for (auto child : children)
                child->skin = skin.now;

            on_change(what);
        }

        void render (sys::window& window, xywh r, xy offset, uint8_t combined_alpha = 255)
        {
            combined_alpha =
            ((combined_alpha+1) * alpha.now) >> 8;
            if (combined_alpha == 0) return;

            // this widget origin is shifted by 'offset'
            // relative to the window frame origin (r.origin)
            on_render (window, r, offset, combined_alpha);

            for(auto child : children) {
                xywh child_global = child->coord.now + r.origin - offset;
                xywh child_frame = r & child_global;
                if  (child_frame.size.x <= 0) continue;
                if  (child_frame.size.y <= 0) continue;
                child->render(window,
                    child_frame,
                    child_frame.origin -
                    child_global.origin,
                    combined_alpha);
            }
        }

        ////////////////////////////////////////////////////////////////////////

        property<bool> focus_on = false;
        property<bool> focusable = false;
        binary_property<widget*> focus = nullptr;

        virtual bool focusable_now ()
        {
            if (alpha.now == 0)
                return false;

            if (focusable.now)
                return true;

            for (auto w : children)
                if (w->focusable_now())
                    return true;

            return false;
        }

        virtual void on_focus (bool on)
        {
            focus_on = on;
            if (focus.now and
                focus.now != this)
                focus.now->on_focus(on);

            else if (on)
            for (auto child: reverse(children))
                if (child->focusable_now()) { focus = child;
                    child->on_focus(on);
                    break; }
        }

        virtual void on_key (str key, bool down, bool input)
        {
            if (focus.now and
                focus.now != this)
                focus.now->on_key(
                key, down, input);
        }

        ////////////////////////////////////////////////////////////////////////

        widget* mouse_click_child = nullptr;
        widget* mouse_hover_child = nullptr;
        double  mouse_wheel_speed = 1.0;
        unary_property<str> mouse_image;

        virtual bool mouse_sensible (xy p) { return false; }
        virtual void on_mouse_click (xy, str button, bool down) {}
        virtual bool on_mouse_wheel (xy, int) { return false; }
        virtual void on_mouse_hover (xy) {}
        virtual void on_mouse_leave () {}

        virtual void on_mouse_click_child (xy, str, bool) {}
        virtual void on_mouse_hover_child (xy) {}

        bool mouse_sense (xy p)
        {
            if (alpha.now == 0) return false;
            if (coord.now.local().excludes(p)) return false;
            for (auto w : children)
                if (w->mouse_sense (p - w->coord.now.origin))
                    return true;
            return mouse_sensible(p);
        }

        void mouse_click (xy p, str button, bool down)
        {
            // button ?
            if (down)
            {
                mouse_click_child = nullptr;
                for (auto w : children)
                    if (w->mouse_sense (p - w->coord.now.origin))
                        mouse_click_child = w; // last sibling wins

                if (mouse_click_child) {

                    if (mouse_click_child->focusable_now()) {
                        focus = mouse_click_child;
                        focus.now->on_focus(
                        focus_on.now); }

                    mouse_click_child->mouse_click (p -
                    mouse_click_child->coord.now.origin, button, true);
                    on_mouse_click_child(p, button, down);
                    return; }
            }
            else
            { 
                if (mouse_click_child) {
                    mouse_click_child->mouse_click (p - 
                    mouse_click_child->coord.now.origin, button, false);
                    mouse_click_child = nullptr;
                    on_mouse_click_child(p, button, down);
                    return; }
            }
            on_mouse_click(p, button, down);
            sys::mouse::image(mouse_image.now);
        }

        void mouse_move(xy p)
        {
            if (mouse_click_child) {
                mouse_click_child->mouse_move(p -
                    mouse_click_child->coord.now.origin);
                return;
            }
            widget* hover = nullptr;
            for (auto w : children)
                if (w->mouse_sense(p - w->coord.now.origin))
                    hover = w; // last sibling wins

            if (hover) hover->mouse_move(p - hover->coord.now.origin);

            if (mouse_hover_child &&
                mouse_hover_child != hover)
                mouse_hover_child->mouse_leave();

            mouse_hover_child = hover;
            // every mouse_move:
            if (mouse_hover_child)
                on_mouse_hover_child(p); else {
                on_mouse_hover(p);
                sys::mouse::image(
                mouse_image.now); }
        }

        void mouse_leave ()
        {
            if (mouse_hover_child) {
                mouse_hover_child->mouse_leave();
                mouse_hover_child = nullptr;
            }
            on_mouse_leave();
        }

        bool mouse_wheel (xy p, int delta)
        {
            if (alpha.now == 0) return false;
            if (coord.now.local().excludes(p)) return false;
            for (auto child: reverse(children))
                if (child->mouse_wheel(p - 
                    child->coord.now.origin, delta))
                    return true;
            return on_mouse_wheel(p, (int)
                (delta*mouse_wheel_speed));
        }

        ////////////////////////////////////////////////////////////////////////

        void notify (void* what = nullptr) {
            if (parent) parent->on_change(
                what ? what : this); }

        ////////////////////////////////////////////////////////////////////////

        size_t size_in_bytes;
        bool inholds (void* p) {
            std::byte* begin = (std::byte*) this;
            std::byte* end = begin + size_in_bytes;
            return begin <= p and p < end;
        }
        widget* descendant_inholder(void* p) {
            if (!inholds(p)) return nullptr;
            for (auto child : children) {
                auto descendant = child->descendant_inholder(p);
                if (descendant) return descendant; }
            return this;
        }
    };
}
namespace gui
{
    std::set<base::widget*> widgets;

    base::widget* inholder (void* p)
    {
        auto it = widgets.upper_bound((base::widget*)p);
        if (it == widgets.begin()) return nullptr;
        return (*--it)->descendant_inholder(p);
    }

    void change (base::widget* widget, void* what) { widget->change(what); }

    template<class T> struct widget : base::widget
    {
        widget ()
        {
            base::widget* p = nullptr;
            p = inholder(this); size_in_bytes = sizeof(T);
            if (p && parent && p != parent) throw std::runtime_error("widget: wrong parent");
            if (p && !parent) { parent = p; parent->children += this; }
            if (!p) widgets.emplace(this);
        }
        widget (base::widget* p)
        {
            size_in_bytes = sizeof(T);
            if (!p) throw std::runtime_error("widget: wrong parent");
            parent = p; parent->children += this;
            widgets.emplace(this);
        }
        widget (widget &&) = delete;
        widget (widget const&) = delete;
       ~widget ()
       {
            update();
            if (parent) {
                parent->children.try_erase(this);
                if (parent->focus == this)
                    parent->focus = nullptr;
                if (parent->mouse_click_child == this)
                    parent->mouse_click_child = nullptr;
                if (parent->mouse_hover_child == this)
                    parent->mouse_hover_child = nullptr;
            }
            auto it = widgets.lower_bound(this);
            if (it != widgets.end() && *it == this)
                widgets.erase(it);
        }
    };
}
