#pragma once
#include "pix_text_solid.h"
namespace pix::text
{
    struct row: metrics
    {
        array<solid> solids;
        format format;
        int lpadding = 0;
        int rpadding = 0;
        xy offset;

        int elpadding (int height)
        {
            int d = 0;
            for (auto bar: format.lwrap) {
                d = aux::max(d, bar.x);
                height -= bar.y;
                if (height <= 0)
                    break;
            }
            return d + lpadding;
        }
        int erpadding (int height)
        {
            int d = 0;
            for (auto bar: format.rwrap) {
                d = aux::max(d, bar.x);
                height -= bar.y;
                if (height <= 0)
                    break;
            }
            return d + rpadding;
        }

        bool add (token_range& tokens)
        {
            solid s(tokens);

            metrics m = *this;
            m += s;


            //int max_width =
            //    format.width -
            //    elpadding(height) -
            //    erpadding(height);
            //
            //if ((format.wordwrap or format.ellipsis)
            //and advance + solid.width() > max_width
            //and not solids.empty()) // at least one should be accepted
            //{
            //    the_last_row = false;
            //    return false;
            //}
            //
            //if (format.ellipsis
            //and advance + solid.width() > max_width
            //and solids.empty())
            //    solid.ellipt(max_width);
            //
            //Ascent  = max(Ascent,  solid.Ascent);
            //ascent  = max(ascent,  solid.ascent);
            //Descent = max(Descent, solid.Descent);
            //descent = max(descent, solid.descent);
            //solid.offset.x = advance;
            //rborder = advance + solid.rborder;
            //advance += solid.advance;
            //solids += std::move(solid);
            //return true;
            //
            //
            //        if (not format.wordwrap)
            //            break;
        }


        void render (frame<rgba> frame, xy shift=xy{}, uint8_t alpha=255)
        {
            for (auto& x: solids)
                x.render(frame, shift + x.offset, alpha);
        }
    };
}
