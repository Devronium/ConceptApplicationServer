
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

#ifndef EAN13_H
#define EAN13_H

#include <stdbool.h>

//Returns the width of an EAN13 barcode (in bytes)
int EAN13_width();

//Builds an EAN13 barcode from th provided data array
//Data should be in the form of a numeric string, ie. "012345678999"
int EAN13_build(char *data, bool *code);

#endif
