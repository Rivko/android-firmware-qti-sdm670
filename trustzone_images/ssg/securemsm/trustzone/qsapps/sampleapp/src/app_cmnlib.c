/*
  @file app_cmnlib.c
  @brief This file is a sample code that tests all QSEE API's exposed from TZ Common Library.

*/
/*===========================================================================
  Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
  ===========================================================================*/

/*===========================================================================

  EDIT HISTORY FOR FILE
  $Header: //components/rel/ssg.tz/1.0.2/securemsm/trustzone/qsapps/sampleapp/src/app_cmnlib.c#1 $
  $DateTime: 2018/02/06 03:00:17 $
  $Author: pwbldsvc $

  # when       who     what, where, why
  # --------   ---     ---------------------------------------------------------
  11/26/12   ah      Initial version
  ===========================================================================*/
#include "IxErrno.h"
#include "com_dtypes.h"
#include "qsee_cipher.h"
#include "qsee_rsa.h"
#include "qsee_log.h"
#include "qsee_heap.h"
#include "qsee_sfs.h"
#include "qsee_fs.h"
#include "qsee_kdf.h"

#define CTEST_DATA "dummy data for sfs test suite"
#define CTEST_DATA2 ". kdfjg sodj sdk gasdoi"

#define TZ_CM_MAX_NAME_LEN          256

/* Endianness converter Macro */
#define htonl(x)                                \
   (((((uint32)(x) & 0x000000FFU) << 24) |      \
     (((uint32)(x) & 0x0000FF00U) <<  8) |      \
     (((uint32)(x) & 0x00FF0000U) >>  8) |      \
     (((uint32)(x) & 0xFF000000U) >> 24)))

/*
 * DER-encoded PKCS#8 format RSA key. Generated using:
 *
 * openssl genrsa 2048 | openssl pkcs8 -topk8 -nocrypt -outform der | recode ../x1
 */
