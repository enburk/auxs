#pragma once
#include "sfx_media_audio.h"
namespace sfx::media::audios
{
    struct player:
    widget<player>
    {
        widgetarium<sfx::audio::player> audios;
        int current = 0;
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

        void load (array<array<byte>> bytess)
        {
            int n = bytess.size();
            for (int i = 0; i<n; i++)
            audios(i).load(std::move(bytess[i]));
            audios.truncate(n);
            medio.load();
            current = 0;
        }

        void play ()
        {
            if (medio.play()
            and audios.size() > current)
                audios[current].play();
        }

        void stop ()
        {
            if (medio.stop()
            and audios.size() > current)
                audios[current].stop();
        }

        void reset ()
        {
            audios.clear();
            medio.done();
        }

        void on_change (void* what) override
        {
            if (what == &playing
            and audios.size() > current)
            {
                elapsed = {};
                for (auto& audio: audios)
                elapsed += audio.
                elapsed;

                if (audios[current].status ==
                    state::playing)
                    return;

                if (audios[current].status ==
                    state::failed) { medio.fail(
                    audios[current].error);
                    return; }

                current++;
                if (audios.size() > current)
                    audios[current].play();
                    else current = 0,
                    medio.done();
            }
            if (what == &loading)
            {
                for (auto& audio: audios)
                    if (audio.status == state::failed) {
                        medio.fail(audio.error);
                        return; }

                for (auto& audio: audios)
                    if (audio.status == state::loading)
                        return;

                elapsed  = {};
                duration = {};
                for (auto& audio: audios)
                duration += audio.
                duration;

                medio.stay();
            }
            if (what == &volume)
            {
                for (auto&
                audio: audios)
                audio.volume =
                volume;
            }
            if (what == &mute)
            {
                for (auto&
                audio: audios)
                audio.mute =
                mute;
            }
        }
    };
}
namespace sfx::audios {
    using sfx::media::
        audio::player;
}
