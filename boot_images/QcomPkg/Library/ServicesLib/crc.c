/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        C R C    S E R V I C E S

GENERAL DESCRIPTION
  This module contains functions for calculating the various software
  generated CRCs specified by the CDMA Common Air Interface (CAI).

REFERENCES
  CRC-30, CAI Section 6.7.1.2.2, 7.7.1.2.2, 7.7.2.2.2
  CRC-16, CAI Section 6.7.2.2.2, 7.7.3.2.2

EXTERNALIZED FUNCTIONS
  crc16_calc - calculates the 16-bit CRC used for signalling messages on
    the traffic channel.

  crc16_l_calc - LSB-first CRC CCITT 16-bit CRC

  crc30_calc - calculates the 30-bit CRC used for messages on the sync
    and paging channels.

  crc16_step - calculate a step-by-step 16-bit CRC over disjunct data.

  crc32_calc -  Calculates the 32-bit CRC used for inner FCS of the 
    packets transmitted on the access channel.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 1990-1992 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 1996-1999 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2005      by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: O:/src/asw/COMMON/vcs/crc.c_v   1.2   28 Aug 2002 14:35:48   ldefauw  $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/05/07   enj     ASSERT that buf_ptr's are not NULL
04/06/05   ss      Moved 32-bit CRC code from hdrcrc.c to this file after
                   appropriate renaming.
11/29/01   lad     Added step-wise CRC30 calculation.
04/09/99   ms      Lint cleanup.
03/17/99   ms      Changes for Virtual Boot Block.
                      Removed bb_ prefixes
                      Not includingbbjmpout.h
12/16/98   jct     Removed 80186 support
09/28/98   jct     Incorporate ARM changes
04/08/96   dna     Added comment to each func. saying it is in the Boot Block
02/28/96   dna     Merged crctab.c into this file for Boot Block because the
                   external references to the tables caused a problem.
                   Prepended bb_ to function and table names to move this 
                   file into the Boot Block.
06/21/93   jah     Fixed bug in crc_16_l_calc().
06/07/93   jah     Added crc_16_l_calc().
04/20/93   jah     Added the table for an LSB-first CCITT-16 CRC.
07/09/92   jah     Changed crc_16_step() from bits to bytes
07/09/92   jah     Removed CRC generation program
06/21/92   jah     Created crc tables
06/18/92   jah     Ported from brassboard and converted to table-driven.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"             /* Definitions for byte, word, etc.     */
#include "crc.h"                /* CRC typedefs and definitions         */
#include "assert.h"


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* The CRC table size is based on how many bits at a time we are going
** to process through the table.  Given that we are processing the data
** 8 bits at a time, this gives us 2^8 (256) entries.
*/
#define CRC_TAB_SIZE    256             /* 2^CRC_TAB_BITS      */

