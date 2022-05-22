#pragma once
#include "gui_widget_text_editor.h"
namespace gui::text
{
    struct one_line_editor:
    widget<one_line_editor>
    {
        editor editor;
        canvas& canvas = editor.canvas;
        box::text_type& text = editor.text;
        box::html_type& html = editor.html;
        property<rgba>& color = editor.color;
        binary_property<font>& font = editor.font;
        binary_property<style>& style = editor.style;
        binary_property<xy>& alignment = editor.alignment;
        binary_property<int>& lpadding = editor.lpadding;
        binary_property<int>& rpadding = editor.rpadding;
        binary_property<array<xy>>& lwrap = editor.lwrap;
        binary_property<array<xy>>& rwrap = editor.rwrap;
        unary_property<array<range>>& highlights = editor.highlights;
        unary_property<array<range>>& selections = editor.selections;
        property<bool>& wordwrap = editor.wordwrap;
        property<bool>& ellipsis = editor.ellipsis;
        property<bool>& update_text   = editor.update_text;
        property<bool>& update_colors = editor.update_colors;
        property<bool>& update_layout = editor.update_layout;
        property<bool>& virtual_space = editor.virtual_space;
        property<bool>& insert_mode = editor.insert_mode;
        property<bool>& read_only = editor.read_only;

        one_line_editor ()
        {
            ellipsis = false;
            wordwrap = false;
            editor.scroll.x.mode = gui::scroll::mode::none;
            editor.scroll.y.mode = gui::scroll::mode::none;
        }

        void on_change (void* what) override
        {
            if (what == &coord)
            {
                editor.coord = coord.now.local();
            }
            if (what == &focus_on)
            {
                editor.focus_on = focus_on.now;
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