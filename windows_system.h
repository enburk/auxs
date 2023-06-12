#pragma once
#include <map>
#include "sys_aux.h"
#include "windows_aux.h"
#include <shlwapi.h>
#include <shlobj.h>
#pragma comment(lib,"shlwapi.lib")

sys::process::process (std::filesystem::path path, str args, options opt)
{
    PROCESS_INFORMATION pi; 
    HANDLE out = 0; if (opt.out != std::filesystem::path{})
    {
        SECURITY_ATTRIBUTES sa;
        sa.nLength = sizeof(sa);
        sa.lpSecurityDescriptor = NULL;
        sa.bInheritHandle = TRUE;       

        out = CreateFileW(
            winstr(opt.out.string()).c_str(),
            FILE_APPEND_DATA,
            FILE_SHARE_WRITE | FILE_SHARE_READ,
            &sa,
            OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL);
    }

    DWORD flags = 0; if (opt.hidden) flags = CREATE_NO_WINDOW;

    if (opt.ascii)
    {
        STARTUPINFOA si;
        ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
        ZeroMemory(&si, sizeof(STARTUPINFOA));
        si.cb = sizeof(STARTUPINFOA); 

        if (opt.out != std::filesystem::path{}) {
            si.dwFlags |= STARTF_USESTDHANDLES;
            si.hStdInput = NULL;
            si.hStdError = out;
            si.hStdOutput = out;
        }

        std::string buffer = " " + args;

        BOOL rc = CreateProcessA(
            path.string().c_str(),
            buffer.data(),
            NULL, NULL, TRUE, flags,
            NULL, NULL, &si, &pi);

        if (!rc) throw std::runtime_error(
            GetErrorMessage(::GetLastError()));
    }
    else
    {
        STARTUPINFOW si;
        ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
        ZeroMemory(&si, sizeof(STARTUPINFO));
        si.cb = sizeof(STARTUPINFO); 

        if (opt.out != std::filesystem::path{}) {
            si.dwFlags |= STARTF_USESTDHANDLES;
            si.hStdInput = NULL;
            si.hStdError = out;
            si.hStdOutput = out;
        }

        auto ws = winstr(" " + args);
        array<WCHAR> buffer; buffer.resize((int)(ws.size()));
        std::copy(ws.begin(), ws.end(), buffer.begin());

        BOOL rc = CreateProcessW(
            winstr(path.string()).c_str(),
            buffer.data(),
            NULL, NULL, TRUE, flags,
            NULL, NULL, &si, &pi);

        if (!rc) throw std::runtime_error(
            GetErrorMessage(::GetLastError()));
    }

    handle = (size_t)(pi.hProcess);
    if (out != 0) CloseHandle(out);
    ::CloseHandle(pi.hThread);

    if (opt.ms_wait_for_input_idle > 0) ::WaitForInputIdle(pi.hProcess, 
        opt.ms_wait_for_input_idle);
}
sys::process::process (std::filesystem::path path, str args)
    : process(path, args, options{})
        {}
bool sys::process::wait (int ms)
{
    if (handle == 0) return false;
    DWORD time = ms == max<int>() ? INFINITE : ms;
    auto rc = ::WaitForSingleObject((HANDLE)(handle), time);
    return rc == WAIT_OBJECT_0 || rc == WAIT_ABANDONED_0;
}
sys::process::~process ()
{
    if (handle != 0) ::CloseHandle((HANDLE)(handle));
}

///////////////////////////////////////////////////////////////////////////////

