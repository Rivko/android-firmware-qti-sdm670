#ifndef QDSS_UTILS_H
#define QDSS_UTILS_H

/*=============================================================================

FILE:         qdss_utils.h

DESCRIPTION:  

================================================================================
            Copyright (c) 2014 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/
#include "DALStdErr.h"

#define TRY(nErr, func) \
   if (DAL_SUCCESS != (nErr = (func))) {\
      goto nErr##bail; \
   }

#define THROW(nErr, errno) \
   nErr = errno; \
   goto nErr##bail;

#define THROW_IF(nErr, condition, errno)        \
   if (condition) {                             \
      nErr = errno;                             \
      goto nErr##bail;                          \
   }


#define CATCH(nErr) nErr##bail: if (nErr != DAL_SUCCESS)

#define IGNORE(x) (void)(x)


#endif //QDSS_UTILS_H
