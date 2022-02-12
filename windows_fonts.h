#include "pix_text.h"
#include "windows_aux.h"
#include <tchar.h>

struct GDI_FONT
{
    HFONT handle;
    
    pix::font::metrics metrics;

    GDI_FONT(pix::font font)
    {
        HDC dc = ::CreateCompatibleDC (NULL);

        if (font.face == "") font.face = "Segoe UI";
        if (font.size ==  0) font.size = gui::metrics::text::height;

        LONG height = font.size >= 0 ? -font.size :
            MulDiv (font.size, ::GetDeviceCaps(dc,LOGPIXELSY), 72);

        str face = font.face;
        if (face == "monospace")
            face =  "Consolas";
        if (face == "small-caps")
            face =  "";

        LOGFONT lf;
        _tcscpy_s (
        lf.lfFaceName       , 32, str(face.upto(31)).c_str());
        lf.lfHeight         = height;
        lf.lfWidth          = 0;
        lf.lfEscapement     = 0;
        lf.lfOrientation    = 0;
        lf.lfWeight         = font.bold ? FW_BOLD : FW_NORMAL;
        lf.lfItalic         = font.italic ? TRUE : FALSE;
        lf.lfStrikeOut      = FALSE;
        lf.lfUnderline      = FALSE;
        lf.lfCharSet        = DEFAULT_CHARSET;
        lf.lfOutPrecision   = 0;//OUT_TT_PRECIS;
        lf.lfClipPrecision  = 0;//CLIP_DEFAULT_PRECIS;
        lf.lfQuality        = DEFAULT_QUALITY;//CLEARTYPE_QUALITY;
        lf.lfPitchAndFamily = 0;//DEFAULT_PITCH | FF_DONTCARE;

        handle = ::CreateFontIndirect (&lf);
        
        HGDIOBJ old = ::SelectObject (dc, handle);

        TEXTMETRIC tm;  ::GetTextMetrics (dc, &tm);

        metrics.height   = tm.tmHeight;  // ascent + descent
        metrics.ascent   = tm.tmAscent;  // units above the base line
        metrics.descent  = tm.tmDescent; // units below the base line
        metrics.linegap  = tm.tmExternalLeading;
        metrics.average_char_width = tm.tmAveCharWidth;
        metrics.maximum_char_width = tm.tmMaxCharWidth;
        metrics.minimum_char_width = 0;

        ::SelectObject (dc, old);

        ::DeleteDC (dc);
    }
    // ~GDI_FONT() { if (handle) ::DeleteObject (handle); }
};

MAKE_HASHABLE(pix::font, t.face, t.size, t.bold, t.italic);

static GDI_FONT cache (pix::font font) {
    static std::unordered_map<pix::font, GDI_FONT> fonts;
    auto it = fonts.find(font); if (it == fonts.end())
    it = fonts.emplace(font, GDI_FONT(font)).first;
    return it->second;
}

pix::font::metrics pix::metrics (pix::font font)
{
    return cache(
        font != pix::font{} ?
        font :  pix::font{"Arial", -8})
        .metrics;
}

struct GDI_CONTEXT
{
    HDC dc;
    HGDIOBJ old;
    GDI_FONT font;

    GDI_CONTEXT(pix::font f) : font (cache(f))
    {
        dc = ::CreateCompatibleDC (NULL);
        old = ::SelectObject (dc, font.handle);
    }
   ~GDI_CONTEXT()
    {
        ::SelectObject (dc, old);
        ::DeleteDC     (dc);
    }
};

///////////////////////////////////////////////////////////////////////////////

struct cache_metrics_key
{
    str text; pix::font font; 

    bool operator == (const cache_metrics_key & k) const = default;
};

MAKE_HASHABLE(cache_metrics_key, t.text, t.font);

static std::unordered_map<cache_metrics_key, pix::text::metrics> cache_metrics;

pix::glyph::glyph (str text, text::style_index i) : text(text), style_index(i)
{
    if (text == "") return;
    const auto & style = this->style();
    cache_metrics_key key {text, style.font};
    auto it = cache_metrics.find(key);
    if (it != cache_metrics.end()) text::metrics::operator = (it->second); else
    {
        auto font = style.font;
        if (font.face == "small-caps" and
            text.is_ascii_lowercased()) {
            text = text.ascii_uppercased();
            font.face = "";
            font.size = (font.size == 0 ?
                gui::metrics::text::height:
                font.size) * 85/100;
        }

        GDI_CONTEXT context(font);

        SIZE Size;
        auto ss = winstr(text);
        auto rc = ::GetTextExtentPoint32W (context.dc, ss.c_str(), (int) ss.size(), &Size);
        if (!rc) throw std::runtime_error("pix::font::render: GetTextExtentPoint32W fail");

        advance = Size.cx;
        ascent  = context.font.metrics.ascent;
        descent = context.font.metrics.descent;
        
        width = 2*advance; // italics are wider
        static pix::image<RGBA> image;
        image.resize(XY(width, ascent+descent));
        image.fill(RGBA::white);

        text::style simple_style;
        simple_style.font = font;
        simple_style.color = RGBA::black;

        sys::glyph simple_glyph = *this;
        simple_glyph.style_index = text::style_index(simple_style);
        simple_glyph.render(image);

        XYXY r = XYWH(0, 0, image.size.x, image.size.y);

//        for (bool stop = false; !stop && r.yh > 0; r.yh--)
//            for (int x = 0; x < r.xh; x++)
//                if (image(x, r.yh-1) != RGBA::white)
//                { stop = true; break; }
//
//        for (bool stop = false; !stop && r.yl < r.yh; r.yl++)
//            for (int x = 0; x < r.xh; x++)
//                if (image(x, r.yl) != RGBA::white)
//                { stop = true; break; }

        for (bool stop = false; !stop && width > 0; width--)
            for (int y = 0; y < image.size.y; y++)
                if (image(width-1, y) != RGBA::white)
                { stop = true; break; }

        ascent_  = ascent;
        descent_ = descent;

        rpadding = advance - width; // negative for italic

        width = max(advance, advance - rpadding);

        // for text editor
        if (width == 0 and text == "\n")
            width = max(1, (ascent+descent)/16);

        cache_metrics.emplace(key, *this);
    }

    ascent  -= style.shift.y;
    ascent_ -= style.shift.y;
    descent += style.shift.y;
    descent_+= style.shift.y;
    bearing += style.shift.x;

    // adapt metrics for style.outline/undeline/shadow here
}

