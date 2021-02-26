/*=============================================================================

FILE:         HalQdssStm.c

DESCRIPTION:   This file implements HAL functionalities for the STM hardware 

================================================================================
            Copyright (c) 2015 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/

/*============================================================================
                          INCLUDE FILES
============================================================================*/


#include "HalQdssStm.h"
#include "HalQdssStmConfig.h"
#include "HwioQdssStm.h"

#define HIGH 1
#define LOW  0


#define CLIP_VALUE(low,high,val) \
   (val)<(low)?(low):((val)>(high)?(high):(val))

#define IS_MULTIPLE_OF(factor,val) (((val)/(factor))*(factor)==(val)?TRUE:FALSE)
#define FLOOR_DOWN_FACTOR(factor,val) ((val)/(factor))*(factor)

#define SYNC_PERIOD_2N_LOW  12
#define SYNC_PERIOD_2N_HIGH 27

#define STM_MASTER_SECURE     0x0
#define STM_MASTER_NON_SECURE 0x40

volatile hal_hwio_qdss_addr_type QDSS_STM_STIMULUS_PORT_START_ADDR;

#define QDSS_STM_NUMBER_OF_STIMULUS_PORTS    QDSS_STM_NUM_STIMULUS_PORTS

// Track which stimulus ports are in use.
uint8 stm_port_in_use_list[QDSS_STM_NUMBER_OF_STIMULUS_PORTS];

static HAL_qdss_stm_FeatureType aSTMFeatList[QDSS_STM_NUM_FEAT];

static uint32 power_of_2(uint32 val);

#if 0  // Replace these with below calls.
static void HAL_qdss_stm_PopulateFeatures(HAL_qdss_stm_FeatureType *pfeatlist);


/*FUNCTION HAL_qdss_stm_HalConfigInit*/
void HAL_qdss_stm_HalConfigInit(uint32 nBaseAddr)
{
#ifdef HAL_DEVICEENUM_ENABLE
#warning *TBD:Provide a run-time function to obtain hardware configuration
#endif
   QDSS_STM_BASE_PTR = nBaseAddr;
/*Init function should also populate features available*/
   HAL_qdss_stm_PopulateFeatures(aSTMFeatList);

}

/*FUNCTION HAL_qdss_stm_SetConfigPhysAddr*/
void HAL_qdss_stm_SetConfigPhysAddr(uint32 nPhysAddr)
{
   QDSS_STM_BASE_PTR = nPhysAddr;
}
#endif

void HAL_qdss_stm_SetConfigStimulusPort()
{
  QDSS_STM_STIMULUS_PORT_START_ADDR =
    QDSS_STM_STIMULUS_PORT_BASE_PHYS + (QDSS_STM_START_STIMULUS_PORT << 8);
    // Multiply by 256 bytes per port.
}

hal_hwio_qdss_addr_type HAL_qdss_stm_GetStimulusPortStart()
{
  return QDSS_STM_STIMULUS_PORT_START_ADDR;
}

uint32 HAL_qdss_stm_GetNumStimulusPorts()
{
  return  QDSS_STM_NUMBER_OF_STIMULUS_PORTS;
}

uint8 *HAL_qdss_stm_GetStmPortInUseArray()
{
  uint8 *retptr;
  retptr = &(stm_port_in_use_list[0]);
  return retptr;
}

/* FUNCTION HAL_qdss_stm_Enable*/
void HAL_qdss_stm_Enable(void)
{
  HWIO_OUTF(QDSS_STMTCSR, EN, HIGH);
}

/* FUNCTION HAL_qdss_stm_Disable*/
void HAL_qdss_stm_Disable(void)
{
  HWIO_OUTF(QDSS_STMTCSR, EN, LOW);
  while (HAL_qdss_stm_isBusy()) {
    /*Do Nothing. Wait for STM to drain its buffers*/
  }
}

/* FUNCTION HAL_qdss_stm_isEnabled*/
boolean  HAL_qdss_stm_isEnabled(void)
{
  return (HWIO_INF(QDSS_STMTCSR, EN));
}

