#ifndef DALDmov_H
#define DALDmov_H

/*==============================================================================

 FILE:         DALDmov.h

DESCRIPTION: Data Mover function and data structure declarations.

===============================================================================

                             Edit History


when       who     what, where, why
--------   ---     ------------------------------------------------------------ 
08/12/10   py      Add support for using the same channel in different modes in parallel 
04/12/10   an      legacy structures cleanup
03/08/10   an      reuse cmd ptr list pool for legacy and async xfers
03/05/10   MK      Added support for another DMOV Device 
02/19/10   an      split shared data structures among devices to avoid race condition
10/06/09   an      added DmovDevXferNotify
09/24/09   an      added flush type field to Dmov_ChanType structure
05/29/09   an      moved DMOV_LEGACY_SUPPORT into compilation files
04/29/09   an      fix compilation warnings 
03/27/09   MK      Added clock management 
01/18/08   ah      Created

===============================================================================
                   Copyright (c) 2008-2011 Qualcomm Technologies Incorporated.
                          All Rights Reserved.
                        QUALCOMM Proprietary/GTDR
==============================================================================*/

/* ----------------------------------------------------------------------------
** Includes
** ------------------------------------------------------------------------- */

#include "DALFramework.h"
#include "DDIDmov.h"
#include "DALQueue.h"
#include "DDIInterruptController.h"

#ifdef DMOV_USE_DAL_CLOCK_DRIVER
#include "DDIClock.h"
#endif
#include "HALdmov.h"

/*-----------------------------------------------------------------------------
** Macros
** ---------------------------------------------------------------------------*/
#ifdef DMOV_STRIP_MSG
#define DMOV_LOGEVENT(...) 
#else
#define DMOV_LOGEVENT(...) DALSYS_LogEvent(__VA_ARGS__)
#endif
/*-----------------------------------------------------------------------------
** Constants
**---------------------------------------------------------------------------*/

/* DMOV dynamic memory allocation */
#define DMOV_CMD_LIST_PTR_SIZE          8  /* 8 bytes, should be 8-byte aligned */
#define DMOV_AVG_NUM_CLP_PER_CHAN       4 
#define DMOV_MAX_NUM_CMDS_PER_LIST      10  /* Any client who needs more than 10 
                                             * cmds per list should pre-allocate */     

#define DMOV_MAX_NUM_QUEUES             1   /* Number of SW command queues per channel */
#define DMOV_MAX_CMD_SIZE               24  /* 24 bytes => 6 uint32 */

/* Token: bit0~15 for transfer id, bit16~30 for transfer channel number, bit31 for transfer mode */
#define DMOV_TRANSFER_MODE_SHFT         31
#define DMOV_TRANSFER_MODE_BMSK         ((uint32)0x1 << DMOV_TRANSFER_MODE_SHFT)
#define DMOV_TRANSFER_ID_SHFT           0
#define DMOV_TRANSFER_ID_BMSK           ((uint32)0xFFFF << DMOV_TRANSFER_ID_SHFT)
#define DMOV_TRANSFER_CHAN_SHFT         16
#define DMOV_TRANSFER_CHAN_BMSK         ((uint32)0x7FFF << DMOV_TRANSFER_CHAN_SHFT)
#define DMOV_GET_TRANSFER_ID(token)     ((token) & DMOV_TRANSFER_ID_BMSK)
#define DMOV_GET_TRANSFER_CHAN(token)   (((token) & DMOV_TRANSFER_CHAN_BMSK) >> DMOV_TRANSFER_CHAN_SHFT)
#define DMOV_GET_TRANSFER_MODE(token)   (((token) & DMOV_TRANSFER_MODE_BMSK) >> DMOV_TRANSFER_MODE_SHFT)

/* DMOV_NUM_CHANS * (avg per channel) */
#define DMOV_MAX_NUM_TRANSFERS_QUEUED   \
            (DMOV_MAX_NUM_CHANNELS_PER_DEV * DMOV_AVG_NUM_CLP_PER_CHAN)		

#define DMOV_MAX_NUM_CHANNELS_PER_DEV   16
#define DMOV_MAX_NUM_CLIENT_INTERFACES  4

