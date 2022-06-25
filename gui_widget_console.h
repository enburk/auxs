#pragma once
#include <mutex>
#include "gui_widget_text_editor.h"
namespace gui
{
    struct console:
    widget<console>
    {
        text::page page;
        text::view& view = page.view;
        scroll& scroll = page.scroll;
        canvas& canvas = page.canvas;
        str& link = page.link;
        property<time> timer;
        array<str> addon;
        std::mutex mutex;
        bool clear_ = false;
        int limit = 1024*1024;
        str log;

        void operator << (str s)
        {
            if (s == "") return;
            if (not s.ends_with("<br>")) s += "<br>";
            std::lock_guard guard{mutex};
            addon += std::move(s);
        }

        void clear ()
        {
            std::lock_guard guard{mutex};
            clear_ = true;
            addon.clear();
            log.clear();
        }

        void on_change (void* what) override
        {
            if (timer.now == time())
                timer.go(time::infinity,
                         time::infinity);

            if (what == &skin)
            {
                canvas.color = skins[skin].ultralight.first;
                view.alignment = xy{pix::left, pix::top};
            }
            
            if (what == &coord)
            {
                page.coord = coord.now.local();
            }

            if (what == &timer)
            {
                str s; bool add = true;
                {
                    std::lock_guard guard{mutex};

                    if (clear_) {
                        clear_ = false;
                        add = false;
                    }
                    if (log.size() > limit) {
                        log.upto(log.size() - limit/2).erase();
                        log.upto(log.first("<br>")
                           .begin()).erase();

                        s = log; add = false;
                    }

                    s += str(addon);
                    addon.clear();
                    log += s;
                }
                if (s == "" and add) return;
                try
                {
                    auto gg = aux::unicode::array(s);
                }
                catch(std::exception const& e)
                {
                    if (str(e.what()) !=
                    "unicode: broken UTF-8")
                        throw;

                    s = aux::unicode::what(s);
                }

                if (add)
                page.html += s; else
                page.html  = s;
                page.scroll.y.top =
                    max<int>();
            }

            if (what == &link)
            {
                notify(&link);
            }
        }
    };
} 