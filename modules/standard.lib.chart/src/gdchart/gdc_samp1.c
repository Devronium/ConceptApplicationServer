/* GDCHART 0.10.0dev  1st CHART SAMPLE  2 Nov 2000 */
/* Copyright Bruce Verderaime 1998-2004 */

/* writes gif file to stdout */

/* sample gdchart usage */
/* this will produce a 3D BAR chart */
/* this is suitable for use as a CGI */

/* for CGI use un-comment the "Content-Type" line */

#include <stdio.h>

#include "gdc.h"
#include "gdchart.h"

main() {
    /* ----- set some data ----- */
/*    float   a[6]  = { 0.5, 0.09, 0.6, 0.85, 0.0, 0.90 }, */
/*            b[6]  = { 1.9, 1.3,  0.6, 0.75, 0.1, 2.0 }; */
    float a[2][6] = { { 0.5, 0.09, 0.6, 0.85, 0.0, 0.90 },
                      { 1.9,  1.3, 0.6, 0.75, 0.1,  2.0 } };

    /* ----- X labels ----- */
    char *t[6] = { "Chicago", "New York", "L.A.", "Atlanta", "Paris, MD\n(USA) ", "London" };
    /* ----- data set colors (RGB) ----- */
    unsigned long sc[2] = { 0xFF8080, 0x8080FF };

    GDC_BGColor   = 0xFFFFFFL;                  /* backgound color (white) */
    GDC_LineColor = 0x000000L;                  /* line color      (black) */
    GDC_SetColor  = &(sc[0]);                   /* assign set colors */

/*    printf( "Content-Type: image/gif\n\n" );     tell browser type */
    GDC_image_type = GDC_GIF;
    /* ----- call the lib ----- */
    GDC_out_graph(250, 200,          /* short       width, height */
                  stdout,            /* FILE*       open FILE pointer */
                  GDC_3DBAR,         /* GDC_CHART_T chart type */
                  6,                 /* int         number of points per data set */
                  t,                 /* char*[]     array of X labels */
                  2,                 /* int         number of data sets */
                  (float *)a, NULL); /* float[]     data set 1 */
/*               b );             ...        data set n */

    exit(0);
}
