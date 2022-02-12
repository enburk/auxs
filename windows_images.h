#include "sys_aux.h"
#include "windows_aux.h"
#include <ocidl.h>
#include <olectl.h>
#include <gdiplus.h>
#pragma comment (lib, "gdiplus.lib")
using namespace Gdiplus;
#define GDI_PLUS_DONE GdiplusShutdown(gdiplusToken);
#define GDI_PLUS_INIT GdiplusStartupInput gdiplusStartupInput; ULONG_PTR gdiplusToken; \
                      GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

static int GetEncoderClsid (const WCHAR* format, CLSID* pClsid);
static expected<image<RGBA>> FromImage (Image*);
static Image* MakeImage (frame<RGBA>);

using aux::error;

expected<XY> pix::size (array<sys::byte>::range_type r)
{
    auto result = unpack(r);
    if (!result.ok()) return result.error();
    return result.value().size;
}

expected<image<RGBA>> pix::unpack (array<sys::byte>::range_type r)
{
    return unpack (r.host.data() + r.offset(), r.size());
}

expected<image<RGBA>> pix::unpack (sys::byte* buffer, int size) try
{
    if (size <= 0) return image<RGBA>();

    GDI_PLUS_INIT;
    HGLOBAL hmem = ::GlobalAlloc(GMEM_MOVEABLE, size); if (!hmem) return error("pix::unpack failed");
    LPVOID  data = ::GlobalLock (hmem);                if (!data) return error("pix::unpack failed");

    ::memcpy(data, buffer, size);
    ::GlobalUnlock (hmem);

    LPSTREAM pstm = nullptr;
    ::CreateStreamOnHGlobal(hmem, TRUE, &pstm);

    Image* image = new Image(pstm); if (pstm) pstm->Release(); 
    auto result = FromImage(image);
    delete image;
    GDI_PLUS_DONE;

    return result;
}
catch(std::exception & e) { return error("pix::unpack: " + str(e.what())); }
catch(...){ return error("pix::unpack: EXCEPTION"); }


expected<pix::image<RGBA>> pix::read (std::filesystem::path path) try
{
    if (!std::filesystem::exists(path))
        return error("pix::read: not exists: " + path.string());

    GDI_PLUS_INIT;
    Image* image = new Image (winstr(path.string()).c_str());
    auto result = FromImage(image);
    delete image;
    GDI_PLUS_DONE;

    return result;
}
catch(std::exception & e) { return error("pix::read: " + str(e.what())); }
catch(...){ return error("pix::read: EXCEPTION " + path.string()); }


expected<nothing> pix::write (frame<RGBA> frame, std::filesystem::path path, int quality) try
{
    auto dir = path.parent_path();
    if (dir != std::filesystem::path())
        std::filesystem::create_directories(dir);

    if( quality == -1 ) quality = 95;
    EncoderParameters encoderParameters;
    encoderParameters.Parameter[0].Guid = EncoderQuality;
    encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
    encoderParameters.Parameter[0].NumberOfValues = 1;
    encoderParameters.Parameter[0].Value = & quality;
    encoderParameters.Count = 1;

    str ext = path.extension().string(); ext = ext.ascii_lowercased();
    str encoder = 
        ext == ".bmp"  ? "image/bmp"  :
        ext == ".gif"  ? "image/gif"  :
        ext == ".png"  ? "image/png"  :
        ext == ".tif"  ? "image/tiff" :
        ext == ".tiff" ? "image/tiff" :
        ext == ".jpg"  ? "image/jpeg" :
        ext == ".jpeg" ? "image/jpeg" : "";
    if (encoder == "") return error("pix::write: unsupported format: " + path.string());
    auto parameters = encoder == "image/jpeg" ? &encoderParameters : nullptr;

    GDI_PLUS_INIT;
    CLSID clsid; GetEncoderClsid(winstr(encoder).c_str(), &clsid);
    Image* image = MakeImage(frame);
    Status rc = image->Save(winstr(path.string()).c_str(), &clsid, parameters);
    delete image;
    GDI_PLUS_DONE;

    if (rc == Ok) return nothing{};
    else return error("pix::write: error: " + path.string());
}
catch(std::exception & e) { return error("pix::write: " + str(e.what())); }
catch(...){ return error("pix::write: EXCEPTION " + path.string()); }


