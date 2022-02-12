#pragma once
#include <thread>
#include <atomic>
#include "pix_color.h"
#include "pix_image.h"
#include "gui_colors.h"
#include "gui_effect.h"
namespace sys
{
    using namespace data;
    using namespace pix;

    namespace screen
    {
        inline XY size;
    }
    namespace keyboard
    {
        inline bool alt = false;
        inline bool ctrl = false;
        inline bool shift = false;
    }
    namespace mouse
    {
        void image(str image);
        auto position() -> XY;
        void position(XY);
    }

    enum class choice
    {
        ok,               
        ok_cancel,
        yes_no,
        yes_no_cancel,
        abort_retry_ignore,
        cancel_try_continue,
        retry_cancel
    };
    str dialog (str title, str text, choice, void* handle = nullptr);

    struct window : polymorphic
    {
        pix::image<RGBA> image;
        void*native_handle1 = nullptr;
        void*native_handle2 = nullptr;
        bool gpu = false;

        void create (str title);
        void update ();
        void timing ();

        str dialog (str title, str text, choice choice) {
            return sys::dialog(title, text, choice,
                native_handle1); }

        virtual void on_timing () = 0;
        virtual void on_resize (XY size) = 0;
        virtual void keyboard_on_focus (bool on) = 0;
        virtual void keyboard_on_press (str key, bool down) = 0;
        virtual void keyboard_on_input (str symbol) = 0;
        virtual void mouse_on_press (XY p, char button, bool down) = 0;
        virtual void mouse_on_wheel (XY p, int delta) = 0;
        virtual void mouse_on_move  (XY p) = 0;
        virtual void mouse_on_leave () = 0;
        virtual void render (XYWH, uint8_t alpha, RGBA);
        virtual void render (XYWH, uint8_t alpha, frame<RGBA>);
        virtual void render (XYWH, uint8_t alpha, glyph, XY, int);
        virtual void renderr () {}

        std::thread timer;
        std::atomic<bool> timer_stop = true;
        std::function<void(void)> timer_proc = [this]()
        {
            while (true) {
                using namespace std::literals::chrono_literals;
                if (timer_stop) break; std::this_thread::sleep_for(16ms); // 60 FPS
                if (timer_stop) break; timing();
            }
        };

        void on_finish  () { on_turn_off(); }
        void on_start   () { on_turn_on (); }
        void on_turn_on () { timer_stop = false; timer = std::thread (timer_proc); }
        void on_turn_off() { timer_stop = true; if (timer.joinable()) timer.join(); }
        void on_pause   () { timer_stop = true; }
        void on_resume  () {
             on_turn_off(); gui::time::set_after_pause();
             on_timing  (); gui::time::reset_after_pause();
             on_turn_on ();
        }
    };

    template<class Widget> struct the_window : window
    {
        Widget widget;

        void keyboard_on_focus(bool on) override {
             widget.on_focus(on);
             on_timing();
        }
        void keyboard_on_press(str key, bool down) override {
             widget.on_key_pressed(key, down);
             on_timing();
        }
        void keyboard_on_input(str symbol) override { if (symbol != "")
             widget.on_key_input(symbol);
             on_timing();
        }
        void mouse_on_press(XY p, char button, bool down) override {
             widget.mouse_press(p, button, down);
             on_timing();
        }
        void mouse_on_wheel(XY p, int delta) override {
             widget.mouse_wheel(p, delta);
             on_timing();
        }
        void mouse_on_move(XY p) override {
             widget.mouse_move(p);
             on_timing();
        }
        void mouse_on_leave() override {
             widget.mouse_leave();
             on_timing();
        }
    };

    template<class Widget> struct pix_window : the_window<Widget>
    {
        using the_window<Widget>::widget;
        using the_window<Widget>::image;

        void on_resize (XY size) override {
             image .resize(size);
             widget.resize(size);
             on_timing();
        }
        void on_timing() override
        {
             gui::time::set();
             auto active_properties_copy = gui::active_properties;
             active_properties_copy.for_each([](auto p){ p->tick(); });
             for (XYWH r : widget.updates) widget.render(*this, r, r.origin);
             widget.updates.clear();
        }
        void render (XYWH r, uint8_t alpha, RGBA color) override
        {
            image.crop(r).blend(color, alpha);
        }
        void render (XYWH r, uint8_t alpha, frame<RGBA> frame) override
        {
            image.crop(r).blend_from(frame, alpha);
        }
        void render (XYWH r, uint8_t alpha, glyph g, XY offset, int x) override
        {
            g.render(image.crop(r), offset, alpha, x);
        }
    };

    template<class Widget> struct gpu_window : the_window<Widget>
    {
        using the_window<Widget>::gpu;
        using the_window<Widget>::widget;
        using the_window<Widget>::update;
        using the_window<Widget>::image;

        gpu_window() { gpu = true; }

        void on_resize (XY size) override {
             widget.resize(size);
             widget.update();
             on_timing();
        }
        void on_timing() override
        {
             gui::time::set();
             auto active_properties_copy = gui::active_properties;
             active_properties_copy.for_each([](auto p){ p->tick(); });
             image.updates = widget.updates;
             widget.updates.clear();
             update();
        }
        void renderr () override {
            widget.render(*this,
            widget.coord, XY{});
        }
    };

    struct app_base : polymorphic
    {
        str title;
        virtual void constructor() = 0;
        virtual void destructor () = 0;
    };
    struct app_instance
    {
        static void init() { gui::init(); }
        static void done() { gui::done(); }
        static inline app_base * app = nullptr;
        app_instance () { init(); if (app) app->constructor(); }
       ~app_instance () { if (app) app->destructor();  done(); }
    };
    template<class Widget> struct app : app_base
    {
        gpu_window<Widget> * winptr = nullptr;
        app (str s) { app_instance::app = this; title = s; }
        void destructor () override { delete winptr; }
        void constructor() override { winptr = new
            typename std::remove_reference
            <decltype(*winptr)>::type{};
            winptr->create(title);
        }
    };
}