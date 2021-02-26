/* ===================================================================
** Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: salt_usta.h
** DESC: interface from salt to usta
** ================================================================ */
#pragma once

#include "see_salt.h"
#include "sensor.h"     // sensors-see/usta/native/sensor.h

salt_rc usta_get_sensor_list(see_salt *psalt,
                             std::vector<sensor_info>& sensor_list);
salt_rc usta_get_request_list(see_salt *psalt,
                              unsigned int handle);
salt_rc usta_get_sttributes(see_salt *psalt,
                            unsigned int handle,
                            std::string& attribute_list);
salt_rc usta_send_request(see_salt *psalt,
                          unsigned int handle,
                          see_client_request_message &client_req);
salt_rc usta_stop_request(see_salt *psalt,
                          unsigned int handle);
void usta_destroy_instance( see_salt *psalt);



