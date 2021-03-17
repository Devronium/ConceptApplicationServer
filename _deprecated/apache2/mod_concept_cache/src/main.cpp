extern "C" {
#include "httpd.h"
#include "http_config.h"
#include "http_main.h"
#include "http_protocol.h"
#include "http_core.h"
#include "apr_strings.h"
//#include "apr_buckets.h"
#include "http_log.h"

#define PROTO_LIST(PROTOTYPE)    PROTOTYPE
#define UINT4    unsigned long
//#include "md5/md5.h"
#include "time.h"

#include <GeoIP.h>
#include <GeoIPCity.h>
}

#ifdef _WIN32
 #include <windows.h>
 #include "win32hack.h"
#endif

#include <libmemcached/memcached.h>

#include "AnsiString.h"
#include <stdlib.h>

#ifndef CONC_DEFAULT_HOST_STRING
 #define CONC_DEFAULT_HOST_STRING    "127.0.0.1"
#endif

#ifndef CONC_DEFAULT_HOST_PORT
 #define CONC_DEFAULT_HOST_PORT    MEMCACHED_DEFAULT_PORT
#endif

#ifndef CONC_DEFAULT_COOKIE_KEY
 #define CONC_DEFAULT_COOKIE_KEY    "QSESSIONID"
#endif

#ifndef CONC_DEFAULT_CONTENT_TYPE
 #define CONC_DEFAULT_CONTENT_TYPE    "Content-Type"
#endif

#ifndef CONC_DEFAULT_HEADERS
 #define CONC_DEFAULT_HEADERS    "Headers"
#endif

#ifndef CONC_DEFAULT_COUNTRY_CACHE
 #define CONC_DEFAULT_COUNTRY_CACHE    0
#endif

extern "C" {
static void *create_mod_concept_cache_config(apr_pool_t *p, server_rec *s);
static int mod_concept_cache_method_handler(request_rec *r);
static const char *set_mod_concept_cache_string(cmd_parms *parms, void *mconfig, const char *arg, const char *arg2);
static void mod_concept_cache_register_hooks(apr_pool_t *p);

static const char *set_mod_concept_cookie_string(cmd_parms *parms, void *mconfig, const char *arg);
static const char *set_mod_concept_contenttype_string(cmd_parms *parms, void *mconfig, const char *arg);
static const char *set_mod_concept_headers_string(cmd_parms *parms, void *mconfig, const char *arg);
static const char *set_mod_concept_country_string(cmd_parms *parms, void *mconfig, const char *arg);
static const char *set_mod_concept_geoip_string(cmd_parms *parms, void *mconfig, const char *arg);
}

static GeoIP *geoip = 0;

typedef struct {
    char *host;
    int  port;
    char *sessionkey;
    char *contenttype;
    char *headers;
    char CacheByCountry;
    char *GeoIPPath;
} mod_concept_cache_config;

static const command_rec mod_concept_cache_cmds[] =
{
    AP_INIT_TAKE2(
        "MemCachedHost",
        (const char *(*)())set_mod_concept_cache_string,
        NULL,
        OR_ALL,
        "MemCached hostname, MemCached port"
        ),
    AP_INIT_TAKE1(
        "UseCookieKey",
        (const char *(*)())set_mod_concept_cookie_string,
        NULL,
        OR_ALL,
        "UseCookieKey cookiename"
        ),
    AP_INIT_TAKE1(
        "ContentTypeKey",
        (const char *(*)())set_mod_concept_contenttype_string,
        NULL,
        OR_ALL,
        "ContentTypeKey key"
        ),
    AP_INIT_TAKE1(
        "HeadersKey",
        (const char *(*)())set_mod_concept_headers_string,
        NULL,
        OR_ALL,
        "HeadersKey key"
        ),
    AP_INIT_TAKE1(
        "CacheByCountry",
        (const char *(*)())set_mod_concept_country_string,
        NULL,
        OR_ALL,
        "CacheByCountry 1|0"
        ),
    AP_INIT_TAKE1(
        "GeoIPDatabase",
        (const char *(*)())set_mod_concept_geoip_string,
        NULL,
        OR_ALL,
        "GeoIPDatabase path"
        ),
    { NULL }
};

module AP_MODULE_DECLARE_DATA concept_cache_module =
{
    STANDARD20_MODULE_STUFF,
    NULL,
    NULL,
    create_mod_concept_cache_config,
    NULL,
    mod_concept_cache_cmds,
    mod_concept_cache_register_hooks,
};

