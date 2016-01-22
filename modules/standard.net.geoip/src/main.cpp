//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"
#include <string.h>
extern "C" {
#include <GeoIP.h>
#include <GeoIPCity.h>
}

#ifdef _WIN32
 #include "win32hack.h"
 #include <Ws2tcpip.h>
WSADATA wsa;
#else
 #include <arpa/inet.h>
#endif

#ifdef _WIN32
 #define CONVERT_ADDR       \
    geoipv6_t st;           \
    int stlen = sizeof(st); \
    WSAStringToAddressA(PARAM(1), AF_INET6, NULL, (struct sockaddr *)&st, &stlen);
#else
 #define CONVERT_ADDR    geoipv6_t st; inet_pton(AF_INET6, PARAM(1), &st);
#endif

//------------------------------------------------------------------------
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    DEFINE_ECONSTANT(GEOIP_STANDARD)
    DEFINE_ECONSTANT(GEOIP_MEMORY_CACHE)
    DEFINE_ECONSTANT(GEOIP_CHECK_CACHE)
    DEFINE_ECONSTANT(GEOIP_INDEX_CACHE)
    DEFINE_ECONSTANT(GEOIP_MMAP_CACHE)

    DEFINE_ECONSTANT(GEOIP_COUNTRY_EDITION)
    DEFINE_ECONSTANT(GEOIP_REGION_EDITION_REV0)
    DEFINE_ECONSTANT(GEOIP_CITY_EDITION_REV0)
    DEFINE_ECONSTANT(GEOIP_ORG_EDITION)
    DEFINE_ECONSTANT(GEOIP_ISP_EDITION)
    DEFINE_ECONSTANT(GEOIP_CITY_EDITION_REV1)
    DEFINE_ECONSTANT(GEOIP_REGION_EDITION_REV1)
    DEFINE_ECONSTANT(GEOIP_PROXY_EDITION)
    DEFINE_ECONSTANT(GEOIP_ASNUM_EDITION)
    DEFINE_ECONSTANT(GEOIP_NETSPEED_EDITION)
    DEFINE_ECONSTANT(GEOIP_DOMAIN_EDITION)
    DEFINE_ECONSTANT(GEOIP_COUNTRY_EDITION_V6)

    DEFINE_ECONSTANT(GEOIP_ANON_PROXY)
    DEFINE_ECONSTANT(GEOIP_HTTP_X_FORWARDED_FOR_PROXY)
    DEFINE_ECONSTANT(GEOIP_HTTP_CLIENT_IP_PROXY)

    DEFINE_ECONSTANT(GEOIP_UNKNOWN_SPEED)
    DEFINE_ECONSTANT(GEOIP_DIALUP_SPEED)
    DEFINE_ECONSTANT(GEOIP_CABLEDSL_SPEED)
    DEFINE_ECONSTANT(GEOIP_CORPORATE_SPEED)
#ifdef _WIN32
    WSAStartup(MAKEWORD(1, 0), &wsa);
#endif
    return 0;
}
//------------------------------------------------------------------------
#define RETURN_RECORD(rec)                                                                                                                    \
    Invoke(INVOKE_CREATE_ARRAY, RESULT);                                                                                                      \
    if (rec) {                                                                                                                                \
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"country_code", (INTEGER)VARIABLE_STRING, rec->country_code, (double)0);      \
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"country_code3", (INTEGER)VARIABLE_STRING, rec->country_code3, (double)0);    \
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"country_name", (INTEGER)VARIABLE_STRING, rec->country_name, (double)0);      \
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"region", (INTEGER)VARIABLE_STRING, rec->region, (double)0);                  \
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"city", (INTEGER)VARIABLE_STRING, rec->city, (double)0);                      \
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"latitude", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)rec->latitude);     \
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"longitude", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)rec->longitude);   \
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"metro_code", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)rec->metro_code); \
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"dma_code", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)rec->dma_code);     \
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"area_code", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)rec->area_code);   \
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"charset", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)rec->charset);       \
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"continent_code", (INTEGER)VARIABLE_STRING, rec->continent_code, (double)0);  \
        GeoIPRecord_delete(rec);                                                                                                              \
        rec = 0;                                                                                                                              \
    }

//------------------------------------------------------------------------
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
#ifdef _WIN32
    if (!HANDLER)
        WSACleanup();
