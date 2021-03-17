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
#include "wv.h"

void
wvCopyLSPD(LSPD *dest, LSPD *src) {
    dest->dyaLine        = src->dyaLine;
    dest->fMultLinespace = src->fMultLinespace;
}

void
wvInitLSPD(LSPD *item) {
    item->dyaLine        = 0;
    item->fMultLinespace = 0;
}

void
wvGetLSPDFromBucket(LSPD *item, U8 *pointer) {
    item->dyaLine        = dread_16ubit(NULL, &pointer);
    item->fMultLinespace = dread_16ubit(NULL, &pointer);
}