expected<array<sys::byte>> pix::pack (frame<RGBA> frame, str format, int quality) try
{
    if( quality == -1 ) quality = 95;
    EncoderParameters encoderParameters;
    encoderParameters.Parameter[0].Guid = EncoderQuality;
    encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
    encoderParameters.Parameter[0].NumberOfValues = 1;
    encoderParameters.Parameter[0].Value = & quality;
    encoderParameters.Count = 1;

    str ext = "." + format; ext = ext.ascii_lowercased();
    str encoder = 
        ext == ".bmp"  ? "image/bmp"  :
        ext == ".gif"  ? "image/gif"  :
        ext == ".png"  ? "image/png"  :
        ext == ".tif"  ? "image/tiff" :
        ext == ".tiff" ? "image/tiff" :
        ext == ".jpg"  ? "image/jpeg" :
        ext == ".jpeg" ? "image/jpeg" : "";
    if (encoder == "") return error("pix::pack: unsupported format: " + format);
    auto parameters = encoder == "image/jpeg" ? &encoderParameters : nullptr;

    array<sys::byte> result; str rc;

    GDI_PLUS_INIT;
    CLSID clsid; GetEncoderClsid(winstr(encoder).c_str(), &clsid);
    Image* image = MakeImage(frame);
    // http://stackoverflow.com/questions/5610075/taking-a-jpeg-encoded-screenshot-to-a-buffer-using-gdi-and-c
    IStream *pStream = NULL;
    if (CreateStreamOnHGlobal(NULL, TRUE, &pStream) == S_OK) {
        if (image->Save(pStream, &clsid, parameters) == Ok) {
            LARGE_INTEGER liZero = {};
            ULARGE_INTEGER pos = {};
            if (pStream->Seek(liZero, STREAM_SEEK_SET, &pos) == S_OK) {
                STATSTG stg = {};
                if (pStream->Stat(&stg, STATFLAG_NONAME) == S_OK) {
                    if (stg.cbSize.QuadPart == stg.cbSize.LowPart) {
                        result.resize(stg.cbSize.LowPart);
                        ULONG bytesRead = 0;
                        if (pStream->Read(result.data(), stg.cbSize.LowPart, &bytesRead) == S_OK) {
                            if (bytesRead == stg.cbSize.LowPart) {
                                /* OK */
                            } else rc = "not all bytes read";
                        } else rc = "pStream read error";
                    } else rc = "u32 overflow";
                } else rc = "pStream stat error";
            } else rc = "pStream seek error";
        } else rc = "image save error";
        pStream->Release();
    } else rc = "create stream error";
    delete image;
    GDI_PLUS_DONE;

    if (rc != "") return error("pix::pack: error: " + rc);
    return result;
}
catch(std::exception & e) {return error("pix::pack: " + str(e.what())); }
catch(...){ return error("pix::pack: EXCEPTION"); }

///////////////////////////////////////////////////////////////////////////////

int GetEncoderClsid (const WCHAR* format, CLSID* pClsid)
{
   UINT num = 0; // number of image encoders
   UINT size = 0; // size of the image encoder array in bytes
   ImageCodecInfo* pImageCodecInfo = NULL;

   GetImageEncodersSize(&num, &size);
   if(size == 0) return -1;  // Failure

   pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
   if(pImageCodecInfo == NULL) return -1;  // Failure

   GetImageEncoders(num, size, pImageCodecInfo);

   for(UINT j = 0; j < num; ++j)
   {
      if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 ) {
         *pClsid = pImageCodecInfo[j].Clsid;
         free(pImageCodecInfo);
         return j; // Success
      }    
   }

   free(pImageCodecInfo);
   return -1; // Failure
}

