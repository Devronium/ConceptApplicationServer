/**
 * EAN13 Barcode Generator
 * Creates a boolean array corresponding to the EAN13 barcode for a string of
 * 12 numeric only characters.
 * From: https://github.com/ryankurte/ean13
 * Based on: https://github.com/astrokin/EAN13BarcodeGenerator
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Ryan Kurte
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "ean13.h"

#include <stdbool.h>
#define EAN13_MODULE     7
#define EAN13_DIGITS     12
#define EAN13_WIDTH   (sizeof(quiet_zone)*2+sizeof(lead_trailer)*2+sizeof(separator)+EAN13_MODULE*EAN13_DIGITS)

enum parity_e {
    PARITY_ODD = 0,
    PARITY_EVEN = 1
};

static bool quiet_zone[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
static bool lead_trailer[] = {1, 0, 1};
static bool separator[] = {0, 1, 0, 1, 0};

static bool modules_odd_left[10][7] = {
    {0, 0, 0, 1, 1, 0, 1}, {0, 0, 1, 1, 0, 0, 1}, {0, 0, 1, 0, 0, 1, 1}, {0, 1, 1, 1, 1, 0, 1},
    {0, 1, 0, 0, 0, 1, 1}, {0, 1, 1, 0, 0, 0, 1}, {0, 1, 0, 1, 1, 1, 1}, {0, 1, 1, 1, 0, 1, 1},
    {0, 1, 1, 0, 1, 1, 1}, {0, 0, 0, 1, 0, 1, 1}
};

static bool modules_even_left[10][7] = {
    {0, 1, 0, 0, 1, 1, 1}, {0, 1, 1, 0, 0, 1, 1}, {0, 0, 1, 1, 0, 1, 1}, {0, 1, 0, 0, 0, 0, 1},
    {0, 0, 1, 1, 1, 0, 1}, {0, 1, 1, 1, 0, 0, 1}, {0, 0, 0, 0, 1, 0, 1}, {0, 0, 1, 0, 0, 0, 1},
    {0, 0, 0, 1, 0, 0, 1}, {0, 0, 1, 0, 1, 1, 1}
};

static bool modules_right[10][7] = {
    {1, 1, 1, 0, 0, 1, 0}, {1, 1, 0, 0, 1, 1, 0}, {1, 1, 0, 1, 1, 0, 0}, {1, 0, 0, 0, 0, 1, 0},
    {1, 0, 1, 1, 1, 0, 0}, {1, 0, 0, 1, 1, 1, 0}, {1, 0, 1, 0, 0, 0, 0}, {1, 0, 0, 0, 1, 0, 0},
    {1, 0, 0, 1, 0, 0, 0}, {1, 1, 1, 0, 1, 0, 0}
};

static bool parities[10][5] = {
    {0, 0, 0, 0, 0}, {0, 1, 0, 1, 1}, {0, 1, 1, 0, 1}, {0, 1, 1, 1, 0}, {1, 0, 0, 1, 1},
    {1, 1, 0, 0, 1}, {1, 1, 1, 0, 0}, {1, 0, 1, 0, 1}, {1, 0, 1, 1, 0}, {1, 1, 0, 1, 0}
};

static void write_n(bool *dest, bool *src, int n)
{
    int i;
    for (i = 0; i < n; i++) {
        dest[i] = src[i];
    }
}

int EAN13_width() {
    return EAN13_WIDTH;
}

int EAN13_build(char *data, bool *code)
{
    int index = 0;
    int num = 0;
    int start = 0;
    int i;

    //Check input
    for(i=0; i<EAN13_DIGITS; i++) {
        if((data[i] < 0x30) || (data[i] > 0x39)) {
            return -1;
        }
    }

    //Clear code
    for (i = 0; i < EAN13_WIDTH; i++) {
        code[i] = 0;
    }

    //Create leading quiet zone
    write_n(code + index, quiet_zone, sizeof(quiet_zone));
    index += sizeof(quiet_zone);

    //Create leading trailer (porch)
    write_n(code + index, lead_trailer, sizeof(lead_trailer));
    index += sizeof(lead_trailer);

    //Left hand data here
    start = data[0] - 0x30;

    //First char is a special case
    num = data[1] - 0x30;
    write_n(code + index, modules_odd_left[num], EAN13_MODULE);
    index += EAN13_MODULE;

    //Write left hand data
    for (i = 2; i < 7; i++) {
        num = data[i] - 0x30;
        if (parities[start][i - 2] == 0) {
            write_n(code + index, modules_odd_left[num], EAN13_MODULE);
        } else {
            write_n(code + index, modules_even_left[num], EAN13_MODULE);
        }
        index += EAN13_MODULE;
    }

    //Create center separator
    write_n(code + index, separator, sizeof(separator));
    index += sizeof(separator);

    //Right hand data here
    for (i = 7; i < 12; i++) {
        num = data[i] - 0x30;
        write_n(code + index, modules_right[num], 7);
        index += EAN13_MODULE;
    }

    //Calculate Checksum
    int odds = 0;
    int evens = 0;
    for (int i = 0; i < 12; i++) {
        if (i % 2 == 0) {
            evens += data[i] - 0x30;
        } else {
            odds += data[i] - 0x30;
        }
    }

    //Write checksum
    int checksum = 10 - (((odds * 3) + evens) % 10);
    write_n(code + index, modules_right[checksum], EAN13_MODULE);
    index += EAN13_MODULE;

    //Following trailer
    write_n(code + index, lead_trailer, sizeof(lead_trailer));
    index += sizeof(lead_trailer);

    //Trailing quiet zone here
    write_n(code + index, quiet_zone, sizeof(quiet_zone));
    index += sizeof(quiet_zone);

    return 0;
}
