//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
extern "C" {
#include "svgtiny.h"
#include <cairo.h>
}
//-------------------------//
// Local variables         //
//-------------------------//
INVOKE_CALL InvokePtr = 0;
//-----------------------------------------------------//
#ifdef _WIN32
xmlFreeFunc xmlFree = 0;
#endif
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
#ifdef _WIN32
    xmlMallocFunc  xmlMalloc  = 0;
    xmlReallocFunc xmlRealloc = 0;
    if (!xmlFree)
        xmlMemGet(&xmlFree, &xmlMalloc, &xmlRealloc, NULL);
#endif
    InvokePtr = Invoke;
    DEFINE_ICONSTANT("SVGT_OK", svgtiny_OK)
    DEFINE_ICONSTANT("SVGT_OUT_OF_MEMORY", svgtiny_OUT_OF_MEMORY)
    DEFINE_ICONSTANT("SVGT_LIBXML_ERROR", svgtiny_LIBXML_ERROR)
    DEFINE_ICONSTANT("SVGT_NOT_SVG", svgtiny_NOT_SVG)
    DEFINE_ICONSTANT("SVG_ERROR", svgtiny_SVG_ERROR)
    DEFINE_ICONSTANT("SVG_PATH_MOVE", svgtiny_PATH_MOVE)
    DEFINE_ICONSTANT("SVG_PATH_CLOSE", svgtiny_PATH_CLOSE)
    DEFINE_ICONSTANT("SVG_PATH_LINE", svgtiny_PATH_LINE)
    DEFINE_ICONSTANT("SVG_PATH_BEZIER", svgtiny_PATH_BEZIER)

    return 0;
}
//-----------------------------------------------------//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//-----------------------------------------------------//
void render_path(cairo_t *cr, float scale, struct svgtiny_shape *path) {
    unsigned int j;

    cairo_new_path(cr);
    for (j = 0; j != path->path_length; ) {
        switch ((int)path->path[j]) {
            case svgtiny_PATH_MOVE:
                cairo_move_to(cr,
                              scale * path->path[j + 1],
                              scale * path->path[j + 2]);
                j += 3;
                break;

            case svgtiny_PATH_CLOSE:
                cairo_close_path(cr);
                j += 1;
                break;

            case svgtiny_PATH_LINE:
                cairo_line_to(cr,
                              scale * path->path[j + 1],
                              scale * path->path[j + 2]);
                j += 3;
                break;

            case svgtiny_PATH_BEZIER:
                cairo_curve_to(cr,
                               scale * path->path[j + 1],
                               scale * path->path[j + 2],
                               scale * path->path[j + 3],
                               scale * path->path[j + 4],
                               scale * path->path[j + 5],
                               scale * path->path[j + 6]);
                j += 7;
                break;

            default:
                j += 1;
        }
    }
    if (path->fill != svgtiny_TRANSPARENT) {
        cairo_set_source_rgb(cr,
                             svgtiny_RED(path->fill) / 255.0,
                             svgtiny_GREEN(path->fill) / 255.0,
                             svgtiny_BLUE(path->fill) / 255.0);
        cairo_fill_preserve(cr);
    }
    if (path->stroke != svgtiny_TRANSPARENT) {
        cairo_set_source_rgb(cr,
                             svgtiny_RED(path->stroke) / 255.0,
                             svgtiny_GREEN(path->stroke) / 255.0,
                             svgtiny_BLUE(path->stroke) / 255.0);
        cairo_set_line_width(cr, scale * path->stroke_width);
        cairo_stroke_preserve(cr);
    }
}

//-----------------------------------------------------//
static cairo_status_t rsvg_cairo_write_func(void *closure, const unsigned char *data, unsigned int length) {
    AnsiString *bufdata = (AnsiString *)closure;

    bufdata->AddBuffer((char *)data, length);
    return CAIRO_STATUS_SUCCESS;
}

//-----------------------------------------------------//
int create_cairo(svgtiny_diagram *diagram, float scale, AnsiString *res) {
    cairo_surface_t *surface;
    cairo_t         *cr;
    cairo_status_t  status;
    unsigned int    i;


    surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, (int)(diagram->width * scale), (int)(diagram->height * scale));

    if (!surface)
        return 0;

    cr     = cairo_create(surface);
    status = cairo_status(cr);

    if (status != CAIRO_STATUS_SUCCESS) {
        cairo_destroy(cr);
        cairo_surface_destroy(surface);
        return 0;
    }

    cairo_set_source_rgba(cr, 1, 1, 1, 0);
    cairo_paint(cr);

    for (i = 0; i != diagram->shape_count; i++) {
        if (diagram->shape[i].path) {
            render_path(cr, scale, &diagram->shape[i]);
        } else if (diagram->shape[i].text) {
            AnsiString text;
            char       *ptr = diagram->shape[i].text;
            while (*ptr) {
                switch (*ptr) {
                    case '\n':
                    case '\r':
                    case '\t':
                        if (text.Length())
                            text += ' ';
                        break;

                    default:
                        text += *ptr;
                }
                ptr++;
            }
            ptr = text.c_str();

            cairo_set_source_rgb(cr,
                                 svgtiny_RED(diagram->shape[i].stroke) / 255.0,
                                 svgtiny_GREEN(diagram->shape[i].stroke) / 255.0,
                                 svgtiny_BLUE(diagram->shape[i].stroke) / 255.0);
            cairo_move_to(cr,
                          scale * diagram->shape[i].text_x,
                          scale * diagram->shape[i].text_y);

            cairo_show_text(cr, ptr);
        }
    }

    status = cairo_status(cr);
    if (status != CAIRO_STATUS_SUCCESS) {
        cairo_destroy(cr);
        cairo_surface_destroy(surface);
        return 0;
    }

    cairo_surface_write_to_png_stream(surface, rsvg_cairo_write_func, res);
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
    return 1;
}

//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(SVGT, 1, 2)
    T_STRING(0)



    float scale = 1;
    if (PARAMETERS_COUNT > 1) {
        T_NUMBER(1)
        scale = PARAM(1);
    }


    svgtiny_diagram *diagram = svgtiny_create();
    if (!diagram) {
        RETURN_NUMBER(svgtiny_OUT_OF_MEMORY);
        return 0;
    }

    svgtiny_code code = svgtiny_parse(diagram, PARAM(0), PARAM_LEN(0), "in.svg", 100000, 100000);
    if (code != svgtiny_OK) {
        RETURN_NUMBER(code);
        return 0;
    }

    AnsiString res;
    if (!create_cairo(diagram, scale, &res)) {
        RETURN_NUMBER(-1);
        return 0;
    }

    svgtiny_free(diagram);
    RETURN_BUFFER(res.c_str(), res.Length());
END_IMPL
//-----------------------------------------------------//
