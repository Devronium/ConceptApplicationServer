/* wvWare
 * Copyright (C) Caolan McNamara, Dom Lachowicz, and others
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
 #include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "wv.h"
#include <glib.h>

static t_printf_HANDLER  printf_HANDLER  = printf;
static t_fprintf_HANDLER fprintf_HANDLER = fprintf;

int (*wvConvertUnicodeToEntity) (U16 char16) = NULL;

U16
wvnLocaleToLIDConverter(U8 nLocale) {
    switch (nLocale) {
#if 0
        /* case 0: */		/* ANSI_CHARSET */
        /* case 1: */		/* DEFAULT_CHARSET */
        /* case 2: */		/* SYMBOL_CHARSET */
#endif
        case 77:                /* MAC_CHARSET */
            return 0xFFF;       /* This number is a hack */

        case 128:               /* SHIFTJIS_CHARSET */
            return 0x411;       /* Japanese */

        case 129:               /* HANGEUL_CHARSET */
            return 0x412;       /* Korean */

        case 130:               /* JOHAB_CHARSET */
            return 0x812;       /* Korean (Johab) */

        case 134:               /* GB2312_CHARSET - Chinese Simplified */
            return 0x804;       /* China PRC - And others!! */

        case 136:               /* CHINESEBIG5_CHARSET - Chinese Traditional */
            return 0x404;       /* Taiwan - And others!! */

        case 161:               /* GREEK_CHARSET */
            return 0x408;       /* Greek */

        case 162:               /* TURKISH_CHARSET */
            return 0x41f;       /* Turkish */

        case 163:               /* VIETNAMESE_CHARSET */
            return 0x42a;       /* Vietnamese */

        case 177:               /* HEBREW_CHARSET */
            return 0x40d;       /* Hebrew */

        case 178:               /* ARABIC_CHARSET */
            return 0x01;        /* Arabic */

        case 186:               /* BALTIC_CHARSET */
            return 0x425;       /* Estonian - And others!! */

        case 204:               /* RUSSIAN_CHARSET */
            return 0x419;       /* Russian - And others!! */

        case 222:               /* THAI_CHARSET */
            return 0x41e;       /* Thai */

        case 238:               /* EASTEUROPE_CHARSET */
            return 0x405;       /* Czech - And many others!! */

#if 0
            /* case 255: */		/* OEM_CHARSET */
#endif

        default:
            return 0x0;
    }
    return 0x0;
}

int
wvOutputTextChar(U16 eachchar, U8 chartype, wvParseStruct *ps, CHP *achp) {
    U16 lid = 0;

    wvVersion v = wvQuerySupported(&ps->fib, NULL);

    /* testing adding a language */

    /* For version <= WORD7, The charset used could
     * depend on the font's charset.
     */
    if ((v <= WORD7) && (!ps->fib.fFarEast)) {
        FFN currentfont;

        if (ps->fonts.ffn == NULL) {
            lid = 0;
        } else {
            currentfont = ps->fonts.ffn[achp->ftc];
            /* Return 0 if no match */
            lid = wvnLocaleToLIDConverter(currentfont.chs);
        }
    }
    if (!lid)
        lid = achp->lidDefault;


    /* No lidDefault for ver < WORD6 */
    if ((lid == 0x400) || (lid == 0))
        lid = ps->fib.lid;

    /* end testing adding a language */

    if (achp->fSpec) {
        /*
           if the character is still one of the special ones then call this other
           handler
           instead
         */
        if (ps->scharhandler)
            return (*(ps->scharhandler))(ps, eachchar, achp);
    } else {
        /* Most Chars go through this baby */
        if (ps->charhandler) {
            if (!(((v == WORD7) || (v == WORD6)) && ps->fib.fFarEast))
                if (v <= WORD7) {
                    /* versions <= 7 do not use unicode. versions >= 8 always do */
                    /* versions 7 and 6 use unicode iff the far-east flag is set */
                    chartype = 1;
                }

            return (*(ps->charhandler))(ps, eachchar, chartype, lid);
        }
    }
    wvError(("No CharHandler registered, programmer error\n"));
    return 0;
}

void
wvOutputHtmlChar(U16 eachchar, U8 chartype, char *outputtype, U16 lid) {
    if (chartype)
        eachchar = wvHandleCodePage(eachchar, lid);
    wvOutputFromUnicode(eachchar, outputtype);
}

#define CPNAME_OR_FALLBACK(name, fallbackname)    \
    {                                             \
        static char *cpname = NULL;               \
        if (!cpname)                              \
        {                                         \
            GIConv cd = g_iconv_open(name, name); \
            if (cd == (GIConv) - 1)               \
            {                                     \
                cpname = fallbackname;            \
            }                                     \
            else                                  \
            {                                     \
                cpname = name;                    \
                g_iconv_close(cd);                \
            }                                     \
        };                                        \
        return cpname;                            \
    }

typedef struct {
    const char *language_tag;
    U16        lid;
} wvLanguageId;

