/*
 @file PEToolDLL.h
 @brief This file contains PETool API signature that can be used by any client
 @      to use the PEToolDLL.lib
 @ PETool can be used as Provisioning Encryption Tool
 @ Tool Version: 1.0
*===========================================================================
   Copyright (c) 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================
*/

#pragma once

#ifdef PETOOLDLL_EXPORTS
#define PETOOLDLL_API __declspec(dllexport) 
#else
#define PETOOLDLL_API __declspec(dllimport) 
#endif

#define OP_SUCCESS                         0
#define OP_FAILURE                        -1
#define OP_INSUFFICIENT_MEMORY            -2

/**
 * @brief This function executes the  Provisioning Encryption Tool API
 *
 * This function will allocate memory for the ciphertext output
 *
 * @param[in] ips                Image Provisioning Secret (4096 bytes)
 * @param[in] ipsLength          Length of the IPS (must be 4096)
 * @param[in] dps                Device Provisioning Secret (16 bytes)
 * @param[in] dpsLength          Length of the DPS (must be 16)
 * @param[in] plaintext          Data to Encrypt
 * @param[in] plaintextLength    Length fo the plaintext
 * @param[out] ciphertext        Output of the encryption (memory allocated by the function)
 * @param[out] ciphertextLength  Length of the ciphertext
 *
 * @return Zero on success, negative one on failure, negative two on insufficient output memory.
*/
PETOOLDLL_API
int __cdecl
PETool(unsigned char *ips, unsigned int ipsLength, unsigned char *dps, unsigned int dpsLength,
	   unsigned char *plaintext, unsigned int plaintextLength, unsigned char **ciphertext, unsigned int *ciphertextLength);