#endif
    return 0;
}
//---------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_open_type, 2)
    T_NUMBER(0)
    T_NUMBER(1)

    GeoIP * gi = GeoIP_open_type(PARAM_INT(0), PARAM_INT(1));
    RETURN_NUMBER((SYS_INT)gi)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_open, 2)
    T_STRING(0)
    T_NUMBER(1)

    GeoIP * gi = GeoIP_open(PARAM(0), PARAM_INT(1));
    RETURN_NUMBER((SYS_INT)gi)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_new, 1)
    T_NUMBER(0)

    GeoIP * gi = GeoIP_new(PARAM_INT(0));
    RETURN_NUMBER((SYS_INT)gi)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_country_code_by_name, 2)
    T_HANDLE(0)
    T_STRING(1)

    const char *s = GeoIP_country_code_by_name((GeoIP *)PARAM_INT(0), PARAM(1));
    if (s) {
        RETURN_STRING(s)
    } else {
        RETURN_STRING("")
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_setup_custom_directory, 1)
    T_STRING(0)     // char*

    GeoIP_setup_custom_directory((char *)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_db_avail, 1)
    T_NUMBER(0)     // int

    RETURN_NUMBER(GeoIP_db_avail((int)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_delete, 1)
    T_NUMBER(0)     // GeoIP*

    if (PARAM_INT(0))
        GeoIP_delete((GeoIP *)(long)PARAM_INT(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_country_code_by_addr, 2)
    T_HANDLE(0)     // GeoIP*
    T_STRING(1)     // char*

    RETURN_STRING((char *)GeoIP_country_code_by_addr((GeoIP *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_country_code3_by_addr, 2)
    T_HANDLE(0)     // GeoIP*
    T_STRING(1)     // char*

    RETURN_STRING((char *)GeoIP_country_code3_by_addr((GeoIP *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_country_code3_by_name, 2)
    T_HANDLE(0)     // GeoIP*
    T_STRING(1)     // char*

    RETURN_STRING((char *)GeoIP_country_code3_by_name((GeoIP *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_country_name_by_addr, 2)
    T_HANDLE(0)     // GeoIP*
    T_STRING(1)     // char*

    RETURN_STRING((char *)GeoIP_country_name_by_addr((GeoIP *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_country_name_by_name, 2)
    T_HANDLE(0)     // GeoIP*
    T_STRING(1)     // char*

    RETURN_STRING((char *)GeoIP_country_name_by_name((GeoIP *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_country_name_by_ipnum, 2)
    T_HANDLE(0)     // GeoIP*
    T_NUMBER(1)     // unsigned

    RETURN_STRING((char *)GeoIP_country_name_by_ipnum((GeoIP *)(long)PARAM(0), PARAM_INT(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_country_code_by_ipnum, 2)
    T_HANDLE(0)     // GeoIP*
    T_NUMBER(1)     // unsigned

    RETURN_STRING((char *)GeoIP_country_code_by_ipnum((GeoIP *)(long)PARAM(0), PARAM_INT(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_country_code3_by_ipnum, 2)
    T_HANDLE(0)     // GeoIP*
    T_NUMBER(1)     // unsigned

    RETURN_STRING((char *)GeoIP_country_code3_by_ipnum((GeoIP *)(long)PARAM(0), PARAM_INT(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_country_name_by_ipnum_v6, 2)
    T_HANDLE(0)     // GeoIP*
    T_STRING(1)     // geoipv6_t

    CONVERT_ADDR

        RETURN_STRING((char *)GeoIP_country_name_by_ipnum_v6((GeoIP *)(long)PARAM(0), st))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_country_code_by_ipnum_v6, 2)
    T_HANDLE(0)     // GeoIP*
    T_STRING(1)     // geoipv6_t

    CONVERT_ADDR
        RETURN_STRING((char *)GeoIP_country_code_by_ipnum_v6((GeoIP *)(long)PARAM(0), st))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_country_code3_by_ipnum_v6, 2)
    T_HANDLE(0)     // GeoIP*
    T_STRING(1)     // geoipv6_t

    CONVERT_ADDR
        RETURN_STRING((char *)GeoIP_country_code3_by_ipnum_v6((GeoIP *)(long)PARAM(0), st))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_id_by_addr, 2)
    T_HANDLE(0)     // GeoIP*
    T_STRING(1)     // char*

    RETURN_NUMBER(GeoIP_id_by_addr((GeoIP *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_id_by_name, 2)
    T_HANDLE(0)     // GeoIP*
    T_STRING(1)     // char*

    RETURN_NUMBER(GeoIP_id_by_name((GeoIP *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_id_by_ipnum, 2)
    T_HANDLE(0)     // GeoIP*
    T_NUMBER(1)     // unsigned

    RETURN_NUMBER(GeoIP_id_by_ipnum((GeoIP *)(long)PARAM(0), PARAM_INT(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_id_by_addr_v6, 2)
    T_HANDLE(0)     // GeoIP*
    T_STRING(1)     // char*

    RETURN_NUMBER(GeoIP_id_by_addr_v6((GeoIP *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_id_by_name_v6, 2)
    T_HANDLE(0)     // GeoIP*
    T_STRING(1)     // char*

    RETURN_NUMBER(GeoIP_id_by_name_v6((GeoIP *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_id_by_ipnum_v6, 2)
    T_HANDLE(0)     // GeoIP*
    T_STRING(1)     // geoipv6_t

    CONVERT_ADDR

        RETURN_NUMBER(GeoIP_id_by_ipnum_v6((GeoIP *)(long)PARAM(0), st))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_region_by_addr, 2)
    T_HANDLE(0)     // GeoIP*
    T_STRING(1)     // char*

    GeoIPRegion * reg = GeoIP_region_by_addr((GeoIP *)(long)PARAM(0), (char *)PARAM(1));

    Invoke(INVOKE_CREATE_ARRAY, RESULT);
    if (reg) {
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"country_code", (INTEGER)VARIABLE_STRING, reg->country_code, (double)0);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"region", (INTEGER)VARIABLE_STRING, reg->region, (double)0);
        GeoIPRegion_delete(reg);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_region_by_name, 2)
    T_HANDLE(0)     // GeoIP*
    T_STRING(1)     // char*


    GeoIPRegion * reg = GeoIP_region_by_name((GeoIP *)(long)PARAM(0), (char *)PARAM(1));

    Invoke(INVOKE_CREATE_ARRAY, RESULT);
    if (reg) {
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"country_code", (INTEGER)VARIABLE_STRING, reg->country_code, (double)0);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"region", (INTEGER)VARIABLE_STRING, reg->region, (double)0);
        GeoIPRegion_delete(reg);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_region_by_ipnum, 2)
    T_HANDLE(0)     // GeoIP
    T_NUMBER(1)     // unsigned

    GeoIPRegion * reg = GeoIP_region_by_ipnum((GeoIP *)PARAM_INT(0), PARAM_INT(1));
    Invoke(INVOKE_CREATE_ARRAY, RESULT);
    if (reg) {
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"country_code", (INTEGER)VARIABLE_STRING, reg->country_code, (double)0);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"region", (INTEGER)VARIABLE_STRING, reg->region, (double)0);
        GeoIPRegion_delete(reg);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_region_by_addr_v6, 2)
    T_HANDLE(0)     // GeoIP*
    T_STRING(1)     // char*

    GeoIPRegion * reg = GeoIP_region_by_addr_v6((GeoIP *)(long)PARAM(0), (char *)PARAM(1));
    Invoke(INVOKE_CREATE_ARRAY, RESULT);
    if (reg) {
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"country_code", (INTEGER)VARIABLE_STRING, reg->country_code, (double)0);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"region", (INTEGER)VARIABLE_STRING, reg->region, (double)0);
        GeoIPRegion_delete(reg);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_region_by_name_v6, 2)
    T_HANDLE(0)     // GeoIP*
    T_STRING(1)     // char*

    GeoIPRegion * reg = GeoIP_region_by_name_v6((GeoIP *)(long)PARAM(0), (char *)PARAM(1));
    Invoke(INVOKE_CREATE_ARRAY, RESULT);
    if (reg) {
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"country_code", (INTEGER)VARIABLE_STRING, reg->country_code, (double)0);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"region", (INTEGER)VARIABLE_STRING, reg->region, (double)0);
        GeoIPRegion_delete(reg);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_region_by_ipnum_v6, 2)
    T_HANDLE(0)     // GeoIP*
    T_STRING(1)     // geoipv6_t

    CONVERT_ADDR

    GeoIPRegion * reg = GeoIP_region_by_ipnum_v6((GeoIP *)PARAM_INT(0), st);
    Invoke(INVOKE_CREATE_ARRAY, RESULT);
    if (reg) {
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"country_code", (INTEGER)VARIABLE_STRING, reg->country_code, (double)0);
        Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"region", (INTEGER)VARIABLE_STRING, reg->region, (double)0);
        GeoIPRegion_delete(reg);
    }
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_name_by_ipnum, 2)
    T_HANDLE(0)     // GeoIP*
    T_NUMBER(1)     // unsigned

    RETURN_STRING((char *)GeoIP_name_by_ipnum((GeoIP *)(long)PARAM(0), PARAM_INT(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_name_by_addr, 2)
    T_HANDLE(0)     // GeoIP*
    T_STRING(1)     // char*

    RETURN_STRING((char *)GeoIP_name_by_addr((GeoIP *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_name_by_name, 2)
    T_HANDLE(0)     // GeoIP*
    T_STRING(1)     // char*

    RETURN_STRING((char *)GeoIP_name_by_name((GeoIP *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_name_by_ipnum_v6, 2)
    T_HANDLE(0)     // GeoIP*
    T_STRING(1)     // geoipv6_t

    CONVERT_ADDR

        RETURN_STRING((char *)GeoIP_name_by_ipnum_v6((GeoIP *)(long)PARAM(0), st))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_name_by_addr_v6, 2)
    T_HANDLE(0)     // GeoIP*
    T_STRING(1)     // char*

    RETURN_STRING((char *)GeoIP_name_by_addr_v6((GeoIP *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_name_by_name_v6, 2)
    T_HANDLE(0)     // GeoIP*
    T_STRING(1)     // char*

    RETURN_STRING((char *)GeoIP_name_by_name_v6((GeoIP *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_code_by_id, 1)
    T_NUMBER(0)     // int

    RETURN_STRING((char *)GeoIP_code_by_id((int)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_code3_by_id, 1)
    T_NUMBER(0)     // int

    RETURN_STRING((char *)GeoIP_code3_by_id((int)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_name_by_id, 1)
    T_NUMBER(0)     // int

    RETURN_STRING((char *)GeoIP_name_by_id((int)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_continent_by_id, 1)
    T_NUMBER(0)     // int

    RETURN_STRING((char *)GeoIP_continent_by_id((int)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_id_by_code, 1)
    T_STRING(0)     // char*

    RETURN_NUMBER(GeoIP_id_by_code((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_num_countries, 0)

    RETURN_NUMBER(GeoIP_num_countries())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_database_info, 1)
    T_HANDLE(0)     // GeoIP*

    RETURN_STRING((char *)GeoIP_database_info((GeoIP *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_database_edition, 1)
    T_HANDLE(0)     // GeoIP*

    RETURN_NUMBER(GeoIP_database_edition((GeoIP *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_charset, 1)
    T_HANDLE(0)     // GeoIP*

    RETURN_NUMBER(GeoIP_charset((GeoIP *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_set_charset, 2)
    T_HANDLE(0)     // GeoIP*
    T_NUMBER(1)     // int

    RETURN_NUMBER(GeoIP_set_charset((GeoIP *)(long)PARAM(0), (int)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_last_netmask, 1)
    T_HANDLE(0)     // GeoIP*

    RETURN_NUMBER(GeoIP_last_netmask((GeoIP *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_region_name_by_code, 2)
    T_STRING(0)     // char*
    T_STRING(1)     // char*

    RETURN_STRING((char *)GeoIP_region_name_by_code((char *)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_time_zone_by_country_and_region, 2)
    T_STRING(0)     // char*
    T_STRING(1)     // char*

    RETURN_STRING((char *)GeoIP_time_zone_by_country_and_region((char *)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_record_by_ipnum, 2)
    T_HANDLE(0)     // GeoIP*
    T_NUMBER(1)     // unsigned

    GeoIPRecord * rec = GeoIP_record_by_ipnum((GeoIP *)(long)PARAM(0), PARAM_INT(1));
    RETURN_RECORD(rec);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_record_by_addr, 2)
    T_HANDLE(0)     // GeoIP*
    T_STRING(1)     // char*

    GeoIPRecord * rec = GeoIP_record_by_addr((GeoIP *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_RECORD(rec);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_record_by_name, 2)
    T_HANDLE(0)     // GeoIP*
    T_STRING(1)     // char*

    GeoIPRecord * rec = GeoIP_record_by_name((GeoIP *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_RECORD(rec);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_record_by_ipnum_v6, 2)
    T_HANDLE(0)     // GeoIP*
    T_STRING(1)     // geoipv6_t

    CONVERT_ADDR

    GeoIPRecord * rec = GeoIP_record_by_ipnum_v6((GeoIP *)(long)PARAM(0), st);
    RETURN_RECORD(rec);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_record_by_addr_v6, 2)
    T_HANDLE(0)     // GeoIP*
    T_STRING(1)     // char*

    GeoIPRecord * rec = GeoIP_record_by_addr_v6((GeoIP *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_RECORD(rec);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_record_by_name_v6, 2)
    T_HANDLE(0)     // GeoIP*
    T_STRING(1)     // char*

    GeoIPRecord * rec = GeoIP_record_by_name_v6((GeoIP *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_RECORD(rec);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_record_id_by_addr, 2)
    T_HANDLE(0)     // GeoIP*
    T_STRING(1)     // char*

    RETURN_NUMBER(GeoIP_record_id_by_addr((GeoIP *)(long)PARAM(0), (char *)PARAM(1)));
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GeoIP_record_id_by_addr_v6, 2)
    T_HANDLE(0)     // GeoIP*
    T_STRING(1)     // char*

    RETURN_NUMBER(GeoIP_record_id_by_addr_v6((GeoIP *)(long)PARAM(0), (char *)PARAM(1)));
END_IMPL
//------------------------------------------------------------------------
