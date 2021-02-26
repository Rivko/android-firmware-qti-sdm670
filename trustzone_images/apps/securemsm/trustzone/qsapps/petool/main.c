// PEToolLibTest.cpp : Defines the entry point for the console application.
//

#include "PEToolLib.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>

//#include <fstream>
#include <time.h>
#include <sys/stat.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <limits.h>


#define DPS_SIZE 16
#define IPS_SIZE 4096
#define SEED_LEN 16
#define KEY_LEN 16
#define _CRT_SECURE_NO_DEPRECATE

#define err(fmt, ...) \
    fprintf(stderr, "error: " fmt "\n", ##__VA_ARGS__)


#define OP_SUCCESS                         0
#define OP_FAILURE                        -1
#define OP_INSUFFICIENT_MEMORY            -2

int FileSize(const char *filename) {
   struct stat st;

   if (stat(filename, &st) == 0)
      return st.st_size;

   return 0;
}

int fopen_s(FILE **f, const char *name, const char *mode) {
    *f = fopen(name, mode);
    return NULL != *f ? 0 : errno;
}

int main(int argc, const char * argv[])
{
   unsigned char  *inputBytes = NULL;
   unsigned int   inputFileLength = 0;

   unsigned char  ipsBytes[IPS_SIZE];
   unsigned char  device_provisioning_seed[DPS_SIZE];

   unsigned char  *ciphertext = NULL;
   unsigned int   ciphertext_len = 0; // Will be computed - rounds up plaintext_len to nearest 16 bytes
   unsigned char  *iv = NULL;

   FILE           *fp = NULL;
   //errno_t        err = OP_FAILURE;
   int r;

   size_t         bytes = 0;
   int            ret = OP_FAILURE;

   if (argc != 5) {
      printf("USAGE: PETool [outputFile] [inputFile] [IPS] [DPS]\n");
      printf("IPS ... %d ... DPS ... %d...", IPS_SIZE, DPS_SIZE);
      return 0;
   }

    const char* path_output = argv[1];
    const char* path_input  = argv[2];
    const char* path_IPS    = argv[3];
    const char* path_DPS    = argv[4];

   /*
   Open Input File and read the data in a buffer
   */
   r = fopen_s(&fp, path_input, "rb");
   if (r != OP_SUCCESS || fp == NULL) {
      printf("ERROR: Input File %s does not exist\n", path_input);
      goto bail;
   }

   //Verify size of input file
   inputFileLength = FileSize(path_input);
   if (inputFileLength == 0) {
      printf("ERROR: Empty input file\n");
      goto bail;
   }

   //Allocate memory for the input file
   inputBytes = (unsigned char *)malloc(inputFileLength);
   if (inputBytes == NULL) {
      printf("ERROR: Allocating memory for Input File\n");
      goto bail;
   }

   //Read in the input file
   bytes = fread(inputBytes, 1, inputFileLength, fp);
   if (bytes != inputFileLength) {
      printf("ERROR: File Read did not return expected bytes.\n");
      goto bail;
   }

   //Close the input file
   fclose(fp);
   fp = NULL;

   /*
   Open Input IPS File and read the data in a buffer
   */
   r = fopen_s(&fp, path_IPS, "rb");
   if (r != 0 || fp == NULL) {
      printf("ERROR: IPS File %s does not exist\n", path_IPS);
      goto bail;
   }

   //Ensure the IPS is 4K
   if (FileSize(path_IPS) != IPS_SIZE) {
      printf("ERROR: IPS is not %d bytes\n", IPS_SIZE);
      goto bail;
   }

   //Read the IPS
   bytes = fread(ipsBytes, 1, IPS_SIZE, fp);
   if (bytes != IPS_SIZE) {
      printf("ERROR: IPS File Read did not return expected (%d) bytes.\n", IPS_SIZE);
      goto bail;
   }

   //Close the input IPS file
   fclose(fp);
   fp = NULL;

   /*
   Open Input DPS File and read the data in a buffer
   */
   r = fopen_s(&fp, path_DPS, "rb");
   if (r != 0 || fp == NULL) {
      printf("ERROR: DPS File %s does not exist\n", path_DPS);
      goto bail;
   }

   //Ensure the DPS is 16 bytes
   if (FileSize(path_DPS) != DPS_SIZE) {
      printf("ERROR: DPS is not %d bytes\n", DPS_SIZE);
      goto bail;
   }

   //Read the DPS
   bytes = fread(device_provisioning_seed, 1, DPS_SIZE, fp);
   if (bytes != DPS_SIZE) {
      printf("ERROR: DPS File Read did not return expected (%d) bytes.\n", DPS_SIZE);
      goto bail;
   }

   //Close the input DPS file
   fclose(fp);
   fp = NULL;

   /*
   Call into the tool library to encrypt the input data with provided IPS/DPS
   */
   ret = PETool(ipsBytes, IPS_SIZE, device_provisioning_seed, DPS_SIZE, inputBytes, inputFileLength, NULL, &ciphertext_len);
   if (ret == OP_INSUFFICIENT_MEMORY) {
      //printf("ERROR: Need to allocate memory to CipherText buffer.\n");
   }
   else if (ret != OP_SUCCESS) {
      printf("ERROR: PETool returned failure: %d", ret);
      goto bail;
   }

   ciphertext = (unsigned char *)malloc(ciphertext_len);
   if (ciphertext == NULL) {
      printf("ERROR: Allocating memory for Cipher Text File\n");
      goto bail;
   }

   ret = PETool(ipsBytes, IPS_SIZE, device_provisioning_seed, DPS_SIZE, inputBytes, inputFileLength, ciphertext, &ciphertext_len);
   if (ret != OP_SUCCESS) {
      printf("ERROR: PETool returned failure: %d", ret);
      goto bail;
   }

   /*
   Open output File in which encrypted data is to be copied
   */
   r = fopen_s(&fp, path_output, "wb");

   if (r != 0 || fp == NULL) {
      printf("ERROR: Couldn't open output file %s\n", path_output);
      goto bail;
   }


   bytes = fwrite(ciphertext, ciphertext_len, 1, fp);

   if(1 != bytes) {
      ret = OP_FAILURE;
      printf("Write error occurred\n");
      goto bail;
   }
   printf("INFO: Wrote %zd bytes to output file\n", bytes * ciphertext_len);

   fclose(fp);
   fp = NULL;

   ret = OP_SUCCESS;
bail:
   if (inputBytes)
      free(inputBytes);
   if (ciphertext)
      free(ciphertext);
   if (fp)
      fclose(fp);



   return ret;
}
