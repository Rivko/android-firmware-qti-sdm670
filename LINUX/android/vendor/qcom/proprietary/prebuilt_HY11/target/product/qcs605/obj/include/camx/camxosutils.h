////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  camxosutils.h
/// @brief CamX OS specific utility implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CAMXOSUTILS_H
#define CAMXOSUTILS_H

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

#if defined (_LINUX)
#include <dirent.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#endif // _LINUX

#if defined (_WIN32)
#include <windows.h>
#endif // _WIN32

#include "camxdebug.h"
#include "camxdefs.h"
#include "camxformats.h"
#include "camxhal3types.h"
#include "camxtypes.h"

CAMX_NAMESPACE_BEGIN

static const INT InvalidNativeFence = -1;   ///< Invalid native hFence

/// Thread entry function type
typedef VOID* (*OSThreadFunc)(VOID* pArg);

/// Dynamic library handle
typedef VOID* OSLIBRARYHANDLE;

/// CAMX_TLS_DEFINE creates a variable in compiler managed TLS. Use to declare a variable in much the same way as a global
/// but each thread will receive its own instance. e.g.:
///
///         CAMX_TLS_DEFINE(UINT64, t_requestId, 1);
///
/// CAMX_TLS_DECLARE can be used to reference such a variable in another file
/// For static class variables, CAMX_TLS_STATIC_CLASS_DECLARE should be used
///
///         CAMX_TLS_STATIC_CLASS_DECLARE(UINT64, m_tRequestId);
///
/// And should be defined with CAMX_TLS_STATIC_CLASS_DEFINE
///
///         CAMX_TLS_STATIC_CLASS_DEFINE(UINT64, Node, m_tRequestId, 1);

#if (__clang__)
// NOWHINE GR022:   // (val) isn't a typecast
#define CAMX_TLS_STATIC_CLASS_DEFINE(type, classname, name, val) __thread type classname::name = (val)
#define CAMX_TLS_STATIC_CLASS_DECLARE(type, name) static __thread type name
// NOWHINE CF020: Simplified code
#elif (_MSC_VER)
// MSVC
#define CAMX_TLS_STATIC_CLASS_DEFINE(type, classname, name, val) __declspec(thread) type classname::name = (val)
#define CAMX_TLS_STATIC_CLASS_DECLARE(type, name) static __declspec(thread) type name
#else
#error unsupported compiler
#endif  // (__clang__) || (((__GNUC__) && (!__clang__)) && (PTHREAD_TLS)))

#if defined (_LINUX)
/// Thread handle type
typedef pthread_t OSThreadHandle;
/// Mutex handle type
typedef pthread_mutex_t OSMutexHandle;
/// Native fence type
typedef INT NativeFence;
/// File system path separator character
static const CHAR PathSeparator[]          = "/";
static const CHAR SharedLibraryExtension[] = "so";
/// The directory from which to read configuration files
#if defined (CAMX_ANDROID_API) && (CAMX_ANDROID_API >= 28) // NOWHINE PR002 <- Win32 definition
static const CHAR ConfigFileDirectory[]    = "/data/vendor/camera";
static const CHAR FileDumpPath[]           = "/data/vendor/camera";
#else
static const CHAR ConfigFileDirectory[]    = "/data/misc/camera";
static const CHAR FileDumpPath[]           = "/data/misc/camera";
#endif // Android-P or later

static const CHAR OverrideSettingsPath[]   = "/vendor/etc/camera";
static const CHAR pLEDCalibrationFilename[] = "LEDCalibrationData.bin";
static const CHAR pLEDTuningFilename[] = "LEDTuningData.csv";

#if defined(_LP64)
static const CHAR SensorModulesPath[]      = "/vendor/lib64/camera";
static const CHAR VendorLibPath[]          = "/vendor/lib64";
static const CHAR VendorPartitionPath[]    = "/vendor/lib64/camera";
static const CHAR CHIOverrideModulePath[]  = "/vendor/lib64/hw";
#else // defined(_LP64)
static const CHAR SensorModulesPath[]      = "/vendor/lib/camera";
static const CHAR VendorLibPath[]          = "/vendor/lib";
static const CHAR VendorPartitionPath[]    = "/vendor/lib/camera";
static const CHAR CHIOverrideModulePath[]  = "/vendor/lib/hw";
#endif // defined(_LP64)

#elif defined (_WIN32)
/// Thread handle type
typedef HANDLE OSThreadHandle;
/// Mutex handle type
typedef CRITICAL_SECTION OSMutexHandle;
/// Native fence type
typedef VOID* NativeFence;
/// File system path separator character
static const CHAR PathSeparator[]          = "\\";
static const CHAR SharedLibraryExtension[] = "dll";
/// The directory from which to read configuration files
static const CHAR ConfigFileDirectory[]    = ".";
static const CHAR VendorPartitionPath[]    = ".";
static const CHAR VendorLibPath[]          = ".";
static const CHAR SensorModulesPath[]      = ".";
static const CHAR CHIOverrideModulePath[]  = ".";
static const CHAR OverrideSettingsPath[] = ".";
static const CHAR pLEDCalibrationFilename[] = ".";
static const CHAR pLEDTuningFilename[] = ".";

