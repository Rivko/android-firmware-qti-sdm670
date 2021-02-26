
/* Target: GENERIC*/

#ifndef DAL_CONFIG_H
#define DAL_CONFIG_H


#include "DALPropDef.h"

#define DAL_BUILD_MULTI_PROC_IMAGE

#if defined (FEATURE_WINCE) || defined (WINSIM)
   //dont include customer.h for WM 
   //microsoft compiler directiver for align
   #define DAL_ALIGN(bits) __declspec(align(bits))
#elif defined (ARCH_QDSP6)
   //dont include customer.h for QDSP builds, for now
   //GCC compiler directiver for align
   #define DAL_ALIGN(bits) __attribute__ ((aligned (bits)))
#else
   #include "customer.h"
   //assume, for now, rvct22 compiler directiver for align
   #define DAL_ALIGN(bits) __align(bits)
#endif


#ifdef ARCH_QDSP6 /* QDSP6 */
   #define DAL_CONFIG_ARCH       DAL_CONFIG_ARCH_QDSP6_V2
#else
   #ifdef __TARGET_CPU_ARM926EJ_S /* ARM9 */
      #define DAL_CONFIG_ARCH    DAL_CONFIG_ARCH_ARM_V5
   #elif defined (__TARGET_CPU_ARM7TDMI_S) /*ARM7*/
      #define DAL_CONFIG_ARCH    DAL_CONFIG_ARCH_ARM_V4
   #else /* ARM11 */
      #define DAL_CONFIG_ARCH    DAL_CONFIG_ARCH_ARM_V7
   #endif
#endif

/* define the OS env for apps processor */
#if defined (FEATURE_APPS_IMAGE_WINMOB) || defined (FEATURE_WINCE)
   #define DAL_CONFIG_HLOS_ENV DALPROP_HLOS_ENV_WM
   #if defined (FEATURE_DAL_APPS_WM7)
   //WM-7
      #define DAL_CONFIG_HLOS_WM_VER DALPROP_HLOS_ENV_WM7
   #else
   //Assume WM-6
      #define DAL_CONFIG_HLOS_WM_VER DALPROP_HLOS_ENV_WM6
   #endif
#elif defined (FEATURE_NATIVELINUX)
   #define DAL_CONFIG_HLOS_ENV DALPROP_HLOS_ENV_LINUX
#elif !defined (FEATURE_QDSP) /* QDSP images must remain OS agnostic */
   #define DAL_CONFIG_HLOS_ENV DALPROP_HLOS_ENV_BREW
#endif

#define DALGLBCTXT_SPINLOCK_PID  DAL_CONFIG_PROC_TZ

#endif /* DAL_CONFIG_H */