static uint8 pkcs8_key[] = {
   0x30, 0x82, 0x04, 0xBE, 0x02, 0x01, 0x00, 0x30, 0x0D, 0x06, 0x09, 0x2A,
   0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01, 0x05, 0x00, 0x04, 0x82,
   0x04, 0xA8, 0x30, 0x82, 0x04, 0xA4, 0x02, 0x01, 0x00, 0x02, 0x82, 0x01,
   0x01, 0x00, 0xD8, 0x58, 0xD4, 0x9F, 0xC0, 0xE8, 0xF0, 0xFF, 0x87, 0x27,
   0x43, 0xE6, 0x2E, 0xE6, 0x9A, 0x42, 0x3B, 0x39, 0x94, 0x84, 0x43, 0x55,
   0x8D, 0x20, 0x5B, 0x71, 0x88, 0xE6, 0xD1, 0x62, 0xC8, 0xF2, 0x20, 0xD0,
   0x75, 0x13, 0x83, 0xA3, 0x5D, 0x19, 0xA8, 0x62, 0xD0, 0x5F, 0x3E, 0x8A,
   0x7C, 0x0E, 0x26, 0xA9, 0xFF, 0xB2, 0x5E, 0x63, 0xAA, 0x3C, 0x8D, 0x13,
   0x41, 0xAA, 0xD5, 0x03, 0x01, 0x01, 0x53, 0xC9, 0x02, 0x1C, 0xEC, 0xE8,
   0xC4, 0x70, 0x3F, 0x43, 0xE5, 0x51, 0xD0, 0x6E, 0x52, 0x0B, 0xC4, 0x0A,
   0xA3, 0x61, 0xDE, 0xE3, 0x72, 0x0C, 0x94, 0xF1, 0x1C, 0x2D, 0x36, 0x77,
   0xBB, 0x16, 0xA8, 0x63, 0x4B, 0xD1, 0x07, 0x00, 0x42, 0x2D, 0x2B, 0x10,
   0x80, 0x45, 0xF3, 0x0C, 0xF9, 0xC5, 0xAC, 0xCC, 0x64, 0x87, 0xFD, 0x5D,
   0xC8, 0x51, 0xD4, 0x1C, 0x9E, 0x6E, 0x9B, 0xC4, 0x27, 0x5E, 0x73, 0xA7,
   0x2A, 0xF6, 0x90, 0x42, 0x0C, 0x34, 0x93, 0xB7, 0x02, 0x19, 0xA9, 0x64,
   0x6C, 0x46, 0x3B, 0x40, 0x02, 0x2F, 0x54, 0x69, 0x79, 0x26, 0x7D, 0xF6,
   0x85, 0x90, 0x01, 0xD0, 0x21, 0x07, 0xD0, 0x14, 0x00, 0x65, 0x9C, 0xAC,
   0x24, 0xE8, 0x78, 0x42, 0x3B, 0x90, 0x75, 0x19, 0x55, 0x11, 0x4E, 0xD9,
   0xE6, 0x97, 0x87, 0xBC, 0x8D, 0x2C, 0x9B, 0xF0, 0x1F, 0x14, 0xEB, 0x6A,
   0x57, 0xCE, 0x78, 0xAD, 0xCE, 0xD9, 0xFB, 0xB9, 0xA1, 0xEF, 0x0C, 0x1F,
   0xDD, 0xE3, 0x5B, 0x73, 0xA0, 0xEC, 0x37, 0x9C, 0xE1, 0xFD, 0x86, 0x28,
   0xC3, 0x4A, 0x42, 0xD0, 0xA3, 0xFE, 0x57, 0x09, 0x29, 0xD8, 0xF6, 0xEC,
   0xE3, 0xC0, 0x71, 0x7C, 0x29, 0x27, 0xC2, 0xD1, 0x3E, 0x22, 0xBC, 0xBD,
   0x5A, 0x85, 0x41, 0xF6, 0x15, 0xDA, 0x0C, 0x58, 0x5A, 0x61, 0x5B, 0x78,
   0xB8, 0xAA, 0xEC, 0x5C, 0x1C, 0x79, 0x02, 0x03, 0x01, 0x00, 0x01, 0x02,
   0x82, 0x01, 0x00, 0x1D, 0x10, 0x31, 0xE0, 0x14, 0x26, 0x36, 0xD9, 0xDC,
   0xEA, 0x25, 0x70, 0xF2, 0xB3, 0xFF, 0xDD, 0x0D, 0xDF, 0xBA, 0x57, 0xDA,
   0x43, 0xCF, 0xE5, 0x9C, 0xE3, 0x2F, 0xA4, 0xF2, 0x53, 0xF6, 0xF2, 0xAF,
   0xFD, 0xD0, 0xFC, 0x82, 0x1E, 0x9C, 0x0F, 0x2A, 0x53, 0xBB, 0xF2, 0x4F,
   0x90, 0x83, 0x01, 0xD3, 0xA7, 0xDA, 0xB5, 0xB7, 0x80, 0x64, 0x0A, 0x26,
   0x59, 0x83, 0xE4, 0xD3, 0x20, 0xC8, 0x2D, 0xC9, 0x77, 0xA3, 0x55, 0x07,
   0x6E, 0x6D, 0x95, 0x36, 0xAA, 0x84, 0x4F, 0xED, 0x54, 0x24, 0xA9, 0x77,
   0xF8, 0x85, 0xE2, 0x4B, 0xF2, 0xFA, 0x0B, 0x3E, 0xA6, 0xF5, 0x46, 0x0D,
   0x9F, 0x1F, 0xFE, 0xF7, 0x37, 0xFF, 0xA3, 0x60, 0xF1, 0x63, 0xF2, 0x75,
   0x6A, 0x8E, 0x10, 0xD7, 0x89, 0xD2, 0xB3, 0xFF, 0x76, 0xA5, 0xBA, 0xAF,
   0x0A, 0xBE, 0x32, 0x5F, 0xF0, 0x48, 0x48, 0x4B, 0x9C, 0x9A, 0x3D, 0x12,
   0xA7, 0xD2, 0x07, 0xC7, 0x59, 0x32, 0x94, 0x95, 0x65, 0x2F, 0x87, 0x34,
   0x76, 0xBA, 0x7C, 0x08, 0x4B, 0xAB, 0xA6, 0x24, 0xDF, 0x64, 0xDB, 0x48,
   0x63, 0x42, 0x06, 0xE2, 0x2C, 0x3D, 0xFB, 0xE5, 0x47, 0x81, 0x94, 0x98,
   0xF7, 0x32, 0x4B, 0x28, 0xEB, 0x42, 0xB8, 0xE9, 0x8E, 0xFC, 0xC9, 0x43,
   0xC9, 0x47, 0xE6, 0xE7, 0x1C, 0xDC, 0x71, 0xEF, 0x4D, 0x8A, 0xB1, 0xFC,
   0x45, 0x37, 0xEC, 0xB3, 0x16, 0x88, 0x5B, 0xE2, 0xEC, 0x8B, 0x6B, 0x75,
   0x16, 0xBE, 0x6B, 0xF8, 0x2C, 0xF8, 0xC9, 0xD1, 0xF7, 0x55, 0x87, 0x57,
   0x5F, 0xDE, 0xF4, 0x7E, 0x72, 0x13, 0x06, 0x2A, 0x21, 0xB7, 0x78, 0x21,
   0x05, 0xFD, 0xE2, 0x5F, 0x7B, 0x7C, 0xF0, 0x26, 0x2B, 0x75, 0x7F, 0x68,
   0xF9, 0xA6, 0x98, 0xFD, 0x54, 0x0E, 0xCC, 0x22, 0x41, 0x7F, 0x29, 0x81,
   0x2F, 0xA3, 0x3C, 0x3D, 0x64, 0xC8, 0x41, 0x02, 0x81, 0x81, 0x00, 0xFA,
   0xFA, 0xE4, 0x2E, 0x30, 0xF0, 0x7A, 0x8D, 0x95, 0xB8, 0x39, 0x58, 0x27,
   0x0F, 0x89, 0x0C, 0xDF, 0xFE, 0x2F, 0x55, 0x3B, 0x6F, 0xDD, 0x5F, 0x12,
   0xB3, 0xD1, 0xCF, 0x5B, 0x8D, 0xB6, 0x10, 0x1C, 0x87, 0x0C, 0x30, 0x89,
   0x2D, 0xBB, 0xB8, 0xA1, 0x78, 0x0F, 0x54, 0xA6, 0x36, 0x46, 0x05, 0x8B,
   0x5A, 0xFF, 0x48, 0x03, 0x13, 0xAE, 0x95, 0x96, 0x5D, 0x6C, 0xDA, 0x5D,
   0xF7, 0xAD, 0x1D, 0x33, 0xED, 0x23, 0xF5, 0x4B, 0x03, 0x78, 0xE7, 0x50,
   0xD1, 0x2D, 0x95, 0x22, 0x35, 0x02, 0x5B, 0x4A, 0x4E, 0x73, 0xC9, 0xB7,
   0x05, 0xC4, 0x21, 0x86, 0x1F, 0x1E, 0x40, 0x83, 0xBC, 0x8A, 0x3A, 0x95,
   0x24, 0x62, 0xF4, 0x58, 0x38, 0x64, 0x4A, 0x89, 0x8A, 0x27, 0x59, 0x12,
   0x9D, 0x21, 0xC3, 0xA6, 0x42, 0x1E, 0x2A, 0x3F, 0xD8, 0x65, 0x1F, 0x6E,
   0x3E, 0x4D, 0x5C, 0xCC, 0xEA, 0x8E, 0x15, 0x02, 0x81, 0x81, 0x00, 0xDC,
   0xAC, 0x9B, 0x00, 0xDB, 0xF9, 0xB2, 0xBF, 0xC4, 0x5E, 0xB6, 0xB7, 0x63,
   0xEB, 0x13, 0x4B, 0xE2, 0xA6, 0xC8, 0x72, 0x90, 0xD8, 0xC2, 0x33, 0x33,
   0xF0, 0x66, 0x75, 0xBD, 0x50, 0x7C, 0xA4, 0x8F, 0x82, 0xFB, 0xFF, 0x44,
   0x3B, 0xE7, 0x15, 0x3A, 0x0C, 0x7A, 0xF8, 0x92, 0x86, 0x4A, 0x79, 0x32,
   0x08, 0x82, 0x1D, 0x6A, 0xBA, 0xAD, 0x8A, 0xB3, 0x3D, 0x7F, 0xA5, 0xB4,
   0x6F, 0x67, 0x86, 0x7E, 0xB2, 0x9C, 0x2A, 0xF6, 0x7C, 0x49, 0x21, 0xC5,
   0x3F, 0x00, 0x3F, 0x9B, 0xF7, 0x0F, 0x6C, 0x35, 0x80, 0x75, 0x73, 0xC0,
   0xF8, 0x3E, 0x30, 0x5F, 0x74, 0x2F, 0x15, 0x41, 0xEA, 0x0F, 0xCE, 0x0E,
   0x18, 0x17, 0x68, 0xBA, 0xC4, 0x29, 0xF2, 0xE2, 0x2C, 0x1D, 0x55, 0x83,
   0xB6, 0x64, 0x2E, 0x03, 0x12, 0xA4, 0x0D, 0xBF, 0x4F, 0x2E, 0xBE, 0x7C,
   0x41, 0xD9, 0xCD, 0xD0, 0x52, 0x91, 0xD5, 0x02, 0x81, 0x81, 0x00, 0xD4,
   0x55, 0xEB, 0x32, 0xC1, 0x28, 0xD3, 0x26, 0x72, 0x22, 0xB8, 0x31, 0x42,
   0x6A, 0xBC, 0x52, 0x6E, 0x37, 0x48, 0xA8, 0x5D, 0x6E, 0xD8, 0xE5, 0x14,
   0x97, 0x99, 0xCC, 0x4A, 0xF2, 0xEB, 0xB3, 0x59, 0xCF, 0x4F, 0x9A, 0xC8,
   0x94, 0x2E, 0x9B, 0x97, 0xD0, 0x51, 0x78, 0x16, 0x5F, 0x18, 0x82, 0x9C,
   0x51, 0xD2, 0x64, 0x84, 0x65, 0xE4, 0x70, 0x9E, 0x14, 0x50, 0x81, 0xB6,
   0xBA, 0x52, 0x75, 0xC0, 0x76, 0xC2, 0xD3, 0x46, 0x31, 0x9B, 0xDA, 0x67,
   0xDF, 0x71, 0x27, 0x19, 0x17, 0xAB, 0xF4, 0xBC, 0x3A, 0xFF, 0x6F, 0x0B,
   0x2F, 0x0F, 0xAE, 0x25, 0x20, 0xB2, 0xA1, 0x76, 0x52, 0xCE, 0xC7, 0x9D,
   0x62, 0x79, 0x6D, 0xAC, 0x2D, 0x99, 0x7C, 0x0E, 0x3D, 0x19, 0xE9, 0x1B,
   0xFC, 0x60, 0x92, 0x7C, 0x58, 0xB7, 0xD8, 0x9A, 0xC7, 0x63, 0x56, 0x62,
   0x18, 0xC7, 0xAE, 0xD9, 0x97, 0x1F, 0xB9, 0x02, 0x81, 0x81, 0x00, 0x91,
   0x40, 0xC4, 0x1E, 0x82, 0xAD, 0x0F, 0x6D, 0x8E, 0xD2, 0x51, 0x2E, 0xD1,
   0x84, 0x30, 0x85, 0x68, 0xC1, 0x23, 0x7B, 0xD5, 0xBF, 0xF7, 0xC4, 0x40,
   0x51, 0xE2, 0xFF, 0x69, 0x07, 0x8B, 0xA3, 0xBE, 0x1B, 0x17, 0xC8, 0x64,
   0x9F, 0x91, 0x71, 0xB5, 0x6D, 0xF5, 0x9B, 0x9C, 0xC6, 0xEC, 0x4A, 0x6E,
   0x16, 0x8F, 0x9E, 0xD1, 0x5B, 0xE3, 0x53, 0x42, 0xBC, 0x1E, 0x43, 0x72,
   0x4B, 0x4A, 0x37, 0x8B, 0x3A, 0x01, 0xF5, 0x7D, 0x9D, 0x3D, 0x7E, 0x0F,
   0x19, 0x73, 0x0E, 0x6B, 0x98, 0xE9, 0xFB, 0xEE, 0x13, 0x8A, 0x3C, 0x11,
   0x2E, 0xD5, 0xB0, 0x7D, 0x84, 0x3A, 0x61, 0xA1, 0xAB, 0x71, 0x8F, 0xCE,
   0x53, 0x29, 0x45, 0x74, 0x7A, 0x1E, 0xAA, 0x93, 0x19, 0x3A, 0x8D, 0xC9,
   0x4E, 0xCB, 0x0E, 0x46, 0x53, 0x84, 0xCC, 0xCF, 0xBA, 0x4D, 0x28, 0x71,
   0x1D, 0xDF, 0x41, 0xCB, 0xF8, 0x2D, 0xA9, 0x02, 0x81, 0x80, 0x04, 0x8B,
   0x4A, 0xEA, 0xBD, 0x39, 0x0B, 0x96, 0xC5, 0x1D, 0xA4, 0x47, 0xFD, 0x46,
   0xD2, 0x8A, 0xEA, 0x2A, 0xF3, 0x9D, 0x3A, 0x7E, 0x16, 0x74, 0xFC, 0x13,
   0xDE, 0x4D, 0xA9, 0x85, 0x42, 0x33, 0x02, 0x92, 0x0B, 0xB6, 0xDB, 0x7E,
   0xEA, 0x85, 0xC2, 0x94, 0x43, 0x52, 0x37, 0x5A, 0x77, 0xAB, 0xCB, 0x61,
   0x88, 0xDE, 0xF8, 0xFA, 0xDB, 0xE8, 0x0B, 0x95, 0x7D, 0x39, 0x19, 0xA2,
   0x89, 0xB9, 0x32, 0xB2, 0x50, 0x38, 0xF7, 0x88, 0x69, 0xFD, 0xA4, 0x63,
   0x1F, 0x9B, 0x03, 0xD8, 0xA6, 0x7A, 0x05, 0x76, 0x02, 0x28, 0x93, 0x82,
   0x73, 0x7F, 0x14, 0xCC, 0xBE, 0x29, 0x10, 0xAD, 0x8A, 0x2E, 0xAC, 0xED,
   0x11, 0xA7, 0x72, 0x7C, 0x60, 0x78, 0x72, 0xFB, 0x78, 0x20, 0x18, 0xC9,
   0x7E, 0x63, 0xAD, 0x55, 0x54, 0x51, 0xDB, 0x9F, 0x7B, 0xD4, 0x8F, 0xB2,
   0xDE, 0x3B, 0xF1, 0x70, 0x23, 0xE5,
};

