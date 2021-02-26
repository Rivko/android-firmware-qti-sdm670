/**********************************************************************
 * File: sdcc_sd_util.c
 *
 * Services: 
 *    This file provides SDCC services for SD device.
 *
 * Description:
 *    This file contains the SD specific utility function.
 *
 * Copyright (c) 2014-2016 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 **********************************************************************/
/*=======================================================================
                        Edit History


when        who     what, where, why
----------  ---     ----------------------------------------------------- 
2016-12-13  jt      Remove legacy code  
2016-01-18  eo      Reduced sdcc code size
2015-12-17  jt      Reuse buffer from tuning to fix ADMA error  
2014-06-18  bn      Initial version. Branched from 8994 SBL

===========================================================================*/
#include "sdcc_api.h"
#include "sdcc_util.h"
#include "sdcc_sd_util.h"
#include "sdcc_handle_util.h"
#include "sdcc_hc.h"
#include "sdcc_priv.h"
#include "sdcc_pal_tool.h"
#include "SdccCommon.h"

/* 128 byte buffer used for reading SD card data */
extern UINT8 gTempBuffer[128];

/*lint -save -e641 Suppress 'Converting enum to int' warning */
/******************************************************************************
* Name: sdcc_find_sd_device
*
* Description: 
*    This function looks for the SD device of the specified driveno.
*
* Parameters:
*    handle [IN] : a pointer to the SDCC device 
*
* Returns:
*    card type that indicates if device is SD or unknown
*
******************************************************************************/
SDCC_CARD_TYPE
sdcc_find_sd_device( struct sdcc_device *handle )
{
   SDCC_CARD_TYPE card_type = SDCC_CARD_UNKNOWN;
   sdcc_dev_type *sdcc_pdev = handle->hdev;
   SDCC_STATUS rc = SDCC_ERR_UNKNOWN;
   UINT32 cmd41_arg = SDCC_NEGOTIATE_OCR;
   sdcc_cmd_type sdcc_cmd;
   UINT32 i;

   /* CMD0: reset card first */
   sdcc_cmd.cmd       = SD_CMD0_GO_IDLE_STATE;
   sdcc_cmd.cmd_arg   = SDCC_CMD_ARGU_NULL;
   sdcc_cmd.resp_type = SDCC_RESP_NONE;
   sdcc_cmd.prog_scan = 0;
   sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

   (void) sdcc_command( sdcc_pdev, &sdcc_cmd );

   /* card may not be in sync, make sure CMD8 response is received */
   for ( i = 0; i < SDCC_CMD8_RETRIES; i++ )
   {
      /* CMD8: verify SDHC card interface operating condition */
      sdcc_cmd.cmd       = SD_CMD8_SEND_IF_COND;
      sdcc_cmd.cmd_arg   = SDCC_HC_VOLT_SUPPLIED;
      sdcc_cmd.resp_type = SDCC_RESP_SHORT;
      sdcc_cmd.prog_scan = 0;
      sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

      rc = sdcc_command( sdcc_pdev, &sdcc_cmd );

      if ( SDCC_NO_ERROR == rc )
      {
         /* If the card can operate on the supplied voltage, the response */
         /* echos back the supply voltage and the check pattern. */
         if ( sdcc_cmd.resp[0] != SDCC_HC_VOLT_SUPPLIED )
         {
            /* non-compatible voltage range or check pattern is incorrect */
            /* unusable card */
            return card_type;
         }

         /* Receiving of CMD8 expands the ACMD41 function.  Setting the */
         /* Host Capacity Support bit to 1 in ACMD41 argument indicates */
         /* the host supports High Capacity SD memory card. */
         cmd41_arg |= SDCC_HC_HCS;
         break;
      }
      sdcc_mdelay(SDCC_SDHC_SEND_IF_COND_WAIT_TIME_MS);
   }

   for ( i = 0; i < SDCC_SD_DETECT_MAX_RETRIES; i++ )
   {
      /* CMD55: the resp will have APP_CMD set, indicating expect
         ACMD from now on */
      sdcc_cmd.cmd       = SD_CMD55_APP_CMD;
      sdcc_cmd.cmd_arg   = SDCC_CMD_ARGU_NULL;
      sdcc_cmd.resp_type = SDCC_RESP_SHORT;
      sdcc_cmd.prog_scan = 0;
      sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

      rc = sdcc_command( sdcc_pdev, &sdcc_cmd );

      if ( SDCC_NO_ERROR != rc )
      {
         /* exit the for-loop for SD card detection if there is issue in */
         /* sending the command to the card */
         break;
      }

      /* ACMD41: sent voltage to be used */
      sdcc_cmd.cmd       = SD_ACMD41_SD_APP_OP_COND;
      sdcc_cmd.cmd_arg   = cmd41_arg;
      sdcc_cmd.resp_type = SDCC_RESP_SHORT;
      sdcc_cmd.prog_scan = 0;
      sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

      rc = sdcc_command( sdcc_pdev, &sdcc_cmd );

      if ( SDCC_NO_ERROR != rc )
      {
         /* exit the for-loop for SD card detection if there is issue in */
         /* sending the command to the card */
         break;
      }
      else if ( sdcc_cmd.resp[0] & SDCC_DEVICE_RDY )
      {
         if ( sdcc_cmd.resp[0] & SDCC_HC_CCS )
         {
            /* The response of ACMD41 includes Card Capacity Status field */
            /* information. If CCS bit is set, the card is a High Capacity */
            /* SD memory card. */
            card_type = SDCC_CARD_SDHC;
            DEBUG ((EFI_D_INFO, "Found SDHC card after %dth retry\n", i));
         }
         else
         {
            card_type = SDCC_CARD_SD;
            DEBUG ((EFI_D_INFO, "Found SD card after %dth retry\n", i));
         }
         return card_type;
      }

      /* Refer to section 3.6 of SD Specifications Part A2 SD Host Controller */
      /* Standard Specification Version 2.00 for the */
      /* SDCC_SD_DETECT_BUSY_RETRY_INTERVAL */
      sdcc_mdelay( SDCC_SD_DETECT_BUSY_RETRY_INTERVAL );
   }

   return card_type;
} /* sdcc_find_sd_device */