#define DMOV_CMD_ADDR_MODE_SHFT         0
#define DMOV_CMD_ADDR_MODE_BMSK         (7 << DMOV_CMD_ADDR_MODE_SHFT)

/* Define the clock strings that is used to identify ADM clocks in the DAL clock driver */
#define ADM0_CLK_STRING       "cc_adm0_aclk"
#define ADM0_PBUS_CLK_STRING  "cc_adm0_pbus_hclk"

#define ADM1_CLK_STRING       "cc_adm1_aclk"
#define ADM1_PBUS_CLK_STRING  "cc_adm1_pbus_hclk"

/*-----------------------------------------------------------------------------
** Types
**---------------------------------------------------------------------------*/

/* Channel state */
typedef enum Dmov_ChanState Dmov_ChanState;
enum Dmov_ChanState
{
  /* The following are inactive states */
  DMOV_CHAN_STATE_INVALID = 0,       /* Not initialized or non-existant */
  DMOV_CHAN_STATE_STOP = 1,          /* Hardware is stopped */
  DMOV_CHAN_STATE_POWER_SAVE = 2,    /* Hardware is power collapsed */
  DMOV_CHAN_STATE_FATAL = 3,         /* Fatal internal error has occurred */
  DMOV_CHAN_STATE_NO_ACCESS = 4,     /* Channel is not assigned to local processor */

  /* The following are remote active states, 
   * and must have values greater than the previous states. */
  DMOV_CHAN_STATE_RUN_NO_ACCESS = 5, /* Channel is running, not local */

  /* The following are local active states, 
   * and must have values greater than the previous states.  */
  DMOV_CHAN_STATE_RUN = 6,           /* Hardware is operational */
  DMOV_CHAN_STATE_STOPPING = 7,      /* Stop is in progress */
  DMOV_CHAN_STATE_FLUSHING = 8,      /* Hardware flush is in progress */
  DMOV_CHAN_STATE_FLUSHING_ALL = 9,  /* Hardware + SW queue flush is in progress */
  DMOV_CHAN_STATE_ERROR = 20,        /* Hardware is in error state, stalled */

  DMOV_CHAN_STATE_FORCE_32_BIT = 0x7FFFFFFF  /* Force enum to be 32-bit */
};

/* Clock IDs */
enum Dmov_ClockID
{
    DMOV_CLK_ADM0 = 0,
    DMOV_CLK_ADM0_PBUS,
    DMOV_CLK_ADM1,
    DMOV_CLK_ADM1_PBUS,
    DMOV_CLK_MAX_ID,
    DMOV_CLK_FORCE_32_BIT = 0x7FFFffff,
};

/* Channel structure */
typedef struct Dmov_ChanType Dmov_ChanType;
struct Dmov_ChanType
{
  uint32          dwChan;
  Dmov_ChanState  eState;
  uint32          dwChanOpMode;
  uint32          dwDevice;
  uint32          dwDomain;
  DALBOOL         bDiscardFlush;

  /* Statistics */
  uint32  dwTransferID;                /* Part of the token */
  uint32  dwCmdsQueuedCount;   		   /* Number of commands queued */
  uint32  dwCmdsIssuedCount;   		   /* Number of commands written to channel */
  uint32  dwResultsCount;       	   /* Number of results received */
#ifdef DMOV_LEGACY_SUPPORT
  uint32  dwXferCallbackCount_Legacy;  /* Number of transfer user callbacks invoked */
#endif
  uint32  dwEventTriggeredCount;
  uint32  dwResultFlushCount;		   /* Number of results received with flush indication */
  uint32  dwResultErrorCount;		   /* Number of results received with error indication */
  uint32  dwInternalErrorCount;		   /* Number of internal driver errors for this channel */

  /* Error state recorded when the channel is stopped or killed
   * by software, or suspended by hardware due to bus error, etc. */
  DALBOOL         bErrorValid;         /* TRUE if error state data is valid */
  Dmov_ChanError  eError;
  DmovResult      ErrorResult;

  /* Bus error state arrays - elements set TRUE if a bus error is pending.
   * Note that if any bus error is pending, the channel will be held in a 
   * disabled state by hardware. For convenience, "bus_error" is provided 
   * to allow the user to quickly check for a bus error condition. */
  DALBOOL  bBusBrror;                  /* Logical OR of all the following bus errors */
  DALBOOL  bDataBusErrors[DMOV_MAX_NUM_CLIENT_INTERFACES];
  DALBOOL  bCmdBusErrors[DMOV_MAX_NUM_CLIENT_INTERFACES];

