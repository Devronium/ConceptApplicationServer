//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
#ifdef NO_DEPENDENCIES
 #include <stdlib.h>
 #define STB_IMAGE_WRITE_IMPLEMENTATION
 #include "stb_image_write.h"
 #define NANOSVG_IMPLEMENTATION
 #include "nanosvg.h"
 #define NANOSVGRAST_IMPLEMENTATION
 #include "nanosvgrast.h"
#else
 #include <glib-object.h>
 #include <librsvg/rsvg.h>
#endif
//-------------------------//
// Local variables         //
//-------------------------//
INVOKE_CALL InvokePtr = 0;
//-----------------------------------------------------//
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;
#ifndef NO_DEPENDENCIES
    #if !GLIB_CHECK_VERSION(2, 35, 0)
        g_type_init();
    #endif
    rsvg_init();
#endif
    return 0;
}
//-----------------------------------------------------//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
#ifndef NO_DEPENDENCIES
    rsvg_term ();
#endif
    return 0;
}
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(SVG, 1, 4)
    T_STRING(SVG, 0)
    char *type = "png";
    double dpi = -1;
    if (PARAMETERS_COUNT > 1) {
        T_STRING(SVG, 1)
        type = PARAM(1);
    }
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(SVG, 2)
        dpi = PARAM(2);
    }
    if (PARAMETERS_COUNT > 3) {
        SET_STRING(3, "");
    }
#ifdef NO_DEPENDENCIES
    NSVGimage      *image = NULL;
    NSVGrasterizer *rast  = NULL;
    if (dpi <= 0)
        dpi = 96.0f;
    image = nsvgParse(PARAM(0), "px", dpi);
    int w, h;
    if (!image) {
        if (PARAMETERS_COUNT > 3) {
            SET_STRING(3, "Cannot parse SVG image");
        }
        RETURN_STRING("");
        return 0;
    }
    w = (int)image->width;
    h = (int)image->height;

    rast = nsvgCreateRasterizer();
    unsigned char *img = (unsigned char *)malloc(w * h * 4);
    nsvgRasterize(rast, image, 0, 0, 1, img, w, h, w * 4);
    unsigned char *png    = NULL;
    int           out_len = 0;
    png = stbi_write_png_to_mem((unsigned char *)img, w * 4, w, h, 4, &out_len);
    if (png) {
        RETURN_BUFFER((char *)png, out_len);
        free(png);
    } else {
        RETURN_STRING("");
    }
    free(img);
    nsvgDeleteRasterizer(rast);
    nsvgDelete(image);
#else
    GError     *gsvgerror = 0;
    RsvgHandle *handle    = rsvg_handle_new_from_data((const guchar *)PARAM(0), PARAM_LEN(0), &gsvgerror);
    if (gsvgerror) {
        if (PARAMETERS_COUNT > 3) {
            SET_STRING(3, gsvgerror->message);
        }
        g_error_free(gsvgerror);
        if (handle) {
            rsvg_handle_close(handle, &gsvgerror);
            g_object_unref(handle);
        }
        RETURN_STRING("");
        return 0;
    }
    if ((handle) && (dpi > 0))
        rsvg_handle_set_dpi(handle, dpi);

    GdkPixbuf *pixbuf = rsvg_handle_get_pixbuf(handle);

    if (pixbuf) {
        gchar *data    = 0;
        gsize buf_size = 0;

        gdk_pixbuf_save_to_buffer(pixbuf, &data, &buf_size, type, &gsvgerror, 0);
        if (gsvgerror) {
            if (PARAMETERS_COUNT > 3) {
                SET_STRING(3, gsvgerror->message);
            }
            g_error_free(gsvgerror);
        }
        if ((data) && (buf_size > 0)) {
            RETURN_BUFFER(data, buf_size);
        } else {
            RETURN_STRING("");
        }
        if (data)
            g_free(data);
    } else {
        RETURN_STRING("");
    }
    if (pixbuf)
        g_object_unref(pixbuf);

    rsvg_handle_close(handle, NULL);
    g_object_unref(handle);
#endif
END_IMPL
//-----------------------------------------------------//

