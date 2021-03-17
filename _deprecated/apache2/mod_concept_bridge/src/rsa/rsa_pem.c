/*	rsa_lib/rsa_pem.c
 *      This is PEM (privacy-Enhanced Mail) related code
 *      read these two documents below for more information
 *      http://www.faqs.org/rfcs/rfc1421.html
 *      http://www.faqs.org/rfcs/rfc1521.html
 *      Last compiled Friday, 2nd November 2001 */

/* Written by Hariyanto Lim
 * hmaxf_urlcr@yahoo.com
 * www.geocities.com/hmaxf_urlcr
 * ====================================================================
 * Copyright (c) 2001.  HMaxF Project, All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Copyright remains to Hariyanto Lim, and as such any Copyright notices in
 *	  the code are not to be removed. If this package is used in a product,
 *    Hariyanto Lim should be given attribution as the author of the parts of the library used.
 *    This can be in the form of a textual message at program startup or
 *    in documentation (online or textual) provided with the package.
 * ===================================================================
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL I, HARIYANTO LIM, THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 */

#include <stdio.h>
#ifdef _WIN32
 #include <malloc.h> // malloc()
#else
 #include <stdlib.h> // malloc()
#endif
#include <memory.h>  // memcpy()

#include "rsa.h"

const unsigned char PEM_encode_data[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', //0-7
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', //8-15
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', //16-23
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', //24-31
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', //32-39
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v', //40-47
    'w', 'x', 'y', 'z', '0', '1', '2', '3', //48-55
    '4', '5', '6', '7', '8', '9', '+', '/',
};                                          //56-63

/*	Since:
 *      (1)	PEM data is not sorted and
 *      (2)	the nature of C/C++ array definition
 *              which have to start from 0(zero)
 *              we have to fill the gap in between with ascii 0 = NULL ..
 *              which also serve as error control */
const unsigned char PEM_decode_data[] = {
    0,   0,  0,  0,  0,  0,  0, 0,                                      //0-7
    0,   0,  0,  0,  0,  0,  0, 0,                                      //8-15
    0,   0,  0,  0,  0,  0,  0, 0,                                      //16-23
    0,   0,  0,  0,  0,  0,  0, 0,                                      //24-31
    0,   0,  0,  0,  0,  0,  0, 0,                                      //32-39
    0,   0,  0, 62,  0,  0,  0, 63,                                     //40-47
    52, 53, 54, 55, 56, 57, 58, 59,                                     //48-55
    60, 61,  0,  0,  0,  0,  0, 0,                                      //56-63 ... 61 = '='
    0,   0,  1,  2,  3,  4,  5, 6,                                      //64-71
    7,   8,  9, 10, 11, 12, 13, 14,                                     //72-79
    15, 16, 17, 18, 19, 20, 21, 22,                                     //80-87
    23, 24, 25,  0,  0,  0,  0, 0,                                      //88-95
    0,  26, 27, 28, 29, 30, 31, 32,                                     //96-103
    33, 34, 35, 36, 37, 38, 39, 40,                                     //104-111
    41, 42, 43, 44, 45, 46, 47, 48,                                     //112-119
    49, 50, 51,
};                                                                      //120-122

const unsigned char pad = '=';
// use constant because memcpy() will give warning if use #define
//const unsigned long crlf_length =	2;
//const unsigned char crlf[] =		{ 0x0D, 0x0A};

const unsigned char CRLF[] = { 13, 10 }; // CR = 13, LF = 10
const unsigned long CRLF_length  = 2;
const unsigned long max_six_bits = 63;
const int           max_total_character_in_a_line           = 64;
const int           max_total_character_in_a_line_plus_CRLF = 66; //max_total_character_in_a_line + CRLF_length;
const int           small_size = 3, big_size = 4, six = 6;

