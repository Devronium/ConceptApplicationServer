//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
#include <string.h>

#include <poppler.h>
#define POPPLER_WITH_CAIRO

#ifdef POPPLER_WITH_CAIRO
cairo_surface_t *_poppler_page_render_buffer(PopplerPage *page, int src_x, int src_y, int src_width, int src_height, double scale, int rotation, int printing) {
    cairo_t         *cr;
    cairo_surface_t *surface;

    surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, src_width, src_height);
    cr      = cairo_create(surface);
    cairo_save(cr);
    switch (rotation) {
        case 90:
            cairo_translate(cr, src_x + src_width, -src_y);
            break;

        case 180:
            cairo_translate(cr, src_x + src_width, src_y + src_height);
            break;

        case 270:
            cairo_translate(cr, -src_x, src_y + src_height);
            break;

        default:
            cairo_translate(cr, -src_x, -src_y);
    }

    if (scale != 1.0)
        cairo_scale(cr, scale, scale);

    if (rotation != 0)
        cairo_rotate(cr, rotation * G_PI / 180.0);

    if (printing)
        poppler_page_render_for_printing(page, cr);
    else
        poppler_page_render(page, cr);
    cairo_restore(cr);

    cairo_set_operator(cr, CAIRO_OPERATOR_DEST_OVER);
    cairo_set_source_rgb(cr, 1., 1., 1.);
    cairo_paint(cr);
    cairo_destroy(cr);
    return surface;
}

static cairo_status_t write_to_buffer(void *closure, const unsigned char *data, unsigned int length) {
    if (closure)
        ((AnsiString *)closure)->AddBuffer((char *)data, length);
    return CAIRO_STATUS_SUCCESS;
}

int poppler_page_render_buffer(PopplerPage *page, int src_x, int src_y, int src_width, int src_height, double scale, int rotation, AnsiString *buf) {
    if (!POPPLER_IS_PAGE(page))
        return CAIRO_STATUS_INVALID_STATUS;
    if (scale <= 0.0)
        return CAIRO_STATUS_INVALID_STATUS;
    if (!buf)
        return CAIRO_STATUS_INVALID_STATUS;

    cairo_surface_t *surface = _poppler_page_render_buffer(page, src_x, src_y, src_width, src_height, scale, rotation, 0);
    if (surface) {
        *buf = "";
        cairo_status_t res = cairo_surface_write_to_png_stream(surface, write_to_buffer, buf);
        cairo_surface_destroy(surface);
        return res;
    }
    return CAIRO_STATUS_INVALID_STATUS;
}

int poppler_page_render_to_file(PopplerPage *page, int src_x, int src_y, int src_width, int src_height, double scale, int rotation, char *filename) {
    if (!POPPLER_IS_PAGE(page))
        return CAIRO_STATUS_INVALID_STATUS;
    if (scale <= 0.0)
        return CAIRO_STATUS_INVALID_STATUS;
    if ((!filename) || (!filename[0]))
        return CAIRO_STATUS_INVALID_STATUS;

    cairo_surface_t *surface = _poppler_page_render_buffer(page, src_x, src_y, src_width, src_height, scale, rotation, 0);
    if (surface) {
        cairo_status_t res = cairo_surface_write_to_png(surface, filename);
        cairo_surface_destroy(surface);
        return res;
    }
    return CAIRO_STATUS_INVALID_STATUS;
}

#else
 #ifndef POPPLER_WITH_GDK
  #include <gdk-pixbuf/gdk-pixbuf.h>
static void copy_cairo_surface_to_pixbuf(cairo_surface_t *surface, GdkPixbuf *pixbuf) {
    int           cairo_width, cairo_height, cairo_rowstride;
    unsigned char *pixbuf_data, *dst, *cairo_data;
    int           pixbuf_rowstride, pixbuf_n_channels;
    unsigned int  *src;
    int           x, y;

    cairo_width     = cairo_image_surface_get_width(surface);
    cairo_height    = cairo_image_surface_get_height(surface);
    cairo_rowstride = cairo_image_surface_get_stride(surface);
    cairo_data      = cairo_image_surface_get_data(surface);

    pixbuf_data       = gdk_pixbuf_get_pixels(pixbuf);
    pixbuf_rowstride  = gdk_pixbuf_get_rowstride(pixbuf);
    pixbuf_n_channels = gdk_pixbuf_get_n_channels(pixbuf);

    if (cairo_width > gdk_pixbuf_get_width(pixbuf))
        cairo_width = gdk_pixbuf_get_width(pixbuf);
    if (cairo_height > gdk_pixbuf_get_height(pixbuf))
        cairo_height = gdk_pixbuf_get_height(pixbuf);
    for (y = 0; y < cairo_height; y++) {
        src = (unsigned int *)(cairo_data + y * cairo_rowstride);
        dst = pixbuf_data + y * pixbuf_rowstride;
        for (x = 0; x < cairo_width; x++) {
            dst[0] = (*src >> 16) & 0xff;
            dst[1] = (*src >> 8) & 0xff;
            dst[2] = (*src >> 0) & 0xff;
            if (pixbuf_n_channels == 4)
                dst[3] = (*src >> 24) & 0xff;
            dst += pixbuf_n_channels;
            src++;
        }
    }
}

