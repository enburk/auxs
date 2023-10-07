#pragma once
#include "sfx_media_audio.h"
namespace sfx::media::audios
{
    struct player:
    widget<player>
    {
        widgetarium<sfx::audio::player> audios;
        medio medio;
        int current = 0;

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

        void load (array<array<byte>> bytess)
        {
            int n = bytess.size();
            for (int i = 0; i<n; i++)
            audios(i).load(std::move(bytess[i]));
            audios.truncate(n);
            medio.load();


            //reset();
            //if (bytes.empty()) {
            //    audio = sys::audio::player{};
            //    medio.stay();
            //    return; }
            //
            //thread = [this, data = std::move(bytes)](auto& cancel)
            //{
            //    sys::audio::decoder decoder(data);
            //
            //    if (cancel) return;
            //
            //    audio.load(
            //    decoder.output,
            //    decoder.channels,
            //    decoder.samples,
            //    decoder.bps);
            //};
        }

        void play ()
        {
            //if (medio.play())
            //{
            //    audio.volume(
            //    mute? 0.0:
            //    volume/
            //    255.0);
            //
            //    audio.play();
            //}
        }

        void stop ()
        {
            //if (medio.stop())
            //    audio.stop();
        }

        void reset ()
        {
            //try {
            //thread.stop = true;
            //thread.join();
            //thread.check(); }
            //catch (...) {}
            //medio.done();
        }

        void on_change (void* what) override
        {
            //if (what == &playing)
            //{
            //    elapsed = time{(int)(
            //    audio.position()*1000)};
            //
            //    if (
            //    not audio.playing()
            //    and elapsed == time{})
            //    medio.done();
            //}
            //if (what == &loading and thread.done)
            //{
            //    duration = time{(int)(audio.
            //    duration*1000)};
            //
            //    try {
            //    thread.join();
            //    thread.check();
            //    medio.stay(); }
            //    catch (std::exception const& e) {
            //    medio.fail(e.what()); }
            //}
            //if (what == &mute
            //and status == state::playing)
            //{
            //    audio.position(0);
            //}
            //if (what == &volume
            //or  what == &mute)
            //{
            //    audio.volume(
            //    mute? 0.0:
            //    volume/
            //    255.0);
            //}
        }
    };
}
namespace sfx::audios {
    using sfx::media::
        audio::player;
}
