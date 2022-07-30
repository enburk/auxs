#pragma once
#include "gui_widget_text_editor.h"
namespace gui::text
{
    struct one_line_editor:
    widget<one_line_editor>
    {
        editor editor;

#define using(x) decltype(editor.x)& x = editor.x;
        using(canvas)
        using(text)
        using(html)
        using(color)
        using(font)
        using(style)
        using(alignment)
        using(padding)
        using(update_text)
        using(update_colors)
        using(update_layout)
        using(highlights)
        using(selections)
        using(virtual_space)
        using(insert_mode)
        using(read_only)
        #undef using

        one_line_editor ()
        {
            editor.ellipsis = false;
            editor.wordwrap = false;
            editor.scroll.x.mode = gui::scroll::mode::none;
            editor.scroll.y.mode = gui::scroll::mode::none;
        }

        void on_change (void* what) override
        {
            if (what == &coord)
            {
                editor.coord = coord.now.local();
            }
            notify(what);
        }

        void select ()
        {
            editor.go(TEXT_BEGIN, false);
            editor.go(TEXT_END, true);
        }

        void on_key (str key, bool down, bool input) override
        {
            if (!down) return;

            if (key == "ctrl+C") key = "ctrl+insert"; else // copy
            if (key == "ctrl+V") key = "shift+insert"; else // paste
            if (key == "ctrl+X") key = "shift+delete"; else // cut
            if (key == "ctrl+Z") key = "alt+backspace"; else // undo
            {}

            if (key == "shift+insert"     )
            {
                str s = sys::clipboard::get::string();
                s.replace_all("\n", " ");
                editor.insert(s);
            }
            else

            if (key == "enter"            ) { notify(); } else
            if (key == "shift+enter"      ) {} else
            if (key == "ctrl+enter"       ) {} else
            if (key == "ctrl+shift+enter" ) {} else

            if (key == "tab"              ) {} else
            if (key == "shift+tab"        ) {} else

            editor.on_key(key, down, input);
        }
    };
}