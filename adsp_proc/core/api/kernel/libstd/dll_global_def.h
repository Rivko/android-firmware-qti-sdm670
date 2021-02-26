#ifndef DLL_GLOBAL_DEF_H
#define DLL_GLOBAL_DEF_H
/** vi: tw=128 ts=3 sw=3 et :
@file dll_global_def.h
@brief This file contains the API for the Run Control Framework, API 3.0
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.qdsp6/2.1/api/kernel/libstd/dll_global_def.h#1 $
$DateTime: 2017/07/21 22:10:47 $
$Change: 13907704 $
$Author: pwbldsvc $
===============================================================================*/

/**
DLL_GLOBAL_DEF H Macros

There will never be a runtime component associated to this header file. It is only
to supply consistent compiler macros that can be used to decorate function signatures
for DLL compilation. There will not be a compile time header file dependency to this
header file; this header file will remain a leaf in any dependency graph.
*/

/**
API Function and Symbol Visibility Helpers

Imperical testing of code structured using these helper macros indicates that the
symbol table management of DLL is much better and facilitates faster load times.
The link time flags to enable symbol default visibility for 'hidden' requires that
symbols be explicitly marked for external visibility.

BEST PRACTICE ONLY, OPERATIONAL ONLY WITH SELECTED COMPILERS, IF THE COMPILER IS
NOT SUPPORTED, THE MECHANICS ARE DISABLED. Even when a compiler is not supported,
the sources can still be annotated. No apologies are offered on macro syntax -- we
have to do portable things with the preprocessor pass of compilation.

Per Internet Tribal Knowledge, DLL are compiled within opensource community based
upon compiler command flags that are defined across the whole baseline. The compiler
command flags must be defined, or a DLL is not being produced. Consult online for
autoconf and automake documentation; this will allow the tech area to understand how
this exists as a defacto industry standard based on tribal knowledge.

Technology areas define their mechanism in their deliverable to trigger if the
DLL is being activly built for their area (EXPORT), or if the API header files
are expected to be making use of the API (IMPORT). Notice how below in the code,
that [SONAME] API is defined and utilized in the header. Source files just carry
the regular function signature without all of the attributes or API decorations.
[SONAME]    Technology Area Specific API Collection Name (or DLL/Shared Object Name)
[module]    Technology Area Specific Source Module
USEAGE:

dll_global_def.h will be included into SCON file as part of CFLAG. There is no need to define it into the header file.
   tech_[module].h                     // [module] represents a template entry for tech area substitution
   ------------------------
HOWEVER, each tech_[module.h] is required to add macro into their public APIs.
DLL_API_GLOBAL  int [module]_function_signature(void*, void*, int)

Explanation of visibility usage.
default:
API or symbols can be seen and referenced either inside or outside of share object.
hidden:
None of API or symbols can be seen outside of share object.
   ------------------------

*/

#if defined(__ARMCC_VERSION) && defined(__APCS_FPIC)
#define DLL_API_GLOBAL              __attribute__((visibility("default")))    /**< ELF */
#define DLL_API_EXPORT_HELPER       __attribute__((visibility("default")))  /**< ELF */
#define DLL_API_IMPORT_HELPER       __attribute__((visibility("default")))    /**< ELF */
#define DLL_API_LOCAL               __attribute__((visibility("hidden")))     /**< ELF */
#elif defined(__clang__) && defined(__PIC__)
#define DLL_API_GLOBAL              __attribute__((visibility("default")))    /**< ELF */
#define DLL_API_EXPORT_HELPER       __attribute__((visibility("default")))  /**< ELF */
#define DLL_API_IMPORT_HELPER       __attribute__((visibility("default")))    /**< ELF */
#define DLL_API_LOCAL               __attribute__((visibility("hidden")))     /**< ELF */
#elif defined(__GNUC__) && defined(__PIC__)
#define DLL_API_GLOBAL              __attribute__((visibility("default")))    /**< ELF */
#define DLL_API_EXPORT_HELPER       __attribute__((visibility("default")))  /**< ELF */
#define DLL_API_IMPORT_HELPER       __attribute__((visibility("default")))    /**< ELF */
#define DLL_API_LOCAL               __attribute__((visibility("hidden")))     /**< ELF */
#elif defined(_WIN32) && defined(__PIC__)
#define DLL_API_GLOBAL              __declspec(dllimport)                     /**< PE/COFF */
#define DLL_API_EXPORT_HELPER       __declspec(dllexport)                     /**< PE/COFF */
#define DLL_API_IMPORT_HELPER       __declspec(dllimport)                     /**< PE/COFF */
#define DLL_API_LOCAL               /*NULL*/
#elif defined(__CYGWIN__) && defined(__PIC__)
#define DLL_API_GLOBAL              __declspec(dllimport)                     /**< PE/COFF */
#define DLL_API_EXPORT_HELPER       __declspec(dllexport)                     /**< PE/COFF */
#define DLL_API_IMPORT_HELPER       __declspec(dllimport)                     /**< PE/COFF */
#define DLL_API_LOCAL               /*NULL*/
#else
#define DLL_API_GLOBAL             
#define DLL_API_EXPORT_HELPER       /*NULL*/
#define DLL_API_IMPORT_HELPER       /*NULL*/
#define DLL_API_LOCAL               /*NULL*/
#endif

#if !defined(DLL_API_EXPORT)
#define DLL_API_EXPORT              DLL_API_IMPORT_HELPER                     /**< Generic Use for DLL_API_EXPORT_[SONAME], Not Recommended */
#endif

#endif