/* CRC table for 30 bit CRC, with generator polynomial 0x6030b9c7,
** calculated 8 bits at a time, MSB first.
*/
const dword crc30_table[ CRC_TAB_SIZE ] = {
  0x00000000, 0x2030b9c7, 0x2051ca49, 0x0061738e,
  0x20932d55, 0x00a39492, 0x00c2e71c, 0x20f25edb,
  0x2116e36d, 0x01265aaa, 0x01472924, 0x217790e3,
  0x0185ce38, 0x21b577ff, 0x21d40471, 0x01e4bdb6,
  0x221d7f1d, 0x022dc6da, 0x024cb554, 0x227c0c93,
  0x028e5248, 0x22beeb8f, 0x22df9801, 0x02ef21c6,
  0x030b9c70, 0x233b25b7, 0x235a5639, 0x036aeffe,
  0x2398b125, 0x03a808e2, 0x03c97b6c, 0x23f9c2ab,
  0x240a47fd, 0x043afe3a, 0x045b8db4, 0x246b3473,
  0x04996aa8, 0x24a9d36f, 0x24c8a0e1, 0x04f81926,
  0x051ca490, 0x252c1d57, 0x254d6ed9, 0x057dd71e,
  0x258f89c5, 0x05bf3002, 0x05de438c, 0x25eefa4b,
  0x061738e0, 0x26278127, 0x2646f2a9, 0x06764b6e,
  0x268415b5, 0x06b4ac72, 0x06d5dffc, 0x26e5663b,
  0x2701db8d, 0x0731624a, 0x075011c4, 0x2760a803,
  0x0792f6d8, 0x27a24f1f, 0x27c33c91, 0x07f38556,
  0x2824363d, 0x08148ffa, 0x0875fc74, 0x284545b3,
  0x08b71b68, 0x2887a2af, 0x28e6d121, 0x08d668e6,
  0x0932d550, 0x29026c97, 0x29631f19, 0x0953a6de,
  0x29a1f805, 0x099141c2, 0x09f0324c, 0x29c08b8b,
  0x0a394920, 0x2a09f0e7, 0x2a688369, 0x0a583aae,
  0x2aaa6475, 0x0a9addb2, 0x0afbae3c, 0x2acb17fb,
  0x2b2faa4d, 0x0b1f138a, 0x0b7e6004, 0x2b4ed9c3,
  0x0bbc8718, 0x2b8c3edf, 0x2bed4d51, 0x0bddf496,
  0x0c2e71c0, 0x2c1ec807, 0x2c7fbb89, 0x0c4f024e,
  0x2cbd5c95, 0x0c8de552, 0x0cec96dc, 0x2cdc2f1b,
  0x2d3892ad, 0x0d082b6a, 0x0d6958e4, 0x2d59e123,
  0x0dabbff8, 0x2d9b063f, 0x2dfa75b1, 0x0dcacc76,
  0x2e330edd, 0x0e03b71a, 0x0e62c494, 0x2e527d53,
  0x0ea02388, 0x2e909a4f, 0x2ef1e9c1, 0x0ec15006,
  0x0f25edb0, 0x2f155477, 0x2f7427f9, 0x0f449e3e,
  0x2fb6c0e5, 0x0f867922, 0x0fe70aac, 0x2fd7b36b,
  0x3078d5bd, 0x10486c7a, 0x10291ff4, 0x3019a633,
  0x10ebf8e8, 0x30db412f, 0x30ba32a1, 0x108a8b66,
  0x116e36d0, 0x315e8f17, 0x313ffc99, 0x110f455e,
  0x31fd1b85, 0x11cda242, 0x11acd1cc, 0x319c680b,
  0x1265aaa0, 0x32551367, 0x323460e9, 0x1204d92e,
  0x32f687f5, 0x12c63e32, 0x12a74dbc, 0x3297f47b,
  0x337349cd, 0x1343f00a, 0x13228384, 0x33123a43,
  0x13e06498, 0x33d0dd5f, 0x33b1aed1, 0x13811716,
  0x14729240, 0x34422b87, 0x34235809, 0x1413e1ce,
  0x34e1bf15, 0x14d106d2, 0x14b0755c, 0x3480cc9b,
  0x3564712d, 0x1554c8ea, 0x1535bb64, 0x350502a3,
  0x15f75c78, 0x35c7e5bf, 0x35a69631, 0x15962ff6,
  0x366fed5d, 0x165f549a, 0x163e2714, 0x360e9ed3,
  0x16fcc008, 0x36cc79cf, 0x36ad0a41, 0x169db386,
  0x17790e30, 0x3749b7f7, 0x3728c479, 0x17187dbe,
  0x37ea2365, 0x17da9aa2, 0x17bbe92c, 0x378b50eb,
  0x185ce380, 0x386c5a47, 0x380d29c9, 0x183d900e,
  0x38cfced5, 0x18ff7712, 0x189e049c, 0x38aebd5b,
  0x394a00ed, 0x197ab92a, 0x191bcaa4, 0x392b7363,
  0x19d92db8, 0x39e9947f, 0x3988e7f1, 0x19b85e36,
  0x3a419c9d, 0x1a71255a, 0x1a1056d4, 0x3a20ef13,
  0x1ad2b1c8, 0x3ae2080f, 0x3a837b81, 0x1ab3c246,
  0x1b577ff0, 0x3b67c637, 0x3b06b5b9, 0x1b360c7e,
  0x3bc452a5, 0x1bf4eb62, 0x1b9598ec, 0x3ba5212b,
  0x3c56a47d, 0x1c661dba, 0x1c076e34, 0x3c37d7f3,
  0x1cc58928, 0x3cf530ef, 0x3c944361, 0x1ca4faa6,
  0x1d404710, 0x3d70fed7, 0x3d118d59, 0x1d21349e,
  0x3dd36a45, 0x1de3d382, 0x1d82a00c, 0x3db219cb,
  0x1e4bdb60, 0x3e7b62a7, 0x3e1a1129, 0x1e2aa8ee,
  0x3ed8f635, 0x1ee84ff2, 0x1e893c7c, 0x3eb985bb,
  0x3f5d380d, 0x1f6d81ca, 0x1f0cf244, 0x3f3c4b83,
  0x1fce1558, 0x3ffeac9f, 0x3f9fdf11, 0x1faf66d6
};