/******************************************************************************
* Name: sdcc_get_scr
*
* Description:
*    This function reads the SD card configuration register.
*
* Arguments:
*    sdcc_pdev    [IN/OUT] : pointer to device type
*    scr             [OUT] : SD configuration register
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS sdcc_get_scr( sdcc_dev_type   *sdcc_pdev,
                          sdcc_scr_type   *scr )
{
   SDCC_STATUS      rc         = SDCC_NO_ERROR;
   sdcc_cmd_type    sdcc_cmd;

   SetMem(gTempBuffer, 8, 0);
   sdcc_cmd.cmd       = SD_ACMD51_SEND_SCR;
   sdcc_cmd.cmd_arg   = SDCC_CMD_ARGU_NULL;
   sdcc_cmd.resp_type = SDCC_RESP_SHORT;
   sdcc_cmd.prog_scan = 0;
   sdcc_cmd.flags = SDCC_TRANSFER_FLAG_IS_READ_TRANSFER | SDCC_TRANSFER_FLAG_IS_APP_COMMAND;
   sdcc_pdev->mem.block_mode = SDCC_MEM_BYTE_MODE;

   /* size of SCR register is 64 bits */
   rc = sdcc_do_transfer(sdcc_pdev, &sdcc_cmd, gTempBuffer, 8/*bytes*/);

   sdcc_pdev->mem.block_mode = SDCC_MEM_BLK_MODE;
   if ( SDCC_NO_ERROR != rc )
   {
      DEBUG ((EFI_D_INFO, "Failed in getting SCR register\n"));
      return rc;
   }

   (void) sdcc_decode_scr( gTempBuffer, scr );

   return rc;
} /* sdcc_get_scr */