/*
 * DER-encoded PKCS#8 format ECC key. Generated using:
 *
 * openssl ecparam -out ec1.pem -genkey -name prime256v1
 * openssl pkcs8 -topk8 -nocrypt -in ec1.pem -out ec2.pem
 */
static uint8 pkcs8_ecc_key[] = {
		0X30, 0X81, 0X87, 0X02, 0X01, 0X00, 0X30, 0X13, 0X06, 0X07, 0X2A, 0X86, 0X48, 0XCE, 0X3D, 0X02,
		0X01, 0X06, 0X08, 0X2A, 0X86, 0X48, 0XCE, 0X3D, 0X03, 0X01, 0X07, 0X04, 0X6D, 0X30, 0X6B, 0X02,
		0X01, 0X01, 0X04, 0X20, 0X3A, 0X01, 0XC9, 0X7B, 0X0E, 0X34, 0XA0, 0X82, 0X71, 0X95, 0XAB, 0X71,
		0XD5, 0X5C, 0XC0, 0X4D, 0XE3, 0XAF, 0XF4, 0X73, 0X63, 0X62, 0X64, 0XA3, 0X60, 0XC2, 0X5D, 0XB8,
		0XFB, 0XC9, 0X27, 0XFE, 0XA1, 0X44, 0X03, 0X42, 0X00, 0X04, 0XAB, 0XEB, 0X59, 0XE6, 0XE7, 0XCD,
		0XD3, 0X43, 0XD2, 0XC7, 0XDF, 0X32, 0XE0, 0X96, 0X67, 0X98, 0X81, 0XB2, 0X27, 0X83, 0XBE, 0X7C,
		0XF0, 0X62, 0X46, 0X82, 0X8B, 0XC6, 0X9D, 0X86, 0XDE, 0X47, 0XD7, 0X50, 0X8F, 0XDE, 0X1A, 0X97,
		0X6F, 0X3E, 0XDA, 0X4E, 0X10, 0XED, 0XCB, 0X76, 0X7F, 0X0B, 0X91, 0X81, 0XCC, 0XE8, 0X14, 0X45,
		0XDC, 0XE5, 0XAE, 0X83, 0XA1, 0X4B, 0X95, 0XA7, 0X09, 0X9D,
};

