#pragma once
#include "sfx_media.h"
namespace sfx::media::image
{
    struct player:
    widget<player>
    {
        int current = 0;
        gui::Image frames[2];
        pix::image<rgba> sources[2];
        std::atomic<bool> frame_ready = false;
        std::atomic<bool> frame_last = false;
        std::atomic<bool> pause = true;
        sys::thread thread;
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
                resolution = xy{};
                frame_ready = true;
                frame_last = true;
                medio.stay();
                return; }

            medio.load();
            thread = [this, data = std::move(bytes)](auto& cancel)
            {
                if (data.size() > 5  
                &&  data[0] == 0x47  // G
                &&  data[1] == 0x49  // I
                &&  data[2] == 0x46  // F
                &&  data[3] == 0x38  // 8
                &&  data[4] == 0x39  // 9
                &&  data[5] == 0x61) // a
                {
                    throw std::runtime_error(
                    "don't support gif yet");
                }
                else
                {
                    sources[1] = pix::unpack(
                    data.from(0)).value();
                    frame_ready = true;
                    frame_last = true;
                }
            };
        }

        void play ()
        {
            pause = false;
            medio.play();
        }

        void stop ()
        {
            pause = true;
            medio.stop();
        }

        void reset ()
        {
            try {
            thread.stop = true;
            thread.join();
            thread.check(); }
            catch (...) {}
            medio.done();
            frame_ready = false;
            frame_last = false;
            frames[0].hide();
            frames[1].hide();
            sources[0].resize(xy{});
            sources[1].resize(xy{});
            current = 0;
        }

        void on_change (void* what) override
        {
            if (what == &coord)
            {
                if (frames[current].alpha.to != 0)
                    frames[current].coord =
                    coord.now.local();
            }
            if (what == &playing and frame_ready and not pause)
            {
                frame_ready = false;
                frames[current].hide(); current = (current + 1) % 2;
                frames[current].source = sources[current].crop();
                frames[current].coord = coord.now.local();
                frames[current].show();

                if (frame_last)
                medio.done();
            }
            if (what == &loading and thread.done)
            {
                resolution = sources[1].size;

                try {
                thread.join();
                thread.check();
                medio.stay(); }
                catch (std::exception const& e) {
                medio.fail(e.what()); }
            }
        }
    };
}
namespace sfx::image {
    using sfx::media::
        image::player;
}
