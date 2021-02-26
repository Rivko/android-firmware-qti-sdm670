#ifndef QSEE_CFG_PROP_H
#define QSEE_CFG_PROP_H

/**
 * Copyright 2015 QUALCOMM Technologies, Inc. 
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 */

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $DateTime: 2018/02/06 03:00:17 $
  $Author: pwbldsvc $

  when       who      what, where, why
  --------   ---      ------------------------------------
  02/02/15   cz       Initial version

===========================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stdint.h>


/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef enum
{
    QSEE_CFG_SUCCESS                   = 0x00,
    QSEE_CFG_INVALID_INPUT             = 0x01,
    QSEE_CFG_BUFFER_TOO_SMALL          = 0x02,
    QSEE_CFG_ERROR_GET_PROP_HDLSTR     = 0x03,
    QSEE_CFG_ERROR_GET_PROP_VALUE      = 0x04,
    QSEE_CFG_ERROR_MEMSCPY             = 0x05,
    QSEE_CFG_UNKNOWN_TYPE              = 0x06,
    
    QSEE_CFG_MAX_ERROR_CODE     = 0x7FFFFFFF
} qsee_cfg_error;

typedef struct
{
    uint32_t dwType;
    uint32_t dwLen;  /* in bytes */
    uint8_t  val[1]; /* If dwLen > 1, the left variables followed by val[0] */
}  qsee_cfg_propvar_t;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
 
/**
 * @brief the configuration value is fetched from TZ kernel via SYS CALL
 *        The current restriction is that qsee app should not use the same prop name as specified in commonlib.
 *        The reason is that commonlib as a static library is statically linked to each tzapp. there is no way in 
 *        tz kernel to find the "commonlib" name.
 *
 * @param[in]    PropName     point to Property Name (a string)
 * @param[in]    PropNameLen  The length of the PropName size. It does not include '\0'. It should be less than 64.
 * @param[in]    PropID       Property ID
 * @param[out]   pPropBuf     output buffer that populates DAL configuration value
 *                            the populated buffer need to cast to the qsee_cfg_propvar_t type
 * @param[in]    PropBufSz    the output buffer size in bytes
 * @param[out]   PropBufSzRet the actual populated buffer size in bytes
 *
 * @return zero on success, non-zero on failure
*/
qsee_cfg_error qsee_cfg_getpropval
(
    const char               *PropName,
    uint32_t                  PropNameLen,
    uint32_t                  PropId,
    qsee_cfg_propvar_t       *pPropBuf,
    uint32_t                  PropBufSz,
    uint32_t                 *PropBufSzRet
);

#endif //QSEE_CFG_PROP_H