static uint8 pkcs8_ecc_key2[] = {
		0X30, 0X81, 0X87, 0X02, 0X01, 0X00, 0X30, 0X13, 0X06, 0X07, 0X2A, 0X86, 0X48, 0XCE, 0X3D, 0X02,
		0X01, 0X06, 0X08, 0X2A, 0X86, 0X48, 0XCE, 0X3D, 0X03, 0X01, 0X07, 0X04, 0X6D, 0X30, 0X6B, 0X02,
		0X01, 0X01, 0X04, 0X20, 0X73, 0X7C, 0X2E, 0XCD, 0X7B, 0X8D, 0X19, 0X40, 0XBF, 0X29, 0X30, 0XAA,
		0X9B, 0X4E, 0XD3, 0XFF, 0X94, 0X1E, 0XED, 0X09, 0X36, 0X6B, 0XC0, 0X32, 0X99, 0X98, 0X64, 0X81,
		0XF3, 0XA4, 0XD8, 0X59, 0XA1, 0X44, 0X03, 0X42, 0X00, 0X04, 0XBF, 0X85, 0XD7, 0X72, 0X0D, 0X07,
		0XC2, 0X54, 0X61, 0X68, 0X3B, 0XC6, 0X48, 0XB4, 0X77, 0X8A, 0X9A, 0X14, 0XDD, 0X8A, 0X02, 0X4E,
		0X3B, 0XDD, 0X8C, 0X7D, 0XDD, 0X9A, 0XB2, 0XB5, 0X28, 0XBB, 0XC7, 0XAA, 0X1B, 0X51, 0XF1, 0X4E,
		0XBB, 0XBB, 0X0B, 0XD0, 0XCE, 0X21, 0XBC, 0XC4, 0X1C, 0X6E, 0XB0, 0X00, 0X83, 0XCF, 0X33, 0X76,
		0XD1, 0X1F, 0XD4, 0X49, 0X49, 0XE0, 0XB2, 0X18, 0X3B, 0XFE,
};