#else
#error Unsupported target defined
#endif // defined(_LINUX) || defined(_WIN32)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// fileNameToken
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
enum class FileNameToken
{
    Com,       ///< com
    Vendor,    ///< vendor name
    Category,  ///< category name
    Module,    ///< module name
    Extension, ///< File Extension
    Max        ///< Max tokens for file name
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief General OS specific utility class implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NOWHINE CP017,CP018: All static class does not need copy/assignment overrides
class OsUtils
{
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Breakpoint Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// PerformSoftwareBreakpoint
    ///
    /// @brief  Generates a software breakpoint
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE VOID PerformSoftwareBreakpoint()
    {
#if defined (_X86_)
        __asm{ INT 3 };
#elif defined(_LINUX)
        raise(SIGTRAP);
#elif defined (_WIN32)
        // Use DebugBreak for _ARM_ and _AMD64_
        __debugbreak();
#else
        CAMX_STATIC_ASSERT_MESSAGE(FALSE, "Unsupported Target");
#endif // defined (_X86_)
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // File System Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(_LINUX)
    // NOWHINE GR016 : External name "struct stat64"
    typedef struct stat64 StatType;
#elif defined(_WIN32)
    // NOWHINE GR016 : External name "struct __stat64"
    typedef struct __stat64 StatType;
#else
    CAMX_STATIC_ASSERT_MESSAGE(FALSE, "Unsupported Target");
#endif // defined(_LINUX)

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetBinaryFileName
    ///
    /// @brief  Gets full filename, including fully qualified path, of binary files to be used with Autogen code
    ///
    /// @param  pFullName   Full filename, including fully qualified path
    /// @param  maxSize     Maximum size for the full file name
    /// @param  pFileName   Filename, without the fully qualified path
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE VOID GetBinaryFileName(
        CHAR*       pFullName,
        SIZE_T      maxSize,
        const CHAR* pFileName)
    {
        OsUtils::SNPrintF(pFullName, maxSize, "%s%s%s", VendorPartitionPath, PathSeparator, pFileName);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Stat
    ///
    /// @brief  Gets status information on a file
    ///
    /// @param  pPath    File to obtain status information from
    /// @param  pBuffer  Pointer to results status structure
    ///
    /// @return CamxResultSuccess on success, CamxResultEFailed otherwise
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CamxResult Stat(
        const CHAR* pPath,
        StatType*   pBuffer);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FOpen
    ///
    /// @brief  Opens a file on the filesystem
    ///
    /// @param  pFilename File to open
    /// @param  pMode     File access mode string
    ///
    /// @return File handle or NULL on failure
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static FILE* FOpen(
        const CHAR* pFilename,
        const CHAR* pMode);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FClose
    ///
    /// @brief  Closes the given file handle
    ///
    /// @param  pFile File handle to close
    ///
    /// @return CamxResultSuccess on success, CamxResultEFailed otherwise
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CamxResult FClose(
        FILE* pFile);

    static const INT SeekSet = SEEK_SET;    ///< Beginning of file origin for Fseek
    static const INT SeekEnd = SEEK_END;    ///< End of file origin for Fseek
    static const INT SeekCur = SEEK_CUR;    ///< Current position of the file pointer origin for Fseek

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FSeek
    ///
    /// @brief  Moves the file pointer to a specified location
    ///
    /// @param  pFile    File handle to seek
    /// @param  offset   Number of bytes from origin
    /// @param  origin   Initial position
    ///
    /// @return CamxResultSuccess on success, CamxResultEFailed otherwise
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CamxResult FSeek(
        FILE*   pFile,
        INT64   offset,
        INT     origin);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FTell
    ///
    /// @brief  Gets the current position of a file pointer
    ///
    /// @param  pFile File handle to tell
    ///
    /// @return Current file position or -1 on error
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static INT64 FTell(
        FILE* pFile);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FPrintF
    ///
    /// @brief Outputs a formatted string to the given file handle
    ///
    /// @param pFile    File handle to use for output
    /// @param format   Format string, printf style
    /// @param ...      Parameters required by format
    ///
    /// @return Number of characters written
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static INT FPrintF(
        FILE*       pFile,
        const CHAR* format,
                    ...);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FScanF
    ///
    /// @brief Reads a formatted string to the given file handle
    ///
    /// @param pFile    File handle to use for input
    /// @param format   Format string, scanf style
    /// @param ...      Parameters required by format
    ///
    /// @return Number of characters written
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static INT FScanF(
        FILE*       pFile,
        const CHAR* format,
        ...);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FGetS
    ///
    /// @brief  Reads characters from the specified stream and stores them as a C string into pDst until (numCharsToCopy-1)
    ///         characters have been read or either a newline or the end-of-file is reached, whichever happens first. A newline
    ///         character will cause the function to stop reading from the stream, but it is considered a valid character and
    ///         included in the string copied to pDst. A terminating null character is automatically appended after the
    ///         characters copied to pDst.
    ///
    /// @param  pDst            Destination string
    /// @param  numCharsToCopy  Number of characters to copy to pDst
    /// @param  pFile           File handle to read from
    ///
    /// @return If successful, pDst is returned. If 0 characters are read, NULL is returned.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE CHAR* FGetS(
        CHAR*   pDst,
        INT     numCharsToCopy,
        FILE*   pFile)
    {
        return fgets(pDst, numCharsToCopy, pFile);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FRead
    ///
    /// @brief  Reads data from a file handle
    ///
    /// @param  pDst     Destination memory
    /// @param  dstSize  Destination memory size
    /// @param  size     Size of objects to read
    /// @param  count    Number of objects to read
    /// @param  pFile    File handle to read from
    ///
    /// @return Number of objects read successfully
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static SIZE_T FRead(
        VOID*   pDst,
        SIZE_T  dstSize,
        SIZE_T  size,
        SIZE_T  count,
        FILE*   pFile);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FWrite
    ///
    /// @brief  Writes data to a file handle
    ///
    /// @param  pSrc     Source memory
    /// @param  size     Size of objects to write
    /// @param  count    Number of objects to write
    /// @param  pFile    File handle to write to
    ///
    /// @return Number of objects written successfully
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static SIZE_T FWrite(
        const VOID* pSrc,
        SIZE_T      size,
        SIZE_T      count,
        FILE*       pFile);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FFlush
    ///
    /// @brief  Flushes a file handle
    ///
    /// @param  pFile File handle to flush
    ///
    /// @return CamxResultSuccess on success, CamxResultEFailed otherwise
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CamxResult FFlush(
        FILE* pFile);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetFileName
    ///
    /// @brief  Extracts just the file name from a file's path plus name
    ///
    /// @param  pFilePath String to parse for the filename
    ///
    /// @return File name
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static const CHAR* GetFileName(
        const CHAR* pFilePath);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetFileNameToken
    ///
    /// @brief  Extract the token based on token count from Filename - com.<vendor>.<category>.<algorithm>.so
    ///
    /// @param  pFileName         Name of the file
    /// @param  tokenInputNum     get token based on tokenInputNum
    /// @param  pOutToken         Pointer to store the output token
    /// @param  maxTokenLen       Max token len.
    ///
    /// @return BOOL              TRUE or FALSE
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static BOOL GetFileNameToken(
        CHAR*  pFileName,
        UINT32 tokenInputNum,
        CHAR*  pOutToken,
        SIZE_T maxTokenLen);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetFilesFromPath
    ///
    /// @brief  Reads the directory and returns the binary file names
    ///
    /// @param  pFileSearchPath   Path to the directory
    /// @param  maxFileNameLength Maximum length of file name
    /// @param  pFileNames        Pointer to the beginning of memory to store an array of file names at the return of function
    /// @param  pVendorName       Vendor name
    /// @param  pCategoryName     Category of binary file
    /// @param  pModuleName       Name of the module
    /// @param  pExtension        Pointer to file extension
    ///
    /// @return Number of binary files in the directory
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static UINT16 GetFilesFromPath(
        const CHAR* pFileSearchPath,
        SIZE_T      maxFileNameLength,
        CHAR*       pFileNames,
        const CHAR* pVendorName,
        const CHAR* pCategoryName,
        const CHAR* pModuleName,
        const CHAR* pExtension);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetFileSize
    ///
    /// @brief  Gets the size, in bytes, for a given filename. Temporarily opens the file in the process
    ///
    /// @param  pFilename File to open
    ///
    /// @return File size, if file exists
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static UINT64 GetFileSize(
        const CHAR* pFilename);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Logging Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// LogSystem
    ///
    /// @param  level Verbosity level of the log
    /// @param  pText String to print
    ///
    /// @brief  Logs the provided string to a mechanism on the OS appropriate to the log level
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static VOID LogSystem(
        UINT32      level,
        const CHAR* pText);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Memory functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MallocAligned
    ///
    /// @brief  Allocates memory aligned to specified alignment. Alignment must be power of 2. Do not use this directly,
    ///         use CAMX_CALLOC_ALIGNED* instead.
    ///
    /// @param  size        size for allocation
    /// @param  alignment   Alignment of memory block
    ///
    /// @return Pointer to malloc'd memory with start address a multiple of alignment and as big as size.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static VOID* MallocAligned(
        SIZE_T size,
        SIZE_T alignment);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FreeAligned
    ///
    /// @brief  Frees memory that was allocated using MallocAligned. Do not use this directly, use CAMX_FREE_ALIGNED* instead.
    ///
    /// @param  pMem    A pointer to the memory to free
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static VOID FreeAligned(
        VOID* pMem);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// PageSize
    ///
    /// @brief  Gets the default allocation page size in bytes.
    ///
    /// @return The number of bytes in a memory page; -1 if page size could not be found.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static INT PageSize();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MemoryProtect
    ///
    /// @brief  Changes the access rights of the buffer provided.
    ///
    /// @note   pMem and size must be page aligned.
    ///
    /// @param  pMem            Memory block to change access restrictions
    /// @param  size            Size of memory block
    /// @param  accessRights    The access rights to change to
    ///
    /// @return CamxResultSuccess if successful.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CamxResult MemoryProtect(
        VOID*               pMem,
        SIZE_T              size,
        CamxProtectionType  accessRights);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MemMap
    ///
    /// @brief  Maps file or device into memory
    ///
    /// @param  bufferFD        file or device to be mapped
    /// @param  bufferLength    length of the mapping
    /// @param  offset          offset to be mapped in memory of the fd
    ///
    /// @return non-NULL pointer if successful
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static VOID* MemMap(
       INT      bufferFD,
       SIZE_T   bufferLength,
       SIZE_T   offset);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MemUnmap
    ///
    /// @brief  Unmaps the memory which was previous mapped
    ///
    /// @param  pAddr   Address or memory to be unmapped
    /// @param  size    length of the memory to be unmapped
    ///
    /// @return 0 if successful and -1 for failure
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static INT MemUnmap(
        VOID*   pAddr,
        SIZE_T  size);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // String Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// DPrintF
    ///
    /// @brief  Write formatted data from variable argument list to sized buffer
    ///
    /// @param  fd       File descriptor to print to
    /// @param  pFormat  Format string, printf style
    /// @param  ...      Parameters required by format
    ///
    /// @return Number of characters written
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static INT DPrintF(
        INT         fd,
        const CHAR* pFormat,
                    ...);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// SNPrintF
    ///
    /// @brief Write formatted data from variable argument list to sized buffer
    ///
    /// @param pDst     Destination buffer
    /// @param sizeDst  Size of the destination buffer
    /// @param pFormat  Format string, printf style
    /// @param ...      Parameters required by format
    ///
    /// @return Number of characters written
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE INT SNPrintF(
        CHAR*       pDst,
        SIZE_T      sizeDst,
        const CHAR* pFormat,
                    ...)
    {
        INT     numCharWritten;
        va_list args;

        va_start(args, pFormat);
        numCharWritten = VSNPrintF(pDst, sizeDst, pFormat, args);
        va_end(args);

        return numCharWritten;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// VSNPrintF
    ///
    /// @brief  Write formatted data from variable argument list to sized buffer
    ///
    /// @param  pDst     Destination buffer
    /// @param  sizeDst  Size of the destination buffer
    /// @param  pFormat  Format string, printf style
    /// @param  argptr   Parameters required by format
    ///
    /// @return Number of characters written
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static INT VSNPrintF(
        CHAR*       pDst,
        SIZE_T      sizeDst,
        const CHAR* pFormat,
        va_list     argptr);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StrStrip
    ///
    /// @brief  Strip all whitespace characters (space, tab, newline, carriage return, form feed, vertical tab) from a string.
    ///
    /// @param  pDst     The string to write to
    /// @param  pSrc     The string to read from
    /// @param  sizeDst  Size of the destination string
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE VOID StrStrip(
        CHAR* pDst,
        CHAR* pSrc,
        SIZE_T sizeDst)
    {
        if ((NULL != pSrc) && (NULL != pDst) && (sizeDst > 0))
        {
            CHAR* pTokenString;
            CHAR* pContext;
            pDst[0] = '\0';
            pTokenString = StrTokReentrant(pSrc, " \t\n\r\f\v", &pContext);
            while ((pTokenString != NULL) && (pTokenString[0] != '\0'))
            {
                StrLCat(pDst, pTokenString, sizeDst);
                pTokenString = StrTokReentrant(NULL, " \t\n\r\f\v", &pContext);
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StrCmp
    ///
    /// @brief  Compares two strings.
    ///
    /// @param  pString1 First string to compare
    /// @param  pString2 Second string to compare
    ///
    /// @return Returns a signed integer indicating the relationship between the input strings:
    ///         <0  the first character that does not match has a lower value in string1 than in string
    ///         0   the contents of both strings are equal
    ///         >0  the first character that does not match has a greater value in string1 than in string2
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE INT StrCmp(
        const CHAR* pString1,
        const CHAR* pString2)
    {
        return strcmp(pString1, pString2);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StrNICmp
    ///
    /// @brief  Compares two strings ignoring the case of the characters.
    ///
    /// @param  pString1 First string to compare
    /// @param  pString2 Second string to compare
    /// @param  maxCount Maximum number of characters to compare
    ///
    /// @return Returns a signed integer indicating the relationship between the input strings:
    ///         <0  the first character that does not match has a lower value in string1 than in string
    ///         0   the contents of both strings are equal
    ///         >0  the first character that does not match has a greater value in string1 than in string2
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static INT StrNICmp(
        const CHAR* pString1,
        const CHAR* pString2,
        SIZE_T      maxCount);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StrLCat
    ///
    /// @brief  Appends (sizeDst - StrLen(pDst) - 1) characters of pSrc to the end of pDst.
    ///
    /// @param  pDst     The string to write to
    /// @param  pSrc     The string to read from
    /// @param  sizeDst  Size of the destination string in bytes
    ///
    /// @return The total length of the new string.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static SIZE_T StrLCat(
        CHAR* pDst,
        const CHAR* pSrc,
        SIZE_T sizeDst);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StrLCpy
    ///
    /// @brief  Static function to copy a string with a maximum destination string length. Will always terminate. Source and
    ///         destination must not overlap
    ///
    /// @param  pDst     Destination string
    /// @param  pSrc     Source string
    /// @param  sizeDst  Size of destination buffer, in bytes
    ///
    /// @return Length of the source string
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static SIZE_T StrLCpy(
        CHAR*       pDst,
        const CHAR* pSrc,
        SIZE_T      sizeDst);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StrLen
    ///
    /// @brief  Static function to return the length of the null-terminated string.
    ///
    /// @param  pSrc The null-terminated string to determine the length of.
    ///
    /// @return Length of the null-terminated string
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE SIZE_T StrLen(
        const CHAR* pSrc)
    {
        return strlen(pSrc);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StrRChr
    ///
    /// @brief  Static function to find the last occurrence of a character within a string
    ///
    /// @param  pSrc        String to search
    /// @param  character   Character to search for
    ///
    /// @return Pointer to the character or NULL
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE const CHAR* StrRChr(
        const CHAR* pSrc,
        CHAR        character)
    {
        return strrchr(pSrc, static_cast<INT>(character));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StrStr
    ///
    /// @brief  Returns a pointer to the first occurrence of pString2 in pString1, or a NULL pointer if pString2 is not part of
    ///         pString1. The matching process stops at the first terminating null-characters and it is not considered in the
    ///         comparison.
    ///
    /// @param  pString1    The string to search
    /// @param  pString2    The string containing the sequence of characters to match
    ///
    /// @return A pointer to the first occurrence in pString1 of the entire sequence of characters specified in pString2, or a
    ///         NULL pointer if the sequence is not present in pString1.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE const CHAR* StrStr(
        const CHAR* pString1,
        const CHAR* pString2)
    {
        return strstr(pString1, pString2);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StrTokReentrant
    ///
    /// @brief  Parses a string into a sequence of tokens. On the first call, the string to be parsed is specified in pSrc and
    ///         the value of pContext should be valid, but is ignored. In each subsequent call that should parse the same
    ///         string, pSrc should be NULL and pContext should be unchanged since the previous call. This function is
    ///         reentrant and different strings may be parsed concurrently using sequences of calls to strtok_r() that specify
    ///         different saveptr arguments.
    ///
    /// @param  pSrc        The string to search
    /// @param  pDelimiter  Specifies a set of bytes that delimit the tokens in the parsed string. The caller may specify
    ///                     different delimiter strings in successive calls that parse the same string.
    /// @param  ppContext   A pointer to a CHAR* variable that is used internally in order to maintain context between
    ///                     successive calls that parse the same string.
    ///
    /// @return A pointer to a null-terminated string containing the next token, or NULL if there are no more tokens.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CHAR* StrTokReentrant(
        CHAR*       pSrc,
        const CHAR* pDelimiter,
        CHAR**      ppContext);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StrToL
    ///
    /// @brief  Parses the string pSrc, interpreting its content as a signed integral number of the specified base, which is
    ///         returned as an value of type signed long int.
    ///
    /// @param  pSrc  String containing the representation of a signed integral number.
    /// @param  ppEnd Reference to an object of type char*, whose value is set by the function to the next character in pSrc
    ///               after the numerical value. This parameter can also be NULL, in which case it is not used.
    /// @param  base  Numerical base (radix) that determines the valid characters and their interpretation.
    ///
    /// @return The converted signed integral number as an INT value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE INT StrToL(
        const CHAR* pSrc,
        CHAR**      ppEnd,
        INT         base)
    {
        return static_cast<INT>(strtol(pSrc, ppEnd, base));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StrToUL
    ///
    /// @brief  Parses the string pSrc, interpreting its content as an unsigned integral number of the specified base, which is
    ///         returned as a value of type unsigned long int.
    ///
    /// @param  pSrc  String containing the representation of an unsigned integral number.
    /// @param  ppEnd Reference to an object of type char*, whose value is set by the function to the next character in pSrc
    ///               after the numerical value. This parameter can also be NULL, in which case it is not used.
    /// @param  base  Numerical base (radix) that determines the valid characters and their interpretation.
    ///
    /// @return The converted unsigned integral number as a UINT value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE UINT StrToUL(
        const CHAR* pSrc,
        CHAR**      ppEnd,
        INT         base)
    {
        return static_cast<UINT>(strtoul(pSrc, ppEnd, base));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StrToD
    ///
    /// @brief  Parses the string pSrc, interpreting its content as a double precision floating point number of the specified
    ///         base, which is returned as a value of type unsigned long int.
    ///
    /// @param  pSrc  String containing the representation of a double precision floating point number.
    /// @param  ppEnd Reference to an object of type char*, whose value is set by the function to the next character in pSrc
    ///               after the numerical value. This parameter can also be NULL, in which case it is not used.
    ///
    /// @return The converted double precision floating point number as a DOUBLE value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE DOUBLE StrToD(
        const CHAR* pSrc,
        CHAR**      ppEnd)
    {
        return strtod(pSrc, ppEnd);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ToLower
    ///
    /// @brief  Converts the input character to its lowercase equivalent.
    ///
    /// @param  character The character to make lowercase.
    ///
    /// @return The converted character value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE CHAR ToLower(
        const CHAR character)
    {
        return static_cast<CHAR>(tolower(static_cast<INT>(character)));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StrError
    ///
    /// @brief  Converts the errno value into the corresponding string describing the error.
    ///
    /// @param  pErrorMsg    The string buffer to copy the error description string to
    /// @param  errorMsgSize The size in bytes of the string buffer to copy the error description string to
    /// @param  errorNum     The errno value to convert
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static VOID StrError(
        CHAR*   pErrorMsg,
        SIZE_T  errorMsgSize,
        INT     errorNum);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Thread Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetThreadID
    ///
    /// @brief  return the thread ID
    ///
    /// @return Numerical ID of the current thread
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static UINT GetThreadID();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetProcessID
    ///
    /// @brief  return the process ID
    ///
    /// @return Numerical ID of the current process
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static UINT GetProcessID();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ThreadCreate
    ///
    /// @brief  Create a OS thread
    ///
    /// @param  threadEntryFunction  Function to execute in new thread
    /// @param  pThreadData          Data passed to threadEntryFunction()
    /// @param  phThread             New thread handle or NULL on failure
    ///
    /// @return Success or EFailed
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CamxResult ThreadCreate(
        OSThreadFunc    threadEntryFunction,
        VOID*           pThreadData,
        OSThreadHandle* phThread);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ThreadSetName
    ///
    /// @brief  Sets the name of an OS thread
    ///
    /// @param  hThread             New thread handle or NULL on failure
    /// @param  pName               New name of the thread
    ///
    /// @return Success or EFailed
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CamxResult ThreadSetName(
        OSThreadHandle  hThread,
        const CHAR*     pName);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ThreadWait
    ///
    /// @brief  Wait for the given thread to terminate
    ///
    /// @param  hThread Thread to wait on
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static VOID ThreadWait(
        OSThreadHandle hThread);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// SleepMicroseconds
    ///
    /// @brief  Put the current thread to sleep
    ///
    /// @param  microseconds  Sleep duration in microsecond
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static VOID SleepMicroseconds(
        UINT microseconds);


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// System
    ///
    /// @brief  Invokes the command processor to execute a command
    ///
    /// @param  pCommand    Command string to execute
    ///
    /// @return status of execution or existence of command processor
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE INT System(
        const CHAR* pCommand)
    {
        return system(pCommand);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// LibMap
    ///
    /// @brief  Static function to obtain a handle to the named library (.dll, .so etc.)
    ///
    /// @param  pLibraryName  Name of library to map
    ///
    /// @return Handle to named module or NULL if failure
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static OSLIBRARYHANDLE LibMap(
        const CHAR* pLibraryName);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// LibUnmap
    ///
    /// @brief  Static function to unmap a library previously mapped by LibMap()
    ///
    /// @param  hLibrary  Handle to library from LibMap()
    ///
    /// @return CamxResultSuccess on success, CamxResultEFailed on failure
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CamxResult LibUnmap(
        OSLIBRARYHANDLE hLibrary);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// LibGetAddr
    ///
    /// @brief  Static function find an entry point in a library previously loaded by LibMap()
    ///
    /// @param  hLibrary  Handle to library from LibMap()
    /// @param  pProcName Name of entry point to find
    ///
    /// @return Pointer to named entry point in library, or NULL on failure
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static VOID* LibGetAddr(
        OSLIBRARYHANDLE hLibrary,
        const CHAR*     pProcName);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Native Fence Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// NativeFenceWait
    ///
    /// @brief  Static function to wait on a platform supplied native hFence
    ///
    /// @param  hFence              The native fence to wait on
    /// @param  timeoutMilliseconds Timeout in milliseconds
    ///
    /// @return CamxResultSuccess if hFence is successfully waited on, CamxResultEFailed otherwise
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CamxResult NativeFenceWait(
        NativeFence hFence,
        UINT32      timeoutMilliseconds);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetTime
    ///
    /// @brief  Gets the current time
    ///
    /// @param  pTime Output time structure
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static VOID GetTime(
        CamxTime* pTime);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetNanoSeconds
    ///
    /// @brief  Get the nanoseconds
    ///
    /// @return Nano seconds
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE UINT64 GetNanoSeconds()
    {
        CamxTime systemTime;
        GetTime(&systemTime);
        UINT64 nanoSeconds = (systemTime.seconds * NanoSecondsPerSecond) + systemTime.nanoSeconds;

        return nanoSeconds;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetDateTime
    ///
    /// @brief  Gets the date and time
    ///
    /// @param  pDateTime Output date and time structure
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static VOID GetDateTime(
        CamxDateTime* pDateTime);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetGMTime
    ///
    /// @brief  Gets UTC/GMT time from local
    ///
    /// @param  pTime     Pointer time structure
    /// @param  pResult   Pointer to result
    ///
    /// @return Return 0 on success or errno on error
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static INT GetGMTime(
        const time_t*   pTime,
        struct tm*      pResult);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// CamxTimeToMillis
    ///
    /// @brief  Translates CamxTime to time in milliseconds
    ///
    /// @param  pTime   Pointer to CamxTime structure
    ///
    /// @return Return milliseconds on success or 0 on error
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static UINT32 CamxTimeToMillis(
        CamxTime*   pTime)
    {
        return (pTime == NULL) ? 0 : ((1000 * pTime->seconds) + ((pTime->nanoSeconds) / 1000000L));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Close
    ///
    /// @brief  Closes the given file descriptor if it is valid one
    ///
    /// @param  FD Filedescriptor to close
    ///
    /// @return CamxResultSuccess on success, CamxResultEFailed otherwise
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CamxResult Close(
        INT FD);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// InitializeFormatAlignment
    ///
    /// @brief  Closes the given file descriptor if it is valid one
    ///
    /// @param  pImageFormat     Image format to initialize
    /// @param  pFormatParamInfo Format param info
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static VOID InitializeFormatAlignment(
        ImageFormat*       pImageFormat,
        FormatParamInfo*   pFormatParamInfo);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///  GetPropertyList
    ///
    ///  @brief   Static function to query a string value from an OS-specific variable (i.e.: Android properties)
    ///
    ///  @param   pFn    Callback function to be used by the android system call
    ///  @param   pData  Pointer to user data to pass along to the callback function
    ///
    ///  @return  None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static VOID GetPropertyList(
        VOID(*pFunc)(const CHAR* pName, const CHAR* pValue, VOID* pData), VOID* pData);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///  GetPropertyString
    ///
    ///  @brief   Static function to query OS specific system properties for type string (i.e.: Android properties)
    ///
    ///  @param   pName         Property name
    ///  @param   pValue        Out parameter for property value to get
    ///  @param   pDefaultValue Default value if property not set
    ///
    ///  @return  Length of the value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static INT GetPropertyString(
        const CHAR*  pName,
        CHAR*        pValue,
        const CHAR*  pDefaultValue);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///  GetPropertyBool
    ///
    ///  @brief   Static function to query OS specific system properties for type boolean (i.e.: Android properties)
    ///
    ///  @param   pName         Property name
    ///  @param   defaultValue  Default value if property not set
    ///
    ///  @return  1 if true, 0 if false
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static BOOL GetPropertyBool(
        const CHAR*  pName,
        BOOL         defaultValue);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///  GetPropertyINT64
    ///
    ///  @brief   Static function to query OS specific system properties for type INT64 (i.e.: Android properties)
    ///
    ///  @param   pName         Property name
    ///  @param   defaultValue  Default value if property not set
    ///
    ///  @return  Returns value for property truncated and coerced to INT64
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static INT64 GetPropertyINT64(
        const CHAR*  pName,
        INT64        defaultValue);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///  GetPropertyINT32
    ///
    ///  @brief   Static function to query OS specific system properties for type INT32 (i.e.: Android properties)
    ///
    ///  @param   pName         Property name
    ///  @param   defaultValue  Default value if property not set
    ///
    ///  @return  Returns value for property truncated and coerced to INT32
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static INT32 GetPropertyINT32(
        const CHAR*  pName,
        INT32        defaultValue);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// SetFDLimit
    ///
    /// @brief  Set the max opening file limit for this process.
    ///
    /// @param  maxFDNum  Max number of open files
    ///
    /// @return CamxResultSuccess if successful, failure otherwise
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CamxResult SetFDLimit(
        UINT32 maxFDNum);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// LoadPreBuiltLibrary
    ///
    /// @brief  Load library
    ///
    /// @param  pLibName    pointer to library  name
    /// @param  pFuncName   pointer to function name
    /// @param  phLibHandle pointer to Library handle to be output from this API
    ///
    /// @return CamxResultSuccess if successful
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static VOID* LoadPreBuiltLibrary(
        const CHAR*      pLibName,
        const CHAR*      pFuncName,
        OSLIBRARYHANDLE* phLibHandle);

private:
    OsUtils() = default;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief General portable mutex class implementation
///
/// Basic wrapping of OS mutex to provide abstraction
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Mutex final
{
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Create
    ///
    /// @brief  Static method to create an instance of Mutex
    ///
    /// @param  pResourceName Human readable name of the resource being protected.
    ///
    /// @return Pointer to newly created mutex object on success, NULL on failure.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static Mutex* Create(
        const CHAR* pResourceName);

#if CAMX_USE_MEMSPY
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// CreateNoSpy
    ///
    /// @brief  Static method to create an instance of Mutex without tracking the internal allocations using MemSpy
    ///
    /// @param  pResourceName Human readable name of the resource being protected
    ///
    /// @return Pointer to newly created untracked mutex object on success, NULL on failure.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static Mutex* CreateNoSpy(
        const CHAR* pResourceName);
#endif // CAMX_USE_MEMSPY

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Destroy
    ///
    /// @brief  Method to delete an instance of Mutex
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID Destroy();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Lock
    ///
    /// @brief  Lock/acquire an Mutex
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID Lock();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// TryLock
    ///
    /// @brief  Try to acquire a Mutex lock
    ///
    /// @return CamxResultSuccess on success, CamxResultEBusy if lock already taken and CamxResultEFailed on all other errors.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult TryLock();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Unlock
    ///
    /// @brief  Unlock/release an Mutex
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID Unlock();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetNativeHandle
    ///
    /// @brief  Get the native mutex handle, needed for other system calls which takes native handle
    ///
    /// @return A handle to the native mutex handle
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    OSMutexHandle* GetNativeHandle();

    static const SIZE_T MaxResourceNameSize = 128;              ///< Max length of resource name

private:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Mutex
    ///
    /// @brief  Default constructor for Mutex object.
    ///
    /// @return A pointer to the new Mutex object
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Mutex() = default;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ~Mutex
    ///
    /// @brief  Default destructor for Mutex object.
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ~Mutex() = default;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Initialize
    ///
    /// @brief  Initialize a newly created Mutex object
    ///
    /// @param  pResourceName Human readable name of the resource being protected
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult Initialize(
        const CHAR* pResourceName);

    Mutex(const Mutex&) = delete;                ///< Disallow the copy constructor
    Mutex& operator=(const Mutex&) = delete;     ///< Disallow assignment operator

#if defined(_LINUX)
    pthread_mutex_t     m_mutex;                ///< (Linux) Mutex
    BOOL                m_validMutex;           ///< (Linux) Indicates if underlying mutex is valid
#else
    CRITICAL_SECTION    m_criticalSection;      ///< (Win32) Critical Section implementing Mutex
#endif // defined(_LINUX)

    CHAR                m_pResourceName[Mutex::MaxResourceNameSize];   ///< Name of resource protected
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief General portable readwrite lock class implementation
///
/// Basic wrapping of OS ReadWriteLock to provide abstraction
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ReadWriteLock final
{
public:

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Create
    ///
    /// @brief  Static method to create an instance of ReadWriteLock
    ///
    /// @param  pResourceName Human readable name of the resource being protected
    ///
    /// @return Pointer to newly created object on success, NULL on failure.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static ReadWriteLock* Create(
        const CHAR* pResourceName);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Destroy
    ///
    /// @brief  Method to delete an instance of ReadWriteLock
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID Destroy();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ReadLock
    ///
    /// @brief  Acquire the lock for reading. Multiple threads may concurrently enter the lock for read
    ///         operations. This method will block if a thread has acquired the lock for writing
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID ReadLock();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// WriteLock
    ///
    /// @brief  Acquire the lock for writing
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID WriteLock();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// TryReadLock
    ///
    /// @brief  Attempt to acquire the lock for reading. Multiple threads may concurrently enter the lock for
    ///         read operations. This method will return an error if a thread has acquired the lock for writing.
    ///
    /// @return TRUE if the lock is acquired, FALSE otherwise
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    BOOL TryReadLock();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// TryWriteLock
    ///
    /// @brief  Attempt to acquire the lock for writing. This method will return an error if the lock has been
    ///         acquired by another thread.
    ///
    /// @return TRUE if the lock is acquired, FALSE otherwise
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    BOOL TryWriteLock();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Unlock
    ///
    /// @brief  Unlock/release an Lock
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID Unlock();

    static const SIZE_T MaxResourceNameSize = 128;              ///< Max length of resource name

private:
    /// @brief Constructor for ReadWriteLock object.
    ReadWriteLock() = default;

    /// @brief Destructor for ReadWriteLock object.
    ~ReadWriteLock() = default;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Initialize
    ///
    /// @brief  Initialize a newly created ReadWriteLock object
    ///
    /// @param  pResourceName Human readable name of the resource being protected
    ///
    /// @return n/a
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult Initialize(
        const CHAR* pResourceName);

    ReadWriteLock(const ReadWriteLock&) = delete;                ///< Disallow the copy constructor
    ReadWriteLock& operator=(const ReadWriteLock&) = delete;     ///< Disallow assignment operator

#if defined(_LINUX)
    pthread_rwlock_t    m_readWriteLock;        ///< (Linux) Readwrite lock
    BOOL                m_validReadWriteLock;   ///< (Linux) Indicates if underlying Readwrite is valid
#else
    SRWLOCK             m_slimReadWriteLock;    ///< (Windows) SlimReadwrite(srw) lock
    BOOL                m_isWriteAcquired;      ///< (Windows) TRUE if it has been acquired for write
#endif // defined(_LINUX)

    CHAR                m_pResourceName[Mutex::MaxResourceNameSize];   ///< Name of resource protected
    INT                 m_lockCount;            ///< Number of locks currently held (used for debug tracing)
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief General portable semaphore class implementation
///
/// Basic wrapping of OS semaphore to provide abstraction
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Semaphore final
{
public:

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Create
    ///
    /// @brief  Static method to create an instance of Semaphore
    ///
    /// @return Pointer to newly created object on success, NULL on failure.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static Semaphore* Create();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Destroy
    ///
    /// @brief  Method to delete an instance of Semaphore
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID Destroy();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Wait
    ///
    /// @brief  Decrement/wait indefinitely on Semaphore (equivalent to sem P)
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID Wait();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// TimedWait
    ///
    /// @brief  Wait with timeout on the Semaphore
    ///
    /// @param  timeoutMilliseconds Timeout in milliseconds
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult TimedWait(
        UINT timeoutMilliseconds);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Signal
    ///
    /// @brief  Increment/signal a Semaphore (equivalent to sem V)
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID Signal();

private:

    /// @brief Constructor for Semaphore object.
    Semaphore() = default;

    /// @brief Destructor for Semaphore object.
    ~Semaphore() = default;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Initialize
    ///
    /// @brief  Initialize a newly created Semaphore object
    ///
    /// @return Success if successful, a failure code otherwise
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult Initialize();

    Semaphore(const Semaphore&) = delete;                ///< Disallow the copy constructor
    Semaphore& operator=(const Semaphore&) = delete;     ///< Disallow assignment operator

#if defined(_LINUX)
    sem_t        m_semaphore;        ///< (Linux) Semaphore
    BOOL         m_validSemaphore;   ///< (Linux) Indicates if underlying semaphore is valid
#else
    HANDLE       m_hSemaphore;       ///< (Windows) Semaphore
#endif // defined(_LINUX)
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief General portable condition class implementation
///
/// Basic wrapping of OS condition variable to provide abstraction
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Condition final
{
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Create
    ///
    /// @brief  Static method to create an instance of Condition
    ///
    /// @param  pResource pointer to description of resource being conditioned
    ///
    /// @return Pointer to newly created object on success, NULL on failure.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static Condition* Create(
        const CHAR* pResource);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Destroy
    ///
    /// @brief  Method to delete an instance of Semaphore
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID Destroy();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Wait
    ///
    /// @brief  Wait on the condition
    ///
    /// @param  phMutex OS mutex handle, caller is responsible for using the same handle as used to lock around the wait
    ///
    /// @return Success if successful, a failure code otherwise
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult Wait(
        OSMutexHandle* phMutex);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// TimedWait
    ///
    /// @brief  Wait with timeout on the Condition
    ///
    /// @param  phMutex                 OS mutex handle, caller is responsible for using the same handle as used to lock
    ///                                 around the wait
    /// @param  timeoutMilliseconds     Timeout in milliseconds
    ///
    /// @return Success if successful, a failure code otherwise
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult TimedWait(
        OSMutexHandle*  phMutex,
        UINT            timeoutMilliseconds);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Signal
    ///
    /// @brief  Signal the condition
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID Signal();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Broadcast
    ///
    /// @brief  Broadcast the condition
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID Broadcast();

private:
    Condition() = default;
    ~Condition() = default;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Initialize
    ///
    /// @brief  Initialize a newly created Condition object
    ///
    /// @param  pResource pointer to description of resource being conditioned
    ///
    /// @return A result code
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult Initialize(
        const CHAR* pResource);

    Condition(const Condition&) = delete;                ///< Disallow the copy constructor
    Condition& operator=(const Condition&) = delete;     ///< Disallow assignment operator

    const CHAR*         m_pResource;                     ///< Name of the resource being conditioned
#if defined(_LINUX)
    pthread_cond_t      m_conditionVar;                  ///< (Linux) Underlying conditional variable
    BOOL                m_validConditionVar;             ///< (Linux) Indicates if condVar is valid
#else
    CONDITION_VARIABLE  m_conditionVar;                  ///< (Windows) Underlying conditional variable
#endif // defined(_LINUX)
};


CAMX_NAMESPACE_END

#endif // CAMXOSUTILS_H
