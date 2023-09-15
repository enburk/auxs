#pragma once
#include "sfx_media.h"
namespace sfx::media::image
{
    struct player:
    widget<player>
    {
        gui::Image frames[2];
        pix::image<rgba> sources[2];
        std::atomic<int> current = 0;
        std::atomic<bool> thread_play = false;
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
                try
                {
                    if (data.size() > 5  
                    &&  data[0] == 0x47  // G
                    &&  data[1] == 0x49  // I
                    &&  data[2] == 0x46  // F
                    &&  data[3] == 0x38  // 8
                    &&  data[4] == 0x39  // 9
                    &&  data[5] == 0x61) // a
                    {
                        pix::gif::decoder gif(std::move(data));
                        while (not frame_last)
                        {
                            frame_ready.wait(true); // until it's false
                            if (not gif.next(cancel)) break;
                            int i = (current+1) % 2;
                            sources[i] = gif.image;
                            frame_last  = gif.last;
                            frame_ready = true;
                            thread_play = true;
                        }
                    }
                    else
                    {
                        sources[1] = pix::unpack(
                        data.from(0)).value();
                        frame_last  = true;
                        frame_ready = true;
                    }
                }
                catch(std::exception const& e) {
                medio.fail(e.what()); }
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
            thread_play = false;
            frame_ready = false;
            frame_ready.notify_all();
            thread.join();
            thread.check(); }
            catch (...) {}
            medio.done();
            frame_last = false;
            frames[0].hide();
            frames[1].hide();
            sources[0].resize(xy{});
            sources[1].resize(xy{});
            current = 0;
        }

        void show_next_frame ()
        {
            frames[current].hide(); current = (current + 1) % 2;
            frames[current].source = sources[current].crop();
            frames[current].coord = coord.now.local();
            frames[current].show();
            frame_ready = false;
            frame_ready.notify_all();
            if (frame_last)
            medio.done();
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
                show_next_frame();
            }
            if (what == &playing and thread.done)
            {
                try {
                thread.join();
                thread.check();
                medio.done(); }
                catch (std::exception const& e) {
                medio.fail(e.what()); }
            }
            if (what == &loading and thread_play)
            {
                resolution = sources[1].size;

                if (frame_ready)
                show_next_frame();
                medio.stay();
            }
            if (what == &loading and thread.done)
            {
                resolution = sources[1].size;

                if (frame_ready)
                show_next_frame();

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