static const wvLanguageId mLanguageIds[] =
{
    { "-none-", 0x0000 },         /* none (language neutral) */
    { "-none-", 0x0400 },         /* none */
    { "af-ZA",  0x0436 },         /* Afrikaans */
    { "am",     0x045e },         /* Amharic */
    { "sq-AL",  0x041c },         /* Albanian */
    { "ar-SA",  0x0401 },         /* Arabic (Saudi) */
    { "ar-IQ",  0x0801 },         /* Arabic (Iraq) */
    { "ar-EG",  0x0c01 },         /* Arabic (Egypt) */
    { "ar-LY",  0x1001 },         /* Arabic (Libya) */
    { "ar-DZ",  0x1401 },         /* Arabic (Algeria) */
    { "ar-MA",  0x1801 },         /* Arabic (Morocco) */
    { "ar-TN",  0x1c01 },         /* Arabic (Tunisia) */
    { "ar-OM",  0x2001 },         /* Arabic (Oman) */
    { "ar-YE",  0x2401 },         /* Arabic (Yemen) */
    { "ar-SY",  0x2801 },         /* Arabic (Syria) */
    { "ar-JO",  0x2c01 },         /* Arabic (Jordan) */
    { "ar-LB",  0x3001 },         /* Arabic (Lebanon) */
    { "ar-KW",  0x3401 },         /* Arabic (Kuwait) */
    { "ar-AE",  0x3801 },         /* Arabic (United Arab Emirates) */
    { "ar-BH",  0x3c01 },         /* Arabic (Bahrain) */
    { "ar-QA",  0x4001 },         /* Arabic (Qatar) */
    { "as",     0x044d },         /* Assamese */
    { "az",     0x042c },         /* Azerbaijani */
    { "hy-AM",  0x042b },         /* Armenian */
    { "az",     0x044c },         /* Azeri (Latin) az- */
    { "az",     0x082c },         /* Azeri (Cyrillic) az- */
    { "eu-ES",  0x042d },         /* Basque */
    { "be-BY",  0x0423 },         /* Belarussian */
    { "bn",     0x0445 },         /* Bengali bn- */
    { "bg-BG",  0x0402 },         /* Bulgarian */
    { "ca-ES",  0x0403 },         /* Catalan */
    { "zh-TW",  0x0404 },         /* Chinese (Taiwan) */
    { "zh-CN",  0x0804 },         /* Chinese (PRC) */
    { "zh-HK",  0x0c04 },         /* Chinese (Hong Kong) */
    { "zh-SG",  0x1004 },         /* Chinese (Singapore) */
    { "ch-MO",  0x1404 },         /* Chinese (Macau SAR) */
    { "hr-HR",  0x041a },         /* Croatian */
    { "cs-CZ",  0x0405 },         /* Czech */
    { "da-DK",  0x0406 },         /* Danish */
    { "div",     0x465 },         /* Divehi div-*/
    { "nl-NL",  0x0413 },         /* Dutch (Netherlands) */
    { "nl-BE",  0x0813 },         /* Dutch (Belgium) */
    { "en-US",  0x0409 },         /* English (USA) */
    { "en-GB",  0x0809 },         /* English (UK) */
    { "en-AU",  0x0c09 },         /* English (Australia) */
    { "en-CA",  0x1009 },         /* English (Canada) */
    { "en-NZ",  0x1409 },         /* English (New Zealand) */
    { "en-IE",  0x1809 },         /* English (Ireland) */
    { "en-ZA",  0x1c09 },         /* English (South Africa) */
    { "en-JM",  0x2009 },         /* English (Jamaica) */
    { "en",     0x2409 },         /* English (Caribbean) */
    { "en-BZ",  0x2809 },         /* English (Belize) */
    { "en-TT",  0x2c09 },         /* English (Trinidad) */
    { "en-ZW",  0x3009 },         /* English (Zimbabwe) */
    { "en-PH",  0x3409 },         /* English (Phillipines) */
    { "et-EE",  0x0425 },         /* Estonian */
    { "fo",     0x0438 },         /* Faeroese fo- */
    { "fa-IR",  0x0429 },         /* Farsi */
    { "fi-FI",  0x040b },         /* Finnish */
    { "fr-FR",  0x040c },         /* French (France) */
    { "fr-BE",  0x080c },         /* French (Belgium) */
    { "fr-CA",  0x0c0c },         /* French (Canada) */
    { "fr-CH",  0x100c },         /* French (Switzerland) */
    { "fr-LU",  0x140c },         /* French (Luxembourg) */
    { "fr-MC",  0x180c },         /* French (Monaco) */
    { "gl",     0x0456 },         /* Galician gl- */
    { "ga-IE",  0x083c },         /* Irish Gaelic */
    { "gd-GB",  0x100c },         /* Scottish Gaelic */
    { "ka-GE",  0x0437 },         /* Georgian */
    { "de-DE",  0x0407 },         /* German (Germany) */
    { "de-CH",  0x0807 },         /* German (Switzerland) */
    { "de-AT",  0x0c07 },         /* German (Austria) */
    { "de-LU",  0x1007 },         /* German (Luxembourg) */
    { "de-LI",  0x1407 },         /* German (Liechtenstein) */
    { "el-GR",  0x0408 },         /* Greek */
    { "gu",     0x0447 },         /* Gujarati gu- */
    { "ha",     0x0468 },         /* Hausa */
    { "he-IL",  0x040d },         /* Hebrew */
    { "hi-IN",  0x0439 },         /* Hindi */
    { "hu-HU",  0x040e },         /* Hungarian */
    { "is-IS",  0x040f },         /* Icelandic */
    { "id-ID",  0x0421 },         /* Indonesian */
    { "iu",     0x045d },         /* Inkutitut */
    { "it-IT",  0x0410 },         /* Italian (Italy) */
    { "it-CH",  0x0810 },         /* Italian (Switzerland) */
    { "ja-JP",  0x0411 },         /* Japanese */
    { "kn",     0x044b },         /* Kannada kn- */
    { "ks",     0x0860 },         /* Kashmiri (India) ks- */
    { "kk",     0x043f },         /* Kazakh kk- */
    { "kok",    0x0457 },         /* Konkani kok- */
    { "ko-KR",  0x0412 },         /* Korean */
    { "ko",     0x0812 },         /* Korean (Johab) ko- */
    { "kir",    0x0440 },         /* Kyrgyz */
    { "la",     0x0476 },         /* Latin */
    { "lo",     0x0454 },         /* Laothian */
    { "lv-LV",  0x0426 },         /* Latvian */
    { "lt-LT",  0x0427 },         /* Lithuanian */
    { "lt-LT",  0x0827 },         /* Lithuanian (Classic) */
    { "mk",     0x042f },         /* FYRO Macedonian */
    { "my-MY",  0x043e },         /* Malaysian */
    { "my-BN",  0x083e },         /* Malay Brunei Darussalam */
    { "ml",     0x044c },         /* Malayalam ml- */
    { "mr",     0x044e },         /* Marathi mr- */
    { "mt",     0x043a },         /* Maltese */
    { "mo",     0x0450 },         /* Mongolian */
    { "ne-NP",  0x0461 },         /* Napali (Nepal) */
    { "ne-IN",  0x0861 },         /* Nepali (India) */
    { "nb-NO",  0x0414 },         /* Norwegian (Bokmai) */
    { "nn-NO",  0x0814 },         /* Norwegian (Nynorsk) */
    { "or",     0x0448 },         /* Oriya or- */
    { "om",     0x0472 },         /* Oromo (Afan, Galla) */
    { "pl-PL",  0x0415 },         /* Polish */
    { "pt-BR",  0x0416 },         /* Portuguese (Brazil) */
    { "pt-PT",  0x0816 },         /* Portuguese (Portugal) */
    { "pa",     0x0446 },         /* Punjabi pa- */
    { "ps",     0x0463 },         /* Pashto (Pushto) */
    { "rm",     0x0417 },         /* Rhaeto-Romanic rm- */
    { "ro-RO",  0x0418 },         /* Romanian */
    { "ro-MD",  0x0818 },         /* Romanian (Moldova) */
    { "ru-RU",  0x0419 },         /* Russian */
    { "ru-MD",  0x0819 },         /* Russian (Moldova) */
    { "se",     0x043b },         /* Sami (Lappish) se- */
    { "sa",     0x044f },         /* Sanskrit sa- */
    { "sr",     0x0c1a },         /* Serbian (Cyrillic) sr- */
    { "sr",     0x081a },         /* Serbian (Latin) sr- */
    { "sd",     0x0459 },         /* Sindhi sd- */
    { "sk-SK",  0x041b },         /* Slovak */
    { "sl-SI",  0x0424 },         /* Slovenian */
    { "wen",    0x042e },         /* Sorbian wen- */
    { "so",     0x0477 },         /* Somali */
    { "es-ES",  0x040a },         /* Spanish (Spain, Traditional) */
    { "es-MX",  0x080a },         /* Spanish (Mexico) */
    { "es-ES",  0x0c0a },         /* Spanish (Modern) */
    { "es-GT",  0x100a },         /* Spanish (Guatemala) */
    { "es-CR",  0x140a },         /* Spanish (Costa Rica) */
    { "es-PA",  0x180a },         /* Spanish (Panama) */
    { "es-DO",  0x1c0a },         /* Spanish (Dominican Republic) */
    { "es-VE",  0x200a },         /* Spanish (Venezuela) */
    { "es-CO",  0x240a },         /* Spanish (Colombia) */
    { "es-PE",  0x280a },         /* Spanish (Peru) */
    { "es-AR",  0x2c0a },         /* Spanish (Argentina) */
    { "es-EC",  0x300a },         /* Spanish (Ecuador) */
    { "es-CL",  0x340a },         /* Spanish (Chile) */
    { "es-UY",  0x380a },         /* Spanish (Uruguay) */
    { "es-PY",  0x3c0a },         /* Spanish (Paraguay) */
    { "es-BO",  0x400a },         /* Spanish (Bolivia) */
    { "es-SV",  0x440a },         /* Spanish (El Salvador) */
    { "es-HN",  0x480a },         /* Spanish (Honduras) */
    { "es-NI",  0x4c0a },         /* Spanish (Nicaragua) */
    { "es-PR",  0x500a },         /* Spanish (Puerto Rico) */
    { "sx",     0x0430 },         /* Sutu */
    { "sw",     0x0441 },         /* Swahili (Kiswahili/Kenya) */
    { "sv-SE",  0x041d },         /* Swedish */
    { "sv-FI",  0x081d },         /* Swedish (Finland) */
    { "ta",     0x0449 },         /* Tamil ta- */
    { "tt",     0x0444 },         /* Tatar (Tatarstan) tt- */
    { "te",     0x044a },         /* Telugu te- */
    { "th-TH",  0x041e },         /* Thai */
    { "ts",     0x0431 },         /* Tsonga ts- */
    { "tn",     0x0432 },         /* Tswana tn- */
    { "tr-TR",  0x041f },         /* Turkish */
    { "tl",     0x0464 },         /* Tagalog */
    { "tg",     0x0428 },         /* Tajik */
    { "bo",     0x0451 },         /* Tibetan */
    { "ti",     0x0473 },         /* Tigrinya */
    { "uk-UA",  0x0422 },         /* Ukrainian */
    { "ur-PK",  0x0420 },         /* Urdu (Pakistan) */
    { "ur-IN",  0x0820 },         /* Urdu (India) */
    { "uz",     0x0443 },         /* Uzbek (Latin) uz- */
    { "uz",     0x0843 },         /* Uzbek (Cyrillic) uz- */
    { "ven",    0x0433 },         /* Venda ven- */
    { "vi-VN",  0x042a },         /* Vietnamese */
    { "cy-GB",  0x0452 },         /* Welsh */
    { "xh",     0x0434 },         /* Xhosa xh */
    { "yi",     0x043d },         /* Yiddish yi- */
    { "yo",     0x046a },         /* Yoruba */
    { "zu",     0x0435 },         /* Zulu zu- */
    { "en-US",  0x0800 }          /* Default */
};

#define NrMappings    (sizeof(mLanguageIds) / sizeof(mLanguageIds[0]))

U16 wvLangToLIDConverter(const char *lang) {
    unsigned int i = 0;

    if (!lang)
        return 0x0400;           /* return -none- */

    for (i = 0; i < NrMappings; i++)
        if (!strcmp(lang, mLanguageIds[i].language_tag))
            return mLanguageIds[i].lid;

    return 0x0400;        /* return -none- */
}

const char *
wvLIDToLangConverter(U16 lid) {
    unsigned int i = 0;

    if (lid == 0)       /* language netural */
        return "-none-";

    for (i = 0; i < NrMappings; i++)
        if (mLanguageIds[i].lid == lid)
            return mLanguageIds[i].language_tag;

    return "-none-";     /* default */
}

