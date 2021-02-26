/***********************************************************************
 * fs_unicode_utf8.c
 *
 * Conversion functions for UTF-8 Unicode filenames
 * Copyright (C) 2006-2009,2013 QUALCOMM Technologies, Inc.
 *
 * These functions provide filename translation to/from UTF-8
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-10-09   nr    Replace banned memory functions with safer versions.
2013-05-24   wek   Make unicode tables and ops as constant.
2009-09-02   yg    Take extension into consideration for LFN decision.
2009-04-22   ebb   Removed memory protection support.
2008-06-27   np    Added support for hfat files with spaces in the name.
2007-07-10   sh    Added Shift-JIS short filename conversion support.
2007-06-28   sh    Correct OEM Codepage lookup to use a byte value.
2007-05-25   umr   Added support for EFS memory protection feature.
2007-03-28   yg    Fix missing . problem in special file names.
2007-03-20   sh    Prefix all UTF conversion functions with fs_
2007-03-12   rp    Return exact UTF conversion results to the caller.
2007-03-09   sh    More static-ness.
2007-03-08   yg    Some code cleanup.
2007-03-02   yg    Made function static.
2007-02-16   yg    Fix implicit cast related problem.
2007-02-14   yg    Added OEM code page support and short file name logic.
2007-02-13   sh    Apply maximum length limits.
2006-12-04   sh    Created stubs
===========================================================================*/

#include "fs_unicode.h"
#include "fs_unicode_sjis.h"
#include "fs_unicode_utf8.h"
#include "fs_convert_utf.h"
#include "string.h"
#include "crc.h"
#include "fs_errno.h"
#include "fs_util.h"

#define IS_LOWER_CASE(x)   (((x) >= 'a') && ((x) <= 'z'))
#define IS_UPPER_CASE(x)   (((x) >= 'A') && ((x) <= 'Z'))
#define IS_DIGITS(x)       (((x) >= '0') && ((x) <= '9'))

#define TO_UPPER_CASE(x)   ((((x) >= 'a') && ((x) <= 'z')) ? \
                            ((x) + 'A' - 'a') : (x))

/*
 * Map Microsoft OEM Code Page 437 to a corresponding Unicode
 * codepoint.
 *
 * This encoding is used ONLY in the short 8.3 filename from media
 * that originated on CodePage437 machines.
 * (Use "mode con" or "chcp" to check the codepage from DOS.)
 * http://www.microsoft.com/globaldev/reference/oem/437.mspx
 *
 * Index is the CP437 value (0-255)
 * Value is the Unicode UTF16 value (all are single words)
 */
static const UTF16 OEM_code_page_437[] = {
  0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,
  0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F,
  0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017,
  0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D, 0x001E, 0x001F,
  0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027,
  0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F,
  0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
  0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F,
  0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
  0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,
  0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
  0x0058, 0x0059, 0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F,
  0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
  0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,
  0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
  0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D, 0x007E, 0x007F,
  0x00C7, 0x00FC, 0x00E9, 0x00E2, 0x00E4, 0x00E0, 0x00E5, 0x00E7,
  0x00EA, 0x00EB, 0x00E8, 0x00EF, 0x00EE, 0x00EC, 0x00C4, 0x00C5,
  0x00C9, 0x00E6, 0x00C6, 0x00F4, 0x00F6, 0x00F2, 0x00FB, 0x00F9,
  0x00FF, 0x00D6, 0x00DC, 0x00A2, 0x00A3, 0x00A5, 0x20A7, 0x0192,
  0x00E1, 0x00ED, 0x00F3, 0x00FA, 0x00F1, 0x00D1, 0x00AA, 0x00BA,
  0x00BF, 0x2310, 0x00AC, 0x00BD, 0x00BC, 0x00A1, 0x00AB, 0x00BB,
  0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556,
  0x2555, 0x2563, 0x2551, 0x2557, 0x255D, 0x255C, 0x255B, 0x2510,
  0x2514, 0x2534, 0x252C, 0x251C, 0x2500, 0x253C, 0x255E, 0x255F,
  0x255A, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256C, 0x2567,
  0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256B,
  0x256A, 0x2518, 0x250C, 0x2588, 0x2584, 0x258C, 0x2590, 0x2580,
  0x03B1, 0x00DF, 0x0393, 0x03C0, 0x03A3, 0x03C3, 0x00B5, 0x03C4,
  0x03A6, 0x0398, 0x03A9, 0x03B4, 0x221E, 0x03C6, 0x03B5, 0x2229,
  0x2261, 0x00B1, 0x2265, 0x2264, 0x2320, 0x2321, 0x00F7, 0x2248,
  0x00B0, 0x2219, 0x00B7, 0x221A, 0x207F, 0x00B2, 0x25A0, 0x00A0,
};

