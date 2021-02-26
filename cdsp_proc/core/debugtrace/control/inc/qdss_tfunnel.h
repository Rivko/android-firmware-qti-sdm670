#ifndef QDSS_TFUNNEL_H
#define QDSS_TFUNNEL_H

/*=============================================================================

  @file  qdss_tfunnel.h
  @brief QUALCOMM Debug Subsystem (QDSS) API for enabling/disabling speicific
         trace funnel ports

================================================================================
Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
        All Rights Reserved.
Qualcomm Technologies, Inc. Confidential and Proprietary.
==============================================================================*/

#define TFUNNEL_SSC_STM_PORT             "port_ssc_stm"
#define TFUNNEL_SSC_ETM_PORT             "port_ssc_etm"
#define TFUNNEL_LPASS_ETM_PORT           "port_lpass_etm"



/**
  @brief Enable a specific trace funnel port.

  This function enables trace data flow through a named funnel port.
  When a funnel port is enabled the required merge funnel port is also enabled.
  However trace sources and trace sinks have to be enabled separately
  to complete the trace data path

  @param port_name [in]: Name of the funnel port to be enabled.
                         Accepted names are listed above

  @return 0 on success. A non-zero error code on failure
*/
int qdss_tfunnel_enable_port(const char *port_name); 


/**
  @brief Disable a specific trace funnel port.

  This function disables trace data flow through a named funnel port.

  @param port_name [in]: Name of the funnel port to be disbled.
                         Accepted names are listed above

  @return 0 on success. A non-zero error code on failure
*/
int qdss_tfunnel_disable_port(const char *port_name); 

/**
  @brief Disable a specific trace funnel port.

  This function disables all trace funnel ports and merge funnel ports.
  This is useful if trace data from all other sources needs to be shut off,
  before enabling trace data from just the source you are interested in

  @return 0 on success. A non-zero error code on failure
*/

int qdss_tfunnel_disable_all_ports(void); 


#endif //QDSS_TFUNNEL_H