const char *
wvLIDToCodePageConverter(U16 lid) {
    if (lid == 0x0FFF)  /*Macintosh Hack */
        return "MACINTOSH";

    switch (lid & 0xff) {
        case 0x01:              /*Arabic */
            return "CP1256";

        case 0x02:              /*Bulgarian */
            return "CP1251";

        case 0x03:              /*Catalan */
            return "CP1252";

        case 0x04:              /*Chinese */
            switch (lid) {
#if 0
                case 0x1404:    /*Chinese (Macau SAR) */
#endif
                case 0x0c04:    /*Chinese (Hong Kong SAR, PRC) */
                    CPNAME_OR_FALLBACK("CP950", "BIG5-HKSCS");

                case 0x0804:    /*Chinese (PRC) */
                    CPNAME_OR_FALLBACK("CP936", "GBK");

#if 0
                case 0x1004:    /*Chinese (Singapore) */
#endif
                case 0x0404:    /*Chinese (Taiwan) */
                    CPNAME_OR_FALLBACK("CP950", "BIG5");
            }

        case 0x05:              /*Czech */
            return "CP1250";

        case 0x06:              /*Danish */
            return "CP1252";

        case 0x07:              /*German */
            return "CP1252";

        case 0x08:              /*Greek */
            return "CP1253";

        case 0x09:              /*English */
            return "CP1252";

        case 0x0a:              /*Spanish */
            return "CP1252";

        case 0x0b:              /*Finnish */
            return "CP1252";

        case 0x0c:              /*French */
            return "CP1252";

        case 0x0d:              /*Hebrew */
            return "CP1255";

        case 0x0e:              /*Hungarian */
            return "CP1250";

        case 0x0f:              /*Icelandic */
            return "CP1252";

        case 0x10:              /*Italian */
            return "CP1252";

        case 0x11:              /*Japanese */
            return "CP932";

        case 0x12:              /*Korean */
            switch (lid) {
                case 0x0812:    /*Korean (Johab) */
                    return "CP1361";

                case 0x0412:    /*Korean */
                    return "CP949";
            }

        case 0x13:              /*Dutch */
            return "CP1252";

        case 0x14:              /*Norwegian */
            return "CP1252";

        case 0x15:              /*Polish */
            return "CP1250";

        case 0x16:              /*Portuguese */
            return "CP1252";

        case 0x17:              /*Rhaeto-Romanic */
            return "CP1252";

        case 0x18:              /*Romanian */
            return "CP1250";

        case 0x19:              /*Russian */
            return "CP1251";

        case 0x1a:              /*Serbian, Croatian, (Bosnian?) */
            switch (lid) {
                case 0x041a:    /*Croatian */
                    return "CP1252";

                case 0x0c1a:    /*Serbian (Cyrillic) */
                    return "CP1251";

                case 0x081a:    /*Serbian (Latin) */
                    return "CP1252";
            }

        case 0x1b:              /*Slovak */
            return "CP1250";

        case 0x1c:              /*Albanian */
            return "CP1251";

        case 0x1d:              /*Swedish */
            return "CP1252";

        case 0x1e:              /*Thai */
            return "CP874";

        case 0x1f:              /*Turkish */
            return "CP1254";

        case 0x20:              /*Urdu. This is Unicode only. */
            return "0";

        case 0x21:              /*Bahasa Indonesian */
            return "CP1252";

        case 0x22:              /*Ukrainian */
            return "CP1251";

        case 0x23:              /*Byelorussian / Belarusian */
            return "CP1251";

        case 0x24:              /*Slovenian */
            return "CP1250";

        case 0x25:              /*Estonian */
            return "CP1257";

        case 0x26:              /*Latvian */
            return "CP1257";

        case 0x27:              /*Lithuanian */
            return "CP1257";

        case 0x29:              /*Farsi / Persian. This is Unicode only. */
            return "0";

        case 0x2a:              /*Vietnamese */
            return "CP1258";

        case 0x2b:              /*Windows 2000: Armenian. This is Unicode only. */
            return "CP0";

        case 0x2c:              /*Azeri */
            switch (lid) {
                case 0x082c:    /*Azeri (Cyrillic) */
                    return "CP1251";

#if 0
                case 0x042c:    /*Azeri (Latin) */
#endif
            }

        case 0x2d:              /*Basque */
            return "CP1252";

        case 0x2f:              /*Macedonian */
            return "CP1251";

#if 0
        case 0x30:              /*Sutu */
#endif
        case 0x36:              /*Afrikaans */
            return "CP1252";

        case 0x37:              /*Windows 2000: Georgian. This is Unicode only. */
            return "CP0";

        case 0x38:              /*Faeroese */
            return "CP1252";

        case 0x39:              /*Windows 2000: Hindi. This is Unicode only. */
            return "CP0";

        case 0x3E:              /*Malaysian / Malay */
            return "CP1252";

#if 0
        case 0x3f:              /*Kazakh */
#endif
        case 0x41:              /*Swahili */
            return "CP1252";

        case 0x43:              /*Uzbek */
            switch (lid) {
                case 0x0843:    /*Uzbek (Cyrillic) */
                    return "CP1251";

#if 0
                case 0x0443:    /*Uzbek (Latin) */
#endif
            }

#if 0
        case 0x44:              /*Tatar */
#endif
        case 0x45:              /*Windows 2000: Bengali. This is Unicode only. */
        case 0x46:              /*Windows 2000: Punjabi. This is Unicode only. */
        case 0x47:              /*Windows 2000: Gujarati. This is Unicode only. */
        case 0x48:              /*Windows 2000: Oriya. This is Unicode only. */
        case 0x49:              /*Windows 2000: Tamil. This is Unicode only. */
        case 0x4a:              /*Windows 2000: Telugu. This is Unicode only. */
        case 0x4b:              /*Windows 2000: Kannada. This is Unicode only. */
        case 0x4c:              /*Windows 2000: Malayalam. This is Unicode only. */
        case 0x4d:              /*Windows 2000: Assamese. This is Unicode only. */
        case 0x4e:              /*Windows 2000: Marathi. This is Unicode only. */
        case 0x4f:              /*Windows 2000: Sanskrit. This is Unicode only. */
            return "CP0";

        case 0x55:              /*Myanmar / Burmese. This is Unicode only. */
            return "CP0";

        case 0x57:              /*Windows 2000: Konkani. This is Unicode only. */
            return "CP0";

#if 0
        case 0x58:              /*Manipuri */
        case 0x59:              /*Sindhi */
        case 0x60:              /*Kashmiri (India) */
#endif
        case 0x61:              /*Windows 2000: Nepali (India). This is Unicode only. */
            return "CP0";
    }

    /* TODO output a warning since this is a guess */
    return "CP1252";
}

static U32
swap_iconv(U16 lid) {
    GIConv     handle = NULL;
    char       f_code[33];      /* From CCSID                           */
    char       t_code[33];      /* To CCSID                             */
    const char *codepage = NULL;
    size_t     ibuflen, obuflen;

    U8 buffer[2];
    U8 buffer2[2];

    gchar *ibuf, *obuf;

    /* do a bit of caching */
    static U16 lastlid = -1;
    static U32 ret     = -1;

    /* shortcut */
    if ((ret != -1) && (lastlid == lid))
        return ret;

    ibuf = buffer;
    obuf = buffer2;

    lastlid  = lid;
    codepage = wvLIDToCodePageConverter(lid);

    memset(f_code, '\0', 33);
    memset(t_code, '\0', 33);

    strcpy(f_code, codepage);
    strcpy(t_code, "UCS-2");

    handle = g_iconv_open(t_code, f_code);
    if (handle == (GIConv) - 1)
        return 0;

    buffer[0] = 0x20 & 0xff;
    buffer[1] = 0;

    ibuflen = obuflen = 2;

    g_iconv(handle, &ibuf, &ibuflen, &obuf, &obuflen);

    g_iconv_close(handle);

    ret = *(U16 *)buffer2 != 0x20;
    return ret;
}

U16
wvHandleCodePage(U16 eachchar, U16 lid) {
    char       f_code[33];      /* From CCSID                           */
    char       t_code[33];      /* To CCSID                             */
    const char *codepage;
    GIConv     g_iconv_handle;  /* Conversion Descriptor returned       */
    /* from g_iconv_open() function           */
    size_t ibuflen;             /* Length of input buffer               */
    size_t obuflen;             /* Length of output buffer              */

    gchar *ibuf;
    gchar *obuf;                        /* Buffer for converted characters      */
    U8    *p;
    U8    buffer[2];
    U8    buffer2[2];

    U16 rtn;

    if (eachchar > 0xff) {
        buffer[0] = (char)(eachchar >> 8);
        buffer[1] = (char)eachchar & 0xff;
    } else {
        buffer[0] = eachchar & 0xff;
        buffer[1] = 0;
    }

    ibuf = buffer;
    obuf = buffer2;

    codepage = wvLIDToCodePageConverter(lid);

    /* All reserved positions of from code (last 12 characters) and to code   */
    /* (last 19 characters) must be set to hexadecimal zeros.                 */

    memset(f_code, '\0', 33);
    memset(t_code, '\0', 33);

    strcpy(f_code, codepage);
    strcpy(t_code, "UCS-2");

    g_iconv_handle = g_iconv_open(t_code, f_code);
    if (g_iconv_handle == (GIConv) - 1) {
        wvError(
            ("g_iconv_open fail: %d, cannot convert %s to unicode\n",
             errno, codepage));
        return '?';
    }

    ibuflen = obuflen = 2;
    p       = obuf;

    g_iconv(g_iconv_handle, &ibuf, &ibuflen, &obuf, &obuflen);

    /* We might have double byte char here. */

    if (swap_iconv(lid)) {
        rtn  = (U16)buffer2[0] << 8;
        rtn |= (U16)buffer2[1];
    } else {
        rtn = *(U16 *)buffer2;
    }

    g_iconv_close(g_iconv_handle);

    return rtn;
}

void
wvOutputFromUnicode(U16 eachchar, char *outputtype) {
    static char   cached_outputtype[33];         /* Last outputtype                  */
    static GIConv g_iconv_handle = (GIConv) - 1; /* Cached iconv descriptor          */
    static int    need_swapping;
    gchar         *ibuf, *obuf;
    size_t        ibuflen, obuflen, len, count, i;
    U8            buffer[2], buffer2[5];

    if ((wvConvertUnicodeToEntity != NULL) &&
        wvConvertUnicodeToEntity(eachchar))
        return;

    if ((g_iconv_handle == (GIConv) - 1) || (strcmp(cached_outputtype, outputtype) != 0)) {
        if ((g_iconv_handle != (GIConv) - 1))
            g_iconv_close(g_iconv_handle);

        g_iconv_handle = g_iconv_open(outputtype, "UCS-2");
        if (g_iconv_handle == (GIConv) - 1) {
            wvError(
                ("g_iconv_open fail: %d, cannot convert %s to %s\n",
                 errno, "UCS-2", outputtype));
            GET_printf_HANDLER() ("?");
            return;
        }

        /* safe to cache the output type here */
        strcpy(cached_outputtype, outputtype);

        /* Determining if unicode biteorder is swapped (glibc < 2.2) */
        need_swapping = 1;

        buffer[0] = 0x20;
        buffer[1] = 0;
        ibuf      = buffer;
        obuf      = buffer2;
        ibuflen   = 2;
        obuflen   = 5;

        count = g_iconv(g_iconv_handle, &ibuf, &ibuflen, &obuf, &obuflen);
        if (count >= 0)
            need_swapping = buffer2[0] != 0x20;
    }

    if (need_swapping) {
        buffer[0] = (eachchar >> 8) & 0x00ff;
        buffer[1] = eachchar & 0x00ff;
    } else {
        buffer[0] = eachchar & 0x00ff;
        buffer[1] = (eachchar >> 8) & 0x00ff;
    }

    ibuf = buffer;
    obuf = buffer2;

    ibuflen = 2;
    len     = obuflen = 5;

    count = g_iconv(g_iconv_handle, &ibuf, &ibuflen, &obuf, &obuflen);
    if (count == (size_t)-1) {
        wvError(("iconv failed errno: %d, char: 0x%X, %s -> %s\n",
                 errno, eachchar, "UCS-2", outputtype));

        /* I'm torn here - do i just announce the failure, continue, or copy over to the other buffer? */

        /* errno is usually 84 (illegal byte sequence)
           should i reverse the bytes and try again? */
        GET_printf_HANDLER() ("%c", ibuf[1]);
    } else {
        len = len - obuflen;

        for (i = 0; i < len; i++)
            GET_printf_HANDLER() ("%c", buffer2[i]);
    }
}

