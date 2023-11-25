#pragma once
#include "sys_files.h"
namespace sys::out
{
    struct pool
    {
        array<byte> bytes;

        pool& operator << (int32_t n)
        {
            int size = sizeof(n);
            byte* data = (byte*)(&n);
            bytes.container::insert(
            bytes.end(), data, data+size);
            return *this;
        }
        pool& operator << (uint32_t n)
        {
            int size = sizeof(n);
            byte* data = (byte*)(&n);
            bytes.container::insert(
            bytes.end(), data, data+size);
            return *this;
        }
        pool& operator << (str const& s)
        {
            *this << s.size();
            int size = s.size();
            byte* data = (byte*)(s.c_str()); 
            bytes.container::insert(
            bytes.end(), data, data+size);
            return *this;
        }
        template<class T>
        pool& operator << (array<T> const& a)
        {
            *this << a.size();
            for (auto const& t: a)
            *this << t;
            return *this;
        }
        pool& operator << (array<byte> const& a)
        {
            *this << a.size();
            bytes += a;
            return *this;
        }
    };

    struct file
    {
        pool pool;
        path path;
        std::ofstream fstream;
        int size = 0;

        file (std::filesystem::path path,
            std::ios_base::openmode mode = std::ios::binary)
            : path(path)
        {
            auto dir = path.parent_path();
            if  (dir != std::filesystem::path{})
                create_directories(dir);

            fstream = std::ofstream(path, mode);

            if ((mode & std::ios::app) != 0 and
                std::filesystem::exists(path))
                size = (int)std::filesystem::
                    directory_entry(path)
                        .file_size();
        }

        ~file () { close(); }

        void close ()
        {
            fstream.write((char*)
                pool.bytes.data(),
                pool.bytes.size());
            fstream.close();
        }

        void operator = (file&& f)
        {
            std::swap(pool, f.pool);
            std::swap(path, f.path);
            std::swap(size, f.size);
            std::swap(fstream, f.fstream);
        }

        template<class entity> 
        file& operator << (entity const& e)
        {
            auto
            size_ = pool.bytes.size();  pool << e;
            size += pool.bytes.size() - size_;
            if (pool.bytes.size() >= 8*1024*1024) {
                fstream.write((char*)
                pool.bytes.data(),
                pool.bytes.size());
                pool.bytes.clear();
            }
            return *this;
        }
    };
}
