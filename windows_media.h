#pragma once
#include "windows_system.h"

extern HWND MainWindowHwnd;

#ifdef AUXS_USE_MP3
//#define MINIMP3_ONLY_MP3
//#define MINIMP3_ONLY_SIMD
//#define MINIMP3_NO_SIMD
//#define MINIMP3_NONSTANDARD_BUT_LOGICAL
//#define MINIMP3_FLOAT_OUTPUT
#define MINIMP3_IMPLEMENTATION
#include "minimp3.h"
#endif

#ifdef AUXS_USE_OGG
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#ifdef _MSC_VER
#pragma comment ( lib, "vorbis.lib" )
#pragma comment ( lib, "vorbisenc.lib" )
#pragma comment ( lib, "vorbisfile.lib" )
#pragma comment ( lib, "ogg.lib" )
#endif
namespace sys::audio
{
    decoder::decoder (const array<byte> & input)
    {
        if (input.empty())
            return;

        OggVorbis_File vf;
        byte pcmout[4096];
        int current_section;
        int eof=0;
        int rc;

        auto data = (char*)(input.data());
        auto size = input.size();

        rc = ov_open_callbacks(0, &vf, data, size, OV_CALLBACKS_DEFAULT);
        
#ifdef AUXS_USE_MP3
        if (rc < 0)
        {
            static mp3dec_t mp3d;
            mp3dec_init(&mp3d);

            mp3dec_frame_info_t info;
            short pcm[MINIMP3_MAX_SAMPLES_PER_FRAME];
            byte* bytes = (byte*)pcm;

            output.reserve(size);

            while (size > 0)
            {
                int samples = mp3dec_decode_frame(&mp3d, (byte*)data, size, pcm, &info);

                output.insert(output.end(), bytes, bytes + samples*info.channels*2);

                data += info.frame_bytes;
                size -= info.frame_bytes;
                if (info.frame_bytes == 0)
                    break;
            }

            channels = info.channels;
            samplerate = info.hz;
            bps = 16;
            return;
        }
#endif // MP3

        if (rc < 0) throw std::runtime_error(
            "Input does not appear to be "
            "an Ogg bitstream.");

        vorbis_info *vi=ov_info(&vf,-1);
        channels = vi->channels;
        samplerate = vi->rate;
        bps = 16;
          
        while (!eof)
        {
            long ret = ov_read(&vf,
                (char*)(pcmout), sizeof(pcmout),
                0, 2, 1, &current_section);

            if (ret == 0) eof=1; else
            if (ret <  0) {} // minor error
            else output.insert(output.end(),
                std::begin(pcmout),
                std::begin(pcmout)+ret);
        }

        ov_clear(&vf);
    }
}
#endif // OGG

#ifdef AUXS_USE_SOUNDTOUCH
#include "vcpkg_installed/vcpkg/pkgs/soundtouch_x64-windows/include/soundtouch/SoundTouch.h"
#pragma comment(lib, "vcpkg_installed/vcpkg/pkgs/soundtouch_x64-windows/lib/SoundTouch.lib")
auto speeded(array<byte> const& input, int channels, int samplerate, int bps, double speed)
{
    if (bps != 16 or not std::is_same_v<soundtouch::SAMPLETYPE, float>) return input;

    int samples = input.size() / (bps/8);

    array<float> floats;
    floats.resize(samples);
    short* src = (short*)input.data();
    float* dst = (float*)floats.data();
    for (int i=0; i<samples; i++)
    dst[i] = src[i] / 32767.0f;

    soundtouch::SoundTouch soundTouch;
    soundTouch.setSampleRate(samplerate);
    soundTouch.setChannels(channels);
    soundTouch.setTempo(speed);
    // use settings for speech processing
    soundTouch.setSetting(SETTING_SEQUENCE_MS, 40);
    soundTouch.setSetting(SETTING_SEEKWINDOW_MS, 15);
    soundTouch.setSetting(SETTING_OVERLAP_MS, 8);
    //soundTouch.setSetting(SETTING_USE_QUICKSEEK, 1); // gain speed, lose quality

    array<byte> output;
    output.reserve(input.size());

    const int buffer_size = 6720; // divisible by 2, 4, 6, 8, 10, 12, 14, 16 channels
    float buffer[buffer_size];

    short sample;
    byte& byte0 = *((byte*)(&sample) + 0);
    byte& byte1 = *((byte*)(&sample) + 1);

    for (int offset = 0; offset<floats.size(); offset += buffer_size)
    {
        float* data = floats.data() + offset;
        int size = min(buffer_size, floats.size() - offset);
        soundTouch.putSamples(data, size/channels);

        while (true)
        {
            int n = soundTouch.receiveSamples(buffer, buffer_size/channels);
            if (n == 0) break;

            for (int i=0; i<n*channels; i++)
            sample = aux::clamp<short>(buffer[i] * 32767.0f),
            output += byte0,
            output += byte1;
        }
    }
    soundTouch.flush();
    while (true)
    {
        int n = soundTouch.receiveSamples(buffer, buffer_size/channels);
        if (n == 0) break;

        for (int i=0; i<n*channels; i++)
        sample = aux::clamp<short>(buffer[i] * 32767.0f),
        output += byte0,
        output += byte1;
    }

    return output;
}
#else
auto speeded(array<byte> const& input, int channels, int samplerate, int bps, double speed){ return input; }
#endif // SOUNDTOUCH