int
wvHandleElement(wvParseStruct *ps, wvTag tag, void *props, int dirty) {
    if (ps->elehandler)
        return (*(ps->elehandler))(ps, tag, props, dirty);
    wvError(("No element handler registered!!\n"));
    return 0;
}

int
wvHandleDocument(wvParseStruct *ps, wvTag tag) {
    if (ps->dochandler)
        return (*(ps->dochandler))(ps, tag);
    wvError(("No dochandler!!\n"));
    return 0;
}

t_printf_HANDLER GET_printf_HANDLER() {
    if (!printf_HANDLER)
        printf_HANDLER = printf;
    return printf_HANDLER;
}

t_fprintf_HANDLER GET_fprintf_HANDLER() {
    if (!fprintf_HANDLER)
        fprintf_HANDLER = fprintf;
    return fprintf_HANDLER;
}

void wvSetPrintFHandler(t_printf_HANDLER proc) {
    printf_HANDLER = proc;
}

void wvSetFPrintFHandler(t_fprintf_HANDLER proc) {
    fprintf_HANDLER = proc;
}

void
wvSetCharHandler(wvParseStruct *ps,
                 int (*proc)(wvParseStruct *, U16, U8, U16)) {
    ps->charhandler = proc;
}

void
wvSetSpecialCharHandler(wvParseStruct *ps,
                        int (*proc)(wvParseStruct *, U16, CHP *)) {
    ps->scharhandler = proc;
}

void
wvSetElementHandler(wvParseStruct *ps,
                    int (*proc)(wvParseStruct *, wvTag, void *, int)) {
    ps->elehandler = proc;
}

void
wvSetDocumentHandler(wvParseStruct *ps,
                     int (*proc)(wvParseStruct *, wvTag)) {
    ps->dochandler = proc;
}

