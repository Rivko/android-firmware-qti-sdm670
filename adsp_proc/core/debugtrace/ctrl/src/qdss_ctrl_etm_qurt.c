/*=============================================================================

FILE:         qdss_ctrl_etm_qurt.c

DESCRIPTION:

================================================================================
              Copyright (c) 2013-2016 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential
==============================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include <stdio.h>
#include "stringl/stringl.h"
#include "com_dtypes.h"

#include "qdss_control.h"
#include "qdss_ctrl_etm.h"
#include "qurt/qurt_trace.h"
#include "qdss_parse_util.h"

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

// Bit flags for ETM parameters
#define QCEQ_ETM_PARAM_FLAG_MODE   0x01
#define QCEQ_ETM_PARAM_FLAG_ROUTE  0x02
#define QCEQ_ETM_PARAM_FLAG_FILTER 0x04
#define QCEQ_ETM_PARAM_FLAG_ALL    0x07

/*---------------------------------------------------------------------------
 * Type Declarations
 * ------------------------------------------------------------------------*/
#if 1 == WINDEV
   #define strcasecmp(a, b)  stricmp(a, b)
#endif

/*---------------------------------------------------------------------------
 * Static Variable Definitions
 * ------------------------------------------------------------------------*/

// Current setting of ETM trace configuration parameters.
struct
{
   uint32 mode;
   uint32 route;
   uint32 filter;
} etm_param_val;


/*---------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * ------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
// Convert QDSS Parse Utility return value to a QDSS Control return value.
static int qctl_qpu_to_rval(int qpu_rval)
{
   switch (qpu_rval)
   {
      case QDSS_PARSE_UTIL_RVAL_SUCCESS:
         return QDSS_CONTROL_RVAL_SUCCESS;
      case QDSS_PARSE_UTIL_RVAL_FAILED:
         return QDSS_CONTROL_RVAL_UNKNOWN_ERR;
      case QDSS_PARSE_UTIL_RVAL_MISSING_ARG:
         return QDSS_CONTROL_RVAL_MISSING_ARG;
      case QDSS_PARSE_UTIL_RVAL_INVALID_ARG:
         return QDSS_CONTROL_RVAL_INVALID_ARG;
      default:
         return QDSS_CONTROL_RVAL_UNKNOWN_ERR;
   }
}

/*---------------------------------------------------------------------------
 * Function Definitions
 * ------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
// Get the value (*val) of an ETM parameter (param_id).
int qdss_ctrl_etm_get_param(uint16 param_id, uint32 *pval)
{
   switch (param_id)
   {
      case QDSS_CTRL_ETM_PARAM_ID_DSP_MODE:
         *pval = etm_param_val.mode;
         break;
      case QDSS_CTRL_ETM_PARAM_ID_DSP_ROUTE:
         *pval =  etm_param_val.route;
         break;
      case QDSS_CTRL_ETM_PARAM_ID_DSP_FILTER:
         *pval = etm_param_val.filter;
         break;
      default:
         *pval = 0;
         return QDSS_CONTROL_RVAL_NOT_SUPPORTED;
   }
   return QDSS_CONTROL_RVAL_SUCCESS;
}
/*-------------------------------------------------------------------------*/
// Set the ETM parameter (param_id) to the value (val).
// The is no validation check on val. This is left to the QURT driver.
int qdss_ctrl_etm_set_param(uint16 param_id, uint32 val)
{
   switch (param_id)
   {
      case QDSS_CTRL_ETM_PARAM_ID_DSP_MODE:
         etm_param_val.mode = val;
         break;
      case QDSS_CTRL_ETM_PARAM_ID_DSP_ROUTE:
         etm_param_val.route = val;
         break;
      case QDSS_CTRL_ETM_PARAM_ID_DSP_FILTER:
         etm_param_val.filter = val;
         break;
      default:
         return QDSS_CONTROL_RVAL_UNKNOWN_ERR;
   }
   return QDSS_CONTROL_RVAL_SUCCESS;
}

