/*
#============================================================================
#  Name:
#    servreg_localdb_user.c 
#
#  Description:
#    File containing API's to access Service Registry local database
#                                                                            
# Copyright (c) 2015-2017 by Qualcomm Technologies, Inc.  All Rights Reserved.        
#============================================================================
*/
#include "stdarg.h"
#include "stdlib.h"
#include <stringl/stringl.h>
#include "comdef.h"             /* Definitions for byte, word, etc.     */
#include "err.h"
#include "tms_utils_msg.h"

#include "servreg_internal.h"
#include "servreg_localdb.h"

/* From servreg_local_db_autogen.c file */
/*
extern servreg_entry_t servreg_local_services[] __attribute__((weak));
extern char *servreg_local_domain __attribute__((weak));
extern char *servreg_soc_name __attribute__((weak));
extern char *servreg_domain_name __attribute__((weak));
extern char *servreg_subdomain_name __attribute__((weak));
extern uint32_t servreg_service_count __attribute__((weak));
extern uint32_t servreg_qmi_instance_id __attribute__((weak));
*/

/* Default weak symbols to wlan pd for multi si */
const servreg_entry_t servreg_local_services[]={
   {"tms/servreg", 0 , 0},
};
const uint32_t servreg_service_count = 1;
const char *servreg_local_domain = "msm/modem/wlan_pd";
const char *servreg_soc_name = "msm";
const char *servreg_domain_name = "modem";
const char *servreg_subdomain_name = "wlan_pd";
const uint32_t servreg_qmi_instance_id = 180 ;

#define SERVREG_RCESN_NAME_PREFIX          "sr:"
#define SERVREG_RCESN_NAME_PREFIX_LEN       3

#define SERVREG_SYNC_NAME_UP_PREFIX         "sr_up:"
#define SERVREG_SYNC_NAME_DOWN_PREFIX       "sr_dn:"
#define SERVREG_SYNC_NAME_PREFIX_LEN        6

#define SERVREG_SSR_NAME_PREFIX             "ssr:"
#define SERVREG_SSR_NAME_PREFIX_LEN         4
#define SERVREG_SSR_NAME_DOWN_SUFFIX        ":before_shutdown"
#define SERVREG_SSR_NAME_DOWN_SUFFIX_LEN    16
#define SERVREG_SSR_NAME_UP_SUFFIX          ":after_powerup"
#define SERVREG_SSR_NAME_UP_SUFFIX_LEN      14

/** =====================================================================
 * Function:
 *     servreg_get_local_domain
 *
 * Description:
 *     Function to get the local domain info i.e "soc/domain/subdomain" string
 *     Queries the local database
 *
 *     Do not modify string return by this API as it is read only memory.
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     string containing soc/domain/subdomain info
 * =====================================================================  */
SERVREG_NAME servreg_get_local_domain(void)
{
   return (SERVREG_NAME)servreg_local_domain;
}

/** =====================================================================
 * Function:
 *     servreg_get_local_soc_scope
 *
 * Description:
 *     Function to get the local soc scope info
 *     Queries the local database
 *
 *     Do not modify string return by this API as it is read only memory.
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     "soc" string
 * =====================================================================  */
SERVREG_NAME servreg_get_local_soc_scope(void)
{
   return (SERVREG_NAME)servreg_soc_name;
}

/** =====================================================================
 * Function:
 *     servreg_get_local_domain_scope
 *
 * Description:
 *     Function to get the local domain scope
 *     Queries the local database
 *
 *     Do not modify string return by this API as it is read only memory.
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     "domain" string
 * =====================================================================  */
SERVREG_NAME servreg_get_local_domain_scope(void)
{
   return (SERVREG_NAME)servreg_domain_name;
}

/** =====================================================================
 * Function:
 *     servreg_get_local_subdomain_scope
 *
 * Description:
 *     Function to get the local subdomain scope
 *     Queries the local database
 *
 *     Do not modify string return by this API as it is read only memory.
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     "subdomain" string
 * =====================================================================  */
SERVREG_NAME servreg_get_local_subdomain_scope(void)
{
   return (SERVREG_NAME)servreg_subdomain_name;
}

/** =====================================================================
 * Function:
 *     servreg_get_local_instance_id
 *
 * Description:
 *     Function to get the local qmi instance id
 *     Queries the local database
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     qmi_instance_id for the notifier module
 * =====================================================================  */
uint32_t servreg_get_local_instance_id(void)
{
   return servreg_qmi_instance_id;
}