void sys::directory_watcher::watch ()
{
    thread = std::thread([this]()
    {
        std::array<std::byte, 64*1024> buffer; DWORD bytes_written = 0;

        auto handle = ::CreateFile(dir.string().c_str(), FILE_LIST_DIRECTORY,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
            OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

        if (handle == INVALID_HANDLE_VALUE) { error(GetErrorMessage(::GetLastError())); return; }

        while (!stop)
        {
            auto return_code = ::ReadDirectoryChangesW
            (
                handle, buffer.data(),
                (DWORD)(buffer.size()),
                TRUE, // recursive
                FILE_NOTIFY_CHANGE_DIR_NAME |
                FILE_NOTIFY_CHANGE_FILE_NAME |
                FILE_NOTIFY_CHANGE_LAST_WRITE,
                &bytes_written,
                NULL, // synchronous calls
                NULL // completion routine
            );

            if (return_code == 0) { error(GetErrorMessage(::GetLastError())); return; }
            if (bytes_written == 0) { error("buffer too big or too small"); return; }

            size_t buffer_offset = 0;

            while (!stop)
            {
                if (buffer_offset == bytes_written) break;
                if (buffer_offset >= bytes_written) {
                    error("offset out of range");
                    return;
                }

                auto info = (PFILE_NOTIFY_INFORMATION)(buffer.data() + buffer_offset);

                if (info->NextEntryOffset > 0) buffer_offset +=
                    info->NextEntryOffset; else buffer_offset =
                    bytes_written;

                str what = "unknown";

                switch (info->Action) {
                case FILE_ACTION_ADDED:            what = "added"; break;
                case FILE_ACTION_RENAMED_OLD_NAME: what = "renamed (was)"; break;
                case FILE_ACTION_RENAMED_NEW_NAME: what = "renamed (now)"; break;
                case FILE_ACTION_REMOVED:          what = "removed"; break;
                case FILE_ACTION_MODIFIED:         what = "modified"; break;
                }

                std::basic_string<WCHAR> filename(info->FileName, info->FileNameLength/2);

                action(dir/filename, what);
            }
        }
    });
};
void sys::directory_watcher::cancel ()
{
    stop = true;
    if (thread.joinable()) ::CancelSynchronousIo(
        thread.native_handle()),
        thread.join();
    stop = false;
}

///////////////////////////////////////////////////////////////////////////////

struct Clipboard
{
    Clipboard (const Clipboard&) = delete;
    Clipboard& operator = (const Clipboard&) = delete;
    Clipboard () { if (!::OpenClipboard(nullptr)) throw std::runtime_error("Can't open clipboard."); }
   ~Clipboard () { ::CloseClipboard(); }
};
struct ClipboardText
{
    ClipboardText (const ClipboardText&) = delete;
    ClipboardText& operator = (const ClipboardText&) = delete;
    explicit ClipboardText(HANDLE handle) : handle(handle) {
    text = static_cast<const WCHAR*>(::GlobalLock(handle));
    if (!text) throw std::runtime_error("Can't acquire lock on clipboard text."); }
   ~ClipboardText () { ::GlobalUnlock(handle); }
    HANDLE handle; const WCHAR * text;
};

str sys::clipboard::get::string ()
{
    try
    {
        Clipboard clipboard;
        HANDLE h = GetClipboardData(CF_UNICODETEXT);
        if (!h) throw std::runtime_error("Can't get clipboard text.");
        str s = unwinstr(std::wstring(ClipboardText(h).text));
        s.erase_if([](auto c){ return c == '\r'; });
        return s;
    }
    catch(...){}
    return "";
}
void sys::clipboard::set (str s)
{
    if (s == "") return;
    try
    {
        Clipboard clipboard;
        std::wstring text = winstr(s);
        HGLOBAL handle = GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE, (text.size()+1) * sizeof(WCHAR)); if (!handle) return;
        LPWSTR buffer = (LPWSTR)::GlobalLock(handle); if (!buffer) return;
        memcpy(buffer, text.data(), text.size() * sizeof(WCHAR));
        buffer[text.size()] = 0;
        ::GlobalUnlock(handle);
        ::EmptyClipboard();
        ::SetClipboardData(CF_UNICODETEXT, buffer);
    }
    catch(...){}
}
pix::image<rgba> sys::clipboard::get::image ()
{
    return pix::image<rgba>();
}
void sys::clipboard::set (pix::frame<rgba> image)
{
}

///////////////////////////////////////////////////////////////////////////////

namespace sys::settings
{
    std::map<str, int> ints, Ints;
    std::map<str, str> strs, Strs;
    using namespace std::literals::chrono_literals;
    auto was = std::chrono::high_resolution_clock::now();
    auto delay = 5*6s;
    std::filesystem::path path;
    std::mutex mutex;

    static void init (str appname)
    {
        appname.replace_all(":", "..");

        TCHAR szPath[MAX_PATH];
        if (FAILED(SHGetFolderPath(NULL,
            CSIDL_COMMON_APPDATA, NULL, 0, szPath)))
            return;

        path = szPath;
        path = path / appname.c_str() / "settings.txt";
        std::ifstream stream(path); str text = std::string{(
        std::istreambuf_iterator<char>(stream)),
        std::istreambuf_iterator<char>()};

        for (str s: text.split_by("\n"))
        {
            str type; s.split_by(":", type, s);
            str key;  s.split_by("=", key,  s);
            if (type == "int") ints[key] = std::stoi(s); else
            if (type == "str") strs[key] = s;
        }
        Ints = ints;
        Strs = strs;
    }
    static void save ()
    {
        auto now = std::chrono::high_resolution_clock::now();
        if (now < was + delay) return; was = now;
        if (ints == Ints
        and strs == Strs)
            return;

        str text;
        for (auto const& [key, val]: ints) text += "int:"+key+"="+std::to_string(val)+"\n";
        for (auto const& [key, val]: strs) text += "str:"+key+"="+val+"\n";
        std::filesystem::create_directories(path.parent_path());
        std::ofstream(path) << text;
        Ints = ints;
        Strs = strs;
    }
    static void done ()
    {
        delay = -999s; save();
    }
    str load (str name, str default_value) {
        std::lock_guard guard{mutex};
        auto [it, ins] = strs.try_emplace(name, default_value);
        return it->second;
    }
    int load (str name, int default_value) {
        std::lock_guard guard{mutex};
        auto [it, ins] = ints.try_emplace(name, default_value);
        return it->second;
    }
    void save (str name, str value) {
        std::lock_guard guard{mutex};
        strs[name] = value;
        save();
    }
    void save (str name, int value) {
        std::lock_guard guard{mutex};
        ints[name] = value;
        save();
    }
}

