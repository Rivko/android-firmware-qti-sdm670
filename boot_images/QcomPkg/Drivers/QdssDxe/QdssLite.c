/*=============================================================================

FILE:         QdssLite.c

DESCRIPTION:  

================================================================================
            Copyright (c) 2015 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/


#include <Library/BaseLib.h>
#include <Library/UefiLib.h>         //AsciiPrint

#include <Library/DebugLib.h>
#include <Library/QcomLib.h>
#include <Library/ParserLib.h>
#include <Library/EfiFileLib.h>    //EmbeddedPkg; EfiOpen, EfiClose, EfiReadAllocPool

#include <Library/npa.h>

#include "QdssLite.h"
#include "QdssStmConfig.h"
#include "QdssTFunnel.h"
#include "QdssEtfEtb.h"
#include "tracer_event_ids.h"


#define QDSS_SW_EVENT_CONFIG_FILE_FV    "FV1:tracer_event.cfg"
#define QDSS_SW_EVENT_CONFIG_FILE_PATH  L"tracer_event.cfg"

enum
{
  QDSS_CLOCK_LEVEL_OFF,      /* clock set to CXO, and off */
  QDSS_CLOCK_LEVEL_DEBUG,    /* clock rate follows VDD_CX */
  QDSS_CLOCK_LEVEL_HSDEBUG,  /* clock set to high speed */
};

enum
{
  QDSS_UNINITIALIZED = 0,
  QDSS_BASE_INIT,
  QDSS_TRACE_ENABLED,
};

// Size of swevent_filter for a 32-bit mask.
#define QDSS_SWEVENT_FILTER_SIZE  ((TRACER_EVENT_ID_MAX >> 5) + \
                                   ((TRACER_EVENT_ID_MAX & 0x1F) ? 1 : 0))

int QdssSwEventCount;   // Track SW event to apply default configuration.

struct qdss_info
{
  uint8 init_level;         // QDSS initialized, Trace enabled
  uint8 num_ports_in_use;
  STMPortAddrType port;
  STMPortAddrType port2;
  npa_client_handle npaClientHandle;
  uint32 swevent_filter[QDSS_SWEVENT_FILTER_SIZE]; // SW event enable bitmask
};

static struct qdss_info qdss_info; //zero initialized

/*============================================================================
 * @brief QDSS Lite API function. See QDSSLite.h for details
 *==========================================================================*/

/**
QdssParseSwEventConfig_Cb()

Only enables those SW events indicated in the config file.
Dependency: Assumes all SW events are already disabled.
*/
VOID QdssParseSwEventConfig_Cb(UINT8* TokenPtrArray[], UINTN MaxTokens)
{
  UINTN FlagVal;

  // Must have at least three tokens.
  if (3 > MaxTokens)
  {
    return;
  }

  QdssSwEventCount++;
  FlagVal = AsciiStrDecimalToUintn ((CHAR8 *)(TokenPtrArray[2]));
  if (1 == FlagVal)
  {
    QdssEnableSwEvent(QdssSwEventCount);
  }

  return;
}

//Uncomment the below line for stand alone testing.
//QDSS clocks have to be enabled externally instead when doing this.

//#define QDSS_STANDALONE 1

#ifdef QDSS_STANDALONE
int QdssClocksEnable(void)
{
   return 0;
}

#else 
int QdssClocksEnable(void)
{
  // Client register with QDSS clocks.
  if (NULL == qdss_info.npaClientHandle) {
    qdss_info.npaClientHandle = npa_create_sync_client(
                                "/clk/qdss",  // Resource connecting to
                                "qdss",        // Client name
                                NPA_CLIENT_REQUIRED); //Must honor request
  }
  if (NULL == qdss_info.npaClientHandle){
    DEBUG((EFI_D_ERROR, "QDSSClocksEnable npa_create_sync_client /clk/qdss failed!\n"));
    return 1;
  }
  npa_issue_required_request(qdss_info.npaClientHandle,
                             QDSS_CLOCK_LEVEL_DEBUG);
  return 0;
}
#endif