/******************************************************************************
* Name: sdcc_decode_scr
*
* Description:
*    This function is to parse out the SD Card Configuration register data.
*
* Arguments:
*    data          [IN] : pointer to buffer that contains scr data
*    scr          [OUT] : pointer to the structure to save the scr data to
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
BOOLEAN
sdcc_decode_scr
(
   const UINT8      *data,
   sdcc_scr_type   *scr
)
{
   UINT8 value;

   if ( NULL == scr )
   {
      return FALSE;
   }

   /* in data[0] */
   value = *data;
   /* scr_structure: SCR structure --> scr[63:60] */
   scr->scr_structure = (UINT8)(value >> 4);
   /* sd_spec: SD memory card spec version --> scr[59:56] */
   scr->sd_spec = (UINT8)(value & 0x0F);

   /* in data[1] */
   value = *(data + 1);
   /* data_stat_after_erase: data status after erases --> scr[55:55] */
   scr->data_stat_after_erase = (UINT8)(value >> 7);
   /* sd_security: SD security support --> scr[54:52] */
   scr->sd_security = (UINT8)((value & 0x70) >> 4);
   /* sd_bus_widths: DAT bus widths supported --> scr[51:48] */
   scr->sd_bus_widths = (UINT8)(value & 0x0F);

   return TRUE;
} /* sdcc_decode_scr */

/******************************************************************************
* Name: sdcc_get_sd_status
*
* Description:
*    This function gets the SD Status that contains the status bits by sending
*    ACMD13 to the SD card.
*
* Arguments:
*    sdcc_pdev      [IN] : pointer to device type
*    sd_status     [OUT] : pointer to SD Status structure that contains the 
*                          status bits
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS
sdcc_get_sd_status
(
   sdcc_dev_type         *sdcc_pdev,
   sdcc_sd_status_type   *sd_status
)
{
   SDCC_STATUS       rc         = SDCC_NO_ERROR;
   sdcc_cmd_type     sdcc_cmd;

   SetMem( gTempBuffer, 64, 0 );
   sdcc_cmd.cmd       = SD_ACMD13_SD_STATUS;
   sdcc_cmd.cmd_arg   = SDCC_CMD_ARGU_NULL;
   sdcc_cmd.resp_type = SDCC_RESP_SHORT;
   sdcc_cmd.prog_scan = 0;
   sdcc_cmd.flags = (SDCC_TRANSFER_FLAG_IS_READ_TRANSFER | SDCC_TRANSFER_FLAG_IS_APP_COMMAND);

   sdcc_pdev->mem.block_mode = SDCC_MEM_BYTE_MODE;

   /* size of the SD Status is one data block of 512 bit */
   rc = sdcc_do_transfer(sdcc_pdev, &sdcc_cmd, gTempBuffer, 64/*bytes*/);

   sdcc_pdev->mem.block_mode = SDCC_MEM_BLK_MODE;
   if ( SDCC_NO_ERROR != rc )
   {
      DEBUG ((EFI_D_INFO, "Failed in getting SD Status\n"));
      return rc;
   }

   /* decode SD Status */
   (void) sdcc_decode_sd_status( gTempBuffer, sd_status );

   return rc;
} /* sdcc_get_sd_status */

/******************************************************************************
* Name: sdcc_decode_sd_status
*
* Description:
*    This function is to parse out the SD Status.
*
* Arguments:
*    data           [IN] : pointer to buffer that contains SD Status data
*    sd_status     [OUT] : pointer to the structure to save the SD Status to
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
BOOLEAN
sdcc_decode_sd_status
(
   const UINT8            *data,
   sdcc_sd_status_type   *sd_status
)
{
   UINT8 value;

   if ( NULL == sd_status )
   {
      return FALSE;
   }

   value = *data;
   /* dat_bus_width: currently defined data bus width --> sd_status[511:510] */
   sd_status->dat_bus_width = (UINT8)(value >> 6);

   /* speed_class: SD speed class --> sd_status[447:440] */
   value = *(data + 8);
   sd_status->speed_class = value;

   /* au_size: allocation unit size --> sd_status[431:428] */
   value = *(data + 10);
   sd_status->au_size = (UINT8)(value >> 4);

   return TRUE;
} /* sdcc_decode_sd_status */

