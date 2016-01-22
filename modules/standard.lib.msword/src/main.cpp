//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include <gsf/gsf-input-stdio.h>
#include <gsf/gsf-input-memory.h>
#include <gsf/gsf-utils.h>
#include <gsf/gsf-infile.h>
#include <gsf/gsf-infile-msole.h>
#include <gsf/gsf-msole-utils.h>
#include <gsf/gsf-docprop-vector.h>
#include <gsf/gsf-meta-names.h>

#ifdef _WIN32
AnsiString static_path;
AnsiString static_path_html;
 #include <windows.h>
 #define WVDATADIR     static_path.c_str()
 #define HTMLCONFIG    static_path_html.c_str()
 #define strcasecmp(s1, s2)    stricmp(s1, s2)
#endif

/* prepare for i18n */
#define N_(X)    (X)
#define _(X)     (X)

INVOKE_CALL InvokePtr = 0;
void        *T_METARESULT;

AnsiString t_result;
AnsiString t_error;


static const struct {
    char *metadata_key;
    char *human_readable_key;
} metadata_names[] = {
    { GSF_META_NAME_TITLE,                 N_("Title")                           },
    { GSF_META_NAME_DESCRIPTION,           N_("Description")                     },
    { GSF_META_NAME_SUBJECT,               N_("Subject")                         },
    { GSF_META_NAME_DATE_MODIFIED,         N_("Last Modified")                   },
    { GSF_META_NAME_DATE_CREATED,          N_("Created")                         },
    { GSF_META_NAME_KEYWORDS,              N_("Keywords")                        },
    { GSF_META_NAME_LANGUAGE,              N_("Language")                        },
    { GSF_META_NAME_REVISION_COUNT,        N_("Revision")                        },
    { GSF_META_NAME_EDITING_DURATION,      N_("Editing Duration")                },
    { GSF_META_NAME_TABLE_COUNT,           N_("Number of Tables")                },
    { GSF_META_NAME_IMAGE_COUNT,           N_("Number of Images")                },
    { GSF_META_NAME_OBJECT_COUNT,          N_("Number of Objects")               },
    { GSF_META_NAME_PAGE_COUNT,            N_("Number of Pages")                 },
    { GSF_META_NAME_PARAGRAPH_COUNT,       N_("Number of Paragraphs")            },
    { GSF_META_NAME_WORD_COUNT,            N_("Number of Words")                 },
    { GSF_META_NAME_CHARACTER_COUNT,       N_("Number of Characters")            },
    { GSF_META_NAME_CELL_COUNT,            N_("Number of Spreadsheet Cells")     },
    { GSF_META_NAME_SPREADSHEET_COUNT,     N_("Number of Sheets")                },
    { GSF_META_NAME_CREATOR,               N_("Creator")                         },
    { GSF_META_NAME_TEMPLATE,              N_("Template")                        },
    { GSF_META_NAME_LAST_SAVED_BY,         N_("Last Saved by")                   },
    { GSF_META_NAME_LAST_PRINTED,          N_("Last Printed")                    },
    { GSF_META_NAME_SECURITY,              N_("Security Level")                  },
    { GSF_META_NAME_CATEGORY,              N_("Category")                        },
    { GSF_META_NAME_PRESENTATION_FORMAT,   N_("")                                },
    { GSF_META_NAME_THUMBNAIL,             N_("Thumbnail")                       },
    { GSF_META_NAME_GENERATOR,             N_("Generator")                       },
    { GSF_META_NAME_LINE_COUNT,            N_("Number of Lines")                 },
    { GSF_META_NAME_SLIDE_COUNT,           N_("Number of Slides")                },
    { GSF_META_NAME_NOTE_COUNT,            N_("Number of Notes")                 },
    { GSF_META_NAME_HIDDEN_SLIDE_COUNT,    N_("Number of Hidden Slides")         },
    { GSF_META_NAME_MM_CLIP_COUNT,         N_("Number of 'Multi-Media' Clips")   },
    { GSF_META_NAME_BYTE_COUNT,            N_("Number of Bytes in the Document") },
    { GSF_META_NAME_SCALE,                 N_("Scale")                           },
    { GSF_META_NAME_HEADING_PAIRS,         N_("Document Pairs")                  },
    { GSF_META_NAME_DOCUMENT_PARTS,        N_("Document Parts")                  },
    { GSF_META_NAME_MANAGER,               N_("Manager")                         },
    { GSF_META_NAME_COMPANY,               N_("Company")                         },
    { GSF_META_NAME_LINKS_DIRTY,           N_("Links Dirty")                     },
    { GSF_META_NAME_MSOLE_UNKNOWN_17,      N_("Unknown1")                        },
    { GSF_META_NAME_MSOLE_UNKNOWN_18,      N_("Unknown2")                        },
    { GSF_META_NAME_MSOLE_UNKNOWN_19,      N_("Unknown3")                        },
    { GSF_META_NAME_MSOLE_UNKNOWN_20,      N_("Unknown4")                        },
    { GSF_META_NAME_MSOLE_UNKNOWN_21,      N_("Unknown5")                        },
    { GSF_META_NAME_MSOLE_UNKNOWN_22,      N_("Unknown6")                        },
    { GSF_META_NAME_MSOLE_UNKNOWN_23,      N_("Unknown7")                        },
    { GSF_META_NAME_DICTIONARY,            N_("Dictionary")                      },
    { GSF_META_NAME_LOCALE_SYSTEM_DEFAULT, N_("Default Locale")                  },
    { GSF_META_NAME_CASE_SENSITIVE,        N_("Case Sensitive")                  }
};
static const gsize nr_metadata_names = G_N_ELEMENTS(metadata_names);

static void
cb_print_property(char const *name, GsfDocProp const *prop, GHashTable *human_readable_names) {
    GValue const *val = gsf_doc_prop_get_val(prop);
    char         *tmp;
    char const   *_name;

    if ((_name = (char *)g_hash_table_lookup(human_readable_names, name)) == NULL)
        _name = name;

    if (VAL_IS_GSF_DOCPROP_VECTOR((GValue *)val)) {
        GValueArray *va = gsf_value_get_docprop_varray(val);
        unsigned    i;

        void *newpData;
        InvokePtr(INVOKE_ARRAY_VARIABLE_BY_KEY, T_METARESULT, _name, &newpData);
        InvokePtr(INVOKE_CREATE_ARRAY, newpData);

        for (i = 0; i < va->n_values; i++) {
            tmp = g_strdup_value_contents(g_value_array_get_nth(va, i));
            InvokePtr(INVOKE_SET_ARRAY_ELEMENT, newpData, (INTEGER)i, (INTEGER)VARIABLE_STRING, (char *)tmp, (NUMBER)0);
            g_free(tmp);
        }
    } else {
        tmp = g_strdup_value_contents(val);
        InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, T_METARESULT, _name, (INTEGER)VARIABLE_STRING, (char *)tmp, (NUMBER)0);
        g_free(tmp);
    }
}

