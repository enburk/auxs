#pragma once
#include "doc_text_model.h"
#include "gui_widget_text_aux.h"
#include "gui_widget_text_page.h"
namespace gui::text
{
    struct editor:
    widget<editor>
    {
        page page;

#define using(x) decltype(page.x)& x = page.x;
        using(canvas)
        using(scroll)
        using(view)
        using(text)
        using(html)
        using(model)
        using(color)
        using(font)
        using(style)
        using(wordwrap)
        using(ellipsis)
        using(alignment)
        using(padding)
        using(lwrap)
        using(rwrap)
        using(update_text)
        using(update_colors)
        using(update_layout)
        using(highlights)
        using(selections)
        using(virtual_space)
        using(insert_mode)
        using(read_only)
        #undef using

        doc::text::model default_model;

        editor ()
        {
            model = &default_model;
            alignment.now = xy{pix::left, pix::top};
            read_only.now = false;
            focusable.now = true;
        }

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.now.size !=
                coord.was.size)
            {
                page.coord = coord.now.local();
            }
            notify(what);
        }

        template<class F> void does (F f)
        {
            if (read_only.now) return;
            model.now->selections = selections;
            if (f()) update_text = true;
        }

        void undo        () { does([=](){ return model.now->undo     (); }); }
        void redo        () { does([=](){ return model.now->redo     (); }); }
        void erase       () { does([=](){ return model.now->erase    (); }); }
        void backspace   () { does([=](){ return model.now->backspace(); }); }
        void insert (str s) { does([=](){ return model.now->insert  (s); }); }

        auto rows() { return view.rows(); }
        auto row(int n) { return view.row(n); }
        auto selected () { return view.selected(); }

        void see (int where) { page.see(where); }
        void go (place place) { page.go(place); }
        void go (int where, bool selective = false) {
            page.go(where, selective); }

        void on_key (str key, bool down, bool input) override
        {
            if (!down) return;
            if (page.touch) return; // mouse
            if (input) { insert(key); return; }
            if (key == "space" ) return;
            if (key.size() <= 1) return; // "", "0", "A", ...
            if (key.size() <= 7 and
                key.starts_with("shift+"))
                return; // shift+0, shift+A, ...

            if (key == "ctrl+C") key = "ctrl+insert"; else // copy
            if (key == "ctrl+V") key = "shift+insert"; else // paste
            if (key == "ctrl+X") key = "shift+delete"; else // cut
            if (key == "ctrl+Z") key = "alt+backspace"; else // undo

            if (key == "ctrl+Y"      ) key = "ctrl+backspace"; else // redo
            if (key == "ctrl+shift+Z") key = "ctrl+backspace"; else // redo
            {}

            if (key == "alt+shift+left" ) key = "shift+left";
            if (key == "alt+shift+right") key = "shift+right";

            if (key == "alt+shift+up"
            or (key == "shift+up" and selections.now.size() > 1))
            {
                auto ss = selections.now;
                int n = ss.size();
                if (n >= 2 &&
                    ss[n-2].upto.line == 
                    ss[n-1].upto.line - 1)
                    ss.truncate();
                else
                if (n >= 1) {
                    auto [from, upto] = ss[n-1];
                    if (upto.line <= 0) return;
                    ss += range{{from.line-1, from.offset},
                                {upto.line-1, upto.offset}};
                }
                selections = ss;
            }
            else
            if (key == "alt+shift+down"
            or (key == "shift+down" and selections.now.size() > 1))
            {
                auto ss = selections.now;
                int n = ss.size();
                if (n >= 2 &&
                    ss[n-2].upto.line == 
                    ss[n-1].upto.line + 1)
                    ss.truncate();
                else
                if (n >= 1) {
                    auto [from, upto] = ss[n-1];
                    if (upto.line >= view.cell.rows()-1) return;
                    ss += range{{from.line+1, from.offset},
                                {upto.line+1, upto.offset}};
                }
                selections = ss;
            }
            else

            if (key == "left" ) go(-GLYPH); else
            if (key == "right") go(+GLYPH); else
            if (key == "up"   ) go(-LINE); else
            if (key == "down" ) go(+LINE); else

            if (key == "ctrl+left" ) go(-TOKEN); else
            if (key == "ctrl+right") go(+TOKEN); else
            if (key == "ctrl+up"   ) see(-LINE); else
            if (key == "ctrl+down" ) see(+LINE); else

            if (key == "shift+left" ) go(-GLYPH, true); else
            if (key == "shift+right") go(+GLYPH, true); else
            if (key == "shift+up"   ) go(-LINE,  true); else
            if (key == "shift+down" ) go(+LINE,  true); else

            if (key == "ctrl+shift+left" ) go(-TOKEN, true); else
            if (key == "ctrl+shift+right") go(+TOKEN, true); else
            if (key == "ctrl+shift+up"   ) go(-LINE,  true); else
            if (key == "ctrl+shift+down" ) go(+LINE,  true); else

            if (key == "home"     ) go(LINE_BEGIN); else
            if (key == "end"      ) go(LINE_END  ); else
            if (key == "page up"  ) { see(-PAGE); go(-PAGE); } else
            if (key == "page down") { see(+PAGE); go(+PAGE); } else

            if (key == "ctrl+home"     ) go(TEXT_BEGIN ); else
            if (key == "ctrl+end"      ) go(TEXT_END   ); else
            if (key == "ctrl+page up"  ) go(PAGE_TOP   ); else
            if (key == "ctrl+page down") go(PAGE_BOTTOM); else

            if (key == "shift+home"     ) go(LINE_BEGIN, true); else
            if (key == "shift+end"      ) go(LINE_END,   true); else
            if (key == "shift+page up"  ) { see(-PAGE); go(-PAGE, true); } else
            if (key == "shift+page down") { see(+PAGE); go(+PAGE, true); } else
            if (key == "ctrl+A"         ) { go(TEXT_BEGIN ); go(TEXT_END, true); } else

            if (key == "ctrl+shift+home"     ) go(TEXT_BEGIN , true); else
            if (key == "ctrl+shift+end"      ) go(TEXT_END   , true); else
            if (key == "ctrl+shift+page up"  ) go(PAGE_TOP   , true); else
            if (key == "ctrl+shift+page down") go(PAGE_BOTTOM, true); else

            if (key == "insert"           ) { insert_mode = !insert_mode.now; } else
            if (key == "shift+insert"     ) { insert(sys::clipboard::get::string()); } else
            if (key == "ctrl+insert"      ) { sys::clipboard::set(selected()); } else
            if (key == "ctrl+shift+insert") {} else // VS: clipboard contex menu

            if (key == "delete"           ) { erase(); } else
            if (key == "shift+delete"     ) { sys::clipboard::set(selected()); erase(); } else
            if (key == "ctrl+delete"      ) {} else
            if (key == "ctrl+shift+delete") {} else

            if (key == "enter"            ) { insert("\n"); } else
            if (key == "shift+enter"      ) { insert("\n"); } else
            if (key == "ctrl+enter"       ) { go(LINE_BEGIN); insert("\n"); } else
            if (key == "ctrl+shift+enter" ) { go(LINE_END  ); insert("\n"); } else

            if (key == "backspace"        ) { backspace(); } else
            if (key == "shift+backspace"  ) { backspace(); } else
            if (key == "alt+backspace"    ) { undo(); } else
            if (key == "ctrl+backspace"   ) { redo(); } else // != VS

            if (key == "tab"              ) { insert("\t"); } else
            if (key == "shift+tab"        ) { insert("shift+\t"); } else
            if (key == "escape"           ) { go(THERE); } else

            {}
        }
    };
} 