/*-------------------------------------------------------------------------*/
// Get the ETM parameters (pget_str) values (reported in presp_str).
int qdss_ctrl_etm_get_config(const char *pget_str,
                             char *presp_str,
                             size_t resp_max_len)
{
   char name[16];       // Max token len + 1 for Null terminator
   char resp_buf[32];   // Temporary string for forming the response.
   const char **ppqry = &pget_str;
   char delims[] = {' ','\t','\n','\r'};  // Delimiters in query
   int qpu_rval;
   int flag = 0;

   memset(resp_buf, '\0', sizeof(resp_buf)); // Null terminate entire str.
   if (NULL == presp_str)
   {  // Nowhere to put response
      return QDSS_CONTROL_RVAL_UNKNOWN_ERR;
   }
   memset(presp_str, '\0', resp_max_len); // Null terminate entire str.
   if (NULL == pget_str)
   {  // Parameters to get not specified.
      return QDSS_CONTROL_RVAL_MISSING_ARG;
   }

   qpu_rval = qdss_parse_util_get_token(
      name, sizeof(name), ppqry, delims);
   if (qpu_rval !=  QDSS_PARSE_UTIL_RVAL_SUCCESS)
   {
      return qctl_qpu_to_rval(qpu_rval);
   }
   // Required keyword for this parser.
   if (strcasecmp(name, QDSS_CTRL_ETM_CONFIG_FORMAT_NAME))
   {
      return QDSS_CONTROL_RVAL_INVALID_ARG;
   }

   // Determine all the parameters being requested.
   while (0 < strlen(*ppqry))
   {
      qpu_rval = qdss_parse_util_get_token(
         name, sizeof(name), ppqry, delims);
      if (qpu_rval !=  QDSS_PARSE_UTIL_RVAL_SUCCESS)
      {
         return qctl_qpu_to_rval(qpu_rval);
      }

      if (!strcasecmp(name, "mode"))
      {
         flag |= QCEQ_ETM_PARAM_FLAG_MODE;
      }
      else if (!strcasecmp(name, "route"))
      {
         flag |= QCEQ_ETM_PARAM_FLAG_ROUTE;
      }
      else if (!strcasecmp(name, "filter"))
      {
         flag |= QCEQ_ETM_PARAM_FLAG_FILTER;
      }
      // else ignore unknown parameters
   }
   // Form the response string.
   strlcpy(presp_str, "ETM config", resp_max_len);
   strlcat(presp_str, QDSS_CTRL_ETM_CONFIG_FORMAT_NAME, resp_max_len);
   if (flag & QCEQ_ETM_PARAM_FLAG_MODE)
   {
      snprintf(resp_buf, sizeof(resp_buf),
               " mode 0x%lX", etm_param_val.mode);
      strlcat(presp_str, resp_buf, resp_max_len);
   }
   if (flag & QCEQ_ETM_PARAM_FLAG_ROUTE)
   {
      snprintf(resp_buf, sizeof(resp_buf),
               " route %lu", etm_param_val.route);
      strlcat(presp_str, resp_buf, resp_max_len);
   }
   if (flag & QCEQ_ETM_PARAM_FLAG_FILTER)
   {
      snprintf(resp_buf, sizeof(resp_buf),
               " filter 0x%lX", etm_param_val.filter);
      strlcat(presp_str, resp_buf, resp_max_len);
   }
   if (0 == flag)
   {
      strlcat(presp_str, "None", resp_max_len);
   }
   return QDSS_CONTROL_RVAL_SUCCESS;
}

