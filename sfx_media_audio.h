#pragma once
#include "sfx_media.h"
namespace sfx::media::audio
{
    struct player:
    widget<player>
    {
        sys::thread thread;
        sys::audio::player audio;
        medio medio;

#define using(x) decltype(medio.x)& x = medio.x;
        using(mute)
        using(volume)
        using(loading)
        using(playing)
        using(resolution)
        using(duration)
        using(elapsed)
        using(status)
        using(error)
        #undef using

        ~player () { reset(); }

        void load (array<byte> bytes)
        {
            reset();
            if (bytes.empty()) {
                medio.stay();
                return; }

            medio.load();
            thread = [this, data = std::move(bytes)](auto& cancel)
            {
                sys::audio::decoder decoder(data);

                if (cancel) return;

                audio.load(
                decoder.output,
                decoder.channels,
                decoder.samples,
                decoder.bps);
            };
        }

        void play ()
        {
            if (medio.play())
            {
                audio.volume(
                mute? 0.0:
                volume/
                255.0);

                audio.play();
            }
        }

        void stop ()
        {
            if (medio.stop())
                audio.stop();
        }

        void reset ()
        {
            try {
            thread.stop = true;
            thread.join();
            thread.check(); }
            catch (...) {}
            medio.done();
        }

        void on_change (void* what) override
        {
            if (what == &playing)
            {
                elapsed = time{(int)(
                audio.position()*1000)};

                if (
                not audio.playing()
                and elapsed == time{})
                medio.done();
            }
            if (what == &loading and thread.done)
            {
                duration = time{(int)(audio.
                duration*1000)};

                try {
                thread.join();
                thread.check();
                medio.stay(); }
                catch (std::exception const& e) {
                medio.fail(e.what()); }
            }
            if (what == &mute
            and status == state::playing)
            {
                audio.position(0);
            }
            if (what == &volume
            or  what == &mute)
            {
                audio.volume(
                mute? 0.0:
                volume/
                255.0);
            }
        }
    };
}
namespace sfx::audio {
    using sfx::media::
        audio::player;
}