static int
unicode_res_to_efs_errno (ConversionResult conv_result)
{
  int efs_errno_map;

  switch (conv_result) {
    case conversionOK:
      efs_errno_map = ENOERR;
      break;

    case sourceExhausted:
    case targetExhausted:
      efs_errno_map = -ENAMETOOLONG;
      break;

    case sourceIllegal:
      efs_errno_map = -EILSEQ;
      break;

    default:
      /* we should never have got here */
      efs_errno_map = -EINVAL;
      break;
  }

  return efs_errno_map;
}

static int
utf8_to_utf16 (const char *char_name, UTF16 *utf16_name, int max_words)
{
  ConversionResult result;
  ConversionFlags flags = strictConversion;
  UTF16 *p_utf16;
  const UTF8 *utf8_name;
  int efs_err_retval;

  utf8_name = (UTF8 *)char_name;
  p_utf16 = utf16_name;
  result = fs_ConvertUTF8toUTF16 (&utf8_name,
                                  utf8_name + strlen (char_name) + 1,
                                  &p_utf16,
                                  utf16_name + max_words,
                                  flags);

  efs_err_retval = unicode_res_to_efs_errno(result);
  return efs_err_retval;
}

static int
utf8_from_utf16 (const UTF16 *utf16_name, char *utf8_name, int max_bytes)
{
  ConversionResult result;
  ConversionFlags flags = strictConversion;
  const UTF16 *p_utf16;
  UTF8 *p_utf8;
  int efs_err_retval;

  p_utf8 = (UTF8 *)utf8_name;
  p_utf16 = utf16_name;
  result = fs_ConvertUTF16toUTF8 (&p_utf16,
                                  p_utf16 + 1024, /* XXX */
                                  &p_utf8, p_utf8 + max_bytes, flags);

  efs_err_retval = unicode_res_to_efs_errno(result);
  return efs_err_retval;
}

static int
is_valid_char_in_short_name (unsigned char ch)
{
  if (ch > 0x7F)
    return 0;

  if (IS_UPPER_CASE(ch) || IS_LOWER_CASE(ch) || IS_DIGITS(ch))
  {
    return 1;
  }

  switch (ch)
  {
    case '$':    case '%':    case '\'':    case '-':
    case '_':    case '@':    case '~':    case '`':
    case '!':    case '(':    case ')':    case '{':
    case '}':    case '^':    case '#':    case '&':
      return 1;

    default:
      return 0;
  }

}

static int
utf16_to_shortname (int pass,
                    const UTF16 *utf16,
                    struct fs_fat_shortname *shortname)
{
  int len;
  int a, b;
  int dotsfromstart = 0;
  int extdot = -1;
  unsigned long crc_num;
  int needs_long_filename;

  /* A sanity check of all pointers */
  if ((shortname == 0) || (utf16 == 0) ||
      (shortname->name == 0) || (shortname->ext == 0))
    return 0;

  /*
   * Algorithm for a file name to end up in LFN entry:
   * a) If the file name length excluding ext is greater than 8 bytes.
   * b) If the file extension length is greater than 3 bytes.
   * c) If the file name length is greater than 12 bytes.
   * d) If the file name has special chars that are not allowed in short file.
   * e) If a collision is detected (pass > 1).
   */

  needs_long_filename = 0;

  /* Pass code is more than 1 means there was an entry found with the same
   * name, so now force the name conversion to use crc */
  if (pass > 1)
    needs_long_filename = 1;

  /* calculate the length and other characteristics of the filename */
  for (len = 0; utf16[len] != 0; len++)
  {
    /* If the char is out side the ASCII range then we need conversion */
    if (utf16[len] > 0x7F)
      needs_long_filename = 1;
  }

  /* If total file name len doesn't fit in 11 chars (without .) then it has
   * to be Long file name entry. */
  if (len > 12)
    needs_long_filename = 1;

  /* Prefill needed if our chars won't fill name, ext completely */
  (void) memset (shortname->name, ' ', 8);
  (void) memset (shortname->ext, ' ', 3);

  /* check if it starts with '.' e.g. ".foo" */
  if (utf16[0] == '.')
  {
    /* calculates the starting dots */
    for (a = 0; a < len; a++)
    {
      if (utf16[a] != '.')
        break;
      dotsfromstart++;
    }
  }

