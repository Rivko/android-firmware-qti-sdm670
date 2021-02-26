#ifndef GD_ERRORS_H
#define GD_ERRORS_H

/*===========================================================================
   Copyright (c) 2013 Qualcomm Technologies, Inc.       
   All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tz/1.0.2/securemsm/sse/qsee/SecureDisplay/libgd/include/gd_errors.h#1 $
  $DateTime: 2018/02/06 03:00:17 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
07/02/13   ts       Initial version.

===========================================================================*/

#if !defined _WIN32 && !defined __ARMCC
    # include <syslog.h>
#else
/*
 * priorities/facilities are encoded into a single 32-bit quantity, where the
 * bottom 3 bits are the priority (0-7) and the top 28 bits are the facility
 * (0-big number).  Both the priorities and the facilities map roughly
 * one-to-one to strings in the syslogd(8) source code.  This mapping is
 * included in this file.
 *
 * priorities (these have been edited to matched qsee_log values)
 */
# define LOG_LOW        0
# define LOG_MED        1
# define LOG_HIGH       2
# define LOG_ERR        3 
# define LOG_FATAL      4
# define LOG_DEBUG      5
#endif

#define GD_ERROR LOG_ERR
#define GD_WARNING LOG_HIGH
#define GD_NOTICE LOG_MED
#define GD_INFO LOG_LOW
#define GD_DEBUG LOG_DEBUG

void gd_error(const char *format, ...);
void gd_error_ex(int priority, const char *format, ...);

#endif