static void _poppler_page_render_to_pixbuf(PopplerPage *page, int src_x, int src_y, int src_width, int src_height, double scale, int rotation, int printing, GdkPixbuf *pixbuf) {
    cairo_t         *cr;
    cairo_surface_t *surface;

    surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                                         src_width, src_height);
    cr = cairo_create(surface);
    cairo_save(cr);
    switch (rotation) {
        case 90:
            cairo_translate(cr, src_x + src_width, -src_y);
            break;

        case 180:
            cairo_translate(cr, src_x + src_width, src_y + src_height);
            break;

        case 270:
            cairo_translate(cr, -src_x, src_y + src_height);
            break;

        default:
            cairo_translate(cr, -src_x, -src_y);
    }

    if (scale != 1.0)
        cairo_scale(cr, scale, scale);

    if (rotation != 0)
        cairo_rotate(cr, rotation * G_PI / 180.0);

    if (printing)
        poppler_page_render_for_printing(page, cr);
    else
        poppler_page_render(page, cr);
    cairo_restore(cr);

    cairo_set_operator(cr, CAIRO_OPERATOR_DEST_OVER);
    cairo_set_source_rgb(cr, 1., 1., 1.);
    cairo_paint(cr);

    cairo_destroy(cr);

    copy_cairo_surface_to_pixbuf(surface, pixbuf);
    cairo_surface_destroy(surface);
}

void poppler_page_render_to_pixbuf(PopplerPage *page, int src_x, int src_y, int src_width, int src_height, double scale, int rotation, GdkPixbuf *pixbuf) {
    g_return_if_fail(POPPLER_IS_PAGE(page));
    g_return_if_fail(scale > 0.0);
    g_return_if_fail(pixbuf != NULL);

    _poppler_page_render_to_pixbuf(page, src_x, src_y,
                                   src_width, src_height,
                                   scale, rotation,
                                   0,
                                   pixbuf);
}
 #endif
