#pragma once
#include "gui_widget_text_editor.h"
namespace gui::text
{
    struct one_line_editor:
    widget<one_line_editor>
    {
        editor editor;
        canvas& canvas = editor.canvas;
        view::text_type& text = editor.text;
        view::html_type& html = editor.html;
        property<RGBA>& color = editor.color;
        binary_property<font>& font = editor.font;
        binary_property<style>& style = editor.style;
        binary_property<XY>& alignment = editor.alignment;
        binary_property<int>& lpadding = editor.lpadding;
        binary_property<int>& rpadding = editor.rpadding;
        binary_property<array<XY>>& lwrap = editor.lwrap;
        binary_property<array<XY>>& rwrap = editor.rwrap;
        unary_property<array<range>>& highlights = editor.highlights;
        unary_property<array<range>>& selections = editor.selections;
        binary_property<bool>& wordwrap = editor.wordwrap;
        binary_property<bool>& ellipsis = editor.ellipsis;
        binary_property<bool>& virtual_space = editor.virtual_space;
        binary_property<bool>& insert_mode = editor.insert_mode;
        binary_property<bool>& focused = editor.focused;
        property<bool>& update_text = editor.update_text;
        property<bool>& update_colors = editor.update_colors;
        property<bool>& update_layout = editor.update_layout;

        one_line_editor ()
        {
            ellipsis = false;
            wordwrap = false;
            editor.page.scroll.x.mode = gui::scroll::mode::none;
            editor.page.scroll.y.mode = gui::scroll::mode::none;
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
            editor.go(gui::text::editor::TEXT_BEGIN, false);
            editor.go(gui::text::editor::TEXT_END, true);
        }

        void on_key_pressed (str key, bool down) override
        {
            if (!down) return;

            if (key == "ctrl+C") key = "ctrl+insert"; else // copy
            if (key == "ctrl+V") key = "shift+insert"; else // paste
            if (key == "ctrl+X") key = "shift+delete"; else // cut
            if (key == "ctrl+Z") key = "alt+backspace"; else // undo

            if (key == "shift+insert"     )
            {
                str s = sys::clipboard::get::string();
                s.replace_all("\n", " ");
                editor.insert(s);
            }
            else

            if (key == "enter"            ) {} else
            if (key == "shift+enter"      ) {} else
            if (key == "ctrl+enter"       ) {} else
            if (key == "ctrl+shift+enter" ) {} else

            if (key == "tab"              ) {} else
            if (key == "shift+tab"        ) {} else

            editor.on_key_pressed(key, down);
        }
        void on_key_input (str symbol) override
        {
            editor.on_key_input(symbol);
        }
        void on_focus (bool on) override
        {
            editor.on_focus(on);
        }
    };
}