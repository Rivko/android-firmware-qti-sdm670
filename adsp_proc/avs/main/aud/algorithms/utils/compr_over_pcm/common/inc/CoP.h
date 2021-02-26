/*========================================================================*/
/**
@file CoP.h

@brief Header for Compressed Over PCM packetization scheme.

*/



/*========================================================================
Copyright (c) 2016-2018 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/algorithms/utils/compr_over_pcm/common/inc/CoP.h#2 $

when			who				what, where, why
--------		---				-------------------------------------------------------
03/28/2016		rbhatnk			Created file.
========================================================================== */

#ifndef _COP_H_
#define _COP_H_

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


#include "mmdefs.h"

#define COP_WORD_SIZE_BYTES (sizeof(uint16_t))

/* First sync word used to recognize a frame in compressed-over-PCM packetization. */
static const uint16_t COP_SYNC_WORD1 = 0xF782;
/* Second sync word used to recognize a frame in compressed-over-PCM packetization. */
static const uint16_t COP_SYNC_WORD2 = 0x41EF;

/* Mask for the version.
   Bits 0 & 1 of header word 3 represent version of the packetization scheme. 
   Updated whenever header structure, or meaning changes. 
   With new version, header size may change.
   
   */
#define COP_HEADER_VERSION_MASK (0x3)
/**
 * Bit shift for the version.
 */
#define COP_HEADER_VERSION_SHIFT (0)

/** Bit mask for the error-bit. Bit 2.
 * 1 -> no error. 0 -> error occurred or don’t know if error occurred
 */
#define COP_HEADER_ERROR_BIT_MASK (0x4)
/**
 * Bit shift for the error bit.
 */
#define COP_HEADER_ERROR_BIT_SHIFT (2)

/**
 * Bit mask for the data format ID.
 * Bit 3-8: 6 bits.
 */
#define COP_HEADER_DATA_FORMAT_ID_MASK (0x1F8)

/**
 * Bit shift for the data format ID.
 */
#define COP_HEADER_DATA_FORMAT_ID_SHIFT (3)

/**
 * Bit mask for indicating whether the previous frame's size was intact.
 * i.e. previous frame is a nominal frame with no additional zero stuffing or
 * zero removal done to adjust the clock skew.
 *
 * Clock skew detection is done by comparing the nominal frames with the other frames.
 *
 * Nominal frame length = PCM frame length * number of subframes in the COP frame.
 *
 * bit 9: 1 bit.
 *
 * 1 -> previous frame size value was intact.
 * 0 -> a) previous frame size was changed to adjust clock skew (at least once this bit must be 1 to kick in clock skew detection/correction).
 *      b) it's not known whether previous frame size is intact or not.
 *      c) if packetizer doesn't support clock skew reporting through zero insertion/removal.
 *
 */
#define COP_HEADER_DATA_PREV_FRAME_SIZE_INTACT_MASK  0x200

/**
 * Bit shift for 'previous-frame-size-intact' flag.
 */
#define COP_HEADER_DATA_PREV_FRAME_SIZE_INTACT_SHIFT  (9)


/**
 * Bit mask indicating whether the subframe length is in bits or bytes.
 *
 * Bit 10: 1 bit
 *
 * 1 -> subframe length is in bits
 * 0 -> subframe length is in bytes (default)
 */
#define COP_HEADER_DATA_SUBFRAME_LENGTH_IN_BITS_MASK  0x400

/**
 * Bit shift for 'subframe-length-in-bits' flag
 */
#define COP_HEADER_DATA_SUBFRAME_LENGTH_IN_BITS_SHIFT (10)

/**
 * Bit mask indicating whether scrambler is enabled or not.
 *
 * Bit 15: 1 bit
 *
 * 1 -> scrambler enable
 * 0 -> scrambler disable (default)
 */
#define COP_HEADER_DATA_SCRAMBLER_ENABLE_MASK  0x8000

/**
 * Bit shift for scrambler flag
 */
#define COP_HEADER_SCRAMBLER_ENABLE_SHIFT (15)

/**
 * Bit mask for the bit rate level
 * Bit 11-13: 3 bits.
 */
#define COP_HEADER_DATA_BIT_RATE_LEVEL_MASK 0x3800