#endif
//------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    g_type_init();

    return 0;
}
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(PDFLoadBuffer, 1, 3)
    T_STRING(0)
    GError * docerr = 0;
    char *password = NULL;

    if (PARAMETERS_COUNT > 1) {
        T_STRING(1)
        password = PARAM(1);
    }

    if (PARAMETERS_COUNT > 2) {
        SET_STRING(2, "");
    }

    PopplerDocument *pdoc = poppler_document_new_from_data(PARAM(0), PARAM_LEN(0), password, &docerr);
    if (docerr) {
        if (PARAMETERS_COUNT > 2) {
            SET_STRING(2, docerr->message);
        }
    }
    RETURN_NUMBER((NUMBER)(SYS_INT)pdoc)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(PDFLoad, 1, 3)
    T_STRING(0)
    GError * docerr = 0;
    char *password = NULL;

    if (PARAMETERS_COUNT > 1) {
        T_STRING(1)
        password = PARAM(1);
    }
    if (PARAMETERS_COUNT > 2) {
        SET_STRING(2, "");
    }

    PopplerDocument *pdoc = poppler_document_new_from_file(PARAM(0), password, &docerr);
    if (docerr) {
        if (PARAMETERS_COUNT > 2) {
            SET_STRING(2, docerr->message);
        }
    }
    RETURN_NUMBER((NUMBER)(SYS_INT)pdoc)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PDFClose, 1)
    T_NUMBER(0)
    PopplerDocument * pdoc = (PopplerDocument *)PARAM_INT(0);

    if (pdoc)
        g_object_unref(pdoc);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PDFPageCount, 1)
    T_HANDLE(0)
    PopplerDocument * pdoc = (PopplerDocument *)PARAM_INT(0);

    RETURN_NUMBER(poppler_document_get_n_pages(pdoc))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PDFPageText, 2)
    T_HANDLE(0)
    T_NUMBER(1)

    PopplerDocument * pdoc = (PopplerDocument *)PARAM_INT(0);

    PopplerPage *page = poppler_document_get_page(pdoc, PARAM_INT(1));
    if (page) {
        PopplerRectangle rect;
        gdouble          width, height;

        poppler_page_get_size(page, &width, &height);
        rect.x1 = rect.y1 = 0;
        rect.x2 = width;
        rect.y2 = height;
#ifdef _WIN32
        char *buf = poppler_page_get_text(page, POPPLER_SELECTION_GLYPH, &rect);
#else
        char *buf = poppler_page_get_text(page);
#endif
        g_free(buf);
        RETURN_STRING(buf)

        g_object_unref(G_OBJECT(page));
    } else {
        RETURN_STRING("")
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PDFFindText, 3)
    T_HANDLE(0)
    T_NUMBER(1)
    T_STRING(2)

    PopplerDocument * pdoc = (PopplerDocument *)PARAM_INT(0);

    PopplerPage *page = poppler_document_get_page(pdoc, PARAM_INT(1));
    if (page) {
        GList *lst = poppler_page_find_text(page, PARAM(2));

        int index = 0;
        Invoke(INVOKE_CREATE_ARRAY, RESULT);
        for (GList *b = lst; b; b = b->next) {
            PopplerRectangle *rect = (PopplerRectangle *)b->data;
            if (rect) {
                void *newpData = 0;
                Invoke(INVOKE_ARRAY_VARIABLE, RESULT, index, &newpData);
                Invoke(INVOKE_CREATE_ARRAY, newpData);

                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "x1", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)rect->x1);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "x2", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)rect->x2);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "y1", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)rect->y1);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "y2", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)rect->y2);
                index++;
            }
        }
        g_object_unref(G_OBJECT(page));

        /*if (lst) {
            RETURN_NUMBER(1)
           } else {
            RETURN_NUMBER(0)
           }*/
    } /* else {
         RETURN_NUMBER(0)
         }*/
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(PDFPageImageBuffer, 2, 5)
    T_HANDLE(0)
    T_NUMBER(1)

    const char *type = "png";
    if (PARAMETERS_COUNT > 2) {
        T_STRING(2);
        type = PARAM(2);
    }

    if (PARAMETERS_COUNT > 4)
        SET_STRING(4, "");

    PopplerDocument *pdoc = (PopplerDocument *)PARAM_INT(0);

    PopplerPage *page = poppler_document_get_page(pdoc, PARAM_INT(1));
    if (page) {
        double src_width  = 0;
        double src_height = 0;
        double thumb_zoom = 1;
        poppler_page_get_size(page, &src_width, &src_height);

        if (PARAMETERS_COUNT > 3) {
            T_NUMBER(3);
            thumb_zoom = PARAM(3);
        }

        if (src_width == 0)
            src_width = 1;
        if (src_height == 0)
            src_height = 1;

#ifdef POPPLER_WITH_CAIRO
        AnsiString tmp;
        int        res = poppler_page_render_buffer(page, 0, 0, (int)src_width * thumb_zoom, (int)src_height * thumb_zoom, thumb_zoom, 0, &tmp);
        if (res) {
            if (PARAMETERS_COUNT > 4) {
                SET_STRING(4, cairo_status_to_string((cairo_status_t)res));
            }
        } else {
            RETURN_NUMBER(1)
        }
        if (tmp.Length()) {
            RETURN_BUFFER(tmp.c_str(), tmp.Length());
        } else {
            RETURN_STRING("");
        }
#else
        GdkPixbuf *pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, false, 8, (int)src_width * thumb_zoom, (int)src_height * thumb_zoom);
        poppler_page_render_to_pixbuf(page, 0, 0, (int)src_width * thumb_zoom, (int)src_height * thumb_zoom, thumb_zoom, 0, pixbuf);

        GError *docerr     = 0;
        gchar  *buffer     = 0;
        gsize  buffer_size = 0;

        if (!gdk_pixbuf_save_to_buffer(pixbuf, &buffer, &buffer_size, type, &docerr, NULL)) {
            if ((PARAMETERS_COUNT > 4) && (docerr)) {
                SET_STRING(4, docerr->message);
            }
            RETURN_STRING("");
        } else {
            if (buffer_size) {
                RETURN_BUFFER((char *)buffer, (INTEGER)buffer_size)
            } else {
                RETURN_STRING("");
            }
        }
        if (pixbuf)
            g_object_unref(G_OBJECT(pixbuf));
