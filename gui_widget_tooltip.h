#pragma once
#include "gui_widget_text_view.h"
namespace gui
{
    struct tooltip:
    widget<tooltip>
    {
        text::view text;
        text::view* tip;
        property<bool> on = false;
        property<bool> enabled = true;
        property<xywh> area;
        property<time> timer;
        time delay = 300ms;
        time lapse = 5000ms;
        time trans = 200ms;
        time notch;

        tooltip () { tip = new text::view; }
       ~tooltip () { delete tip; }

        void on_change (void* what) override
        {
            if (what == &skin)
            {
                text.canvas.color = skins[skin].tooltip.first;
                text.frame .color = skins[skin].tooltip.second;
                text       .color = skins[skin].tooltip.second;
            }

            if (what == &on
            or  what == &enabled
            or  what == &text.update_text)
            {
                if (on.now
                and enabled.now
                and text.text != "")
                {
                    timer.go(
                    time::infinity,
                    time::infinity);
                    notch = time::now;
                }
                else
                {
                    timer.go({},{});
                    hide_tip();
                }
            }

            if (what == &timer)
            {
                if (time::now - notch - delay > lapse)
                {
                    notch = time::now;
                    timer.go({},{});
                    hide_tip();
                }
                else
                if (time::now - notch > delay and tip->hidden())
                {
                    if (area.now == xywh{})
                        area = parent->
                        coord.now.local();

                    show_tip();
                    position();
                }
            }

            if (what == &area and tip->shown())
            {
                position();
            }
        }

        void show_tip ()
        {
            tip->html = text.html;
            tip->canvas.color = text.canvas.color;
            tip->frame.color = text.frame.color;
            tip->color = text.color;
            tip->frame.thickness =
            metrics::line::width;

            if (tip->parent)
                tip->parent->children.
                try_erase(tip);

            tip->parent = parent;
            while (tip->parent->parent)
                tip->parent = tip->parent->parent;
                    
            tip->parent->children.
                push_back(tip);

            tip->show(trans);
        }

        void hide_tip ()
        {
            tip->hide(trans);
        }

        void position ()
        {
            xywh a = area.now;
            auto w = parent;
            while (w->parent) {
                a += w->coord.now.origin;
                w = w->parent; }

            xywh R = w->coord.now.local();
            auto l = text.frame.thickness.now;

            xywh r;
            r.size = text.text_size();
            r.inflate(metrics::text::height/3);
            r.origin = a.origin;
            r.w += 4*l;
            r.h += 4*l;
            r.y += a.h;

            tip->coord = r;
        }
    };
}