/* FUNCTION HAL_qdss_stm_isBusy*/
boolean  HAL_qdss_stm_isBusy(void)
{
  return (HWIO_INF(QDSS_STMTCSR, BUSY));
}

void HAL_qdss_stm_TraceIdCfg(uint32 nTraceId)
{
  HWIO_OUTF(QDSS_STMTCSR, TRACEID, nTraceId);
}

/* FUNCTION HAL_qdss_stm_isCompEnabled*/
boolean HAL_qdss_stm_isCompEnabled(void)
{
  return (HWIO_INF(QDSS_STMTCSR, COMPEN));
}

/* FUNCTION HAL_qdss_stm_CompEnable*/
void HAL_qdss_stm_CompEnable(void)
{
  HWIO_OUTF(QDSS_STMTCSR, COMPEN, HIGH);
}

/* FUNCTION HAL_qdss_stm_CompDisable*/
void HAL_qdss_stm_CompDisable(void)
{
  HWIO_OUTF(QDSS_STMTCSR, COMPEN, LOW);
}

/* FUNCTION HAL_qdss_stm_TimeStampEnable*/
void  HAL_qdss_stm_TimeStampEnable(void)
{
  HWIO_OUTF(QDSS_STMTCSR, TSEN, HIGH);
}

/* FUNCTION HAL_qdss_stm_TimeStampDisable*/
void  HAL_qdss_stm_TimeStampDisable(void)
{
  HWIO_OUTF(QDSS_STMTCSR, TSEN, LOW);
}

/* FUNCTION HAL_qdss_stm_GenerateTimeStamp*/
void  HAL_qdss_stm_GenerateTimeStamp(void)
{
  HWIO_OUT(QDSS_STMTSSTIMR, (HIGH << HWIO_SHFT(QDSS_STMTSSTIMR, FORCETS)) &
             HWIO_FMSK(QDSS_STMTSSTIMR, FORCETS));
}

/* FUNCTION HAL_qdss_stm_GetTimeStampFreq*/
uint32 HAL_qdss_stm_GetTimeStampFreq(void)
{
  return HWIO_INF(QDSS_STMTSFREQR, FREQ);
}

/* FUNCTION HAL_qdss_stm_SetTimeStampFreq*/
void  HAL_qdss_stm_SetTimeStampFreq(uint32 nFreq)
{
  HWIO_OUTF(QDSS_STMTSFREQR, FREQ, nFreq);
}

/* FUNCTION HAL_qdss_stm_HWEventSelectAll*/
void  HAL_qdss_stm_HWEventSelectAll(void)
{
  HWIO_OUT(QDSS_STMHEER, 0xFFFFFFFF);
}

/* FUNCTION HAL_qdss_stm_HWEventTriggerEnableAll*/
void  HAL_qdss_stm_HWEventTriggerEnableAll(void)
{
  HWIO_OUT(QDSS_STMHETER, 0xFFFFFFFF);
}

/* FUNCTION HAL_qdss_stm_HWEventEnable*/
void  HAL_qdss_stm_HWEventEnable(void)
{
  HWIO_OUTF(QDSS_STMHEMCR, EN, 1);
}

/* FUNCTION HAL_qdss_stm_HWEventDisable*/
void  HAL_qdss_stm_HWEventDisable(void)
{
  HWIO_OUTF(QDSS_STMHEMCR, EN, 0);
}

/* FUNCTION HAL_qdss_stm_HWevent_isEnabled*/
boolean  HAL_qdss_stm_HWevent_isEnabled(void)
{
  return (HWIO_INF(QDSS_STMHEMCR, EN));
}


/* FUNCTION HAL_qdss_isSyncEnabled*/
boolean HAL_qdss_isSyncEnabled(void)
{
  return (HWIO_INF(QDSS_STMTCSR, SYNCEN));
}