static memcached_st *DoConnect(request_rec *r) {
    memcached_st *c = memcached_create(0);

    if (c) {
        mod_concept_cache_config *s_cfg = (mod_concept_cache_config *)ap_get_module_config(r->server->module_config, &concept_cache_module);
        memcached_server_add(c, s_cfg->host, s_cfg->port);
    }
    return c;
}

static void DoDisconnect(memcached_st *c) {
    if (c)
        memcached_free(c);
}

#ifdef _WIN32
static AnsiString GetDirectory() {
    char buffer[4096];

    buffer[0] = 0;
    GetModuleFileName(NULL, buffer, 4096);
    for (int i = strlen(buffer) - 1; i >= 0; i--)
        if ((buffer[i] == '/') || (buffer[i] == '\\')) {
            buffer[i + 1] = 0;
            break;
        }
    return AnsiString(buffer);
}
#endif

static AnsiString GetCookie(const char *data, char *cookie_name) {
    AnsiString res;

    if ((!data) || (!data[0]))
        return res;
    if ((!cookie_name) || (!cookie_name[0]))
        return res;

    AnsiString cookie_term(cookie_name);
    cookie_term += (char *)"=";
    char *data_pos = strstr(data, cookie_term.c_str());
    if (data_pos) {
        char *data_end = strchr(data_pos, ';');
        if (data_end) {
            int len = (long)data_end - (long)data_pos;
            res.LoadBuffer(data_pos + cookie_term.Length(), len);
        } else
            res = data_pos + cookie_term.Length();
    }
    return res;
}

void SetHeader(request_rec *r, const char *key, const char *value) {
    if (!strcasecmp(key, "content-type"))
        ap_set_content_type(r, value);
    else
        apr_table_set(r->headers_out, key, value);
}

/*static AnsiString domd5(AnsiString *key) {
 *  MD5_CTX CTX;
 *  unsigned char md5_sum[16];
 *
 *  MD5Init(&CTX);
 *  MD5Update(&CTX,(unsigned char*)key->c_str(),(long)key->Length());
 *  MD5Final(md5_sum,&CTX);
 *
 *  unsigned char result[33];
 *  result[32]=0;
 *  for (int i=0;i<16;i++) {
 *              unsigned char first = md5_sum[i] / 0x10;
 *              unsigned char sec   = md5_sum[i] % 0x10;
 *
 *              if (first<10)
 *                      result[i*2]='0'+first;
 *              else
 *                      result[i*2]='a'+(first-10);
 *
 *              if (sec<10)
 *                      result[i*2+1]='0'+sec;
 *              else
 *                      result[i*2+1]='a'+(sec-10);
 *  }
 *  return AnsiString((char *)result);
 * }
 */

unsigned int hash(AnsiString *key_s) {
    char *key = key_s->c_str();
    int  len  = key_s->Length();

    if ((!key) || (!len))
        return 0;
    const unsigned int  m     = 0x5bd1e995;
    const int           r     = 24;
    unsigned int        seed  = 0x7870AAFF;
    const unsigned char *data = (const unsigned char *)key;

    unsigned int h = seed ^ len;

    while (len >= 4) {
        unsigned int k = *(unsigned int *)data;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len  -= 4;
    }

    switch (len) {
        case 3:
            h ^= data[2] << 16;

        case 2:
            h ^= data[1] << 8;

        case 1:
            h ^= data[0];
            h *= m;
    }

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;
    return h;
}

