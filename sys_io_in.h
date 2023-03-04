#pragma once
#include <fstream>
#include <filesystem>
#include "sys_aux.h"
namespace sys::in
{
    using sys::byte;
    using std::filesystem::path;
    using std::filesystem::exists;
    using aux::expected;

    auto bytes (path path)
    {
        std::ifstream
        ifstream(path, std::ios::binary);
        ifstream.seekg(0, std::ios::end);
        int size = (int)ifstream.tellg();
        ifstream.seekg(0, std::ios::beg);

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

    auto optional_text (path path)
    {
        return exists(path) ?
        text(path) : "";
    }

    inline int32_t endianness = 0;

    struct pool
    {
        str name;
        int offset = 0;
        array<byte> bytes;

        pool () = default;
        pool (path path) {
            name = path.string();
            if (not exists(path)) return;
            bytes = sys::in::bytes(path);
        }

        void get_endianness () {
            endianness = 0; // prevent reversing
            endianness = get_int(); }

        bool done () { return offset >= bytes.size(); }

        auto get_int () -> int 
        {
            if (offset + 4 > bytes.size()) throw
                std::out_of_range("dat::in::pool " +
                name + ": get_int out of range");

            int n = 0;
            if (endianness == 0
            or  endianness == 0x12345678)
            {
                n = (static_cast<uint8_t>(bytes[offset+3]) & 0xFF)       |
                    (static_cast<uint8_t>(bytes[offset+2]) & 0xFF) <<  8 |
                    (static_cast<uint8_t>(bytes[offset+1]) & 0xFF) << 16 |
                    (static_cast<uint8_t>(bytes[offset  ]) & 0xFF) << 24;
            }
            else if (endianness == 0x78563412)
            {
                n = (static_cast<uint8_t>(bytes[offset  ]) & 0xFF)       |
                    (static_cast<uint8_t>(bytes[offset+1]) & 0xFF) <<  8 |
                    (static_cast<uint8_t>(bytes[offset+2]) & 0xFF) << 16 |
                    (static_cast<uint8_t>(bytes[offset+3]) & 0xFF) << 24;
            }
            else throw std::logic_error (
               "unsupported endianness: " +
                std::to_string(endianness));

            offset += 4;
            return n;
        }

        auto get_string () -> std::string_view
        {
            int size = get_int();
            if (size < 0) throw
                std::out_of_range("dat::in::pool " +
                name + ": get_string negative size");

            if (offset + size > bytes.size()) throw
                std::out_of_range("dat::in::pool " +
                name + ": get_string out of range");
        
            std::string_view s {
            (char*)(&bytes[offset]),
            (size_t)(size)};
            offset += size;
            return s;
        }

        struct chunk { byte* data; int size; };

        auto get_bytes () -> chunk
        {
            int size = get_int();
            if (size < 0) throw
                std::out_of_range("dat::in::pool " +
                name + ": get_bytes negative size");

            if (offset + size > bytes.size()) throw
                std::out_of_range("dat::in::pool " +
                name + ": get_bytes out of range");
        
            chunk chunk{&bytes[offset], size};
            offset += size;
            return chunk;
        }

        pool& operator >> (int32_t& n)
        {
            n = get_int();
            return *this;
        }
        pool& operator >> (uint32_t& n)
        {
            n = (uint32_t)get_int();
            return *this;
        }
        pool& operator >> (str& s)
        {
            s = get_string();
            return *this;
        }
        template<class T>
        pool& operator >> (array<T>& a)
        {
            int n = get_int(); a.resize(n);
            for (auto& x: a) *this >> x;
            return *this;
        }
    };

    struct file
    {
        pool pool;
        bool required;

        file (path path, bool required = false) : pool(path), required(required) {}

        template<class entity> 
        file& operator >> (entity& e)
        {
            if (required or
            not pool.bytes.empty())
                pool >> e;

            return *this;
        }
    };
}