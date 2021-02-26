/**@file
  Implements RFC1952 and uses zlib for decompression 

  Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  Portions Copyright (c) 2009 - 2010, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#include <Library/DebugLib.h>
#include <Library/ZlibDecompress.h>

#include "zlib.h"

/* Scratch buffer used by zlib for dictionaries, etc. */
#define SCRATCH_BUFFER_REQUEST_SIZE   SIZE_128KB

/* Defined in RFC 1952 */
#define GZIP_HEADER_ID1         0x1F
#define GZIP_HEADER_ID2         0x8B
#define GZIP_HEADER_CM_DEFLATE  8

/* RFC1952 doesn't impose limitation on string lengths in the header, but since we are 
   skipping CRC check, a corrupted file header could result in delay before we 
   hit a NULL termination during string parsing. Filename string does not include 
   subdirectory path and comments are uncommon so we will limit string length to catch
   errors sooner */
#define GZIP_STRING_LIMIT     256

/* GZIP header defined in RFC 1952 */
typedef struct {
  UINT8 Id1;        /* IDentification 1 */
  UINT8 Id2;        /* IDentification 2 */
  UINT8 Cm;         /* Compression Method */
  UINT8 Flg;        /* FLaGs */
  UINT8 Mtime[4];   /* Modification TIME */  
  UINT8 Xfl;        /* eXtra FLags */
  UINT8 Os;         /* Operating System */ 
} GZIP_HEADER;

/* Flags defined in RFC 1952 */
typedef union {
  struct {
    UINT8 Ftext:1;     /* File is ASCII text */
    UINT8 Fhcrc:1;     /* CRC16 for the gzip header is present */
    UINT8 Fextra:1;    /* Optional extra fields are present */
    UINT8 Fname:1;     /* Original file name is present, terminated by a zero byte */
    UINT8 Fcomment:1;  /* A zero-terminated file comment is present */
    UINT8 Rsvd1:3;
  } Bits;
  UINT8   Uint8;
} GZIP_FLAG;

/* Zlib Scratch Buffer */
struct {
  VOID    *Buffer;
  UINTN   BufferSize;
} ScratchBuffer = { NULL, 0 };


/**
  Allocation routine used by zlib decompression algorithm

  Returns the allocated pointer address, or NULL on failure
**/
STATIC
VOID *
ZlibBufferAlloc (
  VOID   *Opaque, 
  UINT32 Items,
  UINT32 Size
  )
{
  VOID   *Addr;
  UINT32 TotalSize = Items * Size;

  if (ScratchBuffer.Buffer ==  NULL || ScratchBuffer.BufferSize == 0)
  {
    DEBUG ((EFI_D_ERROR, "zlib scratch buffer not initialized before use!\r\n"));
    ASSERT_EFI_ERROR (EFI_BAD_BUFFER_SIZE);
    return NULL;
  }

  if (ScratchBuffer.BufferSize >= TotalSize) 
  {
    Addr = ScratchBuffer.Buffer;
    ScratchBuffer.Buffer = (VOID*) ((UINT8*)Addr + TotalSize);
    ScratchBuffer.BufferSize -= TotalSize;
    return Addr;
  } 
  else 
  {
    DEBUG ((EFI_D_ERROR, "Out of zlib scratch buffer space!\r\n"));
    ASSERT_EFI_ERROR (EFI_OUT_OF_RESOURCES);
    return NULL;
  } 
}

/**
  Free routine used by zlib decompression

**/
STATIC
VOID
ZlibBufferFree (
  VOID *Opaque,
  VOID *Addr
  )
{
  /*  We use the 'scratch buffer' for allocations, so there is no free
      operation required.  The scratch buffer will be freed by the caller
      of the decompression code. */
}

