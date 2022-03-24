#pragma once
#include "pix_draw_line.h"
namespace pix
{



}

//============================================================================================================================//
//                                                                 Copyright (c) Evgeny Burkov (levitz@inbox.ru, ICQ 8759747) //
//============================================================================================================================//
template< typename color_ >  struct  PLANE : public FRAME < color_ >
//============================================================================================================================//
{
    typedef          color_              color;

    typedef img::    traits  <color>     traits;
    typedef typename traits::alpha::type alpha;

    using FRAME<color>::p; using FRAME<color>::size; using FRAME<color>::pitch;

    using FRAME<color>::at; using FRAME<color>::rect;

    xyxy    clip;   // clip rectangle
    xyxy    dirty;  // updated rectangle
    alpha   global; // global alpha

    PLANE   (                  ) {}
    PLANE   ( FRAME<color>   f ) : FRAME<color> (f), clip (f.rect()), dirty (xyxy(0,0,0,0)), global (traits::alpha::solid()) {}
    PLANE   ( IMAGE<color> & f ) : FRAME<color> (f.frame ()), clip (f.rect()), dirty (xyxy(0,0,0,0)), global (traits::alpha::solid()) {}

#   define  TOOL                template< int op > void

    // pixels

    TOOL    pixel               ( xy,       color );
    TOOL    pixel               ( int,int,  color );
    TOOL    pixel_clip          ( int,int,  color );
    TOOL    pixel_clip_dirty    ( int,int,  color );

    TOOL    pixel               ( xy,       color, alpha );
    TOOL    pixel               ( int,int,  color, alpha );
    TOOL    pixel_clip          ( int,int,  color, alpha );
    TOOL    pixel_clip_dirty    ( int,int,  color, alpha );

    // blit

    TOOL    blit                (           FRAME<color> );
    TOOL    blit                ( xy,       FRAME<color> );
    TOOL    blit                ( int,int,  FRAME<color> );

    void    copy                (           FRAME<color> );
    void    copy                ( xy,       FRAME<color> );
    void    copy                ( int,int,  FRAME<color> );

    xywh    blit_rect_src       ( int,int,  FRAME<color> );
    xywh    blit_rect_dst       ( int,int,  FRAME<color> );

    void    fill                (           FRAME<color> pattern );
    void    fill                ( xyxy,     FRAME<color> pattern );

    // rectangles

    TOOL    fill                (           color );
    TOOL    fill                ( xyxy,     color );
    TOOL    rect                ( xyxy,     color );

    // lines ( jaggy )

    TOOL    line                ( xyxy,     color );
    TOOL    line                ( xy, xy,   color );
    TOOL    line_h              ( xy, int,  color );
    TOOL    line_v              ( xy, int,  color );

    // lines ( antialiased )

    TOOL    line                ( geo::vector<2>,       geo::vector<2>,       color );
    TOOL    line                ( geo::vector<2>,       geo::vector<2>,       color, color );
    TOOL    line                ( double x1, double y1, double x2, double y2, color );
    TOOL    line                ( double x1, double y1, double x2, double y2, color, color );

    // circles ( antialiased )

    TOOL    circle              ( geo::vector<2>,       double radius,        color, bool solid = false );
    TOOL    circle              ( double x,  double y,  double radius,        color, bool solid = false );

    TOOL    arc                 ( geo::vector<2>,       double radius,        color, double radian1, double radian2 );
    TOOL    arc                 ( double x,  double y,  double radius,        color, double radian1, double radian2 );

    // figures ( antialiased )

    TOOL    draw                ( geo::segment        < geo::vector<2> >,     color );
    TOOL    draw                ( geo::triangle       < geo::vector<2> >,     color );
    TOOL    draw                ( geo::quad           < geo::vector<2> >,     color );

#   undef   TOOL
};
//============================================================================================================================//
template< class color > inline xywh PLANE<color>::blit_rect_dst ( int x, int y, FRAME<color> frame )
//============================================================================================================================//
{
    return rect () & xywh ( x, y, frame.size.x, frame.size.y );
}
//============================================================================================================================//
template< class color > inline xywh PLANE<color>::blit_rect_src ( int x, int y, FRAME<color> frame )
//============================================================================================================================//
{
    xywh dst = blit_rect_dst ( x, y, frame );  return xywh ( dst.x - x, dst.y - y, dst.w, dst.h );
}
//============================================================================================================================//
template< class color > inline void PLANE<color>::copy (               FRAME<color> frame ){ blit<colorop::copy>( frame ); }
template< class color > inline void PLANE<color>::copy ( xy pt,        FRAME<color> frame ){ blit<colorop::copy>( pt, frame ); }
template< class color > inline void PLANE<color>::copy ( int x, int y, FRAME<color> frame ){ blit<colorop::copy>( x, y, frame ); }
//============================================================================================================================//
#   define  TOOL    template< class color > template< int op > inline void PLANE<color>::

#   include "img_draw_line.hpp"
#   include "img_draw_line_aa.hpp"
#   include "img_draw_tools.hpp"

#   undef   TOOL
//============================================================================================================================//


//============================================================================================================================//
//                                                                 Copyright (c) Evgeny Burkov (levitz@inbox.ru, ICQ 8759747) //
//============================================================================================================================//

TOOL    pixel_clip_dirty    ( int x,int y,  color c          ){ if( clip & xy(x,y) ) colorop::opp<op>::op ( at (   x,   y ), c    ), dirty |= xywh (x,y,1,1); }
TOOL    pixel_clip          ( int x,int y,  color c          ){ if( clip & xy(x,y) ) colorop::opp<op>::op ( at (   x,   y ), c    ); }
TOOL    pixel               ( int x,int y,  color c          ){                      colorop::opp<op>::op ( at (   x,   y ), c    ); }
TOOL    pixel               ( xy p,         color c          ){                      colorop::opp<op>::op ( at ( p.x, p.y ), c    ); }

