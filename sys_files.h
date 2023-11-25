#pragma once
#include <fstream>
#include <filesystem>
#include "sys_aux.h"
namespace sys
{
    using sys::byte;
    using std::filesystem::path;
    using std::filesystem::exists;
    using std::filesystem::create_directories;

    auto dirs (path dir) -> generator<path>
    {
        if (exists(dir))
        for (std::filesystem::directory_iterator
        next(dir), end; next != end; ++next)
        if (is_directory(next->path()))
        co_yield next->path();
    }
    auto files (path dir) -> generator<path>
    {
        if (exists(dir))
        for (std::filesystem::directory_iterator
        next(dir), end; next != end; ++next)
        if (is_regular_file(next->path()))
        co_yield next->path();
    }
    auto paths (path dir) -> generator<path>
    {
        if (exists(dir))
        for (std::filesystem::directory_iterator
        next(dir), end; next != end; ++next)
        co_yield next->path();
    }

    auto bytes (path path)
    {
        std::ifstream
        ifstream(path, std::ios::binary);
        ifstream.seekg(0, std::ios::end);
        int size = (int)ifstream.tellg();
        ifstream.seekg(0, std::ios::beg);

        if (ifstream.fail())
        throw std::runtime_error(
        "couldn't read " + str(path));

        array<byte> pool;
        pool.resize(size);
        ifstream.read((char*)
        (pool.data()), size);
        return pool;
    }

    auto bytes (path path, int offset, int size)
    {
        std::ifstream
        ifstream(path, std::ios::binary);
        ifstream.seekg(offset, std::ios::beg);

        if (ifstream.fail())
        throw std::runtime_error(
        "couldn't read " + str(path));

        array<byte> pool;
        pool.resize(size);
        ifstream.read((char*)
        (pool.data()), size);
        return pool;
    }

    auto text (path path)
    {
        std::ifstream stream(path); str text = std::string{
        std::istreambuf_iterator<char>(stream),
        std::istreambuf_iterator<char>()};

        if (text.starts_with(
        "\xEF""\xBB""\xBF")) // UTF-8 BOM
        text.upto(3).erase();
        return text;
    }

    auto text_lines (path path)
    {
        array<str> ss = text(path).lines();
        return ss;
    }

    auto optional_text (path path)
    {
        return exists(path) ?
        text(path) : "";
    }

    auto optional_text_lines (path path)
    {
        return exists(path) ?
            text_lines(path) :
            array<str>{};
    }

    void write (path path, array<byte> const& data)
    {
        create_directories(path.parent_path());
        std::ofstream ofstream(path, std::ios::binary);
        ofstream.write((char*)(
        data.data()),
        data.size());
    }

    void write (path path, array<str> const& text)
    {
        create_directories(path.parent_path());
        std::ofstream ofstream(path);
        for (str line: text) {
        ofstream << line;
        ofstream << "\n"; }
    }
}