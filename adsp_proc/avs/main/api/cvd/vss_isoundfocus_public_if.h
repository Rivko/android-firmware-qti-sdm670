#ifndef __VSS_ISOUNDFOCUS_PUBLIC_IF_H__
#define __VSS_ISOUNDFOCUS_PUBLIC_IF_H__

/**
  @file vss_isoundfocus_public_if.h
*/
/*===========================================================================
Copyright (c) 2015 Qualcomm Technologies, Inc.  All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated by the U.S. Government,
Diversion contrary to U.S. law prohibited.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

when        who    what, where, why
-------     ---    ----------------------------------------------------------
11/24/15     sw    (Tech Pubs) Edited Doxygen comments for 8952.2.6.

===========================================================================*/

#include "apr_comdef.h"

/****************************************************************************
 * SOUND FOCUS DEFINES                                                      *
 ****************************************************************************/

/** @addtogroup cvd_sound_constants
@{ */

/** Indicates that the sector is not used. */
#define VSS_ISOUNDFOCUS_SECTOR_NOT_USED ( 0xFF )

/** Indicates that the gain step value is ignored. */
#define VSS_ISOUNDFOCUS_GAIN_STEP_NOT_USED ( 0xFFFF )

/** @} */ /* end_addtogroup cvd_sound_constants */

/****************************************************************************
 * SOUND FOCUS APIS                                                         *
 ****************************************************************************/

/** @ingroup cvd_sound_cmd_set_sectors
  Sets the sectors for the sound focus feature. 

  @apr_msgpayload{vss_isoundfocus_cmd_set_sectors_t}
  @table{weak__vss__isoundfocus__cmd__set__sectors__t}

  @detdesc
  When the device is held in the portrait position and using a clock as a
  reference:
  - Twelve o'clock represents zero degrees.
  - The angles are incremental in a counter-clockwise direction.
  - A sector is defined as the region between two boundary angles.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_ISOUNDFOCUS_CMD_SET_SECTORS ( 0x00013133 )

/* Type definition for vss_isoundfocus_cmd_set_sectors_t. */
typedef struct vss_isoundfocus_cmd_set_sectors_t vss_isoundfocus_cmd_set_sectors_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_isoundfocus_cmd_set_sectors_t
@{ */
/* Payload structure for the VSS_ISOUNDFOCUS_CMD_SET_SECTORS command.
*/
struct vss_isoundfocus_cmd_set_sectors_t
{
  uint16_t start_angles[ 8 ];
    /**< Array of boundary angles (in degrees) that form the sectors to
         configure.

         @values Integer between 0 and 359
         - Values in this array must be strictly increasing
         - Values for unused sectors are ignored

         The maximum number of supported sectors is eight. The client can
         configure less than eight sectors.

         With the exception of the last sector, sector n (where 
         n = 1,2,3,...,7)is defined as the region from start_angles[n-1] to
         start_angles[n].

         If sector N is the last sector to be configured (where
         N = 1,2,3,...,8), it is defined as the region from start_angles[N-1]
         to start_angles[0] (if N = 1, the sector represents the entire
         360 degree plane). */

  uint8_t enables[ 8 ];
    /**< Array of enable flags that correspond to each sector.

         @values
         - TRUE -- Sector is enabled
         - FALSE -- Sector is disabled
         - #VSS_ISOUNDFOCUS_SECTOR_NOT_USED

         The maximum number of supported sectors is eight. The client can
         configure less than eight sectors. If the maximim number is less than
         eight, the client uses #VSS_ISOUNDFOCUS_SECTOR_NOT_USED to indicate
         unused sectors. */

  uint16_t gain_step;
    /**< Gain step value to be applied to all enabled sectors.

         @values
         - Integer that lies in the supported range, as defined by the voice
           Tx path calibration database
         - #VSS_ISOUNDFOCUS_GAIN_STEP_NOT_USED -- Gain adjustment is ignored.

         Use of the gain step to change the volume on Tx (uplink) path must be
         restricted with the sound focus feature. It must not be used for
         standalone voice calls.

         The client must ensure that the gain step does not lie outside of the
         supported range, as defined by the voice Tx path calibration database. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_isoundfocus_cmd_set_sectors_t */

/** @ingroup cvd_sound_cmd_get_sectors
  Gets information for the sectors.

  @apr_msgpayload
  None.

  @detdesc
  Default values are returned if this command is called before 
  #VSS_ISOUNDFOCUS_CMD_SET_SECTORS has been successfully processed, or if the 
  sound focus algorithm has not started yet.
   
  @return
  #VSS_ISOUNDFOCUS_RSP_GET_SECTORS - Success. Sector information was retrieved.
  @par
  APRV2_IBASIC_RSP_RESULT - Failure. Sector information was not retrieved.
  (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}.)

  @dependencies
  None.
*/
#define VSS_ISOUNDFOCUS_CMD_GET_SECTORS ( 0x00013134 )

/** @ingroup cvd_sound_rsp_get_sectors
  Response to the #VSS_ISOUNDFOCUS_CMD_GET_SECTORS command.

  @apr_msgpayload{vss_isoundfocus_rsp_get_sectors_t}
  @table{weak__vss__isoundfocus__rsp__get__sectors__t}

  @detdesc
  The response includes sector boundary angles, enable flag for each sector,
  and gain step.

  @return
  None.

  @dependencies
  None.
*/
#define VSS_ISOUNDFOCUS_RSP_GET_SECTORS ( 0x00013135 )

/* Type definition for vss_isoundfocus_rsp_get_sectors_t. */
typedef struct vss_isoundfocus_rsp_get_sectors_t vss_isoundfocus_rsp_get_sectors_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_isoundfocus_rsp_get_sectors_t
@{ */
/* Payload of the VSS_ISOUNDFOCUS_RSP_GET_SECTORS response.
*/
struct vss_isoundfocus_rsp_get_sectors_t
{
  uint16_t start_angles[ 8 ];
    /**< Array of boundary angles (in degrees) that form the sectors.

         @values Integer between 0 and 359
         - Values in this array must be increasing
         - Values for unused sectors are ignored

         By default, all sectors are enabled, in which case values for angles
         are ignored.

         With the exception of the last sector, sector n (where 
         n = 1,2,3,...,7)is defined as the region from start_angles[n-1] to
         start_angles[n].

         If sector N is the last sector to be configured (where
         N = 1,2,3,...,8), it is defined as the region from start_angles[N-1]
         to start_angles[0] (if N = 1, the sector represents the entire
         360 degree plane). */

  uint8_t enables[ 8 ];
    /**< Array of enable flags that correspond to each sector.

         @values
         - TRUE -- Sector is enabled (by default, all sectors are enabled)
         - FALSE -- Sector is disabled
         - #VSS_ISOUNDFOCUS_SECTOR_NOT_USED @tablebulletend */

  uint16_t gain_step;
    /**< Gain step value that is applied to all enabled sectors.

         @values
         - Integer that lies in the supported range, as defined by the voice
           Tx path calibration database
         - #VSS_ISOUNDFOCUS_GAIN_STEP_NOT_USED -- Gain adjustment is ignored.
          (Default) @tablebulletend */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_isoundfocus_rsp_get_sectors_t */

#endif /* __VSS_ISOUNDFOCUS_PUBLIC_IF_H__ */


