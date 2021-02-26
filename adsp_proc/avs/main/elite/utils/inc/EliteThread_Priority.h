/*========================================================================*/
/** @file EliteThread_Priority.h

This file contains definitions of thread priority of
Elite services.  The thread priority is represented by a number,
and the number increases as the thread priority increases.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF. 
      The description that displays in the PDF is located in the
      Elite_mainpage.dox file. Contact Tech Pubs for assistance.
===========================================================================*/

/*======================================================================
Copyright (c) 2010-2011, 2013-2016 Qualcomm Technologies, Inc.  All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*========================================================================
Edit History


when       who     what, where, why
--------   ---     -------------------------------------------------------
08/25/15   sw      (Tech Pubs) Added name group comment from 8952.2.6;
                   added Doxygen condition commands.
09/05/14   sw      (Tech Pubs) Merged Doxygen markup from 2.4.
03/20/13   sw      (Tech Pubs) Updated Doxygen markup/comments for 2.0.
01/14/11   dc      Added (featurized) thread priorities for 8200A
10/28/10   sw      (Tech Pubs) Edited/added Doxygen comments and markup.
02/22/10   WJ       Created file.

========================================================================== */
#ifndef ELITETHREAD_PRIORITY_H
#define ELITETHREAD_PRIORITY_H
#ifndef MDSPMODE
#include "../../../../qdsp6/tp/inc/tp.h"


#ifdef AUDIOFW_8200A_PL3

#include "task_priority.h"

#else

/** @addtogroup elite_svc_thread_priorities
@{ */
/** @name &nbsp;
    @vertspace{-26}
The thread priority is represented by a number, and the number increases
as the thread priority increases.
@{ */

/* The thread priorities in this file are being used by outside AVS component.
 * This file be removed eventually. This is not for AVS thread priorities anymore */


/* Ultrasound thread priority and other dependencies. These will be removed once Ultrasound removes this dependency with ELite */
#define ELITETHREAD_STAT_ULTRASOUND_STREAM_SVC_PRIO   (255 - (AVS_MEDIUM_PRIO_LOW + 0))
#define ELITETHREAD_DYNA_PP_SVC_PRIO                  (255 - (AVS_MEDIUM_PRIO_HIGH + 11))
#define ELITETHREAD_DYNA_ENC_SVC_PRIO                 (255 - (AVS_MEDIUM_PRIO_HIGH + 12))
#define ELITETHREAD_DYNA_DEC_SVC_PRIO                 (255 - (AVS_MEDIUM_PRIO_HIGH + 12))
/* APR thread priority. This will be removed once APR removes this dependency with ELite */
#define ELITETHREAD_CVP_HIGH_PRIO                     (255 - (AVS_HIGH_PRIO_HIGH + 0))


#endif  // AUDIOFW_8200A
#else //MDSPMODE ENDS

/** @addtogroup elite_svc_thread_priorities on MDSP
@{ */
/** @name &nbsp;
    @vertspace{-26}
@{ */
//The following set of thread priorities are copied as such from 8909

/* Ultrasound thread priority and other dependencies. These will be removed once Ultrasound removes this dependency with ELite */
#define ELITETHREAD_STAT_ULTRASOUND_STREAM_SVC_PRIO   (255 - 161)
#define ELITETHREAD_DYNA_DEC_SVC_PRIO                 (255 - 161)
#define ELITETHREAD_DYNA_ENC_SVC_PRIO                 (255 - 161)
#define ELITETHREAD_DYNA_PP_SVC_PRIO                  (255 - 160)

/* APR thread priority. This will be removed once APR removes this dependency with ELite */
#define ELITETHREAD_CVP_PRIO                        (255 - 119)
#define ELITETHREAD_CVP_HIGH_PRIO                     ELITETHREAD_CVP_PRIO


#endif
/** @} */ /* end_name */
/** @} */ /* end_addtogroup elite_svc_thread_priorities */

#endif