  /* generating extension */
  for (a = len; a > 0; a--)
  {
    if (utf16[a - 1] == '.')
    {
      int extlen = 0;
      /* check if we reach the starting dots, in this case this
       * is not an extension */
      if (a == dotsfromstart)
        break;
      extdot = a - 1;

      extlen = len - a;
      if (extlen > 3)
        needs_long_filename = 1;

      for (b = 0; b < 3; a++)
      {
        /* Short file name should only have UPPER case chars. NTRES flag can be
         * used for conversion, but here instead of NTRES, it results into long
         * file name component */
        UTF16 ch = (UTF16) TO_UPPER_CASE(utf16[a]);
        if (!ch)
          break;

        /* Space is an acceptable character in filename extension.
         * Spaces or invalid characters in filename are
         * handled by creating a long file name.
         * We are mapping any outside range char to _    */
        if ((ch == ' ') || !is_valid_char_in_short_name (ch))
        {
          needs_long_filename = 1;
          ch = '_';
        }

        shortname->ext[b++] = (char) ch;
      }
      break;
    }
  }

  /* if extdot == -1 then the file doesn't have extension,
   * or the file name start from . */
  if (extdot == -1)
  {
    if (len > 8)
      needs_long_filename = 1;
  }
  else
  {
    if (extdot > 8)
      needs_long_filename = 1;
  }

  /* Copy file name if conversion not needed. But if illegal chars
   * are there, then the conversion is needed. */
  if (needs_long_filename == 0)
  {
    for (a = b = 0; b < 8; a++)
    {
      /* Upper case needed */
      UTF16 ch = (UTF16) TO_UPPER_CASE (utf16[a]);
      if (!ch)
        break;

      if (ch == '.')
      {
        /* if we reach extension dot then stop */
        if (a == extdot)
          break;
      }

      /* Space is an acceptable character in the filename.
       * Spaces or invalid characters in filename are
       * handled by creating a long file name.              */
      if ( (ch == ' ') || (!is_valid_char_in_short_name (ch)))
      {
        needs_long_filename = 1;
        break;
      }

      shortname->name[b++] = (char) ch;
    }
  }

  /* This cannot be else case of above since there is possibility in the
   * above conditional code for this flag to change state. So potentially
   * both the statements of these should be executed. */
  if (needs_long_filename == 1)
  {
    unsigned char cch;

    /* Came here means we cannot have the short file name, so generate
     * a unique file name by calculating the CRC of the long file name
     * if a collision happens then we can generate a different file
     * name which would be a very rare occurrence. */

    /* Calculate CRC30 of the file name, so that it is unique */
    crc_num = crc_30_step (pass, (byte*)utf16, (len * sizeof (*utf16) * 8));

    /* Convert CRC to string for file name part */
    for (a = 7; a >= 0; --a)
    {
      cch = crc_num & 0xF;
      cch += ((cch < 10) ? '0' : ('A' - 10));
      shortname->name[a] = cch;
      crc_num >>= 4;
    }

  }

  return len;
}

/*
 * This conversion function is used to convert a short FAT filename
 * into UTF16.  (When a long filename is present, it is used instead.)
 *
 * The specific codepage is important when a short file name has
 * extended (non-ASCII) characters placed in the fileanme by a
 * non-AMSS host, such as a desktop PC with its default locale.
 *
 * The locale set on the PC should match the locale implemented here
 * (Codepage 437 or equivalent) in order to get correct results.  If
 * the files were created using a different codepage than this one,
 * then this conversion will produce incorrect results.
 */
static int
utf16_from_cp437_shortname (const struct fs_fat_shortname *shortname,
                            UTF16 *utf16)
{
  int i, len;
  unsigned char uch;

  /* Sanity check on pointers */
  if ((shortname == 0) || (utf16 == 0) ||
      (shortname->name == 0) || (shortname->ext == 0))
    return -1;

  /* Determine how many valid characters are in the name */
  for (len = 8; len > 0; len--)
    if (shortname->name[len - 1] != ' ')
      break;

  /* Convert them from char to UTF16 using the selected code page */
  for (i = 0; i < len; i++)
  {
    uch = shortname->name[i];
    *utf16++ = OEM_code_page_437[uch];
  }

  /* Determine how many valid chars are in the extension */
  for (len = 3; len > 0; len--)
    if (shortname->ext[len - 1] != ' ')
      break;

  /* If we really have some valid chars in ext, then we need . in name */
  if (len)
    *utf16++ = '.';

  /* Convert the ext chars to UTF16 using the selected code page */
  for (i = 0; i < len; i++)
  {
    uch = shortname->ext[i];
    *utf16++ = OEM_code_page_437[uch];
  }

  /* Need NULL terminator */
  *utf16++ = 0;

  return 0;
}