/**
 * Parameters needed to generate an RSA key.
 */
typedef struct {
   uint32_t   modulus_size;
   uint64_t   public_exponent;
} rsa_keygen_params_t;

uint8 cipher_key[16];
static char cipher_key_label[] = {"SAMPLEAPP CPHR HW Crypto Derived key"};
static char cipher_key_context[] = {"SAMPLEAPP CPHR HW Crypto key derived from SHK"};

extern int tz_app_sfs(void);
extern int test_crypto_rsa_enc_dec(void);
extern int test_crypto_rsa_sign_verify(void);

int32 copy_S_BIGINT(uint8 *dst, uint32 dst_len, QSEE_S_BIGINT *src, uint32 max_len)
{
   uint32 i = 0;
   uint32 array_length = 0;

   if (dst_len > max_len)
   {
      return -E_FAILURE;
   }

   array_length = dst_len/4;
   for (i=0; i<array_length; i++)
   {
      *((uint32 *)(uintnt)dst + i) =  htonl(src->bi.a[array_length - 1 - i]);
   }
   return E_SUCCESS;
}

int test_crypto_ecc_complementary(void)
{
	QSEE_pkcs8_privkey_type pkcs8_prv_ecc_key = {0};
	uint16 pkcs8_ecc_key_len = sizeof(pkcs8_ecc_key)/sizeof(uint8);
	int32 ret = E_SUCCESS;

    pkcs8_prv_ecc_key.algo = QSEE_PKCS8_ECC_ENCRYPTION;
    ret = (int32)qsee_secpkcs8_parse(pkcs8_ecc_key, (uint16)pkcs8_ecc_key_len, &pkcs8_prv_ecc_key);
    if (ret != E_SUCCESS) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Secpkcs8 Parser failed: %d.", ret);
    } else {
      qsee_log(QSEE_LOG_MSG_DEBUG, "Secpkcs8 (ECC) Parser succeeded.");
      qsee_log(QSEE_LOG_MSG_DEBUG,"privkey[0] = %02x, privkey[1] = %02x",
    		  pkcs8_prv_ecc_key.key.ecc.privkey_data[0],
			  pkcs8_prv_ecc_key.key.ecc.privkey_data[1]);
      qsee_log(QSEE_LOG_MSG_DEBUG,"pubkey_x[0] = %02x, pubkey_x[1] = %02x",
    		  pkcs8_prv_ecc_key.key.ecc.pubkey_x_data[0],
			  pkcs8_prv_ecc_key.key.ecc.pubkey_x_data[1]);
      qsee_log(QSEE_LOG_MSG_DEBUG,"pubkey_y[0] = %02x, pubkey_y[1] = %02x",
    		  pkcs8_prv_ecc_key.key.ecc.pubkey_y_data[0],
			  pkcs8_prv_ecc_key.key.ecc.pubkey_y_data[1]);
    }

    ret = (int32)qsee_secpkcs8_parse(pkcs8_ecc_key2, (uint16)pkcs8_ecc_key_len, &pkcs8_prv_ecc_key);
    if (ret != E_SUCCESS) {
      qsee_log(QSEE_LOG_MSG_ERROR, "Secpkcs8 Parser failed: %d.", ret);
    } else {
        qsee_log(QSEE_LOG_MSG_DEBUG, "Secpkcs8 (ECC) Parser succeeded.");
        qsee_log(QSEE_LOG_MSG_DEBUG,"privkey[0] = %02x, privkey[1] = %02x",
      		  pkcs8_prv_ecc_key.key.ecc.privkey_data[0],
  			  pkcs8_prv_ecc_key.key.ecc.privkey_data[1]);
        qsee_log(QSEE_LOG_MSG_DEBUG,"pubkey_x[0] = %02x, pubkey_x[1] = %02x",
      		  pkcs8_prv_ecc_key.key.ecc.pubkey_x_data[0],
  			  pkcs8_prv_ecc_key.key.ecc.pubkey_x_data[1]);
        qsee_log(QSEE_LOG_MSG_DEBUG,"pubkey_y[0] = %02x, pubkey_y[1] = %02x",
      		  pkcs8_prv_ecc_key.key.ecc.pubkey_y_data[0],
  			  pkcs8_prv_ecc_key.key.ecc.pubkey_y_data[1]);
    }

	return ret;
}

