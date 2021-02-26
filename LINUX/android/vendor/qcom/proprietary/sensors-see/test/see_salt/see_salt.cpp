/* ===================================================================
** Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: see_salt.cpp
** DESC:
** ================================================================ */
#include <iostream>

#include "see_salt.h"
#include "salt_usta.h"
using namespace std;
static int salt_instance_num = 0;

see_salt* see_salt::get_instance()
{
   return new see_salt();
}

// constructor
see_salt::see_salt()
{
   _salt_instance_num = ++salt_instance_num;
}

see_salt::~see_salt()
{
   cout << "see_salt::~see_salt()" << endl;
   usta_destroy_instance( this);
}

void see_salt::append_sensor(sensor &sens)
{
   _sensor_list.push_back(sens);
}

int see_salt::suid_to_handle( sens_uid *target)
{
   for ( int handle = 0; handle < (int)_sensor_list.size(); handle++) {
      sensor *psens = &_sensor_list[handle];
      sens_uid *psuid = psens->get_suid();
      if ( target->low == psuid->low &&
           target->high == psuid->high) {
         return handle;
      }
   }
   char buffer[96];
   sprintf( buffer, "suid [hi %" PRIx64 " lo %" PRIx64 "] not found\n",
            target->high,
            target->low);
   throw std::runtime_error( std::string( buffer));
   return -1;
}

/**
 * @brief send_request message to USTA
 * @param [i] see_client_request_message
 * @return salt_rc
 */
salt_rc see_salt::send_request(see_client_request_message &client_req)
{
   /* get target sensor handle */
   sens_uid *suid = client_req.get_suid();
   int handle = suid_to_handle( suid);
   salt_rc rc = usta_send_request( this, handle, client_req);
   return rc;
}

std::string see_client_request_message::get_client_symbolic( see_std_client_processor_e client)
{
   if ( client == SEE_STD_CLIENT_PROCESSOR_SSC) { return  "SNS_STD_CLIENT_PROCESSOR_SSC"; }
   if ( client == SEE_STD_CLIENT_PROCESSOR_APSS) { return "SNS_STD_CLIENT_PROCESSOR_APSS"; }
   if ( client == SEE_STD_CLIENT_PROCESSOR_ADSP) { return "SNS_STD_CLIENT_PROCESSOR_ADSP"; }
   if ( client == SEE_STD_CLIENT_PROCESSOR_MDSP) { return "SNS_STD_CLIENT_PROCESSOR_MDSP"; }
   if ( client == SEE_STD_CLIENT_PROCESSOR_CDSP) { return "SEE_STD_CLIENT_PROCESSOR_CDSP"; }
   return "";
}

std::string see_client_request_message::get_wakeup_symbolic( see_client_delivery_e wakeup)
{
   if ( wakeup == SEE_CLIENT_DELIVERY_WAKEUP) { return "SNS_CLIENT_DELIVERY_WAKEUP"; }
   if ( wakeup == SEE_CLIENT_DELIVERY_NO_WAKEUP) { return "SNS_CLIENT_DELIVERY_NO_WAKEUP"; }
   return "";
}

/**
 * @param duration - seconds
 */
void see_salt::sleep( float duration)
{
   cout << "sleep( " << to_string( duration) << ") seconds" << endl;
   useconds_t usec = (int)( duration * 1000000);
   usleep( usec);
}








