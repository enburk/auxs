#pragma once
#include "sfx_media.h"
namespace sfx::media::audio
{
    struct player:
    widget<player>
    {
        std::atomic<state>
        status = state::finished;
        property<byte> volume = 255;
        time duration;
        time elapsed;
        str error;

        sys::thread thread;
        property<time> loading;
        property<time> playing;
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
            loading.go(
            time::infinity,
            time::infinity);
        }

        void play ()
        {
            if (
            status != state::ready and
            status != state::paused and
            status != state::finished) return;
            status  = state::playing;
            audio.play();
            playing.go(
            time::infinity,
            time::infinity);
        }

        void stop ()
        {
            if (
            status != state::playing) return;
            status  = state::paused;
            audio.stop();
            playing.go(
            time{},
            time{});
        }

        void reset ()
        {
            try {
            thread.stop = true;
            thread.join();
            thread.check(); }
            catch (...) {}

            stop();
            duration = time{};
            elapsed = time{};
            status = state::finished;
        }

        void on_change (void* what) override
        {
            if (what == &playing)
            {
                if (not audio.playing())
                {
                    status = state::finished;
                    playing.go(
                    time{},
                    time{});
                }
                elapsed = time{(int)(audio.
                position()*1000)};
            }
            if (what == &loading and thread.done)
            {
                try {
                thread.join();
                thread.check(); }
                catch (std::exception const& e) {
                status = state::failed;
                error = e.what(); }

                loading.go(time{},time{});
                duration = time{(int)(audio.
                duration*1000)};
            }
        }
    };
}