int test_crypto_rsa_complementary(void)
{
   QSEE_pkcs8_privkey_type pkcs8_prv_key = {0};
   uint16 pkcs8_key_len = sizeof(pkcs8_key)/sizeof(uint8);
   QSEE_RSA_KEY rsa_key = {0};
   unsigned char pub_exp[] = {0x00, 0x00, 0x01, 0x00, 0x01};
   rsa_keygen_params_t rsa_params = {0};
   uint8 *pTmp = NULL;
   int32 ret = E_SUCCESS;

   do {
      // PKCS#8 key parser interface test
      pkcs8_prv_key.algo = QSEE_PKCS8_RSA_ENCRYPTION;
      ret = (int32)qsee_secpkcs8_parse(pkcs8_key, (uint16)pkcs8_key_len, &pkcs8_prv_key);
      if (ret != E_SUCCESS) {
         qsee_log(QSEE_LOG_MSG_ERROR, "Secpkcs8 Parser failed.");
         break;
      } else {
         qsee_log(QSEE_LOG_MSG_DEBUG, "Secpkcs8 Parser succeeded.");
      }

      // RSA KeyGen interface test
      rsa_params.modulus_size = 64;
      if ( qsee_util_init_s_bigint(&(rsa_key.e))|| qsee_util_init_s_bigint(&(rsa_key.d)) ||
           qsee_util_init_s_bigint(&(rsa_key.N)) || qsee_util_init_s_bigint(&(rsa_key.p)) || qsee_util_init_s_bigint(&(rsa_key.q)) ){
         qsee_log(QSEE_LOG_MSG_ERROR, "qsee_util_init_s_bigint failed.");
         ret = -1;
         break;
      }

      ret = qsee_rsa_key_gen(&rsa_key, rsa_params.modulus_size, pub_exp, (int)(sizeof(pub_exp)));
      if (ret != E_SUCCESS) {
         qsee_log(QSEE_LOG_MSG_ERROR, "QSEE KeyGen failed.");
         break;
      } else {
         qsee_log(QSEE_LOG_MSG_DEBUG, "QSEE KeyGen succeeded.");
      }

      // Copy key into array of bytes
      pTmp = qsee_malloc(QSEE_RSA_KEY_SIZE(rsa_key.N));
      if (pTmp != NULL) {
         ret = copy_S_BIGINT(pTmp, QSEE_RSA_KEY_SIZE(rsa_key.N), rsa_key.N, 256);
         if (ret != E_SUCCESS) {
            qsee_log(QSEE_LOG_MSG_ERROR, "copy_S_BIGINT failed.");
            break;
         }

         // qsee_BIGINT_read_unsigned_bin interface test
         ret = qsee_BIGINT_read_unsigned_bin(&rsa_key.N->bi, pTmp, QSEE_RSA_KEY_SIZE(rsa_key.N));
         if (ret != E_SUCCESS) {
            qsee_log(QSEE_LOG_MSG_ERROR, "qsee_BIGINT_read_unsigned_bin failed.");
            break;
         } else {
            qsee_log(QSEE_LOG_MSG_DEBUG, "qsee_BIGINT_read_unsigned_bin succeeded.");
         }
      }

      // KDF interface test
      ret = qsee_kdf(NULL, 16, (void*)(uintnt)cipher_key_label, strlen(cipher_key_label), (void*)(uintnt)cipher_key_context, strlen(cipher_key_context), cipher_key, 16);
      if (ret != E_SUCCESS) {
         qsee_log(QSEE_LOG_MSG_ERROR, "qsee_kdf failed.");
         break;
      } else {
         qsee_log(QSEE_LOG_MSG_DEBUG, "qsee_kdf succeeded.");
      }

   }
   while (0);

   if (pTmp != NULL) {
      qsee_free(pTmp);
      pTmp = NULL;
   }

   if (rsa_key.d != NULL)
   {
      qsee_util_free_s_bigint(rsa_key.d);
   }

   if (rsa_key.N != NULL)
   {
      qsee_util_free_s_bigint(rsa_key.N);
   }

   if (rsa_key.p != NULL)
   {
      qsee_util_free_s_bigint(rsa_key.p);
   }

   if (rsa_key.q != NULL)
   {
      qsee_util_free_s_bigint(rsa_key.q);
   }

   if (rsa_key.e != NULL)
   {
      qsee_util_free_s_bigint(rsa_key.e);
   }

   return ret;
}


