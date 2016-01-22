//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"

extern "C" {
#ifndef _WIN32
 #define HAVE_LIBFREETYPE
#endif
#include "gdc.h"
#include "gdchart.h"
#include "gdcpie.h"

int
out_graph(short       GIFWIDTH,   /* no check for a gif that's too small to fit */
          short       GIFHEIGHT,  /* needed info (labels, etc), could core dump */
          FILE        *gif_fptr,  // open file pointer (gif out)
          GDC_CHART_T type,
          int         num_points, // points along x axis (even iterval)
                                  //	all arrays dependant on this
          char        *xlbl[],    // array of xlabels
          int         num_sets,
          ...);
}
//-------------------------//
// Local variables         //
//-------------------------//
INVOKE_CALL InvokePtr = 0;
//-----------------------------------------------------//

CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;

#ifdef WIN32
    AnsiString path = getenv("GDFONTPATH");
    if (!path.Length()) {
        AnsiString windir = getenv("windir");
        if (windir.Length()) {
            _putenv(AnsiString("GDFONTPATH=") + windir + "\\Fonts");
        }
    }
#endif
    GDC_image_type = GDC_GIF;

    DEFINE_ECONSTANT(GDC_LINE)
    DEFINE_ECONSTANT(GDC_AREA)
    DEFINE_ECONSTANT(GDC_BAR)
    DEFINE_ECONSTANT(GDC_HILOCLOSE)
    DEFINE_ECONSTANT(GDC_COMBO_LINE_BAR)
    DEFINE_ECONSTANT(GDC_COMBO_HLC_BAR)
    DEFINE_ECONSTANT(GDC_COMBO_LINE_AREA)
    DEFINE_ECONSTANT(GDC_COMBO_HLC_AREA)
    DEFINE_ECONSTANT(GDC_3DHILOCLOSE)
    DEFINE_ECONSTANT(GDC_3DCOMBO_LINE_BAR)
    DEFINE_ECONSTANT(GDC_3DCOMBO_LINE_AREA)
    DEFINE_ECONSTANT(GDC_3DCOMBO_HLC_BAR)
    DEFINE_ECONSTANT(GDC_3DCOMBO_HLC_AREA)
    DEFINE_ECONSTANT(GDC_3DBAR)
    DEFINE_ECONSTANT(GDC_3DAREA)
    DEFINE_ECONSTANT(GDC_3DLINE)

    DEFINE_ECONSTANT(GDC_GIF)
#ifdef HAVE_JPEG
    DEFINE_ECONSTANT(GDC_JPEG)