expected<image<RGBA>> FromImage (Image* gdimage)
{
    int w = gdimage->GetWidth (); if (w <= 0) return image<RGBA>();
    int h = gdimage->GetHeight(); if (h <= 0) return image<RGBA>();

    BITMAPINFO bi;
    ZeroMemory(&bi,               sizeof(bi));
    bi.bmiHeader.biSize         = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth        = w;
    bi.bmiHeader.biHeight       = -h;
    bi.bmiHeader.biPlanes       = 1;
    bi.bmiHeader.biBitCount     = 32; 
    bi.bmiHeader.biCompression  = BI_RGB;
    bi.bmiHeader.biSizeImage    = w*h*4;

    HDC dc = ::CreateCompatibleDC(NULL);
    LPVOID  bits = NULL;
    HBITMAP bmp  = ::CreateDIBSection(NULL, (BITMAPINFO*)&bi, DIB_RGB_COLORS, &bits, NULL, NULL);
    if    (!bmp) { ::DeleteDC(dc); return error("pix:: FromImage: couldn't create bitmap"); }
    HGDIOBJ old  = ::SelectObject(dc, bmp);

    Graphics graphics (dc); graphics.DrawImage(gdimage, 0, 0, w, h);

    image<RGBA> result(XY(w, h));
   
    ::memcpy(result.data.data(), bits, w*h*4);

    ::SelectObject (dc, old);
    ::DeleteObject (bmp);
    ::DeleteDC     (dc);

    return result;
}

Image* MakeImage (frame<RGBA> frame)
{
    image<RGBA> copy; if (frame.size.x != frame.image->size.x)
    {
        copy = image<RGBA>(frame); frame = copy;
    }

    struct BITMAPINFO256
    {
        BITMAPINFOHEADER bmiHeader; 
        RGBQUAD          bmiColors [256];
    };
    BITMAPINFO256 bi; ZeroMemory(&bi, sizeof(bi));
    bi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth       =  frame.size.x;
    bi.bmiHeader.biHeight      = -frame.size.y;
    bi.bmiHeader.biPlanes      = 1;
    bi.bmiHeader.biBitCount    = 4*8; 
    bi.bmiHeader.biCompression = BI_RGB;
    bi.bmiHeader.biSizeImage   = 4*frame.size.x*frame.size.y;

    for (int n=0; n<256; n++){ RGBA c (n, n, n); ::memcpy (&bi.bmiColors[n], &c, 4); }

    BITMAPFILEHEADER bm;
    bm.bfType      = MAKEWORD('B', 'M'); 
    bm.bfSize      = DWORD(sizeof(bm) + sizeof(bi)) + bi.bmiHeader.biSizeImage; 
    bm.bfOffBits   = sizeof(bm) + sizeof(bi); 
    bm.bfReserved1 = 0; 
    bm.bfReserved2 = 0; 

    DWORD   size = bm.bfSize;
    HGLOBAL hmem = ::GlobalAlloc(GMEM_MOVEABLE, size);
    LPVOID  data = ::GlobalLock (hmem);
    void*   bits = &frame(0,0);

    ::memcpy((BYTE*) data,                            &bm,  sizeof(bm));
    ::memcpy((BYTE*) data + sizeof(bm),               &bi,  sizeof(bi));
    ::memcpy((BYTE*) data + sizeof(bm) + sizeof(bi),  bits, bi.bmiHeader.biSizeImage);

    ::GlobalUnlock(hmem);

    LPSTREAM pstm = NULL;
    HRESULT  hr   = NULL;
    hr = ::CreateStreamOnHGlobal(hmem, TRUE, &pstm);
    Image* gdimage = new Image (pstm);
    if (pstm) pstm->Release (); 
    return gdimage;
}