/*============================================================================
 * @brief QDSS Lite API function. See QDSSLite.h for details
 *==========================================================================*/
int QdssAllocPort(STMPortAddrType *pPort)
{
  int retval;

  if (qdss_info.init_level < QDSS_BASE_INIT) {
    return DAL_ERROR;
  }

  retval = QdssStmTrace_NewPort(pPort);
  if (DAL_SUCCESS == retval) {
    qdss_info.num_ports_in_use++;
  }
  return retval;
}

/*============================================================================
 * @brief QDSS Lite API function. See QDSSLite.h for details
 *==========================================================================*/
int QdssFreePort(STMPortAddrType port)
{

  if (qdss_info.init_level < QDSS_BASE_INIT) {
    return DAL_ERROR;
  }

  QdssStmTrace_FreePort(port);
  qdss_info.num_ports_in_use--;

  return DAL_SUCCESS;
}

/*============================================================================
 * @brief QDSS Lite API function. See QDSSLite.h for details
 *==========================================================================*/
int QdssEnableTrace(void)
{
  if (qdss_info.init_level < QDSS_BASE_INIT) {
    return DAL_ERROR;
  }

  qdss_info.init_level = QDSS_TRACE_ENABLED;
  return DAL_SUCCESS;
}

/*============================================================================
 * @brief QDSS Lite API function. See QDSSLite.h for details
 *==========================================================================*/
int QdssDisableTrace(void)
{
  if (qdss_info.init_level < QDSS_BASE_INIT) {
    return DAL_ERROR;
  }

  qdss_info.init_level = QDSS_BASE_INIT;

  return DAL_SUCCESS;
}

// Enable SW event(s).
int QdssEnableSwEvent(uint32 event_id)
{
  int i;
  if (TRACER_EVENT_ALL == event_id)
  {
    for (i = 0; i < QDSS_SWEVENT_FILTER_SIZE; i++)
    {
      qdss_info.swevent_filter[i] = 0xFFFFFFFF;
    }
  }
  else
  {
    if (TRACER_EVENT_ID_MAX > event_id)
    {
      qdss_info.swevent_filter[event_id >> 5] |= (1 << (event_id & 0x1F));
    }
    else
    {
      return 1;
    }
  }
  return 0;
}

// Disable SW event(s).
int QdssDisableSwEvent(uint32 event_id)
{
  int i;
  if (TRACER_EVENT_ALL == event_id)
  {
    for (i = 0; i < QDSS_SWEVENT_FILTER_SIZE; i++)
    {
      qdss_info.swevent_filter[i] = 0;
    }
  }
  else
  {
    if (TRACER_EVENT_ID_MAX > event_id)
    {
      qdss_info.swevent_filter[event_id >> 5] &= ~(1 << (event_id & 0x1F));
    }
    else
    {
      return 1;
    }
  }
  return 0;
}

EFI_STATUS
LoadFromFV (IN   CHAR8*    Name,
            OUT  UINT8**   FileBuffer,
            OUT  UINTN*    FileSize)
{
  EFI_STATUS                  Status;
  EFI_OPEN_FILE               *File;

  if (Name == NULL || FileBuffer == NULL || FileSize == NULL)
    return EFI_INVALID_PARAMETER;

  File = EfiOpen (Name, EFI_FILE_MODE_READ, EFI_SECTION_RAW);
  if (File == NULL)
    return EFI_LOAD_ERROR;

  Status = EfiReadAllocatePool (File, (VOID**)FileBuffer, FileSize);

  EfiClose (File);

  return Status;
}

/**
 * Initialize SW events.
 *
 * This function initializes Qdss for tracing SW events.
 *
 * @return  0 on success,
 *          1 indicating not initialized.
 */
