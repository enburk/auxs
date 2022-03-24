#pragma once
#include "pix_color.h"
#include "pix_image.h"

//============================================================================================================================//
template<class color_,int op_> struct aa_line  
//============================================================================================================================//
{
    typedef color_ color; enum { op = op_ };

    // Sample code to draw antialiased lines as described in the Journal of
    // Graphic Tools article High Quality Hardware Line Antialiasing by
    // Scott R. Nelson of Sun Microsystems.

    typedef int64s fix_xy;    // S11.20
    typedef int64s fix_z;     // S1.30
    typedef int64s fix_rgb;   // S1.30

    struct aa_vertex // One vertex at any of the various stages of the pipeline
    {
        unsigned header;
        float x, y, z, w;
        float r, g, b, a;
    };

    struct aa_setup_line // All values needed to draw one line
    {
        int x_major;
        int negative;

        fix_xy vs;          // Starting point
        fix_xy us;
        fix_xy ue;          // End (along major axis)
        fix_xy dvdu;        // Delta for minor axis step

        fix_z zs;           // Starting Z and color
        fix_rgb rs;
        fix_rgb gs;
        fix_rgb bs;
        fix_rgb as;

        fix_z dzdu;         // Delta for Z and color
        fix_rgb drdu;
        fix_rgb dgdu;
        fix_rgb dbdu;
        fix_rgb dadu;
    };

    enum // Sizes for tables in Draw
    {
        SRT_INT         =  5,   // Sqrt table index integer bits
        SRT_FRACT       =  4,   // ...fraction bits
        SR_INT          =  3,   // Square root result integer bits
        SR_FRACT        =  5,   // ...fraction bits
        SR_TABLE_SIZE   = (1 << (SRT_INT + SRT_FRACT)),
        FI_TABLE_SIZE   = 64,   // Filter table size
        SC_TABLE_SIZE   = 32,   // Slope correction table size
    };

    static  int64s  slope_corr_table [SC_TABLE_SIZE];
    static  int64s  filter_table     [FI_TABLE_SIZE];
    static  int64s  sqrt_table       [SR_TABLE_SIZE];
    static  void    init_tables      (double  width);

    enum
    {
        EP_MASK         = (int64s) 0x000f0000u, // AA line end-point filter mask
        EP_SHIFT        = 13u,                  // Number of bits to shift end-point
        ONE_XY          = (int64s) 0x00100000,  // Convert from floating-point to internal fixed-point formats
        FIX_XY_SHIFT    = (int64s) 20,
        ONEHALF_XY      = (int64s) 0x00080000,
        ONE_Z           = (int64s) 0x40000000,
        ONE_RGB         = (int64s) 0x40000000,
        ONE_16          = (int64s) 0x4000,
    };

    int64s  FLOAT_TO_FIX_XY (float  x){ return ((int64s) ((x) * (float) ONE_XY)); }
    int64s  FLOAT_TO_FIX_Z  (float  x){ return ((int64s) ((x) * (float) ONE_Z)); }
    int64s  FLOAT_TO_FIX_RGB(float  x){ return ((int64s) ((x) * (float) ONE_RGB)); }
    int64s  FLOAT_TO_FIX_16 (float  x){ return ((int64s) ((x) * (float) ONE_16)); }
    int64s  FIX_TO_INT_XY   (int64s x){ return ((x) >> FIX_XY_SHIFT); }
    float   FIX_16_TO_FLOAT (int64s x){ return ((float) (x) / (float) ONE_16); }
    float   FIX_TO_FLOAT_XY (int64s x){ return ((float) (x) / (float) ONE_XY); }
    float   FIX_TO_FLOAT_Z  (int64s x){ return ((float) (x) / (float) ONE_Z); }
    float   FIX_TO_FLOAT_RGB(int64s x){ return ((float) (x) / (float) ONE_RGB); }
    // Get fractional part, next lowest integer part
    int64s  FRACT_XY        (int64s x){ return ((x) & (int64s) (0x00000000000fffff)); }
    int64s  FLOOR_XY        (int64s x){ return ((x) & (int64s) (0xfffffffffff00000)); }
    int     FIX_XY_TO_INT   (int64s x){ return (int)((int64s) (x) >> (int64s) FIX_XY_SHIFT); }

    int64s  Fix_xy_mult     (int64s a, fix_xy b);//{ return a*b; }
    int     clamp_rgb       (int64s x){ if (x < 0) x = 0; else if (x >= ONE_RGB) x = ONE_RGB - 1; return (int)(x >> (30 - 8)); }

    void    Draw_aa_hspan   ( fix_xy x, fix_xy y, fix_z z,fix_rgb r, fix_rgb g, fix_rgb b, int64s ep_corr, int64s slope);
    void    Draw_aa_vspan   ( fix_xy x, fix_xy y, fix_z z,fix_rgb r, fix_rgb g, fix_rgb b, int64s ep_corr, int64s slope);
    void    SetupLine       ( aa_vertex *v1, aa_vertex *v2);
    void    DrawLine        ( aa_setup_line *line);

    void    DrawLine        ( double x1, double y1, double x2, double y2, COLOR color1, COLOR color2);
    void    ProcessPixel    ( int x, int y, fix_rgb r, fix_rgb g, fix_rgb b, fix_rgb a );


    bool    m_Antialiased;  // TRUE by default
    bool    m_Capline;      // edge points drawing TRUE by default
    float   m_Gamma;        // boldness 1.8 by default

    FRAME<color> frame;

    aa_line ( double width = 1. );

    double extra;

    static void draw_line   ( FRAME<color> port, double x1, double y1, double x2, double y2, COLOR color1, COLOR color2, double width = 1 );
    static void draw_circle ( FRAME<color> port, double x, double y, double r, COLOR co, double a1, double a2, double width = 1 );
};
//============================================================================================================================//
template<class color,int op> inline void aa_line<color,op>::init_tables ( double width ) 
//============================================================================================================================//
{
    int    i;   // Iterative counter 
    double m;   // Slope 
    double d;   // Distance from center of curve 
    double v;   // Value to put in table 
    double sr;  // The square root value 

    const double FILTER_WIDTH = 0.7 * width; // Line filter width adjustment

    // Build slope correction table.  The index into this table
    // is the truncated 5-bit fraction of the slope used to draw
    // the line.  Round the computed values here to get the closest
    // fit for all slopes matching an entry.

    for (i = 0; i < SC_TABLE_SIZE; i++)  // Round and make a fraction 
    {
        m = ((double) i + 0.5) / (float) SC_TABLE_SIZE;
        v = sqrt(m * m + 1) * 0.707106781; // (m + 1)^2 / sqrt(2) 
        slope_corr_table[i] = (int64s) (v * 256.0);
    }

    // Build the Gaussian filter table, round to the middle of the sample region.

    for (i = 0; i < FI_TABLE_SIZE; i++)
    {
        d = ((double) i + 0.5) / (float) (FI_TABLE_SIZE / 2.0);
        d = d / FILTER_WIDTH;
        v = 1.0 / exp(d * d);        // Gaussian function 
        filter_table[i] = (int64s) (v * 256.0);
    }

    // Build the square root table for big dots.

    for (i = 0; i < SR_TABLE_SIZE; i++)
    {
        v = (double) ((i << 1) + 1) / (double) (1 << (SRT_FRACT + 1));
        sr = sqrt(v);
        sqrt_table[i] = (int64s) (sr * (double) (1 << SR_FRACT));
    }
}
//============================================================================================================================//
template<class color,int op> inline int64s aa_line<color,op>::Fix_xy_mult (int64s a, fix_xy b)
//============================================================================================================================//
{
    int64s negative;            /* 1 = result is negative */
    int64s a1;                /* Multiplier */
    int64s bh, bl;                /* Multiplicant (high and low) */
    int64s ch, cl, c;            /* Product */

    /* Determine the sign, then force multiply to be unsigned */
    negative = 0;
    if (a < 0) {
    negative ^= 1;
    a = -a;
    }
    if (b < 0) {
    negative ^= 1;
    b = -b;
    }

    /* Grab the bits we want to use */
    a1 = a >> 10;            /* Just use 10-bit fraction */

    /* Split the 32-bit number into two 16-bit halves */
    bh = (b >> 16) & 0xffff;
    bl = b & 0xffff;

    /* Perform the multiply */
    ch = bh * a1;            /* 30 bit product (with no carry) */
    cl = bl * a1;
    /* Put the halves back together again */
    c = (ch << 6) + (cl >> 10);
    if (negative)
    c = -c;

    return c;
}
//============================================================================================================================//
template<class color,int op> inline void aa_line<color,op>::DrawLine ( double x1, double y1, double x2, double y2, COLOR color1, COLOR color2 )
//============================================================================================================================//
{
    aa_vertex v1, v2;

    v1.z = 1.0;
    v1.r = (float) color1.v.x;
    v1.g = (float) color1.v.y;
    v1.b = (float) color1.v.z;
    v1.a = (float) color1.a; // 1.0;

    v2.z = 1.0;
    v2.r = (float) color2.v.x;
    v2.g = (float) color2.v.y;
    v2.b = (float) color2.v.z;
    v2.a = (float) color2.a; // 1.0;

    v1.x = (float) x1;
    v1.y = (float) y1;
    v2.x = (float) x2;
    v2.y = (float) y2;

    SetupLine (&v1, &v2);
}
//============================================================================================================================//
template<class color,int op> inline void aa_line<color,op>::SetupLine (aa_vertex *v1, aa_vertex *v2)
//============================================================================================================================//
{
    float dx, dy;            // Deltas in X and Y 
    float udx, udy;          // Positive version of deltas 
    float one_du;            // 1.0 / udx or udy 
    aa_setup_line line;

    dx = v1->x - v2->x;
    dy = v1->y - v2->y;
    if (dx < 0.0)
    udx = -dx;
    else
    udx = dx;
    if (dy < 0.0)
    udy = -dy;
    else
    udy = dy;

    if (udx > udy) {
    // X major line 
    line.x_major = 1;
    line.negative = (dx < 0.0);
    line.us = FLOAT_TO_FIX_XY(v2->x);
    line.vs = FLOAT_TO_FIX_XY(v2->y);
    line.ue = FLOAT_TO_FIX_XY(v1->x);
    one_du = (float)1.0 / udx;
    line.dvdu = FLOAT_TO_FIX_XY(dy * one_du);
    }
    else {
    // Y major line 
    line.x_major = 0;
    line.negative = (dy < 0.0);
    line.us = FLOAT_TO_FIX_XY(v2->y);
    line.vs = FLOAT_TO_FIX_XY(v2->x);
    line.ue = FLOAT_TO_FIX_XY(v1->y);
    one_du = (float)1.0 / udy;
    line.dvdu = FLOAT_TO_FIX_XY(dx * one_du);
    }

    // Convert start Z and colors to fixed-point 
    line.zs = FLOAT_TO_FIX_Z(v2->z);
    line.rs = FLOAT_TO_FIX_RGB(v2->r);
    line.gs = FLOAT_TO_FIX_RGB(v2->g);
    line.bs = FLOAT_TO_FIX_RGB(v2->b);
    line.as = FLOAT_TO_FIX_RGB(v2->a);

    // Compute delta values for Z and colors 
    line.dzdu = FLOAT_TO_FIX_Z((v1->z - v2->z) * one_du);
    line.drdu = FLOAT_TO_FIX_RGB((v1->r - v2->r) * one_du);
    line.dgdu = FLOAT_TO_FIX_RGB((v1->g - v2->g) * one_du);
    line.dbdu = FLOAT_TO_FIX_RGB((v1->b - v2->b) * one_du);
    line.dadu = FLOAT_TO_FIX_RGB((v1->a - v2->a) * one_du);

    // Now go draw it 

    DrawLine(&line);
}
//============================================================================================================================//
template<class color,int op> inline void aa_line<color,op>::DrawLine (aa_setup_line *line)
//============================================================================================================================//
{
    fix_xy x, y;            // Start value 
    fix_xy dudu;            // Constant 1 or -1 for step 
    fix_xy dx, dy;          // Steps in X and Y 
    fix_z z;
    fix_rgb r, g, b, a;
    fix_xy u_off;           // Offset to starting sample grid 
    fix_xy us, vs, ue;      // Start and end for drawing 
    fix_xy count;           // How many pixels to draw 
    int64s slope_index;     // Index into slope correction table 
    int64s slope;           // Slope correction value 
    int64s ep_corr;         // End-point correction value 
    int64s scount, ecount;  // Start/end count for endpoints 
    int64s sf, ef;          // Sand and end fractions 
    int64s ep_code;         // One of 9 endpoint codes 

    // Get directions 
    if (line->negative)
    dudu = -ONE_XY;
    else
    dudu = ONE_XY;

    if (line->x_major) {
    dx = dudu;
    dy = line->dvdu;
    }
    else {
    dx = line->dvdu;
    dy = dudu;
    }

    // Get initial values and count 
    if (m_Antialiased) {
    // Antialiased 
    if (line->negative) {
        u_off = FRACT_XY(line->us) - ONE_XY;
        us = line->us + ONE_XY;
        ue = line->ue;
        count = FLOOR_XY(us) - FLOOR_XY(ue);
    }
    else {
        u_off = 0 - FRACT_XY(line->us);
        us = line->us;
        ue = line->ue + ONE_XY;
        count = FLOOR_XY(ue) - FLOOR_XY(us);
    }
    }
    else {
    // Jaggy 
    if (line->negative) {
        u_off = FRACT_XY(line->us + ONEHALF_XY) - ONEHALF_XY;
        us = FLOOR_XY(line->us + ONEHALF_XY);
        ue = FLOOR_XY(line->ue - ONEHALF_XY);
        count = us - ue;
    }
    else {
        u_off = ONEHALF_XY - FRACT_XY(line->us - ONEHALF_XY);
        us = FLOOR_XY(line->us + ONEHALF_XY);
        ue = FLOOR_XY(line->ue + ONEHALF_XY + ONE_XY);
        count = ue - us;
    }
    }

    vs = line->vs + Fix_xy_mult(line->dvdu, u_off) + ONEHALF_XY;

    if (line->x_major) {
    x = us;
    y = vs;
    }
    else {
    x = vs;
    y = us;
    }

    z = line->zs + Fix_xy_mult(line->dzdu, u_off);
    r = line->rs + Fix_xy_mult(line->drdu, u_off);
    g = line->gs + Fix_xy_mult(line->dgdu, u_off);
    b = line->bs + Fix_xy_mult(line->dbdu, u_off);
    a = line->as + Fix_xy_mult(line->dadu, u_off);

    if ((m_Antialiased) == 0) {
    // Jaggy line 

    // If not capped, shorten by one 
    if (m_Capline == 0)
        count -= ONE_XY;

    // Interpolate the edges 
    while ((count -= ONE_XY) >= 0) {

        ProcessPixel ( FIX_XY_TO_INT(x), FIX_XY_TO_INT(y), r, g, b, a );

        x += dx;
        y += dy;
        z += line->dzdu;
        r += line->drdu;
        g += line->dgdu;
        b += line->dbdu;
        a += line->dadu;

    } // End of interpolating the line parameters 
    } // End of jaggy line code 

    else {
    // Antialiased line 

    // Compute slope correction once per line 
    slope_index = (line->dvdu >> (FIX_XY_SHIFT - 5)) & 0x3fu;
    if (line->dvdu < 0)
        slope_index ^= 0x3fu;
    if ((slope_index & 0x20u) == 0)
        slope = slope_corr_table[slope_index];
    else
        slope = 0x100;        // True 1.0 

    // Set up counters for determining endpoint regions 
    scount = 0;
    ecount = FIX_TO_INT_XY(count);

    // Get 4-bit fractions for end-point adjustments 
    sf = (us & EP_MASK) >> EP_SHIFT;
    ef = (ue & EP_MASK) >> EP_SHIFT;

    // Interpolate the edges 
    while (count >= 0) {

         // Compute end-point code (defined as follows):
         //  0 =  0, 0: short, no boundary crossing
         //  1 =  0, 1: short line overlap (< 1.0)
         //  2 =  0, 2: 1st pixel of 1st endpoint
         //  3 =  1, 0: short line overlap (< 1.0)
         //  4 =  1, 1: short line overlap (> 1.0)
         //  5 =  1, 2: 2nd pixel of 1st endpoint
         //  6 =  2, 0: last of 2nd endpoint
         //  7 =  2, 1: first of 2nd endpoint
         //  8 =  2, 2: regular part of line
         
        ep_code = ((scount < 2) ? scount : 2) * 3 + ((ecount < 2) ? ecount : 2);
        if (line->negative) {
        // Drawing in the negative direction 

        // Compute endpoint information 
        switch (ep_code) {
          case 0: ep_corr = 0;                break;
          case 1: ep_corr = ((sf - ef) & 0x78) | 4;    break;
          case 2: ep_corr = sf | 4;            break;
          case 3: ep_corr = ((sf - ef) & 0x78) | 4;    break;
          case 4: ep_corr = ((sf - ef) + 0x80) | 4;    break;
          case 5: ep_corr = (sf + 0x80) | 4;        break;
          case 6: ep_corr = (0x78 - ef) | 4;        break;
          case 7: ep_corr = ((0x78 - ef) + 0x80) | 4;    break;
          case 8: ep_corr = 0x100;            break;
        } // End of switch on endpoint type 
        }
        else {
        // Drawing in the positive direction 

        // Compute endpoint information 
        switch (ep_code) {
          case 0: ep_corr = 0;                break;
          case 1: ep_corr = ((ef - sf) & 0x78) | 4;    break;
          case 2: ep_corr = (0x78 - sf) | 4;        break;
          case 3: ep_corr = ((ef - sf) & 0x78) | 4;    break;
          case 4: ep_corr = ((ef - sf) + 0x80) | 4;    break;
          case 5: ep_corr = ((0x78 - sf) + 0x80) | 4;   break;
          case 6: ep_corr = ef | 4;            break;
          case 7: ep_corr = (ef + 0x80) | 4;        break;
          case 8: ep_corr = 0x100;            break;
        } // End of switch on endpoint type 
        }

        if (line->x_major)
        Draw_aa_hspan(x, y, z, r, g, b, ep_corr, slope);
        else
        Draw_aa_vspan(x, y, z, r, g, b, ep_corr, slope);

        x += dx;
        y += dy;
        z += line->dzdu;
        r += line->drdu;
        g += line->dgdu;
        b += line->dbdu;
        a += line->dadu;

        scount++;
        ecount--;
        count -= ONE_XY;

    } // End of interpolating the line parameters 

    } // End of antialiased line code 

} // End of draw_line 
//============================================================================================================================//
template<class color,int op> inline void aa_line<color,op>::Draw_aa_hspan (fix_xy x, fix_xy y, fix_z z,fix_rgb r, fix_rgb g, fix_rgb b, int64s ep_corr, int64s slope)
//============================================================================================================================//
{
    int64s sample_dist;      // Distance from line to sample point 
    int64s filter_index;     // Index into filter table 
    int64s i;                // Count pixels across span 
    int64s index;            // Final filter table index 
    fix_rgb a;               // Alpha 

    sample_dist = (FRACT_XY(y) >> (FIX_XY_SHIFT - 5)) - 16;
    y = y - ONE_XY;
    filter_index = sample_dist + 32;

    for (i = 0; i < 4; i++) {
    if (filter_index < 0)
        index = ~filter_index;    // Invert when negative 
    else
        index = filter_index;
    if (index > 47)
        continue;            // Not a valid pixel 

    a = ((((slope * ep_corr) & 0x1ff00) * filter_table[index]) &
        0xff0000) << (30-8-16); // >> 16;
    // Should include the alpha value as well... 

    ProcessPixel ( FIX_XY_TO_INT(x), FIX_XY_TO_INT(y), r, g, b, a );

    filter_index -= 32;
    y += ONE_XY;
    }
}
//============================================================================================================================//
template<class color,int op> inline void aa_line<color,op>::Draw_aa_vspan (fix_xy x, fix_xy y, fix_z z,fix_rgb r, fix_rgb g, fix_rgb b, int64s ep_corr, int64s slope)
//============================================================================================================================//
{
    int64s sample_dist;      // Distance from line to sample point 
    int64s filter_index;     // Index into filter table 
    int64s i;                // Count pixels across span 
    int64s index;            // Final filter table index 
    fix_rgb a;               // Alpha 

    sample_dist = (FRACT_XY(x) >> (FIX_XY_SHIFT - 5)) - 16;
    x = x - ONE_XY;
    filter_index = sample_dist + 32;

    for (i = 0; i < 4; i++) {
    if (filter_index < 0)
        index = ~filter_index;    // Invert when negative 
    else
        index = filter_index;
    if (index > 47)
        continue;            // Not a valid pixel 

    a = ((((slope * ep_corr) & 0x1ff00) * filter_table[index]) &
        0xff0000) << (30-8-16); // >> 16;
    // Should include the alpha value as well... 

    ProcessPixel ( FIX_XY_TO_INT(x), FIX_XY_TO_INT(y), r, g, b, a );

    filter_index -= 32;
    x += ONE_XY;
    }
}
//============================================================================================================================//
template<class color,int op> inline aa_line<color,op>::aa_line ( double width )
//============================================================================================================================//
{
    static bool _init = false; if(!_init) _init = true, init_tables ( 1 );

    m_Antialiased=1;
    m_Capline=1;
}
//============================================================================================================================//
template<class color,int op> inline void aa_line<color,op>::ProcessPixel ( int x, int y, fix_rgb r, fix_rgb g, fix_rgb b, fix_rgb a )
//============================================================================================================================//
{
    int w = frame.size.x;
    int h = frame.size.y;

    if( x<0 || x>=w || y<0 || y>=h ) return;

    color c = COLOR ( FIX_TO_FLOAT_RGB (r), FIX_TO_FLOAT_RGB (g), FIX_TO_FLOAT_RGB (b) );

    typename traits<color>::alpha::type alpha = traits<color>::alpha::clamp ( FIX_TO_FLOAT_RGB (a) * extra );

    colorop::opp<op>::op ( frame ( x, y ), c, alpha );
}
//============================================================================================================================//
template<class color,int op> int64s aa_line<color,op>::slope_corr_table [SC_TABLE_SIZE];
template<class color,int op> int64s aa_line<color,op>::filter_table  [FI_TABLE_SIZE];
template<class color,int op> int64s aa_line<color,op>::sqrt_table [SR_TABLE_SIZE];
//============================================================================================================================//
template<class color,int op> inline void aa_line<color,op>::draw_line ( FRAME<color> port, double x1, double y1, double x2, double y2, COLOR color1, COLOR color2, double width )
//============================================================================================================================//
{

    aa_line<color,op> al ( width );

    al.extra = color1.a;
    al.frame = port;
    al.DrawLine ( x1-.5, y1-.5, x2-.5, y2-.5, color1, color2 ); // PIXELS' CENTER COORDINATES
}
//============================================================================================================================//
template<class color,int op> inline void aa_line<color,op>::draw_circle ( FRAME<color> port, double x, double y, double r, COLOR co, double a1, double a2, double width )
//============================================================================================================================//
{

    aa_line<color,op> al ( width );
    
    al.extra = co.a;
    al.frame = port;

    aa_vertex v1, v2;
    float cx, cy;            // Center X and Y 
    float sx, sy;            // Scale X and Y 
    float nx, ny;            // New X and Y, from trig functions 
    float x1, y1, x2, y2;    // Computed values 
    float angle;
    float pi = float(3.1415926535);

    v2.z = v1.z = 1.0;
    v2.r = v1.r = (float) co.v.x;
    v2.g = v1.g = (float) co.v.y;
    v2.b = v1.b = (float) co.v.z;
    v2.a = v1.a = (float) co.a;
    cx = float(x);
    cy = float(y);
    sx = float(r);
    sy = float(r);
    x1 =   (float) cos (a1) * sx + cx;
    y1 = - (float) sin (a1) * sy + cy;

    for(angle = float(a1); angle <= float(a2); angle += float(2*pi/180))
    {
        nx =   (float) cos (angle);
        ny = - (float) sin (angle);
        x2 = nx * sx + cx;
        y2 = ny * sy + cy;
        v1.x = x1;
        v1.y = y1;
        v2.x = x2;
        v2.y = y2;
        x1 = x2;
        y1 = y2;

        al.SetupLine(&v2, &v1);
    }
}
//============================================================================================================================//









