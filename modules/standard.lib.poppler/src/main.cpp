//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
#include <string.h>
#include <stdio.h>
#include "miniz.c"

#include <cpp/poppler-document.h>
#include <cpp/poppler-page.h>
#include <cpp/poppler-embedded-file.h>
#include <cpp/poppler-image.h>
#include <cpp/poppler-page-renderer.h>

//------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(PDFLoadBuffer, 1, 3)
    T_STRING(0)

    char *password = NULL;

    if (PARAMETERS_COUNT > 1) {
        T_STRING(1)
        password = PARAM(1);
    }
    if (PARAMETERS_COUNT > 2) {
        SET_STRING(2, "");
    }

    poppler::document *pdoc = poppler::document::load_from_raw_data(PARAM(0), PARAM_LEN(0), password, password);

    if (!pdoc) {
        if (PARAMETERS_COUNT > 2) {
            SET_STRING(2, "Error reading document");
        }
    }
    RETURN_NUMBER((NUMBER)(SYS_INT)pdoc)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(PDFLoad, 1, 3)
    T_STRING(0)

    char *password = NULL;

    if (PARAMETERS_COUNT > 1) {
        T_STRING(1)
        password = PARAM(1);
    }
    if (PARAMETERS_COUNT > 2) {
        SET_STRING(2, "");
    }

    poppler::document *pdoc = poppler::document::load_from_file(PARAM(0), password, password);

    if (!pdoc) {
        if (PARAMETERS_COUNT > 2) {
            SET_STRING(2, "Error reading document");
        }
    }
    RETURN_NUMBER((NUMBER)(SYS_INT)pdoc)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PDFClose, 1)
    T_NUMBER(0)
    poppler::document * pdoc = (poppler::document *)(SYS_INT)PARAM(0);

    if (pdoc) {
        delete pdoc;
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PDFPageCount, 1)
    T_HANDLE(0)
    poppler::document * pdoc = (poppler::document *)(SYS_INT)PARAM(0);

    RETURN_NUMBER(pdoc->pages())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PDFPageText, 2)
    T_HANDLE(0)
    T_NUMBER(1)

    poppler::document * pdoc = (poppler::document *)PARAM_INT(0);

    poppler::page *page = pdoc->create_page(PARAM_INT(1));
    if (page) {
        poppler::ustring buf = page->text();
        if (buf.to_utf8().size()) {
#if __cplusplus <= 199711L
            char *buf_ptr = &buf.to_utf8().front();
#else
            char *buf_ptr = &buf.to_utf8().data();
#endif
            int size = buf.to_utf8().size();

            if (size > 0) {
                RETURN_BUFFER(buf_ptr, size);
            } else {
                RETURN_STRING("");
            }
        } else {
            RETURN_STRING("");
        }

        delete page;
    } else {
        RETURN_STRING("")
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PDFFindText, 3)
    T_HANDLE(0)
    T_NUMBER(1)
    T_STRING(2)

    poppler::document * pdoc = (poppler::document *)PARAM_INT(0);
    int index = 0;
    Invoke(INVOKE_CREATE_ARRAY, RESULT);
    poppler::page *page = pdoc->create_page(PARAM_INT(1));
    if (page) {
        poppler::ustring text = poppler::ustring::from_utf8(PARAM(2), PARAM_LEN(2));
        poppler::rectf   rect;
        if (page->search(text, rect, poppler::page::search_from_top, poppler::case_insensitive)) {
            do {
                void *newpData = 0;
                Invoke(INVOKE_ARRAY_VARIABLE, RESULT, index, &newpData);
                Invoke(INVOKE_CREATE_ARRAY, newpData);

                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "x1", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)rect.x());
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "x2", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(rect.x() + rect.width()));
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "y1", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)rect.y());
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, newpData, "y2", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)(rect.y() + rect.height()));
                index++;
            } while (page->search(text, rect, poppler::page::search_next_result, poppler::case_insensitive));
        }
        delete page;
    }
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

    poppler::document *pdoc = (poppler::document *)PARAM_INT(0);

    poppler::page *page = pdoc->create_page(PARAM_INT(1));
    if (page) {
        poppler::image         image;
        poppler::page_renderer renderer;
        if (PARAMETERS_COUNT > 3) {
            T_NUMBER(3);
            NUMBER         thumb_zoom = PARAM(3);
            poppler::rectf rect       = page->page_rect();
            image = renderer.render_page(page, 72.0 * thumb_zoom, 72.0 * thumb_zoom);
        } else {
            image = renderer.render_page(page);
        }
        int channels = 0;
        switch (image.format()) {
            case poppler::image::format_mono:
                channels = 1;
                break;

            case poppler::image::format_rgb24:
                channels = 3;
                break;

            case poppler::image::format_argb32:
                channels = 4;
                break;
        }
        size_t png_data_size = 0;
        void   *pPNG_data    = NULL;
        if (channels)
            pPNG_data = tdefl_write_image_to_png_file_in_memory(image.const_data(), image.width(), image.height(), channels, &png_data_size);

        if ((pPNG_data) && (png_data_size > 0)) {
            RETURN_BUFFER(pPNG_data, png_data_size);
        } else {
            if (PARAMETERS_COUNT > 4) {
                SET_STRING(4, "Error saving image")
            }
            RETURN_STRING("");
        }
        if (pPNG_data)
            mz_free(pPNG_data);
        delete page;
    } else {
        RETURN_STRING("")
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

    poppler::document *pdoc = (poppler::document *)PARAM_INT(0);

    poppler::page *page = pdoc->create_page(PARAM_INT(1));
    if (page) {
        poppler::image         image;
        poppler::page_renderer renderer;
        if (PARAMETERS_COUNT > 4) {
            T_NUMBER(4);
            NUMBER         thumb_zoom = PARAM(4);
            poppler::rectf rect       = page->page_rect();
            image = renderer.render_page(page, 72.0 * thumb_zoom, 72.0 * thumb_zoom);
        } else {
            image = renderer.render_page(page);
        }
#ifdef _WIN32
        int channels = 0;
        switch (image.format()) {
            case poppler::image::format_mono:
                channels = 1;
                break;

            case poppler::image::format_rgb24:
                channels = 3;
                break;

            case poppler::image::format_argb32:
                channels = 4;
                break;
        }
        size_t png_data_size = 0;
        void   *pPNG_data    = NULL;
        if (channels)
            pPNG_data = tdefl_write_image_to_png_file_in_memory(image.const_data(), image.width(), image.height(), channels, &png_data_size);

        if ((pPNG_data) && (png_data_size > 0)) {
            FILE *f = fopen(PARAM(2), "wb");
            if (f) {
                fwrite(pPNG_data, png_data_size, 1, f);
                fclose(f);
            } else
            if (PARAMETERS_COUNT > 5) {
                SET_STRING(5, "Error writing image")
            }
        } else {
            if (PARAMETERS_COUNT > 5) {
                SET_STRING(5, "Error creating image")
            }
            RETURN_STRING("");
        }
        if (pPNG_data)
            mz_free(pPNG_data);
#else
        if (image.save(PARAM(2), type)) {
            RETURN_NUMBER(1)
        } else {
            if (PARAMETERS_COUNT > 5) {
                SET_STRING(5, "Error writing image")
            }
            RETURN_NUMBER(0)
        }
#endif
        delete page;
    } else {
        RETURN_NUMBER(0)
    }
END_IMPL
//------------------------------------------------------------------------
const char *getVectorData(poppler::ustring buf) {
    if (buf.to_utf8().size()) {
#if __cplusplus <= 199711L
        return &buf.to_utf8().front();
#else
        return buf.to_utf8().data();
#endif
    }
    return "";
}

CONCEPT_FUNCTION_IMPL(PDFAttachments, 1)
    T_HANDLE(0)

    poppler::document * pdoc = (poppler::document *)PARAM_INT(0);
    CREATE_ARRAY(RESULT);
    if (!pdoc->has_embedded_files())
        return 0;
    std::vector<poppler::embedded_file *> list = pdoc->embedded_files();
    INTEGER index = 0;
    for (std::vector<poppler::embedded_file *>::iterator it = list.begin(); it != list.end(); ++it) {
        poppler::embedded_file *attachment = *it;
        if (attachment) {
            void *ARR = 0;
            Invoke(INVOKE_ARRAY_VARIABLE, RESULT, (INTEGER)index++, &ARR);
            if (ARR) {
                CREATE_ARRAY(ARR);

                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, ARR, (char *)"name", (INTEGER)VARIABLE_STRING, (char *)(attachment->name().size() ? attachment->name().c_str() : "Unknown"), (NUMBER)0);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, ARR, (char *)"description", (INTEGER)VARIABLE_STRING, getVectorData(attachment->description()), (NUMBER)0);
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, ARR, (char *)"size", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)attachment->size());
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, ARR, (char *)"ctime", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)attachment->creation_date());
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, ARR, (char *)"mtime", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)attachment->modification_date());
                Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, ARR, (char *)"mime", (INTEGER)VARIABLE_STRING, attachment->mime_type().c_str(), (NUMBER)0);
                poppler::byte_array data = attachment->data();
                if (data.size()) {
#if __cplusplus <= 199711L
                    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, ARR, (char *)"content", (INTEGER)VARIABLE_STRING, (char *)&data.front(), (NUMBER)data.size());
#else
                    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, ARR, (char *)"content", (INTEGER)VARIABLE_STRING, (char *)data.data(), (NUMBER)data.size());
#endif
                } else
                    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, ARR, (char *)"content", (INTEGER)VARIABLE_STRING, (char *)"", (NUMBER)0);
            }
        }
    }
END_IMPL
//------------------------------------------------------------------------