int
wvConvertUnicodeToLaTeX(U16 char16) {
    /*
       german and scandinavian characters, MV 1.7.2000
       See man iso_8859_1

       This requires the inputencoding latin1 package,
       see latin1.def. Chars in range 160...255 are just
       put through as these are legal iso-8859-1 symbols.
       (see above)

       Best way to do it until LaTeX is Unicode enabled
       (Omega project).
       -- MV 4.7.2000

       We use a separate if-statement here ... the 'case range'
       construct is gcc specific :-(  -- MV 13/07/2000
     */

    if ((char16 >= 0xa0) && (char16 <= 0xff)) {
        switch (char16) {
            case 0xa0:
                GET_printf_HANDLER() ("\\ ");   /* hard space */
                return 1;

            /* Fix up these as math characters: */
            case 0xb1:
                GET_printf_HANDLER() ("$\\pm$");
                return 1;

            case 0xb2:
                GET_printf_HANDLER() ("$\\mathtwosuperior$");
                return 1;

            case 0xb3:
                GET_printf_HANDLER() ("$\\maththreesuperior$");
                return 1;

            case 0xb5:
                GET_printf_HANDLER() ("$\\mu$");
                return 1;

            case 0xb9:
                GET_printf_HANDLER() ("$\\mathonesuperior$");
                return 1;

            case 0xd7:
                GET_printf_HANDLER() ("$\\times$");
                return 1;
        }
        GET_printf_HANDLER() ("%c", char16);
        return 1;
    }
    switch (char16) {
        case 37:
            GET_printf_HANDLER() ("\\%%");
            return 1;

        case 10:
        case 11:
            GET_printf_HANDLER() ("\\\\\n");
            return 1;

        case 31:                /* non-required hyphen */
            GET_printf_HANDLER() ("\\-");
            return 1;

        case 30:                /* non-breaking hyphen */
            GET_printf_HANDLER() ("-");
            return 1;

        /* case 45: minus/hyphen, pass through */

        case 12:
            GET_printf_HANDLER()("\\newpage\n");
            return 1;

        case 13:
        case 14:
        case 7:
            return 1;

        case 9:
            GET_printf_HANDLER() ("\\hfill{}"); /* tab -- horrible cludge */
            return 1;

        case 0xf020:
            GET_printf_HANDLER() (" ");         /* Mac specialty ? MV 10.10.2000 */
            return 1;

        case 0xf02c:
            GET_printf_HANDLER() (",");         /* Mac */
            return 1;

        case 0xf028:
            GET_printf_HANDLER() ("(");         /* Mac */
            return 1;

        case 34:
            GET_printf_HANDLER() ("\"");
            return 1;

        case 35:
            GET_printf_HANDLER() ("\\#"); /* MV 14.8.2000 */
            return 1;

        case 36:
            GET_printf_HANDLER() ("\\$"); /* MV 14.8.2000 */
            return 1;

        case 38:
            GET_printf_HANDLER() ("\\&"); /* MV 1.7.2000 */
            return 1;

        case 92:
            GET_printf_HANDLER() ("$\\backslash$");     /* MV 23.9.2000 */
            return 1;

        case 94:
            GET_printf_HANDLER() ("\\^"); /* MV 13.9.2000 */
            return 1;

        case 95:
            GET_printf_HANDLER() ("\\_"); /* MV 13.9.2000 */
            return 1;

        case 60:
            GET_printf_HANDLER() ("<");
            return 1;

        case 0xf03e:            /* Mac */
        case 62:
            GET_printf_HANDLER() (">");
            return 1;

        case 0xF8E7:
            /* without this, things should work in theory, but not for me */
            GET_printf_HANDLER() ("_");
            return 1;

        /* Added some new Unicode characters. It's probably difficult
           to write these characters in AbiWord, though ... :(
           -- 2000-08-11 huftis@bigfoot.com */

        case 0x0100:
            GET_printf_HANDLER() ("\\=A");      /* A with macron */
            return 1;

        case 0x0101:
            GET_printf_HANDLER() ("\\=a");      /* a with macron */
            return 1;

        case 0x0102:
            GET_printf_HANDLER() ("\\u{A}");    /* A with breve */
            return 1;

        case 0x0103:
            GET_printf_HANDLER() ("\\u{a}");    /* a with breve */
            return 1;

        case 0x0104:
            GET_printf_HANDLER() ("\\k{A}");    /* A with ogonek */
            return 1;

        case 0x0105:
            GET_printf_HANDLER() ("\\k{a}");    /* a with ogonek */
            return 1;

        case 0x0106:
            GET_printf_HANDLER() ("\\'C");      /* C with acute */
            return 1;

        case 0x0107:
            GET_printf_HANDLER() ("\\'c");      /* c with acute */
            return 1;

        case 0x0108:
            GET_printf_HANDLER() ("\\^C");      /* C with circumflex */
            return 1;

        case 0x0109:
            GET_printf_HANDLER() ("\\^c");      /* c with circumflex */
            return 1;

        case 0x010A:
            GET_printf_HANDLER() ("\\.C");      /* C with dot above */
            return 1;

        case 0x010B:
            GET_printf_HANDLER() ("\\.c");      /* c with dot above */
            return 1;

        case 0x010C:
            GET_printf_HANDLER() ("\\v{C}");    /* C with caron */
            return 1;

        case 0x010D:
            GET_printf_HANDLER() ("\\v{c}");    /* c with caron */
            return 1;

        case 0x010E:
            GET_printf_HANDLER() ("\\v{D}");    /* D with caron */
            return 1;

        case 0x010F:
            GET_printf_HANDLER() ("\\v{d}");    /* d with caron */
            return 1;

        case 0x0110:
            GET_printf_HANDLER() ("\\DJ{}");    /* D with stroke */
            return 1;

        case 0x0111:
            GET_printf_HANDLER() ("\\dj{}");    /* d with stroke */
            return 1;

        case 0x0112:
            GET_printf_HANDLER() ("\\=E");      /* E with macron */
            return 1;

        case 0x0113:
            GET_printf_HANDLER() ("\\=e");      /* e with macron */
            return 1;

        case 0x0114:
            GET_printf_HANDLER() ("\\u{E}");    /* E with breve */
            return 1;

        case 0x0115:
            GET_printf_HANDLER() ("\\u{e}");    /* e with breve */
            return 1;

        case 0x0116:
            GET_printf_HANDLER() ("\\.E");      /* E with dot above */
            return 1;

        case 0x0117:
            GET_printf_HANDLER() ("\\.e");      /* e with dot above */
            return 1;

        case 0x0118:
            GET_printf_HANDLER() ("\\k{E}");    /* E with ogonek */
            return 1;

        case 0x0119:
            GET_printf_HANDLER() ("\\k{e}");    /* e with ogonek */
            return 1;

        case 0x011A:
            GET_printf_HANDLER() ("\\v{E}");    /* E with caron */
            return 1;

        case 0x011B:
            GET_printf_HANDLER() ("\\v{e}");    /* e with caron */
            return 1;

        case 0x011C:
            GET_printf_HANDLER() ("\\^G");      /* G with circumflex */
            return 1;

        case 0x011D:
            GET_printf_HANDLER() ("\\^g");      /* g with circumflex */
            return 1;

        case 0x011E:
            GET_printf_HANDLER() ("\\u{G}");    /* G with breve */
            return 1;

        case 0x011F:
            GET_printf_HANDLER() ("\\u{g}");    /* g with breve */
            return 1;

        case 0x0120:
            GET_printf_HANDLER() ("\\.G");      /* G with dot above */
            return 1;

        case 0x0121:
            GET_printf_HANDLER() ("\\u{g}");    /* g with dot above */
            return 1;

        case 0x0122:
            GET_printf_HANDLER() ("^H"); /* H with circumflex */
            return 1;

        case 0x0123:
            GET_printf_HANDLER() ("^h"); /* h with circumflex */
            return 1;

        case 0x0128:
            GET_printf_HANDLER() ("\\~I");      /* I with tilde */
            return 1;

        case 0x0129:
            GET_printf_HANDLER() ("\\~{\\i}");  /* i with tilde (dotless) */
            return 1;

        case 0x012A:
            GET_printf_HANDLER() ("\\=I");      /* I with macron */
            return 1;

        case 0x012B:
            GET_printf_HANDLER() ("\\={\\i}");  /* i with macron (dotless) */
            return 1;

        case 0x012C:
            GET_printf_HANDLER() ("\\u{I}");    /* I with breve */
            return 1;

        case 0x012D:
            GET_printf_HANDLER() ("\\u{\\i}");  /* i with breve */
            return 1;

        case 0x0130:
            GET_printf_HANDLER() ("\\.I");      /* I with dot above */
            return 1;

        case 0x0131:
            GET_printf_HANDLER() ("\\i{}");     /* dotless i */
            return 1;

        case 0x0132:
            GET_printf_HANDLER() ("IJ"); /* IJ ligature */
            return 1;

        case 0x0133:
            GET_printf_HANDLER() ("ij"); /* ij ligature  */
            return 1;

        case 0x0134:
            GET_printf_HANDLER() ("\\^J");      /* J with circumflex (dotless) */
            return 1;

        case 0x0135:
            GET_printf_HANDLER() ("\\^{\\j}");  /* j with circumflex (dotless) */
            return 1;

        case 0x0136:
            GET_printf_HANDLER() ("\\c{K}");    /* K with cedilla */
            return 1;

        case 0x0137:
            GET_printf_HANDLER() ("\\c{k}");    /* k with cedilla */
            return 1;

        case 0x0138:
            GET_printf_HANDLER() ("k");         /* NOTE: Not the correct character (kra), but similar */
            return 1;

        case 0x0139:
            GET_printf_HANDLER() ("\\'L");      /* L with acute */
            return 1;

        case 0x013A:
            GET_printf_HANDLER() ("\\'l");      /* l with acute  */
            return 1;

        case 0x013B:
            GET_printf_HANDLER() ("\\c{L}");    /* L with cedilla */
            return 1;

        case 0x013C:
            GET_printf_HANDLER() ("\\c{l}");    /* l with cedilla */
            return 1;

        case 0x013D:
            GET_printf_HANDLER() ("\\v{L}");    /* L with caron */
            return 1;

        case 0x013E:
            GET_printf_HANDLER() ("\\v{l}");    /* l with caron */
            return 1;

        case 0x0141:
            GET_printf_HANDLER() ("\\L{}");     /* L with stroke */
            return 1;

        case 0x0142:
            GET_printf_HANDLER() ("\\l{}");     /* l with stroke  */
            return 1;

        case 0x0143:
            GET_printf_HANDLER() ("\\'N");      /* N with acute */
            return 1;

        case 0x0144:
            GET_printf_HANDLER() ("\\'n");      /* n with acute */
            return 1;

        case 0x0145:
            GET_printf_HANDLER() ("\\c{N}");    /* N with cedilla */
            return 1;

        case 0x0146:
            GET_printf_HANDLER() ("\\c{n}");    /* n with cedilla */
            return 1;

        case 0x0147:
            GET_printf_HANDLER() ("\\v{N}");    /* N with caron */
            return 1;

        case 0x0148:
            GET_printf_HANDLER() ("\\v{n}");    /* n with caron */
            return 1;

        case 0x0149:
            GET_printf_HANDLER() ("'n"); /* n preceed with apostroph  */
            return 1;

        case 0x014A:
            GET_printf_HANDLER() ("\\NG{}");    /* ENG character */
            return 1;

        case 0x014B:
            GET_printf_HANDLER() ("\\ng{}");    /* eng character */
            return 1;

        case 0x014C:
            GET_printf_HANDLER() ("\\=O");      /* O with macron */
            return 1;

        case 0x014D:
            GET_printf_HANDLER() ("\\=o");      /* o with macron */
            return 1;

        case 0x014E:
            GET_printf_HANDLER() ("\\u{O}");    /* O with breve */
            return 1;

        case 0x014F:
            GET_printf_HANDLER() ("\\u{o}");    /* o with breve */
            return 1;

        case 0x0150:
            GET_printf_HANDLER() ("\\H{O}");    /* O with double acute */
            return 1;

        case 0x0151:
            GET_printf_HANDLER() ("\\H{o}");    /* o with double acute */
            return 1;

        case 0x0152:
            GET_printf_HANDLER() ("\\OE{}");    /* OE ligature */
            return 1;

        case 0x0153:
            GET_printf_HANDLER() ("\\oe{}");    /* oe ligature */
            return 1;

        case 0x0154:
            GET_printf_HANDLER() ("\\'R");      /* R with acute */
            return 1;

        case 0x0155:
            GET_printf_HANDLER() ("\\'r");      /* r with acute */
            return 1;

        case 0x0156:
            GET_printf_HANDLER() ("\\c{R}");    /* R with cedilla */
            return 1;

        case 0x0157:
            GET_printf_HANDLER() ("\\c{r}");    /* r with cedilla */
            return 1;

        case 0x0158:
            GET_printf_HANDLER() ("\\v{R}");    /* R with caron */
            return 1;

        case 0x0159:
            GET_printf_HANDLER() ("\\v{r}");    /* r with caron */
            return 1;

        case 0x015A:
            GET_printf_HANDLER() ("\\'S");      /* S with acute */
            return 1;

        case 0x015B:
            GET_printf_HANDLER() ("\\'s");      /* s with acute */
            return 1;

        case 0x015C:
            GET_printf_HANDLER() ("\\^S");      /* S with circumflex */
            return 1;

        case 0x015D:
            GET_printf_HANDLER() ("\\^s");      /* c with circumflex */
            return 1;

        case 0x015E:
            GET_printf_HANDLER() ("\\c{S}");    /* S with cedilla */
            return 1;

        case 0x015F:
            GET_printf_HANDLER() ("\\c{s}");    /* s with cedilla */
            return 1;

        case 0x0160:
            GET_printf_HANDLER() ("\\v{S}");    /* S with caron */
            return 1;

        case 0x0161:
            GET_printf_HANDLER() ("\\v{s}");    /* s with caron */
            return 1;

        case 0x0162:
            GET_printf_HANDLER() ("\\c{T}");    /* T with cedilla */
            return 1;

        case 0x0163:
            GET_printf_HANDLER() ("\\c{t}");    /* t with cedilla */
            return 1;

        case 0x0164:
            GET_printf_HANDLER() ("\\v{T}");    /* T with caron */
            return 1;

        case 0x0165:
            GET_printf_HANDLER() ("\\v{t}");    /* t with caron */
            return 1;

        case 0x0168:
            GET_printf_HANDLER() ("\\~U");      /* U with tilde */
            return 1;

        case 0x0169:
            GET_printf_HANDLER() ("\\~u");      /* u with tilde */
            return 1;

        case 0x016A:
            GET_printf_HANDLER() ("\\=U");      /* U with macron */
            return 1;

        /* Greek (thanks Petr Vanicek!): */
        case 0x0391:
            GET_printf_HANDLER() ("$A$");
            return 1;

        case 0x0392:
            GET_printf_HANDLER() ("$B$");
            return 1;

        case 0x0393:
            GET_printf_HANDLER() ("$\\Gamma$");
            return 1;

        case 0xf044:            /* Mac ? */
        case 0x2206:            /* Mac */
        case 0x0394:
            GET_printf_HANDLER() ("$\\Delta$");
            return 1;

        case 0x0395:
            GET_printf_HANDLER() ("$E$");
            return 1;

        case 0x0396:
            GET_printf_HANDLER() ("$Z$");
            return 1;

        case 0x0397:
            GET_printf_HANDLER() ("$H$");
            return 1;

        case 0x0398:
            GET_printf_HANDLER() ("$\\Theta$");
            return 1;

        case 0x0399:
            GET_printf_HANDLER() ("$I$");
            return 1;

        case 0x039a:
            GET_printf_HANDLER() ("$K$");
            return 1;

        case 0x039b:
            GET_printf_HANDLER() ("$\\Lambda$");
            return 1;

        case 0xf04d:            /* Mac? */
        case 0x039c:
            GET_printf_HANDLER() ("$M$");
            return 1;

        case 0x039d:
            GET_printf_HANDLER() ("$N$");
            return 1;

        case 0x039e:
            GET_printf_HANDLER() ("$\\Xi$");
            return 1;

        case 0x039f:
            GET_printf_HANDLER() ("$O$"); /* Omicron */
            return 1;

        case 0x03a0:
            GET_printf_HANDLER() ("$\\Pi$");
            return 1;

        case 0x03a1:
            GET_printf_HANDLER() ("$R$");
            return 1;

        case 0x03a3:
            GET_printf_HANDLER() ("$\\Sigma$");
            return 1;

        case 0x03a4:
            GET_printf_HANDLER() ("$T$");
            return 1;

        case 0x03a5:
            GET_printf_HANDLER() ("$Y$");
            return 1;

        case 0x03a6:
            GET_printf_HANDLER() ("$\\Phi$");
            return 1;

        case 0x03a7:
            GET_printf_HANDLER() ("$X$"); /* Chi */
            return 1;

        case 0x03a8:
            GET_printf_HANDLER() ("$\\Psi$");
            return 1;

        case 0x2126:            /* Mac */
        case 0x03a9:
            GET_printf_HANDLER() ("$\\Omega$");
            return 1;

        /* ...and lower case: */

        case 0x03b1:
            GET_printf_HANDLER() ("$\\alpha$");
            return 1;

        case 0x03b2:
            GET_printf_HANDLER() ("$\\beta$");
            return 1;

        case 0xf067:            /* Mac */
        case 0x03b3:
            GET_printf_HANDLER() ("$\\gamma$");
            return 1;

        case 0xf064:            /* Mac */
        case 0x03b4:
            GET_printf_HANDLER() ("$\\delta$");
            return 1;

        case 0x03b5:
            GET_printf_HANDLER() ("$\\epsilon$");
            return 1;

        case 0xf04e:            /* Mac? variant? */
        case 0xf07a:            /* Mac? */
        case 0x03b6:
            GET_printf_HANDLER() ("$\\zeta$");
            return 1;

        case 0x03b7:
            GET_printf_HANDLER() ("$\\eta$");
            return 1;

        case 0x03b8:
            GET_printf_HANDLER() ("$\\theta$");
            return 1;

        case 0x03b9:
            GET_printf_HANDLER() ("$\\iota$");
            return 1;

        case 0x03ba:
            GET_printf_HANDLER() ("$\\kappa$");
            return 1;

        case 0xf06c:            /* Mac? */
        case 0x03bb:
            GET_printf_HANDLER() ("$\\lambda$");
            return 1;

        case 0x03bc:
            GET_printf_HANDLER() ("$\\mu$");
            return 1;

        case 0x03bd:
            GET_printf_HANDLER() ("$\\nu$");
            return 1;

        case 0x03be:
            GET_printf_HANDLER() ("$\\xi$");
            return 1;

        case 0x03bf:
            GET_printf_HANDLER() ("$o$"); /* omicron */
            return 1;

        case 0x03c0:
            GET_printf_HANDLER() ("$\\pi$");
            return 1;

        case 0xf072:            /* Mac */
            GET_printf_HANDLER() ("$\\varrho$");
            return 1;

        case 0x03c1:
            GET_printf_HANDLER() ("$\\rho$");
            return 1;

        case 0xf073:            /* Mac */
        case 0x03c3:
            GET_printf_HANDLER() ("$\\sigma$");
            return 1;

        case 0x03c4:
            GET_printf_HANDLER() ("$\\tau$");
            return 1;

        case 0x03c5:
            GET_printf_HANDLER() ("$\\upsilon$");
            return 1;

        case 0x03c6:
            GET_printf_HANDLER() ("$\\phi$");
            return 1;

        case 0x03c7:
            GET_printf_HANDLER() ("$\\chi$");
            return 1;

        case 0x03c8:
            GET_printf_HANDLER() ("$\\psi$");
            return 1;

        case 0x03c9:
            GET_printf_HANDLER() ("$\\omega$");
            return 1;

        case 0xf06a:                             /* Mac? */
        case 0x03d5:
            GET_printf_HANDLER() ("$\\varphi$"); /* ? */
            return 1;

        /* More math, typical inline: */
        case 0x2111:
            GET_printf_HANDLER() ("$\\Im$");
            return 1;

        case 0x2118:
            GET_printf_HANDLER() ("$\\wp$");    /* Weierstrass p */
            return 1;

        case 0x211c:
            GET_printf_HANDLER() ("$\\Re$");
            return 1;

        case 0x2135:
            GET_printf_HANDLER() ("$\\aleph$");
            return 1;

        case 0x2190:
            GET_printf_HANDLER() ("$\\leftarrow$");
            return 1;

        case 0x2191:
            GET_printf_HANDLER() ("$\\uparrow$");
            return 1;

        case 0xf0ae:            /* Mac */
        case 0x2192:
            GET_printf_HANDLER() ("$\\rightarrow$");
            return 1;

        case 0x2193:
            GET_printf_HANDLER() ("$\\downarrow$");
            return 1;

        case 0x21d0:
            GET_printf_HANDLER() ("$\\Leftarrow$");
            return 1;

        case 0x21d1:
            GET_printf_HANDLER() ("$\\Uparrow$");
            return 1;

        case 0x21d2:
            GET_printf_HANDLER() ("$\\Rightarrow$");
            return 1;

        case 0x21d3:
            GET_printf_HANDLER() ("$\\Downarrow$");
            return 1;

        case 0x21d4:
            GET_printf_HANDLER() ("$\\Leftrightarrow$");
            return 1;

        case 0x2200:
            GET_printf_HANDLER() ("$\\forall$");
            return 1;

        case 0xf0b6:            /* Mac */
        case 0x2202:
            GET_printf_HANDLER() ("$\\partial$");
            return 1;

        case 0x2203:
            GET_printf_HANDLER() ("$\\exists$");
            return 1;

        case 0x2205:
            GET_printf_HANDLER() ("$\\emptyset$");
            return 1;

        case 0x2207:
            GET_printf_HANDLER() ("$\\nabla$");
            return 1;

        case 0x2208:
            GET_printf_HANDLER() ("$\\in$");    /* element of */
            return 1;

        case 0x2209:
            GET_printf_HANDLER() ("$\\notin$"); /* not an element of */
            return 1;

        case 0x220b:
            GET_printf_HANDLER() ("$\\ni$");    /* contains as member */
            return 1;

        case 0x221a:
            GET_printf_HANDLER() ("$\\surd$");  /* sq root */
            return 1;

        case 0x2212:
            GET_printf_HANDLER() ("$-$"); /* minus */
            return 1;

        case 0x221d:
            GET_printf_HANDLER() ("$\\propto$");
            return 1;

        case 0x221e:
            GET_printf_HANDLER() ("$\\infty$");
            return 1;

        case 0x2220:
            GET_printf_HANDLER() ("$\\angle$");
            return 1;

        case 0x2227:
            GET_printf_HANDLER() ("$\\land$");  /* logical and */
            return 1;

        case 0x2228:
            GET_printf_HANDLER() ("$\\lor$");   /* logical or */
            return 1;

        case 0x2229:
            GET_printf_HANDLER() ("$\\cap$");   /* intersection */
            return 1;

        case 0x222a:
            GET_printf_HANDLER() ("$\\cup$");   /* union */
            return 1;

        case 0x223c:
            GET_printf_HANDLER() ("$\\sim$");   /* similar to  */
            return 1;

        case 0x2248:
            GET_printf_HANDLER() ("$\\approx$");
            return 1;

        case 0x2261:
            GET_printf_HANDLER() ("$\\equiv$");
            return 1;

        case 0x2260:
            GET_printf_HANDLER() ("$\\neq$");
            return 1;

        case 0x2264:
            GET_printf_HANDLER() ("$\\leq$");
            return 1;

        case 0xf0b3:            /* Mac? */
        case 0x2265:
            GET_printf_HANDLER() ("$\\geq$");
            return 1;

        case 0x2282:
            GET_printf_HANDLER() ("$\\subset$");
            return 1;

        case 0x2283:
            GET_printf_HANDLER() ("$\\supset$");
            return 1;

        case 0x2284:
            GET_printf_HANDLER() ("$\\notsubset$");
            return 1;

        case 0x2286:
            GET_printf_HANDLER() ("$\\subseteq$");
            return 1;

        case 0x2287:
            GET_printf_HANDLER() ("$\\supseteq$");
            return 1;

        case 0x2295:
            GET_printf_HANDLER() ("$\\oplus$"); /* circled plus */
            return 1;

        case 0x2297:
            GET_printf_HANDLER() ("$\\otimes$");
            return 1;

        case 0x22a5:
            GET_printf_HANDLER() ("$\\perp$");  /* perpendicular */
            return 1;



        case 0x2660:
            GET_printf_HANDLER() ("$\\spadesuit$");
            return 1;

        case 0x2663:
            GET_printf_HANDLER() ("$\\clubsuit$");
            return 1;

        case 0x2665:
            GET_printf_HANDLER() ("$\\heartsuit$");
            return 1;

        case 0x2666:
            GET_printf_HANDLER() ("$\\diamondsuit$");
            return 1;


        case 0x01C7:
            GET_printf_HANDLER() ("LJ"); /* the LJ letter */
            return 1;

        case 0x01C8:
            GET_printf_HANDLER() ("Lj"); /* the Lj letter */
            return 1;

        case 0x01C9:
            GET_printf_HANDLER() ("lj"); /* the lj letter */
            return 1;

        case 0x01CA:
            GET_printf_HANDLER() ("NJ"); /* the NJ letter */
            return 1;

        case 0x01CB:
            GET_printf_HANDLER() ("Nj"); /* the Nj letter */
            return 1;

        case 0x01CC:
            GET_printf_HANDLER() ("nj"); /* the nj letter */
            return 1;

        case 0x01CD:
            GET_printf_HANDLER() ("\\v{A}");    /* A with caron */
            return 1;

        case 0x01CE:
            GET_printf_HANDLER() ("\\v{a}");    /* a with caron */
            return 1;

        case 0x01CF:
            GET_printf_HANDLER() ("\\v{I}");    /* I with caron */
            return 1;

        case 0x01D0:
            GET_printf_HANDLER() ("\\v{\\i}");  /* i with caron (dotless) */
            return 1;

        case 0x01D1:
            GET_printf_HANDLER() ("\\v{O}");    /* O with caron */
            return 1;

        case 0x01D2:
            GET_printf_HANDLER() ("\\v{o}");    /* o with caron */
            return 1;

        case 0x01D3:
            GET_printf_HANDLER() ("\\v{U}");    /* U with caron */
            return 1;

        case 0x01D4:
            GET_printf_HANDLER() ("\\v{u}");    /* u with caron */
            return 1;

        case 0x01E6:
            GET_printf_HANDLER() ("\\v{G}");    /* G with caron */
            return 1;

        case 0x01E7:
            GET_printf_HANDLER() ("\\v{g}");    /* g with caron */
            return 1;

        case 0x01E8:
            GET_printf_HANDLER() ("\\v{K}");    /* K with caron */
            return 1;

        case 0x01E9:
            GET_printf_HANDLER() ("\\v{k}");    /* k with caron */
            return 1;


        case 0x01F0:
            GET_printf_HANDLER() ("\\v{\\j}");  /* j with caron (dotless) */
            return 1;

        case 0x01F1:
            GET_printf_HANDLER() ("DZ"); /* the DZ letter */
            return 1;

        case 0x01F2:
            GET_printf_HANDLER() ("Dz"); /* the Dz letter */
            return 1;

        case 0x01F3:
            GET_printf_HANDLER() ("dz"); /* the dz letter */
            return 1;

        case 0x01F4:
            GET_printf_HANDLER() ("\\'G");      /* G with acute */
            return 1;

        case 0x01F5:
            GET_printf_HANDLER() ("\\'g");      /* g with acute */
            return 1;

        case 0x01FA:
            GET_printf_HANDLER() ("\\'{\\AA}"); /* Å with acute */
            return 1;

        case 0x01FB:
            GET_printf_HANDLER() ("\\'{\\aa}"); /* å with acute */
            return 1;

        case 0x01FC:
            GET_printf_HANDLER() ("\\'{\\AE}"); /* Æ with acute */
            return 1;

        case 0x01FD:
            GET_printf_HANDLER() ("\\'{\\ae}"); /* æ with acute */
            return 1;

        case 0x01FE:
            GET_printf_HANDLER() ("\\'{\\O}");  /* Ø with acute */
            return 1;

        case 0x01FF:
            GET_printf_HANDLER() ("\\'{\\o}");  /* ø with acute */
            return 1;

        case 0x2010:
            GET_printf_HANDLER() ("-");         /* hyphen */
            return 1;

        case 0x2011:
            GET_printf_HANDLER() ("-");         /* non-breaking hyphen (is there a way to get this in LaTeX?) */
            return 1;

        case 0x2012:
            GET_printf_HANDLER() ("--"); /* figure dash (similar to en-dash) */
            return 1;

        case 0x2013:

            /*
               soft-hyphen? Or en-dash? I find that making
               this a soft-hyphen works very well, but makes
               the occasional "hard" word-connection hyphen
               (like the "-" in roller-coaster) disappear.
               (Are these actually en-dashes? Dunno.)
               How does MS Word distinguish between the 0x2013's
               that signify soft hyphens and those that signify
               word-connection hyphens? wvware should be able
               to as well. -- MV 8.7.2000

               U+2013 is the en-dash character and not a soft
               hyphen. Soft hyphen is U+00AD. Changing to
               "--". -- 2000-08-11 huftis@bigfoot.com
             */
            GET_printf_HANDLER() ("--");
            return 1;

        case 0x016B:
            GET_printf_HANDLER() ("\\=u");      /* u with macron */
            return 1;

        case 0x016C:
            GET_printf_HANDLER() ("\\u{U}");    /* U with breve */
            return 1;

        case 0x016D:
            GET_printf_HANDLER() ("\\u{u}");    /* u with breve */
            return 1;

        case 0x016E:
            GET_printf_HANDLER() ("\\r{U}");    /* U with ring above */
            return 1;

        case 0x016F:
            GET_printf_HANDLER() ("\\r{u}");    /* u with ring above */
            return 1;

        case 0x0170:
            GET_printf_HANDLER() ("\\H{U}");    /* U with double acute */
            return 1;

        case 0x0171:
            GET_printf_HANDLER() ("\\H{u}");    /* u with double acute */
            return 1;

        case 0x0174:
            GET_printf_HANDLER() ("\\^W");      /* W with circumflex */
            return 1;

        case 0x0175:
            GET_printf_HANDLER() ("\\^w");      /* w with circumflex */
            return 1;

        case 0x0176:
            GET_printf_HANDLER() ("\\^Y");      /* Y with circumflex */
            return 1;

        case 0x0177:
            GET_printf_HANDLER() ("\\^y");      /* y with circumflex */
            return 1;

        case 0x0178:
            GET_printf_HANDLER() ("\\\"Y");     /* Y with diaeresis */
            return 1;

        case 0x0179:
            GET_printf_HANDLER() ("\\'Z");      /* Z with acute */
            return 1;

        case 0x017A:
            GET_printf_HANDLER() ("\\'z");      /* z with acute */
            return 1;

        case 0x017B:
            GET_printf_HANDLER() ("\\.Z");      /* Z with dot above */
            return 1;

        case 0x017C:
            GET_printf_HANDLER() ("\\.z");      /* z with dot above */
            return 1;

        case 0x017D:
            GET_printf_HANDLER() ("\\v{Z}");    /* Z with caron */
            return 1;

        case 0x017E:
            GET_printf_HANDLER() ("\\v{z}");    /* z with caron */
            return 1;

        /* Windows specials (MV 4.7.2000). More could be added.
           See http://www.hut.fi/u/jkorpela/www/windows-chars.html
         */

        case 0x2014:
            GET_printf_HANDLER() ("---"); /* em-dash */
            return 1;

        case 0x2018:
            GET_printf_HANDLER() ("`");         /* left single quote, Win */
            return 1;

        case 0x2019:
            GET_printf_HANDLER() ("'");         /* Right single quote, Win */
            return 1;

        case 0x201A:
            GET_printf_HANDLER() ("\\quotesinglbase{}"); /* single low 99 quotation mark */
            return 1;

        case 0x201C:
            GET_printf_HANDLER() ("``"); /* inverted double quotation mark */
            return 1;

        case 0x201D:
            GET_printf_HANDLER() ("''"); /* double q.m. */
            return 1;

        case 0x201E:
            GET_printf_HANDLER() ("\\quotedblbase{}");  /* double low 99 quotation mark */
            return 1;

        case 0x2020:
            GET_printf_HANDLER() ("\\dag{}");   /* dagger */
            return 1;

        case 0x2021:
            GET_printf_HANDLER() ("\\ddag{}");  /* double dagger */
            return 1;

        case 0x25cf:                             /* FilledCircle */
        case 0x2022:
            GET_printf_HANDLER() ("$\\bullet$"); /* bullet */
            return 1;

        case 0x2023:
            GET_printf_HANDLER() ("$\\bullet$"); /* NOTE: Not a real triangular bullet */
            return 1;

        case 0x2024:
            GET_printf_HANDLER() (".");         /* One dot leader (for use in TOCs) */
            return 1;

        case 0x2025:
            GET_printf_HANDLER() (".."); /* Two dot leader (for use in TOCs) */
            return 1;

        case 0x2026:
            GET_printf_HANDLER() ("\\ldots");   /* ellipsis */
            return 1;

        case 0x2039:
            GET_printf_HANDLER() ("\\guilsinglleft{}"); /* single left angle quotation mark */
            return 1;

        case 0x203A:
            GET_printf_HANDLER() ("\\guilsinglright{}"); /* single right angle quotation mark */
            return 1;

        case 0x203C:
            GET_printf_HANDLER() ("!!"); /* double exclamation mark */
            return 1;

        case 0x2215:
            GET_printf_HANDLER() ("$/$"); /* Division slash */
            return 1;

        case 0x2030:
            GET_printf_HANDLER() ("o/oo");
            return 1;

        case 0x20ac:
            GET_printf_HANDLER() ("\\euro");

            /* No known implementation ;-)

               Shouldn't we use the package 'eurofont'?
               -- 2000-08-15 huftis@bigfoot.com
             */
            return 1;

        case 0x2160:
            GET_printf_HANDLER() ("I");         /* Roman numeral I */
            return 1;

        case 0x2161:
            GET_printf_HANDLER() ("II"); /* Roman numeral II */
            return 1;

        case 0x2162:
            GET_printf_HANDLER() ("III"); /* Roman numeral III */
            return 1;

        case 0x2163:
            GET_printf_HANDLER() ("IV"); /* Roman numeral IV */
            return 1;

        case 0x2164:
            GET_printf_HANDLER() ("V");         /* Roman numeral V */
            return 1;

        case 0x2165:
            GET_printf_HANDLER() ("VI"); /* Roman numeral VI */
            return 1;

        case 0x2166:
            GET_printf_HANDLER() ("VII"); /* Roman numeral VII */
            return 1;

        case 0x2167:
            GET_printf_HANDLER() ("VIII");      /* Roman numeral VIII */
            return 1;

        case 0x2168:
            GET_printf_HANDLER() ("IX"); /* Roman numeral IX */
            return 1;

        case 0x2169:
            GET_printf_HANDLER() ("X");         /* Roman numeral X */
            return 1;

        case 0x216A:
            GET_printf_HANDLER() ("XI"); /* Roman numeral XI */
            return 1;

        case 0x216B:
            GET_printf_HANDLER() ("XII"); /* Roman numeral XII */
            return 1;

        case 0x216C:
            GET_printf_HANDLER() ("L");         /* Roman numeral L */
            return 1;

        case 0x216D:
            GET_printf_HANDLER() ("C");         /* Roman numeral C */
            return 1;

        case 0x216E:
            GET_printf_HANDLER() ("D");         /* Roman numeral D */
            return 1;

        case 0x216F:
            GET_printf_HANDLER() ("M");         /* Roman numeral M */
            return 1;

        case 0x2170:
            GET_printf_HANDLER() ("i");         /* Roman numeral i */
            return 1;

        case 0x2171:
            GET_printf_HANDLER() ("ii"); /* Roman numeral ii */
            return 1;

        case 0x2172:
            GET_printf_HANDLER() ("iii"); /* Roman numeral iii */
            return 1;

        case 0x2173:
            GET_printf_HANDLER() ("iv"); /* Roman numeral iv */
            return 1;

        case 0x2174:
            GET_printf_HANDLER() ("v");         /* Roman numeral v */
            return 1;

        case 0x2175:
            GET_printf_HANDLER() ("vi"); /* Roman numeral vi */
            return 1;

        case 0x2176:
            GET_printf_HANDLER() ("vii"); /* Roman numeral vii */
            return 1;

        case 0x2177:
            GET_printf_HANDLER() ("viii");      /* Roman numeral viii */
            return 1;

        case 0x2178:
            GET_printf_HANDLER() ("ix"); /* Roman numeral ix */
            return 1;

        case 0x2179:
            GET_printf_HANDLER() ("x");         /* Roman numeral x */
            return 1;

        case 0x217A:
            GET_printf_HANDLER() ("xi"); /* Roman numeral xi */
            return 1;

        case 0x217B:
            GET_printf_HANDLER() ("xiii");      /* Roman numeral xii */
            return 1;

        case 0x217C:
            GET_printf_HANDLER() ("l");         /* Roman numeral l */
            return 1;

        case 0x217D:
            GET_printf_HANDLER() ("c");         /* Roman numeral c */
            return 1;

        case 0x217E:
            GET_printf_HANDLER() ("d");         /* Roman numeral d */
            return 1;

        case 0x217F:
            GET_printf_HANDLER() ("m");         /* Roman numeral m */
            return 1;
    }
    /* Debugging aid: */
    if (char16 >= 0x80) {
        GET_printf_HANDLER() ("[%x]", char16);
        return 1;
    }
    return 0;
}