int QdssSwEventInit(void)
{
  EFI_STATUS Status;
  UINT8      *CfgBuffer = NULL;
  UINTN      FileSize = 0;
  INTN       Pd;              // Parser descriptor.
  UINT32     BlkIOAttrib = 0;

  BlkIOAttrib = BLK_IO_SEL_PARTITIONED_MBR;
  BlkIOAttrib |= BLK_IO_SEL_MEDIA_TYPE_REMOVABLE;
  BlkIOAttrib |= BLK_IO_SEL_SELECT_MOUNTED_FILESYSTEM;

  Status = LoadFileFromPartition (BlkIOAttrib,
                                  QDSS_SW_EVENT_CONFIG_FILE_PATH,
                                  (EFI_GUID*)&BlkIOAttrib,
                                  (EFI_GUID*)&BlkIOAttrib,
                                  NULL,
                                  &CfgBuffer, &FileSize);

  if (Status != EFI_SUCCESS)
  {
    Status = LoadFromFV (QDSS_SW_EVENT_CONFIG_FILE_FV, &CfgBuffer, &FileSize);
  }
  if (EFI_ERROR(Status) || (CfgBuffer == NULL) || (FileSize == 0) )
  {
    AsciiPrint("QdssEventInit: Load of config file failed.\r\n");
    return -1;
  }

  Pd = OpenParser (CfgBuffer, FileSize, NULL);
  if (Pd < 0)
  {
    AsciiPrint ("QdssEventInit: OpenParser failed\r\n");
    return -1;
  }

  QdssSwEventCount = 0;
  QdssDisableSwEvent(TRACER_EVENT_ALL);
  EnumCommaDelimItems (Pd, QdssParseSwEventConfig_Cb);

  CloseParser(Pd);

  return 0;
}

/*============================================================================
 * @brief QDSS Lite API function. See QDSSLite.h for details
 *==========================================================================*/
int QdssInit()
{
  if (QDSS_UNINITIALIZED != qdss_info.init_level) {
    return 0;  // Already initialized.
  }

  //SetMem(&qdss_info, sizeof(struct qdss_info), 0x00);

  // Setup target-specific references.
  if (0 != QdssClocksEnable()) {  // Enable the QDSS clocks.
    return 1;                     // Failed.
  }

  QdssEtfEtb_PreInit();
  QdssTFunnel_PreInit();
  QdssStmConfig_PreInit(300000000);  // with counter freq

  qdss_info.init_level = QDSS_BASE_INIT;

  // Configure and initialize HW
  QdssEtfEtb_Init();
  QdssTFunnel_Init();
  QdssStmConfig_Init();

  if (DAL_SUCCESS != QdssAllocPort(&qdss_info.port)) {
    return 1;
  }
  if (DAL_SUCCESS != QdssAllocPort(&qdss_info.port2)) {
    return 1;
  }

  QdssSwEventInit();
  QdssEnableTrace();  // Enable tracing capability.


  return 0;
}

/*============================================================================
 * @brief QDSS Lite API function. See QDSSLite.h for details
 *==========================================================================*/

#define OST_TOKEN_SWEVENT      0x0080
#define PACK_EVENTID(_id) ((((uint32)(_id)) << 8 ) | OST_TOKEN_SWEVENT)

boolean qdss_is_sw_event_enabled(UINT32 event_id)
{
    if ( (event_id < TRACER_EVENT_ID_MAX) &&
         (0 < (qdss_info.swevent_filter[event_id >> 5] &
               (1 << (event_id & 0x1F))
              ))
       )
    {
      return TRUE;
    }
    else
    {
      return FALSE;
    }
}

int QdssTraceSwEvent(UINT32 event_id)
{
  if (qdss_info.init_level < QDSS_TRACE_ENABLED) {
    return DAL_ERROR;
  }
  if (qdss_is_sw_event_enabled(event_id))
  {
      QdssStmTrace_Data32(qdss_info.port,
                          (TRACE_DATA_MARKED | TRACE_DATA_TIMESTAMPED),
                          PACK_EVENTID(event_id));
  }
  return DAL_SUCCESS;
}