static void print_summary_stream(GsfInfile *msole,
                                 const char *file_name,
                                 const char *stream_name,
                                 GHashTable *human_readable_names, void *RESULT) {
    GsfInput *stream = gsf_infile_child_by_name(msole, stream_name);

    if (stream != NULL) {
        GsfDocMetaData *meta_data = gsf_doc_meta_data_new();
        GError         *err       = NULL;

        err = gsf_msole_metadata_read(stream, meta_data);
        if (err != NULL) {
            g_warning("Error getting metadata for %s->%s: %s",
                      file_name, stream_name, err->message);
            g_error_free(err);
            err = NULL;
        } else
            T_METARESULT = RESULT;
        gsf_doc_meta_data_foreach(meta_data,
                                  (GHFunc)cb_print_property, human_readable_names);

        g_object_unref(meta_data);
        g_object_unref(G_OBJECT(stream));
    }
}

int MyPrintF(const char *format, ...) {
    va_list argp;
    //char buffer[0xFFFF];

    int     n, size = 0xFFF;
    char    *p, *np;
    va_list ap;

    if ((p = (char *)malloc(size)) == NULL)
        return -1;

    while (1) {
        va_start(argp, format);
        n = vsnprintf(p, size, format, argp);
        va_end(argp);

        if ((n > -1) && (n < size)) {
            t_result += p;
            free(p);
            return n;
        }
        if (n > -1)
            size = n + 1;
        else
            size *= 2;
        if ((np = (char *)realloc(p, size)) == NULL) {
            free(p);
            return 0;
        } else {
            p = np;
        }
    }
    return 0;
}

int MyFPrintF(FILE *out, const char *format, ...) {
    va_list argp;
    //char buffer[0xFFFF];

    int     n, size = 0xFFF;
    char    *p, *np;
    va_list ap;

    if ((p = (char *)malloc(size)) == NULL)
        return -1;

    while (1) {
        va_start(argp, format);
        n = vsnprintf(p, size, format, argp);
        va_end(argp);

        if ((n > -1) && (n < size)) {
            if (out == stdin)
                t_result += p;
            else
                t_error += p;
            free(p);
            return n;
        }
        if (n > -1)
            size = n + 1;
        else
            size *= 2;
        if ((np = (char *)realloc(p, size)) == NULL) {
            free(p);
            return 0;
        } else {
            p = np;
        }
    }
    return 0;
}

