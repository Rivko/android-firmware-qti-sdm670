/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      ERR Service Decompression Module
                
GENERAL DESCRIPTION
  implements code required to zlib decompress err data compressed in elf image

Copyright (c) 2013 - 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*==========================================================================

                        EDIT HISTORY FOR MODULE
$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/err/src/err_decompress.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/30/13   rks     CR526960: replace memcpy with memscpy
04/11/13   mcg     File created.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "err_decompress.h"
#include "rex.h"
#include "errlog.h"
#include "zlib.h"
#include "stringl.h"


/*===========================================================================

                    Enums for internal function prototypes

===========================================================================*/
typedef enum  {
                ERR_DECOMP_INFLATEINIT_FAILED,
                ERR_DECOMP_NO_AVAIL_DATA,
                ERR_DECOMP_Z_STREAM_ERROR,
                ERR_DECOMP_Z_NEED_DICT,
                ERR_DECOMP_Z_DATA_ERROR,
                ERR_DECOMP_Z_MEM_ERROR,
                ERR_DECOMP_STR_OVERFLOW,
                ERR_DECOMP_TWO_PASS_NO_STR_END,
                ERR_DECOMP_SWITCH_DEFAULT,
                ERR_DECOMP_GARBAGE_DATA,
                ERR_DECOMP_INVALID_SEARCH_ADDR,
                ERR_DECOMP_INVALID_PASS,
                ERR_DECOMP_DECOMP_OVERFLOW,
              } ErrDecompFails_t;


/*===========================================================================

                     EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/




/*===========================================================================

                     INTERNAL DATA DECLARATIONS

===========================================================================*/

// DEF_WBITS should be defined in zutil.h
// MAX_WBITS should be defined in zconf.h
#ifndef DEF_WBITS
#define DEF_WBITS MAX_WBITS
#endif // DEF_WBITS

#if (DEF_WBITS <= 0)
  #error DEF_WBITS <= 0
#endif // DEF_WBITS <= 0

// max amount of mem requested by zlib for its decompression window
#define                     ERR_MAX_INF_BLOCK       (1U << DEF_WBITS)

// these values are updated post-link
// follow the same format as image_vend
uintptr_t                   __ro_fatal_old_start
                              __attribute__((section(".data"))) = 0xffffffff;

uintptr_t                   __ro_fatal_new_start
                              __attribute__((section(".data"))) = 0xffffffff;

uintptr_t                   __ro_fatal_new_end
                              __attribute__((section(".data"))) = 0xffffffff;

// decompressed structs for err_SaveFatal[0-3]
err_const_type              err_decomp_msg_const;

// ERR_LOG_MAX_MSG_LEN is the max len copied to coredump in
// err_fatal_jettison_core, so use that as the max amount
static char                 fmt[ERR_LOG_MAX_MSG_LEN] = {0};

// 0x3000: approximate size of 'struct inflate_state' in inflate.h
// if the build breaks and the zlib version changed, please re-verify this
static unsigned char        zlib_blk[0x3000 + ERR_MAX_INF_BLOCK]
                              __attribute__((aligned(8))) = {0};

// keep track of the number of malloc calls made, s.t. we can free them later
static unsigned int         extern_malloc_calls = 0;

// offset from the start of &zlib_blk[0] to use in our "malloc"
static unsigned int         zlib_blk_offset = 0;

/*===========================================================================

                        External function prototypes

===========================================================================*/
extern void dog_force_kick(void);

/*===========================================================================

                        Private function prototypes

===========================================================================*/
static voidpf err_decompress_malloc(voidpf opaque, unsigned items, unsigned size);
static void err_decompress_free(voidpf opaque, voidpf ptr);
static void err_tear_down_decompression(ErrDecompFails_t fail_reason, err_decompress_struct_type* dest_ptr);



/*===========================================================================

                              Function definitions

===========================================================================*/

