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

    XYXY    clip;   // clip rectangle
    XYXY    dirty;  // updated rectangle
    alpha   global; // global alpha

    PLANE   (                  ) {}
    PLANE   ( FRAME<color>   f ) : FRAME<color> (f), clip (f.rect()), dirty (XYXY(0,0,0,0)), global (traits::alpha::solid()) {}
    PLANE   ( IMAGE<color> & f ) : FRAME<color> (f.frame ()), clip (f.rect()), dirty (XYXY(0,0,0,0)), global (traits::alpha::solid()) {}

#   define  TOOL                template< int op > void

    // pixels

    TOOL    pixel               ( XY,       color );
    TOOL    pixel               ( int,int,  color );
    TOOL    pixel_clip          ( int,int,  color );
    TOOL    pixel_clip_dirty    ( int,int,  color );

    TOOL    pixel               ( XY,       color, alpha );
    TOOL    pixel               ( int,int,  color, alpha );
    TOOL    pixel_clip          ( int,int,  color, alpha );
    TOOL    pixel_clip_dirty    ( int,int,  color, alpha );

    // blit

    TOOL    blit                (           FRAME<color> );
    TOOL    blit                ( XY,       FRAME<color> );
    TOOL    blit                ( int,int,  FRAME<color> );

    void    copy                (           FRAME<color> );
    void    copy                ( XY,       FRAME<color> );
    void    copy                ( int,int,  FRAME<color> );

    XYWH    blit_rect_src       ( int,int,  FRAME<color> );
    XYWH    blit_rect_dst       ( int,int,  FRAME<color> );

    void    fill                (           FRAME<color> pattern );
    void    fill                ( XYXY,     FRAME<color> pattern );

    // rectangles

    TOOL    fill                (           color );
    TOOL    fill                ( XYXY,     color );
    TOOL    rect                ( XYXY,     color );

    // lines ( jaggy )

    TOOL    line                ( XYXY,     color );
    TOOL    line                ( XY, XY,   color );
    TOOL    line_h              ( XY, int,  color );
    TOOL    line_v              ( XY, int,  color );

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
template< class color > inline XYWH PLANE<color>::blit_rect_dst ( int x, int y, FRAME<color> frame )
//============================================================================================================================//
{
    return rect () & XYWH ( x, y, frame.size.x, frame.size.y );
}
//============================================================================================================================//
template< class color > inline XYWH PLANE<color>::blit_rect_src ( int x, int y, FRAME<color> frame )
//============================================================================================================================//
{
    XYWH dst = blit_rect_dst ( x, y, frame );  return XYWH ( dst.x - x, dst.y - y, dst.w, dst.h );
}
//============================================================================================================================//
template< class color > inline void PLANE<color>::copy (               FRAME<color> frame ){ blit<colorop::copy>( frame ); }
template< class color > inline void PLANE<color>::copy ( XY pt,        FRAME<color> frame ){ blit<colorop::copy>( pt, frame ); }
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

TOOL    pixel_clip_dirty    ( int x,int y,  color c          ){ if( clip & XY(x,y) ) colorop::opp<op>::op ( at (   x,   y ), c    ), dirty |= XYWH (x,y,1,1); }
TOOL    pixel_clip          ( int x,int y,  color c          ){ if( clip & XY(x,y) ) colorop::opp<op>::op ( at (   x,   y ), c    ); }
TOOL    pixel               ( int x,int y,  color c          ){                      colorop::opp<op>::op ( at (   x,   y ), c    ); }
TOOL    pixel               ( XY p,         color c          ){                      colorop::opp<op>::op ( at ( p.x, p.y ), c    ); }

TOOL    pixel_clip_dirty    ( int x,int y,  color c, alpha a ){ if( clip & XY(x,y) ) colorop::opp<op>::op ( at (   x,   y ), c, a ), dirty |= XYWH (x,y,1,1); }
TOOL    pixel_clip          ( int x,int y,  color c, alpha a ){ if( clip & XY(x,y) ) colorop::opp<op>::op ( at (   x,   y ), c, a ); }
TOOL    pixel               ( int x,int y,  color c, alpha a ){                      colorop::opp<op>::op ( at (   x,   y ), c, a ); }
TOOL    pixel               ( XY p,         color c, alpha a ){                      colorop::opp<op>::op ( at ( p.x, p.y ), c, a ); }

//============================================================================================================================//
TOOL    blit                (               FRAME<color> frame ){ blit <op> ( XY(0,0), frame ); }
TOOL    blit                ( int x,int y,  FRAME<color> frame ){ blit <op> ( XY(x,y), frame ); }
TOOL    blit                ( XY p,         FRAME<color> frame )
//============================================================================================================================//
{
    XYWH dst = blit_rect_dst ( p.x, p.y, frame );
    XYWH src = blit_rect_src ( p.x, p.y, frame );

    dst &= clip; dirty |= dst;

    for( int y=0; y<dst.h; y++ )
    for( int x=0; x<dst.w; x++ )
    {
        pixel <op> ( dst.x + x, dst.y + y, frame ( src.x + x, src.y + y ) );
    }
}
//============================================================================================================================//
TOOL    fill                (               color c ){ fill <op> ( rect (), c ); }
TOOL    fill                ( XYXY r,       color c )
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
TOOL    rect                ( XYXY r,       color c )
//============================================================================================================================//
{
    line_h <op> ( XY ( r.xl, r.yl ), r.size ().x, c );    line_h <op> ( XY ( r.xh-1, r.yh-1 ), - r.size ().x, c );
    line_v <op> ( XY ( r.xl, r.yl ), r.size ().y, c );    line_v <op> ( XY ( r.xh-1, r.yh-1 ), - r.size ().y, c );
}
//============================================================================================================================//
TOOL    line                ( XYXY r,       color c ){ jaggy_line <color,op> ::draw ( *this, r.xl, r.yl, r.xh, r.yh, c ); }
TOOL    line                ( XY p1, XY p2, color c ){ jaggy_line <color,op> ::draw ( *this, p1.x, p1.y, p2.x, p2.y, c ); }
TOOL    line_h              ( XY p, int d,  color c ){ if( d == 0 ) return;  d = d < 0 ? d+1 : d-1;  line <op> ( p, p + XY (d,0), c ); }
TOOL    line_v              ( XY p, int d,  color c ){ if( d == 0 ) return;  d = d < 0 ? d+1 : d-1;  line <op> ( p, p + XY (0,d), c ); }
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