/******************************************************************************
* Name: sdcc_sd_switch_to_hs
*
* Description:
*    This function sends CMD6 to query (mode 0) which functions the SD card
*    supports.  It also sends CMD6 to set (mode 1) the card to high speed
*    if the card supports it.
*
* Arguments:
*    sdcc_pdev    [IN/OUT] : pointer to device type
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS 
sdcc_sd_switch_to_hs( sdcc_dev_type *sdcc_pdev )
{
   SDCC_STATUS     rc         = SDCC_NO_ERROR;
   sdcc_cmd_type   sdcc_cmd;
   sdcc_switch_data_type switch_status;

   SetMem(gTempBuffer, 64, 0);
   sdcc_cmd.cmd       = SD_CMD6_SWITCH_FUNC;
   /* query switchable for high speed */
   sdcc_cmd.cmd_arg   = SDCC_QUERY_HS;
   sdcc_cmd.resp_type = SDCC_RESP_SHORT;
   sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_READ_TRANSFER;
   sdcc_cmd.prog_scan = 0;
   sdcc_pdev->mem.block_mode = SDCC_MEM_BYTE_MODE;

   /* CMD6 mode 0 to query */
   rc = sdcc_do_transfer( sdcc_pdev, &sdcc_cmd, gTempBuffer, 64/*bytes*/);

   sdcc_pdev->mem.block_mode = SDCC_MEM_BLK_MODE;
   if ( SDCC_NO_ERROR != rc )
   {
      DEBUG ((EFI_D_INFO, "Failed for querying high speed func\n"));
      return rc;
   }

   /* decode CMD6 query result */
   (void) sdcc_decode_switch_data( gTempBuffer, &switch_status );

   /* check if error occur in query */
   if ( 0 == switch_status.max_curr_consumption )
   {
      DEBUG ((EFI_D_INFO, "Invalid in querying high speed func\n"));
      return SDCC_ERR_UNKNOWN;
   }

   /* check if access mode high speed func is busy */
   if ( SDCC_BUSY_STATUS_DEF == switch_status.data_struct_vers &&
        SDCC_BUSY_HS == switch_status.funcgrp_busy_status[0] )
   {
      /* high speed func is busy */
      DEBUG ((EFI_D_INFO, "Query: High speed func is busy\n"));
      return SDCC_ERR_UNKNOWN;
   }

   /* CMD6 mode 1 to switch. Check if access mode supports high speed */
   if ( SDCC_ACCESS_MODE_HS == switch_status.funcgrp_support_func[0] &&
        SDCC_HS_ARG == switch_status.funcgrp_status[0] )
   {
      SetMem(gTempBuffer, 64, 0);
      SetMem(&switch_status, sizeof(sdcc_switch_data_type), 0);
      /* switch to high speed */
      sdcc_cmd.cmd_arg = SDCC_SWITCH_HS;
      sdcc_cmd.flags = SDCC_TRANSFER_FLAG_IS_READ_TRANSFER;
      sdcc_pdev->mem.block_mode = SDCC_MEM_BYTE_MODE;

      rc = sdcc_do_transfer( sdcc_pdev, &sdcc_cmd, gTempBuffer, 64/*bytes*/);

      sdcc_pdev->mem.block_mode = SDCC_MEM_BLK_MODE;
      if ( SDCC_NO_ERROR != rc )
      {
         DEBUG ((EFI_D_INFO, "Failed in switching to high speed\n"));
         return rc;
      }

      /* decode CMD6 switch result */
      (void) sdcc_decode_switch_data( gTempBuffer, &switch_status );

      /* check if error occur in switching */
      if ( 0 == switch_status.max_curr_consumption ||
           SDCC_HS_ARG != switch_status.funcgrp_status[0] )
      {
         DEBUG ((EFI_D_ERROR, "Failed in switching to high speed func\n"));
         return SDCC_ERR_UNKNOWN;
      }

      /* check if access mode high speed func is busy */
      if ( SDCC_BUSY_STATUS_DEF == switch_status.data_struct_vers &&
           SDCC_BUSY_HS == switch_status.funcgrp_busy_status[0] )
      {
         /* high speed func is busy */
         DEBUG ((EFI_D_ERROR, "Switch: High speed func is busy\n"));
         return SDCC_ERR_UNKNOWN;
      }

      /* latch data and command coming in on feedback clock */
      HAL_sdhci_SetUHSMode(sdcc_pdev->driveno, (UINT16)SDHCI_SDR25_MODE); 
      HAL_sdhci_HCSelectIn(sdcc_pdev->driveno, (UINT32)HAL_SDCC_CLOCK_EDGE_FEEDBACK); 

      /* Per SD2.0 spec, CMD6 function switching period is within 8 clocks */
      /* after the end bit of status data.  When CMD6 changes the bus */
      /* behavior (i.e., access mode), the host is allowed to use the new */
      /* functions (increase/decrease CLK frequency beyond the current max */
      /* CLK frequency), at least 8 clocks after at the end of the switch */
      /* command transaction. */
      sdcc_mdelay(SDCC_SD_SWITCH_CLK_WAIT_TIME_MS);
      /* set clk_freq to indicate card now runs at high speed mode */
      sdcc_pdev->mem.clk_freq = SDCC_SD_HIGH_FREQUENCY;
      /* re-program clock to high speed */
      sdcc_config_clk( sdcc_pdev, SDCC_DATA_TRANSFER_MODE);
   }

   return rc;
} /* sdcc_sd_switch_to_hs */

