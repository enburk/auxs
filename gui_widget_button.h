#pragma once
#include "gui_widget_image.h"
#include "gui_widget_tooltip.h"
namespace gui
{
    struct button:
    widget<button>
    {
        binary_property<bool> on = false;
        binary_property<bool> enabled = true;
        binary_property<bool> mouse_hover = false;
        binary_property<bool> mouse_clicked = false;
        binary_property<bool> enter_pressed = false;
        enum {normal, toggle, sticky} kind = normal;
        bool notify_hover = false;
        bool notify_off = false;
        time repeat_lapse = time::infinity;
        time repeat_delay = 200ms;
        time repeat_notch;
        timer timer;

        frame frame;
        canvas canvas;
        image image;
        icon icon;
        text::view text;
        tooltip tooltip;

        std::function<void(void)> on_change_state = [this]()
        {
            const auto & style = skins[skin.now];

            frame.color = style.focused.first;
            frame.alpha.go (focus_on ? 255 : 0);

            auto colors = // order important
            not enabled  .now? style.disabled:
            mouse_clicked.now? style.touched:
            enter_pressed.now? style.touched:
            mouse_hover  .now? style.hovered:
            on           .now? style.active:
                               style.soft;

            canvas.color.go(colors.first);
            icon  .color.go(colors.second);
            text  .color.go(colors.second);

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
            or  what == &mouse_clicked
            or  what == &mouse_hover
            or  what == &focus_on
            or  what == &enabled
            or  what == &coord
            or  what == &skin
            or  what == &on)
                on_change_state();

            if (what == &coord and
                coord.was.size !=
                coord.now.size)
                on_change_state();

            if (what == &on)
            if (on.now or kind == toggle
            or  notify_off)
                notify();

            if (what == &mouse_hover)
                tooltip.on =
                mouse_hover;

            if (what == &mouse_hover)
            if (notify_hover)
                notify();

            if (what == &text.update_text)
                notify(what);

            if (what == &timer)
            if (repeat_notch < time::now) {
                if (mouse_clicked.now) notify(); // can take some time
                repeat_notch = time::get() +
                repeat_lapse;
            }
        }

        bool mouse_sensible (xy) override { return enabled.now; }
        void on_mouse_hover (xy) override { mouse_hover = true; }
        void on_mouse_leave (  ) override { mouse_hover = false;}
        void on_mouse_click (xy, str button, bool down) override
        {
            if (button != "left") return;
            
            mouse_clicked = down;
            
            if (enabled.now) {
                switch(kind) {
                case normal: on = down; break;
                case toggle: on = down || on.was != on.now; break;
                case sticky: if (down) { on.now = false; on = true; }; break;
                }
            }
            else // if disabled
            if (kind == normal && !down)
                on = false; // do not stick when disabled on notify

            if (repeat_lapse != time::infinity) {
                repeat_notch = time::now + repeat_delay;
                if (down) timer.start();
                    else  timer.stop();
            }
        }

        bool focusable_now () override {
            return alpha.now > 0
            and focusable.now
            and enabled.now; }

        void on_key (str key, bool down, bool input) override
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
            bool inner_call = false;

            void on_change (void* w) override
            {
                if (inner_call) return;

                int n = -1;
                
                for (int i=0; i<size(); i++)
                    if (w == &at(i))
                        n = i;

                if (n == -1) return;

                if (!(*this)(n).on.now) return;

                inner_call = true;
                for (int i=0; i<size(); i++)
                    if (i != n)
                        at(i).on = false;
                inner_call = false;

                notifier = &at(n);
                notifier_index = n;
                widget::notify();
            }
        };
    };

    struct selector:
    widget<selector>
    {
        canvas canvas;
        radio::group buttons;
        property<int> selected = -1;

        void layout ()
        {
            if (buttons.empty()) return;

            int W = coord.now.w;
            int H = coord.now.h;
            int w = W/buttons.size();
            int e = W - w;
            int x = 0;

            for (auto& button: buttons)
            {
                int v = w;
                if (e > 0) v++;
                if (e > 0) e--;
                button.coord = xywh(
                x, 0, v, H);
                x += v;
            }
        }

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                canvas.coord  = coord.now.local();
                buttons.coord = coord.now.local();
                layout();
            }

            if (what == &skin)
            canvas.color = gui::skins[skin].light.first;

            if (what == &selected
            and selected.now >= 0
            and selected.now < buttons.size())
            buttons(selected.now).on = true;

            if (what == &buttons)
            selected = buttons.notifier_index;

            if (what == &buttons)
            notify();
        }
    };
}