/* Helper function for Shift-JIS translation.  This takes a shortname
 * string (interpreted as Shift-JIS) and adds the characters to a
 * given UTF-16 string.
 *
 * The (advanced) UTF-16 pointer is returned.
 */
static UTF16 *
utf16_append_sjis_string (const unsigned char *s,
                          unsigned int count,
                          UTF16 *utf16_name)
{
  unsigned char uch;
  unsigned int i, end;

  /* End is the first character after the end of the name. The first
   * non-space character from the trailing end can help determine the
   * end of the name. */

  end = count;
  while (end)
  {
    if (s[end - 1] != ' ') {
      break;
    }
    end--;
  }

  for (i = 0; i < end; i++)
  {
    uch = s[i];                 /* First input byte. */

    /*
     * The variable 'end' indicates the end of the name.
     * The ASCII set (0x00-0x7F) is a straight 1:1 match.
     * 0xA1 - 0xDF (inclusive) map to 0xFF61 - 0xFF9F (Halfwidth Katakana)
     * Anything else is either a double-byte or invalid.
     */
    if (uch <= 0x7F)
      *utf16_name++ = (UTF16) uch;   /* ASCII */
    else if ((uch >= 0xA1) && uch <= 0xDF)
      *utf16_name++ = (UTF16) uch - 0xA1 + 0xFF61; /* Katakana */
    /* We can have a double-byte character where the second byte
     * might be a space positioned at "end". So we should not break
     * the loop on the if condition (i == end - 1) */
    else if (i == (count - 1))
      break;                    /* This IS the last byte, so no double-byte */
    else
    {
      i++;                      /* Decode double-character S-JIS */
      *utf16_name++ = fs_sjis_dcbs_to_utf16 (uch, s[i]);
    }
  }

  return utf16_name;                 /* Return the updated string */
}


/*
 * Windows machines in CodePage 932 may write short filenames using
 * "Shift-JIS" character encoding.
 *
 * Additionally, there is a substitution of 0x05 for 0xE5 in the first
 * character.
 *
 * This conversion function translates such shortnames to UTF16.
 */
static int
utf16_from_sjis_shortname (const struct fs_fat_shortname *shortname,
                           UTF16 *utf16_name)
{
  unsigned char sname[8];       /* NOT null-terminated */

  /* Sanity check on pointers */
  if ((shortname == 0) || (utf16_name == 0) ||
      (shortname->name == 0) || (shortname->ext == 0))
    return -1;

  /* We can not modify the actual shortname in memory, since we might
   * insert the "DELETED" marker in the next step.  This will confuse
   * the HFAT code that is sharing this string with us. */
  fs_memscpy (sname, sizeof(sname), shortname->name, sizeof(sname));

  /* Here we have to convert any 0x05 into the 0xE5 that it actually
   * represents.  Since 0xE5 is both the deleted-file marker and a
   * valid S-JIS lead byte, MS chose to substitute the value 0x05 into
   * the name.  This needs adjustment before it can be translated to
   * UTF16. */
  if (sname[0] == 0x05)
    sname[0] = 0xE5;

  /* Convert the name portion into UTF16 */
  utf16_name = utf16_append_sjis_string (sname, 8, utf16_name);

  /* If there is any (non-space) extension, make sure we add a dot
   * after the name itself.  (No extension means no dot.) */
  if (shortname->ext[0] != ' ') {
    *utf16_name++ = '.';

    /* Convert the three extension characters to UTF16 as well */
    utf16_name = utf16_append_sjis_string (shortname->ext, 3, utf16_name);
  }

  /* Need a NULL terminator, regardless */
  *utf16_name = 0;

  return 0;
}


/*
 * The following structures define "sets" of conversion operations.
 * These should not be called directly, but used as assignment values
 * for the 'fs_convert' pointer.
 *
 * Note that lThese structures should be the ONLY non-static symbols
 * defined in this file.
 */

/* Western Generic: Shortnames are treated as CodePage 437 */
const struct unicode_conversion_ops fs_convert_utf8 = {
  utf8_to_utf16,
  utf8_from_utf16,
  utf16_to_shortname,
  utf16_from_cp437_shortname,
};

/* Japanese: Shift-JIS
 * Existing shortnames are treated as Shift-JIS. (CP932)
 *
 * Note that we do not create new Shift-JIS shortnames -- those will
 * always get longnames.
 */
const struct unicode_conversion_ops fs_convert_utf8_sjis = {
  utf8_to_utf16,
  utf8_from_utf16,
  utf16_to_shortname,           /* Creates ASCII shortnames */
  utf16_from_sjis_shortname,    /* Parses S-JIS shortnames */
};
