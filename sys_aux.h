#pragma once
#include <atomic>
#include <thread>
#include <filesystem>
#include "aux_abc.h"
#include "pix_color.h"
#include "pix_image.h"
namespace sys
{
    using byte = uint8_t;
    using pix::frame;
    using pix::image;
    using pix::rgba;

    namespace clipboard
    {
        void set (str);
        void set (frame<rgba>);
        namespace get {
            auto image  () -> pix::image<rgba>;
            auto string () -> str;
        }
    }

    namespace settings
    {
        void init (str appname);
        str  load (str name, str default_value);
        int  load (str name, int default_value);
        void save (str name, str value);
        void save (str name, int value);
        void save ();
        void done ();
    }

    struct thread
    {
        std::thread th;
        std::atomic<bool> done = true;
        std::atomic<bool> stop = false;
        std::exception_ptr exception = nullptr;
        std::chrono::high_resolution_clock::
        duration duration;

        thread () = default;
        thread (auto&& f) { *this = std::forward(f); }
       ~thread () { stop = true; join(); }

        void join ()
        {
            if (th.joinable())
                th.join();
        }
        void check ()
        {
            if (exception) {
            auto e = exception;
            exception = nullptr;
            std::rethrow_exception(e); }
        }
        void operator = (auto f)
        {
            stop = true; join();
            stop = false;
            done = false;
            check();

            th = std::thread([this,f]()
            {
                aux::timing t0;
                try { f(stop); }
                catch(...) { exception =
                std::current_exception(); }
                aux::timing t1;
                duration = t1-t0;
                done = true;
            });
        }
    };

    struct process
    {
        size_t handle = 0;
        struct options
        {
            std::filesystem::path out;
            int ms_wait_for_input_idle = 0;
            bool hidden = false;
            bool ascii = false;
        };
        process () noexcept = default;
        process (const process&) = delete;
        process (process&& p) noexcept { handle = p.handle; }
        process (std::filesystem::path, str args, options);
        process (std::filesystem::path, str args);
       ~process ();

        bool wait (int ms = max<int>());
    };

    struct directory_watcher
    {
        using path = std::filesystem::path; path dir;
        std::function<void(path, str)> action = [](path, str){};
        std::function<void(aux::error)> error = [](aux::error){};
        std::thread thread; void watch(); void cancel();
        std::atomic<bool> stop = false;
        ~directory_watcher(){cancel();}
    };

    namespace audio
    {
        struct player
        {
            player();
            player(player &&) = default;
            player(player const&) = delete;
            player& operator = (player const&) = delete;
            player& operator = (player &&) = default;
           ~player();

            void load (array<byte>, int channels, int samples, int bps);
            void play (double rise = 0.0, double fade = 0.0);
            void stop (double fade = 0.0);
            double duration = 0.0;

            void* data_ = nullptr;
        };

        struct decoder
        {
            decoder (const
            array<byte>& input);
            array<byte> output;
            int channels = 0;
            int samples = 0;
            int bps = 0;
        };
    }
}

namespace pix
{
    using byte = sys::byte;

    expected<image<rgba>> read   (std::filesystem::path);
    expected<nothing>     write  (frame<rgba>, std::filesystem::path, int quality = -1);
    expected<array<byte>> pack   (frame<rgba>, str format, int quality = -1);
    expected<image<rgba>> unpack (byte* buffer, int size);
    expected<image<rgba>> unpack (array<byte>::range_type);
    expected<xy>          size   (array<byte>::range_type);
}
