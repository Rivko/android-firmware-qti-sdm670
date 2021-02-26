#ifndef _SECURITY_STREAM_CHECKER_H_
#define _SECURITY_STREAM_CHECKER_H_

/*===========================================================================
   Copyright (c) 2011-2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
   Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/24/14    ck     V6.0 Added AAC-ADTS filter.
11/06/13    rk     Added error codes for invalid start bits and IV check failure
05/09/13    ck     SCHK dxhdcp2 support. Independent lib, Added callback reg function to remove config dependency.
                   Removed operation parameter. Added log for debugging. Added datarate check.
03/20/13    ck     Created.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <comdef.h>


#define IN
#define OUT
#define INOUT


#define TZAPP_SCHK_PLAYREADY				1
#define TZAPP_SCHK_WIDEVINE					2
#define TZAPP_SCHK_DXHDCPV2					3	// Number fixed for DX.
#define TZAPP_SCHK_APP_END					3	// Update to the last app number.

#define TZAPP_SCHK_AES128_CTR				0x0001
#define TZAPP_SCHK_AES128_CBC				0x0002

#define TZAPP_SCHK_STREAM_VIDEO				0x0001
#define TZAPP_SCHK_STREAM_AUDIO				0x0002

#define TZAPP_SCHK_CODEC_AAC				1
#define TZAPP_SCHK_CODEC_AC3				2
#define TZAPP_SCHK_CODEC_LPCM				3


#define TZAPP_SCHK_CHECK_FSIZE				0x00000001
#define TZAPP_SCHK_CHECK_FCONTINUITY		0x00000002
#define TZAPP_SCHK_CHECK_FSTART_INVALID_A	0x00000004
#define TZAPP_SCHK_CHECK_FSTART_VALID_A		0x00000008
#define TZAPP_SCHK_CHECK_IV_CONSTRAINT		0x00000010
#define TZAPP_SCHK_CHECK_DATARATE			0x00000020
#define TZAPP_SCHK_CHECK_PCM_REGULATOR		0x00000040
#define TZAPP_SCHK_CHECK_DX_RESERVED		0x00000100
#define TZAPP_SCHK_CHECK_ALL				0xFFFFFFFF


#define TZAPP_SCHK_FSIZE_AUDIO_LIMIT				1024*3
#define TZAPP_SCHK_SAVABLE_DATABAND_MS_LPCM			250
#define TZAPP_SCHK_MAX_SAVABLE_DURATION_MS_WFD		2000
#define TZAPP_SCHK_SAVABLE_DATABAND_MS_A			100
#define TZAPP_SCHK_MAX_SAVABLE_DURATION_MS_A		8000


#define TZAPP_SCHK_R_OK								0x0000
#define TZAPP_SCHK_R_INVALID_PARAM					0x0001
#define TZAPP_SCHK_R_NOT_IMPLEMENTED				0x0002
#define TZAPP_SCHK_R_STREAM_NOT_ALLOWED				0x0100
#define TZAPP_SCHK_R_IV_CONSTRAINT_FAIL             0x0101



typedef struct schk_callback {
	int32(* oem_disable_security_stream_check)(uint32 application);
	int32(* oem_get_IV_constraint)(
			uint32		application,
		    uint32		crypto_mode,
			uint8		*oem_mask,
			uint8		*oem_pattern,
			int32		*allow);
} schk_callback;


/* Do not share this function details and algorithms to customers.
 * Please contact dev team if it is requested */

const uint8* security_stream_checker_get_version(
	IN	int32 reserved	// Pass 0
);

uint32 security_stream_checker_register_callback(
	IN	schk_callback	*callback
);

uint32 security_stream_checker_init(
    IN	uint32			caller_application,
    IN	uint32			crypto_mode
);

uint32 security_stream_checker(
	IN	uint32			caller_application,
    IN	uint32			crypto_mode,
    IN	uint32			stream_type,
    IN	uint8			*data,
    IN	uint32			data_length,
    // 64bit for AES CTR. 128bit for AES CBC.
    IN	uint8			*vector1,
    // 64bit for AES CTR. NULL for AES CBC.
    IN	uint8			*vector2
);

#endif /* _SECURITY_STREAM_CHECKER_H_ */
