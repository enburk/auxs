#pragma once
#include "sfx_media.h"
namespace sfx::media::audio
{
    struct player:
    widget<player>
    {
        std::atomic<state>
        status = state::finished;
        property<bool> pause = false;
        property<bool> mute = false;
        time duration;
        time elapsed;
        str error;

        sys::thread thread;
        property<time> timer;
        using byte = sys::byte;
        sys::audio::player audio;

        ~player () { reset(); }

        void load (array<byte> bytes)
        {
            reset();
            status = state::loading;
            thread = [this, data = std::move(bytes)](auto& cancel)
            {
                sys::audio::decoder decoder(data);

                if (cancel) return;

                audio.load(
                decoder.output,
                decoder.channels,
                decoder.samples,
                decoder.bps);

                status = state::ready;
            };
            timer.go(
            time::infinity,
            time::infinity);
        }

        void play (time rise = time{}, time fade = time{})
        {
        }

        void stop (time fade = time{})
        {
        }

                void play ()
        {
            switch(status) {
            case state::ready:
            case state::finished:
            {
                start = time::now;
                status = state::playing;

                if (muted) {
                    timer.go (time{0}, time{0});
                    timer.go (time{1}, time{3*stay.ms});
                    break; }

                auto duration = time{(int)(audio.duration*1000)};
                duration = max(duration, stay);
                audio.play(0.0, 0.0);
                timer.go (time{0}, time{0});
                timer.go (time{1}, duration);
                break;
            }
            default: break;
            }
        }

        void stop ()
        {
            switch(status) {
            case state::ready:
            case state::playing:

                audio.stop(0.0);
                status = state::finished;
                timer.go (time{},
                          time{});
                break;

            default: break;
            }
        }

        void mute (bool mute)
        {
            if (status == state::playing)
            {
                if (mute)
                {
                    audio.stop(0.0);
                }
                else
                {
                    start = time::now;
                    auto duration = time{(int)(audio.duration*1000)};
                    duration = max(duration, stay);
                    audio.play(0.0, 0.0);
                    timer.go (time{0}, time{0});
                    timer.go (time{1}, duration);
                }
            }
            muted = mute;
        }

        void play () { pause = false; }
        void stop () { pause = true;  }

        void reset ()
        {
            try {
            thread.stop = true;
            thread.join();
            thread.check(); }
            catch (...) {}

            duration = time{};
            elapsed = time{};
            status = state::finished;
        }

        void on_change (void* what) override
        {
            if (what == &timer and frame_ready and not pause)
            {
                frame_ready = false;
                frames[current].hide(); current = (current + 1) % 2;
                frames[current].source = sources[current].crop();
                frames[current].coord = coord.now.local();
                frames[current].show();
            }
            if (what == &timer and thread.done)
            {
                try {
                thread.join();
                thread.check(); }
                catch (std::exception const& e) {
                status = state::failed;
                error = e.what(); }
                timer.go(
                time{},
                time{});
            }
        }



        void reset (media::media_index index_, array<str> links_)
        {

            stop();

            status = state::loading;
        }


        void on_change (void* what) override
        {
            if (what == &timer and timer == time{1})
            {
                if (text.link != "") {
                timer.go(time{0}, 0s);
                timer.go(time{1}, 1s); }
                else status = state::finished;
            }
        }
    };
}