#include <dsound.h>
#pragma comment(lib, "dsound.lib")
namespace sys::audio
{
    struct Device
    {
        LPDIRECTSOUND8      DS = nullptr;
        LPDIRECTSOUNDBUFFER PB = nullptr; // primary buffer
        LPDIRECTSOUNDBUFFER B1 = nullptr; // secondary buffer
        LPDIRECTSOUNDBUFFER B2 = nullptr; // secondary buffer

        double duration = 0.0; DWORD size = 0;

        Device ()
        {
            HRESULT hr;
            hr = DirectSoundCreate8(0, &DS, 0);
            if (FAILED(hr)) throw std::runtime_error(
                "DirectSoundCreate failed");

            hr = DS->SetCooperativeLevel(MainWindowHwnd, DSSCL_PRIORITY);
            if (FAILED(hr)) throw std::runtime_error(
                "SetCooperativeLevel failed");

            DSBUFFERDESC desc;
            ZeroMemory (&desc, sizeof(desc));
            desc.dwSize      = sizeof(desc);
            desc.dwFlags = DSBCAPS_PRIMARYBUFFER;
            desc.guid3DAlgorithm = GUID_NULL;

            hr = DS->CreateSoundBuffer(&desc, &PB, 0);
            if (FAILED(hr)) throw std::runtime_error(
                "CreateSoundBuffer failed");
        }
        ~Device ()
        {
            if (B1) B1->Release();
            if (B2) B2->Release();
            if (PB) PB->Release();
            if (DS) DS->Release();
        }

        void load(array<byte> const& input, int channels, int samplerate, int bps)
        {
            if (B1) B1->Release(), B1 = nullptr;

            int align = channels * bps / 8;

            size = input.size();

            duration = (double) size / (align*samplerate);

            if (input.empty()) return;

            WAVEFORMATEX wfmt;
            ZeroMemory (&wfmt, sizeof(wfmt));
            wfmt.wFormatTag      = WAVE_FORMAT_PCM;
            wfmt.nChannels       = channels;
            wfmt.nSamplesPerSec  = samplerate;
            wfmt.nAvgBytesPerSec = samplerate * align;
            wfmt.nBlockAlign     = align;
            wfmt.wBitsPerSample  = bps;

            DSBUFFERDESC desc;
            ZeroMemory (&desc, sizeof(desc));
            desc.dwSize          = sizeof(desc);
            desc.guid3DAlgorithm = GUID_NULL;
            desc.dwFlags         = DSBCAPS_CTRLVOLUME;
            desc.dwBufferBytes   = input.size();
            desc.lpwfxFormat     = & wfmt;

            HRESULT hr;
            hr = DS->CreateSoundBuffer(&desc, &B1, 0);
            if (FAILED(hr)) throw std::runtime_error(
                "CreateSoundBuffer failed");

            byte* p1; DWORD s1;
            byte* p2; DWORD s2;

            B1->Lock(0, 0,
                (void**) &p1, &s1,
                (void**) &p2, &s2,
                DSBLOCK_ENTIREBUFFER);

            memcpy (p1, input.data(), s1);

            B1->Unlock(p1, s1, p2, s2);
            B1->SetCurrentPosition(0);
        }
        void play()
        {
            if (B1) B1->Play(0,0,0);
        }
        void stop()
        {
            if (B1) B1->Stop();
        }
        bool playing()
        {
            if (!B1) return false;
            DWORD status; B1->GetStatus(&status);
            return status & DSBSTATUS_PLAYING;
        }
        bool finished()
        {
            if (!B1) return true;
            DWORD dwCurrentPlayCursor;
            B1->GetCurrentPosition(&dwCurrentPlayCursor, nullptr);
            return size <= dwCurrentPlayCursor;
        }
        void volume(double x)
        {
            if (!B1) return;
            B1->SetVolume(
            DSBVOLUME_MIN + (LONG)((
            DSBVOLUME_MAX -
            DSBVOLUME_MIN ) *
            x));
        }
        auto volume() -> double
        {
            if (!B1) return 0.0;
            LONG volume;
            B1->GetVolume(&volume);
            return (double)(volume -
            DSBVOLUME_MIN) / (
            DSBVOLUME_MAX -
            DSBVOLUME_MIN );
        }
        void position(double sec)
        {
            if (!B1) return;
            B1->SetCurrentPosition(max(0, min((DWORD)(
            size*sec/duration),
            size-1)));
        }
        auto position() -> double
        {
            if (!B1) return 0.0;
            DWORD dwCurrentPlayCursor;
            DWORD dwCurrentWriteCursor;
            B1->GetCurrentPosition(
            &dwCurrentPlayCursor,
            &dwCurrentWriteCursor);
            return duration*
            dwCurrentPlayCursor/
            size;
        }
    };

