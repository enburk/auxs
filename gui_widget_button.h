#pragma once
#include "gui_colors.h"
#include "gui_widget_canvas.h"
#include "gui_widget_image.h"
#include "gui_widget_text_view.h"
namespace gui
{
    struct button:
    widget<button>
    {
        binary_property<bool> on = false;
        binary_property<bool> enabled = true;
        binary_property<bool> focused = false;
        binary_property<bool> mouse_hover = false;
        binary_property<bool> mouse_pressed = false;
        binary_property<bool> enter_pressed = false;
        enum {normal, toggle, sticky} kind = normal;
        bool notify_hover = false;
        bool notify_off = false;
        bool repeating = false;
        time repeat_delay = 500ms;
        time repeat_lapse = 100ms;
        time repeat_notch;
        property<time> timer;

        frame frame;
        canvas canvas;
        image image;
        icon icon;
        text::view text;

        std::function<void(void)> on_change_state = [this]()
        {
            const auto & style = skins[skin.now];

            frame.color = style.focused.first;
            frame.alpha.go (focused ? 255 : 0);

            auto colors = style.normal;

            // order important
            if (!enabled     .now) colors = style.disabled; else
            if (mouse_pressed.now) colors = style.touched; else
            if (enter_pressed.now) colors = style.touched; else
            if (on           .now) colors = style.active; else
            if (mouse_hover  .now) colors = style.hovered;

            canvas.color.go(colors.first);
            icon.color.go(colors.second);
            text.color.go(colors.second);

            auto r = coord.now.local();
            frame.thickness = metrics::line::width;
            frame.coord = r; r.deflate(frame.thickness.now); canvas.coord = r;
            image.coord = r; r.deflate(frame.thickness.now);
            icon .coord = r;
            text .coord = r;
        };

        void on_change (void* what) override
        {
            if (what == &enter_pressed
            or  what == &mouse_pressed
            or  what == &mouse_hover
            or  what == &focused
            or  what == &enabled
            or  what == &coord
            or  what == &skin
            or  what == &on)
                on_change_state();

            if (what == &on)
            if (on.now or notify_off or kind == toggle)
                notify();

            if (what == &mouse_hover)
            if (notify_hover)
                notify();

            if (what == &timer)
            if (repeat_notch < time::now) {
                repeat_notch = time::now + repeat_lapse;
                if (mouse_pressed.now) notify();
            }
        }

        bool mouse_sensible (XY) override { return enabled.now; }
        void on_mouse_hover (XY) override { mouse_hover = true; }
        void on_mouse_leave (  ) override { mouse_hover = false;}
        void on_mouse_press (XY, char button, bool down) override
        {
            if (button != 'L') return;
            
            mouse_pressed = down;
            
            if (enabled.now) {
                switch(kind) {
                case normal: on = down; break;
                case toggle: on = down || on.was != on.now; break;
                case sticky: if (down) { on.now = false; on = true; }; break;
                }
            }
            else // if disabled
                if (kind == normal && !down)
                    on = false; // do not stick on disable on notify

            if (repeating) {
                repeat_notch = time::now + repeat_delay;
                timer.go (down ? time::infinity : time(),
                          down ? time::infinity : time());
            }
        }

        void on_focus (bool on) override { focused = on; }

        void on_key_pressed (str key, bool down) override
        {
            if (key == "enter") enter_pressed = down;
        }
    };

    struct checkbox : button
    {
        checkbox ()
        {
            on_change_state = []()
            {
            };
        }
    };

    struct radio
    {
        struct button : gui::button
        {
            button ()
            {
                kind = gui::button::sticky;
            }
        };

        struct group : widgetarium<button>
        {
            void on_change (void* w) override
            {
                int n = -1;
                
                for (int i=0; i<size(); i++)
                    if (w == &(*this)(i))
                        n = i;

                if (n == -1) return;

                if (!(*this)(n).on.now) return;

                if (!(*this)(n).mouse_pressed.now) return;

                for (int i=0; i<size(); i++)
                    if (i != n)
                        (*this)(i).on = false;

                notifier = &(*this)(n);
                notifier_index = n;
                widget::notify();
            }
        };
    };
}