/*============================================================================
 *
 *==========================================================================*/
static __inline uint32 endian_swap32(uint32 x)
{
  return (x >> 24) |
           ((x << 8) & 0x00FF0000) |
           ((x >> 8) & 0x0000FF00) |
           (x << 24);
}

static __inline int ost_start_packet(STMPortAddrType port, uint32 ost_header)
{
  QdssStmTrace_Data32(port,
                      TRACE_DATA_MARKED,
                      endian_swap32(ost_header));
  return DAL_SUCCESS;
}

static __inline int ost_end_packet(STMPortAddrType port)
{

  QdssStmTrace_Flag(port,
                    TRACE_DATA_TIMESTAMPED);

  return DAL_SUCCESS;
}


static __inline int ost_write_data(STMPortAddrType port, byte *pCurrent, int nRemaining)
{
  //Make pCurrent 32 bit aligned first
  while (((uint32)pCurrent & 0x3) && (nRemaining > 0)) {
    QdssStmTrace_Data8(port,
                       TRACE_DATA_NONETYPE,
                       *pCurrent);
    pCurrent++;
    nRemaining--;
  }


#define OST_WRITE_DATA_WORDS(_nBytes,_nBits)                            \
   while (nRemaining >= _nBytes) {                                      \
      QdssStmTrace_Data##_nBits(port,                                       \
                                TRACE_DATA_NONETYPE,                        \
                                (*(uint##_nBits *)pCurrent)); \
      pCurrent += _nBytes;                                              \
      nRemaining -=_nBytes;                                             \
   }

  OST_WRITE_DATA_WORDS(4, 32);
  OST_WRITE_DATA_WORDS(2, 16);
  OST_WRITE_DATA_WORDS(1, 8);

  return DAL_SUCCESS;
}


static int WriteOstPacket(STMPortAddrType port, uint32 ost_header, byte *packet, int nLen)
{
  if (nLen < 1) {
    return -1;
  }

  ost_start_packet(port, ost_header);
  ost_write_data(port, packet, nLen);
  ost_end_packet(port);
  return DAL_SUCCESS;
}

/*============================================================================
 * @brief QDSS Lite API function. See QDSSLite.h for details
 *==========================================================================*/
int QdssWriteOstPacket(STMPortAddrType port, uint32 ost_header, byte *packet, int nLen)
{

  if (qdss_info.init_level < QDSS_TRACE_ENABLED) {
    return DAL_ERROR;
  }
  return WriteOstPacket(port, ost_header, packet, nLen);
}

#define QDSS_SWEVT_ENTITY    2
#define QDSS_SWEVT_PROTOCOL  1
#define QDSS_SWEVT_MAX_ARGS  10

int QdssTraceSwEventData(UINT32 event_id, UINT32 n, VA_LIST args)
{
  UINT32 message[QDSS_SWEVT_MAX_ARGS + 1];
    // largest message is the event_id plus QDSS_SWEVT_MAX_ARGS.
  int i = 0;
  int nLen = 0;

  if (!qdss_is_sw_event_enabled(event_id))
  {
    return DAL_SUCCESS;
  }
  // Argument limit
  n = (QDSS_SWEVT_MAX_ARGS < n) ? QDSS_SWEVT_MAX_ARGS : n;

  message[0] = event_id;
  for (i = 1; i < (n + 1); i++) {
    message[i] = VA_ARG(args, UINT32);
  }

  nLen = (n + 1) * sizeof(uint32);

  return QdssWriteOstPacket(
      qdss_info.port2,
      QdssOstHeader(QDSS_SWEVT_ENTITY, QDSS_SWEVT_PROTOCOL),
      (byte *)message,
      nLen);
}


