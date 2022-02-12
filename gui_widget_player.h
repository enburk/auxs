#pragma once
#include <fstream>
#include "sys_aux.h"
#include "pix_sampling.h"
#include "gui_widget_image.h"
#include "gui_widgetarium.h"
namespace gui
{
    struct media
    {
        enum class state { vacant,
            loading, ready, playing,
            finished, failure };
    };

    struct player : widget<player>
    {
        std::atomic<media::state> state = media::state::vacant;
        std::atomic<time> duration = time{};
        std::atomic<time> elapsed = time{};
        std::atomic<bool> finish = false;
        std::atomic<bool> pause = false;
        XY resolution;
        str error;

        image frames[2];
        pix::image<RGBA> sources[2];
        binary_property<XYXY> padding;
        std::atomic<int> current_frame = 0;
        std::atomic<bool> frame_ready = false;
        property<time> timer;
        std::thread thread;
        using byte = sys::byte;
        array<byte> data;

        ~player () { reset(); }

        void load (pix::frame<RGBA> frame)
        {
            reset();
            int next = (current_frame + 1) % 2;
            sources[next].resize(frame.size);
            sources[next].crop().copy_from(frame);
            resolution = sources[next].size;
            state = media::state::finished;
            frame_ready = true;
        }
        void load (array<sys::byte>::range_type range)
        {
            reset();
            state = media::state::loading;
            thread = std::thread([this, range]()
            {
                try
                {
                    data.resize(range.size());
                    std::copy(range.begin(), range.end(), data.begin());
                    ready();
                }
                catch (std::exception & e) {
                    error = e.what();
                    state = media::state::failure;
                }
            });
        }
        void load (std::filesystem::path path, int offset = 0, int length = 0)
        {
            reset();
            state = media::state::loading;
            thread = std::thread([this, path, offset, length]()
            {
                try
                {
                    std::ifstream ifstream(path, std::ios::binary);

                    int size = length;
                    if (size == 0)
                    {
                        ifstream.seekg(0, std::ios::end);
                        size = (int)ifstream.tellg();
                    }
                    ifstream.seekg(offset, std::ios::beg);

                    data.resize(size);
                    ifstream.read((char*)(data.data()), size);

                    if (false) {
                    std::ofstream ofstream("test.jpg", std::ios::binary);
                    ofstream.write((char*)(data.data()), size); }

                    ready();
                }
                catch (std::exception & e) {
                    error = e.what();
                    state = media::state::failure;
                }
            });
        }

        void play () { pause = false; state = media::state::finished; }
        void stop () { pause = true;  }

        void on_change (void* what) override
        {
            if (timer.now == time())
                timer.go (time::infinity,
                          time::infinity);

            if (what == &coord or what == &padding)
            {
                if (frames[current_frame].alpha.to != 0)
                    frames[current_frame].coord = coord.now.local()
                        + padding;
            }
            if (what == &timer and frame_ready)
            {
                frames[current_frame].hide();
                current_frame = (current_frame + 1) % 2;
                frames[current_frame].source = sources[current_frame].crop();
                frames[current_frame].coord = coord.now.local() + padding;
                frames[current_frame].show();
                frame_ready = false;
            }
        }

        private: void reset ()
        {
            finish = true;
            if (thread.joinable())
                thread.join();

            data.clear();
            resolution = XY{};
            duration = time{};
            elapsed = time{};
            current_frame = 0;
            frame_ready = false;
            state = media::state::finished;
            error = "";
        }
        private: void ready ()
        {
            if (data.size() > 5  
            &&  data[0] == 0x47  // G
            &&  data[1] == 0x49  // I
            &&  data[2] == 0x46  // F
            &&  data[3] == 0x38  // 8
            &&  data[4] == 0x39  // 9
            &&  data[5] == 0x61) // a
            {
                error = "don't support gif yet";
                state = media::state::failure;
            }
            else
            {
                int next = (current_frame + 1) % 2;
                sources[next] = pix::unpack(data.from(0)).value();
                resolution = sources[next].size;
                state = media::state::ready;
                frame_ready = true;
            }
        }
    };
}
