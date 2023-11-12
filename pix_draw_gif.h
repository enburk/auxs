#pragma once
#include "pix_color.h"
#include "pix_image.h"
namespace pix::gif
{
    using byte = uint8_t;
    using word = uint16_t;

    #pragma pack(push, 1)

    struct header
    {
        char chars[6] = { 'G','I','F','8','9','a' };
        str string () { return str(chars, chars+6); }
    };
    
    struct lsd // logical screen descriptor
    {
        word w = 0; // canvas width
        word h = 0; // canvas height
        byte bits = 0; // packed field
        byte background_color_index = 0;
        byte pixel_aspect_ratio = 0;

        byte gct_size () { return (bits >> 0) & 0b111; } // global color table size
        byte gct_sort () { return (bits >> 3) & 0b001; } // global color table sorted
        byte colorres () { return (bits >> 4) & 0b111; } // color_resolution
        byte gct_flag () { return (bits >> 7) & 0b001; } // global color table present
        void normalize ()
        {
            if (std::endian::native !=
                std::endian::little)
                w = std::byteswap(w),
                h = std::byteswap(h);
        }
    };

    struct gce // graphics control extension
    {
        byte size = 4; // block size;
        byte bits = 0; // packed field
        word delay = 0; // delay time
        byte transparent_color = 0; // transparent color index
        byte zero = 0; // terminator

        unsigned char transparent () { return (bits >> 0) & 0b001; } // transparent color flag
        unsigned char user_input  () { return (bits >> 1) & 0b001; } // user input flag
        unsigned char disposal    () { return (bits >> 2) & 0b111; } // disposal method
        unsigned char reserved    () { return (bits >> 5) & 0b111; } // reserved for future use
        void normalize ()
        {
            if (std::endian::native !=
                std::endian::little)
                delay = std::byteswap(
                delay);
        }
    };

    struct descriptor // image decriptor
    {
        word x = 0; // image left
        word y = 0; // image right
        word w = 0; // image width
        word h = 0; // image height
        byte bits = 0; // packed field

        unsigned char lct_size  () { return (bits >> 0) & 0b111; } // local color table size
        unsigned char reserved  () { return (bits >> 3) & 0b011; } // reserved for future use
        unsigned char sorted    () { return (bits >> 5) & 0b001; } // sort flag
        unsigned char interlace () { return (bits >> 6) & 0b001; } // interlace flag
        unsigned char lct_flag  () { return (bits >> 7) & 0b001; } // local color table flag
        void normalize ()
        {
            if (std::endian::native !=
                std::endian::little)
                x = std::byteswap(x),
                y = std::byteswap(y),
                w = std::byteswap(w),
                h = std::byteswap(h);
        }
    };

    #pragma pack(pop)

    void error (str s) { throw std::runtime_error(s); }

    struct Image
    {
        gce gce;
        array<rgba> lct;
        descriptor descriptor;
        const byte* source = nullptr;
        pix::image<rgba> image;
        byte lzw_min_size = 0;

        void unpack (const array<rgba>& gct)
        {
            int npix = 0;
            {
                xy size {
                descriptor.w,
                descriptor.h};

                if (image.size != size)
                    image.resize(size);
                    else return;

                npix = size.x * size.y;
            }

            if (lct.empty()) lct = gct;
            if (gce.transparent()) lct[
                gce.transparent_color] =
                rgba{};

            const int MAX_SIZE = 4096;
            word prefix[MAX_SIZE];
            byte suffix[MAX_SIZE];
            byte colors[MAX_SIZE+1];

            int null_code = -1;
            int clear = 1 << lzw_min_size;
            int end_of_information = clear + 1;
            int available = clear + 2;
            int old_code = null_code;
            int code_size = lzw_min_size + 1;
            int code_mask = (1 << code_size) - 1;
            for (int code = 0; code < clear; code++)
                prefix[code] = 0,
                suffix[code] =
                (byte) code;

            int datum = 0, bits = 0, count = 0, first = 0, top = 0;

            auto src = source;
            auto dst = image.data.data();

            for (int i = 0; i < npix; ) {
                if (top == 0) {
                    if (bits < code_size) {
                        // Load bytes until there are enough bits for a code.
                        if (count == 0) {
                            // Read a new data block.
                            count = *src++;
                            if (count <= 0) {
                                break;
                            }
                        }
                        datum += (((int) *src++) & 0xff) << bits;
                        bits += 8;
                        count--;
                        continue;
                    }
                    // Get the next code.
                    int code = datum & code_mask;
                    datum >>= code_size;
                    bits -= code_size;
                    // Interpret the code
                    if ((code > available) || (code == end_of_information)) {
                        break;
                    }
                    if (code == clear) {
                        // Reset decoder.
                        code_size = lzw_min_size + 1;
                        code_mask = (1 << code_size) - 1;
                        available = clear + 2;
                        old_code = null_code;
                        continue;
                    }
                    if (old_code == null_code) {
                        colors[top++] = suffix[code];
                        old_code = code;
                        first = code;
                        continue;
                    }
                    int in_code = code;
                    if (code == available) {
                        colors[top++] = (byte) first;
                        code = old_code;
                    }
                    while (code > clear) {
                        colors[top++] = suffix[code];
                        code = prefix[code];
                    }
                    first = ((int) suffix[code]) & 0xff;
                    // Add a new string to the string table,
                    if (available >= MAX_SIZE) {
                        break;
                    }
                    colors[top++] = (byte) first;
                    prefix[available] = (short) old_code;
                    suffix[available] = (byte) first;
                    available++;
                    if (((available & code_mask) == 0) && (available < MAX_SIZE)) {
                        code_size++;
                        code_mask += available;
                    }
                    old_code = in_code;
                }
                // Pop a pixel off the pixel stack.
                top--;
                *dst++ = lct[colors[top]];
                i++;
            }
        }
    };

