#pragma once
#include "sfx_media_audio.h"
#include "sfx_media_image.h"
#include "sfx_media_video.h"
namespace sfx::media
{
    struct player:
    widget<player>
    {
        image::player video;
        audio::player audio;
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

        void load (array<byte> video_bytes, array<byte> audio_bytes)
        {
            reset();
            video.load(std::move(video_bytes));
            audio.load(std::move(audio_bytes));
            medio.load();
        }

        void play ()
        {
            if (medio.play())
                video.play(),
                audio.play();
        }

        void stop ()
        {
            if (medio.stop())
                video.stop(),
                audio.stop();
        }

        void reset ()
        {
            video.reset();
            audio.reset();
            medio.done();
        }

        void on_change (void* what) override
        {
            if (what == &coord)
                video.coord =
                coord.now.
                local();

            if (what == &volume)
            {
                video.volume = volume;
                audio.volume = volume;
            }
            if (what == &mute)
            {
                video.mute = mute;
                audio.mute = mute;
            }

            if (what == &playing)
            {
                if (video.status == state::finished
                and audio.status == state::finished)
                    medio.done();

                elapsed = max(video.
                elapsed, audio.
                elapsed);
            }
            if (what == &loading)
            {
                if (video.status == state::ready
                and audio.status == state::ready)
                {
                    medio.stay();
                    resolution = video.resolution;
                    duration = max(video.
                    duration, audio.
                    duration);
                }
            }
            if (what == &playing
            or  what == &loading)
            {
                if (video.status == state::failed)
                {
                    if (
                    error != "")
                    error += "<br>";
                    error += video.error;
                    medio.fail(error);
                }
                if (audio.status == state::failed)
                {
                    if (
                    error != "")
                    error += "<br>";
                    error += audio.error;
                    medio.fail(error);
                }
            }
        }
    };
}