///////////////////////////////////////////////////////////////////////////////

extern HWND MainWindowHwnd;

#include <dsound.h>
#pragma comment(lib, "dsound.lib")
namespace sys::audio
{
    struct DATA
    {
        LPDIRECTSOUND8      DS = nullptr;
        LPDIRECTSOUNDBUFFER PB = nullptr; // primary buffer
        LPDIRECTSOUNDBUFFER B1 = nullptr; // secondary buffer
        LPDIRECTSOUNDBUFFER B2 = nullptr; // secondary buffer

        int bytes = 0;

        DATA ()
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
        ~DATA ()
        {
            if (B1) B1->Release();
            if (B2) B2->Release();
            if (PB) PB->Release();
            if (DS) DS->Release();
        }
    };

    player:: player() {}
    player::~player()
    {
        if (data_) delete (DATA*)(data_);
    }

    void player::load(array<byte> input, int channels, int samples, int bps)
    {
        if (!data_) data_ = new DATA;
        DATA & data = *(DATA*)(data_);
        data.bytes = input.size();

        int align = channels * bps / 8;
        duration = (double) input.size() / (align*samples);

        if (data.B1) { data.B1->Release(); data.B1 = nullptr; }

        if (input.empty()) return;

        WAVEFORMATEX wfmt;
        ZeroMemory (&wfmt, sizeof(wfmt));
        wfmt.wFormatTag      = WAVE_FORMAT_PCM;
        wfmt.nChannels       = channels;
        wfmt.nSamplesPerSec  = samples;
        wfmt.nAvgBytesPerSec = samples * align;
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
        hr = data.DS->CreateSoundBuffer(&desc, &data.B1, 0);
        if (FAILED(hr)) throw std::runtime_error(
            "CreateSoundBuffer failed");

        byte* p1; DWORD s1;
        byte* p2; DWORD s2;

        data.B1->Lock(0, 0,
            (void**) &p1, &s1,
            (void**) &p2, &s2,
            DSBLOCK_ENTIREBUFFER);

        memcpy (p1, input.data(), s1);

        data.B1->Unlock(p1, s1, p2, s2);
        data.B1->SetCurrentPosition(0);
    }
    void player::play()
    {
        DATA& data = *(DATA*)(data_);
        if (data_ and
            data.B1)
            data.B1->Play(0,0,0);
    }
    void player::stop()
    {
        DATA& data = *(DATA*)(data_);
        if (data_ and
            data.B1)
            data.B1->Stop();
    }
    bool player::playing()
    {
        DATA& data = *(DATA*)(data_);
        if (!data_
        or  !data.B1)
            return false;
            DWORD status;
            data.B1->GetStatus(&status);
            return status &
            DSBSTATUS_PLAYING;
    }
    bool player::finished()
    {
        DATA& data = *(DATA*)(data_);
        if (!data_
        or  !data.B1
        or  !data.bytes)
            return true;
        DWORD dwCurrentPlayCursor;
        data.B1->GetCurrentPosition(
        &dwCurrentPlayCursor, nullptr);
        return (DWORD)data.bytes <=
        dwCurrentPlayCursor;
    }
    void player::volume(double O_1)
    {
        HRESULT hr;
        DATA& data = *(DATA*)(data_);
        if (data_ and
            data.B1) hr =
            data.B1->SetVolume(
            DSBVOLUME_MIN + (LONG)((
            DSBVOLUME_MAX -
            DSBVOLUME_MIN ) *
            O_1));
    }
    auto player::volume() -> double
    {
        DATA& data = *(DATA*)(data_);
        if (!data_
        or  !data.B1)
            return 0.0;
            LONG volume;
            data.B1->GetVolume(&volume);
            return (double)(volume -
            DSBVOLUME_MIN) / (
            DSBVOLUME_MAX -
            DSBVOLUME_MIN );
    }
    void player::position (double sec)
    {
        DATA& data = *(DATA*)(data_);
        if (data_ and
            data.B1)
            data.B1->SetCurrentPosition(
            max(0, min(data.bytes-1, (int)
            (data.bytes*sec/duration))));
    }
    auto player::position () -> double
    {
        DATA& data = *(DATA*)(data_);
        if (!data_
        or  !data.B1
        or  !data.bytes)
            return 0.0;
        DWORD dwCurrentPlayCursor;
        DWORD dwCurrentWriteCursor;
        data.B1->GetCurrentPosition(
        &dwCurrentPlayCursor,
        &dwCurrentWriteCursor);
        return duration*
        dwCurrentPlayCursor/
            data.bytes;
    }
}

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
        if (rc < 0) throw std::runtime_error(
            "Input does not appear to be "
            "an Ogg bitstream.");

        vorbis_info *vi=ov_info(&vf,-1);
        channels = vi->channels;
        samples = vi->rate;
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
#endif
