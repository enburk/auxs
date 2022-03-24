#pragma once
#include "sys_ui.h"
#include "windows_aux.h"
#include <gl\gl.h>
#include <gl\glu.h>

inline void glcheck (str what)
{
    str s; GLenum rc, rcprev = GL_NO_ERROR;
    while ((rc = ::glGetError()) != GL_NO_ERROR) {
        if (rc & GL_INVALID_ENUM     ) s += "Invalid Enum, "; else
        if (rc & GL_INVALID_VALUE    ) s += "Invalid Value, "; else
        if (rc & GL_INVALID_OPERATION) s += "Invalid Operation, "; else
        if (rc & GL_STACK_OVERFLOW   ) s += "Stack Overflow, "; else
        if (rc & GL_STACK_UNDERFLOW  ) s += "Stack Underflow, "; else
        if (rc & GL_OUT_OF_MEMORY    ) s += "Out of Memory, "; else s += "Unknow Error, ";
        if (rcprev == rc) break; rcprev = rc; // prevent endless loop
    }
    s.truncate();
    s.truncate();
    //if (s != "") throw std::runtime_error(
    //    "OpenGL: " + what + " : " + s);
}

void sys::window::render (
    xywh r, uint8_t alpha, rgba c,
    xy offset, pix::geo geo,
    double* points, int n)
{
    if (geo == pix::geo::none)
        return;

    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    int kind = 0;
    switch (geo) { using enum geo;
    break; case points: kind = GL_POINTS;
    break; case lines: kind = GL_LINES;
    break; case line_strip: kind = GL_LINE_STRIP;
    break; case line_loop: kind = GL_LINE_LOOP;
    break; case triangles: kind = GL_TRIANGLES;
    break; case triangle_strip: kind = GL_TRIANGLE_STRIP;
    break; case triangle_fan: kind = GL_TRIANGLE_FAN;
    break; default: kind = GL_POINTS;
    }

    GLint xywh[4];
    glGetIntegerv(GL_VIEWPORT, xywh);
    int H = xywh[3];

    auto x = r.x;
    auto y = r.y; y = H - y - r.h;
    auto w = r.w;
    auto h = r.h;

    glScissor(x, y, w, h);
    glEnable(GL_SCISSOR_TEST);

    glBegin(kind);
    glColor4d(c.r/255.0, c.g/255.0, c.b/255.0, c.a*alpha/255.0/255.0);
    for (int i=0; i<n/2; i++) {
    double x = 0 + (r.x + offset.x + points[i*2+0]);
    double y = H - (r.y + offset.y + points[i*2+1]);
    glVertex3d(x,y,0); }
    glEnd();

    glDisable(GL_SCISSOR_TEST);
}

void sys::window::render (xywh r, uint8_t alpha, rgba c)
{
    GLint xywh[4];
    glGetIntegerv(GL_VIEWPORT, xywh);
    int H = xywh[3];

    auto x = (float)r.x;
    auto y = (float)r.y; y = H - y;
    auto w = (float)r.w;
    auto h = (float)r.h; h = -h;

    glBegin(GL_QUADS);
    glColor4d(c.r/255.0, c.g/255.0, c.b/255.0, c.a*alpha/255.0/255.0);
    glVertex3f(x,y,0);
    glVertex3f(x+w,y,0);
    glVertex3f(x+w,y+h,0);
    glVertex3f(x,y+h,0);
    glEnd();
}

void sys::window::render (xywh r, uint8_t alpha, frame<rgba> frame)
{
    struct texture { unsigned handle = max<unsigned>(); };
    static std::map<pix::image<rgba>*, texture> textures;

    auto & [handle] = textures[frame.image];
    if (handle == max<unsigned>()) glGenTextures(1, &handle);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, handle); glcheck("bind texture");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (frame.image->updates.size() > 0) {
        frame.image->updates.clear();
        int w = frame.image->size.x;
        int h = frame.image->size.y;
        int W = 1; while (W < w) W *= 2;
        int H = 1; while (H < h) H *= 2;
        int mipmap = 0; int border = 0;
        glPixelStorei(GL_UNPACK_ROW_LENGTH, w);
        glTexImage2D(GL_TEXTURE_2D, mipmap, GL_RGBA8, W, H,
            border, GL_BGRA_EXT, GL_UNSIGNED_BYTE, NULL);
        glcheck("init texture");
        glTexSubImage2D(GL_TEXTURE_2D, mipmap, 0,0, w,h,
            GL_BGRA_EXT, GL_UNSIGNED_BYTE,
            frame.image->data.data());
        glcheck("load texture");
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    }

    int mipmap = 0;
    int W; glGetTexLevelParameteriv(GL_TEXTURE_2D, mipmap, GL_TEXTURE_WIDTH,  &W);
    int H; glGetTexLevelParameteriv(GL_TEXTURE_2D, mipmap, GL_TEXTURE_HEIGHT, &H);

    auto tx1 = (double) frame.offset.x / W;
    auto tx2 = (double) frame.size  .x / W + tx1;
    auto ty1 = (double) frame.offset.y / H;
    auto ty2 = (double) frame.size  .y / H + ty1;

    GLint xywh[4]; glGetIntegerv(GL_VIEWPORT, xywh);
    r.y = xywh[3] - r.y;
    r.h = -r.h;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_QUADS);
    glColor4d(1.0, 1.0, 1.0, alpha/255.0);
    glTexCoord2d(tx1, ty1); glVertex2d(r.x,     r.y);
    glTexCoord2d(tx2, ty1); glVertex2d(r.x+r.w, r.y);
    glTexCoord2d(tx2, ty2); glVertex2d(r.x+r.w, r.y+r.h);
    glTexCoord2d(tx1, ty2); glVertex2d(r.x,     r.y+r.h);
    glEnd(); //glcheck("end texture");
    glDisable(GL_TEXTURE_2D);
}

void sys::window::render (xywh r, uint8_t alpha, glyph g, xy offset, int x)
{
    const auto & style = g.style();
    if (alpha == 0) return;
    if (g.text == "") return;
    if (g.text.contains_only(one_of(" \t\r\n"))
    &&  style.underline.color.a == 0
    &&  style.strikeout.color.a == 0)
        return;

    int w = g.width;
    int h = g.ascent + g.descent;
    if (w <= 0 || h <= 0) return;

    rgba fore = style.color;
    rgba back = (fore.r + fore.g + fore.b)/3 >= 128 ? rgba::black : rgba::white;

    static std::unordered_map<cache_glyphs_key, pix::image<rgba>> cache;
    auto key = cache_glyphs_key{g.text, style.font, fore, back};
    auto it = cache.find(key);
    if (it == cache.end())
    {
        pix::image<rgba> color (xy(3*w,h), fore);
        pix::image<rgba> alpha (xy(w,h), rgba::black);

        text::style simple_style;
        simple_style.font = style.font;
        simple_style.color = rgba::white;

        pix::glyph simple_glyph = g;
        simple_glyph.style_index = text::style_index(simple_style);
        simple_glyph.render(alpha);

        for (int y=0; y<h; y++)
        for (int x=0; x<w; x++)
        {
            color(3*x+0,y).a = alpha(x,y).r;
            color(3*x+1,y).a = alpha(x,y).r; // g; - worse
            color(3*x+2,y).a = alpha(x,y).r; // b; - worse
        }

        it = cache.emplace(key,
            std::move(color)).first;
    }

    render(r, alpha, it->second.crop(xywh(
        3*offset.x, offset.y,
        3*r.w, r.h)));
}