#endif
        g_object_unref(G_OBJECT(page));
    } else {
        RETURN_STRING("");
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(PDFPageImage, 3, 6)
    T_HANDLE(0)
    T_NUMBER(1)
    T_STRING(2)

    const char *type = "png";
    if (PARAMETERS_COUNT > 3) {
        T_STRING(3);
        type = PARAM(3);
    }

    if (PARAMETERS_COUNT > 5)
        SET_STRING(5, "");

    PopplerDocument *pdoc = (PopplerDocument *)PARAM_INT(0);

    PopplerPage *page = poppler_document_get_page(pdoc, PARAM_INT(1));
    if (page) {
        double src_width  = 0;
        double src_height = 0;
        double thumb_zoom = 1;
        poppler_page_get_size(page, &src_width, &src_height);

        if (PARAMETERS_COUNT > 4) {
            T_NUMBER(4);
            thumb_zoom = PARAM(4);
        }

        if (src_width == 0)
            src_width = 1;
        if (src_height == 0)
            src_height = 1;
#ifdef POPPLER_WITH_CAIRO
        int res = poppler_page_render_to_file(page, 0, 0, (int)src_width * thumb_zoom, (int)src_height * thumb_zoom, thumb_zoom, 0, PARAM(2));
        if (res) {
            if (PARAMETERS_COUNT > 5) {
                SET_STRING(5, cairo_status_to_string((cairo_status_t)res));
            }
        } else {
            RETURN_NUMBER(1)
        }
#else
        GdkPixbuf *pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, false, 8, (int)src_width * thumb_zoom, (int)src_height * thumb_zoom);
        poppler_page_render_to_pixbuf(page, 0, 0, (int)src_width * thumb_zoom, (int)src_height * thumb_zoom, thumb_zoom, 0, pixbuf);

        GError *docerr = 0;

        if (!gdk_pixbuf_save(pixbuf, PARAM(2), type, &docerr, NULL)) {
            if ((PARAMETERS_COUNT > 5) && (docerr)) {
                SET_STRING(5, docerr->message);
            }
            RETURN_NUMBER(0)
        } else {
            RETURN_NUMBER(1)
        }
        if (pixbuf)
            g_object_unref(G_OBJECT(pixbuf));
#endif
        g_object_unref(G_OBJECT(page));
    } else {
        RETURN_NUMBER(0)
    }
END_IMPL
//------------------------------------------------------------------------
gboolean save_data(const gchar *buf, gsize count, gpointer data, GError **error) {
    AnsiString *S = (AnsiString *)data;

    if ((buf) && (count > 0))
        S->AddBuffer((char *)buf, count);
    return TRUE;
}

//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PDFAttachments, 1)
    T_HANDLE(0)

    GList * list, *l;
    PopplerDocument *pdoc = (PopplerDocument *)PARAM_INT(0);
    CREATE_ARRAY(RESULT);
    if (!poppler_document_has_attachments(pdoc))
        return 0;
    list = poppler_document_get_attachments(pdoc);
    INTEGER index = 0;
    for (l = list; l && l->data; l = g_list_next(l)) {
        PopplerAttachment *attachment = POPPLER_ATTACHMENT(l->data);
        if (attachment) {
            void *ARR = 0;
            Invoke(INVOKE_ARRAY_VARIABLE, RESULT, (INTEGER)index++, &ARR);
            if (ARR) {
                CREATE_ARRAY(ARR);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, ARR, (char *)"name", (INTEGER)VARIABLE_STRING, (char *)attachment->name ? attachment->name : "Unknown", (NUMBER)0);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, ARR, (char *)"description", (INTEGER)VARIABLE_STRING, (char *)attachment->description ? attachment->description : "", (NUMBER)0);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, ARR, (char *)"size", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)attachment->size);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, ARR, (char *)"ctime", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)attachment->ctime);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, ARR, (char *)"mtime", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)attachment->mtime);
                AnsiString *S = new AnsiString();
                poppler_attachment_save_to_callback(attachment, save_data, S, NULL);
                if (S->Length())
                    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, ARR, (char *)"content", (INTEGER)VARIABLE_STRING, (char *)S->c_str(), (NUMBER)S->Length());
                else
                    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, ARR, (char *)"content", (INTEGER)VARIABLE_STRING, (char *)"", (NUMBER)0);
                delete S;
            }
        }
    }
    if (list)
        g_list_free(list);
END_IMPL
//------------------------------------------------------------------------