#endif
    DEFINE_ECONSTANT(GDC_PNG)
    DEFINE_ECONSTANT(GDC_WBMP)

    DEFINE_ECONSTANT(GDC_STACK_DEPTH)
    DEFINE_ECONSTANT(GDC_STACK_SUM)
    DEFINE_ECONSTANT(GDC_STACK_BESIDE)
    DEFINE_ECONSTANT(GDC_STACK_LAYER)

    DEFINE_ECONSTANT(GDC_HLC_DIAMOND)
    DEFINE_ECONSTANT(GDC_HLC_CLOSE_CONNECTED)
    DEFINE_ECONSTANT(GDC_HLC_CONNECTING)
    DEFINE_ECONSTANT(GDC_HLC_I_CAP)

    DEFINE_ECONSTANT(GDC_SCATTER_TRIANGLE_DOWN)
    DEFINE_ECONSTANT(GDC_SCATTER_TRIANGLE_UP)

    DEFINE_ECONSTANT(GDC_3DPIE)
    DEFINE_ECONSTANT(GDC_2DPIE)

    DEFINE_ECONSTANT(GDCPIE_PCT_NONE)
    DEFINE_ECONSTANT(GDCPIE_PCT_ABOVE)
    DEFINE_ECONSTANT(GDCPIE_PCT_BELOW)
    DEFINE_ECONSTANT(GDCPIE_PCT_RIGHT)
    DEFINE_ECONSTANT(GDCPIE_PCT_LEFT)

    DEFINE_ECONSTANT(GDC_TINY)
    DEFINE_ECONSTANT(GDC_SMALL)
    DEFINE_ECONSTANT(GDC_MEDBOLD)
    DEFINE_ECONSTANT(GDC_LARGE)
    DEFINE_ECONSTANT(GDC_GIANT)
    DEFINE_ECONSTANT(GDC_numfonts)
    return 0;
}
//-----------------------------------------------------//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    if (GDCPIE_explode)
        delete[] (int *)GDCPIE_explode;
    if (GDCPIE_Color)
        delete[] (int *)GDCPIE_Color;
    if (GDCPIE_missing)
        delete[] (bool *)GDCPIE_missing;
    if (GDC_ExtVolColor)
        delete[] (int *)GDC_ExtVolColor;
    if (GDC_SetColor)
        delete[] (int *)GDC_SetColor;
    if (GDC_ExtColor)
        delete[] (int *)GDC_ExtColor;

    return 0;
}
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDCPIE_BGColor, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDCPIE_BGColor = PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDCPIE_PlotColor, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDCPIE_PlotColor = PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDCPIE_LineColor, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDCPIE_LineColor = PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDCPIE_EdgeColor, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDCPIE_EdgeColor = PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDCPIE_other_threshold, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDCPIE_other_threshold = (char)PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDCPIE_3d_angle, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDCPIE_3d_angle = (short)PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDCPIE_3d_depth, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDCPIE_3d_depth = (short)PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDCPIE_label_dist, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDCPIE_label_dist = (int)PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
AnsiString str;
CONCEPT_FUNCTION_IMPL(_GDCPIE_title, 1)
    T_STRING(0)

    str = PARAM(0);

    GDCPIE_title = str.c_str();

    RETURN_STRING(str.c_str());
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDCPIE_label_line, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDCPIE_label_line = (unsigned char)PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDCPIE_title_size, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDCPIE_title_size = (GDC_font_size)PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDCPIE_label_size, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDCPIE_label_size = (GDC_font_size)PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDCPIE_percent_labels, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDCPIE_percent_labels = (GDCPIE_PCT_TYPE)PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDCPIE_explode, 1)
    T_ARRAY(0)

    if (GDCPIE_explode)
        delete[] (int *)GDCPIE_explode;
    GDCPIE_explode = GetIntList(PARAMETER(0), Invoke);

    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDCPIE_Color, 1)
    T_ARRAY(0)

    if (GDCPIE_Color)
        delete[] (int *)GDCPIE_Color;
    GDCPIE_Color = (unsigned long *)GetIntList(PARAMETER(0), Invoke);

    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDCPIE_missing, 1)
    T_ARRAY(0)

    if (GDCPIE_missing)
        delete[] (bool *)GDCPIE_missing;

    GDCPIE_missing = (unsigned char *)GetBoolList(PARAMETER(0), Invoke);

    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
AnsiString yt;
CONCEPT_FUNCTION_IMPL(_GDC_ytitle, 1)
    T_STRING(0)

    yt         = PARAM(0);
    GDC_ytitle = yt.c_str();

    RETURN_STRING(yt.c_str())
END_IMPL
//-----------------------------------------------------//
AnsiString xt;
CONCEPT_FUNCTION_IMPL(_GDC_xtitle, 1)
    T_STRING(0)

    xt         = PARAM(0);
    GDC_xtitle = xt.c_str();

    RETURN_STRING(xt.c_str())
END_IMPL
//-----------------------------------------------------//
AnsiString yt2;
CONCEPT_FUNCTION_IMPL(_GDC_ytitle2, 1)
    T_STRING(0)

    yt2         = PARAM(0);
    GDC_ytitle2 = yt2.c_str();

    RETURN_STRING(yt2.c_str())
END_IMPL
//-----------------------------------------------------//
AnsiString title;
CONCEPT_FUNCTION_IMPL(_GDC_title, 1)
    T_STRING(0)

    title     = PARAM(0);
    GDC_title = title.c_str();

    RETURN_STRING(title.c_str())
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_title_size, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDC_title_size = (GDC_font_size)PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_xtitle_size, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDC_xtitle_size = (GDC_font_size)PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_ytitle_size, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDC_ytitle_size = (GDC_font_size)PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_yaxisfont_size, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDC_yaxisfont_size = (GDC_font_size)PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_xaxis_angle, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDC_xaxis_angle = PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_xaxisfont_size, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDC_xaxisfont_size = (GDC_font_size)PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
AnsiString ylabel_fmt;
CONCEPT_FUNCTION_IMPL(_GDC_ylabel_fmt, 1)
    T_STRING(0)

    ylabel_fmt     = PARAM(0);
    GDC_ylabel_fmt = ylabel_fmt.c_str();

    RETURN_STRING(ylabel_fmt.c_str())