int
wvConvertUnicodeToHtml(U16 char16) {
    switch (char16) {
        case 11:
            GET_printf_HANDLER() ("<br>");
            return 1;

        case 31:                           /* non-required hyphen */
            GET_printf_HANDLER()("&shy;"); /*vladimir@lukianov.name HTML 4.01 spec*/
            return 1;

        case 30:
        case 45:
        case 0x2013:
            GET_printf_HANDLER() ("-");         /* en-dash */
            return 1;

        case 12:
        case 13:
        case 14:
        case 7:
            return 1;

        case 34:
            GET_printf_HANDLER() ("&quot;");
            return 1;

        case 38:
            GET_printf_HANDLER() ("&amp;");
            return 1;

        case 60:
            GET_printf_HANDLER() ("&lt;");
            return 1;

        case 62:
            GET_printf_HANDLER() ("&gt;");
            return 1;

        /*
           german characters, im assured that this is the right way to handle them
           by Markus Schulte <markus@dom.de>

           As the output encoding for HTML was chosen as UTF-8,
           we don't need &Auml; etc. etc. I removed all but sz
           -- MV 6.4.2000
         */

        case 0xdf:
            GET_printf_HANDLER() ("&szlig;");
            return 1;

        /* end german characters */
        case 0x2026:
#if 0

/*
   this just looks awful in netscape 4.5, so im going to do a very foolish
   thing and just put ... instead of this
 */
            GET_printf_HANDLER() ("&#133;");
/*is there a proper html name for ... &ellipse;? Yes, &hellip; -- MV */
#endif
            GET_printf_HANDLER() ("&hellip;");
            return 1;

        case 0x2019:
            GET_printf_HANDLER() ("'");
            return 1;

        case 0x2215:
            GET_printf_HANDLER() ("/");
            return 1;

        case 0xF8E7:            /* without this, things should work in theory, but not for me */
            GET_printf_HANDLER() ("_");
            return 1;

        case 0x2018:
            GET_printf_HANDLER() ("`");
            return 1;

        /* Windows specials (MV): */
        case 0x0160:
            GET_printf_HANDLER() ("&Scaron;");
            return 1;

        case 0x0161:
            GET_printf_HANDLER() ("&scaron;");
            return 1;

        case 0x2014:
            GET_printf_HANDLER() ("&mdash;");
            return 1;

        case 0x201c:
            GET_printf_HANDLER() ("&ldquo;");   /* inverted double quotation mark */
            return 1;

        case 0x201d:
            GET_printf_HANDLER() ("&rdquo;");   /* double q.m. */
            return 1;

        case 0x201e:
            GET_printf_HANDLER() ("&bdquo;");   /* below double q.m. */
            return 1;

        case 0x2020:
            GET_printf_HANDLER() ("&dagger;");
            return 1;

        case 0x2021:
            GET_printf_HANDLER() ("&Dagger;");
            return 1;

        case 0x2022:
            GET_printf_HANDLER() ("&bull;");
            return 1;

        case 0x0152:
            GET_printf_HANDLER() ("&OElig;");
            return 1;

        case 0x0153:
            GET_printf_HANDLER() ("&oelig;");
            return 1;

        case 0x0178:
            GET_printf_HANDLER() ("&Yuml;");
            return 1;

        case 0x2030:
            GET_printf_HANDLER() ("&permil;");
            return 1;

        case 0x20ac:
            GET_printf_HANDLER() ("&euro;");
            return 1;

        /* Mac specials (MV): */

        case 0xf020:
            GET_printf_HANDLER() (" ");
            return 1;

        case 0xf02c:
            GET_printf_HANDLER() (",");
            return 1;

        case 0xf028:
            GET_printf_HANDLER() ("(");
            return 1;

        case 0xf03e:
            GET_printf_HANDLER() ("&gt;");
            return 1;

        case 0xf067:
            GET_printf_HANDLER() ("&gamma;");
            return 1;

        case 0xf064:
            GET_printf_HANDLER() ("&delta;");
            return 1;

        case 0xf072:
            GET_printf_HANDLER() ("&rho;");
            return 1;

        case 0xf073:
            GET_printf_HANDLER() ("&sigma;");
            return 1;

        case 0xf0ae:
            GET_printf_HANDLER() ("&rarr;");    /* right arrow */
            return 1;

        case 0xf0b6:
            GET_printf_HANDLER() ("&part;");    /* partial deriv. */
            return 1;

        case 0xf0b3:
            GET_printf_HANDLER() ("&ge;");
            return 1;
    }
    /* Debugging aid: */
    /* if (char16 >= 0x100) GET_printf_HANDLER()("[%x]", char16); */
    return 0;
}

