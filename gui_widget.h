#pragma once
#include <set>
#include "sys_ui.h"
#include "pix_image.h"
#include "gui_effect.h"
namespace gui::base
{
    struct widget : polymorphic
    {
        widget* parent = nullptr;
        array <widget*> children;

        property<XYWH> coord;
        property<uint8_t> alpha = 255;
        unary_property<str> skin;

        void hide    (bool off, time t=time()) { alpha.go(off? 0 : 255, t); }
        void show    (bool on , time t=time()) { alpha.go(on ? 255 : 0, t); }
        void hide    (          time t=time()) { hide(true, t); }
        void show    (          time t=time()) { show(true, t); }
        void move_to (XYWH  r,  time t=time()) { coord.go(r, t); }
        void move_to (XY    p,  time t=time()) { coord.go(XYWH(p.x, p.y, coord.to.w, coord.to.h), t); }
        void shift   (XY    d,  time t=time()) { coord.go(XYWH(coord.to.x+d.x, coord.to.y+d.y, coord.to.w, coord.to.h), t); }
        void resize  (XY size,  time t=time()) { coord.go(XYWH(coord.to.x, coord.to.y, size.x, size.y), t); }

        virtual void on_render (sys::window& window, XYWH r, XY offset, uint8_t alpha) {}
        virtual void on_change (void* what) { on_change(); }
        virtual void on_change () {}

        enum Opacity { transparent, semitransparent, opaque };
        virtual Opacity opacity () { return transparent; }

        rectifier updates;
        void update () { update (coord.now.local()); }
        void update (XYWH r) {
            if (alpha.now == 0) return; r &= coord.now.local();
            if (parent) parent->update (r +  coord.now.origin);
        /// prevent multiple redraw with OpenGL
        /// (saving rectangles union time also):
            else updates += coord.now.local();
        /// else updates += r;
        }

        void change (void* what) {
            if (what == &coord && parent) parent->update(coord.was);
            if (what == &coord && parent) parent->update(coord.now);
            if (what == &alpha && parent) parent->update(coord.now);
            if (what == &skin) for (auto child : children)
                child->skin = skin.now;
            on_change(what);
        }

        void render (sys::window& window, XYWH r, XY offset, uint8_t combined_alpha = 255)
        {
            combined_alpha =
            ((combined_alpha+1) * alpha.now) >> 8;
            if (combined_alpha == 0) return;

            // this widget origin is shifted by 'offset'
            // relative to the window frame origin (r.origin)
            on_render (window, r, offset, combined_alpha);

            for(auto child : children) {
                XYWH child_global = child->coord.now + r.origin - offset;
                XYWH child_frame = r & child_global;
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

        widget* mouse_press_child = nullptr;
        widget* mouse_hover_child = nullptr;
        double  mouse_wheel_speed = 1.0;
        unary_property<str> mouse_image;

        virtual bool mouse_sensible (XY p) { return false; }
        virtual void on_mouse_press (XY, char button, bool down) {}
        virtual bool on_mouse_wheel (XY, int) { return false; }
        virtual void on_mouse_hover (XY) {}
        virtual void on_mouse_leave () {}

        virtual void on_mouse_press_child (XY, char, bool) {}
        virtual void on_mouse_hover_child (XY) {}

        bool mouse_sense (XY p)
        {
            if (alpha.now == 0) return false;
            if (coord.now.local().excludes(p)) return false;
            for (auto w : children)
                if (w->mouse_sense (p - w->coord.now.origin))
                    return true;
            return mouse_sensible(p);
        }

        void mouse_press (XY p, char button, bool down)
        {
            // button ?
            if (down) {
                mouse_press_child = nullptr;
                for (auto w : children)
                    if (w->mouse_sense (p - w->coord.now.origin))
                        mouse_press_child = w; // last sibling wins

                if (mouse_press_child) {
                    mouse_press_child->mouse_press (p -
                    mouse_press_child->coord.now.origin, button, true);
                    on_mouse_press_child(p, button, down);
                    return;
                }
            }
            else { 
                if (mouse_press_child) {
                    mouse_press_child->mouse_press (p - 
                    mouse_press_child->coord.now.origin, button, false);
                    mouse_press_child = nullptr;
                    on_mouse_press_child(p, button, down);
                    return;
                }
            }
            on_mouse_press(p, button, down);
            sys::mouse::image(mouse_image.now);
        }

        void mouse_move(XY p)
        {
            if (mouse_press_child) {
                mouse_press_child->mouse_move(p -
                    mouse_press_child->coord.now.origin);
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
                mouse_hover_child->mouse_leave ();
                mouse_hover_child = nullptr;
            }
            else on_mouse_leave ();
        }

        bool mouse_wheel (XY p, int delta)
        {
            if (alpha.now == 0) return false;
            if (coord.now.local().excludes(p)) return false;
            for (auto it = children.rbegin(); it != children.rend(); ++it)
                if ((*it)->mouse_wheel(p - (*it)->coord.now.origin, delta))
                    return true;
            return on_mouse_wheel(p, int(delta*mouse_wheel_speed));
        }

        ////////////////////////////////////////////////////////////////////////

        virtual void on_focus (bool) {}
        virtual void on_key_input (str) {}
        virtual void on_key_pressed (str, bool) {}

        ////////////////////////////////////////////////////////////////////////

        void notify (void* what = nullptr) {
            if (parent) parent->on_change(
                what ? what : this); }

        ////////////////////////////////////////////////////////////////////////

        size_t size_in_bytes;
        bool inholds (void* p) {
            std::byte* begin = (std::byte*) this;
            std::byte* end = begin + size_in_bytes;
            return begin <= p && p < end;
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
    inline void change (base::widget* widget, void* what) {
        widget->change (what);
    }

    inline std::set<base::widget*> widgets;

    inline base::widget* inholder (void* p) {
        auto it = widgets.upper_bound((base::widget*)p);
        if (it == widgets.begin()) return nullptr;
        return (*--it)->descendant_inholder(p);
    }

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
        widget (widget &&) = delete;
        widget (widget const&) = delete;
       ~widget ()
       {
            update();
            if (parent)
                parent->children.try_erase(this);
            auto it = widgets.lower_bound(this);
            if (it != widgets.end() && *it == this)
                widgets.erase(it);
        }
    };
}
