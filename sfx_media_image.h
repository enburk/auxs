#pragma once
#include "sfx_media.h"
namespace sfx::media::image
{
    struct player:
    widget<player>
    {
        std::atomic<state>
        status = state::finished;
        property<bool> pause = false;
        xy resolution;
        time duration;
        time elapsed;
        str error;

        gui::image frames[2];
        pix::image<rgba> sources[2];
        std::atomic<bool> frame_ready = false;
        int current = 0;

        sys::thread thread;
        property<time> timer;
        using byte = sys::byte;

        ~player () { reset(); }

        void load (pix::frame<rgba> frame)
        {
            reset();
            int next = (current + 1) % 2;
            sources[next].resize(frame.size);
            sources[next].crop().copy_from(frame);
            resolution = sources[next].size;
            status = state::finished;
            frame_ready = true;
        }
        void load (array<byte> bytes)
        {
            reset();
            status = state::loading;
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
                    int next = (current + 1) % 2;
                    sources[next] = pix::unpack(data.from(0)).value();
                    resolution = sources[next].size;
                    status = state::ready;
                    frame_ready = true;
                }
            };
            timer.go(
            time::infinity,
            time::infinity);
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

            resolution = xy{};
            duration = time{};
            elapsed = time{};
            status = state::finished;
            frame_ready = false;
            frames[0].hide();
            frames[1].hide();
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
    };
}
