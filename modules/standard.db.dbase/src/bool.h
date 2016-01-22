/* bool.h */

#ifndef __BOOL_H__
#define __BOOL_H__

#if defined(_INC_WINDOWS) || defined(_WINDOWS_H)
 #define _BOOL_DEFINED
#endif

#ifndef _BOOL_DEFINED
 #define _BOOL_DEFINED
typedef signed int   BOOL;
#endif
#ifndef FALSE
 #define FALSE    0
#endif
#ifndef TRUE
 #define TRUE     1
#endif
#ifndef B2b
 #ifdef __cplusplus
  #define B2b(b)    ((b) ? true : false)
 #else
  #define B2b(b)    ((b) ? TRUE : FALSE)
 #endif
#endif
#endif /* __BOOL_H__ */