/**
 * Bit shift for bit rate level
 */
#define COP_HEADER_DATA_BIT_RATE_LEVEL_SHIFT (11)

/**
 * Bit rate level used in the compressed-over-PCM packetization
 */
typedef enum cop_bit_rate_level_t
{
	COP_QUALITY_LEVEL_DONT_CARE = 0,
	COP_QUALITY_LEVEL_1,
	COP_QUALITY_LEVEL_2,
	COP_QUALITY_LEVEL_3,
	COP_QUALITY_LEVEL_4,
	COP_QUALITY_LEVEL_5,
	//COP_QUALITY_LEVEL_6 = 110,
	//COP_QUALITY_LEVEL_7 = 111,
}cop_bit_rate_level_t;

/**
 * Data format ID used in the compressed-over-PCM packetization

 */
typedef enum cop_data_format_id_t
{
	COP_DATA_FMT_ID_NULL = 0x00,	/** Used for idle state */
	COP_DATA_FMT_ID_PAUSE = 0x01,   /** Used for Pause state */
	COP_DATA_FMT_ID_SBC = 0x02,     /** Used for SBC bit stream */
	COP_DATA_FMT_ID_AAC = 0x03,     /** Used for AAC bit stream */
	COP_DATA_FMT_ID_APTX = 0x04,     /** Used for APTX bit stream */
	COP_DATA_FMT_ID_APTX_HD = 0x05,     /** Used for APTX-HD bit stream */
	/*Rest of the values are reserved for future use. */
	COP_DATA_FMT_ID_UNSET = 0x3F    /** Used when format-ID is not known. This is not necessarily an error. 
										It's not always necessary to know the format-ID. */

} cop_data_format_id_t;

/**
 * Structure of a subframe in compressed-over-PCM packetized frame.
 */
typedef struct cop_subframe_t
{
	uint16_t subframe_length;		/* Length of a subframe. greater than or equal to zero. 
									   Length is in bytes/bits depending on 'subframe-length-in-bits' flag.
									   The length doesn’t include padding used for achieve 16bit alignment. 
									   Doesn’t include the size of this field 'subframe_length'.
									   Bit 15 will be 1 if scrambler is enabled. */
#if 0
	uint16_t  subframe_data[rounded_subframe_length_words]; 
				/* the actual data that's packetized. 
				   the data corresponds to one encoded frame.
				   When rounded_subframe_length_words is not an integral multiple of subframe_length,
				   the data added for alignment must be set to zero.
				   If subframe_length is in bytes, then rounded_subframe_length_words = (subframe_length+1)/2.
				   If subframe_length is in bits, then rounded_subframe_length_words = (subframe_length+15)/16 */
#endif
} cop_subframe_t;

/**
 * Structure of the compressed-over-PCM packetized frame.
 */
typedef struct cop_packet_t
{
	uint16_t sync_word1;		/** COP_SYNC_WORD1 */
	uint16_t sync_word2;		/** COP_SYNC_WORD2 */
	uint16_t header_metadata;	/** Version, error-bit, data format-ID, is-frame-size-intact, frame-length-in-bits.
	                                Unused bits must be set to zero. Bit 15 is set to 1 if scrambler is enabled*/
	uint16_t num_sub_frames;	/** Number of subframes in this feame. greater than or equal to zero. 
								    If there’s only one encoded frame in a packet, then num_sub_frames = 1. For pause/NULL packets, num_sub_frames=0.
									Multiple subframes will be in a packet only when encoder frame size is very small 
									(frame is <= 1 ms -> this is implementation dependent. 
									Depacketizer must be agnostic of this).
								*/
#if 0
	cop_subframe_t subframe[num_sub_frames];	/** if num_sub_frames is nonzero, then cop_subframe_t structure is present here. */
#endif
	uint16_t zero_pad_length;	/** length of zero padding in bytes. greater than or equal to zero.
	                                Bit 15 is set to 1, if scrambler is enabled */
#if 0
	uint16_t zero_padding[ (zero_pad_length + 1) / 2];	/** if nonzero zero-padding is present, then zero bytes are present here */
#endif
} cop_packet_t;

#ifdef __cplusplus
}
#endif //__cplusplus

#endif /** _COP_H_ */