static int Do(memcached_st *memc, request_rec *r) {
    size_t             len   = 0;
    size_t             len2  = 0;
    size_t             len3  = 0;
    unsigned int       flags = 0;
    memcached_return_t mem_error;
    AnsiString         masterkey;
    AnsiString         key;
    AnsiString         typemaster;

    const char *host = apr_table_get(r->headers_in, "Host");

    if (host)
        masterkey = (char *)host;
    else
        masterkey = (char *)r->hostname;
    masterkey += (char *)r->uri;
    if (r->args) {
        masterkey += (char *)"?";
        masterkey += (char *)r->args;
    }

    char buf[0xFF];
    sprintf(buf, "%u", hash(&masterkey));
    masterkey = buf;
    AnsiString superkey((char *)"*");
    superkey += masterkey;

    mod_concept_cache_config *s_cfg = (mod_concept_cache_config *)ap_get_module_config(r->server->module_config, &concept_cache_module);
    const char *cookie = apr_table_get(r->headers_in, "Cookie");
    AnsiString typekey((char *)s_cfg->contenttype);
    AnsiString headerskey((char *)s_cfg->headers);

    typemaster  = typekey;
    typemaster += (char *)"@";
    typemaster += masterkey;

    headerskey += (char *)"@";
    headerskey += masterkey;

    char *ret = memcached_get(memc, superkey.c_str(), superkey.Length(), &len, &flags, &mem_error);
    if (mem_error != MEMCACHED_SUCCESS) {
        if ((s_cfg->CacheByCountry) && (!geoip)) {
            if ((s_cfg->GeoIPPath) && (s_cfg->GeoIPPath[0])) {
                geoip = GeoIP_open(s_cfg->GeoIPPath, GEOIP_STANDARD);
#ifdef _WIN32
                if (!geoip)
                    geoip = GeoIP_open(GetDirectory() + s_cfg->GeoIPPath, GEOIP_STANDARD);
#endif
            }
            if (!geoip)
                geoip = GeoIP_new(GEOIP_STANDARD);
            if (geoip)
                ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, r->server, "GeoIP successfully initialised");
        }


        AnsiString the_key;

        if (cookie) {
            key = GetCookie(cookie, s_cfg->sessionkey);

            AnsiString cookie_check = key;
            the_key += (char *)"(queryuser)";
            the_key += masterkey;

            char *val = memcached_get(memc, cookie_check.c_str(), cookie_check.Length(), &len3, &flags, &mem_error);
            if (mem_error == MEMCACHED_SUCCESS) {
                if (val[0] == '0') {
                    free(val);
                    // if val is 0, the session is invalidated ... refresh !
                    return DECLINED;
                }
                if (val)
                    free(val);
            } else
                cookie = 0;
        }
        if (cookie) {
            the_key  = key;
            the_key += (char *)"@";
            the_key += masterkey;
        } else
        if (geoip) {
            AnsiString geo_check = (char *)GeoIP_country_code_by_addr(geoip, r->connection->remote_ip);
            if (geo_check.Length()) {
                geo_check += (char *)"(querylang)";
                geo_check += masterkey;

                char *val = memcached_get(memc, geo_check.c_str(), geo_check.Length(), &len3, &flags, &mem_error);
                if (mem_error == MEMCACHED_SUCCESS) {
                    if (val[0] == '0') {
                        // if val is 0, the session is invalidated ... refresh !
                        free(val);
                        return DECLINED;
                    }
                    if (val)
                        free(val);
                }
                the_key  = geo_check;
                the_key += (char *)"(lang)";
                the_key += masterkey;
            } else
                the_key = masterkey;
        } else
            the_key = masterkey;

        //char *ret=memcached_get_by_key(memc, masterkey.c_str(), masterkey.Length(), key.c_str(), key.Length(), &len, &flags, &mem_error);
        ret = memcached_get(memc, the_key.c_str(), the_key.Length(), &len, &flags, &mem_error);
        if (mem_error != MEMCACHED_SUCCESS)
            return DECLINED;
    }

    /*if (!len) {
     *  if (ret)
     *      free(ret);
     *  return DECLINED;
     * }*/
    //char *type=memcached_get_by_key(memc, masterkey.c_str(), masterkey.Length(), typekey.c_str(), typekey.Length(), &len2, &flags, &mem_error);
    char *type = memcached_get(memc, typemaster.c_str(), typemaster.Length(), &len2, &flags, &mem_error);
    if ((mem_error == MEMCACHED_SUCCESS) && (type) && (type[0]))
        // type is zero-terminated
        ap_set_content_type(r, type);
    else
        ap_set_content_type(r, "text/html");

    char *hdr = memcached_get(memc, headerskey.c_str(), headerskey.Length(), &len3, &flags, &mem_error);
    if ((mem_error == MEMCACHED_SUCCESS) && (hdr) && (hdr[0])) {
        AnsiString h_key;
        AnsiString h_value;
        bool       do_value = false;
        char       c;
        for (int i = 0; i <= len3; i++) {
            c = (i == len3) ? 0 : hdr[i];
            if (do_value) {
                if ((c == '\r') || (c == '\n') || (!c)) {
                    if ((h_key.Length()) && (h_value.Length()))
                        SetHeader(r, h_key.c_str(), h_value.c_str());
                    h_key    = (char *)"";
                    h_value  = (char *)"";
                    do_value = false;
                    if ((i < len3) && (c == '\r') && (hdr[i + 1] == '\n'))
                        i++;
                } else
                    h_value += c;
            } else {
                if (c == ':')
                    do_value = true;
                else
                    h_key += c;
            }
        }
        free(hdr);
    }
    if (len)
        ap_rwrite(ret, len, r);
    if (ret)
        free(ret);
    return OK;
}

