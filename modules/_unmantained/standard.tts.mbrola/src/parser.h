/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File:    parser_fifo.c
 * Purpose: polymorphic type to parse a "pho file"
 * Author:  Vincent Pagel
 * Email :  mbrola@tcts.fpms.ac.be
 *
 * Copyright (c) 98 Faculte Polytechnique de Mons (TCTS lab)
 * All rights reserved.
 * PERMISSION IS HEREBY DENIED TO USE, COPY, MODIFY, OR DISTRIBUTE THIS
 * SOFTWARE OR ITS DOCUMENTATION FOR ANY PURPOSE WITHOUT WRITTEN
 * AGREEMENT OR ROYALTY FEES.
 *
 * 18/06/98 : Created
 */

#ifndef _PARSER_H
#define _PARSER_H

typedef void * LPPHONE;

/* Return values of the nextphone function */
typedef enum {
    PHO_OK,
    PHO_EOF,
    PHO_FLUSH
} StatePhone;


/* Polymorphic type */

typedef struct Parser   Parser;

typedef void (*reset_ParserFunction)(Parser *ps);
typedef void (*close_ParserFunction)(Parser *ps);
typedef StatePhone (*nextphone_ParserFunction)(Parser *ps, LPPHONE *ph);

/*
 * Generic parser :
 *   reset: forget remaining data in the buffer (when the user STOP synthesis for example
 *
 *   close: release the memory
 *
 *   nextphone: return the next Phoneme from input.
 *
 *     PRECONDITION: this phoneme MUST have a pitch point at 0 and 100%
 *
 *     THE CALLER IS IN CHARGE OF CALLING close_Phone ON THE PHONES HE GETS
 *     WITH nextphone
 */

struct Parser {
    void                     *self;            /* Polymorphic on the real type */
    reset_ParserFunction     reset_Parser;     /* virtual func */
    close_ParserFunction     close_Parser;     /* virtual func */
    nextphone_ParserFunction nextphone_Parser; /* virtual func */
};
#endif