//return the REAL REQUIRED target size
// convert source to PEM store in target ... line-by-line
int PEM_encode(char *source, char *target, int sourceLen, int targetLen) { // convert 8 bits to 6 bits .. increase 33.3% in size
    int           total_block, left_over;
    int           i, y, total_line;
    char          *original_target = target;
    unsigned long *pSource         = (unsigned long *)source;
    unsigned long sourceValue;
    int           final_target_size;
    char          temp[buffer_size];
    char          *pTemp = temp;

    // check size .... this is the minimum size required ... also to prevent memory leak
    if (sourceLen + (sourceLen / 2) > targetLen)
        return 0;

    total_block = sourceLen / small_size;
    pSource     = (unsigned long *)source;
    for (i = 0; i < total_block; i++) {  // start from 1 to avoid subtraction by one to total_block ... optimize
        sourceValue = (*pSource & 0x00ffffffL);

        // store them
        for (y = 0; y < big_size; y++) {
            *(target++)   = PEM_encode_data[sourceValue & max_six_bits];           // direct convert to PEM
            sourceValue >>= six;
        }

        // increase counter
        pSource = (unsigned long *)(((char *)pSource) + small_size);
    }

    // handle the left over if any
    left_over = sourceLen % small_size;
    if (left_over) {
        sourceValue = 0;
        memcpy(&sourceValue, pSource, left_over);

        for (y = 0; y < left_over; y++) {
            *(target++)   = PEM_encode_data[sourceValue & max_six_bits];           // direct convert to PEM
            sourceValue >>= six;
        }
        // last few bit
        *(target++) = PEM_encode_data[sourceValue & max_six_bits];         // direct convert to PEM

        // insert padding
        if (left_over == 1) { // two byte paddding
            *(target++) = '=';
            *(target++) = '=';
        } else
            *(target++) = '=';             // one byte paddding
    }

    // calculate current final size
    if (left_over)
        final_target_size = ((total_block + 1) * big_size);
    else
        final_target_size = (total_block * big_size);

    // rewind
    target = original_target;

    // separates them every max_total_character_in_a_line per-line
    total_line = final_target_size / max_total_character_in_a_line;
    for (i = 0; i < total_line; i++) {
        memcpy(pTemp, target, max_total_character_in_a_line);
        target += max_total_character_in_a_line;
        pTemp  += max_total_character_in_a_line;
        memcpy(pTemp, CRLF, CRLF_length);
        pTemp += CRLF_length;
    }

    // check leftover
    left_over = final_target_size - (total_line * max_total_character_in_a_line);
    if (left_over) { // there is left over ... do it
        memcpy(pTemp, target, left_over);
        pTemp += left_over;
    }

    final_target_size = pTemp - temp;
    if (final_target_size > targetLen)
        return 0;

    memcpy(original_target, temp, final_target_size);

    return final_target_size;
}

int PEM_decode(char *source, char *target, int sourceLen, int targetLen) {
    int           final_size = 0;
    int           total_line, left_over, total_block;
    int           i, y;
    char          *pTarget = target;
    char          *pSource = source;
    char          temp[buffer_size];
    char          *pTemp = temp;
    int           total_padding;
    unsigned long *value256;
    unsigned long longValue;

    // check limit
    if (sourceLen > targetLen)
        return 0;

    // find total line
    total_line = sourceLen / max_total_character_in_a_line_plus_CRLF;
    for (i = 0; i < total_line; i++) {
        memcpy(pTemp, pSource, max_total_character_in_a_line);
        pTemp   += max_total_character_in_a_line;
        pSource += max_total_character_in_a_line_plus_CRLF;
    }

    // check leftover
    left_over = sourceLen - (total_line * max_total_character_in_a_line_plus_CRLF);
    if (left_over) { // there is left over ... do it
        memcpy(pTemp, pSource, left_over);
        pTemp += left_over;
    }

    // get total block
    if (*(pTemp - 1) == pad) {   // check padding
        if (*(pTemp - 2) == pad) // check is there any other padding
            // there is two padding character
            total_padding = 2;
        else
            // there is only one padding character
            total_padding = 1;

        total_block = ((pTemp - temp) / big_size) - 1;
    } else {
        total_padding = 0;         // no padding
        total_block   = (pTemp - temp) / big_size;
    }


    // DO IT .. convert 4 of 6bit to 3 of 8bit
    pTemp = temp;                            // rewind
    for (i = 0; i < total_block; i++) {
        value256 = (unsigned long *)pTarget; // direct store in final .. faster
        for (y = small_size; y >= 0; y--) {  // reverse order .. loop 4 times => 3 to 0
            *value256 <<= six;
            *value256  |= PEM_decode_data[*(pTemp + y)];
        }

        // increase conter
        pTemp   += big_size;
        pTarget += small_size;
    }

    // do the left over if any
    if (total_padding) {                    // if there is padding
        longValue = 0;
        for (y = small_size; y >= 0; y--) { // reverse order .. loop 4 times => 3 to 0
            longValue <<= six;
            longValue  |= PEM_decode_data[*(pTemp + y)];
        }

        memcpy(pTarget, &longValue, small_size - total_padding);
        pTarget += (small_size - total_padding);
    }

    final_size = pTarget - target;

    return final_size;
}

/*
 *      // rewind the target pointer
 *      target = original_target_pointer;
 *
 *      // now translate the six bit value to the standard PEM with CRLF character after 76 character
 *      for(i = 0; i < total_line; i++)
 *      {
 *              memcpy(final, target, max_total_character_in_a_line);
 *
 *              memcpy(final + max_total_character_in_a_line, CRLF, CRLF_length); // add CRLF ...
 *
 *              // increase counter
 *              target += max_total_character_in_a_line;
 *              final += max_total_character_in_a_line_plus_CRLF;
 *      }
 *
 *      // handle the left over if any
 *      left_over = target_size % max_total_character_in_a_line;
 *      if(left_over)
 *              memcpy(final, target, left_over);
 *
 *      //target = original_target_pointer; // rewind pointer before free
 *      //free(target); // free target
 *      free(original_target_pointer);
 *
 *      //final = original_final_pointer; //rewind final pointer ... before return
 *
 *      //return(final);
 *      return(original_final_pointer);
 *
 * }
 */