static int iterate_func(void *req, const char *key, const char *value) {
    int         stat;
    char        *line;
    request_rec *r = (request_rec *)req;

    if ((key == NULL) || (value == NULL) || (value[0] == '\0'))
        return 1;

    line = apr_psprintf(r->pool, "%s => %s<br>\n", key, value);
    stat = ap_rputs(line, r);
    return 1;
}

static int mod_concept_cache_method_handler(request_rec *r) {
    int res = DECLINED;

    if (r->method_number != M_GET)
        return DECLINED;
    memcached_st *c = DoConnect(r);
    res = Do(c, r);
    DoDisconnect(c);

    /*apr_table_do(iterate_func, r, r->headers_in, NULL);
     * ap_rputs(apr_psprintf(r->pool, "Hostname => %s<br>\n", r->hostname), r);
     * ap_rputs(apr_psprintf(r->pool, "URI => %s<br>\n", r->uri), r);
     * ap_rputs(apr_psprintf(r->pool, "Path => %s<br>\n", r->path_info), r);
     * ap_rputs(apr_psprintf(r->pool, "UURI => %s<br>\n", r->unparsed_uri), r);
     * ap_rputs(apr_psprintf(r->pool, "Filename => %s<br>\n", r->filename), r);
     * ap_rputs(apr_psprintf(r->pool, "Args => %s<br>\n", r->args), r);*/

    return res;
}

static void mod_concept_cache_register_hooks(apr_pool_t *p) {
    ap_hook_handler(mod_concept_cache_method_handler, NULL, NULL, APR_HOOK_LAST);
}

static const char *set_mod_concept_cache_string(cmd_parms *parms, void *mconfig, const char *arg, const char *arg2) {
    mod_concept_cache_config *s_cfg = (mod_concept_cache_config *)ap_get_module_config(parms->server->module_config, &concept_cache_module);

    s_cfg->host = (char *)arg;
    s_cfg->port = atoi(arg2);
    return NULL;
}

static const char *set_mod_concept_cookie_string(cmd_parms *parms, void *mconfig, const char *arg) {
    mod_concept_cache_config *s_cfg = (mod_concept_cache_config *)ap_get_module_config(parms->server->module_config, &concept_cache_module);

    s_cfg->sessionkey = (char *)arg;
    return NULL;
}

static const char *set_mod_concept_contenttype_string(cmd_parms *parms, void *mconfig, const char *arg) {
    mod_concept_cache_config *s_cfg = (mod_concept_cache_config *)ap_get_module_config(parms->server->module_config, &concept_cache_module);

    s_cfg->contenttype = (char *)arg;
    return NULL;
}

static const char *set_mod_concept_headers_string(cmd_parms *parms, void *mconfig, const char *arg) {
    mod_concept_cache_config *s_cfg = (mod_concept_cache_config *)ap_get_module_config(parms->server->module_config, &concept_cache_module);

    s_cfg->headers = (char *)arg;
    return NULL;
}

static const char *set_mod_concept_country_string(cmd_parms *parms, void *mconfig, const char *arg) {
    mod_concept_cache_config *s_cfg = (mod_concept_cache_config *)ap_get_module_config(parms->server->module_config, &concept_cache_module);

    if (arg)
        s_cfg->CacheByCountry = (arg[0] == '1');
    else
        s_cfg->CacheByCountry = 0;
    return NULL;
}

static const char *set_mod_concept_geoip_string(cmd_parms *parms, void *mconfig, const char *arg) {
    mod_concept_cache_config *s_cfg = (mod_concept_cache_config *)ap_get_module_config(parms->server->module_config, &concept_cache_module);

    s_cfg->GeoIPPath = (char *)arg;
    return NULL;
}

static void *create_mod_concept_cache_config(apr_pool_t *p, server_rec *s) {
    mod_concept_cache_config *newcfg;

    newcfg                 = (mod_concept_cache_config *)apr_pcalloc(p, sizeof(mod_concept_cache_config));
    newcfg->host           = CONC_DEFAULT_HOST_STRING;
    newcfg->port           = CONC_DEFAULT_HOST_PORT;
    newcfg->sessionkey     = CONC_DEFAULT_COOKIE_KEY;
    newcfg->contenttype    = CONC_DEFAULT_CONTENT_TYPE;
    newcfg->headers        = CONC_DEFAULT_HEADERS;
    newcfg->CacheByCountry = CONC_DEFAULT_COUNTRY_CACHE;
    newcfg->GeoIPPath      = 0;
    return (void *)newcfg;
}
