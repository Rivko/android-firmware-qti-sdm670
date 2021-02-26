/******************************************************************************

  @file    qcril_file_utils.h
  @brief   Provides interface to communicate with files

  DESCRIPTION

  ---------------------------------------------------------------------------

  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef QCRIL_FILE_UTILS_H
#define QCRIL_FILE_UTILS_H

#include "qcrili.h"

/*===========================================================================

  FUNCTION qcril_file_open_file_for_read

===========================================================================*/
/*!
    @brief
    open file for reading.

    @return
    file identifier or NULL
*/
/*=========================================================================*/
void *qcril_file_open_file_for_read
(
    char *file_name
);

/*===========================================================================

  FUNCTION qcril_file_open_file_for_write

===========================================================================*/
/*!
    @brief
    open file for writing.

    @return
    file identifier or NULL
*/
/*=========================================================================*/
void *qcril_file_open_file_for_write
(
    char *file_name
);

/*===========================================================================

  FUNCTION qcril_file_create_file

===========================================================================*/
/*!
    @brief
    Create File.

    @return
    file identifier or NULL
*/
/*=========================================================================*/
void *qcril_file_create_file
(
    char *file_name
);

/*===========================================================================

  FUNCTION qcril_file_read_next_bytes

===========================================================================*/
/*!
    @brief
    reads the next byte from file

    @return
    file descriptor.
*/
/*=========================================================================*/
int qcril_file_read_next_bytes
(
    void *file_desc,
    void *destination,
    int   length
);

/*===========================================================================

  FUNCTION qcril_file_read_bytes_from_front

===========================================================================*/
/*!
    @brief
    reads bytes from the beginning of the file.

    @return
    file descriptor.
*/
/*=========================================================================*/
int qcril_file_read_bytes_from_front
(
    void *file_desc,
    void *destination,
    int   length
);

/*===========================================================================

  FUNCTION qcril_file_write_bytes_from_front

===========================================================================*/
/*!
    @brief
    writes bytes from the beginning of the file.

    @return
    file descriptor.
*/
/*=========================================================================*/
int qcril_file_write_bytes_from_front
(
    void          *file_desc,
    unsigned char *content,
    int           length
);

/*===========================================================================

  FUNCTION qcril_file_read_bytes_till_end

===========================================================================*/
/*!
    @brief
    reads bytes from the end - length of the file.

    @return
    file descriptor.
*/
/*=========================================================================*/
int qcril_file_read_bytes_till_end
(
    void *file_desc,
    void *destination,
    int   offset,
    int   length
);

/*===========================================================================

  FUNCTION qcril_file_get_size

===========================================================================*/
/*!
    @brief
    get file size.

    @return
    size on success
    -1 on error
*/
/*=========================================================================*/
int qcril_file_get_size
(
    void *file_desc
);

/*===========================================================================

  FUNCTION qcril_file_close_file

===========================================================================*/
/*!
    @brief
    close file.

    @return
    0 on success
    -1 on error
*/
/*=========================================================================*/
int qcril_file_close_file
(
    void *file_desc
);

/*===========================================================================

  FUNCTION qcril_file_write_a_string_to_file

===========================================================================*/
/*!
    @brief
    Utility to write a string given file name.

    @return
    0 on success
    -1 on error
*/
/*=========================================================================*/
int qcril_file_write_a_string_to_file
(
    char *file_name,
    char *string
);

/*===========================================================================

  FUNCTION qcril_file_copy_file

===========================================================================*/
/*!
    @brief
    Copy one file to another byte by byte.

    @return
    0 on success
    -1 on error
*/
/*=========================================================================*/
int qcril_file_copy_file
(
    char    *source,
    char    *destination
);

/*===========================================================================

  FUNCTION qcril_file_read_string_from_file

===========================================================================*/
/*!
    @brief
   Read upto the requested number of bytes. If the length of the string buffer
   to read into is less than the number of bytes read, the bytes read are
   null-terminated.

    @return
    0 on success
    -1 on error
*/
/*=========================================================================*/
int qcril_file_read_string_from_file
(
    char    *file_name,
    char    *string,
    uint32_t string_len,
    uint32_t bytes_to_read
);

void *qcril_file_open_mmap
(
    char *file_name,
    int open_flags,
    int mmap_prot,
    int mmap_flags,
    size_t *size
);

void *qcril_file_open_mmap_for_read
(
    char *file_name,
    size_t *size
);

void *qcril_file_open_mmap_for_write
(
    char *file_name,
    size_t *size
);

int qcril_file_close_mmap
(
    void *address,
    size_t size
);

void *qcril_file_open_mmap_at
(
    int dirfd,
    char *file_name,
    int open_flags,
    int mmap_prot,
    int mmap_flags,
    size_t *size
);

void *qcril_file_open_mmap_at_for_read
(
    int dirfd,
    char *file_name,
    size_t *size
);

void *qcril_file_open_mmap_at_for_write
(
    int dirfd,
    char *file_name,
    size_t *size
);
#endif /* QCRIL_FILE_UTILS_H */