/*
 * //int PEM_decode(unsigned char *source, long source_size, unsigned long *outputSize)
 * int PEM_decode(char *source, char *target, int sourceLen, int targetLen)
 * {
 *      unsigned char *final = NULL;
 *      //unsigned char *target = NULL;
 *      unsigned long total_lines, left_over;
 *      unsigned long target_size;//, final_size;
 *      unsigned long i;
 *      long y; // allow negative value;
 *      unsigned char *original_final_pointer = NULL;
 *      unsigned char *original_target_pointer = NULL;
 *      unsigned char *source_pointer = NULL;
 *      unsigned long *current_value;
 *      unsigned long total_padding;
 *      unsigned long total_block, temp;
 *
 *      unsigned long tester;
 *
 *      // find total_padding
 *      if(*(source + (source_size - 1)) == pad)
 *      {
 *              if(*(source + (source_size - 2)) == pad)
 *                      total_padding = 2;
 *              else
 *                      total_padding = 1;
 *      }
 *      else
 *              total_padding = 0;
 *
 *      // find total lines
 *      total_lines = source_size / max_total_character_in_a_line_plus_CRLF;
 *
 *      // reserve target space
 *      target_size = source_size - (total_lines * CRLF_length);
 *      target = (unsigned char *)malloc(target_size);
 *      if(target == NULL)
 *              return(NULL);
 *      original_target_pointer = target; // save the original pointer
 *
 *      // reserve final (return value) space
 *      //final_size = target_size - (target_size / big_size) - total_padding;
 * *outputSize = target_size - (target_size / big_size) - total_padding;
 *      //final = (unsigned char *)malloc(final_size);
 *      final = (unsigned char *)malloc(*outputSize);
 *      if(final == NULL)
 *      {
 *              free(target);
 *              return(NULL);
 *      }
 *      original_final_pointer = final;  // save the original pointer
 *
 * //	tester = _msize(original_final_pointer); ///=========================
 *
 *      // define total block of 32bits of 6bits
 *      if(total_padding)
 *              total_block = (target_size / big_size) - 1;
 *      else
 *              total_block = (target_size / big_size);
 *
 *      // get rid of CRLF .. 2 bytes
 *      source_pointer = source;
 *      for(i = 0; i < total_lines; i++)
 *      {
 *              memcpy(target, source_pointer, max_total_character_in_a_line);
 *
 *              // increase counter
 *              target += max_total_character_in_a_line;
 *              source_pointer += max_total_character_in_a_line_plus_CRLF;
 *      }
 *
 *      // do left_over if any
 *      left_over = source_size % max_total_character_in_a_line_plus_CRLF;
 *      if(left_over)
 *              memcpy(target, source_pointer, left_over);
 *
 *      // translate 32bits of 6bits to 24bits of 8bits ascii
 *      //memset(final, 0, final_size); // clear value before we fill data in
 *      memset(final, 0, *outputSize); // clear value before we fill data in
 * //	tester = _msize(original_final_pointer); ///=========================
 *      target = original_target_pointer; // rewind target pointer
 *      for(i = 0; i < total_block; i++)
 *      {
 *              current_value = (unsigned long *)final; // direct store in final .. faster
 *              for(y = small_size; y >= 0; y--) // reverse order .. loop 4 times => 3 to 0
 *              {
 * *current_value <<= six;
 * *current_value |= PEM_decode_data[*(target + y)];
 *              }
 *
 *              // increase conter
 *              target += big_size;
 *              final += small_size;
 *      }
 *
 *      // do the left over if any
 *      if(total_padding) // if there is padding
 *      {
 *              temp = 0;//(unsigned long *)final; // direct store in final .. faster
 *              for(y = small_size; y >= 0; y--) // reverse order .. loop 4 times => 3 to 0
 *              {
 *                      temp <<= six;
 *                      temp |= PEM_decode_data[*(target + y)];
 *              }
 *
 *              memcpy(final, &temp, small_size - total_padding);
 *      }
 *
 *      // rewind the pointer and free
 *      //target = original_target_pointer;
 *      //free(target);
 *
 *      free(original_target_pointer);
 *
 *      //final = original_final_pointer ; // rewind pointer
 *
 *      //return(final);
 *
 *      tester = _msize(original_final_pointer);
 *      return(original_final_pointer);
 * }
 */