    struct decoder
    {
        array<byte> source;
        array<Image> Images;
        pix::image<rgba> image;
        int duration = 0;
        int current = 0;
        int loops = 1;
        bool last = false;
        array<rgba> gct;
        lsd lsd;

        bool next (std::atomic<bool>& cancel)
        {
            if (cancel) return false;
            if (Images.size() == 0) return false;
            if (Images.size() == current and loops == 1) return false;
            if (Images.size() == current) last = true;
            if (Images.size() == current) loops--;
            if (Images.size() == current) current = 0;

            aux::timing t0;
            auto& img = Images[current++];
            img.unpack(gct);

            if (img.gce.disposal() == 2
            and lsd.background_color_index < gct.size()) image.fill(
            gct[lsd.background_color_index]);

            image.crop().from({img.descriptor.x,img.descriptor.y}).
            blend_from(img.image.crop());
            aux::timing t1;

            using namespace std::chrono;

            high_resolution_clock::duration delay =
            milliseconds(10 * img.gce.delay);
            delay -= t1-t0;

            auto ms = duration_cast<milliseconds>(delay);
            if (ms > 1ms) std::this_thread::sleep_for(ms);
            if (cancel) return false;
            return true;
        }

        decoder (array<byte> src) : source(std::move(src))
        {
            auto data = source.whole();

            auto read = [&data](auto& x)
            {
                if (data.size() < sizeof(x))
                error("gif: unexpected end");
                std::memcpy(&x, data.data(), sizeof(x));
                data >>= sizeof(x);
            };
            auto read_color_table = [&data](int nn)
            {
                nn = 1 << (nn+1);
                array<rgba> table;
                table.reserve(nn);
                auto* p = data.data();
                if (data.size() < 3*nn)
                error("gif: unexpected end");
                data >>= 3*nn;
                while (nn--) {
                    byte r = *p++;
                    byte g = *p++;
                    byte b = *p++;
                    table += rgba(r,g,b);
                }
                return table;
            };
            auto skip_block = [&data, read]()
            {
                while (true) {
                    byte nn; read(nn);
                    if (nn == 0) return;
                    if (data.size() < nn)
                    error("gif: unexpected end");
                    data >>= nn;
                }
            };

            header header; read(header);
            if (header.string() != "GIF87a"
            and header.string() != "GIF89a")
                error("not a gif");

            read(lsd);
            lsd.normalize();
            if (lsd.gct_flag())
                gct = read_color_table(
                lsd.gct_size());

            image.resize({lsd.w, lsd.h});
            if (lsd.background_color_index < gct.size()) image.fill(
            gct[lsd.background_color_index]);

            gce gce;

            while (true)
            {
                byte code; read(code);
                if (code == 0x3B) return; // trailer
                if (code == 0x21) // extension
                {
                    byte label; read(label);
                    if (label == 0xF9) { // graphics control extension
                        read(gce);
                        gce.normalize();
                    }
                    else
                    if (label == 0xFF // application extension
                    and data.size() >= 17
                    and data[ 0] == 0x0B
                    and data[ 1] == 0x4E // N
                    and data[ 2] == 0x45 // E
                    and data[ 3] == 0x54 // T
                    and data[ 4] == 0x53 // S
                    and data[ 5] == 0x43 // C
                    and data[ 6] == 0x41 // A
                    and data[ 7] == 0x50 // P
                    and data[ 8] == 0x45 // E
                    and data[ 9] == 0x32 // 2
                    and data[10] == 0x2E // .
                    and data[11] == 0x30 // 0
                    and data[12] == 0x03
                    and data[13] == 0x01)
                    {
                        loops = (data[14] << 8) + data[15];
                        skip_block();
                    }
                    else skip_block();
                    continue;
                }

                if (code != 0x2C) // image descriptor
                error("gif: expected image descriptor");

                Image Image;
                Image.gce = gce;
                duration += gce.delay*10;
                read(Image.descriptor);
                Image.descriptor.normalize();
                if (Image.descriptor.lct_flag())
                    Image.lct = read_color_table(
                    Image.descriptor.lct_size());

                read(Image.lzw_min_size);
                Image.source = data.data();
                skip_block();

                Images += Image;
            }

            duration = loops == 0 ? max<int>() :
            duration * loops;
        }
    };
}
// https://giflib.sourceforge.net/whatsinagif/bits_and_bytes.html
// https://www.w3.org/Graphics/GIF/spec-gif89a.txt