  /* Command queues */
  uint32  dwNumQueues;                 /* Number of valid command queues */
  DALFW_Q_Type  CMD_Q[DMOV_MAX_NUM_QUEUES];  /* Commands awaiting service */
  DALFW_Q_Type  PENDING_Q;                   /* Commands issued to Data Mover pending results */
  DALFW_Q_Type  DONE_Q;                      /* Commands waiting to be picked by 
                                              * clients after they get notified via events 
                                              */

  DALSYSEventHandle  hEvent;           /* Event to notify the owner of this channel when
                                        * an error occurs
                                        */
};

#ifdef DMOV_LEGACY_SUPPORT
/* Data Mover transfer results structure */
typedef struct
{
  uint32 result;    /* CHn_RSLT_SDc register value */

  /* Values of the CHn_FLUSH_STATE*_SDc registers
   * The following values are valid only if the ERR or F flag was
   * set in the CHn_RSLT_SDc register
   */
  uint32 flush_state[DMOV_MAX_NUM_FLUSH_STATE_REGS];
} dmov_result_s_type_legacy;

typedef void (*dmov_xfer_callback_f_type_legacy)
(
  /* Pointer to transfer results.  If user wishes to retain results,
   * they must copy them before returning.  The buffer to which result_ptr
   * points will be cleared and deallocated after the user callback returns.
   */
  const dmov_result_s_type_legacy *result_ptr,

  /* User pointer registered with dmov_transfer() */
  void *user_ptr
);
#endif

/* DMOV private data struct for queueing transfer descriptors */
typedef struct DmovTransferQItem DmovTransferQItem;
struct DmovTransferQItem
{
  DALFW_Q_LinkType  Link;                      /* Queue link header */
  uint32       TaskId;                         /* Task identifier */
  DmovResult   Results;                        /* Transfer results */

  uint32 * pCmdListPtrBuffer;                  /* Memory for command list pointer */
  uint32 * pCmdListPtrPhysBuffer;              /* Physical Memory for command list pointer */
  uint32 * pCmdBuffer;                         /* Memory for command  */
  uint32 * pCmdPhysBuffer;                     /* Physical Memory for command  */

  DmovTransferType  TransferType;

  /* Event to be issued to the client when ISR is received for 
  this transfer, if this is null the client won't be notified */
  DALSYSEventHandle  hEvent;
  DALSYS_EVENT_OBJECT(ReqEventObj);

  uint32 dwToken; /* clients will retrieve results by sending a token that we 
                   * can compare to this one and make sure they are interested in 
                   * the result for this transfer 
                   */

  DmovCommandType  CommandType;  
  uint32         * CommandPtr;

#ifdef DMOV_LEGACY_SUPPORT
  dmov_xfer_callback_f_type_legacy  callback_ptr;   /* User callback function or NULL */
#endif
};

/*------------------------------------------------------------------------------
Declaring a "Dmov" Driver,Device and Client Context
------------------------------------------------------------------------------*/
typedef struct DmovDrvCtxt DmovDrvCtxt;
typedef struct DmovDevCtxt DmovDevCtxt;
typedef struct DmovClientCtxt DmovClientCtxt;

/*------------------------------------------------------------------------------
Declaring a private "Dmov" Vtable
------------------------------------------------------------------------------*/
typedef struct DmovDALVtbl DmovDALVtbl;
struct DmovDALVtbl
{
	int (*Dmov_DriverInit)(DmovDrvCtxt *);
	int (*Dmov_DriverDeInit)(DmovDrvCtxt *);
};

/* DMOV device error */
typedef enum DmovDevError DmovDevError;
enum DmovDevError
{
  DMOV_DEV_NO_ERROR = 0,
  DMOV_DEV_ATTACH_INTERRUPTCONTROLLER_FAILED = 1,
  DMOV_DEV_OPEN_INTERRUPTCONTROLLER_FAILED = 2,
  DMOV_DEV_REGISTER_ISR_FAILED = 3,
  DMOV_DEV_UNREGISTER_ISR_FAILED = 4,
  DMOV_DEV_SYNC_OBJECT_CREATION_FAILED = 5,
  DMOV_DEV_MEM_ALLOCATION_FAILED = 6,
  DMOV_DEV_MEM_FREE_FAILED = 7,
  DMOV_DEV_OUT_OF_XFER_DESCRIPTORS = 8,