    struct DATA
    {
        std::unique_ptr<Device> device;

        array<byte> input;
        array<byte> tempo;
        double old_speed = 1.0;
        double new_speed = 1.0;
        double duration = 0.0;
        double volume_ = 1.0;
        int channels = 1;
        int samplerate = 44100;
        int bps = 16;

        void speed(double x) { new_speed = x; speedup(); }
        auto speed() -> double { return new_speed; }
        void speedup()
        {
            double dur = duration;
            
            if (not playing()
            or  not temp())
                return;

            stop();
            double pos = position();
            pos /= dur;
            pos *= duration;

            preset();
            position(pos);
            play();
        }
        bool temp()
        {
            if (abs(new_speed - old_speed) < 0.1
            or  duration - position() < 2.0)
                return false;

            if (abs(new_speed - 1.0) < 0.1 or duration < 2.0)
            {
                tempo = input;
                old_speed = 1.0;
            }
            else
            {
                tempo = speeded(input, channels, samplerate, bps, new_speed);
                old_speed = new_speed;
            }
            int align = channels * bps / 8;
            duration = (double) tempo.size() / (align*samplerate);
            return true;
        }
        void load()
        {
            int align = channels * bps / 8;
            duration = (double) input.size() / (align*samplerate);
            tempo = input;
            old_speed = 1.0;
        }
        void preset()
        {
            if (!device) device = std::make_unique<Device>();

            device->load(tempo, channels, samplerate, bps);
        }
        void play()
        {
            if (temp() or !device) preset();
            if (device) device->volume(volume_), volume_ = device->volume();
            if (device) device->play();
        }
        void stop()
        {
            if (device) device->stop();
        }
        bool playing()
        {
            return device and device->playing();
        }
        bool finished()
        {
            return device and device->finished();
        }
        void volume(double x)
        {
            volume_ = x;
            if (playing()) device->volume(volume_), volume_ = device->volume();
        }
        auto volume() -> double
        {
            return volume_;
        }
        void position(double sec)
        {
            if (device) device->position(sec);
        }
        auto position() -> double
        {
            return device? device->position() : 0.0;
        }
    };

    #define data ((DATA*)(data_))

    player:: player() {}
    player::~player() { delete data; }

    void player::load(array<byte> input, int channels, int samplerate, int bps)
    {
        if (!data_) data_ = new DATA;
        data->input = std::move(input);
        data->channels = channels;
        data->samplerate = samplerate;
        data->bps = bps;
        data->load();
    }
    void player::play()
    {
        if (data) data->play();
    }
    void player::stop()
    {
        if (data) data->stop();
    }
    bool player::playing()
    {
        return data and data->playing();
    }
    bool player::finished()
    {
        return data and data->finished();
    }
    void player::volume(double x)
    {
        if (!data_) data_ = new DATA; data->volume(x);
    }
    auto player::volume() -> double
    {
        return data ? data->volume() : 0.5;
    }
    void player::position (double sec)
    {
        if (!data_) data_ = new DATA; data->position(sec);
    }
    auto player::position () -> double
    {
        return data ? data->position() : 0.0;
    }
    void player::speed(double x)
    {
        if (!data_) data_ = new DATA; data->speed(x);
    }
    auto player::speed() -> double
    {
        return data ? data->speed() : 1.0;
    }
    auto player::duration () -> double
    {
        return data ? data->duration : 0.0;
    }

    #undef data
}