/******************************************************************************
* Name: sdcc_decode_switch_data
*
* Description:
*    This function is to parse out the CMD6 status data structure.
*
* Arguments:
*    data          [IN] : pointer to buffer that contains CMD6 status data
*    switch_status [OUT]: pointer to the structure to save the CMD6 status
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
BOOLEAN
sdcc_decode_switch_data
( 
   const UINT8              *data,
   sdcc_switch_data_type   *switch_status
)
{
   UINT8  value;
   INTN  i;
   UINT8  idx;

   if ( NULL == switch_status )
   {
      return FALSE;
   }

   /* in data[0] */
   value = *data;
   /* max_curr_consumption: max current consumption --> status[511:496] */
   switch_status->max_curr_consumption = (UINT16)value << 8;
   /* in data[1] */
   value = *(data + 1);
   switch_status->max_curr_consumption |= (UINT16)value;
   
   /* parse out func grps 1 to 6 info */
   idx = 2;

   for ( i = SDCC_SD_FUNC_GRP-1; i >= 0; i-- )
   {
      value = *(data + idx);
      /* funcgrp_support_func: func group i info */
      switch_status->funcgrp_support_func[i] = (UINT16)value << 8;
      idx++;
      value = *(data + idx);
      switch_status->funcgrp_support_func[i] |= (UINT16)value;
      idx++;
   }

   /* parse out func grps 1 to 6 mode 0/1 status */
   idx = 14;

   for ( i = SDCC_SD_FUNC_GRP-1; i > 0; i-- )
   {
      value = *(data + idx);
      /* funcgrp_status */
      /* mode 0 - func which can be switched in func grp i */
      /* mode 1 - result of switch cmd in func grp i */
      switch_status->funcgrp_status[i--] = (UINT8)(value >> 4);
      switch_status->funcgrp_status[i]   = (UINT8)(value & 0x0F);
      idx++;
   }

   /* in data[17] */
   value = *(data + 17);
   /* data_struct_vers: data structure version --> status[375:368] */
   switch_status->data_struct_vers = value;

   /* parse out func grps 1 to 6 busy status */
   idx = 18;

   for ( i = SDCC_SD_FUNC_GRP-1; i >= 0; i-- )
   {
      value = *(data + idx);
      /* funcgrp_busy_status: func group i busy status */
      switch_status->funcgrp_busy_status[i] = (UINT16)value << 8;
      idx++;
      value = *(data + idx);
      switch_status->funcgrp_busy_status[i] |= (UINT16)value;
      idx++;
   }

   return TRUE;
} /* sdcc_decode_switch_data */

/******************************************************************************
* Name: sdcc_get_sd_speed_class
*
* Description:
*    This function gets the SD Status which contains the speed class
*    information.  It then updates the speed class field in the
*    device structure.
*
* Arguments:
*    sdcc_pdev    [IN/OUT] : pointer to device type
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS
sdcc_get_sd_speed_class( sdcc_dev_type *sdcc_pdev )
{
   SDCC_STATUS           rc         = SDCC_NO_ERROR;
   sdcc_sd_status_type   sd_status;

   /* send ACMD13 to get the speed class */
   SetMem(&sd_status, sizeof(sdcc_sd_status_type), 0);
   rc = sdcc_get_sd_status( sdcc_pdev, &sd_status );
   if ( SDCC_NO_ERROR != rc )
   {
      return rc;
   }

   sdcc_pdev->mem.speed_class = sd_status.speed_class;

   return rc;
} /* sdcc_get_sd_speed_class */