  DMOV_DEV_ERROR_FORCE_32_BIT = 0x7FFFFFFF
};

/* DMOV device state */
typedef enum DmovDevState DmovDevState;
enum DmovDevState
{
  /* The following are inactive states */
  DMOV_DEV_NOT_INITILIZED = 0,  /* Not initialized or non-existant */
  DMOV_DEV_RUN = 1,             /* Initialized and running */

  DMOV_DEV_STATE_FORCE_32_BIT = 0x7FFFFFFF
};

/* max number of client can be 64 in current design due to 
   limitation in bits of nActiveClientsMask */
#define DMOV_MAX_CLIENTS 64
#define DMOV_CLIENT_NOT_REGISTERED 0xFFFFFFFF

/* DMOV device context */
struct DmovDevCtxt
{
  /* Base Members */
  uint32   dwRefs;                                    
  DALDEVICEID DevId;                                  
  uint32   dwDevCtxtRefIdx; 
  DmovDrvCtxt  *pDmovDrvCtxt;                             
  DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
  uint32 Reserved[16];

  DALSYSSyncHandle  hDeviceSynchronization;

  DmovDevState  eState;
  DmovDevError  eError;
  uint32        dwDeviceInstance;
  uint32        dwNumChans;
  uint32        dwNumSecDomains;
  uint32        dwNumFlushRegisters;

  // device interrupt
  //DalDeviceHandle  hDALIntCtrl;
  DALInterruptID   InterruptId;
  DALBOOL          bInterruptsAvailable;

  uint32 dwOpenCount;

  /* Number of transfers queued for this Data Mover device, including
   * pending transfers (active_count) 
   */
  uint32  dwQueuedCount;
  uint32  dwActiveCount;
      
  /* Channel state array */
  Dmov_ChanType*  Chans[DMOV_MAX_NUM_CHANNELS_PER_DEV];

  /* Queue for transfers awaiting client notification */
  DALFW_Q_Type DmovDevXferNotify;

  /* Statistics */
  uint32  dwFlushedChanCounter;
  uint32  dwFlushTotalResultsCounter;
  uint32  dwFlushNotIdleCounter;
  uint32  dwFlushResultsCounter;
  uint32  dwFlushIRQStatus;
  uint32  dwIRQCounter;


  /* IRQ_STATUS after dmov_domain_flush() */
  uint32  dwDmovDomainFlushIRQStatus;

  /* For use when DALClkRegime comes in */
  DalDeviceHandle*  hDALClkRegim;
  /* *** Clock Management Features ***  */

  /* Clock IDs for ADM core clock and PBUS clocks
   * These clock IDs are used to control the clocks
   * using the DAL Clock driver
   */
#ifdef DMOV_USE_DAL_CLOCK_DRIVER
  ClockIdType  hClockID[DMOV_CLK_MAX_ID];
#endif
  /* Clocks could be managed or left on forever 
   *  - Depends on system limitations 
   */
  DALBOOL          bClocksManaged;

  /* Bitflag mask of all active clients for this DMOV device 
   *  - Each bit represent their entry in the hClients Array
   */
  uint64           nActiveClientsMask;

  /* Bitflag mask for all active clients' ADM Perf Level Preferences */
  /* This is valid only for ADM as of now   */
  uint64           nActiveClientsPerfLvls[DMOV_NUM_PERFLVLS];
  DmovPerfLvlType  eCurrentPerfLvl;

  /* Pointers to the clients' context */ 
  DmovClientCtxt * hClients[DMOV_MAX_CLIENTS];

  /* Number of clients */
  uint32           hClientsCount;

  /* *** End of Clock Management Features *** */

  DALFW_Q_Type       DmovXferFreePool;
  uint32             nDmovTransferCount;
  DmovTransferQItem  *pDmovTransfer;


  /* DMOV device memory pools */
  uint32           dwCmdMemNeeded;
  DALSYSMemHandle  hCmdMem;                   /* Command List Pointers */
  DALSYSMemInfo    CmdMemInfo;
};