TOOL    pixel_clip_dirty    ( int x,int y,  color c, alpha a ){ if( clip & xy(x,y) ) colorop::opp<op>::op ( at (   x,   y ), c, a ), dirty |= xywh (x,y,1,1); }
TOOL    pixel_clip          ( int x,int y,  color c, alpha a ){ if( clip & xy(x,y) ) colorop::opp<op>::op ( at (   x,   y ), c, a ); }
TOOL    pixel               ( int x,int y,  color c, alpha a ){                      colorop::opp<op>::op ( at (   x,   y ), c, a ); }
TOOL    pixel               ( xy p,         color c, alpha a ){                      colorop::opp<op>::op ( at ( p.x, p.y ), c, a ); }

//============================================================================================================================//
TOOL    blit                (               FRAME<color> frame ){ blit <op> ( xy(0,0), frame ); }
TOOL    blit                ( int x,int y,  FRAME<color> frame ){ blit <op> ( xy(x,y), frame ); }
TOOL    blit                ( xy p,         FRAME<color> frame )
//============================================================================================================================//
{
    xywh dst = blit_rect_dst ( p.x, p.y, frame );
    xywh src = blit_rect_src ( p.x, p.y, frame );

    dst &= clip; dirty |= dst;

    for( int y=0; y<dst.h; y++ )
    for( int x=0; x<dst.w; x++ )
    {
        pixel <op> ( dst.x + x, dst.y + y, frame ( src.x + x, src.y + y ) );
    }
}
//============================================================================================================================//
TOOL    fill                (               color c ){ fill <op> ( rect (), c ); }
TOOL    fill                ( xyxy r,       color c )
//============================================================================================================================//
{
    r &= clip; dirty |= r;

    for( int y=r.yl; y<r.yh; y++ )
    for( int x=r.xl; x<r.xh; x++ )
    {
        pixel <op> ( x, y, c );
    }
}
//============================================================================================================================//
TOOL    rect                ( xyxy r,       color c )
//============================================================================================================================//
{
    line_h <op> ( xy ( r.xl, r.yl ), r.size ().x, c );    line_h <op> ( xy ( r.xh-1, r.yh-1 ), - r.size ().x, c );
    line_v <op> ( xy ( r.xl, r.yl ), r.size ().y, c );    line_v <op> ( xy ( r.xh-1, r.yh-1 ), - r.size ().y, c );
}
//============================================================================================================================//
TOOL    line                ( xyxy r,       color c ){ jaggy_line <color,op> ::draw ( *this, r.xl, r.yl, r.xh, r.yh, c ); }
TOOL    line                ( xy p1, xy p2, color c ){ jaggy_line <color,op> ::draw ( *this, p1.x, p1.y, p2.x, p2.y, c ); }
TOOL    line_h              ( xy p, int d,  color c ){ if( d == 0 ) return;  d = d < 0 ? d+1 : d-1;  line <op> ( p, p + xy (d,0), c ); }
TOOL    line_v              ( xy p, int d,  color c ){ if( d == 0 ) return;  d = d < 0 ? d+1 : d-1;  line <op> ( p, p + xy (0,d), c ); }
//============================================================================================================================//
TOOL    line                ( geo::vector<2> p1,    geo::vector<2> p2,    color c            ){ line <op> ( p1.x, p1.y, p2.x, p2.y, c,  c  ); }
TOOL    line                ( geo::vector<2> p1,    geo::vector<2> p2,    color c1, color c2 ){ line <op> ( p1.x, p1.y, p2.x, p2.y, c1, c2 ); }
TOOL    line                ( double x1, double y1, double x2, double y2, color c            ){ line <op> ( x1,   y1,   x2,   y2,   c,  c  ); }
TOOL    line                ( double x1, double y1, double x2, double y2, color c1, color c2 )
//============================================================================================================================//
{
    aa_line <color,op> ::draw_line ( *this, x1, y1, x2, y2, c1, c2 );
}
//============================================================================================================================//
TOOL    circle              ( geo::vector<2> p,     double r, color c, bool solid ){ circle <op> ( p.x, p.y, r, c, solid ); }
TOOL    circle              ( double x,  double y,  double r, color c, bool solid )
//============================================================================================================================//
{
    if( solid ) solid_circle <color,op> ::draw ( *this, x, y, r, c ); else arc <op> ( x, y, r, c, 0, 2*pi );
}
//============================================================================================================================//
TOOL    arc                 ( geo::vector<2> p,     double r, color c, double r1, double r2 ){ arc <op> ( p.x, p.y, r, c, r1, r2 ); }
TOOL    arc                 ( double x,  double y,  double r, color c, double r1, double r2 )
//============================================================================================================================//
{
    aa_line <color,op> ::draw_circle ( *this, x, y, r, c, r1, r2 );
}
//============================================================================================================================//
TOOL    draw                ( geo::segment  < geo::vector<2> > p, color c ){ line<op>( p.p1,    p.p2,    c ); }
TOOL    draw                ( geo::triangle < geo::vector<2> > p, color c ){ line<op>( p.v [0], p.v [1], c ); line<op>( p.v [1], p.v [2], c ); line<op>( p.v [2], p.v [0], c ); }
TOOL    draw                ( geo::quad     < geo::vector<2> > p, color c ){ line<op>( p.v [0], p.v [1], c ); line<op>( p.v [1], p.v [2], c ); line<op>( p.v [2], p.v [3], c ); line<op>( p.v [3], p.v [0], c ); }
//============================================================================================================================//