/******************************************************************************
* Name: sdcc_set_sd_bus_width
*
* Description:
*    This function sets the SD data bus width to 4-bit mode by 
*    sending ACMD6 to the SD card.
*
* Arguments:
*    sdcc_pdev          [IN] : pointer to device type
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS
sdcc_set_sd_bus_width( sdcc_dev_type *sdcc_pdev )
{
   SDCC_STATUS      rc        = SDCC_NO_ERROR;
   sdcc_cmd_type    sdcc_cmd;
   UINT32  driveno = sdcc_pdev->driveno;

   sdcc_cmd.resp_type = SDCC_RESP_SHORT;
   sdcc_cmd.prog_scan = 0;
   sdcc_cmd.cmd       = SD_CMD55_APP_CMD;
   sdcc_cmd.cmd_arg   = SDCC_ARG_RCA(sdcc_pdev->rca);
   sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

   rc = sdcc_command(sdcc_pdev, &sdcc_cmd);
   if ( SDCC_NO_ERROR != rc )
   {
      DEBUG ((EFI_D_INFO, "Failed in SD bus width CMD55 %d\n", rc));
      return rc;
   }

   sdcc_cmd.cmd       = SD_ACMD6_SET_BUS_WIDTH;
   /* 10b represents 4 bits bus */
   sdcc_cmd.cmd_arg   = 1 << 1;
   sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

   rc = sdcc_command(sdcc_pdev, &sdcc_cmd);
   if ( SDCC_NO_ERROR != rc )
   {
      DEBUG ((EFI_D_INFO, "sdcc_set_sd_bus_width() : sdcc_command CMD6 returned with ERROR\n"));
      return rc;
   }

   rc = sdcc_hc_config_buswidth (driveno, SDCC_MMC_BUSWIDTH_4BIT);

   return rc;
} /* sdcc_set_sd_bus_width */

/******************************************************************************
* Name: sdcc_decode_sd_cid
*
* Description:
*    This function is to parse out the Card Identification (CID) register.
*
* Arguments:
*    raw_cid        [IN] : pointer to raw CID info
*    sdcc_pdev     [OUT] : pointer to device type
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
BOOLEAN
sdcc_decode_sd_cid
(
   const UINT32   *raw_cid,
   sdcc_dev_type  *sdcc_pdev
)
{
   int i;
   UINT32 prod_name;
   UINT16 mfr_date;

   if ( NULL == raw_cid || NULL == sdcc_pdev )
   {
      return FALSE;
   }

   /* manufacturer ID: CID[127:120] */
   sdcc_pdev->mem.mem_info.mfr_id = (UINT16)(raw_cid[0] >> 24);

   /* OEM/application ID: CID[119:104] */
   sdcc_pdev->mem.mem_info.oem_id = (UINT16)(raw_cid[0] >> 8);

   /* product name: CID[103:64] */
   sdcc_pdev->mem.mem_info.prod_name[0] = (UINT8)(raw_cid[0]);
   prod_name = raw_cid[1];
   for ( i = 4; i > 0; i-- )
   {
      sdcc_pdev->mem.mem_info.prod_name[i] = (UINT8)(prod_name & 0xFF);
      prod_name >>= 8;
   }
   sdcc_pdev->mem.mem_info.prod_name[5] = '\0';

   /* product revision: CID[63:56] */
   sdcc_pdev->mem.mem_info.prod_rev = (UINT8)(raw_cid[2] >> 24);

   /* product serial number: CID[55:24] */
   sdcc_pdev->mem.mem_info.prod_serial_num =
      ((raw_cid[2] & 0x00FFFFFF) << 8) | (raw_cid[3] >> 24);

   /* manufacturing date: CID[19:8] */
   mfr_date = (UINT16)((raw_cid[3] & 0x000FFF00) >> 8);
   (void)sdcc_decode_sd_mfr_date( mfr_date, sdcc_pdev );

   return TRUE;
} /* sdcc_decode_sd_cid */

/******************************************************************************
* Name: sdcc_decode_sd_mfr_date
*
* Description:
*    This function decodes the manufacturing date as mm/yyyy format.
*
* Arguments:
*    mfr_date       [IN] : raw manufacturing date
*    sdcc_pdev     [OUT] : pointer to device type
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
BOOLEAN
sdcc_decode_sd_mfr_date
(
   UINT16         mfr_date,
   sdcc_dev_type *sdcc_pdev
)
{
   (void) mfr_date;
   (void) sdcc_pdev;
   return TRUE;
} /* sdcc_decode_sd_mfr_date */

/*lint -restore */

