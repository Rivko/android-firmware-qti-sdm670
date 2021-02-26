/*
*===========================================================================
   Copyright (c) 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

#include <stdint.h>




#define OP_SUCCESS                         0
#define OP_FAILURE                        -1
#define OP_INSUFFICIENT_MEMORY            -2

/**
 * @brief This function executes the  Provisioning Encryption Tool API
 *
 *
 * @param[in] ips                Image Provisioning Secret (4096 bytes)
 * @param[in] ipsLength          Length of the IPS (must be 4096)
 * @param[in] dps                Device Provisioning Secret (16 bytes)
 * @param[in] dpsLength          Length of the DPS (must be 16)
 * @param[in] plaintext          Data to Encrypt
 * @param[in] plaintextLength    Length fo the plaintext
 * @param[out] ciphertext        Output of the encryption
 * @param[out] ciphertextLength  Length of the ciphertext
 *
 * @return Zero on success, negative one on failure, negative two on insufficient output memory.
*/
__attribute__ ((visibility ("default")))
int PETool(unsigned char *ips, unsigned int ipsLength, unsigned char *dps, unsigned int dpsLength,
    unsigned char *plaintext, unsigned int plaintextLength, unsigned char *ciphertext, unsigned int *ciphertextLength);