int tz_app_cmnlib_test(void)
{
   int status = E_SUCCESS;

   qsee_log(QSEE_LOG_MSG_DEBUG, "**********CMNLIB TESTS **********");

  status = test_crypto_ecc_complementary();
  if (status != E_SUCCESS) {
    qsee_log(QSEE_LOG_MSG_ERROR, "ECC Complemetary Tests failed.");
  } else {
    qsee_log(QSEE_LOG_MSG_DEBUG, "ECC Complemetary Tests PASS!");
  }

   status = test_crypto_rsa_enc_dec();
   if (status != E_SUCCESS) {
      qsee_log(QSEE_LOG_MSG_ERROR, "RSA Enc/Dec Tests failed.");
   }

   status = test_crypto_rsa_sign_verify();
   if (status != E_SUCCESS) {
      qsee_log(QSEE_LOG_MSG_ERROR, "RSA Sign/Verify Tests failed.");
   }

   status = test_crypto_rsa_complementary();
   if (status != E_SUCCESS) {
      qsee_log(QSEE_LOG_MSG_ERROR, "RSA Complemetary Tests failed.");
   }

   status = tz_app_sfs();
   if (status != E_SUCCESS) {
      qsee_log(QSEE_LOG_MSG_ERROR, "SFS Tests failed.");
   }

   // QSEE Log Test
   {
      int num = 10;
      char ch = 'A';
      char mystr[] = {"Hello From cmnlib Test"};
      uint32 wd = 0xDEADBEEF;

      qsee_log(QSEE_LOG_MSG_DEBUG, "QSEE LOG Test1: number = %d", num);
      qsee_log(QSEE_LOG_MSG_DEBUG, "QSEE LOG Test2: character = %c", ch);
      qsee_log(QSEE_LOG_MSG_DEBUG, "QSEE LOG Test3: %s", mystr);
      qsee_log(QSEE_LOG_MSG_DEBUG, "QSEE LOG Test4: %x", wd);
      qsee_log(QSEE_LOG_MSG_DEBUG, "QSEE LOG Test5: %d %c %s %x", num, ch, mystr, wd);
   }

   return status;
}
