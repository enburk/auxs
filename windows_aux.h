#pragma once
#include <memory>
#include <string>
#include <system_error>
#include <windows.h>
#include <windowsx.h>

std::wstring sys::acpstr(std::string s)
{
    std::wstring ss;
    auto p = s.data();
    auto l = (int)(s.size());
    if (l <= 0) return ss; int n =
    ::MultiByteToWideChar(CP_ACP, 0, p, l, nullptr, 0); if (n <= 0) return ss; ss.resize(n);
    ::MultiByteToWideChar(CP_ACP, 0, p, l, ss.data(), n);
    return ss;
}
std::wstring sys::winstr(std::string s)
{
    std::wstring ss;
    auto p = s.data();
    auto l = (int)(s.size());
    if (l <= 0) return ss; int n =
    ::MultiByteToWideChar(CP_UTF8, 0, p, l, nullptr, 0); if (n <= 0) return ss; ss.resize(n);
    ::MultiByteToWideChar(CP_UTF8, 0, p, l, ss.data(), n);
    return ss;
}
std::string sys::unwinstr(std::wstring ss)
{
    std::string s;
    auto p = ss.data();
    auto l = (int)(ss.size());
    if (l <= 0) return s; int n =
    ::WideCharToMultiByte(CP_UTF8, 0, p, l, nullptr, 0, NULL, NULL); if (n <= 0) return s; s.resize(n);
    ::WideCharToMultiByte(CP_UTF8, 0, p, l, s.data(), n, NULL, NULL);
    return s;
}

using sys::acpstr;
using sys::winstr;
using sys::unwinstr;

std::string GetErrorMessage(DWORD dwErrorCode)
{
    LPWSTR s = nullptr;
    DWORD n = FormatMessageW(
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS |
        FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, dwErrorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPWSTR>(&s), 0, NULL);
    if (n > 0) {
        auto deleter = [](void* p) { ::HeapFree(::GetProcessHeap(), 0, p); };
        std::unique_ptr<WCHAR, decltype(deleter)> buffer(s, deleter);
        return unwinstr(std::wstring(buffer.get(), n));
    }
    else throw std::system_error(::GetLastError(), std::system_category(),
        "Failed to retrieve error message string.");
}

namespace aux::unicode
{
    str what (str s) { return unwinstr(acpstr(s)); }
}