/**
  Get the size of the uncompressed buffer by parsing EncodeData header.

  Returns the size of the uncompressed buffer.
**/
STATIC
UINT32
GetDecodedSize(
  UINT8  *EncodedData,
  UINT32 SourceSize
  )
{
  UINT32 ISize; 
  UINT8  *Byte;
  UINT32 i = 0;

  /* RFC1952 requires the last 4 bytes (ISIZE) of the file to contain the
     size of the original (uncompressed) input data modulo 2^32. 
     ASSUMPTION: Original file is less than 4GB */
  Byte = EncodedData + SourceSize - 1;
  ISize = *Byte--;
  while (++i < 4) {
    ISize <<= 8;
    ISize += *Byte--; 
  };

  return ISize; 
}

STATIC
BOOLEAN
CheckZlibHeader (
  GZIP_HEADER *Header
  )
{
  if(Header->Id1 != GZIP_HEADER_ID1 ||
     Header->Id2 != GZIP_HEADER_ID2 ||
     Header->Cm  != GZIP_HEADER_CM_DEFLATE)
  {
    DEBUG ((EFI_D_ERROR, "Invalid zlib header!\r\n"));
    ASSERT(EFI_INVALID_PARAMETER);
    return FALSE; 
  }

  return TRUE;
}

/**
  Given a Zlib compressed source buffer, this function retrieves the size of 
  the uncompressed buffer and the size of the scratch buffer required 
  to decompress the compressed source buffer.

  Retrieves the size of the uncompressed buffer and the temporary scratch buffer 
  required to decompress the buffer specified by Source and SourceSize.
  The size of the uncompressed buffer is returned in DestinationSize, 
  the size of the scratch buffer is returned in ScratchSize, and EFI_SUCCESS is returned.

  @param  Source          The source buffer containing the compressed data.
  @param  SourceSize      The size, in bytes, of the source buffer.
  @param  DestinationSize A pointer to the size, in bytes, of the uncompressed buffer
                          that will be generated when the compressed buffer specified
                          by Source and SourceSize is decompressed.
  @param  ScratchSize     A pointer to the size, in bytes, of the scratch buffer that
                          is required to decompress the compressed buffer specified 
                          by Source and SourceSize.

  @retval  EFI_SUCCESS    The size of the uncompressed data was returned 
                          in DestinationSize and the size of the scratch 
                          buffer was returned in ScratchSize.

**/
EFI_STATUS
EFIAPI
ZlibGetInfo (
  IN  CONST VOID  *Source,
  IN  UINT32      SourceSize,
  OUT UINT32      *DestinationSize,
  OUT UINT32      *ScratchSize
  )
{
  if (Source == NULL || DestinationSize == NULL || ScratchSize == NULL ||
      SourceSize < sizeof(GZIP_HEADER))
  {
    return EFI_INVALID_PARAMETER;
  }

  if (!CheckZlibHeader((GZIP_HEADER *)Source))
  {
    return EFI_INVALID_PARAMETER;
  }

  *DestinationSize = GetDecodedSize((UINT8*)Source, SourceSize); 
  *ScratchSize = SCRATCH_BUFFER_REQUEST_SIZE;
  return EFI_SUCCESS;
}