/* FUNCTION HAL_qdss_stm_SyncEnable*/
void HAL_qdss_stm_SyncEnable(void)
{
  HAL_qdss_stm_FeatureType const *pstmfeat;
  pstmfeat = HAL_QDSS_STM_GET_FEAT_VAL(STMSYNCR);
  if (pstmfeat->eSTMFeatName == HAL_QDSS_STM_FEAT_NAME(STMSYNCR)) {
    if (pstmfeat->eSTMFeatValue == QDSS_STM_FEAT_SYNCEN_DEFINE_RW) {
      HWIO_OUTF(QDSS_STMTCSR, SYNCEN, HIGH);
    }
  }
}

/* FUNCTION HAL_qdss_stm_SyncDisable*/
void HAL_qdss_stm_SyncDisable(void)
{
  HAL_qdss_stm_FeatureType const *pstmfeat;
  pstmfeat = HAL_QDSS_STM_GET_FEAT_VAL(STMSYNCR);
  if (pstmfeat->eSTMFeatName == HAL_QDSS_STM_FEAT_NAME(STMSYNCR)) {
    if (pstmfeat->eSTMFeatValue == QDSS_STM_FEAT_SYNCEN_DEFINE_RW) {
      HWIO_OUTF(QDSS_STMTCSR, SYNCEN, LOW);
    }
  }
}

/* FUNCTION HAL_qdss_stm_SyncPeriodCfg*/
void HAL_qdss_stm_SyncPeriodCfg(HAL_qdss_SyncModeType eSyncMode, uint32 nBytes)
{
  uint32 mode, count;

/**
** HW implementation restricts range of N
*/
  if (eSyncMode == STM_SYNC_MODE_PERIOD_2_POWER_N_BYTES) {
    nBytes = CLIP_VALUE(SYNC_PERIOD_2N_LOW, SYNC_PERIOD_2N_HIGH, nBytes);
  }

  mode = (eSyncMode << (HWIO_SHFT(QDSS_STMSYNCR, MODE))) & HWIO_FMSK(QDSS_STMSYNCR, MODE);
  count = (nBytes << (HWIO_SHFT(QDSS_STMSYNCR, COUNT))) & HWIO_FMSK(QDSS_STMSYNCR, COUNT);
  HWIO_OUT(QDSS_STMSYNCR, mode | count);
}

/* FUNCTION HAL_qdss_stm_AccessUnLock*/
void  HAL_qdss_stm_AccessUnLock(void)
{
  HWIO_OUT(QDSS_STMLAR, (0xC5ACCE55 << HWIO_SHFT(QDSS_STMLAR, ACCESS_W)) &
             (HWIO_FMSK(QDSS_STMLAR, ACCESS_W)));
}

// 3316436565 = 0xC5ACCE55

/* FUNCTION HAL_qdss_stm_AccessLock*/
void  HAL_qdss_stm_AccessLock(void)
{
  HWIO_OUT(QDSS_STMLAR, ((~0xC5ACCE55) << HWIO_SHFT(QDSS_STMLAR, ACCESS_W)) &
             (HWIO_FMSK(QDSS_STMLAR, ACCESS_W)));
}

/* FUNCTION HAL_qdss_stm_isAccessLocked*/
boolean  HAL_qdss_stm_isAccessLocked(void)
{
  return (HWIO_INF(QDSS_STMLSR, LOCKGRANT));
}

/* FUNCTION HAL_qdss_stm_SetClaimTag*/
void HAL_qdss_stm_SetClaimTag(uint32 nTag)
{
  HWIO_OUTF(QDSS_STMCLAIMSET, CLAIMSET, nTag);
}

void HAL_qdss_stm_ClrClaimTag(uint32 nTag)
{
  HWIO_OUTF(QDSS_STMCLAIMCLR, CLAIMCLR, nTag);
}


uint32  HAL_qdss_stm_ClaimTagImplementMask(void)
{
  return HWIO_FMSK(QDSS_STMCLAIMSET, CLAIMSET);

}