/** =====================================================================
 * Function:
 *     servreg_get_service_list_count
 *
 * Description:
 *     Function to get the local qmi instance id
 *     Queries the local database
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     qmi_instance_id for the notifier module
 * =====================================================================  */
uint32_t servreg_get_service_list_count(void)
{
   return servreg_service_count;
}

/** =====================================================================
 * Function:
 *     servreg_name_check
 *
 * Description:
 *     This function checks if the given name is a valid service name or not.
 *
 * Parameters:
 *     name : "soc/domain/subdomain/provider/service" or just "soc/domain/subdomain" info
 *
 * Returns:
 *     SERVREG_SUCCESS       : If name is valid
 *     SERVREG_INVALID_PARAM : If name is invalid
 * =====================================================================  */
SERVREG_RESULT servreg_name_check(SERVREG_NAME domain, SERVREG_NAME service)
{
   SERVREG_NAME sub_name = SERVREG_NULL, str_domain = domain, str_service = service;
   SERVREG_RESULT ret = SERVREG_INVALID_PARAM;
   uint32_t d_count = 0, s_count = 0;

   if(SERVREG_NULL != str_domain)
   {
      while(*str_domain != '\0')
      {
         if(strncmp(str_domain, "/", 1) == 0)
         {
            d_count = d_count + 1;
         }
         str_domain ++;
      }

      if(SERVREG_NULL != str_service)
      {
         while(*str_service != '\0')
         {
            if(strncmp(str_service, "/", 1) == 0)
            {
               s_count = s_count + 1;
            }
            str_service++;
         }

         if(d_count == 2)
         {
            sub_name = strstr(domain , servreg_soc_name);

            if(NULL != sub_name && sub_name == domain)
            {
               if(s_count == 1)
               {
                  ret = SERVREG_SUCCESS;
               }
            }
         }
      }
      else
      {
         /* domain = "soc/domain/subdomain/provider/service" or just domain = "soc/domain/subdomain" */
         if(d_count == 2 || d_count == 4)
         {
            sub_name = strstr(domain , servreg_soc_name);

            if(NULL != sub_name && sub_name == domain)
            {
               ret = SERVREG_SUCCESS;
            }
         }
      }
   }

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_create_rcesn_name
 *
 * Description:
 *     This function takes in the service name and returns the rcesn sync name
 *
 * Parameters:
 *     name : "soc/domain/subdomain/provider/service" or just "soc/domain/subdomain" info
 *
 * Returns:
 *     rcecb sync name : "sr:soc/domain/subdomain/provider/service" or just "sr:soc/domain/subdomain" 
 * =====================================================================  */
SERVREG_NAME servreg_create_rcesn_name(SERVREG_NAME name)
{
   SERVREG_NAME rcesn_name = SERVREG_NULL;
   uint32_t len = 0;

   if(SERVREG_NULL != name)
   {
      len = SERVREG_RCESN_NAME_PREFIX_LEN + servreg_nmelen(name) + 1;
      rcesn_name = (SERVREG_NAME)servreg_calloc(sizeof(char), len);

      if(SERVREG_NULL != rcesn_name)
      {
         strlcpy(rcesn_name, SERVREG_RCESN_NAME_PREFIX, len);
         strlcat(rcesn_name, name, len);
      }
      else
      {
         TMS_MSG_ERROR("Calloc failed");
      }
   }
   else
   {
      TMS_MSG_ERROR("Name NULL");
   }

   return rcesn_name;
}

/** =====================================================================
 * Function:
 *     servreg_create_sync_name
 *
 * Description:
 *     This function takes in the service name and returns the rcecb sync name
 *
 * Parameters:
 *     name : "soc/domain/subdomain/provider/service" or just "soc/domain/subdomain" info
 *
 * Returns:
 *     rcecb sync name : "sr_xx:soc/domain/subdomain/provider/service" or just "sr_xx:soc/domain/subdomain" 
 *                        sr_xx : sr_up or sr_down supported for now
 * =====================================================================  */
SERVREG_NAME servreg_create_sync_name(SERVREG_NAME name, SERVREG_SERVICE_STATE state)
{
   SERVREG_NAME sync_name = SERVREG_NULL;
   uint32_t len = 0;

   if(SERVREG_NULL != name)
   {
      len = SERVREG_SYNC_NAME_PREFIX_LEN + servreg_nmelen(name) + 1;
      sync_name = (SERVREG_NAME)servreg_calloc(sizeof(char), len);

      if(SERVREG_NULL != sync_name)
      {
         if(SERVREG_SERVICE_STATE_UP == state)
         {
            strlcpy(sync_name, SERVREG_SYNC_NAME_UP_PREFIX, len);
            strlcat(sync_name, name, len);
         }
         else if(SERVREG_SERVICE_STATE_DOWN == state)
         {
            strlcpy(sync_name, SERVREG_SYNC_NAME_DOWN_PREFIX, len);
            strlcat(sync_name, name, len);
         }
         else
         {
            servreg_free(sync_name);
            sync_name = SERVREG_NULL;
            TMS_MSG_ERROR("Invalid state");
         }
      }
      else
      {
         TMS_MSG_ERROR("Calloc failed");
      }
   }
   else
   {
      TMS_MSG_ERROR("Name NULL");
   }

   return sync_name;
}

/** =====================================================================
 * Function:
 *     servreg_create_ssr_name
 *
 * Description:
 *     This function takes in the service name and returns the before shutdown ssr message name
 *
 * Parameters:
 *     name : "soc/domain/subdomain/provider/service" or just "soc/domain/subdomain" info
 *     state : state of the service. UP or DOWN
 *
 * Returns:
 *     ssr name : "ssr:####:before_shutdown" or "ssr:####:after_powerup" #### will be the name extracted from the input param domain
 * =====================================================================  */
SERVREG_NAME servreg_create_ssr_name(SERVREG_NAME domain, SERVREG_SERVICE_STATE state)
{
   SERVREG_NAME sub_name = SERVREG_NULL, ssr_name = SERVREG_NULL, sub_name1 = SERVREG_NULL, sub_name2 = SERVREG_NULL;
   uint32_t sub_name_len = 0, len = 0;

   /* Using strstr function. As name can be domain+service or just domain */
   sub_name = strstr(domain , "/");
   sub_name1 = strstr(domain , "/");

   if(NULL != sub_name)
   {
      sub_name++;
      sub_name1++;

      while(*sub_name != '/')
      {
         sub_name_len++;
         sub_name++;
      }

      /* First create a null terminated string that has only the domain info from the soc/domain/subdomain string */
      sub_name2 = (SERVREG_NAME)servreg_calloc(sizeof(char), (sub_name_len + 1));

      if(SERVREG_NULL != sub_name2)
      {
         strlcpy(sub_name2, sub_name1, sub_name_len + 1);

         if(SERVREG_SERVICE_STATE_DOWN == state)
         {
            len = SERVREG_SSR_NAME_PREFIX_LEN + sub_name_len + SERVREG_SSR_NAME_DOWN_SUFFIX_LEN + 1;
            ssr_name = (SERVREG_NAME)servreg_calloc(sizeof(char), len);
            if(SERVREG_NULL != ssr_name)
            {
               strlcpy(ssr_name, SERVREG_SSR_NAME_PREFIX, len);
               strlcat(ssr_name, sub_name2, len);
               strlcat(ssr_name, SERVREG_SSR_NAME_DOWN_SUFFIX, len);

               servreg_free(sub_name2);
            }
            else
            {
               TMS_MSG_ERROR("Calloc failed");
            }
         }
         else if(SERVREG_SERVICE_STATE_UP == state)
         {
            len = SERVREG_SSR_NAME_PREFIX_LEN + sub_name_len + SERVREG_SSR_NAME_UP_SUFFIX_LEN + 1;
            ssr_name = (SERVREG_NAME)servreg_calloc(sizeof(char), len);
            if(SERVREG_NULL != ssr_name)
            {
               strlcpy(ssr_name, SERVREG_SSR_NAME_PREFIX, len);
               strlcat(ssr_name, sub_name2, len);
               strlcat(ssr_name, SERVREG_SSR_NAME_UP_SUFFIX, len);

               servreg_free(sub_name2);
            }
            else
            {
               TMS_MSG_ERROR("Calloc failed");
            }
         }
      }
      else
      {
         TMS_MSG_ERROR("Calloc failed");
      }
   }
   else
   {
      TMS_MSG_ERROR("strstr() failed");
   }
   return ssr_name;

}

/** =====================================================================
 * Function:
 *     servreg_is_local
 *
 * Description:
 *     This function checks if the domain is local or remote
 *     Given domain is said to be local if "soc/domian/subdomain" scopes matches with
 *     the local "soc/domian/subdomain" string
 *
 * Parameters:
 *     name : "soc/domain/subdomain/provider/service" or just "soc/domain/subdomain" info
 *
 * Returns:
 *     TRUE  : If domain is local
 *     FALSE : If domain is remote
 * =====================================================================  */
SERVREG_BOOL servreg_is_local(SERVREG_NAME name)
{
   SERVREG_NAME sub_name = SERVREG_NULL;
   SERVREG_BOOL ret = SERVREG_FALSE;

   /* Using strstr function. As name can be domain+service or just domain */
   sub_name = strstr(name , servreg_local_domain);

   if(NULL != sub_name)
   {
      ret = SERVREG_TRUE;
   }
   else
   {
      ret = SERVREG_FALSE;
   }
   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_is_domain_local
 *
 * Description:
 *     This function checks if the domain is local or remote for root-pd vs user-pd
 *     if root-pd : name is local if "soc/domain" matches with that of the root-pd's "soc/domain"
 *     if user-pd : local if "soc/domian/subdomain" scopes matches with the local "soc/domian/subdomain" string
 *
 * Parameters:
 *     name : "soc/domain/subdomain/provider/service" or just "soc/domain/subdomain" info
 *
 * Returns:
 *     TRUE  : If domain is local
 *     FALSE : If domain is remote
 * =====================================================================  */
SERVREG_BOOL servreg_is_domain_local(SERVREG_NAME name)
{
   SERVREG_NAME sub_name = SERVREG_NULL;
   SERVREG_BOOL ret = SERVREG_FALSE;

   if(strcmp(servreg_subdomain_name, "root_pd") == 0)
   {
      /* Check if the service name is in the same subsystem */
      sub_name = strstr(name , servreg_domain_name);
      if(NULL != sub_name)
      {
         ret = SERVREG_TRUE;
      }
      else
      {
         ret = SERVREG_FALSE;
      }
   }
   else
   {
      ret = servreg_is_local(name);
   }

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_is_service_local
 *
 * Description:
 *     This function checks if the service is local or remote for root-pd and user-pd
 *
 * Parameters:
 *     name : "soc/domain/subdomain/provider/service" or just "soc/domain/subdomain" info
 *
 * Returns:
 *     TRUE  : If service is local
 *     FALSE : If service is remote
 * =====================================================================  */
SERVREG_BOOL servreg_is_service_local(SERVREG_NAME name)
{
   SERVREG_NAME sub_name = SERVREG_NULL;
   SERVREG_BOOL ret = SERVREG_TRUE;
   uint32_t i, count = 0;

   /* Check if the service name is in the same pd */
   if(SERVREG_NULL != name)
   {
      sub_name = strstr(name , servreg_subdomain_name);
      if(NULL != sub_name)
      {
         ret = SERVREG_TRUE;
      }
      else
      {
         for(i = 0; i < strlen(name); i++)
         {
            if(name[i] == '/')
            {
               count++;
            }
         }
         
         if(count >= 3)
         {
            ret = SERVREG_FALSE;
         }
         else
         {
            ret = SERVREG_TRUE;
         }
      }
   }
   else
   {
      ret = SERVREG_FALSE;
   }

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_concat
 *
 * Description:
 *     This functions concatenates "soc/domain/subdomain" and "provider/service"
 *     string
 *
 * Parameters:
 *     domain : "soc/domain/subdomain" info
 *     service : "provider/service" info
 *
 * Returns:
 *     concatenated name : "soc/domain/subdomain/provider/service"
 * =====================================================================  */
SERVREG_NAME servreg_concat(SERVREG_NAME domain, SERVREG_NAME service)
{
   SERVREG_NAME servreg_name = SERVREG_NULL;
   uint32_t len = 0;

   if(SERVREG_NULL != domain)
   {
      if(SERVREG_NULL != service)
      {
         len = servreg_nmelen(domain) + 1 + servreg_nmelen(service) + 1;
         servreg_name = (SERVREG_NAME)servreg_calloc(sizeof(char), len);
         if(SERVREG_NULL != servreg_name)
         {
            strlcpy(servreg_name, domain, len);
            strlcat(servreg_name, "/", len);
            strlcat(servreg_name, service, len);
         }
         else
         {
            TMS_MSG_ERROR("Calloc failed");
         }
      }
      else
      {
         len = servreg_nmelen(domain) + 1;
         servreg_name = (SERVREG_NAME)servreg_calloc(sizeof(char), len);
         if(SERVREG_NULL != servreg_name)
         {
            strlcpy(servreg_name, domain, len);
         }
         else
         {
            TMS_MSG_ERROR("Calloc failed");
         }
      }
   }
   else
   {
      TMS_MSG_ERROR("Domain NULL");
   }

   return servreg_name;
}