/**
  Decompresses a zlib compressed source buffer.

  PRECONDITION: Destination buffer and Scratch buffer must be at least the size
                returned by ZlibGetInfo()
  
  Extracts decompressed data to its original form.
  If the compressed source data specified by Source is successfully decompressed 
  into Destination, then EFI_SUCCESS is returned.  If the compressed source data 
  specified by Source is not in a valid compressed data format,
  then EFI_INVALID_PARAMETER is returned.

  @param  Source      The source buffer containing the compressed data.
  @param  SourceSize  The size of source buffer.
  @param  Destination The destination buffer to store the decompressed data
  @param  Scratch     A temporary scratch buffer that is used to perform the decompression.
                      This is an optional parameter that may be NULL if the 
                      required scratch buffer size is 0.
                     
  @retval  EFI_SUCCESS Decompression completed successfully, and 
                          the uncompressed buffer is returned in Destination.
  @retval  EFI_INVALID_PARAMETER 
                          The source buffer specified by Source is corrupted 
                          (not in a valid compressed format).
**/
EFI_STATUS
EFIAPI
ZlibDecompress(
  IN CONST VOID  *Source,
  IN UINTN       SourceSize,
  IN OUT VOID    *Destination,
  IN OUT VOID    *Scratch
  )
{
  GZIP_HEADER       *Header;
  GZIP_FLAG         Flags;
  UINTN             i = 0;
  UINT32            Xlen;
  struct z_stream_s Stream;
  INT32             rc;
  UINTN             DecompressedSize;

  if (Source == NULL || Destination == NULL || Scratch == NULL ||
      SourceSize < sizeof(GZIP_HEADER))
  {
    return EFI_INVALID_PARAMETER;
  }

  /* Header sanity check */
  Header = (GZIP_HEADER *)Source;
  if (!CheckZlibHeader(Header))
  {
    return EFI_INVALID_PARAMETER;
  }

  DecompressedSize = GetDecodedSize((UINT8*)Source, SourceSize); 

  /* ASUMPTION: Function PRECONDITION has been met */
  ScratchBuffer.Buffer = Scratch;
  ScratchBuffer.BufferSize = SCRATCH_BUFFER_REQUEST_SIZE;

  /* Initialize Stream */
  Stream.zalloc    = ZlibBufferAlloc; 
  Stream.zfree     = ZlibBufferFree; 
  Stream.next_out  = Destination; 
  Stream.avail_out = DecompressedSize; 
 
  /* Skip Header */
  Stream.next_in  = (UINT8*)Source + sizeof(GZIP_HEADER); 
  Stream.avail_in = SourceSize - sizeof(GZIP_HEADER);
  
  Flags = (GZIP_FLAG)Header->Flg;

	/* Skip "Extra Field" if present */
  if (Flags.Bits.Fextra)
  {
    /* XLEN defined in little endian format */
    Xlen = *Stream.next_in++;
    Xlen += (*Stream.next_in++) << 8; 
    Stream.avail_in -= 2;

    Stream.next_in += Xlen;
    Stream.avail_in -= Xlen;
  }

  /* Skip "File Name" if present */
  if (Flags.Bits.Fname)
  {
    for (i = 0; i < GZIP_STRING_LIMIT && *Stream.next_in++; i++) {
      ASSERT(Stream.avail_in != 0);
      Stream.avail_in--;
    }
  }
  /* Test for corruption */
  ASSERT(i < GZIP_STRING_LIMIT);

  /* Skip "Comment" if present */
  if (Flags.Bits.Fcomment)
  {
    for (i = 0; i < GZIP_STRING_LIMIT && *Stream.next_in++; i++) {
      ASSERT(Stream.avail_in != 0);
      Stream.avail_in--;
    }
  }
  /* Test for corruption */
  ASSERT(i < GZIP_STRING_LIMIT);

  /* Skip "CRC16" if present */
  if (Flags.Bits.Fhcrc)
  {
    Stream.next_in+=2;
    Stream.avail_in-=2;
  }

  /* Non-zero reserved bits could indicate corruption or the presence of a 
     new field that could cause subsequent data to be interpreted incorrectly.
     Output a warning and continue. */
  if (Flags.Bits.Rsvd1 != 0)
  {
    DEBUG((EFI_D_WARN, "Non-zero reserved bits found!\r\n"));
  }

  /* Stream points to the compressed blocks and is ready
     for consumption by zlib */
  rc = inflateInit2(&Stream, -MAX_WBITS);
  ASSERT (rc == Z_OK);

  rc = inflate(&Stream, 0);
  ASSERT (rc == Z_STREAM_END || rc == Z_OK);

  (VOID) inflateEnd(&Stream);

  /* Sanity check expected decompressed size against decompressed size 
     returned by zlib */
  ASSERT(Stream.total_out == DecompressedSize);

  return EFI_SUCCESS; 
}

