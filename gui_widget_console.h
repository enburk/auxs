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
        int limit = 64*1024;
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
            addon.clear();
            addon += "<script type=\"text/javascript\"></script>"; // HTML NOOP
            log.clear();
            page.text = "";
        }

        void on_change (void* what) override
        {
            if (timer.now == time())
                timer.go(time::infinity,
                         time::infinity);

            if (what == &skin)
            {
                view.canvas.color = skins[skin].ultralight.first;
                view.alignment = xy{pix::left, pix::top};
            }
            
            if (what == &coord)
            {
                page.coord = coord.now.local();
            }

            if (what == &timer)
            {
                str s;
                {
                    std::lock_guard guard{mutex};
                    s = str(addon);
                    addon.clear();
                }
                if (s == "") return;

                if (log.size() > limit*3/2) {
                    log.erase(log.begin(),
                    log.from(limit/2)
                       .first("<br>")
                       .begin());

                    page.html = log;
                }

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

                log += s;
                page.html += s;
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