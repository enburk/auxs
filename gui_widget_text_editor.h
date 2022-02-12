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
        lines& lines = page.lines;
        canvas& canvas = page.canvas;
        view::text_type& text = page.text;
        view::html_type& html = page.html;
        property<RGBA>& color = page.color;
        binary_property<font>& font = page.font;
        binary_property<style>& style = page.style;
        binary_property<XY>& alignment = page.alignment;
        binary_property<int>& lpadding = page.lpadding;
        binary_property<int>& rpadding = page.rpadding;
        binary_property<array<XY>>& lwrap = page.lwrap;
        binary_property<array<XY>>& rwrap = page.rwrap;
        unary_property<array<range>>& highlights = page.highlights;
        unary_property<array<range>>& selections = page.selections;
        binary_property<bool>& wordwrap = page.wordwrap;
        binary_property<bool>& ellipsis = page.ellipsis;
        binary_property<bool>& virtual_space = page.virtual_space;
        binary_property<bool>& insert_mode = page.insert_mode;
        binary_property<bool>& focused = page.focused;
        property<bool>& update_text = page.update_text;
        property<bool>& update_colors = page.update_colors;
        property<bool>& update_layout = page.update_layout;

        doc::text::model model_;
        doc::model*& model = page.view.model;

        editor ()
        {
            model = &model_;
            page.alignment = XY{left, top};
        }

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.now.size !=
                coord.was.size)
            {
                page.coord = coord.now.local();
            }
            if (what == &selections and not selections.now.empty())
            {
                XYXY r = page.view.cell.carets.back().coord.now +
                         page.view.shift.now;

                int d = gui::metrics::text::height;
                int w = coord.now.size.x, dx = 0;
                int h = coord.now.size.y, dy = 0;

                if (r.xl-d < 0) dx = r.xl-d; else if (r.xh+d > w) dx = r.xh+d-w;
                if (r.yl-d < 0) dy = r.yl-d; else if (r.yh+d > h) dy = r.yh+d-h;

                if (dx != 0) page.scroll.x.top = page.scroll.x.top.now + dx;
                if (dy != 0) page.scroll.y.top = page.scroll.y.top.now + dy;
            }
                
            notify(what);
        }

        template<class F> void does (F f) {
            model->selections = selections;
            if (f()) update_text = true; }

        void undo        () { does([=](){ return model->undo     (); }); }
        void redo        () { does([=](){ return model->redo     (); }); }
        void erase       () { does([=](){ return model->erase    (); }); }
        void backspace   () { does([=](){ return model->backspace(); }); }
        void insert (str s) { does([=](){ return model->insert  (s); }); }

        enum WHERE { THERE = 0,
             GLYPH, LINE, LINE_BEGIN, LINE_END, PAGE_TOP,
             TOKEN, PAGE, TEXT_BEGIN, TEXT_END, PAGE_BOTTOM,
        };

        void go (int where, bool selective = false)
        {
            auto ss = selections.now;
            int n = ss.size();
            if (n >= 2 and not selective) {
                auto b = ss[0].from; // begin of multiline caret
                auto e = ss[n-1].from; // end of multiline caret
                if ((b < e and (where == +GLYPH or where == +LINE))
                or  (b > e and (where == -GLYPH or where == -LINE)))
                ss[0] = ss[n-1];
                ss.resize(1);
            }

            for (auto& caret: ss)
                go(caret, where, selective);

            selections = ss;
        }
        void go (range& caret, int where, bool selective)
        {
            if (lines.empty()) return;

            auto & [from, upto] = caret;
            auto & [line, offset] = upto;

            int lines_on_page =
                page.view.coord.now.h /
                sys::metrics(font.now).height;

            switch(where){
            case THERE: from = upto; break;

            case-GLYPH:
                offset--;
                if (!virtual_space.now)
                if (offset < 0 and line > 0)
                    offset = lines[--line].length;
                break;
            case+GLYPH:
                offset++;
                if (!virtual_space.now)
                if (offset > lines[line].length and
                    line < lines.size()-1) {
                    line++; offset = 0; }
                break;

            //case-TOKEN: break;
            //case+TOKEN: break;

            case-LINE: line--; break;
            case+LINE: line++; break;

            case LINE_END  : offset = lines[line].length; break;
            case LINE_BEGIN: offset = offset !=
                lines[line].start ?
                lines[line].start : 0;
                break;

            //case PAGE_TOP   : break;
            //case PAGE_BOTTOM: break;

            case-PAGE: line -= lines_on_page; break;
            case+PAGE: line += lines_on_page; break;

            case TEXT_BEGIN: upto = place{}; break;
            case TEXT_END  : upto = place{
                lines.size()-1,
                lines.back().length};
                break;
            }

            if (line > lines.size()-1)
                line = lines.size()-1;
            if (line < 0)
                line = 0;

            if (offset > lines[line].length && !virtual_space.now)
                offset = lines[line].length;
            if (offset < 0)
                offset = 0;

            if (!selective) from = upto;
        }

        void go (place place)
        {
            page.scroll.y.top = place.line *
                sys::metrics(font.now).height -
                    page.view.coord.now.h / 2;

            selections = array<range>{
                range{place, place}};
        }

        void show (int where)
        {
            int h = sys::metrics(font.now).height;

            switch(where){
            case-LINE:
                page.scroll.y.top =
                page.scroll.y.top.now - h;
                break;
            case+LINE:
                page.scroll.y.top =
                page.scroll.y.top.now + h;
                break;
            case-PAGE:
                page.scroll.y.top =
                page.scroll.y.top.now -
                page.view.coord.now.h/h*h;
                break;
            case+PAGE:
                page.scroll.y.top =
                page.scroll.y.top.now +
                page.view.coord.now.h/h*h;
                break;
            }
        }

        auto selected () { return page.selected(); }

        void on_key_pressed (str key, bool down) override
        {
            if (!down) return;
            if (page.touch) return; // mouse
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

            if (key == "alt+shift+up")
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
            if (key == "alt+shift+down")
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
                    if (upto.line >= lines.size()-1) return;
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
            if (key == "ctrl+up"   ) show(-LINE); else
            if (key == "ctrl+down" ) show(+LINE); else

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
            if (key == "page up"  ) { show(-PAGE); go(-PAGE); } else
            if (key == "page down") { show(+PAGE); go(+PAGE); } else

            if (key == "ctrl+home"     ) go(TEXT_BEGIN ); else
            if (key == "ctrl+end"      ) go(TEXT_END   ); else
            if (key == "ctrl+page up"  ) go(PAGE_TOP   ); else
            if (key == "ctrl+page down") go(PAGE_BOTTOM); else

            if (key == "shift+home"     ) go(LINE_BEGIN, true); else
            if (key == "shift+end"      ) go(LINE_END,   true); else
            if (key == "shift+page up"  ) { show(-PAGE); go(-PAGE, true); } else
            if (key == "shift+page down") { show(+PAGE); go(+PAGE, true); } else

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
        void on_key_input (str symbol) override
        {
            if (!page.touch) insert(symbol);
        }
        void on_focus (bool on) override
        {
            page.view.on_focus(on);
        }
    };
} 