///////////////////////////////////////////////////////////////////////////////

struct cache_glyphs_key
{
    aux::str  text;
    pix::font font;
    pix::RGBA fore;
    pix::RGBA back;

    bool operator == (const cache_glyphs_key & k) const = default;
};

MAKE_HASHABLE(cache_glyphs_key, t.text, t.font, t.fore, t.back);

static std::unordered_map<cache_glyphs_key, pix::image<RGBA>> cache_glyphs;

void sys::glyph::render (pix::frame<RGBA> frame, XY offset, uint8_t alpha, int x)
{
    const auto & style = this->style();
    if (alpha == 0) return;
    if (text == "") return;
    if (text.contains_only(one_of(" \t\r\n"))
    &&  style.underline.color.a == 0
    &&  style.strikeout.color.a == 0)
        return;

    auto text = this->text;
    auto font = style.font;
    if (font.face == "small-caps" and
        text.is_ascii_lowercased()) {
        text = text.ascii_uppercased();
        font.face = "";
        font.size = (font.size == 0 ?
            gui::metrics::text::height:
            font.size) * 85/100;
    }

    RGBA fore = style.color;
    RGBA back;

    bool solid_color_background = back.a == 255;
    if (!solid_color_background)
    {
        bool ok = true;
        RGBA c = frame(0,0);
        for (int y=0; y<frame.size.y; y++)
        for (int x=0; x<frame.size.x; x++) if (frame(x,y) != c) ok = false;
        if (ok) { solid_color_background = true; c.blend(back, alpha); back = c; }
    }

    text::style cacheable_style;
    cacheable_style.font = font;
    cacheable_style.color = style.color;

    bool cacheable =
        text.size() <= 4 and
        solid_color_background and
        style == cacheable_style;

    if (cacheable)
    {
        auto it = cache_glyphs.find(cache_glyphs_key{text, font, fore, back});
        if (it != cache_glyphs.end())
        {
            frame.blend_from(it->second.crop()
                .from(offset.x, offset.y), alpha);
            return;
        }
    }

    if (false) { // test for rendering speed
        frame.blend(RGBA::random(), alpha);
        return;
    }

    int w = 2*advance; // italics are wider
    int h = ascent + descent;
    if (w <= 0 or h <= 0) return;

    GDI_CONTEXT context(font);

    BITMAPINFO bi;
    ZeroMemory(&bi,               sizeof(bi));
    bi.bmiHeader.biSize         = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth        = w;
    bi.bmiHeader.biHeight       = -h;
    bi.bmiHeader.biPlanes       = 1;
    bi.bmiHeader.biBitCount     = 32; 
    bi.bmiHeader.biCompression  = BI_RGB;
    bi.bmiHeader.biSizeImage    = w*h*4;

    LPVOID  bits = NULL;
    HBITMAP bmp = ::CreateDIBSection (NULL, (BITMAPINFO*)&bi, DIB_RGB_COLORS, &bits, NULL, NULL);
    if    (!bmp) throw std::runtime_error ( "pix::font::render : CreateDIBSection fail" );
    HGDIOBJ old = ::SelectObject (context.dc, bmp);

    auto ss = winstr(text);

    if (!solid_color_background)
    ::SetBkMode    (context.dc, TRANSPARENT); else
    ::SetBkColor   (context.dc, RGB(back.r, back.g, back.b));
    ::SetTextColor (context.dc, RGB(fore.r, fore.g, fore.b));

    pix::view<RGBA> view ((RGBA*)bits, XY(w,h), w);

    if (!solid_color_background) frame.copy_to(view.from(offset.x, offset.y));
    if (!solid_color_background) view.from(offset.x, offset.y).blend(back, alpha);

    for (int y=0; y<h; y++)
    for (int x=0; x<w; x++) view(x,y).a = 255;

    ::TextOutW (context.dc, 0, 0, ss.c_str(), (int) ss.size());
    ::GdiFlush ();

    for (int y=0; y<h; y++)
    for (int x=0; x<w; x++) view(x,y).a = min (fore.a, 255 - view(x,y).a);

    // https://github.com/wentin/underlineJS
    // default underline thickness = 1/6 of width of the period mark
    // optimal Y position is the goden ratio point between the baseline and the descender line

    frame.blend(back, alpha);
    frame.blend_from(view.from(offset.x, offset.y), alpha);
    
    if (cacheable)
    {
        pix::image<RGBA> image (XY(w,h));
        image.crop().copy_from(view);
        cache_glyphs.emplace(
            cache_glyphs_key{text, font, fore, back},
            image);
    }

    ::SelectObject (context.dc, old);
    ::DeleteObject (bmp);
}