END_IMPL
//-----------------------------------------------------//
AnsiString ylabel2_fmt;
CONCEPT_FUNCTION_IMPL(_GDC_ylabel2_fmt, 1)
    T_STRING(0)

    ylabel2_fmt     = PARAM(0);
    GDC_ylabel2_fmt = ylabel2_fmt.c_str();

    RETURN_STRING(ylabel2_fmt.c_str())
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_xlabel_spacing, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDC_xlabel_spacing = (short)PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_ylabel_density, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDC_ylabel_density = (char)PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_requested_ymin, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDC_requested_ymin = (float)PARAM(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_requested_ymax, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDC_requested_ymax = (float)PARAM(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_requested_yinterval, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDC_requested_yinterval = (float)PARAM(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_0Shelf, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDC_0Shelf = (char)PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_grid, 1)
    T_NUMBER(0)
//#ifndef _WIN32
    RETURN_NUMBER(GDC_grid = (GDC_TICK_T)PARAM_INT(0));

/*#else
    RETURN_NUMBER(GDC_grid=(char)PARAM_INT(0));
 #endif*/
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_xaxis, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDC_xaxis = (char)PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_yaxis, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDC_yaxis = (char)PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_yaxis2, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDC_yaxis2 = (char)PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_yval_style, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDC_yval_style = (char)PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_stack_type, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDC_stack_type = (GDC_STACK_T)PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_3d_depth, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDC_3d_depth = (float)PARAM(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_3d_angle, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDC_3d_angle = (unsigned char)PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_bar_width, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDC_bar_width = (unsigned char)PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_HLC_style, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDC_HLC_style = (GDC_HLC_STYLE_T)PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_HLC_cap_width, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDC_HLC_cap_width = (unsigned char)PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
//EXTERND GDC_ANNOTATION_T	*GDC_annotation		DEFAULTO( (GDC_ANNOTATION_T*)NULL );
CONCEPT_FUNCTION_IMPL(_GDC_annotation_font, 1)
//T_NUMBER(0)
    T_STRING(0)
//#ifndef _WIN32
    RETURN_STRING((char *)(GDC_annotation_font = PARAM(0)));

/*#else
    RETURN_STRING(AnsiString(GDC_annotation_font=(GDC_font_size)AnsiString(PARAM(0)).ToInt()));
 #endif*/
END_IMPL

//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDCPIE_title_font, 1)
    T_STRING(0)

    RETURN_STRING((char *)(GDCPIE_title_font = PARAM(0)));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDCPIE_label_font, 1)
    T_STRING(0)

    RETURN_STRING((char *)(GDCPIE_label_font = PARAM(0)));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDCPIE_title_ptsize, 1)
    T_NUMBER(0)

    RETURN_NUMBER((INTEGER)(GDCPIE_title_ptsize = PARAM(0)));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDCPIE_label_ptsize, 1)
    T_NUMBER(0)

    RETURN_NUMBER((INTEGER)(GDCPIE_label_ptsize = PARAM(0)));
END_IMPL
//-----------------------------------------------------//

CONCEPT_FUNCTION_IMPL(_GDC_annotation_ptsize, 1)
    T_NUMBER(0)

    RETURN_NUMBER((INTEGER)(GDC_annotation_ptsize = PARAM(0)));
END_IMPL
//-----------------------------------------------------//

CONCEPT_FUNCTION_IMPL(_GDC_image_type, 1)
    T_NUMBER(0)

    RETURN_NUMBER((INTEGER)(GDC_image_type = (GDC_image_type_t)PARAM_INT(0)));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_title_ptsize, 1)
    T_NUMBER(0)

    RETURN_NUMBER((INTEGER)(GDC_title_ptsize = PARAM(0)));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_title_font, 1)
    T_STRING(0)

    RETURN_STRING((char *)(GDC_title_font = PARAM(0)));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_ytitle_ptsize, 1)
    T_NUMBER(0)

    RETURN_NUMBER((INTEGER)(GDC_ytitle_ptsize = PARAM(0)));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_ytitle_font, 1)
    T_STRING(0)

    RETURN_STRING((char *)(GDC_ytitle_font = PARAM(0)));
    GDC_ytitle_ptsize = 12;
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_xtitle_ptsize, 1)
    T_NUMBER(0)

    RETURN_NUMBER((INTEGER)(GDC_xtitle_ptsize = PARAM(0)));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_xtitle_font, 1)
    T_STRING(0)

    RETURN_STRING((char *)(GDC_xtitle_font = PARAM(0)));
    GDC_xtitle_ptsize = 12;
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_yaxis_ptsize, 1)
    T_NUMBER(0)

    RETURN_NUMBER((INTEGER)(GDC_yaxis_ptsize = PARAM(0)));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_yaxis_font, 1)
    T_STRING(0)

    RETURN_STRING((char *)(GDC_yaxis_font = PARAM(0)));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_xaxis_ptsize, 1)
    T_NUMBER(0)

    RETURN_NUMBER((INTEGER)(GDC_xaxis_ptsize = PARAM(0)));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_xaxis_font, 1)
    T_STRING(0)

    RETURN_STRING((char *)(GDC_xaxis_font = PARAM(0)));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_num_scatter_pts, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDC_num_scatter_pts = (int)PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