/* FUNCTION HAL_qdss_stm_GetClaimTag*/
uint32  HAL_qdss_stm_GetClaimTag(void)
{
  return HWIO_INF(QDSS_STMCLAIMCLR, CLAIMCLR);
}

/* FUNCTION HAL_qdss_stm_PortEnableMask*/
void HAL_qdss_stm_PortEnableMask(uint32 mPortEnableMask)
{
  HWIO_OUTF(QDSS_STMSPER, SPE, mPortEnableMask);
}

/* FUNCTION HAL_qdss_stm_PortSelectAll*/
void HAL_qdss_stm_PortSelectCfgAll(void)
{
  HWIO_OUTF(QDSS_STMSPSCR, PORTCTL, PORT_SELECT_CTL_SELECT_ALL);
}


/* FUNCTION HAL_qdss_stm_PortSelectCfg*/
void HAL_qdss_stm_PortSelectCfg(uint32 nStartPort,
                                uint32 nEndPort,
                                HAL_qdss_stm_PortSelectCtl ePortSelectCtl,
                                HAL_qdss_stm_PortSelRet *pPortSel)
{
  /*
   * A group contains 32 ports. Calculate group id=nStartPort/32.
   * Convert nNumPorts to nearest power of 2
   * Write to STMSPSCR
  **/
  uint32 nStartGroup, nNumGroups, nEndGroup, nStartGroupMultiple;
  uint32 nNewEndGroup;

  switch (ePortSelectCtl) {
    case PORT_SELECT_CTL_TRIGGER_ONLY:
    case PORT_SELECT_CTL_TRACE_TRIGGER:

      nStartGroup = nStartPort >> 5; /*divide by 32*/
      nEndGroup = nEndPort >> 5;
      nNumGroups = nEndGroup - nStartGroup + 1; /*include the start group*/

      /* We now have nNumGroups@nStartGroup. nNumGroups is rounded to higher ^2.
       * nStartGroup needs to be a multiple of nNumGroups and yet be in the range of
       * groups indicated by StartGroup and EndGroup.
      **/
      nNumGroups = power_of_2(nNumGroups);
      nStartGroupMultiple = FLOOR_DOWN_FACTOR(nNumGroups, nStartGroup);

      /* If the start group is revised, check if new end group covers the range of
         groups required to be enabled. If it does not cover the range then double the
         number of groups needed to be enabled.
      **/
      nNewEndGroup = nStartGroupMultiple + nNumGroups - 1;
      if (nNewEndGroup < nEndGroup) {
        nNumGroups *= 2;
      }
      while (!IS_MULTIPLE_OF(nNumGroups, nStartGroupMultiple)) {
        nStartGroupMultiple = nStartGroupMultiple / nNumGroups;
        nNumGroups = nNumGroups * 2;
      }

      HWIO_OUTM(QDSS_STMSPSCR,
        (HWIO_FMSK(QDSS_STMSPSCR, PORTSEL)) | (HWIO_FMSK(QDSS_STMSPSCR, PORTCTL)),
        ((nStartGroupMultiple | nNumGroups) << HWIO_SHFT(QDSS_STMSPSCR, PORTSEL)) |
         (ePortSelectCtl << HWIO_SHFT(QDSS_STMSPSCR, PORTCTL))
        );

      pPortSel->nStartPort = nStartGroupMultiple << 5;
      pPortSel->nEndPort = (nStartGroupMultiple << 5) + (nNumGroups << 5) - 1;
      break;

    case PORT_SELECT_CTL_SELECT_ALL:
    default:
      HAL_qdss_stm_PortSelectCfgAll();
  }

}

void HAL_qdss_stm_MasterSelectCfgAll(void)
{
  HWIO_OUTF(QDSS_STMSPMSCR, MASTCTL, MASTER_SELECT_CTL_ALL);
}