//------------------------------------------------------------------------
AnsiString GetDirectory() {
#ifdef _WIN32
    char buffer[4096];
    buffer[0] = 0;
    GetModuleFileName(NULL, buffer, 4096);
    for (int i = strlen(buffer) - 1; i >= 0; i--) {
        if ((buffer[i] == '/') || (buffer[i] == '\\')) {
            buffer[i + 1] = 0;
            break;
        }
    }
    return AnsiString(buffer);
#endif
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------
extern "C" {
#include <wv.h>
#include <string.h>


static char *config_Base = "wvHtml.xml";

char *config          = config_Base;
int  xml_output       = 0;
int  no_graphics      = 0;
char *wv_arg_basename = 0;
char *figure_name(wvParseStruct *ps);
char *name_to_url(char *name);

char *charset = NULL;
char wv_cwd[4097];

extern char *xml_slash;

extern char *str_copy(char *d, size_t n, char *s);
extern char *str_append(char *d, size_t n, char *s);

int myelehandler(wvParseStruct *ps, wvTag tag, void *props, int dirty);
int mydochandler(wvParseStruct *ps, wvTag tag);
int myCharProc(wvParseStruct *ps, U16 eachchar, U8 chartype, U16 lid);
int mySpecCharProc(wvParseStruct *ps, U16 eachchar, CHP *achp);
int wvOpenConfig(state_data *myhandle, char *config);

int HandleBitmap(wvParseStruct *ps, char *name, BitmapBlip *bitmap);
int HandleMetafile(wvParseStruct *ps, char *name, MetaFileBlip *bitmap);

/* should really be a config.h decl for having strdup, but... */
#ifdef _WIN32
char *
strdup(const char *text) {
    char   *buf;
    size_t len;

    len = strlen(text) + 1;
    buf = (char *)wvMalloc(len);
    memcpy(buf, text, len);

    return buf;
}
#endif

char *
wvHtmlGraphic(wvParseStruct *ps, Blip *blip) {
    char     *name;
    wvStream *fd;
    char     test[3];

    name = figure_name(ps);
    if (name == 0) return 0;

    /*
       temp hack to test older included bmps in word 6 and 7,
       should be wrapped in a modern escher strucure before getting
       to here, and then handled as normal
     */
    wvTrace(("type is %d\n", blip->type));
    switch (blip->type) {
        case msoblipJPEG:
        case msoblipDIB:
        case msoblipPNG:
            fd      = (blip->blip.bitmap.m_pvBits);
            test[2] = '\0';
            test[0] = read_8ubit(fd);

            test[1] = read_8ubit(fd);
            wvStream_rewind(fd);
            if (!(strcmp(test, "BM"))) {
                wvAppendStr(&name, ".bmp");
                if (0 != HandleBitmap(ps, name, &blip->blip.bitmap))
                    return NULL;
                return name;
            }

        default:
            break;
    }

    switch (blip->type) {
        case msoblipWMF:
            wvAppendStr(&name, ".wmf");
            if (0 != HandleMetafile(ps, name, &blip->blip.metafile))
                return NULL;
            break;

        case msoblipEMF:
            wvAppendStr(&name, ".emf");
            if (0 != HandleMetafile(ps, name, &blip->blip.metafile))
                return NULL;
            break;

        case msoblipPICT:
            wvAppendStr(&name, ".pict");
            if (0 != HandleMetafile(ps, name, &blip->blip.metafile))
                return NULL;
            break;

        case msoblipJPEG:
            wvAppendStr(&name, ".jpg");
            if (0 != HandleBitmap(ps, name, &blip->blip.bitmap))
                return NULL;
            break;

        case msoblipDIB:
            wvAppendStr(&name, ".dib");
            if (0 != HandleBitmap(ps, name, &blip->blip.bitmap))
                return NULL;
            break;

        case msoblipPNG:
            wvAppendStr(&name, ".png");
            if (0 != HandleBitmap(ps, name, &blip->blip.bitmap))
                return NULL;
            break;
    }
    return name;
}

int
HandleBitmap(wvParseStruct *ps, char *name, BitmapBlip *bitmap) {
    wvStream *pwv = bitmap->m_pvBits;
    FILE     *fd  = NULL;
    size_t   size = 0, i;

    if (ps->dir) chdir(ps->dir);
    fd = fopen(name, "wb");
    if (ps->dir) chdir(wv_cwd);
    if (fd == NULL) {
        //fprintf (stderr,"\nCannot open %s for writing\n",name);
        //exit (1);
        return -1;
    }
    size = wvStream_size(pwv);
    wvStream_rewind(pwv);

    for (i = 0; i < size; i++)
        fputc(read_8ubit(pwv), fd);
    fclose(fd);
    wvTrace(("Name is %s\n", name));
    return 0;
}

int
HandleMetafile(wvParseStruct *ps, char *name, MetaFileBlip *bitmap) {
    wvStream *pwv        = bitmap->m_pvBits;
    FILE     *fd         = NULL;
    size_t   size        = 0, i;
    U8       decompressf = 0;

    if (ps->dir) chdir(ps->dir);
    fd = fopen(name, "wb");
    if (ps->dir) chdir(wv_cwd);
    if (fd == NULL) {
        //fprintf (stderr,"\nCannot open %s for writing\n",name);
        //exit (1);
        return -1;
    }
    size = wvStream_size(pwv);
    wvStream_rewind(pwv);

    if (bitmap->m_fCompression == msocompressionDeflate)
        decompressf = setdecom();

    if (!decompressf) {
        for (i = 0; i < size; i++)
            fputc(read_8ubit(pwv), fd);
    } else {    /* decompress here */
        FILE *tmp = tmpfile();
        FILE *out = tmpfile();

        for (i = 0; i < size; i++)
            fputc(read_8ubit(pwv), tmp);

        rewind(tmp);
        decompress(tmp, out, bitmap->m_cbSave, bitmap->m_cb);
        fclose(tmp);

        rewind(out);

        for (i = 0; i < bitmap->m_cb; i++)
            fputc(fgetc(out), fd);

        fclose(out);
    }

    fclose(fd);
    wvTrace(("Name is %s\n", name));
    return 0;
}

int
myelehandler(wvParseStruct *ps, wvTag tag, void *props, int dirty) {
    static PAP  *ppap;
    expand_data *data = (expand_data *)ps->userData;

    data->anSttbfAssoc = &ps->anSttbfAssoc;
    data->lfo          = &ps->lfo;
    data->lfolvl       = ps->lfolvl;
    data->lvl          = ps->lvl;
    data->nolfo        = &ps->nolfo;
    data->nooflvl      = &ps->nooflvl;
    data->stsh         = &ps->stsh;
    data->lst          = &ps->lst;
    data->noofLST      = &ps->noofLST;
    data->liststartnos = &ps->liststartnos;
    data->listnfcs     = &ps->listnfcs;
    data->finallvl     = &ps->finallvl;
    data->fib          = &ps->fib;
    data->dop          = &ps->dop;
    data->intable      = &ps->intable;
    data->cellbounds   = &ps->cellbounds;
    data->nocellbounds = &ps->nocellbounds;
    data->endcell      = &ps->endcell;
    data->vmerges      = &ps->vmerges;
    data->norows       = &ps->norows;
    data->nextpap      = &ps->nextpap;
    if (charset == NULL) {
        data->charset = wvAutoCharset(ps);
        charset       = data->charset;
    } else
        data->charset = charset;
    data->props = props;

    switch (tag) {
        case PARABEGIN:
            {
                S16 tilfo = 0;
                /* test begin */
                if (*(data->endcell)) {
                    tilfo = ((PAP *)(data->props))->ilfo;
                    ((PAP *)(data->props))->ilfo = 0;
                }
                /* test end */
                ppap = (PAP *)data->props;
                wvTrace(
                    ("fore back is %d %d\n",
                     ((PAP *)(data->props))->shd.icoFore,
                     ((PAP *)(data->props))->shd.icoBack));
                wvBeginPara(data);
                if (tilfo)
                    ((PAP *)(data->props))->ilfo = tilfo;
            }
            break;

        case PARAEND:
            {
                S16 tilfo = 0;
                /* test begin */
                if (*(data->endcell)) {
                    tilfo = ((PAP *)(data->props))->ilfo;
                    ((PAP *)(data->props))->ilfo = 0;
                }
                /* test end */
                wvEndCharProp(data);    /* danger will break in the future */
                wvEndPara(data);
                if (tilfo)
                    ((PAP *)(data->props))->ilfo = tilfo;
                wvCopyPAP(&data->lastpap, (PAP *)(data->props));
            }
            break;

        case CHARPROPBEGIN:
            wvBeginCharProp(data, ppap);
            break;

        case CHARPROPEND:
            wvEndCharProp(data);
            break;

        case SECTIONBEGIN:
            wvBeginSection(data);
            break;

        case SECTIONEND:
            wvEndSection(data);
            break;

        case COMMENTBEGIN:
            wvBeginComment(data);
            break;

        case COMMENTEND:
            wvEndComment(data);
            break;

        default:
            break;
    }
    return 0;
}

int
mydochandler(wvParseStruct *ps, wvTag tag) {
    static int  i;
    expand_data *data = (expand_data *)ps->userData;

    data->anSttbfAssoc = &ps->anSttbfAssoc;
    data->lfo          = &ps->lfo;
    data->lfolvl       = ps->lfolvl;
    data->lvl          = ps->lvl;
    data->nolfo        = &ps->nolfo;
    data->nooflvl      = &ps->nooflvl;
    data->stsh         = &ps->stsh;
    data->lst          = &ps->lst;
    data->noofLST      = &ps->noofLST;
    data->liststartnos = &ps->liststartnos;
    data->listnfcs     = &ps->listnfcs;
    data->finallvl     = &ps->finallvl;
    data->fib          = &ps->fib;
    data->dop          = &ps->dop;
    data->intable      = &ps->intable;
    data->cellbounds   = &ps->cellbounds;
    data->nocellbounds = &ps->nocellbounds;
    data->endcell      = &ps->endcell;
    data->vmerges      = &ps->vmerges;
    data->norows       = &ps->norows;
    if (i == 0) {
        wvSetEntityConverter(data);
        data->filename  = ps->filename;
        data->whichcell = 0;
        data->whichrow  = 0;
        data->asep      = NULL;
        i++;
        wvInitPAP(&data->lastpap);
        data->nextpap = NULL;
        data->ps      = ps;
    }

    if (charset == NULL) {
        data->charset = wvAutoCharset(ps);
        charset       = data->charset;
    } else
        data->charset = charset;

    switch (tag) {
        case DOCBEGIN:
            wvBeginDocument(data);
            break;

        case DOCEND:
            wvEndDocument(data);
            break;

        default:
            break;
    }
    return 0;
}

void
wvStrangeNoGraphicData(char *config, int graphicstype) {
    wvError(("Strange No Graphic Data in the 0x01/0x08 graphic\n"));

    if ((strstr(config, "wvLaTeX.xml") != NULL) ||
        (strstr(config, "wvCleanLaTeX.xml") != NULL))
        GET_printf_HANDLER()
            ("\n\\resizebox*{\\baselineskip}{!}{\\includegraphics{placeholder.eps}}\
        \n-- %#.2x graphic: StrangeNoGraphicData --",
            graphicstype);
    else
        GET_printf_HANDLER() ("<img alt=\"%#.2x graphic\" src=\"%s\"%s/><br%s/>", graphicstype,
                              "StrangeNoGraphicData", xml_slash, xml_slash);
    return;
}

/* routines for conversion from WMF to EPS or PNG using libwmf(2) library.
 */
int wv_wmfRead(void *);
int wv_wmfSeek(void *, long);
long wv_wmfTell(void *);

void wvConvert_WMF_to_EPS(int, int, char **);
void wvConvert_WMF_to_PNG(int, int, char **);
void wvConvert_PNG_to_EPS(int, int, char **);
void wvConvert_JPG_to_EPS(int, int, char **);

int
wv_wmfRead(void *context) {
    return fgetc((FILE *)context);
}

int
wv_wmfSeek(void *context, long position) {
    return fseek((FILE *)context, position, SEEK_SET);
}

long
wv_wmfTell(void *context) {
    return ftell((FILE *)context);
}

#ifdef HAVE_LIBWMF
 #include <libwmf/api.h>
 #include <libwmf/eps.h>
 #ifdef HAVE_LIBWMF_FOREIGN_H
  #include <libwmf/foreign.h>
 #endif
#endif /* HAVE_LIBWMF */

void
wvConvert_WMF_to_EPS(int width, int height, char **source) {
#ifdef HAVE_LIBWMF
    FILE *in  = 0;
    FILE *out = 0;

    char *sink = 0;

    unsigned long flags;

    wmf_error_t err;

    wmf_eps_t *ddata = 0;

    wmfAPI *API = 0;

    wmfAPI_Options api_options;

    wmfD_Rect bbox;

    in = fopen(*source, "rb");

    if (in == 0)
        return;

    sink = strdup(*source);

    remove_suffix(sink, ".wmf");
    wvAppendStr(&sink, ".eps");

    out = fopen(sink, "wb");

    if (out == 0) {
        wvFree(sink);
        fclose(in);
        return;
    }

    flags = WMF_OPT_IGNORE_NONFATAL | WMF_OPT_FUNCTION;
    api_options.function = wmf_eps_function;

    err = wmf_api_create(&API, flags, &api_options);
    if (err != wmf_E_None)
        goto _wmf_error;

    ddata = WMF_EPS_GetData(API);

    err = wmf_bbuf_input(API, wv_wmfRead, wv_wmfSeek, wv_wmfTell, (void *)in);
    if (err != wmf_E_None)
        goto _wmf_error;

    err = wmf_scan(API, 0, &bbox);
    if (err != wmf_E_None)
        goto _wmf_error;

    ddata->out = wmf_stream_create(API, out);
    if (out == 0)
        goto _wmf_error;

    ddata->bbox = bbox;

    ddata->eps_width  = width;
    ddata->eps_height = height;

    err = wmf_play(API, 0, &bbox);
    if (err != wmf_E_None)
        goto _wmf_error;

    wmf_api_destroy(API);

    fclose(in);
    fclose(out);

    *source = sink;

    return;

_wmf_error:
    if (API)
        wmf_api_destroy(API);

    fclose(in);
    fclose(out);

    wvFree(sink);
#endif  /* HAVE_LIBWMF */
}

#ifdef HAVE_LIBWMF
 #include <libwmf/api.h>
 #include <libwmf/gd.h>
#endif /* HAVE_LIBWMF */

void
wvConvert_WMF_to_PNG(int width, int height, char **source) {
#ifdef HAVE_LIBWMF
    FILE *in  = 0;
    FILE *out = 0;

    char *sink = 0;

    unsigned long flags;

    wmf_error_t err;

    wmf_gd_t *ddata = 0;

    wmfAPI *API = 0;

    wmfAPI_Options api_options;

    wmfD_Rect bbox;

    in = fopen(*source, "rb");

    if (in == 0)
        return;

    sink = strdup(*source);

    remove_suffix(sink, ".wmf");
    wvAppendStr(&sink, ".png");

    out = fopen(sink, "wb");

    if (out == 0) {
        wvFree(sink);
        fclose(in);
        return;
    }

    flags = WMF_OPT_IGNORE_NONFATAL | WMF_OPT_FUNCTION;
    api_options.function = wmf_gd_function;

    err = wmf_api_create(&API, flags, &api_options);
    if (err != wmf_E_None)
        goto _wmf_error;

    ddata = WMF_GD_GetData(API);
    if ((ddata->flags & WMF_GD_SUPPORTS_PNG) == 0)
        goto _wmf_error;

    err = wmf_bbuf_input(API, wv_wmfRead, wv_wmfSeek, wv_wmfTell, (void *)in);
    if (err != wmf_E_None)
        goto _wmf_error;

    err = wmf_scan(API, 0, &bbox);
    if (err != wmf_E_None)
        goto _wmf_error;

    ddata->type = wmf_gd_png;

    ddata->flags |= WMF_GD_OUTPUT_FILE;
    ddata->file   = out;

    ddata->bbox = bbox;

    ddata->width  = width;
    ddata->height = height;

    err = wmf_play(API, 0, &bbox);
    if (err != wmf_E_None)
        goto _wmf_error;

    wmf_api_destroy(API);

    fclose(in);
    fclose(out);

    *source = sink;

    return;

_wmf_error:
    if (API)
        wmf_api_destroy(API);

    fclose(in);
    fclose(out);

    wvFree(sink);
#endif  /* HAVE_LIBWMF */
}

void
wvConvert_PNG_to_EPS(int width, int height, char **source) {
#ifdef HAVE_LIBWMF_FOREIGN_H
    FILE *in  = 0;
    FILE *out = 0;

    char *sink = 0;

    unsigned long flags;

    wmf_error_t err;

    wmf_foreign_t *ddata = 0;

    wmfAPI *API = 0;

    wmfAPI_Options api_options;

    wmfImage image;

    flags = WMF_OPT_IGNORE_NONFATAL | WMF_OPT_FUNCTION;
    api_options.function = wmf_foreign_function;

    err = wmf_api_create(&API, flags, &api_options);
    if (err != wmf_E_None)
        return;

    ddata = WMF_FOREIGN_GetData(API);

    if ((ddata->flags & WMF_FOREIGN_SUPPORTS_PNG) == 0) {
        wmf_api_destroy(API);
        return;
    }

    in = fopen(*source, "rb");

    if (in == 0) {
        wmf_api_destroy(API);
        return;
    }

    if (wmf_image_load_png(API, in, &image) == (-1)) {
        fclose(in);
        wmf_api_destroy(API);
        return;
    }

    fclose(in);

    sink = strdup(*source);

    remove_suffix(sink, ".png");
    wvAppendStr(&sink, ".eps");

    out = fopen(sink, "wb");

    if (out == 0) {
        wvFree(sink);
        wmf_image_free(API, &image);
        wmf_api_destroy(API);
        return;
    }

    wmf_image_save_eps(API, out, &image);

    fclose(out);

    wmf_image_free(API, &image);
    wmf_api_destroy(API);

    *source = sink;

    return;
#endif  /* HAVE_LIBWMF_FOREIGN_H */
}

void
wvConvert_JPG_to_EPS(int width, int height, char **source) {
#ifdef HAVE_LIBWMF_FOREIGN_H
    FILE *in  = 0;
    FILE *out = 0;

    char *sink = 0;

    unsigned long flags;

    wmf_error_t err;

    wmf_foreign_t *ddata = 0;

    wmfAPI *API = 0;

    wmfAPI_Options api_options;

    wmfImage image;

    flags = WMF_OPT_IGNORE_NONFATAL | WMF_OPT_FUNCTION;
    api_options.function = wmf_foreign_function;

    err = wmf_api_create(&API, flags, &api_options);
    if (err != wmf_E_None)
        return;

    ddata = WMF_FOREIGN_GetData(API);

    if ((ddata->flags & WMF_FOREIGN_SUPPORTS_JPEG) == 0) {
        wmf_api_destroy(API);
        return;
    }

    in = fopen(*source, "rb");

    if (in == 0) {
        wmf_api_destroy(API);
        return;
    }

    if (wmf_image_load_jpg(API, in, &image) == (-1)) {
        fclose(in);
        wmf_api_destroy(API);
        return;
    }

    fclose(in);

    sink = strdup(*source);

    remove_suffix(sink, ".jpg");
    wvAppendStr(&sink, ".eps");

    out = fopen(sink, "wb");

    if (out == 0) {
        wvFree(sink);
        wmf_image_free(API, &image);
        wmf_api_destroy(API);
        return;
    }

    wmf_image_save_eps(API, out, &image);

    fclose(out);

    wmf_image_free(API, &image);
    wmf_api_destroy(API);

    *source = sink;

    return;
#endif  /* HAVE_LIBWMF_FOREIGN_H */
}

static void wv_query_eps(const char *format) {
#ifdef HAVE_LIBWMF
    unsigned long flags;

    wmf_error_t err;
 #ifdef HAVE_LIBWMF_FOREIGN_H
    wmf_foreign_t *ddata = 0;
 #endif /* HAVE_LIBWMF_FOREIGN_H */
    wmfAPI         *API = 0;
    wmfAPI_Options api_options;
#endif  /* HAVE_LIBWMF */

    if (format == 0) {
        GET_printf_HANDLER() ("no\n");
        return;
    }

#ifdef HAVE_LIBWMF
    if (strcmp(format, "wmf") == 0) {
        GET_printf_HANDLER() ("yes\n");
        return;
    }
 #ifdef HAVE_LIBWMF_FOREIGN_H
    if (strcmp(format, "png") == 0) {
        flags = WMF_OPT_IGNORE_NONFATAL | WMF_OPT_FUNCTION;
        api_options.function = wmf_foreign_function;

        err = wmf_api_create(&API, flags, &api_options);
        if (err != wmf_E_None) {
            GET_printf_HANDLER() ("no\n");
            return;
        }

        ddata = WMF_FOREIGN_GetData(API);

        if (ddata->flags & WMF_FOREIGN_SUPPORTS_PNG) {
            GET_printf_HANDLER() ("yes\n");
        } else {
            GET_printf_HANDLER() ("no\n");
        }

        wmf_api_destroy(API);
        return;
    }
    if (strcmp(format, "jpg") == 0) {
        flags = WMF_OPT_IGNORE_NONFATAL | WMF_OPT_FUNCTION;
        api_options.function = wmf_foreign_function;

        err = wmf_api_create(&API, flags, &api_options);
        if (err != wmf_E_None) {
            GET_printf_HANDLER() ("no\n");
            return;
        }

        ddata = WMF_FOREIGN_GetData(API);

        if (ddata->flags & WMF_FOREIGN_SUPPORTS_JPEG) {
            GET_printf_HANDLER() ("yes\n");
        } else {
            GET_printf_HANDLER() ("no\n");
        }

        wmf_api_destroy(API);
        return;
    }
 #endif /* HAVE_LIBWMF_FOREIGN_H */
#endif  /* HAVE_LIBWMF */

    GET_printf_HANDLER() ("no\n");
    return;
}

static int Convert_WMF  = 1;
static int Convert_EMF  = 1;
static int Convert_PNG  = 1;
static int Convert_JPG  = 1;
static int Convert_PICT = 1;

static void wv_suppress(const char *format) {
    const char *ptr = format;

    if (format == 0) {
        Convert_WMF  = 1;
        Convert_EMF  = 1;
        Convert_PNG  = 1;
        Convert_JPG  = 1;
        Convert_PICT = 1;

        return;
    }

    while (*ptr) {
        if (strncmp(ptr, "wmf,", 4) == 0) {
            Convert_WMF = 0;
            ptr        += 4;
            continue;
        }
        if (strncmp(ptr, "emf,", 4) == 0) {
            Convert_EMF = 0;
            ptr        += 4;
            continue;
        }
        if (strncmp(ptr, "png,", 4) == 0) {
            Convert_PNG = 0;
            ptr        += 4;
            continue;
        }
        if (strncmp(ptr, "jpg,", 4) == 0) {
            Convert_JPG = 0;
            ptr        += 4;
            continue;
        }
        if (strncmp(ptr, "pict,", 5) == 0) {
            Convert_PICT = 0;
            ptr         += 5;
            continue;
        }

        if (strcmp(ptr, "wmf") == 0) {
            Convert_WMF = 0;
            break;
        }
        if (strcmp(ptr, "emf") == 0) {
            Convert_EMF = 0;
            break;
        }
        if (strcmp(ptr, "png") == 0) {
            Convert_PNG = 0;
            break;
        }
        if (strcmp(ptr, "jpg") == 0) {
            Convert_JPG = 0;
            break;
        }
        if (strcmp(ptr, "pict") == 0) {
            Convert_PICT = 0;
            break;
        }

        GET_fprintf_HANDLER() (stderr, "format(s) `%s' not recognized!\n", ptr);
        break;
    }
}

void
wvPrintGraphics(char *config, int graphicstype, int width, int height,
                char *source) {
    if ((strstr(config, "wvLaTeX.xml") != NULL) ||
        (strstr(config, "wvCleanLaTeX.xml") != NULL)) {
        if (strlen(source) >= 4) {
            if (Convert_WMF && (strcmp(source + strlen(source) - 4, ".wmf") == 0))
                wvConvert_WMF_to_EPS(width, height, &source);
            else if (Convert_PNG && (strcmp(source + strlen(source) - 4, ".png") == 0))
                wvConvert_PNG_to_EPS(width, height, &source);
            else if (Convert_JPG && (strcmp(source + strlen(source) - 4, ".jpg") == 0))
                wvConvert_JPG_to_EPS(width, height, &source);
        }
        remove_suffix(source, ".eps");
        remove_suffix(source, ".wmf");
        remove_suffix(source, ".pict");
        remove_suffix(source, ".png");
        remove_suffix(source, ".jpg");

        /*
           Output to real file name. Conversion to .eps must be done manually for now
         */
        GET_printf_HANDLER() ("\n\\resizebox{%dpt}{%dpt}\
		  {\\includegraphics{%s.eps}}\
		  \n% -- %#.2x graphic -- \n"          , width, height, source, graphicstype);
    } else {
        if (strlen(source) >= 4)
            if (strcmp(source + strlen(source) - 4, ".wmf") == 0)
                wvConvert_WMF_to_PNG(width, height, &source);
        if ((strstr(config, "wvHtml.xml") != NULL) ||
            (strstr(config, "wvWml.xml") != NULL)) {
            GET_printf_HANDLER() ("<img width=\"%d\" height=\"%d\" alt=\"%#.2x graphic\" src=\"%s\"%s/><br%s/>",
                                  width, height, graphicstype, name_to_url(source),
                                  xml_slash, xml_slash);
        } else {
            GET_printf_HANDLER() ("<img width=\"%d\" height=\"%d\" alt=\"%#.2x graphic\" src=\"%s\"%s/><br%s/>",
                                  width, height, graphicstype, source,
                                  xml_slash, xml_slash);
        }
    }
    return;
}

int
mySpecCharProc(wvParseStruct *ps, U16 eachchar, CHP *achp) {
    static int  message;
    PICF        picf;
    FSPA        *fspa;
    expand_data *data = (expand_data *)ps->userData;

    switch (eachchar) {
        case 19:
            wvError(("field began\n"));
            ps->fieldstate++;
            ps->fieldmiddle = 0;
            fieldCharProc(ps, eachchar, 0, 0x400);      /* temp */
            return 0;
            break;

        case 20:
            wvTrace(("field middle\n"));
            if (achp->fOle2) {
                wvError(
                    ("this field has an associated embedded object of id %x\n",
                     achp->fcPic_fcObj_lTagObj));

                /*test = wvFindObject(achp->fcPic_fcObj_lTagObj);
                   if (test)
                   wvError(("data can be found in object entry named %s\n",test->name));
                 */}
            fieldCharProc(ps, eachchar, 0, 0x400);      /* temp */
            ps->fieldmiddle = 1;
            return 0;
            break;

        case 21:
            wvTrace(("field end\n"));
            ps->fieldstate--;
            ps->fieldmiddle = 0;
            fieldCharProc(ps, eachchar, 0, 0x400);      /* temp */
            return 0;
            break;
    }

    if (ps->fieldstate) {
        if (fieldCharProc(ps, eachchar, 0, 0x400))
            return 0;
    }

    switch (eachchar) {
        case 0x05:
            /* this should be handled by the COMMENTBEGIN and COMMENTEND events */
            return 0;
            break;

        case 0x01:
            {
                wvStream *f;
                Blip     blip;
                char     *name;
                long     p = wvStream_tell(ps->data);
                wvError(
                    ("picture 0x01 here, at offset %x in Data Stream, obj is %d, ole is %d\n",
                     achp->fcPic_fcObj_lTagObj, achp->fObj, achp->fOle2));

                if (achp->fOle2)
                    return 0;
                if (!no_graphics) {
                    wvStream_goto(ps->data, achp->fcPic_fcObj_lTagObj);
                    wvGetPICF(wvQuerySupported(&ps->fib, NULL), &picf, ps->data);
                    f = picf.rgb;
                    if (wv0x01(&blip, f, picf.lcb - picf.cbHeader)) {
                        wvTrace(("Here\n"));
                        name = wvHtmlGraphic(ps, &blip);
                        if (ps->dir) chdir(ps->dir);
                        wvPrintGraphics(config, 0x01,
                                        (int)wvTwipsToHPixels(picf.dxaGoal),
                                        (int)wvTwipsToVPixels(picf.dyaGoal),
                                        name);
                        if (ps->dir) chdir(wv_cwd);
                        wvFree(name);
                    } else
                        wvStrangeNoGraphicData(config, 0x01);
                }

                wvStream_goto(ps->data, p);
                return 0;
            }

        case 0x08:
            {
                Blip blip;
                char *name;
                if (wvQuerySupported(&ps->fib, NULL) == WORD8) {
                    if (!no_graphics) {
                        if (ps->nooffspa > 0) {
                            fspa =
                                wvGetFSPAFromCP(ps->currentcp, ps->fspa,
                                                ps->fspapos, ps->nooffspa);

                            if (!fspa) {
                                wvError(("No fspa! Insanity abounds!\n"));
                                return 0;
                            }

                            data->props = fspa;
                            if (wv0x08(&blip, fspa->spid, ps)) {
                                wvTrace(("Here\n"));
                                name = wvHtmlGraphic(ps, &blip);
                                if (ps->dir) chdir(ps->dir);
                                wvPrintGraphics(config, 0x08,
                                                (int)
                                                wvTwipsToHPixels(fspa->xaRight
                                                                 -
                                                                 fspa->
                                                                    xaLeft),
                                                (int)wvTwipsToVPixels(fspa->
                                                                         yaBottom
                                                                      -
                                                                      fspa->
                                                                         yaTop),
                                                name);
                                if (ps->dir) chdir(wv_cwd);
                                wvFree(name);
                            } else
                                wvStrangeNoGraphicData(config, 0x08);
                        } else {
                            wvError(("nooffspa was <=0!  Ignoring.\n"));
                        }
                    }
                } else {
                    FDOA *fdoa;
                    wvError(
                        ("pre word8 0x08 graphic, unsupported at the moment\n"));
                    fdoa =
                        wvGetFDOAFromCP(ps->currentcp, ps->fdoa, ps->fdoapos,
                                        ps->nooffdoa);
                    data->props = fdoa;
                }



#if 0
                if ((fspa) && (data->sd != NULL) &&
                    (data->sd->elements[TT_PICTURE].str) &&
                    (data->sd->elements[TT_PICTURE].str[0] != NULL)) {
                    wvExpand(data, data->sd->elements[TT_PICTURE].str[0],
                             strlen(data->sd->elements[TT_PICTURE].str[0]));
                    if (data->retstring) {
                        wvTrace(
                            ("picture string is now %s",
                             data->retstring));
                        GET_printf_HANDLER() ("%s", data->retstring);
                        wvFree(data->retstring);
                    }
                }
#endif
                return 0;
            }

        case 0x28:
            {
                U16 symbol[6]    = { 'S', 'y', 'm', 'b', 'o', 'l' };
                U16 wingdings[9] =
                { 'W', 'i', 'n', 'g', 'd', 'i', 'n', 'g', 's' };
                U16 mtextra[8] =
                { 'M', 'T', ' ', 'E', 'x', 't', 'r', 'a' };

                wvTrace(
                    ("no of strings %d %d\n", ps->fonts.nostrings,
                     achp->ftcSym));
                if (0 == memcmp(symbol, ps->fonts.ffn[achp->ftcSym].xszFfn, 12)) {
                    if ((!message) && (strcasecmp("UTF-8", charset))) {
                        wvWarning
                            ("Symbol font detected (too late sorry!), rerun wvHtml with option --charset utf-8\n\
option to support correct symbol font conversion to a viewable format.\n");
                        message++;
                    }
                    wvTrace(
                        ("symbol char %d %x %c, using font %d %s\n",
                         achp->xchSym, achp->xchSym, achp->xchSym,
                         achp->ftcSym,
                         wvWideStrToMB(ps->fonts.ffn[achp->ftcSym].
                                          xszFfn)));
                    wvTrace(
                        ("symbol char ends up as a unicode %x\n",
                         wvConvertSymbolToUnicode(achp->xchSym - 61440)));
                    wvOutputFromUnicode(wvConvertSymbolToUnicode
                                            (achp->xchSym - 61440), charset);
                    return 0;
                } else if (0 ==
                           memcmp(mtextra, ps->fonts.ffn[achp->ftcSym].xszFfn,
                                  16)) {
                    if ((!message) && (strcasecmp("UTF-8", charset))) {
                        wvWarning
                            ("MT Extra font detected (too late sorry!), rerun wvHtml with option --charset utf-8\n\
option to support correct symbol font conversion to a viewable format.\n");
                        message++;
                    }
                    wvTrace(
                        ("Symbol char %d %x %c, using font %d %s\n",
                         achp->xchSym, achp->xchSym, achp->xchSym,
                         achp->ftcSym,
                         wvWideStrToMB(ps->fonts.ffn[achp->ftcSym].
                                          xszFfn)));
                    wvTrace(
                        ("symbol char ends up as a unicode %x\n",
                         wvConvertMTExtraToUnicode(achp->xchSym - 61440)));
                    wvOutputFromUnicode(wvConvertMTExtraToUnicode
                                            (achp->xchSym - 61440), charset);
                    return 0;
                } else if (0 ==
                           memcmp(wingdings, ps->fonts.ffn[achp->ftcSym].xszFfn,
                                  18)) {
                    if (!message) {
                        wvError(
                            ("I have yet to do a wingdings to unicode mapping table, if you know of one tell me\n"));
                        message++;
                    }
                } else {
                    if (!message) {
                        char *fontname =
                            wvWideStrToMB(ps->fonts.ffn[achp->ftcSym].
                                             xszFfn);
                        wvError(
                            ("Special font %s, i need a mapping table to unicode for this\n",
                             fontname));
                        wvFree(fontname);
                        GET_printf_HANDLER() ("*");
                    }
                    return 0;
                }
            }

        default:
            return 0;
    }



    return 0;
}

int
myCharProc(wvParseStruct *ps, U16 eachchar, U8 chartype, U16 lid) {
    switch (eachchar) {
        case 19:
            wvTrace(("field began\n"));
            ps->fieldstate++;
            ps->fieldmiddle = 0;
            fieldCharProc(ps, eachchar, chartype, lid); /* temp */
            return 0;
            break;

        case 20:
            wvTrace(("field middle\n"));
            fieldCharProc(ps, eachchar, chartype, lid);
            ps->fieldmiddle = 1;
            return 0;
            break;

        case 21:
            wvTrace(("field began\n"));
            ps->fieldmiddle = 0;
            ps->fieldstate--;
            fieldCharProc(ps, eachchar, chartype, lid); /* temp */
            return 0;
            break;

        case 0x08:
            wvError(
                ("hmm did we loose the fSpec flag ?, this is possibly a bug\n"));
            break;
    }

    if (ps->fieldstate) {
        if (fieldCharProc(ps, eachchar, chartype, lid))
            return 0;
    }

    wvTrace(
        ("charset is %s, lid is %x, type is %d, char is %x\n", charset,
         lid, chartype, eachchar));

    if ((chartype) && (wvQuerySupported(&ps->fib, NULL) == WORD8))
        wvTrace(("lid is %x\n", lid));

    if (charset != NULL)
        wvOutputHtmlChar(eachchar, chartype, charset, lid);
    else
        wvOutputHtmlChar(eachchar, chartype, wvAutoCharset(ps), lid);
    return 0;
}

int
wvOpenConfig(state_data *myhandle, char *config) {
    static char buf[BUFSIZ] = "";
    FILE        *tmp;
    int         i = 0;

    if (config == NULL)
        config = "wvHtml.xml";
    else
        i = 1;
    tmp = fopen(config, "rb");

    if (tmp == NULL) {
        str_copy(buf, sizeof(buf), WVDATADIR);
        str_append(buf, sizeof(buf), "/");
        str_append(buf, sizeof(buf), config);
        config = buf;
        tmp    = fopen(config, "rb");
    }

    if (tmp == NULL) {
        if (i)
            wvError(
                ("Attempt to open %s failed, using %s\n", config,
                 HTMLCONFIG));
        config = HTMLCONFIG;
        tmp    = fopen(config, "rb");
    }
    myhandle->path = config;
    myhandle->fp   = tmp;
    return tmp == NULL ? 0 : 1;
}

char *figure_name(wvParseStruct *ps) {
    static int  number;
    static char *b_name = 0;
    char        *f_name = 0;
    char        buffer[10];

    if (b_name == 0) {
        if (wv_arg_basename) {
            b_name = strdup(wv_arg_basename);
#ifdef WV_REMOVE_SUFFIX
            if (b_name) { /* remove any suffix */
                char *dot = 0;
                char *ptr = b_name;
                while (*ptr) {
                    if (*ptr == '.') dot = ptr;
                    ptr++;
                }
                if (dot) *dot = 0;
            }
#endif          /* WV_REMOVE_SUFFIX */
        } else {
            b_name = strdup(base_name(ps->filename));
            if (b_name) { /* remove '.doc' suffix; case insensitive */
                if (strlen(b_name) >= 4) {
                    char *dot = b_name + strlen(b_name) - 4;
                    if (strcasecmp(dot, ".doc") == 0) *dot = 0;
                }
            }
        }
    }

    if (b_name == 0) {
        //fprintf (stderr,"error: unable to create basename!");
        //exit (1);
        //return
        b_name = "__defbname";
    }

    f_name = strdup(b_name);
    if (f_name) {
        sprintf(buffer, "%d", number++);
        wvAppendStr(&f_name, buffer);
    } else {
        //fprintf (stderr,"error: unable to create filename!");
        //exit (1);
        return f_name;
    }

    return f_name;
}

char *name_to_url(char *name) {
    static char *url  = 0;
    static long max   = 0;
    char        *ptr  = 0;
    long        count = 0;

    ptr = name;
    while (*ptr) {
        switch (*ptr) {
            case ' ':
                count += 3;
                break;

            default:
                count++;
                break;
        }
        ptr++;
    }
    count++;

    if (count > max) {
        char *more = 0;
        if (url == 0) {
            more = (char *)malloc(count);
        } else {
            more = (char *)realloc(url, count);
        }
        if (more) {
            url = more;
            max = count;
        }
    }

    if (url) {
        count = 0;
        ptr   = name;
        while (*ptr && (count < max)) {
            switch (*ptr) {
                case ' ':
                    url[count++] = '%';
                    if (count < max) url[count++] = '2';
                    if (count < max) url[count++] = '0';
                    break;

                default:
                    url[count++] = *ptr;
                    break;
            }
            ptr++;
        }
        url[max - 1] = 0;
    } else {
        wvError(("failed to convert name to URL\n"));
        return name;
    }

    return url;
}
} // "extern C {"

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    //g_type_init ();
    InvokePtr = Invoke;
#ifdef _WIN32
    static_path      = GetDirectory() + (char *)"..\\share\\wv";
    static_path_html = static_path + "\\wvHtml.xml";
#endif
    return 0;
}
//------------------------------------------------------------------------
// string MSWConvert(filename_in [,var error][, password][, charset][, nographics][, dir][, basename][, configfile][, xml][, var err_stream])
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MSWRead, 1, 10)
    T_STRING(0) // filename_in

    wvParseStruct ps;
    state_data  myhandle;
    expand_data expandhandle;
    int         ret = 0;

    char *password = NULL;
    char *dir      = NULL;

    config          = config_Base;
    xml_output      = 0;
    no_graphics     = 0;
    wv_arg_basename = 0;
    charset         = NULL;
    wv_cwd[0]       = 0;

    t_result.LoadBuffer("", 0);
    t_error.LoadBuffer("", 0);

    RETURN_STRING("");

    if (PARAMETERS_COUNT > 1) {
        SET_STRING(1, "");

        if (PARAMETERS_COUNT > 9) {
            SET_STRING(9, "");
        }
    }
    if (PARAMETERS_COUNT > 2) {
        T_STRING(2); // password
        if (PARAM_LEN(2))
            password = PARAM(2);
        else
            password = NULL;
    }
    if (PARAMETERS_COUNT > 3) {
        T_STRING(3); // charset
        if (PARAM_LEN(3))
            charset = PARAM(3);
        else
            charset = NULL;
    }
    if (PARAMETERS_COUNT > 4) {
        T_NUMBER(4); // nographics
        no_graphics = (PARAM_INT(4) != 0);
    }
    if (PARAMETERS_COUNT > 5) {
        T_STRING(5); // dir
        if (PARAM_LEN(5))
            dir = PARAM(5);
        else
            dir = NULL;
    }
    if (PARAMETERS_COUNT > 6) {
        T_STRING(6); // basename
        if (PARAM_LEN(6))
            wv_arg_basename = PARAM(6);
        else
            wv_arg_basename = NULL;
    }

    if (PARAMETERS_COUNT > 7) {
        T_STRING(7); // configfile
        if (PARAM_LEN(7))
            config = PARAM(7);
        else
            config = NULL;
    }
    if (PARAMETERS_COUNT > 8) {
        T_NUMBER(8); // nographics
        if (PARAM_INT(8)) {
            config     = "wvXml.xml";
            charset    = "utf-8";
            xml_output = 1;
            xml_slash  = " /";
        }
    }


    getcwd(wv_cwd, 4096);
    wv_cwd[4096] = 0;

    wvInit();
    ret         = wvInitParser(&ps, PARAM(0));
    ps.filename = PARAM(0);
    ps.dir      = dir;

    if (ret & 0x8000) {         /* Password protected? */
        if ((ret & 0x7fff) == WORD8) {
            ret = 0;
            if (password == NULL) {
                if (PARAMETERS_COUNT > 1) {
                    SET_STRING(1, "Password required, this is an encrypted document");
                }
                GET_fprintf_HANDLER() (stderr,
                                       "Password required, this is an encrypted document\n");
                return 0;
            } else {
                wvSetPassword(password, &ps);
                if (wvDecrypt97(&ps)) {
                    if (PARAMETERS_COUNT > 1) {
                        SET_STRING(1, "Incorrect Password");
                    }
                    wvError(("Incorrect Password\n"));
                    return 0;
                }
            }
        } else if (((ret & 0x7fff) == WORD7) || ((ret & 0x7fff) == WORD6)) {
            ret = 0;
            if (password == NULL) {
                if (PARAMETERS_COUNT > 1) {
                    SET_STRING(1, "Password required, this is an encrypted document");
                }
                GET_fprintf_HANDLER() (stderr,
                                       "Password required, this is an encrypted document\n");
                return 0;
            } else {
                wvSetPassword(password, &ps);
                if (wvDecrypt95(&ps)) {
                    if (PARAMETERS_COUNT > 1) {
                        SET_STRING(1, "Incorrect Password");
                    }
                    wvError(("Incorrect Password\n"));
                    return 0;
                }
            }
        }
    }

    if (ret) {
        if (PARAMETERS_COUNT > 1) {
            if (ret == 3) {
                SET_STRING(1, "Cannot open file");
            } else {
                AnsiString err("Startup error ");
                err += AnsiString((long)ret);
                SET_STRING(1, err.c_str());
            }
        }
        wvError(("startup error #%d\n", ret));
        wvOLEFree(&ps);
        return 0;
    }

    wvSetElementHandler(&ps, myelehandler);
    wvSetDocumentHandler(&ps, mydochandler);
    wvSetCharHandler(&ps, myCharProc);
    wvSetSpecialCharHandler(&ps, mySpecCharProc);
    wvSetPrintFHandler(MyPrintF);
    wvSetFPrintFHandler(MyFPrintF);

    wvInitStateData(&myhandle);

    if (wvOpenConfig(&myhandle, config) == 0) {
        if (PARAMETERS_COUNT > 1) {
            SET_STRING(1, "Config file not found");
        }
        wvError(("config file not found\n"));
        if (PARAMETERS_COUNT > 9) {
            SET_BUFFER(9, t_error.c_str(), t_error.Length());
        }
        return 0;
    } else {
        wvTrace(("x for FILE is %x\n", myhandle.fp));
        ret = wvParseConfig(&myhandle);
    }

    if (!ret) {
        expandhandle.sd = &myhandle;
        ps.userData     = &expandhandle;
        ret             = wvHtml(&ps);
    }
    wvReleaseStateData(&myhandle);

    if (ret == 2) {
        if (PARAMETERS_COUNT > 1) {
            AnsiString err("Shutdown error ");
            err += AnsiString((long)ret);
            SET_STRING(1, err.c_str());
        }
        if (PARAMETERS_COUNT > 9) {
            SET_BUFFER(9, t_error.c_str(), t_error.Length());
        }
        return 0;
    } else
    if (ret != 0)
        ret = -1;
    wvOLEFree(&ps);
    wvShutdown();
    if ((PARAMETERS_COUNT > 1) && (ret)) {
        AnsiString err("Unspecified error ");
        err += AnsiString((long)ret);
        SET_STRING(1, err.c_str());
    }
    if (PARAMETERS_COUNT > 9) {
        SET_BUFFER(9, t_error.c_str(), t_error.Length());
    }
    RETURN_BUFFER(t_result.c_str(), t_result.Length());
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MSWMeta, 1, 2)
    T_STRING(0) // filename_in
    GHashTable * human_readable_keys;
    int i;

    if (PARAMETERS_COUNT > 1) {
        SET_STRING(1, "");
    }

    Invoke(INVOKE_CREATE_ARRAY, RESULT);
    wvInit();
    human_readable_keys = g_hash_table_new(g_str_hash, g_str_equal);

    for (i = 0; i < nr_metadata_names; i++)
        g_hash_table_insert(human_readable_keys, metadata_names[i].metadata_key, _(metadata_names[i].human_readable_key));

    GsfInput  *input;
    GsfInfile *msole;
    GError    *err = NULL;

    input = gsf_input_stdio_new(PARAM(0), &err);

    if (!input) {
        if (PARAMETERS_COUNT > 1) {
            if (err->message) {
                SET_STRING(1, err->message);
            } else {
                SET_STRING(1, "Problem with getting metadata");
            }
        }
        g_error_free(err);
    } else {
        input = gsf_input_uncompress(input);
        msole = gsf_infile_msole_new(input, &err);

        if (!msole) {
            if (PARAMETERS_COUNT > 1) {
                if (err->message) {
                    SET_STRING(1, err->message);
                } else {
                    SET_STRING(1, "Problem with getting metadata(ole)");
                }
            }
            g_error_free(err);
        } else {
            print_summary_stream(msole, PARAM(0), "\05SummaryInformation", human_readable_keys, RESULT);
            print_summary_stream(msole, PARAM(0), "\05DocumentSummaryInformation", human_readable_keys, RESULT);
        }
        if (msole)
            g_object_unref(G_OBJECT(msole));
        if (input)
            g_object_unref(G_OBJECT(input));
    }
    if (human_readable_keys)
        g_hash_table_destroy(human_readable_keys);
    wvShutdown();

END_IMPL