//EXTERND GDC_SCATTER_T		*GDC_scatter		DEFAULTO( (GDC_SCATTER_T*)NULL );
CONCEPT_FUNCTION_IMPL(_GDC_thumbnail, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDC_thumbnail = (char)PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
AnsiString thumblabel;
CONCEPT_FUNCTION_IMPL(_GDC_thumblabel, 1)
    T_STRING(0)

    thumblabel     = PARAM(0);
    GDC_thumblabel = thumblabel.c_str();

    RETURN_STRING(thumblabel.c_str())
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_thumbval, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDC_thumbval = (float)PARAM(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_border, 1)
    T_NUMBER(0)
//#ifndef _WIN32
    RETURN_NUMBER(GDC_border = (GDC_BORDER_T)PARAM_INT(0));

/*#else
    RETURN_NUMBER(GDC_border=(char)PARAM_INT(0));
 #endif*/
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_BGColor, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDC_BGColor = PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_GridColor, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDC_GridColor = PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_LineColor, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDC_LineColor = PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_PlotColor, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDC_PlotColor = PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_VolColor, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDC_VolColor = PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_TitleColor, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDC_TitleColor = PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_XTitleColor, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDC_XTitleColor = PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_YTitleColor, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDC_YTitleColor = PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_YTitle2Color, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDC_YTitle2Color = PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_XLabelColor, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDC_XLabelColor = PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_YLabelColor, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDC_YLabelColor = PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_YLabel2Color, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDC_YLabel2Color = PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_ExtVolColor, 1)
    T_ARRAY(0)

    if (GDC_ExtVolColor)
        delete[] (int *)GDC_ExtVolColor;

    GDC_ExtVolColor = (unsigned long *)GetIntList(PARAMETER(0), Invoke);

    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_SetColor, 1)
    T_ARRAY(0)

    if (GDC_SetColor)
        delete[] (int *)GDC_SetColor;

    GDC_SetColor = (unsigned long *)GetIntList(PARAMETER(0), Invoke);

    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_ExtColor, 1)
    T_ARRAY(0)

    if (GDC_ExtColor)
        delete[] (int *)GDC_ExtColor;

    GDC_ExtColor = (unsigned long *)GetIntList(PARAMETER(0), Invoke);

    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_GDC_transparent_bg, 1)
    T_NUMBER(0)
    RETURN_NUMBER(GDC_transparent_bg = (char)PARAM_INT(0));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_pie_gif, 7)
    T_NUMBER(0)
    T_NUMBER(1)
    T_NUMBER(2)
    T_NUMBER(3)
    T_NUMBER(4)
    T_ARRAY(5)
    T_ARRAY(6)

    char **labels = GetCharList(PARAMETER(5), Invoke);
    float *data = GetFloatList(PARAMETER(6), Invoke);

    pie_gif((short)PARAM_INT(0),  // width
            (short)PARAM_INT(1),  // height
            (FILE *)PARAM_INT(2), // file *
            (GDCPIE_TYPE)PARAM_INT(3),
            PARAM_INT(4),         // num_points
            labels,
            data
            );

    delete[] labels;
    delete[] data;
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_VARIABLE_PARAMS(_out_graph, 7)
    T_NUMBER(0)
    T_NUMBER(1)
    T_NUMBER(2)
    T_NUMBER(3)
    T_NUMBER(4)
    T_ARRAY(5)
    T_NUMBER(6)

    short gifwidth = (short)PARAM_INT(0);
    short       gifheight  = (short)PARAM_INT(1);
    FILE        *gif_fptr  = (FILE *)PARAM_INT(2);
    GDC_CHART_T type       = (GDC_CHART_T)PARAM_INT(3);
    int         num_points = PARAM_INT(4);
    char        **xlbl     = GetCharList(PARAMETER(5), Invoke);
    int         num_sets   = PARAM_INT(6);

    switch (type) {
        case GDC_LINE:
        case GDC_BAR:
        case GDC_3DBAR:
        case GDC_3DAREA:
        case GDC_AREA:
            {
                float *vals[100];
                int   params = (7 + num_sets);
                PARAMETERS_CHECK_MIN(params, "out_graph optional parameters count missmatch");

                if (num_sets > 100)
                    return (void *)"out_graph: too many num_sets. Max 100";

                for (int i = 0; i < num_sets; i++) {
                    int param = 7 + i;
                    T_ARRAY(param)

                    vals[i] = GetFloatList(PARAMETER(7 + i), Invoke);
                }

                out_graph(gifwidth, gifheight, gif_fptr, type, num_points, xlbl, num_sets,
                          vals[0], vals[1], vals[2], vals[3], vals[4], vals[5], vals[6], vals[7], vals[8], vals[9],
                          vals[10], vals[11], vals[12], vals[13], vals[14], vals[15], vals[16], vals[17], vals[18], vals[19],
                          vals[20], vals[21], vals[22], vals[23], vals[24], vals[25], vals[26], vals[27], vals[28], vals[29],
                          vals[30], vals[31], vals[32], vals[33], vals[34], vals[35], vals[36], vals[37], vals[38], vals[39],
                          vals[40], vals[41], vals[42], vals[43], vals[44], vals[45], vals[46], vals[47], vals[48], vals[49],
                          vals[50], vals[51], vals[52], vals[53], vals[54], vals[55], vals[56], vals[57], vals[58], vals[59],
                          vals[60], vals[61], vals[62], vals[63], vals[64], vals[65], vals[66], vals[67], vals[68], vals[69],
                          vals[70], vals[71], vals[72], vals[73], vals[74], vals[75], vals[76], vals[77], vals[78], vals[79],
                          vals[80], vals[81], vals[82], vals[83], vals[84], vals[85], vals[86], vals[87], vals[88], vals[89],
                          vals[90], vals[91], vals[92], vals[93], vals[94], vals[95], vals[96], vals[97], vals[98], vals[99]
                          );
                for (int i = 0; i < num_sets; i++) {
                    delete[] vals[i];
                }
            }
            break;

        case GDC_HILOCLOSE:
            {
                float *vals[100];
                int   total  = (num_sets * 2) + 1; //*2;
                int   params = (7 + total);
                PARAMETERS_CHECK_MIN(params, "out_graph optional parameters count missmatch");

                if (total > 100)
                    return (void *)"out_graph: too many num_sets. Max 100";

                for (int i = 0; i < total; i++) {
                    int param = 7 + i;
                    T_ARRAY(param)

                    vals[i] = GetFloatList(PARAMETER(7 + i), Invoke);
                }

                out_graph(gifwidth, gifheight, gif_fptr, type, num_points, xlbl, num_sets,
                          vals[0], vals[1], vals[2], vals[3], vals[4], vals[5], vals[6], vals[7], vals[8], vals[9],
                          vals[10], vals[11], vals[12], vals[13], vals[14], vals[15], vals[16], vals[17], vals[18], vals[19],
                          vals[20], vals[21], vals[22], vals[23], vals[24], vals[25], vals[26], vals[27], vals[28], vals[29],
                          vals[30], vals[31], vals[32], vals[33], vals[34], vals[35], vals[36], vals[37], vals[38], vals[39],
                          vals[40], vals[41], vals[42], vals[43], vals[44], vals[45], vals[46], vals[47], vals[48], vals[49],
                          vals[50], vals[51], vals[52], vals[53], vals[54], vals[55], vals[56], vals[57], vals[58], vals[59],
                          vals[60], vals[61], vals[62], vals[63], vals[64], vals[65], vals[66], vals[67], vals[68], vals[69],
                          vals[70], vals[71], vals[72], vals[73], vals[74], vals[75], vals[76], vals[77], vals[78], vals[79],
                          vals[80], vals[81], vals[82], vals[83], vals[84], vals[85], vals[86], vals[87], vals[88], vals[89],
                          vals[90], vals[91], vals[92], vals[93], vals[94], vals[95], vals[96], vals[97], vals[98], vals[99]
                          );

                for (int i = 0; i < total; i++) {
                    delete[] vals[i];
                }
            }
            break;

        case GDC_COMBO_LINE_BAR:
        case GDC_COMBO_LINE_AREA:
            {
                float *vals[100];
                int   total  = num_sets + 1; //*2;
                int   params = (7 + total);
                PARAMETERS_CHECK_MIN(params, "out_graph optional parameters count missmatch");

                if (total > 100)
                    return (void *)"out_graph: too many num_sets. Max 100";

                for (int i = 0; i < total; i++) {
                    int param = 7 + i;
                    T_ARRAY(param)

                    vals[i] = GetFloatList(PARAMETER(7 + i), Invoke);
                }

                out_graph(gifwidth, gifheight, gif_fptr, type, num_points, xlbl, num_sets,
                          vals[0], vals[1], vals[2], vals[3], vals[4], vals[5], vals[6], vals[7], vals[8], vals[9],
                          vals[10], vals[11], vals[12], vals[13], vals[14], vals[15], vals[16], vals[17], vals[18], vals[19],
                          vals[20], vals[21], vals[22], vals[23], vals[24], vals[25], vals[26], vals[27], vals[28], vals[29],
                          vals[30], vals[31], vals[32], vals[33], vals[34], vals[35], vals[36], vals[37], vals[38], vals[39],
                          vals[40], vals[41], vals[42], vals[43], vals[44], vals[45], vals[46], vals[47], vals[48], vals[49],
                          vals[50], vals[51], vals[52], vals[53], vals[54], vals[55], vals[56], vals[57], vals[58], vals[59],
                          vals[60], vals[61], vals[62], vals[63], vals[64], vals[65], vals[66], vals[67], vals[68], vals[69],
                          vals[70], vals[71], vals[72], vals[73], vals[74], vals[75], vals[76], vals[77], vals[78], vals[79],
                          vals[80], vals[81], vals[82], vals[83], vals[84], vals[85], vals[86], vals[87], vals[88], vals[89],
                          vals[90], vals[91], vals[92], vals[93], vals[94], vals[95], vals[96], vals[97], vals[98], vals[99]
                          );

                for (int i = 0; i < total; i++) {
                    delete[] vals[i];
                }
            }
            break;

        case GDC_COMBO_HLC_BAR:
        case GDC_COMBO_HLC_AREA:
            {
                float *vals[100];
                int   total  = num_sets + 3; //*2;
                int   params = (7 + total);
                PARAMETERS_CHECK_MIN(params, "out_graph optional parameters count missmatch");

                if (total > 100)
                    return (void *)"out_graph: too many num_sets. Max 100";

                for (int i = 0; i < total; i++) {
                    int param = 7 + i;
                    T_ARRAY(param)

                    vals[i] = GetFloatList(PARAMETER(7 + i), Invoke);
                }

                out_graph(gifwidth, gifheight, gif_fptr, type, num_points, xlbl, num_sets,
                          vals[0], vals[1], vals[2], vals[3], vals[4], vals[5], vals[6], vals[7], vals[8], vals[9],
                          vals[10], vals[11], vals[12], vals[13], vals[14], vals[15], vals[16], vals[17], vals[18], vals[19],
                          vals[20], vals[21], vals[22], vals[23], vals[24], vals[25], vals[26], vals[27], vals[28], vals[29],
                          vals[30], vals[31], vals[32], vals[33], vals[34], vals[35], vals[36], vals[37], vals[38], vals[39],
                          vals[40], vals[41], vals[42], vals[43], vals[44], vals[45], vals[46], vals[47], vals[48], vals[49],
                          vals[50], vals[51], vals[52], vals[53], vals[54], vals[55], vals[56], vals[57], vals[58], vals[59],
                          vals[60], vals[61], vals[62], vals[63], vals[64], vals[65], vals[66], vals[67], vals[68], vals[69],
                          vals[70], vals[71], vals[72], vals[73], vals[74], vals[75], vals[76], vals[77], vals[78], vals[79],
                          vals[80], vals[81], vals[82], vals[83], vals[84], vals[85], vals[86], vals[87], vals[88], vals[89],
                          vals[90], vals[91], vals[92], vals[93], vals[94], vals[95], vals[96], vals[97], vals[98], vals[99]
                          );

                for (int i = 0; i < total; i++) {
                    delete[] vals[i];
                }
            }
            break;

        default:
            return (void *)"out_graph: invalid graph type";
    }

    if (xlbl)
        delete[] xlbl;

    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(_out_err, 6)
    T_NUMBER(0)
    T_NUMBER(1)
    T_NUMBER(2)
    T_NUMBER(3)
    T_NUMBER(4)
    T_STRING(5)

    out_err(PARAM_INT(0), PARAM_INT(1), (FILE *)PARAM_INT(2), PARAM_INT(3), PARAM_INT(4), PARAM(5));

    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