void HAL_qdss_stm_MasterSelectCfg(uint32 nMasterID,
                                  HAL_qdss_stm_MasterSelectCtl eMasSelCtl)
{
  switch (eMasSelCtl) {
    case MASTER_SELECT_CTL_ALL:
      HAL_qdss_stm_MasterSelectCfgAll();
      break;

    case MASTER_SELECT_CTL_SUBSET:
      if (nMasterID == STM_MASTER_SECURE || nMasterID == STM_MASTER_NON_SECURE) {
        HWIO_OUTM(QDSS_STMSPMSCR, HWIO_FMSK(QDSS_STMSPMSCR, MASTSEL) |
                    HWIO_FMSK(QDSS_STMSPMSCR, MASTCTL), (nMasterID << HWIO_SHFT(QDSS_STMSPMSCR, MASTSEL)) |
                    MASTER_SELECT_CTL_SUBSET << HWIO_SHFT(QDSS_STMSPMSCR, MASTCTL));
      }
      break;
    default: /* Do nothing */
      break;
  }
}

static uint32 power_of_2(uint32 val)
{
  if (val & (val - 1)) {
    while (val) {
      val = val & (val - 1);
      if (!(val & (val - 1))) {
        break;
      }
    }
    /* round to higher power of 2 */
    val = val << 1;
  }
  return val;
}

HAL_qdss_stm_FeatureType const* HAL_qdss_stm_GetFeature(
  HAL_qdss_stm_FeatureName eSTMFeatName)
{
  if (eSTMFeatName < QDSS_STM_NUM_FEAT) {
    return (&aSTMFeatList[eSTMFeatName]);
  } else {
    return (HAL_qdss_stm_FeatureType const *)0;
  }
}
#if 0  // unused.
static void HAL_qdss_stm_PopulateFeatures(HAL_qdss_stm_FeatureType *pfeatlist)
{
  uint32 feat1;
  feat1 = HWIO_IN(QDSS_STMSPFEAT1R);

  pfeatlist[HAL_QDSS_STM_FEAT_NAME(SWOEN)].eSTMFeatName =
    HAL_QDSS_STM_FEAT_NAME(SWOEN);

  pfeatlist[HAL_QDSS_STM_FEAT_NAME(SWOEN)].eSTMFeatValue =
    (HAL_qdss_stm_FeatureValue)((feat1)&HWIO_FMSK(QDSS_STMSPFEAT1R, SWOEN)) >>
    (HWIO_SHFT(QDSS_STMSPFEAT1R, SWOEN));

  /*TBD: Other features*/
}
#endif

void HAL_qdss_stm_TrigGenTrigLocation(void)
{
  HWIO_OUTF(QDSS_STMSPTRIGCSR, ATBTRIGEN_DIR, 1);
}

void HAL_qdss_stm_EnableSingleShot(void)
{
  /* Enable single shot */
  HWIO_OUTF(QDSS_STMSPTRIGCSR, TRIGCTL, 1);
}

void HAL_qdss_stm_DisableSingleShot(void)
{
  /* Disable single shot, Enable multi-shot */
  HWIO_OUTF(QDSS_STMSPTRIGCSR, TRIGCTL, 0);
}

void HAL_qdss_stm_HWEventErrorDetectEnable(void)
{
  HWIO_OUTF(QDSS_STMHEMCR, ERRDETECT, 1);
}

void HAL_qdss_stm_HWEventCompressionEnable(void)
{
  HWIO_OUTF(QDSS_STMHEMCR, COMPEN, 1);
}

void HAL_qdss_stm_HWEventATIDTriggerGenDisable(void)
{
  HWIO_OUTF(QDSS_STMHEMCR, ATBTRIGEN, 0);
}

void HAL_qdss_stm_HWEventTrigOutMultiShotMode(void)
{
  HWIO_OUTF(QDSS_STMHEMCR, TRIGCTL, 0);
}

#define QDSS_STM_SYNCR_CFG  0x1600  //for (2*12) or 4096 bytes
void HAL_qdss_stm_SetDefaultSyncPeriod(void)
{
  HWIO_OUT(QDSS_STMSYNCR, QDSS_STM_SYNCR_CFG);
  HWIO_OUTF(QDSS_STMTCSR, SYNCEN, HIGH);
}

