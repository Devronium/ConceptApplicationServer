/*******
*
* FILE INFO:
* project:	RTP_lib
* file:	Macros.h
* started on:	03/26/03
* started by:	Cedric Lacroix <lacroix_cedric@yahoo.com>
*
*
* TODO:
*
* BUGS:
*
* UPDATE INFO:
* updated on:	05/13/03
* updated by:	Cedric Lacroix <lacroix_cedric@yahoo.com>
*
*******/


#ifndef MACROS_H
#define MACROS_H

#ifndef TRUE
 #define     TRUE    1
#endif /* TRUE */

#ifndef FALSE
 #define     FALSE    0
#endif /* FALSE */


/* Set flags version, padding, extention, csrc count, marker and payload type */

#define F_V_FLAGS(hd, v)        do { (hd) &= 0x3F; (hd) |= (((v) << 6) & 0xC0); } while (0)
#define F_P_FLAGS(hd, v)        do { (hd) &= 0xDF; (hd) |= (((v) << 5) & 0x20); } while (0)
#define F_X_FLAGS(hd, v)        do { (hd) &= 0xEF; (hd) |= (((v) << 4) & 0x10); } while (0)
#define F_CC_FLAGS(hd, v)       do { (hd) &= 0xF0; (hd) |= ((v) & 0x0F); } while (0)
#define F_M_MKPT(hd, v)         do { (hd) &= 0x7F; (hd) |= (((v) << 7) & 0x80); } while (0)
#define F_PT_MKPT(hd, v)        do { (hd) &= 0x80; (hd) |= ((v) & 0x7F); } while (0)

#define MEM_TSALLOC(p, n, s)    do { ((p) = calloc(n, s)); } while (FALSE)
#define MEM_SALLOC(p, s)        MEM_TSALLOC(p, 1, s)
#define MEM_TALLOC(p, n)        MEM_TSALLOC(p, n, sizeof *(p))
#define MEM_ALLOC(p)            MEM_TALLOC(p, 1)

#define MEM_SZERO(p, s)         memset((p), 0, s)
#define MEM_ZERO(p)             MEM_SZERO((p), sizeof *(p))

#define LS_ALLOC(p, t)          do { MEM_ALLOC(p); t ## _INIT(p); } while (FALSE)
#define TAILQ_ALLOC(p)          LS_ALLOC(p, TAILQ)
#endif /* MACROS_H */
