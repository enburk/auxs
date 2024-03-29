#pragma once
#include <chrono>
#include <sstream>
namespace aux
{
    struct timing
    {
        decltype (std::chrono::high_resolution_clock::now())
        stamp  =  std::chrono::high_resolution_clock::now();
        friend auto operator - (timing t1, timing t0) noexcept
            { return t1.stamp - t0.stamp; }
    };

    std::ostream& operator << (std::ostream& stream,
    std::chrono::high_resolution_clock::duration d)
    {
        using std::chrono::duration_cast;
        auto ss = duration_cast<std::chrono::seconds     >(d); d -= ss;
        auto ms = duration_cast<std::chrono::milliseconds>(d); d -= ms;
        auto us = duration_cast<std::chrono::microseconds>(d); d -= us;
        auto ns = duration_cast<std::chrono::nanoseconds >(d);

        stream.fill('0');
        stream          << ss.count() << "."
        << std::setw(3) << ms.count() << "'"
        << std::setw(3) << us.count() << "'"
        << std::setw(3) << ns.count(); 

        return stream;
    }
    std::string format_nano (
    std::chrono::high_resolution_clock::duration duration)
    {
        std::stringstream stream;
        stream << duration;
        return stream.str();
    }
    std::string format (
    std::chrono::high_resolution_clock::duration duration)
    {
        std::string s = format_nano(duration);
        s.resize(s.size()-4);
        return s;
    }
    std::string ms (
    std::chrono::high_resolution_clock::duration duration)
    {
        return
        std::to_string(
        std::chrono::duration_cast<
        std::chrono::milliseconds>
        (duration).count());
    }

    str format (str fmt, std::chrono::system_clock::time_point time)
    {
    //  return
    //  std::format(fmt, time);
        std::time_t ctime =
        std::chrono::system_clock::to_time_t(time);
        std::stringstream stringstream;
        stringstream << std::put_time(
        std::gmtime(&ctime), fmt.c_str());
        str s = stringstream.str();
        return s;
    }
}