/*===========================================================================

FUNCTION       err_decompress_malloc

DESCRIPTION
  An allocation wrapper to return a static block of memory for zlib to use.

  Only to be used by err_decompress_msg_const

DEPENDENCIES
  Only should be called from within err_decompress_msg_const

RETURN VALUE
  A void pointer to the memory block

SIDE EFFECTS
  None.

===========================================================================*/
static voidpf err_decompress_malloc(voidpf opaque, unsigned items, unsigned size)
{
  size_t malloc_amount = (items * size + 0x7) & ~(0x7);

  if (zlib_blk_offset + malloc_amount > sizeof(zlib_blk)) {
    ++extern_malloc_calls;
    return (voidpf)malloc(malloc_amount);
  }

  zlib_blk_offset += malloc_amount;

  return (voidpf)(&zlib_blk[0] + zlib_blk_offset - malloc_amount);
}

/*===========================================================================

FUNCTION       err_decompress_free

DESCRIPTION
  An allocation wrapper to 'free' a static block of memory for zlib to use.
  Actually just zeros out memory for the next call of err_decompress_malloc.

  Only to be used by err_decompress_msg_const

DEPENDENCIES
  CAVEAT: YOU MUST FREE IN THE REVERSE ORDER THAT YOU CALLED MALLOC, ELSE BAD
  THINGS WILL HAPPEN

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void err_decompress_free(voidpf opaque, voidpf ptr)
{
  if (extern_malloc_calls > 0 &&
      (ptr < (voidpf)&zlib_blk[0] ||
      ptr > (voidpf)(&zlib_blk[0] + sizeof(zlib_blk)))) {
    --extern_malloc_calls;
    free(ptr);
    return;
  }

  if (opaque && ptr) return;
}

/*===========================================================================

FUNCTION       err_decompress_msg_const

DESCRIPTION

DEPENDENCIES
  - "fatal.struct.rodata" comes before "fatal.fmt.rodata" (see linker script)

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void err_decompress_msg_const (const err_const_type* const_blk_ptr, err_decompress_struct_type* dest_ptr)
{
  // place to store the decompressed data; don't allocate on stack since it's
  // pretty big
  static unsigned char  inf_out[ERR_DECOMP_CHUNK];

  int                   ret;

  size_t                cpy_amt;
  size_t                cpy_amt_temp;
  size_t                already_copied = 0;

  uintptr_t             avail_in;
  uintptr_t             compressed_read = 0;
  uintptr_t             decompressed_wrote = 0;

  uintptr_t             offset =
                          (uintptr_t)const_blk_ptr - __ro_fatal_old_start;

  unsigned int          inf_have;
  unsigned int          offset_in_chunk = 0;

  void*                 dst_start;
  void*                 src_start;

  z_stream              strm;

  enum                  {
                          ERR_DECOMP_START,
                          ERR_DECOMP_STRUCT,
                          ERR_DECOMP_STRING,
                          ERR_DECOMP_FIX_STRUCT,
                          ERR_DECOMP_END,
                        } err_decomp_state = ERR_DECOMP_START;

  enum                  {
                          ERR_DECOMP_FIRST_PASS,
                          ERR_DECOMP_SECOND_PASS,
                        } err_decomp_pass = ERR_DECOMP_FIRST_PASS;

  ERR_CRUMB_TRAIL_BMSK(ERR_CRUMB_TRAIL_BMSK_DECOMPRESS_MSG_PRE);

  if (__ro_fatal_old_start == 0xffffffff ||
      __ro_fatal_new_start == 0xffffffff ||
      __ro_fatal_new_end == 0xffffffff) {
    // no compressed section, copy over the struct and back out
    memscpy(
        (void *)(&(dest_ptr->msg_blk)),
        sizeof(err_const_type),
        (void *)const_blk_ptr,
        sizeof(err_const_type));

    return;
  }

  zlib_blk_offset = 0;

  strm.zalloc = err_decompress_malloc;
  strm.zfree = err_decompress_free;
  strm.opaque = Z_NULL;
  strm.avail_in = 0;
  strm.next_in = Z_NULL;

  if (inflateInit(&strm) != Z_OK) {
    err_tear_down_decompression(ERR_DECOMP_INFLATEINIT_FAILED, dest_ptr);
    return;
  }

  if ((uintptr_t)const_blk_ptr < __ro_fatal_old_start) {
    err_tear_down_decompression(ERR_DECOMP_INVALID_SEARCH_ADDR, dest_ptr);
    return;
  }

  do {
    dog_force_kick();
	
    strm.next_in = (Bytef *)(__ro_fatal_new_start + compressed_read);
    avail_in = __ro_fatal_new_end - (uintptr_t)strm.next_in;

    if (avail_in > ERR_DECOMP_CHUNK) {
      avail_in = ERR_DECOMP_CHUNK;
    }
    /* else if (avail_in < 0) {
      err_tear_down_decompression(ERR_DECOMP_NO_AVAIL_DATA, dest_ptr);
      return;
    } */
    else if (avail_in == 0) {
      break;
    }

    compressed_read += avail_in;
    strm.avail_in = avail_in;

    do {
      strm.avail_out = ERR_DECOMP_CHUNK;
      strm.next_out = inf_out;
      ret = inflate(&strm, Z_NO_FLUSH);

      switch (ret) {
        case Z_STREAM_ERROR:
          err_tear_down_decompression(ERR_DECOMP_Z_STREAM_ERROR, dest_ptr);
          (void)inflateEnd(&strm);
          return;

        case Z_NEED_DICT:
          err_tear_down_decompression(ERR_DECOMP_Z_NEED_DICT, dest_ptr);
          (void)inflateEnd(&strm);
          return;

        case Z_DATA_ERROR:
          err_tear_down_decompression(ERR_DECOMP_Z_DATA_ERROR, dest_ptr);
          (void)inflateEnd(&strm);
          return;

        case Z_MEM_ERROR:
          err_tear_down_decompression(ERR_DECOMP_Z_MEM_ERROR, dest_ptr);
          (void)inflateEnd(&strm);
          return;
      }

      inf_have = ERR_DECOMP_CHUNK - strm.avail_out;
      decompressed_wrote += (uintptr_t)inf_have;

      switch (err_decomp_state) {
        case ERR_DECOMP_START: // {{{
        if (offset > decompressed_wrote) {
          break;
        }

        // we've hit the struct we're meant to use
        err_decomp_state = ERR_DECOMP_STRUCT;
        //                (small)  - ((large)            - (large))
        offset_in_chunk = inf_have - (decompressed_wrote - offset);
        err_decomp_pass = ERR_DECOMP_FIRST_PASS;
        already_copied = 0;

        // }}}
        // fall through

        case ERR_DECOMP_STRUCT: // {{{
        // msg_diag_service.h:114
        // ----------------------
        //
        // typedef struct
        // {
        //   uint16     line;
        //   const char *fmt;
        //   const char *fname;
        // }
        // err_const_type;

        // byte copy from the decompression chunk into our struct
        if (err_decomp_pass == ERR_DECOMP_FIRST_PASS) {
          if (offset_in_chunk + sizeof(err_decomp_msg_const) < inf_have) {
            // we have the whole struct
            cpy_amt = sizeof(err_decomp_msg_const);
            err_decomp_state = ERR_DECOMP_STRING;
          }
          else {
            // we need to decompress more to get the whole struct
            cpy_amt = inf_have - offset_in_chunk;
          }

          /* KW hack, pretty sure we would abort long before getting here */
          if (offset_in_chunk >= sizeof(inf_out)) 
          {
            err_tear_down_decompression(ERR_DECOMP_DECOMP_OVERFLOW, dest_ptr);
            (void)inflateEnd(&strm);
            return;
          }
          src_start = inf_out + offset_in_chunk;
          err_decomp_pass = ERR_DECOMP_SECOND_PASS;
        }
        else if (err_decomp_pass == ERR_DECOMP_SECOND_PASS) {
          src_start = inf_out;
          cpy_amt = sizeof(err_decomp_msg_const) - already_copied;

          if (cpy_amt > inf_have) {
            err_tear_down_decompression(ERR_DECOMP_STR_OVERFLOW, dest_ptr);
            (void)inflateEnd(&strm);
            return;
          }

          err_decomp_state = ERR_DECOMP_STRING;
        }
        else {
          err_tear_down_decompression(ERR_DECOMP_INVALID_PASS, dest_ptr);
          (void)inflateEnd(&strm);
          return;
        }

        dst_start = (char *)&err_decomp_msg_const + already_copied;

        memscpy(dst_start, sizeof(err_decomp_msg_const) - already_copied, src_start, cpy_amt);
        already_copied += cpy_amt;

        if (err_decomp_state != ERR_DECOMP_STRING) {
          // we need the next chunk to complete decompression
          break;
        }

        offset = (uintptr_t)err_decomp_msg_const.fmt - __ro_fatal_old_start;

        err_decomp_state = ERR_DECOMP_STRING;

        err_decomp_pass = ERR_DECOMP_FIRST_PASS;

        already_copied = 0;

        // }}}
        // fall through

        case ERR_DECOMP_STRING: // {{{
        if (offset > decompressed_wrote) {
          break;
        }

        if (err_decomp_pass == ERR_DECOMP_FIRST_PASS) {
          //                (small)  - ((large)            - (large))
          offset_in_chunk = inf_have - (decompressed_wrote - offset);
        }
        else if (err_decomp_pass == ERR_DECOMP_SECOND_PASS) {
          offset_in_chunk = 0;
        }
        else {
          err_tear_down_decompression(ERR_DECOMP_INVALID_PASS, dest_ptr);
          (void)inflateEnd(&strm);
          return;
        }

        // assumption: at most a two-pass operation; we're kind of screwed if
        // this takes more than 2 passes
        src_start = inf_out + offset_in_chunk;
        cpy_amt = 0;

        // find out how much we have to copy; to a max of
        // ERR_LOG_MAX_MSG_LEN - 1 characters
        while (offset_in_chunk + cpy_amt < inf_have) {
          ++cpy_amt;

        cpy_amt_temp = cpy_amt;
	if ( cpy_amt > 0)
          cpy_amt_temp--;

          if ((cpy_amt + already_copied > ERR_LOG_MAX_MSG_LEN - 1) ||
              (*(inf_out + offset_in_chunk + cpy_amt_temp ) == '\0')) {
            // we have everything we need
            err_decomp_state = ERR_DECOMP_FIX_STRUCT;
            break;
          }
        }

        dst_start = (char *)fmt + already_copied;

        memscpy(dst_start, sizeof(fmt) - already_copied, src_start, cpy_amt);

        already_copied += cpy_amt;

        if (err_decomp_state != ERR_DECOMP_FIX_STRUCT) {
          if (err_decomp_pass == ERR_DECOMP_FIRST_PASS) {
            // some of the string resides in this chunk, the rest in the next
            // chunk; we have to copy what we have right now and copy the next
            // segment together on the next err_decomp_pass
            err_decomp_pass = ERR_DECOMP_SECOND_PASS;

            break;
          }
          else if (err_decomp_pass == ERR_DECOMP_SECOND_PASS) {
            err_tear_down_decompression(ERR_DECOMP_TWO_PASS_NO_STR_END, dest_ptr);
            (void)inflateEnd(&strm);
            return;
          }
          else {
            err_tear_down_decompression(ERR_DECOMP_INVALID_PASS, dest_ptr);
            (void)inflateEnd(&strm);
            return;
          }
        }

        // we have the whole string stored completely in fmt
        fmt[MIN(sizeof(fmt), already_copied) - 1] = '\0';
        err_decomp_state = ERR_DECOMP_FIX_STRUCT;

        // }}}
        // fall through

        // don't really need its own case
        case ERR_DECOMP_FIX_STRUCT: // {{{
        err_decomp_msg_const.fmt = fmt;

        err_decomp_state = ERR_DECOMP_END;

        // }}}
        break;

        default: // {{{
        err_tear_down_decompression(ERR_DECOMP_SWITCH_DEFAULT, dest_ptr);
        (void)inflateEnd(&strm);
        return;
        // }}}
      }

    } while (strm.avail_out == 0 && err_decomp_state != ERR_DECOMP_END);

  } while (compressed_read < __ro_fatal_new_end && err_decomp_state != ERR_DECOMP_END);

  if (err_decomp_state != ERR_DECOMP_END) {
    err_tear_down_decompression(ERR_DECOMP_GARBAGE_DATA, dest_ptr);
  }

  (void)inflateEnd(&strm);

  /* copy results to destination */
  memscpy(
    (void *)&(dest_ptr->msg_blk),
    sizeof(dest_ptr->msg_blk),
    (void *)&err_decomp_msg_const,
    sizeof(err_const_type)
  );
  /* copy the fmt string as well */
  memscpy(
    (void *)(dest_ptr->fmt_buf),
     sizeof(dest_ptr->fmt_buf),
    (void *)fmt,
    sizeof(dest_ptr->fmt_buf)
  );
  /* link to the copied fmt string */
  dest_ptr->msg_blk.fmt = dest_ptr->fmt_buf;

  return;
}

