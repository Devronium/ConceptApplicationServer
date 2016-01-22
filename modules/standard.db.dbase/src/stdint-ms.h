/* stdint.h adapted for MS compilers */

#ifndef _MSC_VER
/* Use compiler vendor stdint definitions */
 #include <stdint.h>
#else
/* MS is not providing the stdint definitions */

 #ifndef _STDINT_H
  #define _STDINT_H


  #ifdef _MSDOS
typedef signed char        int8_t;
typedef unsigned char      uint8_t;

typedef short              int16_t;
typedef unsigned short     uint16_t;

typedef long               int32_t;
typedef unsigned long      uint32_t;
  #else
typedef __int8             int8_t;
typedef unsigned __int8    uint8_t;

typedef __int16            int16_t;
typedef unsigned __int16   uint16_t;

typedef __int32            int32_t;
typedef unsigned __int32   uint32_t;

typedef __int64            int64_t;
typedef unsigned __int64   uint64_t;
  #endif
 #endif /* _STDINT_H */
#endif  /* !_MSC_VER */