typedef enum DmovDrvError DmovDrvError;
enum DmovDrvError
{
  DMOV_DRV_NO_ERROR = 0,
  DMOV_DRV_MEM_ALLOCATION_FAILED = 1,
  DMOV_DRV_MEM_FREE_FAILED = 2,
  DMOV_DRV_SYNC_OBJECT_CREATION_FAILED = 3,
  DMOV_DRV_SYNC_OBJECT_DESTRUCTION_FAILED = 4,
  DMOV_DRV_OUT_OF_XFER_DESCRIPTORS = 5,

  DMOV_DRV_ERROR_FORCE_32_BIT = 0x7FFFFFFF
};

struct DmovDrvCtxt
{
  /*Base Members */
  DmovDALVtbl DmovDALVtbl;
  uint32  dwNumDev;
  uint32  dwSizeDevCtxt;
  uint32  bInit;
  uint32  dwRefs;
  DmovDevCtxt DmovDevCtxt[HAL_DMOV_MAX_NUM_DEVICES];
  DALBOOL  DrvInitialized;

  DmovDrvError eError;

  DALSYSSyncHandle  hDriverSynchronization;
};

/*------------------------------------------------------------------------------
Declaring a "Dmov" Client Context
------------------------------------------------------------------------------*/
struct DmovClientCtxt
{
  /* Base Members */
  uint32          dwRefs;                     
  uint32          dwAccessMode;  
  void          * pPortCtxt;
  DmovDevCtxt   * pDmovDevCtxt;            
  DalDmovHandle   DalDmovHandle; 

  /* Extended Members */
  /* Handle Info */
  uint32          dwOpenCount;
  uint32          nHandleID;
  const char    * pszClientName;
  DmovPerfLvlType ePerfLvl;
  /* Transfer Info */
  uint32  dwTransferID;                       
  uint32  dwCurrentResultToken;
  uint32  dwChannelsModes;          /* Each mode bit for each channel */
};

/* ----------------------------------------------------------------------------
** Interface 
** ------------------------------------------------------------------------- */

DALResult Dmov_DriverInit(DmovDrvCtxt *);
DALResult Dmov_DriverDeInit(DmovDrvCtxt *);
DALResult Dmov_DeviceInit(DmovClientCtxt *);
DALResult Dmov_DeviceDeInit(DmovClientCtxt *);
DALResult Dmov_Reset(DmovClientCtxt *);
DALResult Dmov_PowerEvent(DmovClientCtxt *, DalPowerCmd, DalPowerDomain);
DALResult Dmov_RegisterClient(DmovClientCtxt * pCtxt);
DALResult Dmov_DeregisterClient(DmovClientCtxt * pCtxt);
DALResult Dmov_Open(DmovClientCtxt *, uint32);
DALResult Dmov_Close(DmovClientCtxt *);
DALResult Dmov_Info(DmovClientCtxt *,DalDeviceInfo *, uint32);
DALResult Dmov_InheritObjects(DmovClientCtxt *,DALInheritSrcPram *,DALInheritDestPram *);

/* ----------------------------------------------------------------------------
** DalDmov Interface 
** ------------------------------------------------------------------------- */


/*------------------------------------------------------------------------------
Functions specific to DalDmov interface
------------------------------------------------------------------------------*/
DALResult Dmov_Transfer( DmovClientCtxt *, const DmovTransferRequest* , uint32* , DmovReqStatus*);
DALResult Dmov_GetTransferResult( DmovClientCtxt *, uint32 ,  DmovResult*);
DALResult Dmov_SetChanOperationMode( DmovClientCtxt *, uint32 , uint32 );
DALResult Dmov_FlushChannel( DmovClientCtxt *, uint32 , uint32 , uint32 );
DALResult Dmov_FlushDomain( DmovClientCtxt *, uint32 );
DALResult Dmov_PrepareSGDescList( DmovClientCtxt *, uint32 , uint32* );
DALResult Dmov_PrepareCommand( DmovClientCtxt *, DmovCommandOptions *, uint32* );
DALResult Dmov_PrepareCommandListPtr( DmovClientCtxt *, uint32* , uint32 , uint32* );

#endif /*DALDmov_H*/


