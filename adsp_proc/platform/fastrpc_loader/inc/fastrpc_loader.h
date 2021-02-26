#ifndef FASTRPC_LOADER_H
#define FASTRPC_LOADER_H

#include "qurt.h"
#include "qurt_process.h"

struct smq_phy_page; 

typedef struct {
   const char*    pszName;
   unsigned       bFixed:1;
   unsigned int   dwPhysicalSize;
   unsigned int   dwPhysicalOffset;
   unsigned int   dwDataSize;
   unsigned char* pFileData;
} AEEConstFile;

#ifndef QURT_PROCESS_NON_SYSTEM_CRITICAL
#define QURT_PROCESS_NON_SYSTEM_CRITICAL        (1u << 1)     /* Bit 1 (0x00000002) */
#endif

/**
 * spawn a new process
 * prorcess_init, this gets called when a asid and client ahndle are known, but before we actually spawn
 */
int fastrpc_spawn(int pidA, const char* appsname, const char* elffile, int elfsize, const struct smq_phy_page* pages, int numPages, int attrs);
//! return 0 if the sid is in one of the loaded pages
int fastrpc_loader_check_load_sid(int asid, int sid);
int fastrpc_kill(int pidA);
int fastrpc_get_process_attrs(unsigned int);
int fastrpc_get_process_name(unsigned int,char*,unsigned int*elfSize);

#endif // FASTRPC_LOADER_H