int
wvConvertUnicodeToXml(U16 char16) {
    switch (char16) {
        case 11:
            GET_printf_HANDLER() ("<br />");
            return 1;

        case 30:
        case 31:
        case 12:
        case 13:
        case 14:
        case 7:
            return 1;

        /* Much simpler here, because XML supports only a few entities */
        case 34:
            GET_printf_HANDLER() ("&quot;");
            return 1;

        case 38:
            GET_printf_HANDLER() ("&amp;");
            return 1;

        case 39:
            GET_printf_HANDLER() ("&apos;");
            return 1;

        case 60:
            GET_printf_HANDLER() ("&lt;");
            return 1;

        case 62:
            GET_printf_HANDLER() ("&gt;");
            return 1;
    }

    return 0;
}

char *str_copy(char *d, size_t n, char *s) {
    strncpy(d, s, n);
    d[n - 1] = 0;
    return d;
}

char *str_append(char *d, size_t n, char *s) {
    int max = n - strlen(d) - 1;

    strncat(d, s, max);
    d[n - 1] = 0;
    return d;
}

#define BUF_COPY(d, s)    str_copy(d, sizeof(d), s)

char *
wvConvertStylename(char *stylename, char *outputtype) {
    static char   cached_outputtype[36];
    static GIConv g_iconv_handle = (GIConv) - 1;
    /**FIXME: 100 is just the size of stylename[] from wv.h**/
    static char buffer[100];
    char        *ibuf, *obuf;
    size_t      ibuflen, obuflen, len;

    /* Destroy */
    if (!outputtype) {
        if ((g_iconv_handle != (GIConv) - 1))
            g_iconv_close(g_iconv_handle);
        return NULL;
    }

    /* Initialize */
    if (!g_iconv_handle || strcmp(cached_outputtype, outputtype)) {
        if ((g_iconv_handle != (GIConv) - 1))
            g_iconv_close(g_iconv_handle);

        /**FIXME: don´t know if ISO-8859-1 is really the correct
         **charset for style names with eg umlauts.             **/
        g_iconv_handle = g_iconv_open(outputtype, "ISO-8859-1");
        if (g_iconv_handle == (GIConv) - 1) {
            wvError(("g_iconv_open fail: %d, cannot convert %s to %s\n",
                     errno, "ISO-8859-1", outputtype));
            return stylename;
        }

        BUF_COPY(cached_outputtype, outputtype);
    }

    /* Convert */
    ibuf    = stylename;
    ibuflen = strlen(stylename);
    obuf    = buffer;
    obuflen = sizeof(buffer) - 1;
    len     = g_iconv(g_iconv_handle, &ibuf, &ibuflen, &obuf, &obuflen);
    *obuf   = 0;
    if (len == -1) {
        wvError(("wvConfig.c: can´t iconv()\n"));
        return stylename;
    }

    return buffer;
}
