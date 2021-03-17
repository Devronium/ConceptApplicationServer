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
#include <errno.h>
#include <string.h>
#include "wv.h"

/*
   Released under GPL, written by Caolan.McNamara@ul.ie.

   Copyright (C) 1998,1999
        Caolan McNamara

   Real Life: Caolan McNamara           *  Doing: MSc in HCI
   Work: Caolan.McNamara@ul.ie          *  Phone: +353-86-8790257
   URL: http://skynet.csn.ul.ie/~caolan *  Sig: an oblique strategy
   How would you have done it?
 */

/*
   returns 1 for not an ole doc
   2 ole but not word doc
   -1 for an error of some unknown kind
   0 on success
 */

void
usage(void) {
    GET_fprintf_HANDLER() (stderr, "Usage: wvVersion filename.doc\n");
    exit(-1);
}

int
main(int argc, char *argv[]) {
    FILE          *input;
    int           ret;
    wvParseStruct ps;

    if (argc < 2)
        usage();

    input = fopen(argv[1], "rb");
    if (!input) {
        GET_fprintf_HANDLER() (stderr, "Failed to open %s\n", argv[1]);
        return -1;
    }
    fclose(input);

    wvInit();
    ret = wvInitParser(&ps, argv[1]);

    if (ret == -1) {
        GET_fprintf_HANDLER() (stderr,
                               "%s couldn't be opened as any known word document\n",
                               argv[1]);
        return -1;
    }

    ret = wvQuerySupported(&ps.fib, NULL);

    GET_printf_HANDLER() ("Version: ");
    switch (ret & 0x7fff) {
        case WORD8:
            GET_printf_HANDLER() ("word8 or higher");
            break;

        case WORD7:
            GET_printf_HANDLER() ("word7");
            break;

        case WORD6:
            GET_printf_HANDLER() ("word6");
            break;

        case WORD5:
            GET_printf_HANDLER() ("word5");
            break;

        case WORD2:
            GET_printf_HANDLER() ("word2 (maybe)");
            break;

        default:
            GET_printf_HANDLER() ("unknown msword version");
            break;
    }

    GET_printf_HANDLER() (", Encrypted: ");
    if (ret & 0x8000)
        GET_printf_HANDLER() ("Yes\n");
    else
        GET_printf_HANDLER() ("No\n");

    wvOLEFree(&ps);
    return 0;
}