static void err_tear_down_decompression (ErrDecompFails_t fail_reason, err_decompress_struct_type* dest_ptr)
{
    static const char* const decompress_err_strings[] = {
      [ERR_DECOMP_INFLATEINIT_FAILED]   = "err_decompress: could not set "
                                          "initialize inflate",

      [ERR_DECOMP_NO_AVAIL_DATA]        = "err_decompress: available data is"
                                          "less than 0",

      [ERR_DECOMP_Z_STREAM_ERROR]       = "err_decompress: Z_STREAM_ERROR",

      [ERR_DECOMP_Z_NEED_DICT]          = "err_decompress: Z_NEED_DICT",

      [ERR_DECOMP_Z_DATA_ERROR]         = "err_decompress: Z_DATA_ERROR",

      [ERR_DECOMP_Z_MEM_ERROR]          = "err_decompress: Z_MEM_ERROR",

      [ERR_DECOMP_STR_OVERFLOW]         = "err_decompress: second struct "
                                          "decomp pass tried to read more "
                                          "data than avail",

      [ERR_DECOMP_TWO_PASS_NO_STR_END]  = "err_decompress: could not get err "
                                          "string after two passes",

      [ERR_DECOMP_SWITCH_DEFAULT]       = "err_decompress: hit the default "
                                          "case in the giant switch statement",

      [ERR_DECOMP_GARBAGE_DATA]         = "err_decompress: hit the end of the "
                                          "decomp fxn w/o getting the correct "
                                          "data",

      [ERR_DECOMP_INVALID_SEARCH_ADDR]  = "err_decompress: passed ERR_FATAL "
                                          " struct is before "
                                          "__ro_fatal_old_start",

      [ERR_DECOMP_INVALID_PASS]         = "err_decompress: invalid pass "
                                          "number",
      [ERR_DECOMP_DECOMP_OVERFLOW]      = "err_decompress: decompression buffer "
                                          "inf_out overflowed",
    };

    // followed format in core/api/debugtools/err.h
    dest_ptr->msg_blk.line = __LINE__;
    dest_ptr->msg_blk.fname = msg_file;
    dest_ptr->msg_blk.fmt = decompress_err_strings[fail_reason];
}


