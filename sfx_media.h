#pragma once
#include <fstream>
#include "sys_aux.h"
#include "pix_sampling.h"
#include "sfx.h"
namespace sfx::media
{
    enum class state {
    loading,  ready,
    playing,  paused,
    finished, failed };

    struct medio:
    widget<medio>
    {
        state status = state::finished;
        property<bool> mute = false;
        property<byte> volume = 255;
        gui::timer loading;
        gui::timer playing;
        xy resolution;
        time duration;
        time elapsed;
        str error;

        void load ()
        {
            loading.start();
            status = state::loading;
            resolution = xy{};
            duration = time{};
            elapsed = time{};
            error = "";
        }
        void stay ()
        {
            loading.stop();
            status = state::ready;
        }
        bool play ()
        {
            if (status != state::ready
            and status != state::paused
            and status != state::finished)
            return false;

            playing.start();
            status = state::playing;
            return true;
        }
        bool stop ()
        {
            playing.stop();

            if (status != state::playing)
            return false;

            status = state::paused;
            return true;
        }
        void done ()
        {
            stay();
            stop();
            status = state::finished;
        }
        void fail (str what)
        {
            stay();
            stop();
            status = state::failed;
            error = what;
        }
        void on_change (void* what) override
        {
            notify(what);
        }
    };

    struct playback:
    widget<playback>
    {
        gui::button play;
        gui::button stop;
        gui::button prev, Prev;
        gui::button next, Next;
        gui::property<bool> enabled = true;
        gui::property<double> roundness = 0.5;

        playback ()
        {
            stop.hide();
            prev.repeat_lapse = 60ms;
            Prev.repeat_lapse = 60ms;
            next.repeat_lapse = 60ms;
            Next.repeat_lapse = 60ms;
        }

        void on_change (void* what) override
        {
            if (what == &roundness 
            or  what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                int W = coord.now.w; if (W <= 0) return;
                int H = coord.now.h; if (H <= 0) return;
                int u = 4+3+3+3+4;
                int w = W/u;
                int x = 0;

                // oooo ooo ooo ooo oooo
                Prev.coord = xywh(x, 0, 4*w, H); x += 4*w;
                prev.coord = xywh(x, 0, 3*w, H); x += 3*w;
                play.coord = xywh(x, 0, 3*w, H);
                stop.coord = xywh(x, 0, 3*w, H); x += 3*w;
                next.coord = xywh(x, 0, 3*w, H); x += 3*w;
                Next.coord = xywh(x, 0, 4*w, H); x += 4*w;

                roundness.now =
                max(0.0,min(1.0,
                roundness.now));

                rgba c = rgba::white;
                int r = (int)(roundness.now*H/4);
                play.icon.load(pix::util::icon("play.play",  play.coord.now.size, c, r));
                stop.icon.load(pix::util::icon("play.pause", stop.coord.now.size, c, r));
                next.icon.load(pix::util::icon("play.next",  next.coord.now.size, c, r));
                prev.icon.load(pix::util::icon("play.prev",  prev.coord.now.size, c, r));
                Next.icon.load(pix::util::icon("play.Next",  Next.coord.now.size, c, r));
                Prev.icon.load(pix::util::icon("play.Prev",  Prev.coord.now.size, c, r));
            }
            if (what == &enabled)
            {
                play.enabled = enabled.now;
                stop.enabled = enabled.now;
                next.enabled = enabled.now;
                prev.enabled = enabled.now;
                Next.enabled = enabled.now;
                Prev.enabled = enabled.now;
            }
            if (what == &play)
            {
                play.hide();
                stop.show();
            }
            if (what == &stop)
            {
                stop.hide();
                play.show();
            }

            notify(what);
        }
    };
}
