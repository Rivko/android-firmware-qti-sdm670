#ifndef __AMRWB_STDDEF_H 
#define __AMRWB_STDDEF_H

#if defined(COMDEF_H) /* guards against a known re-definer */
#define _BOOLEAN_DEFINED
#define _UINT32_DEFINED
#define _UINT16_DEFINED
#define _UINT8_DEFINED
#define _INT32_DEFINED
#define _INT16_DEFINED
#define _INT8_DEFINED
#define _UINT64_DEFINED
#define _INT64_DEFINED
#define _BYTE_DEFINED
#endif /* #if !defined(COMDEF_H) */

#ifndef int8
#ifndef _INT8_DEFINED
typedef signed char int8;
#define _INT8_DEFINED               /**< flag */
#endif
#endif

#ifndef uint8
#ifndef _UINT8_DEFINED
typedef unsigned char uint8;
#define _UINT8_DEFINED             /**< flag */
#endif
#endif


#ifndef boolean
#ifndef _BOOLEAN_DEFINED
typedef unsigned char boolean;
#define _BOOLEAN_DEFINED          /**< flag */
#endif
#endif

#ifndef int16
#ifndef _INT16_DEFINED
typedef short int16;
#define _INT16_DEFINED              /**< flag */
#endif
#endif


#ifndef uint16
#ifndef _UINT16_DEFINED
typedef unsigned short uint16;
#define _UINT16_DEFINED             /**< flag */
#endif
#endif

#ifndef int32
#ifndef _INT32_DEFINED
typedef long int int32;
#define _INT32_DEFINED           /**< flag */
#endif
#endif

#ifndef uint32
#ifndef _UINT32_DEFINED
typedef unsigned long int uint32;
#define _UINT32_DEFINED          /**< flag */
#endif
#endif

#ifndef int64
#ifndef _INT64_DEFINED
typedef long long int64;
#define _INT64_DEFINED                    /**< flag */
#endif
#endif


#endif // __AMRWB_STDDEF_H 