/*-------------------------------------------------------------------------*/
/* Parse etm configuration command and save in the etm configuration
   database.
   Returns: QDSS_CONTROL_RVAL_...
      SUCCESS = execution of instruction completed successfully.
      UNKNOWN_ERR = Unexpected error occurred (primarily SW bug).
      NOT_SUPPORTED = Requested configuration (mode, route, etc.)
         not supported.
      INVALID_ARG = Parameter encountered has invalid format
      MISSING_ARG = Expected parameter is missing.
*/
int qdss_ctrl_etm_set_config(const char *pset_str)
{
   int rval = QDSS_CONTROL_RVAL_UNKNOWN_ERR;
   int qpu_rval;
   const char **ppcmd = &pset_str;
   size_t remaining_len;

   char name[16];       // Max token len + 1 for Null terminator
   unsigned long val = 0;
   char delims[] = {' ','\t', '\0'};  // Delimiters. Null terminated string.
   int param_id = QDSS_CTRL_ETM_PARAM_ID_DSP_NONE;

   if (NULL == pset_str)
   {
      return QDSS_CONTROL_RVAL_MISSING_ARG;
   }

   qpu_rval = qdss_parse_util_get_token(
      name, sizeof(name), ppcmd, delims);
   if (qpu_rval !=  QDSS_PARSE_UTIL_RVAL_SUCCESS)
   {
      return qctl_qpu_to_rval(qpu_rval);
   }
   // Required keyword for this parser.
   if (strcasecmp(name, QDSS_CTRL_ETM_CONFIG_FORMAT_NAME))
   {
      return QDSS_CONTROL_RVAL_INVALID_ARG;
   }

   // Process the entire command.
   remaining_len = strlen(*ppcmd);
   while (0 < remaining_len)
   {
      qpu_rval = qdss_parse_util_get_token(
         name, sizeof(name), ppcmd, delims);

      if (qpu_rval ==  QDSS_PARSE_UTIL_RVAL_SUCCESS)
      {
         if (0 == strlen(name))
         {  // No more tokens found.
            return QDSS_CONTROL_RVAL_SUCCESS;
         }
         if (!strcasecmp(name, "mode"))
         {
            param_id = QDSS_CTRL_ETM_PARAM_ID_DSP_MODE;
         }
         else if (!strcasecmp(name, "route"))
         {
            param_id = QDSS_CTRL_ETM_PARAM_ID_DSP_ROUTE;
         }
         else if (!strcasecmp(name, "filter"))
         {
            param_id = QDSS_CTRL_ETM_PARAM_ID_DSP_FILTER;
         }
         else
         {  // Error occurred, abort processing.
            return QDSS_CONTROL_RVAL_NOT_SUPPORTED;
         }
         qpu_rval = qdss_parse_util_get_ul(&val, ppcmd, delims);
         if (QDSS_PARSE_UTIL_RVAL_SUCCESS == qpu_rval)
         {
            rval = qdss_ctrl_etm_set_param(param_id, (unsigned int)val);
            if (QDSS_CONTROL_RVAL_SUCCESS != rval)
            {  // Error occurred, abort processing.
               return rval;
            }
         }
         else
         {  // Error occurred, stop processing.
            return qctl_qpu_to_rval(qpu_rval);
         }
      }
      // else not token found. Nothing to do.

      remaining_len = strlen(*ppcmd);
   }  // while more command string to process

   return QDSS_CONTROL_RVAL_SUCCESS;
}

/*-------------------------------------------------------------------------*/
//int qdss_ctrl_etm_enable(unsigned char etm_mode)
int qdss_ctrl_etm_enable(void)
{
   unsigned int ret1,ret2;
   ret1 =  qurt_etm_set_config(etm_param_val.mode,
                               etm_param_val.route,
                               etm_param_val.filter);
   ret2 = qurt_etm_enable(QURT_ETM_ON);
   return ((int)(ret1||ret2));
}

/*-------------------------------------------------------------------------*/
int qdss_ctrl_etm_disable(void)
{
   return ((int) qurt_etm_enable(QURT_ETM_OFF));
}

/*-------------------------------------------------------------------------*/
// Default setting for ETM trace configurable parameters.
void qdss_ctrl_etm_init_config_def(void)
{
   etm_param_val.mode = QURT_ETM_TYPE_PC_AND_MEMORY_ADDR;
   etm_param_val.route = QURT_ETM_ROUTE_TO_QDSS;
   etm_param_val.filter = QURT_ETM_TRACE_FILTER_ALL;
}


void qdss_qurt_lpm_enable(void) 
{
   qurt_etm_set_atb(QURT_ATB_ON);

}