/* CRC table for 16 bit CRC, with generator polynomial 0x1021,
** calculated 8 bits at a time, MSB first.
*/
const word crc16_table[ CRC_TAB_SIZE ] = {
  0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
  0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
  0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
  0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
  0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
  0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
  0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
  0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
  0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
  0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
  0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
  0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
  0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
  0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
  0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
  0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
  0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
  0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
  0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
  0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
  0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
  0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
  0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
  0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
  0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
  0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
  0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
  0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
  0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
  0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
  0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
  0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};

/* CRC table for 16 bit CRC, with generator polynomial 0x8408,
** calculated 8 bits at a time, LSB first.  This table is used
** from a macro in sio.c.
*/
const word crc_16_l_table[ CRC_TAB_SIZE ] = {
  0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
  0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
  0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
  0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
  0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
  0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
  0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
  0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
  0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
  0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
  0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
  0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
  0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
  0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
  0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
  0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
  0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
  0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
  0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
  0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
  0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
  0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
  0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
  0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
  0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
  0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
  0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
  0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
  0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
  0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
  0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
  0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

/* CRC table for 32 bit CRC with generator polynomial 0x04c11db7,
** calculated 8 bits at a time, MSB first.
*/
const uint32 crc32_table[ CRC_TAB_SIZE ] = {  
  0x00000000UL,  0x04c11db7UL,  0x09823b6eUL,  0x0d4326d9UL,
  0x130476dcUL,  0x17c56b6bUL,  0x1a864db2UL,  0x1e475005UL,
  0x2608edb8UL,  0x22c9f00fUL,  0x2f8ad6d6UL,  0x2b4bcb61UL,
  0x350c9b64UL,  0x31cd86d3UL,  0x3c8ea00aUL,  0x384fbdbdUL,
  0x4c11db70UL,  0x48d0c6c7UL,  0x4593e01eUL,  0x4152fda9UL,
  0x5f15adacUL,  0x5bd4b01bUL,  0x569796c2UL,  0x52568b75UL,
  0x6a1936c8UL,  0x6ed82b7fUL,  0x639b0da6UL,  0x675a1011UL,
  0x791d4014UL,  0x7ddc5da3UL,  0x709f7b7aUL,  0x745e66cdUL,
  0x9823b6e0UL,  0x9ce2ab57UL,  0x91a18d8eUL,  0x95609039UL,
  0x8b27c03cUL,  0x8fe6dd8bUL,  0x82a5fb52UL,  0x8664e6e5UL,
  0xbe2b5b58UL,  0xbaea46efUL,  0xb7a96036UL,  0xb3687d81UL,
  0xad2f2d84UL,  0xa9ee3033UL,  0xa4ad16eaUL,  0xa06c0b5dUL,
  0xd4326d90UL,  0xd0f37027UL,  0xddb056feUL,  0xd9714b49UL,
  0xc7361b4cUL,  0xc3f706fbUL,  0xceb42022UL,  0xca753d95UL,
  0xf23a8028UL,  0xf6fb9d9fUL,  0xfbb8bb46UL,  0xff79a6f1UL,
  0xe13ef6f4UL,  0xe5ffeb43UL,  0xe8bccd9aUL,  0xec7dd02dUL,
  0x34867077UL,  0x30476dc0UL,  0x3d044b19UL,  0x39c556aeUL,
  0x278206abUL,  0x23431b1cUL,  0x2e003dc5UL,  0x2ac12072UL,
  0x128e9dcfUL,  0x164f8078UL,  0x1b0ca6a1UL,  0x1fcdbb16UL,
  0x018aeb13UL,  0x054bf6a4UL,  0x0808d07dUL,  0x0cc9cdcaUL,
  0x7897ab07UL,  0x7c56b6b0UL,  0x71159069UL,  0x75d48ddeUL,
  0x6b93dddbUL,  0x6f52c06cUL,  0x6211e6b5UL,  0x66d0fb02UL,
  0x5e9f46bfUL,  0x5a5e5b08UL,  0x571d7dd1UL,  0x53dc6066UL,
  0x4d9b3063UL,  0x495a2dd4UL,  0x44190b0dUL,  0x40d816baUL,
  0xaca5c697UL,  0xa864db20UL,  0xa527fdf9UL,  0xa1e6e04eUL,
  0xbfa1b04bUL,  0xbb60adfcUL,  0xb6238b25UL,  0xb2e29692UL,
  0x8aad2b2fUL,  0x8e6c3698UL,  0x832f1041UL,  0x87ee0df6UL,
  0x99a95df3UL,  0x9d684044UL,  0x902b669dUL,  0x94ea7b2aUL,
  0xe0b41de7UL,  0xe4750050UL,  0xe9362689UL,  0xedf73b3eUL,
  0xf3b06b3bUL,  0xf771768cUL,  0xfa325055UL,  0xfef34de2UL,
  0xc6bcf05fUL,  0xc27dede8UL,  0xcf3ecb31UL,  0xcbffd686UL,
  0xd5b88683UL,  0xd1799b34UL,  0xdc3abdedUL,  0xd8fba05aUL,
  0x690ce0eeUL,  0x6dcdfd59UL,  0x608edb80UL,  0x644fc637UL,
  0x7a089632UL,  0x7ec98b85UL,  0x738aad5cUL,  0x774bb0ebUL,
  0x4f040d56UL,  0x4bc510e1UL,  0x46863638UL,  0x42472b8fUL,
  0x5c007b8aUL,  0x58c1663dUL,  0x558240e4UL,  0x51435d53UL,
  0x251d3b9eUL,  0x21dc2629UL,  0x2c9f00f0UL,  0x285e1d47UL,
  0x36194d42UL,  0x32d850f5UL,  0x3f9b762cUL,  0x3b5a6b9bUL,
  0x0315d626UL,  0x07d4cb91UL,  0x0a97ed48UL,  0x0e56f0ffUL,
  0x1011a0faUL,  0x14d0bd4dUL,  0x19939b94UL,  0x1d528623UL,
  0xf12f560eUL,  0xf5ee4bb9UL,  0xf8ad6d60UL,  0xfc6c70d7UL,
  0xe22b20d2UL,  0xe6ea3d65UL,  0xeba91bbcUL,  0xef68060bUL,
  0xd727bbb6UL,  0xd3e6a601UL,  0xdea580d8UL,  0xda649d6fUL,
  0xc423cd6aUL,  0xc0e2d0ddUL,  0xcda1f604UL,  0xc960ebb3UL,
  0xbd3e8d7eUL,  0xb9ff90c9UL,  0xb4bcb610UL,  0xb07daba7UL,
  0xae3afba2UL,  0xaafbe615UL,  0xa7b8c0ccUL,  0xa379dd7bUL,
  0x9b3660c6UL,  0x9ff77d71UL,  0x92b45ba8UL,  0x9675461fUL,
  0x8832161aUL,  0x8cf30badUL,  0x81b02d74UL,  0x857130c3UL,
  0x5d8a9099UL,  0x594b8d2eUL,  0x5408abf7UL,  0x50c9b640UL,
  0x4e8ee645UL,  0x4a4ffbf2UL,  0x470cdd2bUL,  0x43cdc09cUL,
  0x7b827d21UL,  0x7f436096UL,  0x7200464fUL,  0x76c15bf8UL,
  0x68860bfdUL,  0x6c47164aUL,  0x61043093UL,  0x65c52d24UL,
  0x119b4be9UL,  0x155a565eUL,  0x18197087UL,  0x1cd86d30UL,
  0x029f3d35UL,  0x065e2082UL,  0x0b1d065bUL,  0x0fdc1becUL,
  0x3793a651UL,  0x3352bbe6UL,  0x3e119d3fUL,  0x3ad08088UL,
  0x2497d08dUL,  0x2056cd3aUL,  0x2d15ebe3UL,  0x29d4f654UL,
  0xc5a92679UL,  0xc1683bceUL,  0xcc2b1d17UL,  0xc8ea00a0UL,
  0xd6ad50a5UL,  0xd26c4d12UL,  0xdf2f6bcbUL,  0xdbee767cUL,
  0xe3a1cbc1UL,  0xe760d676UL,  0xea23f0afUL,  0xeee2ed18UL,
  0xf0a5bd1dUL,  0xf464a0aaUL,  0xf9278673UL,  0xfde69bc4UL,
  0x89b8fd09UL,  0x8d79e0beUL,  0x803ac667UL,  0x84fbdbd0UL,
  0x9abc8bd5UL,  0x9e7d9662UL,  0x933eb0bbUL,  0x97ffad0cUL,
  0xafb010b1UL,  0xab710d06UL,  0xa6322bdfUL,  0xa2f33668UL,
  0xbcb4666dUL,  0xb8757bdaUL,  0xb5365d03UL,  0xb1f740b4UL
};


/*===========================================================================

FUNCTION CRC_16_CALC

DESCRIPTION
  This function calculates a 16-bit CRC over a specified number of data
  bits.  It can be used to produce a CRC and to check a CRC.

DEPENDENCIES
  None

RETURN VALUE
  Returns a word holding 16 bits which are the contents of the CRC
  register as calculated over the specified data bits.  If this
  function is being used to check a CRC, then the return value will be
  equal to CRC_16_OK (defined in crc.h) if the CRC checks correctly.

SIDE EFFECTS
  None

===========================================================================*/

word crc_16_calc
(
  byte *buf_ptr,
    /* Pointer to bytes containing the data to CRC.  The bit stream starts
    ** in the MS bit of the first byte.
    */

  word len
    /* Number of data bits to calculate the CRC over */
)
{
  word data, crc_16;

  ASSERT( buf_ptr != NULL );
  
  /* Generate a CRC-16 by looking up the transformation in a table and
  ** XOR-ing it into the CRC, one byte at a time.
  */
  for (crc_16 = CRC_16_SEED ; len >= 8; len -= 8, buf_ptr++) {
    crc_16 = (word)(crc16_table[ (crc_16 >> (16 - 8)) ^ *buf_ptr ] ^ (crc_16 << 8));
  }

  /* Finish calculating the CRC over the trailing data bits
  **
  ** XOR the MS bit of data with the MS bit of the CRC.
  ** Shift the CRC and data left 1 bit.
  ** If the XOR result is 1, XOR the generating polynomial in with the CRC.
  */
  if (len != 0) {

    data = ((word) (*buf_ptr)) << (16-8); /* Align data MSB with CRC MSB */

    while (len-- != 0) {
      if ( ((crc_16 ^ data) & ((word) 1 << 15)) != 0 ){ /* Is MSB of XOR a 1 */

        crc_16 <<= 1;                   /* Left shift CRC          */
        crc_16 ^= CRC_16_POLYNOMIAL;    /* XOR polynomial into CRC */

      } else {

        crc_16 <<= 1;                   /* Left shift CRC          */

      }

      data <<= 1;                       /* Left shift data         */
    }
  }

  return( ~crc_16 );            /* return the 1's complement of the CRC */

} /* end of crc_16_calc */


/*===========================================================================

FUNCTION CRC_16_L_CALC

DESCRIPTION
  This function calculates an LSB-first 16-bit CRC over a specified number
  of data bits.  It can be used to produce a CRC and to check a CRC.

DEPENDENCIES
  None

RETURN VALUE
  Returns a word holding 16 bits which are the contents of the CRC
  register as calculated over the specified data bits.  If this
  function is being used to check a CRC, then the return value will be
  equal to CRC_16_L_OK (defined in crc.h) if the CRC checks correctly.

SIDE EFFECTS
  None

===========================================================================*/

word crc_16_l_calc
(
  byte *buf_ptr,
    /* Pointer to bytes containing the data to CRC.  The bit stream starts
    ** in the LS bit of the first byte.
    */

  word len
    /* Number of data bits to calculate the CRC over */
)
{
  word data, crc_16;

  ASSERT( buf_ptr != NULL );

  /* Generate a CRC-16 by looking up the transformation in a table and
  ** XOR-ing it into the CRC, one byte at a time.
  */
  for (crc_16 = CRC_16_L_SEED ; len >= 8; len -= 8, buf_ptr++) {
    crc_16 = crc_16_l_table[ (crc_16 ^ *buf_ptr) & 0x00ff ] ^ (crc_16 >> 8);
  }

  /* Finish calculating the CRC over the trailing data bits
  **
  ** XOR the MS bit of data with the MS bit of the CRC.
  ** Shift the CRC and data left 1 bit.
  ** If the XOR result is 1, XOR the generating polynomial in with the CRC.
  */
  if (len != 0) {

    data = ((word) (*buf_ptr)) << (16-8); /* Align data MSB with CRC MSB */

    while (len-- != 0) {
      if ( ((crc_16 ^ data) & 0x01) != 0 ){   /* Is LSB of XOR a 1 */

        crc_16 >>= 1;                   /* Right shift CRC         */
        crc_16 ^= CRC_16_L_POLYNOMIAL;  /* XOR polynomial into CRC */

      } else {

        crc_16 >>= 1;                   /* Right shift CRC         */

      }

      data >>= 1;                       /* Right shift data        */
    }
  }

  return( ~crc_16 );            /* return the 1's complement of the CRC */

} /* end of crc_16_l_calc */


/*===========================================================================

FUNCTION CRC_30_CALC

DESCRIPTION
  This function calculates a 30-bit CRC over a specified number of data
  bits.  It can be used to produce a CRC and to check a CRC.

DEPENDENCIES
  None

RETURN VALUE
  Returns a double word holding 30 bits which are the contents of the
  CRC register as calculated over the specified data bits.  If this
  function is being used to check a CRC, then the return value will be
  equal to CRC_30_OK (defined in crc.h) if the CRC checks correctly.

SIDE EFFECTS
  None

===========================================================================*/

dword crc_30_calc
(
  byte *buf_ptr,
    /* Pointer to bytes containing the data to CRC.  The bit stream starts
    ** in the MS bit of the first byte.
    */

  word len
    /* Number of data bits to calculate the CRC over */
)
{
  dword data, crc_30;

  ASSERT( buf_ptr != NULL );

  /* Generate a CRC-30 by looking up the transformation in a table and
  ** XOR-ing it into the CRC, one byte at a time.
  */
  for (crc_30 = CRC_30_SEED ; len >= 8; len -= 8, buf_ptr++) {
    crc_30 = crc30_table[ ((crc_30 >> (30 - 8)) ^ *buf_ptr) & 0xff ]
                ^ (crc_30 << 8);
  }

  /* Finish calculating the CRC over the trailing data bits
  **
  ** XOR the MS bit of data with the MS bit of the CRC.
  ** Shift the CRC and data left 1 bit.
  ** If the XOR result is 1, XOR the generating polynomial in with the CRC.
  */
  if (len != 0) {

    data = ((dword) (*buf_ptr)) << (30-8); /* Align data MSB with CRC MSB */

    while (len-- != 0) {
      if ( ((crc_30 ^ data) & (1L << 29)) != 0 ) {       /* Is MSB of XOR a 1 */

        crc_30 <<= 1;                   /* Left shift CRC          */
        crc_30 ^= CRC_30_POLYNOMIAL;    /* XOR polynomial into CRC */

      } else {

        crc_30 <<= 1;                   /* Left shift CRC          */

      }

      data <<= 1;                       /* Left shift data         */
    }
  }

  crc_30 = ~crc_30;                     /* 1's complement the CRC     */

  return( crc_30 & 0x3FFFFFFF );        /* mask the result to 30 bits */

} /* end of crc_30_calc */


/*===========================================================================

FUNCTION CRC_30_STEP

DESCRIPTION
  This function calculates a 30-bit CRC over a specified number of data
  bits.  It can be used to produce a CRC and to check a CRC.  The CRC value
  passed in is used to continue the CRC calculation from a previous call,
  this allows this routine to be used to CRC discontinuous data.

DEPENDENCIES
  None

RETURN VALUE
  Returns a double word holding 30 bits which are the contents of the
  CRC register as calculated over the specified data bits.  If this
  function is being used to check a CRC, then the return value will be
  equal to CRC_30_OK (defined in crc.h) if the CRC checks correctly.
  
  NOTE: The caller is expected to mask the bottom 30 bits from the value, 

SIDE EFFECTS
  None

===========================================================================*/

dword crc_30_step
(
  dword seed,
    /* Either the result of a previous crc_30_step() or CRC_16_STEP_SEED
    ** the first time the routine is called.  Note that the input is
    ** inverted prior to use, to counteract the inversion applied when
    ** it was returned as a result of the previous step.
    */

  byte *buf_ptr,
    /* Pointer to bytes containing the data to CRC.  The bit stream starts
    ** in the MS bit of the first byte.
    */

  word len
    /* Number of data bits to calculate the CRC over */
)
{
  dword data, crc_30;

  ASSERT( buf_ptr != NULL );

  /* Generate a CRC-30 by looking up the transformation in a table and
  ** XOR-ing it into the CRC, one byte at a time.
  */
  for (crc_30 = ~seed & 0x3FFFFFFF; len >= 8; len -= 8, buf_ptr++) {
    crc_30 = crc30_table[ ((crc_30 >> (30 - 8)) ^ *buf_ptr) & 0xff ]
                ^ (crc_30 << 8);
  }

  /* Finish calculating the CRC over the trailing data bits
  **
  ** XOR the MS bit of data with the MS bit of the CRC.
  ** Shift the CRC and data left 1 bit.
  ** If the XOR result is 1, XOR the generating polynomial in with the CRC.
  */
  if (len != 0) {

    data = ((dword) (*buf_ptr)) << (30-8); /* Align data MSB with CRC MSB */

    while (len-- != 0) {
      if ( ((crc_30 ^ data) & (1L << 29)) != 0 ) {       /* Is MSB of XOR a 1 */

        crc_30 <<= 1;                   /* Left shift CRC          */
        crc_30 ^= CRC_30_POLYNOMIAL;    /* XOR polynomial into CRC */

      } else {

        crc_30 <<= 1;                   /* Left shift CRC          */

      }

      data <<= 1;                       /* Left shift data         */
    }
  }

  crc_30 = ~crc_30;                     /* 1's complement the CRC     */

  return( crc_30 & 0x3FFFFFFF );        /* mask the result to 30 bits */

} /* end of crc_30_calc */


/*===========================================================================

FUNCTION CRC_16_STEP

DESCRIPTION
  This function calculates a 16-bit CRC over a specified number of data
  bytes.  It can be used to produce a CRC and to check a CRC.  The CRC value
  passed in is used to continue the CRC calculation from a previous call;
  this allows this routine to be used to CRC discontinuous data.

DEPENDENCIES
  None

RETURN VALUE
  Returns a word holding 16 bits which are the contents of the CRC
  register as calculated over the specified data bits.  If this
  function is being used to check a CRC, then the return value will be
  equal to CRC_16_OK (defined in crc.h) if the CRC checks correctly.

SIDE EFFECTS
  None

===========================================================================*/

word crc_16_step
(
  word seed,
    /* Either the result of a previous crc_16_step() or CRC_16_STEP_SEED
    ** the first time the routine is called.  Note that the input is
    ** inverted prior to use, to counteract the inversion applied when
    ** it was returned as a result of the previous step.
    */

  byte *buf_ptr,
    /* Pointer to bytes containing the data to CRC.  The bit stream starts
    ** in the MS bit of the first byte.
    */

  word byte_len
    /* Number of data bytes to calculate the CRC over */
)
{
  word crc_16;

  ASSERT( buf_ptr != NULL );

  /* Generate a CRC-16 by looking up the transformation in a table and
  ** XOR-ing it into the CRC, one byte at a time.
  **
  ** Note: complement the seed to account for it having been complemented
  ** in a previous call to crc_16_step().
  */
  for (crc_16 = ~seed ; byte_len > 0; byte_len--, buf_ptr++) {
    crc_16 = (word)(crc16_table[ (crc_16 >> (16 - 8)) ^ *buf_ptr ] ^ (crc_16 << 8));
  }

  return( ~crc_16 );            /* return the 1's complement of the CRC */

} /* end of crc_16_step */



/*===========================================================================

FUNCTION CRC_16_L_STEP

DESCRIPTION
  This function calculates a LSB-first 16-bit CRC over a specified number 
  of data bytes.  It can be used to produce a CRC and to check a CRC.  
  The CRC value passed in is used to continue the CRC calculation from a
  previous call; this allows this routine to be used to CRC discontinuous data.

DEPENDENCIES
  None

RETURN VALUE
  Returns a word holding 16 bits which are the contents of the CRC
  register as calculated over the specified data bits.  If this
  function is being used to check a CRC, then the return value will be
  equal to CRC_16_OK (defined in crc.h) if the CRC checks correctly.

SIDE EFFECTS
  None

===========================================================================*/

word crc_16_l_step
(
  word seed,

  const void *buf_ptr,
    /* Either the result of a previous crc_16_l_step() or CRC_16_L_STEP_SEED
    ** the first time the routine is called.  Note that the input is
    ** inverted prior to use, to counteract the inversion applied when
    ** it was returned as a result of the previous step.
    */

  unsigned int byte_len
    /* Number of data bytes to calculate the CRC over */
)
{
  word crc_16;
  byte *byte_ptr = (byte *) buf_ptr;

  ASSERT( byte_ptr != NULL );

  /* Generate a CRC-16 by looking up the transformation in a table and
  ** XOR-ing it into the CRC, one byte at a time.
  */
  for (crc_16 = ~seed ; byte_len > 0; byte_len--, byte_ptr++) {
    crc_16 = crc_16_l_table[ (crc_16 ^ *byte_ptr) & 0x00ff ] ^ (crc_16 >> 8);
  }

  return( ~crc_16 );            /* return the 1's complement of the CRC */

} /* end of crc_16_l_step */


/*===========================================================================

FUNCTION CRC_32_CALC

DESCRIPTION
  This function calculates a 32-bit CRC over a specified number of data
  bits.  It can be used to produce a CRC and to check a CRC.  The CRC
  seed value can be used to compute a CRC over multiple (non-adjacent)
  chunks of data.

DEPENDENCIES
  None.

RETURN VALUE
  Returns a 32-bit word holding the contents of the CRC register as
  calculated over the specified data bits.

SIDE EFFECTS
  None.

===========================================================================*/
uint32 crc_32_calc
(
  /* Pointer to data over which to compute CRC */
  uint8  *buf_ptr,

  /* Number of bits over which to compute CRC */
  uint16  len,

  /* Seed for CRC computation */
  uint32  seed
)
{
  /* Input data buffer */
  uint32  data;

  /* Working CRC buffer */
  uint32  crc;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT( buf_ptr != NULL );

  /*-------------------------------------------------------------------------
   Generate the CRC by looking up the transformation in a table and
   XOR-ing it into the CRC, one byte at a time.
  -------------------------------------------------------------------------*/
  for ( crc = seed; len >= 8; len -= 8, buf_ptr++ )
  {
    crc = crc32_table[ ( ( crc >> 24 ) ^ ( *buf_ptr ) ) ] ^ ( crc << 8 );
  }

  /*-------------------------------------------------------------------------
   Finish calculating the CRC over the trailing data bits. This is done by
   aligning the remaining data bits with the CRC MSB, and then computing the
   CRC one bit at a time.
  -------------------------------------------------------------------------*/
  if ( len != 0 )
  {
    /*-----------------------------------------------------------------------
     Align data MSB with CRC MSB.
    -----------------------------------------------------------------------*/
    data = ( (uint32)( *buf_ptr ) ) << 24;

    /*-----------------------------------------------------------------------
     While there are bits left, compute CRC one bit at a time.
    -----------------------------------------------------------------------*/
    while (len-- != 0)
    {
      /*---------------------------------------------------------------------
       If the MSB of the XOR combination of the data and CRC is one, then
       advance the CRC register by one bit XOR with the CRC polynomial.
       Otherwise, just advance the CRC by one bit.
      ---------------------------------------------------------------------*/
      if ( ( ( crc ^ data ) & CRC_32_MSB_MASK ) != 0 )
      {
        crc <<= 1;
        crc ^= CRC_32_POLYNOMIAL;
      }      
      else
      {
        crc <<= 1;
      }

      /*---------------------------------------------------------------------
       Advance to the next input data bit and continue.
      ---------------------------------------------------------------------*/
      data <<= 1;
    }
  }

  /*-------------------------------------------------------------------------
   Return the resulting CRC value.
  -------------------------------------------------------------------------*/
  return( crc );

} /* crc_32_calc */
