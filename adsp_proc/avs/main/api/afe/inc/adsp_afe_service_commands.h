#ifndef _ADSP_AFE_SERVICE_COMMANDS_H_
#define _ADSP_AFE_SERVICE_COMMANDS_H_

/*========================================================================*/
/**
@file adsp_afe_service_commands.h

This file contains AFE service commands and events structures definitions.
 */
/*===========================================================================
NOTE: The description above does not appear in the PDF.
      Any descriptions that display in the PDF are located in the
      AFE_mainpage.dox file. Contact Tech Pubs for assistance.
===========================================================================*/
/*===========================================================================
  Copyright (c) 2012, 2014-2017 Qualcomm Technologies, Inc.  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
============================================================================ */
/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/api/afe/inc/adsp_afe_service_commands.h#2 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
05/20/17    sw     (Tech Pubs) Edited Doxygen comments for AVS 2.8 (SDM845).
08/03/16    sj     Separate header file for proprietary module and param ID's
07/16/16    sw     (Tech Pubs) Merged Doxygen comments from 8953.2.7;
                    edited comments/markup for 8998.2.8.
06/20/16    rr     Adding support for 32-bit data processing framework
08/23/15    sw     (Tech Pubs) Edited Doxygen comments/markup for 8996.2.7.
07/08/15   unni    Added API support for codec slimbus la offset 
06/24/15   unni    Codec paging added for 2.6. Edited by sw (Tech Pubs).
04/30/15    sw     (Tech Pubs) Edited Doxygen comments/markup for 8952.2.6.
10/20/14    sw     (Tech Pubs) Edited Doxygen comments/markup for 2.6.
03/01/14    sw     (Tech Pubs) Edited Doxygen comments/markup for 2.4.
11/16/12    sw     (Tech Pubs) Edited comments and applied latest Doxygen markup.
03/27/12   mspk    Non backward compatible API changes for next generation
========================================================================== */
#include "adsp_afe_version.h"
#include "afe_common.h"
#include "hw_mad.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/** @ingroup afe_service_api_deprecated
    Deprecated. Use #AFE_API_VERSION and #AFE_API_BRANCH_VERSION.

    Version of the aDSP AFE module. */
#define ADSP_AFE_VERSION    0x00260000


/** @addtogroup afe_port_id_ranges
@{ */

/** Size of the range of port IDs for the audio interface. */
#define  AFE_PORT_ID_AUDIO_IF_PORT_RANGE_SIZE                       0x3E

/** Size of the range of port IDs for internal Bluetooth FM ports. */
#define AFE_PORT_ID_INTERNAL_BT_FM_RANGE_SIZE                       0x6

/** Size of the range of port IDs for SLIMbus multichannel ports. */
#define AFE_PORT_ID_SLIMBUS_RANGE_SIZE                              0x12

/** Size of the range of port IDs for real-time proxy ports. */
#define  AFE_PORT_ID_RT_PROXY_PORT_RANGE_SIZE                       0x6

/** Size of the range of port IDs for pseudoports. */
#define AFE_PORT_ID_PSEUDOPORT_RANGE_SIZE                           0x5

/** @} */ /* end_addtogroup afe_port_id_ranges */

/** @ingroup afe_mod_param_id_deprecated
    Size of the range of port IDs for HDMI input. */
#define AFE_PORT_ID_HDMI_INPUT_RANGE_SIZE                           0x1

/** @addtogroup afe_port_id_ranges
@{ */
/** Size of the range of port IDs for HDMI multistream input. */
#define AFE_PORT_ID_HDMI_MULTISTREAM_RANGE_SIZE                     0x8

/** Size of the range of port IDs for HDMI audio over the display port. */
#define AFE_PORT_ID_HDMI_OVER_DP_RANGE_SIZE                         0x1

/** Size of the range of port IDs for the USB audio interface. */
#define AFE_PORT_ID_USB_AUDIO_RANGE_SIZE                            0x2

/** Start of the range of port IDs for the audio interface. */
#define  AFE_PORT_ID_AUDIO_IF_PORT_RANGE_START                      0x1000

/** End of the range of port IDs for the audio interface. */
#define  AFE_PORT_ID_AUDIO_IF_PORT_RANGE_END                        (AFE_PORT_ID_AUDIO_IF_PORT_RANGE_START+AFE_PORT_ID_AUDIO_IF_PORT_RANGE_SIZE-1)

/** Start of the range of port IDs for real-time proxy ports. */
#define  AFE_PORT_ID_RT_PROXY_PORT_RANGE_START                      0x2000

/** End of the range of port IDs for real-time proxy ports. */
#define  AFE_PORT_ID_RT_PROXY_PORT_RANGE_END                        (AFE_PORT_ID_RT_PROXY_PORT_RANGE_START+AFE_PORT_ID_RT_PROXY_PORT_RANGE_SIZE-1)

/** Start of the range of port IDs for internal Bluetooth FM devices. */
#define AFE_PORT_ID_INTERNAL_BT_FM_RANGE_START                      0x3000

/** End of the range of port IDs for internal Bluetooth FM devices. */
#define AFE_PORT_ID_INTERNAL_BT_FM_RANGE_END                        (AFE_PORT_ID_INTERNAL_BT_FM_RANGE_START+AFE_PORT_ID_INTERNAL_BT_FM_RANGE_SIZE-1)

/** Start of the range of port IDs for SLIMbus devices. */
#define AFE_PORT_ID_SLIMBUS_RANGE_START                             0x4000

/** End of the range of port IDs for SLIMbus devices. */
#define AFE_PORT_ID_SLIMBUS_RANGE_END                               (AFE_PORT_ID_SLIMBUS_RANGE_START+AFE_PORT_ID_SLIMBUS_RANGE_SIZE-1)

/** Start of the range of port IDs for pseudoports. */
#define AFE_PORT_ID_PSEUDOPORT_RANGE_START                          0x8001

/** End of the range of port IDs for pseudoports. */
#define AFE_PORT_ID_PSEUDOPORT_RANGE_END                            (AFE_PORT_ID_PSEUDOPORT_RANGE_START+AFE_PORT_ID_PSEUDOPORT_RANGE_SIZE-1)

/** @} */ /* end_addtogroup afe_port_id_ranges */

/** @addtogroup afe_mod_param_id_deprecated
@{ */
/** Start of the range of port IDs for HDMI input. */
#define AFE_PORT_ID_HDMI_INPUT_RANGE_START                          0x6001

/** End of the range of port IDs for HDMI input. */
#define AFE_PORT_ID_HDMI_INPUT_RANGE_END                            (AFE_PORT_ID_HDMI_INPUT_RANGE_START+AFE_PORT_ID_HDMI_INPUT_RANGE_SIZE-1)

/** @} */ /* end_addtogroup afe_mod_param_id_deprecated */

/** @addtogroup afe_port_id_ranges
@{ */
/** Start of the range of port IDs for HDMI multistream input. */
#define AFE_PORT_ID_HDMI_MULTISTREAM_RANGE_START                    0x6002

/** End of the range of port IDs for HDMI multistream input. */
#define AFE_PORT_ID_HDMI_MULTISTREAM_RANGE_END                            (AFE_PORT_ID_HDMI_MULTISTREAM_RANGE_START+AFE_PORT_ID_HDMI_MULTISTREAM_RANGE_SIZE-1)

/** Start of the range of port IDs for HDMI audio over the display port. */
#define AFE_PORT_ID_HDMI_OVER_DP_RANGE_START                       0x6020

/** End of the range of port IDs for HDMI audio over the display port. */
#define AFE_PORT_ID_HDMI_OVER_DP_RANGE_END                            (AFE_PORT_ID_HDMI_OVER_DP_RANGE_START+AFE_PORT_ID_HDMI_OVER_DP_RANGE_SIZE-1)

/** Start of the range of port IDs for USB audio ports. */
#define AFE_PORT_ID_USB_AUDIO_RANGE_START                          0x7000

/** End of the range of port IDs for USB audio ports. */
#define AFE_PORT_ID_USB_AUDIO_RANGE_END                            (AFE_PORT_ID_USB_AUDIO_RANGE_START+AFE_PORT_ID_USB_AUDIO_RANGE_SIZE-1)

/** @} */ /* end_addtogroup afe_port_id_ranges */

/** @addtogroup afe_defined_port_ids
@{ */
/** ID of the primary MI2S Rx port. */
#define AFE_PORT_ID_PRIMARY_MI2S_RX                                 0x1000

/** ID of the primary MI2S Tx port. */
#define AFE_PORT_ID_PRIMARY_MI2S_TX                                 0x1001

/** ID of the secondary MI2S Rx port. */
#define AFE_PORT_ID_SECONDARY_MI2S_RX                               0x1002

/** ID of the secondary MI2S Tx port. */
#define AFE_PORT_ID_SECONDARY_MI2S_TX                               0x1003

/** ID of the tertiary MI2S Rx port. */
#define AFE_PORT_ID_TERTIARY_MI2S_RX                                0x1004

/** ID of the tertiary MI2S Tx port. */
#define AFE_PORT_ID_TERTIARY_MI2S_TX                                0x1005

/** ID of the quaternary MI2S Rx port. */
#define AFE_PORT_ID_QUATERNARY_MI2S_RX                              0x1006

/** ID of the quaternary MI2S Tx port. */
#define AFE_PORT_ID_QUATERNARY_MI2S_TX                              0x1007

/** ID of the I2S Rx port. This macro provides backward compatibility.
    For naming consistency, use #AFE_PORT_ID_I2S_RX instead.
 */
#define AUDIO_PORT_ID_I2S_RX                                        0x1008

/** ID of the I2S Rx port. */
#define AFE_PORT_ID_I2S_RX                                          AUDIO_PORT_ID_I2S_RX

/** ID of the primary auxiliary PCM Rx port. */
#define AFE_PORT_ID_PRIMARY_PCM_RX                                  0x100A

/** ID of the primary auxiliary PCM Tx port. */
#define AFE_PORT_ID_PRIMARY_PCM_TX                                  0x100B

/** ID of the secondary auxiliary PCM Rx port. */
#define AFE_PORT_ID_SECONDARY_PCM_RX                                0x100C

/** ID of the secondary auxiliary PCM Tx port. */
#define AFE_PORT_ID_SECONDARY_PCM_TX                                0x100D

/** ID of the multichannel HDMI Rx port. */
#define AFE_PORT_ID_MULTICHAN_HDMI_RX                               0x100E

/** ID of the tertiary auxiliary PCM Rx port. */
#define AFE_PORT_ID_TERTIARY_PCM_RX                                 0x1012

/** ID of the tertiary auxiliary PCM Tx port. */
#define AFE_PORT_ID_TERTIARY_PCM_TX                                 0x1013

/** ID of the quaternary auxiliary PCM Rx port. */
#define AFE_PORT_ID_QUATERNARY_PCM_RX                               0x1014

/** ID of the quaternary auxiliary PCM Tx port. */
#define AFE_PORT_ID_QUATERNARY_PCM_TX                               0x1015

/** ID of the quinary MI2S Rx port. */
#define AFE_PORT_ID_QUINARY_MI2S_RX                                 0x1016

/** ID of the quinary MI2S Tx port. */
#define AFE_PORT_ID_QUINARY_MI2S_TX                                 0x1017

/** ID of the senary MI2S Rx port. */
#define AFE_PORT_ID_SENARY_MI2S_RX                                  0x1018

/** ID of the senary MI2S Tx port. */
#define AFE_PORT_ID_SENARY_MI2S_TX                                  0x1019

/** ID of primary MI2S Rx port 1. */
#define AFE_PORT_ID_PRIMARY_MI2S_RX_1                                0x101A

/** ID of primary MI2S Tx port 1. */
#define AFE_PORT_ID_PRIMARY_MI2S_TX_1                                0x101B

/** ID of primary MI2S Rx port 2. */
#define AFE_PORT_ID_PRIMARY_MI2S_RX_2                                0x101C

/** ID of primary MI2S Tx port 2. */
#define AFE_PORT_ID_PRIMARY_MI2S_TX_2                                0x101D

/** ID of primary MI2S Rx port 3. */
#define AFE_PORT_ID_PRIMARY_MI2S_RX_3                                0x101E

/** ID of primary MI2S Tx port 3. */
#define AFE_PORT_ID_PRIMARY_MI2S_TX_3                                0x101F

/** ID of quaternary MI2S Rx port 1. */
#define AFE_PORT_ID_QUATERNARY_MI2S_RX_1                            0x1020

/** ID of quaternary MI2S Tx port 1. */
#define AFE_PORT_ID_QUATERNARY_MI2S_TX_1                            0x1021

/** ID of quaternary MI2S Rx port 2. */
#define AFE_PORT_ID_QUATERNARY_MI2S_RX_2                            0x1022

/** ID of quaternary MI2S Tx port 2. */
#define AFE_PORT_ID_QUATERNARY_MI2S_TX_2                            0x1023

/** ID of quaternary MI2S Rx port 3. */
#define AFE_PORT_ID_QUATERNARY_MI2S_RX_3                            0x1024

/** ID of quaternary MI2S Tx port 3. */
#define AFE_PORT_ID_QUATERNARY_MI2S_TX_3                            0x1025

/** ID of quaternary MI2S Rx port 4. */
#define AFE_PORT_ID_QUATERNARY_MI2S_RX_4                            0x1026

/** ID of quaternary MI2S Tx port 4. */
#define AFE_PORT_ID_QUATERNARY_MI2S_TX_4                            0x1027

/** ID of quaternary MI2S Rx port 5. */
#define AFE_PORT_ID_QUATERNARY_MI2S_RX_5                            0x1028

/** ID of quaternary MI2S Tx port 5. */
#define AFE_PORT_ID_QUATERNARY_MI2S_TX_5                            0x1029

/** ID of quaternary MI2S Rx port 6. */
#define AFE_PORT_ID_QUATERNARY_MI2S_RX_6                            0x102A

/** ID of quaternary MI2S Tx port 6. */
#define AFE_PORT_ID_QUATERNARY_MI2S_TX_6                            0x102B

/** ID of quaternary MI2S Rx port 7. */
#define AFE_PORT_ID_QUATERNARY_MI2S_RX_7                            0x102C

/** ID of quaternary MI2S Tx port 7. */
#define AFE_PORT_ID_QUATERNARY_MI2S_TX_7                            0x102D

/** ID of Mi2S Rx port 0 that is used with the integrated codec. */
#define AFE_PORT_ID_INT0_MI2S_RX                                    0x102E

/** ID of Mi2S Tx port 0 that is used with the integrated codec. */
#define AFE_PORT_ID_INT0_MI2S_TX                                    0x102F

/** ID of Mi2S Rx port 1 that is used with the integrated codec. */
#define AFE_PORT_ID_INT1_MI2S_RX                                    0x1030

/** ID of Mi2S Tx port 1 that is used with the integrated codec. */
#define AFE_PORT_ID_INT1_MI2S_TX                                    0x1031

/** ID of Mi2S Rx port 2 that is used with the integrated codec. */
#define AFE_PORT_ID_INT2_MI2S_RX                                    0x1032

/** ID of Mi2S Tx port 2 that is used with the integrated codec. */
#define AFE_PORT_ID_INT2_MI2S_TX                                    0x1033

/** ID of Mi2S Rx port 3 that is used with the integrated codec. */
#define AFE_PORT_ID_INT3_MI2S_RX                                    0x1034

/** ID of Mi2S Tx port 3 that is used with the integrated codec. */
#define AFE_PORT_ID_INT3_MI2S_TX                                    0x1035

/** ID of Mi2S Rx port 4 that is used with the integrated codec. */
#define AFE_PORT_ID_INT4_MI2S_RX                                    0x1036

/** ID of Mi2S Tx port 4 that is used with the integrated codec. */
#define AFE_PORT_ID_INT4_MI2S_TX                                    0x1037

/** ID of Mi2S Rx port 5 that is used with the integrated codec. */
#define AFE_PORT_ID_INT5_MI2S_RX                                    0x1038

/** ID of Mi2S Tx port 5 that is used with the integrated codec. */
#define AFE_PORT_ID_INT5_MI2S_TX                                    0x1039

/** ID of Mi2S Rx port 6 that is used with the integrated codec. */
#define AFE_PORT_ID_INT6_MI2S_RX                                    0x103A

/** ID of Mi2S Tx port 6 that is used with the integrated codec. */
#define AFE_PORT_ID_INT6_MI2S_TX                                    0x103B

/** ID of the quinary auxiliary PCM Rx port. */
#define AFE_PORT_ID_QUINARY_PCM_RX                                  0x103C

/** ID of the quinary auxiliary PCM Tx port. */
#define AFE_PORT_ID_QUINARY_PCM_TX                                  0x103D

/** ID of real-time Rx proxy port 1.

    When this port is opened, the real-time port driver reads data
    from the AFE service and sends it to the real-time device.
 */
#define  AFE_PORT_ID_RT_PROXY_PORT_001_RX                           0x2000

/** ID of real-time Tx proxy port 1.

    When this port is opened, the real-time port driver writes data
    to the AFE service from the real-time device.
 */
#define  AFE_PORT_ID_RT_PROXY_PORT_001_TX                           0x2001

/** ID of real-time Rx proxy port 2.

    When this port is opened, the real-time port driver reads data
    from the AFE service and sends it to the real-time device.
 */
#define  AFE_PORT_ID_RT_PROXY_PORT_002_RX                           0x2002

/** ID of real-time Tx proxy port 2.

    When this port is opened, the real-time port driver writes data
    to the AFE service from the real-time device.
 */
#define  AFE_PORT_ID_RT_PROXY_PORT_002_TX                           0x2003

/** ID of real-time Rx proxy port 3.

    When this port is opened, the real-time port driver reads data
    from the AFE service and sends it to the real-time device.
 */
#define  AFE_PORT_ID_RT_PROXY_PORT_003_RX                           0x2004

/** ID of real-time Tx proxy port 3.

    When this port is opened, the real-time port driver writes data
    to the AFE service from the real-time device.
 */
#define  AFE_PORT_ID_RT_PROXY_PORT_003_TX                           0x2005

/** ID of the internal Bluetooth SCO Rx port. */
#define AFE_PORT_ID_INTERNAL_BT_SCO_RX                              0x3000

/** ID of the internal Bluetooth SCO Tx port. */
#define AFE_PORT_ID_INTERNAL_BT_SCO_TX                              0x3001

/** ID of the internal Bluetooth A2DP Rx port. */
#define AFE_PORT_ID_INTERNAL_BT_A2DP_RX                             0x3002

/** ID of the internal FM audio Rx port. */
#define AFE_PORT_ID_INTERNAL_FM_RX                                  0x3004

/** ID of the internal FM audio Tx port ID. */
#define AFE_PORT_ID_INTERNAL_FM_TX                                  0x3005

/** ID of SLIMbus Rx multichannel port 0. */
#define AFE_PORT_ID_SLIMBUS_MULTI_CHAN_0_RX                         0x4000

/** ID of SLIMbus Tx multichannel port 0. */
#define AFE_PORT_ID_SLIMBUS_MULTI_CHAN_0_TX                         0x4001

/** ID of SLIMbus Rx multichannel port 1. */
#define AFE_PORT_ID_SLIMBUS_MULTI_CHAN_1_RX                         0x4002

/** ID of SLIMbus Tx multichannel port 1. */
#define AFE_PORT_ID_SLIMBUS_MULTI_CHAN_1_TX                         0x4003

/** ID of SLIMbus Rx multichannel port 2. */
#define AFE_PORT_ID_SLIMBUS_MULTI_CHAN_2_RX                         0x4004

/** ID of SLIMbus Tx multichannel port 2. */
#define AFE_PORT_ID_SLIMBUS_MULTI_CHAN_2_TX                         0x4005

/** ID of SLIMbus Rx multichannel port 3. */
#define AFE_PORT_ID_SLIMBUS_MULTI_CHAN_3_RX                         0x4006

/** ID of SLIMbus Tx multichannel port 3. */
#define AFE_PORT_ID_SLIMBUS_MULTI_CHAN_3_TX                         0x4007

/** ID of SLIMbus Rx multichannel port 4. */
#define AFE_PORT_ID_SLIMBUS_MULTI_CHAN_4_RX                         0x4008

/** ID of SLIMbus Tx multichannel port 4. */
#define AFE_PORT_ID_SLIMBUS_MULTI_CHAN_4_TX                         0x4009

/** ID of SLIMbus Rx multichannel port 5. */
#define AFE_PORT_ID_SLIMBUS_MULTI_CHAN_5_RX                         0x400A

/** ID of SLIMbus Tx multichannel port 5. */
#define AFE_PORT_ID_SLIMBUS_MULTI_CHAN_5_TX                         0x400B

/** ID of SLIMbus Rx multichannel port 6. */
#define AFE_PORT_ID_SLIMBUS_MULTI_CHAN_6_RX                         0x400C

/** ID of SLIMbus Tx multichannel port 6. */
#define AFE_PORT_ID_SLIMBUS_MULTI_CHAN_6_TX                         0x400D

/** ID of SLIMbus Rx multichannel port 7. */
#define AFE_PORT_ID_SLIMBUS_MULTI_CHAN_7_RX                         0x400E

/** ID of SLIMbus Tx multichannel port 7. */
#define AFE_PORT_ID_SLIMBUS_MULTI_CHAN_7_TX                         0x400F

/** ID of SLIMbus Rx multichannel port 8. */
#define AFE_PORT_ID_SLIMBUS_MULTI_CHAN_8_RX                         0x4010

/** ID of SLIMbus Tx multichannel port 8. */
#define AFE_PORT_ID_SLIMBUS_MULTI_CHAN_8_TX                         0x4011

/** ID of the SPDIF Rx port. */
#define AFE_PORT_ID_SPDIF_RX                                        0x5000

/** ID of multistream HDMI Rx port 0. */
#define AFE_PORT_ID_HDMI_MULTISTREAM_0_RX                           0x6002

/** ID of multistream HDMI Rx port 1. */
#define AFE_PORT_ID_HDMI_MULTISTREAM_1_RX                           0x6004

/** ID of multistream HDMI Rx port 2. */
#define AFE_PORT_ID_HDMI_MULTISTREAM_2_RX                           0x6006

/** ID of multistream HDMI Rx port 3. */
#define AFE_PORT_ID_HDMI_MULTISTREAM_3_RX                           0x6008

/** ID of the HDMI-over-display-port Rx port. */
#define AFE_PORT_ID_HDMI_OVER_DP_0_RX                               0x6020

/** ID of the HDMI Tx port. */
#define AFE_PORT_ID_HDMI_TX                                         0x6001

/** ID of USB audio Rx port 0. */
#define AFE_PORT_ID_USB_AUDIO_0_RX                                  0x7000

/** ID of USB audio Tx port 0. */
#define AFE_PORT_ID_USB_AUDIO_0_TX                                  0x7001

/** ID of generic pseudoport 1. */
#define AFE_PORT_ID_PSEUDOPORT_01                                   0x8001

/** ID of generic pseudoport 2. */
#define AFE_PORT_ID_PSEUDOPORT_02                                   0x8002

/** ID of the pseudoport that corresponds to the voice Rx path.

    For recording, the voice Rx path samples are written to this port and
    consumed by the audio path.
 */
#define AFE_PORT_ID_VOICE_RECORD_RX                                 0x8003

/** ID of the pseudoport that corresponds to the voice Tx path.

    For recording, the voice Tx path samples are written to this port and
    consumed by the audio path.
 */
#define AFE_PORT_ID_VOICE_RECORD_TX                                 0x8004

/** ID of the pseudoport that corresponds to in-call voice delivery samples.

    During in-call audio delivery, the audio path delivers samples to this port
    from where the voice path delivers them on the Rx path.
 */
#define AFE_PORT_ID_VOICE_PLAYBACK_TX                               0x8005

/** @} */ /* end_addtogroup afe_defined_port_ids */

/** @ingroup afe_port_id_ranges
    Start of the range of TDM port IDs for the audio interface. */
#define AFE_PORT_ID_TDM_PORT_RANGE_START                            0x9000

/** @addtogroup afe_defined_port_ids
@{ */
/** ID of the primary TDM Rx port. */
#define AFE_PORT_ID_PRIMARY_TDM_RX        (AFE_PORT_ID_TDM_PORT_RANGE_START + 0x00)

/** ID of primary TDM Rx port 1. */
#define AFE_PORT_ID_PRIMARY_TDM_RX_1      (AFE_PORT_ID_PRIMARY_TDM_RX + 0x02)

/** ID of primary TDM Rx port 2. */
#define AFE_PORT_ID_PRIMARY_TDM_RX_2      (AFE_PORT_ID_PRIMARY_TDM_RX + 0x04)

/** ID of primary TDM Rx port 3. */
#define AFE_PORT_ID_PRIMARY_TDM_RX_3      (AFE_PORT_ID_PRIMARY_TDM_RX + 0x06)

/** ID of primary TDM Rx port 4. */
#define AFE_PORT_ID_PRIMARY_TDM_RX_4      (AFE_PORT_ID_PRIMARY_TDM_RX + 0x08)

/** ID of primary TDM Rx port 5. */
#define AFE_PORT_ID_PRIMARY_TDM_RX_5      (AFE_PORT_ID_PRIMARY_TDM_RX + 0x0A)

/** ID of primary TDM Rx port 6. */
#define AFE_PORT_ID_PRIMARY_TDM_RX_6      (AFE_PORT_ID_PRIMARY_TDM_RX + 0x0C)

/** ID of primary TDM Rx port 7. */
#define AFE_PORT_ID_PRIMARY_TDM_RX_7      (AFE_PORT_ID_PRIMARY_TDM_RX + 0x0E)

/** ID of the primary TDM Tx port. */
#define AFE_PORT_ID_PRIMARY_TDM_TX        (AFE_PORT_ID_TDM_PORT_RANGE_START + 0x01)

/** ID of primary TDM Tx port 1. */
#define AFE_PORT_ID_PRIMARY_TDM_TX_1      (AFE_PORT_ID_PRIMARY_TDM_TX + 0x02)

/** ID of primary TDM Tx port 2. */
#define AFE_PORT_ID_PRIMARY_TDM_TX_2      (AFE_PORT_ID_PRIMARY_TDM_TX + 0x04)

/** ID of primary TDM Tx port 3. */
#define AFE_PORT_ID_PRIMARY_TDM_TX_3      (AFE_PORT_ID_PRIMARY_TDM_TX + 0x06)

/** ID of primary TDM Tx port 4. */
#define AFE_PORT_ID_PRIMARY_TDM_TX_4      (AFE_PORT_ID_PRIMARY_TDM_TX + 0x08)

/** ID of primary TDM Tx port 5. */
#define AFE_PORT_ID_PRIMARY_TDM_TX_5      (AFE_PORT_ID_PRIMARY_TDM_TX + 0x0A)

/** ID of primary TDM Tx port 6. */
#define AFE_PORT_ID_PRIMARY_TDM_TX_6      (AFE_PORT_ID_PRIMARY_TDM_TX + 0x0C)

/** ID of primary TDM Tx port 7. */
#define AFE_PORT_ID_PRIMARY_TDM_TX_7      (AFE_PORT_ID_PRIMARY_TDM_TX + 0x0E)

/** ID of the secondary TDM Rx port. */
#define AFE_PORT_ID_SECONDARY_TDM_RX      (AFE_PORT_ID_TDM_PORT_RANGE_START + 0x10)

/** ID of secondary TDM Rx port 1. */
#define AFE_PORT_ID_SECONDARY_TDM_RX_1    (AFE_PORT_ID_SECONDARY_TDM_RX + 0x02)

/** ID of secondary TDM Rx port 2. */
#define AFE_PORT_ID_SECONDARY_TDM_RX_2    (AFE_PORT_ID_SECONDARY_TDM_RX + 0x04)

/** ID of secondary TDM Rx port 3. */
#define AFE_PORT_ID_SECONDARY_TDM_RX_3    (AFE_PORT_ID_SECONDARY_TDM_RX + 0x06)

/** ID of secondary TDM Rx port 4. */
#define AFE_PORT_ID_SECONDARY_TDM_RX_4    (AFE_PORT_ID_SECONDARY_TDM_RX + 0x08)

/** ID of secondary TDM Rx port 5. */
#define AFE_PORT_ID_SECONDARY_TDM_RX_5    (AFE_PORT_ID_SECONDARY_TDM_RX + 0x0A)

/** ID of secondary TDM Rx port 6. */
#define AFE_PORT_ID_SECONDARY_TDM_RX_6    (AFE_PORT_ID_SECONDARY_TDM_RX + 0x0C)

/** ID of secondary TDM Rx port 7. */
#define AFE_PORT_ID_SECONDARY_TDM_RX_7    (AFE_PORT_ID_SECONDARY_TDM_RX + 0x0E)

/** ID of the secondary TDM Tx port. */
#define AFE_PORT_ID_SECONDARY_TDM_TX      (AFE_PORT_ID_TDM_PORT_RANGE_START + 0x11)

/** ID of secondary TDM Tx port 1. */
#define AFE_PORT_ID_SECONDARY_TDM_TX_1    (AFE_PORT_ID_SECONDARY_TDM_TX + 0x02)

/** ID of secondary TDM Tx port 2. */
#define AFE_PORT_ID_SECONDARY_TDM_TX_2    (AFE_PORT_ID_SECONDARY_TDM_TX + 0x04)

/** ID of secondary TDM Tx port 3. */
#define AFE_PORT_ID_SECONDARY_TDM_TX_3    (AFE_PORT_ID_SECONDARY_TDM_TX + 0x06)

/** ID of secondary TDM Tx port 4. */
#define AFE_PORT_ID_SECONDARY_TDM_TX_4    (AFE_PORT_ID_SECONDARY_TDM_TX + 0x08)

/** ID of secondary TDM Tx port 5. */
#define AFE_PORT_ID_SECONDARY_TDM_TX_5    (AFE_PORT_ID_SECONDARY_TDM_TX + 0x0A)

/** ID of secondary TDM Tx port 6. */
#define AFE_PORT_ID_SECONDARY_TDM_TX_6    (AFE_PORT_ID_SECONDARY_TDM_TX + 0x0C)

/** ID of secondary TDM Tx port 7. */
#define AFE_PORT_ID_SECONDARY_TDM_TX_7    (AFE_PORT_ID_SECONDARY_TDM_TX + 0x0E)

/** ID of the tertiary TDM Rx port. */
#define AFE_PORT_ID_TERTIARY_TDM_RX       (AFE_PORT_ID_TDM_PORT_RANGE_START + 0x20)

/** ID of tertiary TDM Rx port 1. */
#define AFE_PORT_ID_TERTIARY_TDM_RX_1    (AFE_PORT_ID_TERTIARY_TDM_RX + 0x02)

/** ID of tertiary TDM Rx port 2. */
#define AFE_PORT_ID_TERTIARY_TDM_RX_2    (AFE_PORT_ID_TERTIARY_TDM_RX + 0x04)

/** ID of tertiary TDM Rx port 3. */
#define AFE_PORT_ID_TERTIARY_TDM_RX_3    (AFE_PORT_ID_TERTIARY_TDM_RX + 0x06)

/** ID of tertiary TDM Rx port 4. */
#define AFE_PORT_ID_TERTIARY_TDM_RX_4    (AFE_PORT_ID_TERTIARY_TDM_RX + 0x08)

/** ID of tertiary TDM Rx port 5. */
#define AFE_PORT_ID_TERTIARY_TDM_RX_5    (AFE_PORT_ID_TERTIARY_TDM_RX + 0x0A)

/** ID of tertiary TDM Rx port 6. */
#define AFE_PORT_ID_TERTIARY_TDM_RX_6    (AFE_PORT_ID_TERTIARY_TDM_RX + 0x0C)

/** ID of tertiary TDM Rx port 7. */
#define AFE_PORT_ID_TERTIARY_TDM_RX_7    (AFE_PORT_ID_TERTIARY_TDM_RX + 0x0E)

/** ID of the tertiary TDM Tx port. */
#define AFE_PORT_ID_TERTIARY_TDM_TX       (AFE_PORT_ID_TDM_PORT_RANGE_START + 0x21)

/** ID of tertiary TDM Tx port 1. */
#define AFE_PORT_ID_TERTIARY_TDM_TX_1    (AFE_PORT_ID_TERTIARY_TDM_TX + 0x02)

/** ID of tertiary TDM Tx port 2. */
#define AFE_PORT_ID_TERTIARY_TDM_TX_2    (AFE_PORT_ID_TERTIARY_TDM_TX + 0x04)

/** ID of tertiary TDM Tx port 3. */
#define AFE_PORT_ID_TERTIARY_TDM_TX_3    (AFE_PORT_ID_TERTIARY_TDM_TX + 0x06)

/** ID of tertiary TDM Tx port 4. */
#define AFE_PORT_ID_TERTIARY_TDM_TX_4    (AFE_PORT_ID_TERTIARY_TDM_TX + 0x08)

/** ID of tertiary TDM Tx port 5. */
#define AFE_PORT_ID_TERTIARY_TDM_TX_5    (AFE_PORT_ID_TERTIARY_TDM_TX + 0x0A)

/** ID of tertiary TDM Tx port 6. */
#define AFE_PORT_ID_TERTIARY_TDM_TX_6    (AFE_PORT_ID_TERTIARY_TDM_TX + 0x0C)

/** ID of tertiary TDM Tx port 7. */
#define AFE_PORT_ID_TERTIARY_TDM_TX_7    (AFE_PORT_ID_TERTIARY_TDM_TX + 0x0E)

/** ID of the quaternary TDM Rx port. */
#define AFE_PORT_ID_QUATERNARY_TDM_RX    (AFE_PORT_ID_TDM_PORT_RANGE_START + 0x30)

/** ID of quaternary TDM Rx port 1. */
#define AFE_PORT_ID_QUATERNARY_TDM_RX_1  (AFE_PORT_ID_QUATERNARY_TDM_RX + 0x02)

/** ID of quaternary TDM Rx port 2. */
#define AFE_PORT_ID_QUATERNARY_TDM_RX_2  (AFE_PORT_ID_QUATERNARY_TDM_RX + 0x04)

/** ID of quaternary TDM Rx port 3. */
#define AFE_PORT_ID_QUATERNARY_TDM_RX_3  (AFE_PORT_ID_QUATERNARY_TDM_RX + 0x06)

/** ID of quaternary TDM Rx port 4. */
#define AFE_PORT_ID_QUATERNARY_TDM_RX_4  (AFE_PORT_ID_QUATERNARY_TDM_RX + 0x08)

/** ID of quaternary TDM Rx port 5. */
#define AFE_PORT_ID_QUATERNARY_TDM_RX_5  (AFE_PORT_ID_QUATERNARY_TDM_RX + 0x0A)

/** ID of quaternary TDM Rx port 6. */
#define AFE_PORT_ID_QUATERNARY_TDM_RX_6  (AFE_PORT_ID_QUATERNARY_TDM_RX + 0x0C)

/** ID of quaternary TDM Rx port 7. */
#define AFE_PORT_ID_QUATERNARY_TDM_RX_7  (AFE_PORT_ID_QUATERNARY_TDM_RX + 0x0E)

/** ID of the quaternary TDM Tx port. */
#define AFE_PORT_ID_QUATERNARY_TDM_TX     (AFE_PORT_ID_TDM_PORT_RANGE_START + 0x31)

/** ID of quaternary TDM Tx port 1. */
#define AFE_PORT_ID_QUATERNARY_TDM_TX_1  (AFE_PORT_ID_QUATERNARY_TDM_TX + 0x02)

/** ID of quaternary TDM Tx port 2. */
#define AFE_PORT_ID_QUATERNARY_TDM_TX_2  (AFE_PORT_ID_QUATERNARY_TDM_TX + 0x04)

/** ID of quaternary TDM Tx port 3. */
#define AFE_PORT_ID_QUATERNARY_TDM_TX_3  (AFE_PORT_ID_QUATERNARY_TDM_TX + 0x06)

/** ID of quaternary TDM Tx port 4. */
#define AFE_PORT_ID_QUATERNARY_TDM_TX_4  (AFE_PORT_ID_QUATERNARY_TDM_TX + 0x08)

/** ID of quaternary TDM Tx port 5. */
#define AFE_PORT_ID_QUATERNARY_TDM_TX_5  (AFE_PORT_ID_QUATERNARY_TDM_TX + 0x0A)

/** ID of quaternary TDM Tx port 6. */
#define AFE_PORT_ID_QUATERNARY_TDM_TX_6  (AFE_PORT_ID_QUATERNARY_TDM_TX + 0x0C)

/** ID of quaternary TDM Tx port 7. */
#define AFE_PORT_ID_QUATERNARY_TDM_TX_7  (AFE_PORT_ID_QUATERNARY_TDM_TX + 0x0E)

/** ID of the quinary TDM Rx port. */
#define AFE_PORT_ID_QUINARY_TDM_RX    (AFE_PORT_ID_TDM_PORT_RANGE_START + 0x40)

/** ID of quinary TDM Rx port 1. */
#define AFE_PORT_ID_QUINARY_TDM_RX_1  (AFE_PORT_ID_QUINARY_TDM_RX + 0x02)

/** ID of quinary TDM Rx port 2. */
#define AFE_PORT_ID_QUINARY_TDM_RX_2  (AFE_PORT_ID_QUINARY_TDM_RX + 0x04)

/** ID of quinary TDM Rx port 3. */
#define AFE_PORT_ID_QUINARY_TDM_RX_3  (AFE_PORT_ID_QUINARY_TDM_RX + 0x06)

/** ID of quinary TDM Rx port 4. */
#define AFE_PORT_ID_QUINARY_TDM_RX_4  (AFE_PORT_ID_QUINARY_TDM_RX + 0x08)

/** ID of quinary TDM Rx port 5. */
#define AFE_PORT_ID_QUINARY_TDM_RX_5  (AFE_PORT_ID_QUINARY_TDM_RX + 0x0A)

/** ID of quinary TDM Rx port 6. */
#define AFE_PORT_ID_QUINARY_TDM_RX_6  (AFE_PORT_ID_QUINARY_TDM_RX + 0x0C)

/** ID of quinary TDM Rx port 7. */
#define AFE_PORT_ID_QUINARY_TDM_RX_7  (AFE_PORT_ID_QUINARY_TDM_RX + 0x0E)

/** ID of the quinary TDM Tx port. */
#define AFE_PORT_ID_QUINARY_TDM_TX     (AFE_PORT_ID_TDM_PORT_RANGE_START + 0x41)

/** ID of quinary TDM Tx port 1. */
#define AFE_PORT_ID_QUINARY_TDM_TX_1  (AFE_PORT_ID_QUINARY_TDM_TX + 0x02)

/** ID of quinary TDM Tx port 2. */
#define AFE_PORT_ID_QUINARY_TDM_TX_2  (AFE_PORT_ID_QUINARY_TDM_TX + 0x04)

/** ID of quinary TDM Tx port 3. */
#define AFE_PORT_ID_QUINARY_TDM_TX_3  (AFE_PORT_ID_QUINARY_TDM_TX + 0x06)

/** ID of quinary TDM Tx port 4. */
#define AFE_PORT_ID_QUINARY_TDM_TX_4  (AFE_PORT_ID_QUINARY_TDM_TX + 0x08)

/** ID of quinary TDM Tx port 5. */
#define AFE_PORT_ID_QUINARY_TDM_TX_5  (AFE_PORT_ID_QUINARY_TDM_TX + 0x0A)

/** ID of quinary TDM Tx port 6. */
#define AFE_PORT_ID_QUINARY_TDM_TX_6  (AFE_PORT_ID_QUINARY_TDM_TX + 0x0C)

/** ID of quinary TDM Tx port 7. */
#define AFE_PORT_ID_QUINARY_TDM_TX_7  (AFE_PORT_ID_QUINARY_TDM_TX + 0x0E)

/** @} */ /* end_addtogroup afe_defined_port_ids */

/** @addtogroup afe_port_id_ranges
@{ */
/** End of the range of TDM port IDs. */
#define AFE_PORT_ID_TDM_PORT_RANGE_END    (AFE_PORT_ID_TDM_PORT_RANGE_START + 0x50 - 1)

/** Size of the range of TDM port IDs for the audio interface. */
#define AFE_PORT_ID_TDM_PORT_RANGE_SIZE   (AFE_PORT_ID_TDM_PORT_RANGE_END - AFE_PORT_ID_TDM_PORT_RANGE_START + 1)

/** @} */ /* end_addtogroup afe_port_id_ranges */

/** @addtogroup afe_defined_group_device_ids
@{ */
/** Primary TDM Rx group device ID. */
#define AFE_GROUP_DEVICE_ID_PRIMARY_TDM_RX       (AFE_PORT_ID_PRIMARY_TDM_RX +    0x100)

/** Primary TDM Tx group device ID. */
#define AFE_GROUP_DEVICE_ID_PRIMARY_TDM_TX       (AFE_PORT_ID_PRIMARY_TDM_TX +    0x100)

/** Secondary TDM Rx group device ID. */
#define AFE_GROUP_DEVICE_ID_SECONDARY_TDM_RX     (AFE_PORT_ID_SECONDARY_TDM_RX +  0x100)

/** Secondary TDM Tx group device ID. */
#define AFE_GROUP_DEVICE_ID_SECONDARY_TDM_TX     (AFE_PORT_ID_SECONDARY_TDM_TX +  0x100)

/** Tertiary TDM Rx group device ID. */
#define AFE_GROUP_DEVICE_ID_TERTIARY_TDM_RX      (AFE_PORT_ID_TERTIARY_TDM_RX +   0x100)

/** Tertiary TDM Tx group device ID. */
#define AFE_GROUP_DEVICE_ID_TERTIARY_TDM_TX      (AFE_PORT_ID_TERTIARY_TDM_TX +   0x100)

/** Quaternary TDM Rx group device ID. */
#define AFE_GROUP_DEVICE_ID_QUATERNARY_TDM_RX    (AFE_PORT_ID_QUATERNARY_TDM_RX + 0x100)

/** Quaternary TDM Tx group device ID. */
#define AFE_GROUP_DEVICE_ID_QUATERNARY_TDM_TX    (AFE_PORT_ID_QUATERNARY_TDM_TX + 0x100)

/** Quinary TDM Rx group device ID. */
#define AFE_GROUP_DEVICE_ID_QUINARY_TDM_RX    (AFE_PORT_ID_QUINARY_TDM_RX + 0x100)

/** Quinary TDM Tx group device ID. */
#define AFE_GROUP_DEVICE_ID_QUINARY_TDM_TX    (AFE_PORT_ID_QUINARY_TDM_TX + 0x100)

/** @} */ /* end_addtogroup afe_defined_group_device_ids */

/** @ingroup afe_defined_port_ids
    Invalid port ID. */
#define AFE_PORT_ID_INVALID                                         0xFFFF

/* Secondary MI2S group device IDs. */

/** @addtogroup afe_defined_group_device_ids
@{ */
/** Primary MI2S Rx group device ID. */
#define AFE_GROUP_DEVICE_ID_PRIMARY_MI2S_RX                           (AFE_PORT_ID_PRIMARY_MI2S_RX + 0x100)

/** Primary MI2S Tx group device ID. */
#define AFE_GROUP_DEVICE_ID_PRIMARY_MI2S_TX                           (AFE_PORT_ID_PRIMARY_MI2S_TX + 0x100)

/** Quaternary MI2S Rx group device ID. */
#define AFE_GROUP_DEVICE_ID_QUATERNARY_MI2S_RX                       (AFE_PORT_ID_QUATERNARY_MI2S_RX + 0x100)

/** Quaternary MI2S Tx group device ID. */
#define AFE_GROUP_DEVICE_ID_QUATERNARY_MI2S_TX                       (AFE_PORT_ID_QUATERNARY_MI2S_TX + 0x100)

/** @} */ /* end_addtogroup afe_defined_group_device_ids */

/** @addtogroup afe_module_loopback
@{ */
/** ID of the Loopback module, which is used to set up the loopback between AFE
    ports.

    This module supports the following parameter IDs:
    - #AFE_PARAM_ID_LOOPBACK_GAIN_PER_PATH
    - #AFE_PARAM_ID_LOOPBACK_CONFIG

    @apr_hdr_fields
    Opcode -- AFE_MODULE_LOOPBACK
 */
#define AFE_MODULE_LOOPBACK                                           0x00010205

/** ID of the parameter used by #AFE_MODULE_LOOPBACK to get/set the loopback
    gain on a specified path.

    When multiple loopbacks are enabled, this parameter ID controls the
    loopback gain from the source port ID in a SET_PARAM command to the
    destination port ID.

    #AFE_PORT_CMD_SET_PARAM_V2 (deprecated), #AFE_PORT_CMD_GET_PARAM_V2
    (deprecated), #AFE_PORT_CMD_SET_PARAM_V3, and #AFE_PORT_CMD_GET_PARAM_V3
    can use this parameter ID.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_LOOPBACK_GAIN_PER_PATH

    @apr_msgpayload{afe_loopback_gain_per_path_param_t}
    @tablens{weak__afe__loopback__gain__per__path__param__t} @newpage
 */
#define AFE_PARAM_ID_LOOPBACK_GAIN_PER_PATH                         0x00010206

typedef struct afe_loopback_gain_per_path_param_t afe_loopback_gain_per_path_param_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_loopback_gain_per_path_param_t
@{ */
/* Payload of the AFE_PARAM_ID_LOOPBACK_GAIN_PER_PATH parameter,
    which gets/sets loopback gain of a port to a Rx port. */
/** The Tx port ID of the loopback is part of #AFE_PORT_CMD_SET_PARAM_V2
    (deprecated)and #AFE_PORT_CMD_SET_PARAM_V3.
 */
struct afe_loopback_gain_per_path_param_t
{
   uint16_t                  rx_port_id;
   /**< Rx port of the loopback. */

   uint16_t                  gain;
   /**< Loopback gain per path of the port.

        @values Any number (in Q13 format) */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_loopback_gain_per_path_param_t */

/** ID of the parameter used by #AFE_MODULE_LOOPBACK to configure and
    enable/disable the loopback path.

    This parameter ID allows an Rx port to be configured as a source port on
    the loopback path. The port_id field in a SET_PARAM command is the source
    port that can be designated as a Tx or Rx port.

    #AFE_PORT_CMD_SET_PARAM_V2 (deprecated), #AFE_PORT_CMD_GET_PARAM_V2
    (deprecated), #AFE_PORT_CMD_SET_PARAM_V3, and #AFE_PORT_CMD_GET_PARAM_V3
    can use this parameter ID.

    Also, this parameter ID can be used to configure the type of
    routing mode, which is useful for handling different use cases.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_LOOPBACK_CONFIG

    @apr_msgpayload{afe_loopback_cfg_v2_t}
    @table{weak__afe__loopback__cfg__v2__t}
 */
#define AFE_PARAM_ID_LOOPBACK_CONFIG                                 0x0001020B

/** Version information used to handle future additions to
    AFE_PARAM_ID_LOOPBACK_CONFIG configuration (for backward compatibility).
 */
#define AFE_API_VERSION_LOOPBACK_CONFIG                                0x2

/** Data path type of the loopback routing mode.
 */
typedef enum afe_loopback_routing_mode_t
{
    LB_MODE_DEFAULT = 1,
    /**< Regular loopback from the source port to the destination port. */

    LB_MODE_SIDETONE,
    /**< Sidetone feed from the source port to the destination port.
         This field is supported only on the Tx port. */

    LB_MODE_EC_REF_VOICE_AUDIO,
    /**< Echo cancellation reference: voice + audio.
         This field is supported only on the Rx port. */

    LB_MODE_EC_REF_VOICE
    /**< Echo cancellation reference: voice only. Currently, this field is not
         supported. */

}/** @cond */ afe_loopback_routing_mode_t /** @endcond */ ;
/** @} *//* end_addtogroup afe_module_loopback */


/*Deprecated. Structure for the AFE_PARAM_ID_LOOPBACK_CONFIG param. */
typedef struct afe_loopback_cfg_v1_t afe_loopback_cfg_v1_t;

#include "adsp_begin_pack.h"

/** @ingroup afe_mod_param_id_deprecated
  Deprecated. Use #afe_loopback_cfg_v2_t.

  Payload of the AFE_PARAM_ID_LOOPBACK_CONFIG parameter, which
  enables/disables one AFE loopback.
 */
struct afe_loopback_cfg_v1_t
{
    uint32_t                  loopback_cfg_minor_version;
    /**< Tracks the configuration of this parameter.

         @values #AFE_API_VERSION_LOOPBACK_CONFIG */

    uint16_t                  dst_port_id;
    /**< ID of the destination port.

        @values See <i>Supported Hardware Ports</i> in Appendix
        @xref{chp:portIDs}. */

    uint16_t                  routing_mode;
    /**< Type of data path from the source to the destination port.

         @values
         - #LB_MODE_DEFAULT
         - #LB_MODE_SIDETONE
         - #LB_MODE_EC_REF_VOICE_AUDIO
         - #LB_MODE_EC_REF_VOICE @tablebulletend */

    uint16_t                  enable;
    /**< Specifies whether AFE loopback is enabled.

         @values
         - 0 -- Disabled
         - 1 -- Enabled @tablebulletend */

    uint16_t                  reserved;
    /**< This field must be set to 0. */
}
#include "adsp_end_pack.h"
;

/** @addtogroup afe_module_loopback
@{ */
/* Structure for the AFE_PARAM_ID_LOOPBACK_CONFIG param with AFE_API_VERSION_LOOPBACK_CONFIG API version 2. */
typedef struct afe_loopback_cfg_v2_t afe_loopback_cfg_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_loopback_cfg_v2_t
@{ */
/* Payload of the AFE_PARAM_ID_LOOPBACK_CONFIG parameter, which
    enables/disables one AFE loopback. It also has support to
    enable/disable rate matching within the loopback
*/
struct afe_loopback_cfg_v2_t
{
    uint32_t                  loopback_cfg_minor_version;
    /**< Tracks the configuration of this parameter.

         @values #AFE_API_VERSION_LOOPBACK_CONFIG */

    uint16_t                  dst_port_id;
    /**< ID of the destination port.

        @values See <i>Supported Hardware Ports</i> in Appendix
        @xref{chp:portIDs}. */

    uint16_t                  routing_mode;
    /**< Type of data path from the source to the destination port.

         @values
         - #LB_MODE_DEFAULT
         - #LB_MODE_SIDETONE
         - #LB_MODE_EC_REF_VOICE_AUDIO
         - #LB_MODE_EC_REF_VOICE @tablebulletend */

    uint16_t                  enable;
    /**< Specifies whether AFE loopback is enabled.

          @values
          - 0 -- Disabled
          - 1 -- Enabled @tablebulletend */

    uint16_t                  reserved;
    /**< This field must be set to 0. */

    uint32_t                  rate_matching_enable;
    /**< Specifies whether rate matching is enabled in an AFE loopback.

         @values
         - 0 -- Disabled
         - 1 -- Enabled

         If latency is important for the loopback path (sidetone processing and
         rate matching are not required between the source and destination
         ports), disable this rate_matching_enable flag while using
         afe_loopback_cfg_v2_t. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_loopback_cfg_v2_t */
/** @} */ /* end_addtogroup afe_module_loopback */


/** @addtogroup afe_cmd_remcore_hw_vote_req
@{ */
/** @weakgroup weakb_afe_cmd_remcore_hw_vote_req_values
@{ */
//lpass_core_hw_block_id define
/** Hardware block ID is invalid. */
#define AFE_LPASS_CORE_HW_BLOCK_ID_NONE                        0

/** Hardware block is an audio-video timer. */
#define AFE_LPASS_CORE_HW_BLOCK_ID_AVTIMER                     2

/** @} */ /* end_weakgroup weakb_afe_cmd_remcore_hw_vote_req_values */

/** Handles audio-video timer (avtimer) and BTSC vote requests from
    clients.

  @apr_hdr_fields
    Opcode -- AFE_CMD_REMOTE_LPASS_CORE_HW_VOTE_REQUEST

  @apr_msgpayload{afe_cmd_remote_lpass_core_hw_vote_request}
    @table{weak__afe__cmd__remote__lpass__core__hw__vote__request}

  @description
    Clients can use this command to request that the LPASS core hardware module
    be enabled. Remote clients can access the LPASS core hardware module only
    after this vote request.
    @par
    The AFE returns a handle to the LPASS core hardware module in response to
    this command.
    @par
    A client must ensure that it votes the LPASS core hardware module only
    once. If the client sends multiple vote commands, the same LPASS core
    hardware module handle is sent in response.
    @par
    After processing finishes, the client must issue
    #AFE_CMD_REMOTE_LPASS_CORE_HW_DEVOTE_REQUEST to release the LPASS core
    hardware module.

  @return
    #AFE_CMD_RSP_REMOTE_LPASS_CORE_HW_VOTE_REQUEST

  @dependencies
    None. @newpage

  @inputfile{group__weakb__afe__cmd__remcore__hw__vote__req__values.tex}
 */
#define AFE_CMD_REMOTE_LPASS_CORE_HW_VOTE_REQUEST            0x000100f4

typedef struct afe_cmd_remote_lpass_core_hw_vote_request afe_cmd_remote_lpass_core_hw_vote_request;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_cmd_remote_lpass_core_hw_vote_request
@{ */
/* Payload of the AFE_CMD_REMOTE_LPASS_CORE_HW_VOTE_REQUEST command.
 */
struct afe_cmd_remote_lpass_core_hw_vote_request
{
   uint32_t  hw_block_id;
   /**< ID of the hardware block.

        @values
        - #AFE_LPASS_CORE_HW_BLOCK_ID_NONE
        - #AFE_LPASS_CORE_HW_BLOCK_ID_AVTIMER @tablebulletend */

   char client_name[8];
   /**< Name of the client.

        @values Valid string with a maximum of 8 characters */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_cmd_remote_lpass_core_hw_vote_request */
/** @} */ /* end_addtogroup afe_cmd_remcore_hw_vote_req */


/** @addtogroup afe_rsp_remcore_hw_vote_req
@{ */
/** In response to the #AFE_CMD_REMOTE_LPASS_CORE_HW_VOTE_REQUEST command, the
    AFE sends a handle to the LPASS core hardware module.

  @apr_hdr_fields
   Opcode -- AFE_CMD_RSP_REMOTE_LPASS_CORE_HW_VOTE_REQUEST

  @apr_msgpayload{afe_cmd_rsp_remote_lpass_core_hw_vote_request}
    @table{weak__afe__cmd__rsp__remote__lpass__core__hw__vote__request}

  @description
    After processing finishes, the client must issue
    #AFE_CMD_REMOTE_LPASS_CORE_HW_DEVOTE_REQUEST to release the LPASS core
    hardware module.

  @return
    None.

  @dependencies
    The client must send #AFE_CMD_REMOTE_LPASS_CORE_HW_VOTE_REQUEST before this
    response is sent.
 */
#define AFE_CMD_RSP_REMOTE_LPASS_CORE_HW_VOTE_REQUEST        0x000100f5

typedef struct afe_cmd_rsp_remote_lpass_core_hw_vote_request afe_cmd_rsp_remote_lpass_core_hw_vote_request;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_cmd_rsp_remote_lpass_core_hw_vote_request
@{ */
/* Payload of the AFE_CMD_RSP_REMOTE_LPASS_CORE_HW_VOTE_REQUEST command.
 */
struct afe_cmd_rsp_remote_lpass_core_hw_vote_request
{
   uint32_t client_handle;
   /**< Handle of the client.

        @values Valid uint32 number */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_cmd_rsp_remote_lpass_core_hw_vote_request */
/** @} */ /* end_addtogroup afe_rsp_remcore_hw_vote_req */


/** @addtogroup afe_rsp_remcore_hw_devote_req
@{ */
/** Handles client requests to disable the LPASS core hardware module.
  The client must send a valid LPASS core hardware module handle.

  @apr_hdr_fields
   Opcode -- AFE_CMD_REMOTE_LPASS_CORE_HW_DEVOTE_REQUEST

  @apr_msgpayload{afe_cmd_remote_lpass_core_hw_devote_request}
    @table{weak__afe__cmd__remote__lpass__core__hw__devote__request}

  @return
    None.

  @dependencies
    The client must send #AFE_CMD_REMOTE_LPASS_CORE_HW_VOTE_REQUEST before this
    command is sent.
 */
#define AFE_CMD_REMOTE_LPASS_CORE_HW_DEVOTE_REQUEST            0x000100f6

typedef struct afe_cmd_remote_lpass_core_hw_devote_request afe_cmd_remote_lpass_core_hw_devote_request;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_cmd_remote_lpass_core_hw_devote_request
@{ */
/* Payload of the AFE_CMD_REMOTE_LPASS_CORE_HW_DEVOTE_REQUEST command.
 */
struct afe_cmd_remote_lpass_core_hw_devote_request
{
   uint32_t  hw_block_id;
   /**< ID of the hardware block.

        @values
        - #AFE_LPASS_CORE_HW_BLOCK_ID_NONE
        - #AFE_LPASS_CORE_HW_BLOCK_ID_AVTIMER @tablebulletend */

   uint32_t client_handle;
   /**< Handle of the client.

        @values Valid uint32 number */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_cmd_remote_lpass_core_hw_devote_request */
/** @} */ /* end_addtogroup afe_rsp_remcore_hw_devote_req */


/** @addtogroup afe_cmd_register_rt_port_dr
@{ */
/** Registers a real-time port driver with the AFE service for a given port.

  @apr_hdr_fields
    Opcode -- AFE_SERVICE_CMD_REGISTER_RT_PORT_DRIVER

  @apr_msgpayload{afe_service_cmd_register_rt_port_driver_t}
    @table{weak__afe__service__cmd__register__rt__port__driver__t}

  @detdesc
    The APR destination address and port number are obtained from the header of
    this packet and are used for sending different events.
    @par
    By registering, the customer applications receive
    #AFE_EVENT_RT_PROXY_PORT_STATUS events indicating when to start and stop
    the PCM exchange flow.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    None.
 */
#define AFE_SERVICE_CMD_REGISTER_RT_PORT_DRIVER                     0x000100E0

/* Structure for the AFE_SERVICE_CMD_REGISTER_RT_PORT_DRIVER command. */
typedef struct afe_service_cmd_register_rt_port_driver_t    afe_service_cmd_register_rt_port_driver_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_service_cmd_register_rt_port_driver_t
@{ */
/* Payload of the AFE_SERVICE_CMD_REGISTER_RT_PORT_DRIVER command,
    which registers a real-time port driver with the AFE service.
 */
struct afe_service_cmd_register_rt_port_driver_t
{
    uint16_t                  port_id;
    /**< ID of the AFE port with which the real-time driver exchanges data
         (registers for events).

         @values #AFE_PORT_ID_RT_PROXY_PORT_RANGE_START to
                 #AFE_PORT_ID_RT_PROXY_PORT_RANGE_END */

    uint16_t                  reserved;
    /**< This field must be set to 0. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_service_cmd_register_rt_port_driver_t */
/** @} */ /* end_addtogroup afe_cmd_register_rt_port_dr */


/** @addtogroup afe_cmd_unregister_rt_port_dr
@{ */
/** Unregisters a real-time port driver from the AFE service for a given port.

  @apr_hdr_fields
    Opcode -- AFE_SERVICE_CMD_UNREGISTER_RT_PORT_DRIVER

  @apr_msgpayload{afe_service_cmd_unregister_rt_port_driver_t}
    @table{weak__afe__service__cmd__unregister__rt__port__driver__t}

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    #AFE_SERVICE_CMD_REGISTER_RT_PORT_DRIVER must be processed successfully
    before this command is sent.
 */
#define AFE_SERVICE_CMD_UNREGISTER_RT_PORT_DRIVER                      0x000100E1

/* Structure for the AFE_SERVICE_CMD_UNREGISTER_RT_PORT_DRIVER command. */
typedef struct afe_service_cmd_unregister_rt_port_driver_t    afe_service_cmd_unregister_rt_port_driver_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_service_cmd_unregister_rt_port_driver_t
@{ */
/* Payload of the AFE_SERVICE_CMD_UNREGISTER_RT_PORT_DRIVER command,
    which unregisters a real-time port driver from the AFE service.
 */
struct afe_service_cmd_unregister_rt_port_driver_t
{
    uint16_t                  port_id;
    /**< ID of the AFE port from which the real-time driver unregisters for
         events.

         @values #AFE_PORT_ID_RT_PROXY_PORT_RANGE_START to
                 #AFE_PORT_ID_RT_PROXY_PORT_RANGE_END */

    uint16_t                  reserved;
    /**< This field must be set to 0. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_service_cmd_unregister_rt_port_driver_t */
/** @} */ /* end_addtogroup afe_cmd_unregister_rt_port_dr */


/** @addtogroup afe_evt_rt_proxy_port_status
@{ */
/** Sent when an event is reported from the AFE service to a real-time driver.

  @apr_hdr_fields
    Opcode -- AFE_EVENT_RT_PROXY_PORT_STATUS

  @apr_msgpayload{afe_event_rt_proxy_port_status_t}
    @table{weak__afe__event__rt__proxy__port__status__t}

  @detdesc
    This event communicates information about the real-time proxy port to the
    client. A Start or Stop event indicates that data transfer begins or stops.
    For a client to receive this notification, it must register using
    #AFE_SERVICE_CMD_REGISTER_RT_PORT_DRIVER.
    @par
    For a Start or Stop event, and depending on the value of port_id, the
    client starts or stops sending buffers as follows:
    - Filled buffers when port_id is set for the mic path
    - Empty buffers when port_id is set for the speaker path
    @par
    After acknowledging the Port Stop command, the AFE service returns all
    buffers.
    @par
    Low and high watermark events are sent based on the values of the
    corresponding thresholds that are configured using the
    #AFE_PARAM_ID_RT_PROXY_CONFIG parameter ID in
    #AFE_MODULE_AUDIO_DEV_INTERFACE.
    - In the source path, a low/high watermark event is sent when the application
      is too slow/fast in sending data.
    - In the sink path, a low watermark event indicates that the application is
      consuming data at a faster rate than the AFE is getting data from the Rx
      path. A high watermark event indicates that the application is consuming
      data slowly.
    @par
    @keep{4}
    These events can be used to adjust the rate at which applications
    produce or consume data. However, these events are added for debugging
    purposes and should not be used for non-real-time applications.

  @return
    None.

  @dependencies
    #AFE_SERVICE_CMD_REGISTER_RT_PORT_DRIVER.
 */
#define AFE_EVENT_RT_PROXY_PORT_STATUS                              0x00010105

/** Type of event sent when a real-time proxy port is started. */
#define AFE_EVENT_TYPE_RT_PROXY_PORT_START                           0

/** Type of event sent when a real-time proxy port is stopped. */
#define AFE_EVENT_TYPE_RT_PROXY_PORT_STOP                            1

/** Type of event sent when a low watermark is reached. */
#define AFE_EVENT_TYPE_RT_PROXY_PORT_LOW_WATER_MARK                  2

/** Type of event sent when a high watermark is reached. */
#define AFE_EVENT_TYPE_RT_PROXY_PORT_HIGH_WATER_MARK                 3

/** Invalid event. */
#define AFE_EVENT_TYPE_RT_PROXY_PORT_INVALID                         0xFFFF

/* Structure for the AFE_EVENT_RT_PROXY_PORT_STATUS event. */
typedef struct afe_event_rt_proxy_port_status_t  afe_event_rt_proxy_port_status_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_event_rt_proxy_port_status_t
@{ */
/* Payload of the AFE_EVENT_RT_PROXY_PORT_STATUS message, which sends
    an event from the AFE service to a registered client.
 */
struct afe_event_rt_proxy_port_status_t
{
    uint16_t                  port_id;
    /**< ID of the AFE port that receives the event.

         @values #AFE_PORT_ID_RT_PROXY_PORT_RANGE_START to
                 #AFE_PORT_ID_RT_PROXY_PORT_RANGE_END */

    uint16_t                  event_type;
    /**< Type of event.

         @values
         - #AFE_EVENT_TYPE_RT_PROXY_PORT_START
         - #AFE_EVENT_TYPE_RT_PROXY_PORT_STOP
         - #AFE_EVENT_TYPE_RT_PROXY_PORT_LOW_WATER_MARK
         - #AFE_EVENT_TYPE_RT_PROXY_PORT_HIGH_WATER_MARK @vertspace{-28} */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_event_rt_proxy_port_status_t */
/** @} */ /* end_addtogroup afe_evt_rt_proxy_port_status */


/** @addtogroup afe_cmd_rt_proxy_port_write
@{ */
/** Delivers a data buffer to the AFE service from a real-time device.

  @apr_hdr_fields
    Opcode -- AFE_PORT_DATA_CMD_RT_PROXY_PORT_WRITE_V2

  @apr_msgpayload{afe_port_data_cmd_rt_proxy_port_write_v2_t}
    @table{weak__afe__port__data__cmd__rt__proxy__port__write__v2__t}

  @detdesc
    The client uses this command to send the data obtained from the
    source device to the AFE. An acknowledgment is sent immediately after
    copying the data to the circular buffer.
    @par
    If incoming data does not fit in the circular buffer due to overflow, it
    overwrites a portion of the data in the existing buffer, and that data is
    lost.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    #AFE_SERVICE_CMD_REGISTER_RT_PORT_DRIVER -- The following commands must be
    sent with port_id set as a source (Tx) real-time proxy port ID:
    - #AFE_PARAM_ID_RT_PROXY_CONFIG through #AFE_MODULE_AUDIO_DEV_INTERFACE
    - #AFE_PORT_CMD_DEVICE_START
    @par
    The physical buffer must be fully contained in a memory region that
    #AFE_SERVICE_CMD_SHARED_MEM_MAP_REGIONS mapped to the AFE service.
 */
#define AFE_PORT_DATA_CMD_RT_PROXY_PORT_WRITE_V2                    0x000100ED

/* Structure for the AFE_PORT_DATA_CMD_RT_PROXY_PORT_WRITE_V2 command. */
typedef struct afe_port_data_cmd_rt_proxy_port_write_v2_t  afe_port_data_cmd_rt_proxy_port_write_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_port_data_cmd_rt_proxy_port_write_v2_t
@{ */
/* Payload of the AFE_PORT_DATA_CMD_RT_PROXY_PORT_WRITE_V2 command, which
    delivers a filled buffer to the AFE service.
 */
struct afe_port_data_cmd_rt_proxy_port_write_v2_t
{
    uint16_t                  port_id;
    /**< ID of the Tx (mic) proxy port with which the real-time driver
         exchanges data.

         @values #AFE_PORT_ID_RT_PROXY_PORT_RANGE_START to
                 #AFE_PORT_ID_RT_PROXY_PORT_RANGE_END */

    uint16_t                  reserved;
    /**< This field must be set to 0. */

    uint32_t                  buffer_address_lsw;
    /**< Lower 32 bits of the 64-bit address buffer containing the data from
         the real-time source device on a client. */

    uint32_t                  buffer_address_msw;
    /**< Upper 32 bits of the 64-bit address buffer containing the data from
         the real-time source device on a client. */

    uint32_t                   mem_map_handle;
    /**< Unique identifier for a shared memory address.

         @values Any 32-bit value

         The aDSP returns the memory map handle through
         #AFE_SERVICE_CMD_SHARED_MEM_MAP_REGIONS. */

    uint32_t                  available_bytes;
    /**< Number of valid bytes available in the buffer (including all
         channels: number of bytes per channel = availableBytes/numChannels).

         @values > 0

         This field must be equal to the frame size specified in the
         #AFE_MODULE_AUDIO_DEV_INTERFACE command that was sent to configure this
         port. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup afe_port_data_cmd_rt_proxy_port_write_v2_t */
/** @} */ /* end_addtogroup afe_cmd_rt_proxy_port_write */


/** @addtogroup afe_cmd_rt_proxy_port_read
@{ */
/** Delivers an empty data buffer to the AFE service to be filled and sent to a
    real-time device.

  @apr_hdr_fields
    Opcode -- AFE_PORT_DATA_CMD_RT_PROXY_PORT_READ

  @apr_msgpayload{afe_port_data_cmd_rt_proxy_port_read_v2_t}
    @table{weak__afe__port__data__cmd__rt__proxy__port__read__v2__t}

  @detdesc
    The client uses this command to receive data (from the AFE service)
    that is written to the sink device. If sufficient data is available in the
    circular buffer, data is immediately copied and an acknowledgment is sent.
    If the circular buffer is not filled with have enough data, the following
    occurs:
     - An underrun is detected
     - Partial data is copied
     - The rest of the buffer is filled with zeros
     - An acknowledgment is sent @newpage

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    #AFE_SERVICE_CMD_REGISTER_RT_PORT_DRIVER -- The following commands must be
    sent with the port_id set as a sink (Rx) real-time proxy port ID:
    - #AFE_PARAM_ID_RT_PROXY_CONFIG through #AFE_MODULE_AUDIO_DEV_INTERFACE
    - #AFE_PORT_CMD_DEVICE_START
    @par
    The physical buffer must be fully contained in a memory region that
    #AFE_SERVICE_CMD_SHARED_MEM_MAP_REGIONS mapped to the AFE service.
 */
#define AFE_PORT_DATA_CMD_RT_PROXY_PORT_READ_V2                        0x000100EE

/* Structure for the AFE_PORT_DATA_CMD_RT_PROXY_PORT_READ_V2 command. */
typedef struct afe_port_data_cmd_rt_proxy_port_read_v2_t  afe_port_data_cmd_rt_proxy_port_read_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_port_data_cmd_rt_proxy_port_read_v2_t
@{ */
/* Payload of the AFE_PORT_DATA_CMD_RT_PROXY_PORT_READ_V2 command, which
    delivers an empty buffer to the AFE service. On acknowledgment, data is
    filled in the buffer.
 */
struct afe_port_data_cmd_rt_proxy_port_read_v2_t
{
    uint16_t                  port_id;
    /**< ID of the Rx proxy port with which the real-time driver exchanges data.

         @values #AFE_PORT_ID_RT_PROXY_PORT_RANGE_START to
                 #AFE_PORT_ID_RT_PROXY_PORT_RANGE_END

         This port must be an Rx (speaker) port. */

    uint16_t                  reserved;
    /**< This field must be set to 0. */

    uint32_t                  buffer_address_lsw;
    /**< Lower 32 bits of the 64-bit address containing the data sent from the
         AFE service to a real-time sink device on the client. */

    uint32_t                  buffer_address_msw;
    /**< Upper 32 bits of the 64-bit address containing the data sent from the
         AFE service to a real-time sink device on the client. */

    uint32_t                   mem_map_handle;
    /**< Unique identifier for a shared memory address.

         @values Any 32-bit value

         The aDSP returns the memory map handle through
         #AFE_SERVICE_CMD_SHARED_MEM_MAP_REGIONS. */

    uint32_t                  available_bytes;
    /**< Number of valid bytes available in the buffer (including all
         channels).

         @values > 0 bytes

         This field must be equal to the frame size specified in the
         #AFE_MODULE_AUDIO_DEV_INTERFACE command that was sent to configure this
         port. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup afe_port_data_cmd_rt_proxy_port_read_v2_t */
/** @} */ /* end_addtogroup afe_cmd_rt_proxy_port_read */


/** @addtogroup afe_module_audio_device
@{ */
/** ID of the Audio Device Interface module, which configures devices such as
    I2S, PCM, HDMI, and SLIMbus.
    This module supports following parameter IDs:
     - #AFE_PARAM_ID_I2S_CONFIG
     - #AFE_PARAM_ID_PCM_CONFIG
     - #AFE_PARAM_ID_TDM_CONFIG
     - #AFE_PARAM_ID_HDMI_CONFIG
     - #AFE_PARAM_ID_USB_AUDIO_CONFIG
     - #AFE_PARAM_ID_SPDIF_CONFIG
     - #AFE_PARAM_ID_CH_STATUS_CONFIG
     - #AFE_PARAM_ID_GROUP_DEVICE_HDMI_MULTISTREAM_CONFIG
     - #AFE_PARAM_ID_INTERNAL_BT_FM_CONFIG
     - #AFE_PARAM_ID_SLIMBUS_CONFIG
     - #AFE_PARAM_ID_RT_PROXY_CONFIG
     - #AFE_PARAM_ID_PSEUDO_PORT_CONFIG
     - #AFE_PARAM_ID_AHB_CLOCK_CONFIG
     - #AFE_PARAM_ID_DEVICE_HW_DELAY
     - #AFE_PARAM_ID_LPAIF_CLK_CONFIG
     - #AFE_PARAM_ID_SPDIF_CLK_CONFIG
     - #AFE_PARAM_ID_INTERNAL_DIGIATL_CDC_CLK_CONFIG
     - #AFE_PARAM_ID_TOPO_ID
     - #AFE_PARAM_ID_CONT_BUF_LATENCY_STATS
     - #AFE_PARAM_ID_PORT_MEDIA_TYPE
     - #AFE_PARAM_ID_DEV_TIMING_STATS

    To configure the AFE port, the client must set the port using
    #AFE_PORT_CMD_SET_PARAM_V3, and fill the module ID with the correct
    parameter IDs.

    Before the client sends this command to the AFE, the client must ensure
    that the codec configuration and SLIMbus interface configuration (for a
    SLIMbus device) are correct.

    @apr_hdr_fields
    Opcode -- AFE_MODULE_AUDIO_DEV_INTERFACE @newpage
 */
#define AFE_MODULE_AUDIO_DEV_INTERFACE                                0x0001020C

/** ID of the parameter used by #AFE_MODULE_AUDIO_DEV_INTERFACE to configure
    the I2S interface.

    #AFE_PORT_CMD_SET_PARAM_V2 (deprecated), #AFE_PORT_CMD_GET_PARAM_V2
    (deprecated), #AFE_PORT_CMD_SET_PARAM_V3 and #AFE_PORT_CMD_GET_PARAM_V3
    can use this parameter ID.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_I2S_CONFIG

    @apr_msgpayload{afe_param_id_i2s_cfg_t}
    @table{weak__afe__param__id__i2s__cfg__t}
 */
#define AFE_PARAM_ID_I2S_CONFIG                                      0x0001020D

/** Version information used to handle future additions to
    AFE_PARAM_ID_I2S_CONFIG processing (for backward compatibility).
 */
#define AFE_API_VERSION_I2S_CONFIG                                     0x1

/** Sample rate is 8 kHz. */
#define AFE_PORT_SAMPLE_RATE_8K                                     8000

/** Sample rate is 11.025 kHz. */
#define AFE_PORT_SAMPLE_RATE_11_025K                                11025

/** Sample rate is 12 kHz. */
#define AFE_PORT_SAMPLE_RATE_12K                                    12000

/** Sample rate is 16 kHz. */
#define AFE_PORT_SAMPLE_RATE_16K                                    16000

/** Sample rate is 22.05 kHz. */
#define AFE_PORT_SAMPLE_RATE_22_05K                                 22050

/** Sample rate is 24 kHz. */
#define AFE_PORT_SAMPLE_RATE_24K                                    24000

/** Sample rate is 32 kHz. */
#define AFE_PORT_SAMPLE_RATE_32K                                    32000

/** Sample rate is 44.1 kHz. */
#define AFE_PORT_SAMPLE_RATE_44_1K                                  44100

/** Sample rate is 88.2 kHz. */
#define AFE_PORT_SAMPLE_RATE_88_2K                                  88200

/** Sample rate is 48 kHz. */
#define AFE_PORT_SAMPLE_RATE_48K                                    48000

/** Sample rate is 96 kHz. */
#define AFE_PORT_SAMPLE_RATE_96K                                    96000

/** Sample rate is 176.4 kHz.*/
#define AFE_PORT_SAMPLE_RATE_176_4K                                 176400

/** Sample rate is 192 kHz. */
#define AFE_PORT_SAMPLE_RATE_192K                                   192000

/** Sample rate is 352.8 kHz. */
#define AFE_PORT_SAMPLE_RATE_352_8K                                 352800

/** Sample rate is 384 kHz. */
#define AFE_PORT_SAMPLE_RATE_384K                                   384000

/** Enumeration for setting the data format to PCM when PCM data is
    transmitted or received over I2S lines.
 */
#define AFE_LINEAR_PCM_DATA                                         0x0

/** Enumeration for setting the data format to nonlinear (compressed) when
    nonlinear (compressed) data is transmited/received over I2S lines.
 */
#define AFE_NON_LINEAR_DATA                                          0x1

/** Enumeration for setting the data format when PCM data packed in a 60958
    container is transmited/received over I2S lines.
 */
#define AFE_LINEAR_PCM_DATA_PACKED_60958                             0x2

/** Enumeration for setting the data format when nonlinear data packed in a
    60958 container is transmited/received over I2S lines.
 */
#define AFE_NON_LINEAR_DATA_PACKED_60958                             0x3

/** @} */ /* end_addtogroup afe_module_audio_device */

/** @addtogroup afe_mod_param_id_deprecated
@{ */
/** Deprecated.

    Enumeration for setting the data format when PCM data is packed in the
    custom parallel audio interface format.
 */
#define AFE_LINEAR_PCM_DATA_PACKED_CUSTOM_PAO                        0x4

/** Deprecated.

    Enumeration for setting the data format when nonlinear data is packed in
    the custom parallel audio interface format. @newpage
 */
#define AFE_NON_LINEAR_DATA_PACKED_CUSTOM_PAO                        0x5

/** @} */ /* end_addtogroup afe_mod_param_id_deprecated */

/** @addtogroup afe_module_audio_device
@{ */
/** Enumeration for setting the data format when two 16-bit data
    samples are packed in a 32-bit device sample. The least significant
    sample is mapped to channel 0, and the most significant sample is
    mapped to channel 1.
 */
#define AFE_LINEAR_PCM_DATA_PACKED_16BIT                        0x6

/** Enumeration for setting the data format to Direct-Stream Digital (DSD)
    when DSD over PCM (DoP) data is transmitted or received over I2S lines
    (currently supported only on the AFE Rx path).
 */
#define AFE_DSD_DOP_DATA                                        0x7

/** Enumeration for setting the data format to generic compressed
    when client data is transmitted or received over I2S/TDM lines via a
    compressed path.
 */
#define AFE_GENERIC_COMPRESSED_DATA                                  0x8

/** Enumeration for setting the I2S configuration channel_mode parameter to
    serial data wire number 0 (SD0).
 */
#define AFE_PORT_I2S_SD0                                            0x1

/** Enumeration for setting the I2S configuration channel_mode parameter to
    serial data wire number 1 (SD1).
 */
#define AFE_PORT_I2S_SD1                                            0x2

/** Enumeration for setting the I2S configuration channel_mode parameter to
    serial data wire number 2 (SD2).
 */
#define AFE_PORT_I2S_SD2                                            0x3

/** Enumeration for setting the I2S configuration channel_mode parameter to
    serial data wire number 3 (SD3).
 */
#define AFE_PORT_I2S_SD3                                            0x4

/** Enumeration for setting the I2S configuration channel_mode parameter to
    SD0 and SD1.
 */
#define AFE_PORT_I2S_QUAD01                                         0x5

/** Enumeration for setting the I2S configuration channel_mode parameter to
    SD2 and SD3.
 */
#define AFE_PORT_I2S_QUAD23                                         0x6

/** Enumeration for setting the I2S configuration channel_mode parameter to
    six channels.
 */
#define AFE_PORT_I2S_6CHS                                           0x7

/** Enumeration for setting the I2S configuration channel_mode parameter to
    8 channels.
 */
#define AFE_PORT_I2S_8CHS                                           0x8

/** Enumeration for setting the I2S configuration mono_stereo parameter to
    mono.
 */
#define AFE_PORT_I2S_MONO                                           0x0

/** Enumeration for setting the I2S configuration mono_stereo parameter to
    stereo.
 */
#define AFE_PORT_I2S_STEREO                                         0x1

/** Enumeration for setting the I2S configuration ws_src parameter to
    external.
 */
#define AFE_PORT_CONFIG_I2S_WS_SRC_EXTERNAL                         0x0

/** Enumeration for setting the I2S configuration ws_src parameter to
    internal. @newpage
 */
#define AFE_PORT_CONFIG_I2S_WS_SRC_INTERNAL                         0x1

typedef struct afe_param_id_i2s_cfg_t afe_param_id_i2s_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_i2s_cfg_t
@{ */
/* Payload of the AFE_PARAM_ID_I2S_CONFIG parameter ID used by
    AFE_MODULE_AUDIO_DEV_INTERFACE.
 */
struct afe_param_id_i2s_cfg_t
{
   uint32_t                  i2s_cfg_minor_version;
   /**< Tracks the configuration of this parameter.

        @values #AFE_API_VERSION_I2S_CONFIG */

    uint16_t                  bit_width;
    /**< Bit width of the sample.

         @values 16, 24, 32 */

    uint16_t                  channel_mode;
    /**< I2S lines and multichannel operation.

         @values
         - #AFE_PORT_I2S_SD0
         - #AFE_PORT_I2S_SD1
         - #AFE_PORT_I2S_SD2
         - #AFE_PORT_I2S_SD3
         - #AFE_PORT_I2S_QUAD01
         - #AFE_PORT_I2S_QUAD23
         - #AFE_PORT_I2S_6CHS
         - #AFE_PORT_I2S_8CHS @tablebulletend */

    uint16_t                  mono_stereo;
    /**< Specifies mono or stereo. This value applies only when a single I2S
         line is used.

         @values
         - #AFE_PORT_I2S_MONO
         - #AFE_PORT_I2S_STEREO @tablebulletend */

    uint16_t                  ws_src;
    /**< Specifies whether the word-select source is internal or external.

         @values
         - #AFE_PORT_CONFIG_I2S_WS_SRC_EXTERNAL
         - #AFE_PORT_CONFIG_I2S_WS_SRC_INTERNAL @tablebulletend */

    uint32_t                  sample_rate;
    /**< Sampling rate of the port.

         @values
         - #AFE_PORT_SAMPLE_RATE_8K
         - #AFE_PORT_SAMPLE_RATE_11_025K
         - #AFE_PORT_SAMPLE_RATE_16K
         - #AFE_PORT_SAMPLE_RATE_22_05K
         - #AFE_PORT_SAMPLE_RATE_24K
         - #AFE_PORT_SAMPLE_RATE_32K
         - #AFE_PORT_SAMPLE_RATE_44_1K
         - #AFE_PORT_SAMPLE_RATE_48K
         - #AFE_PORT_SAMPLE_RATE_88_2K
         - #AFE_PORT_SAMPLE_RATE_96K
         - #AFE_PORT_SAMPLE_RATE_176_4K
         - #AFE_PORT_SAMPLE_RATE_192K
         - #AFE_PORT_SAMPLE_RATE_352_8K
         - #AFE_PORT_SAMPLE_RATE_384K

         At 44.1 kHz, the I2S interface supports both PCM and compressed data
         in both the Tx direction and Rx direction.

         There is no concurrent support of a voice call when the port operates
         at 44.1 kHz because of RTD requirements.

         The AFE and ADM must run with the same sampling rate.
         All AFE clients must be at the same sampling rate (44.1 kHz) on the
         same device. This requirement applies to other sampling rates that are
         either multiples or divisors of 44.1 kHz (22.05 kHz, 88.2 kHz,
         176.4 kHz).

         @note1hang Sampling rates of 24 kHz and 32 kHz are typically used
                    for compressed data. */

    uint16_t                   data_format;
    /**< Data format supported by the I2S interface.

         @values
         - #AFE_LINEAR_PCM_DATA
         - #AFE_NON_LINEAR_DATA
         - #AFE_LINEAR_PCM_DATA_PACKED_60958
         - #AFE_NON_LINEAR_DATA_PACKED_60958
         - #AFE_LINEAR_PCM_DATA_PACKED_16BIT
         - #AFE_DSD_DOP_DATA
         - #AFE_GENERIC_COMPRESSED_DATA @tablebulletend */

    uint16_t                  reserved;
    /**< This field must be set to 0. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_param_id_i2s_cfg_t */

/** ID of the parameter used by #AFE_MODULE_AUDIO_DEV_INTERFACE to configure
    the PCM interface.

    #AFE_PORT_CMD_SET_PARAM_V2 (deprecated), #AFE_PORT_CMD_GET_PARAM_V2
    (deprecated), #AFE_PORT_CMD_SET_PARAM_V3, and #AFE_PORT_CMD_GET_PARAM_V3
    can use this parameter ID.

    Because of restrictions from the hardware, the AUX PCM interface must be
    enabled for both Rx and Tx at the same time. Rx or Tx cannot be used alone.
    For example, the primary AUX Tx PCM port must be enabled with the primary
    AUX Rx PCM port.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_PCM_CONFIG

    @apr_msgpayload{afe_param_id_pcm_cfg_t}
    @table{weak__afe__param__id__pcm__cfg__t}
 */
#define AFE_PARAM_ID_PCM_CONFIG                                      0x0001020E

/** Version information used to handle future additions to
    AFE_PARAM_ID_PCM_CONFIG processing (for backward compatibility).
 */
#define AFE_API_VERSION_PCM_CONFIG                                     0x1

/** Enumeration for the auxiliary PCM synchronization signal provided by an
    external source.
 */
#define AFE_PORT_PCM_SYNC_SRC_EXTERNAL                              0x0

/** Enumeration for the auxiliary PCM synchronization signal provided by an
    internal source.
 */
#define AFE_PORT_PCM_SYNC_SRC_INTERNAL                              0x1

/** Enumeration for the PCM configuration aux_mode parameter, which configures
    the auxiliary PCM interface to use short synchronization.
 */
#define AFE_PORT_PCM_AUX_MODE_PCM                                   0x0

/** Enumeration for the PCM configuration aux_mode parameter, which configures
    the auxiliary PCM interface to use long synchronization.
 */
#define AFE_PORT_PCM_AUX_MODE_AUX                                   0x1

/** Enumeration for setting the PCM configuration frame to 8. */
#define AFE_PORT_PCM_BITS_PER_FRAME_8                               0x0

/** Enumeration for setting the PCM configuration frame to 16. */
#define AFE_PORT_PCM_BITS_PER_FRAME_16                              0x1

/** Enumeration for setting the PCM configuration frame to 32. */
#define AFE_PORT_PCM_BITS_PER_FRAME_32                              0x2

/** Enumeration for setting the PCM configuration frame to 64. */
#define AFE_PORT_PCM_BITS_PER_FRAME_64                              0x3

/** Enumeration for setting the PCM configuration frame to 128. */
#define AFE_PORT_PCM_BITS_PER_FRAME_128                             0x4

/** Enumeration for setting the PCM configuration frame to 256. */
#define AFE_PORT_PCM_BITS_PER_FRAME_256                             0x5

/** @} */ /* end_addtogroup afe_module_audio_device */

/** @addtogroup afe_service_api_deprecated
@{ */
/** Deprecated.

    Enumeration for setting the PCM configuration quant_type parameter to
    A-law with no padding.
 */
#define AFE_PORT_PCM_ALAW_NOPADDING                                 0x0

/** Deprecated.

    Enumeration for setting the PCM configuration quant_type parameter to
    mu-law with no padding.
 */
#define AFE_PORT_PCM_MULAW_NOPADDING                                0x1

/** @} */ /* end_addtogroup afe_mod_param_id_deprecated */

/** @addtogroup afe_module_audio_device
@{ */
/** Enumeration for setting the PCM configuration quant_type parameter to
    linear with no padding.
 */
#define AFE_PORT_PCM_LINEAR_NOPADDING                               0x2

/** @} */ /* end_addtogroup afe_module_audio_device */

/** @addtogroup afe_service_api_deprecated
@{ */
/** Deprecated.

    Enumeration for setting the PCM configuration quant_type parameter to
    A-law with padding.
 */
#define AFE_PORT_PCM_ALAW_PADDING                                   0x3

/** Deprecated.

    Enumeration for setting the PCM configuration quant_type parameter to
     mu-law with padding. @newpage
 */
#define AFE_PORT_PCM_MULAW_PADDING                                  0x4

/** @} */ /* end_addtogroup afe_mod_param_id_deprecated */

/** @addtogroup afe_module_audio_device
@{ */
/** Enumeration for setting the PCM configuration quant_type parameter to
    linear with padding.
 */
#define AFE_PORT_PCM_LINEAR_PADDING                                 0x5

/** Enumeration for disabling the PCM configuration ctrl_data_out_enable
    parameter. The PCM block is the only master.
 */
#define AFE_PORT_PCM_CTRL_DATA_OE_DISABLE                           0x0

/** Enumeration for enabling the PCM configuration ctrl_data_out_enable
    parameter. The PCM block shares the signal with other masters. @newpage
 */
#define AFE_PORT_PCM_CTRL_DATA_OE_ENABLE                            0x1

typedef struct afe_param_id_pcm_cfg_t afe_param_id_pcm_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_pcm_cfg_t
@{ */
/* Payload of the AFE_PARAM_ID_PCM_CONFIG parameter used by
    AFE_MODULE_AUDIO_DEV_INTERFACE.
 */
struct afe_param_id_pcm_cfg_t
{
   uint32_t                  pcm_cfg_minor_version;
   /**< Tracks the configuration of this parameter.

        @values #AFE_API_VERSION_PCM_CONFIG */

    uint16_t                  aux_mode;
    /**< PCM synchronization setting.

         @values
         - #AFE_PORT_PCM_AUX_MODE_PCM
         - #AFE_PORT_PCM_AUX_MODE_AUX @tablebulletend */

    uint16_t                  sync_src;
    /**< Synchronization source.

         @values
         - #AFE_PORT_PCM_SYNC_SRC_EXTERNAL
         - #AFE_PORT_PCM_SYNC_SRC_INTERNAL @tablebulletend */

    uint16_t                  frame_setting;
    /**< Number of bits per frame.

         @values
         - #AFE_PORT_PCM_BITS_PER_FRAME_8
         - #AFE_PORT_PCM_BITS_PER_FRAME_16
         - #AFE_PORT_PCM_BITS_PER_FRAME_32
         - #AFE_PORT_PCM_BITS_PER_FRAME_64
         - #AFE_PORT_PCM_BITS_PER_FRAME_128
         - #AFE_PORT_PCM_BITS_PER_FRAME_256 @tablebulletend */

    uint16_t                  quant_type;
    /**< Type of PCM quantization.

         @values
         - #AFE_PORT_PCM_ALAW_NOPADDING
         - #AFE_PORT_PCM_MULAW_NOPADDING
         - #AFE_PORT_PCM_LINEAR_NOPADDING
         - #AFE_PORT_PCM_ALAW_PADDING
         - #AFE_PORT_PCM_MULAW_PADDING
         - #AFE_PORT_PCM_LINEAR_PADDING @tablebulletend */

    uint16_t                  ctrl_data_out_enable;
    /**< Specifies whether the PCM block shares the data-out signal to the
         drive with other masters.

         @values
         - #AFE_PORT_PCM_CTRL_DATA_OE_DISABLE
         - #AFE_PORT_PCM_CTRL_DATA_OE_ENABLE @tablebulletend */

    uint16_t                  reserved;
    /**< This field must be set to 0. */

    uint32_t                  sample_rate;
    /**< Sampling rate of the port.

         @values
         - #AFE_PORT_SAMPLE_RATE_8K
         - #AFE_PORT_SAMPLE_RATE_16K
         - #AFE_PORT_SAMPLE_RATE_24K
         - #AFE_PORT_SAMPLE_RATE_32K
         - #AFE_PORT_SAMPLE_RATE_48K @tablebulletend */

    uint16_t                  bit_width;
    /**< Bit width of the sample.

         @values 16 */

    uint16_t                  num_channels;
    /**< Number of channels.

         @values 1 to 4 */

    uint16_t                  slot_number_mapping[4];
    /**< Array that specifies the slot number for each channel in a
         multichannel scenario.

         @values 1 to 32

         To transfer the data at higher sample rate, use more slots.
         For example, transfer 16 kHz data using 8 kHz samples in two slots.

         Always place the valid slot value setting in from index 0.
         For example, use slot_number_mapping[0] for one slot, and use
         slot_number_mapping[0] and slot_number_mapping[1] for two slots.

         If slot_number_mapping[] is not used, clear it to 0. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_param_id_pcm_cfg_t */

/** ID of the parameter used by #AFE_MODULE_AUDIO_DEV_INTERFACE to configure
    the TDM interface.

    #AFE_PORT_CMD_SET_PARAM_V2 (deprecated), #AFE_PORT_CMD_GET_PARAM_V2
    (deprecated), #AFE_PORT_CMD_SET_PARAM_V3, and #AFE_PORT_CMD_GET_PARAM_V3
    can use this parameter ID.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_TDM_CONFIG

    @apr_msgpayload{afe_param_id_tdm_cfg_t}
    @table{weak__afe__param__id__tdm__cfg__t}
*/
#define AFE_PARAM_ID_TDM_CONFIG                                      0x0001029D

/** Version information used to handle future additions to
    AFE_PARAM_ID_TDM_CONFIG processing (for backward compatibility).
*/
#define AFE_API_VERSION_TDM_CONFIG                                   1

/** Short (one-bit) Synchronization mode. */
#define AFE_PORT_TDM_SHORT_SYNC_BIT_MODE                             0

/** Long Synchronization mode. */
#define AFE_PORT_TDM_LONG_SYNC_MODE                                  1

/** Short (one-slot) Synchronization mode. */
#define AFE_PORT_TDM_SHORT_SYNC_SLOT_MODE                            2

/** Synchronization source is external. */
#define AFE_PORT_TDM_SYNC_SRC_EXTERNAL                               0

/** Synchronization source is internal. */
#define AFE_PORT_TDM_SYNC_SRC_INTERNAL                               1

/** Disable sharing of the data-out signal. */
#define AFE_PORT_TDM_CTRL_DATA_OE_DISABLE                            0

/** Enable sharing of the data-out signal. */
#define AFE_PORT_TDM_CTRL_DATA_OE_ENABLE                             1

/** Normal synchronization. */
#define AFE_PORT_TDM_SYNC_NORMAL                                     0

/** Invert the synchronization. */
#define AFE_PORT_TDM_SYNC_INVERT                                     1

/** Zero-bit clock cycle synchronization data delay. */
#define AFE_PORT_TDM_DATA_DELAY_0_BCLK_CYCLE                         0

/** One-bit clock cycle synchronization data delay. */
#define AFE_PORT_TDM_DATA_DELAY_1_BCLK_CYCLE                         1

/** Two-bit clock cycle synchronization data delay. @newpage */
#define AFE_PORT_TDM_DATA_DELAY_2_BCLK_CYCLE                         2

typedef struct afe_param_id_tdm_cfg_t afe_param_id_tdm_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_tdm_cfg_t
@{ */
/* Payload of the AFE_PARAM_ID_TDM_CONFIG parameter used by
    AFE_MODULE_AUDIO_DEV_INTERFACE.
 */
struct afe_param_id_tdm_cfg_t
{
   uint32_t                  tdm_cfg_minor_version;
   /**< Tracks the configuration of this parameter.

        @values #AFE_API_VERSION_TDM_CONFIG */

    uint32_t                  num_channels;
    /**< Number of enabled slots for the TDM frame.

         @values 1 to 8 */

    uint32_t                  sample_rate;
    /**< Sampling rate of the port.

         @values
         - #AFE_PORT_SAMPLE_RATE_8K
         - #AFE_PORT_SAMPLE_RATE_11_025K
         - #AFE_PORT_SAMPLE_RATE_16K
         - #AFE_PORT_SAMPLE_RATE_22_05K
         - #AFE_PORT_SAMPLE_RATE_24K
         - #AFE_PORT_SAMPLE_RATE_32K
         - #AFE_PORT_SAMPLE_RATE_44_1K
         - #AFE_PORT_SAMPLE_RATE_48K
         - #AFE_PORT_SAMPLE_RATE_88_2K
         - #AFE_PORT_SAMPLE_RATE_96K
         - #AFE_PORT_SAMPLE_RATE_176_4K
         - #AFE_PORT_SAMPLE_RATE_192K
         - #AFE_PORT_SAMPLE_RATE_352_8K
         - #AFE_PORT_SAMPLE_RATE_384K @tablebulletend */

    uint32_t                  bit_width;
    /**< Bit width of the sample.

         @values 16, 24, 32 */

    uint16_t                  data_format;
    /**< Data format supported by the TDM frame.

         @values
         - #AFE_LINEAR_PCM_DATA
         - #AFE_NON_LINEAR_DATA
         - #AFE_GENERIC_COMPRESSED_DATA @tablebulletend */

    uint16_t                  sync_mode;
    /**< TDM Synchronization mode setting.

         @values
         - #AFE_PORT_TDM_SHORT_SYNC_BIT_MODE
         - #AFE_PORT_TDM_LONG_SYNC_MODE
         - #AFE_PORT_TDM_SHORT_SYNC_SLOT_MODE @tablebulletend */

    uint16_t                  sync_src;
    /**< Synchronization source.

         @values
         - #AFE_PORT_TDM_SYNC_SRC_EXTERNAL
         - #AFE_PORT_TDM_SYNC_SRC_INTERNAL @tablebulletend */

    uint16_t                  nslots_per_frame;
    /**< Number of slots per frame.

         @values 1 to 32

         Typical values are 1, 2, 4, 8, 16, and 32. */

    uint16_t                  ctrl_data_out_enable;
    /**< Specifies whether the TDM hardware block shares the data-out signal
         to the driver with other masters.

         @values
         - #AFE_PORT_TDM_CTRL_DATA_OE_DISABLE
         - #AFE_PORT_TDM_CTRL_DATA_OE_ENABLE @tablebulletend */

    uint16_t                  ctrl_invert_sync_pulse;
    /**< Specifies whether to invert synchronization.

         @values
         - #AFE_PORT_TDM_SYNC_NORMAL
         - #AFE_PORT_TDM_SYNC_INVERT @tablebulletend */

    uint16_t                  ctrl_sync_data_delay;
    /**< Number of bit-clock cycles for delaying the data for synchronization.

         @values
         - #AFE_PORT_TDM_DATA_DELAY_0_BCLK_CYCLE
         - #AFE_PORT_TDM_DATA_DELAY_1_BCLK_CYCLE
         - #AFE_PORT_TDM_DATA_DELAY_2_BCLK_CYCLE @tablebulletend */

    uint16_t                  slot_width;
    /**< Width of the slot in a TDM frame.

         @values 16, 24, 32

         The slot_width value must be @ge the bit_width value.*/

    uint32_t                  slot_mask;
    /**< Position of the active slots.

         @values 1 to 2^32-1

         Bits 0 to 31 correspond to slots 0 to 31. When a bit is set, the
         corresponding slot is active. The number of active slots can be
         inferred from the number of bits set in the mask.

         Only eight individual bits can be enabled. */

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_weak_afe_param_id_tdm_cfg_t */

/** ID of the parameter used by #AFE_MODULE_AUDIO_DEV_INTERFACE to configure
    the HDMI interface.

    #AFE_PORT_CMD_SET_PARAM_V2 (deprecated), #AFE_PORT_CMD_GET_PARAM_V2
    (deprecated), #AFE_PORT_CMD_SET_PARAM_V3, and #AFE_PORT_CMD_GET_PARAM_V3
    can use this parameter ID.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_HDMI_CONFIG

    @apr_msgpayload{afe_param_id_hdmi_multi_chan_audio_cfg_t}
    @table{weak__afe__param__id__hdmi__multi__chan__audio__cfg__t}
 */
#define AFE_PARAM_ID_HDMI_CONFIG                                      0x00010210

/** Version information used to handle future additions to
    AFE_PARAM_ID_HDMI_CONFIG processing (for backward compatibility). @newpage
 */
#define AFE_API_VERSION_HDMI_CONFIG                                 0x1

typedef struct afe_param_id_hdmi_multi_chan_audio_cfg_t afe_param_id_hdmi_multi_chan_audio_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_hdmi_multi_chan_audio_cfg_t
@{ */
/* Payload of the AFE_PARAM_ID_HDMI_CONFIG parameter used by
    AFE_MODULE_AUDIO_DEV_INTERFACE.
 */
struct afe_param_id_hdmi_multi_chan_audio_cfg_t
{
   uint32_t                  hdmi_cfg_minor_version;
   /**< Tracks the configuration of this parameter.

        @values #AFE_API_VERSION_HDMI_CONFIG */

    uint16_t                  data_type;
    /**< Valid data type.

         @values
         - #AFE_LINEAR_PCM_DATA
         - #AFE_NON_LINEAR_DATA
         - #AFE_LINEAR_PCM_DATA_PACKED_60958
         - #AFE_NON_LINEAR_DATA_PACKED_60958 @tablebulletend */

   uint16_t                  channel_allocation;
   /**< HDMI channel allocation information for programming an HDMI frame.
        The default is 0 (Stereo).

        This information is defined in the HDMI standard, CEA 861-D
        (refer to the @xhyperref{CEA861D,CEA 861-D} standard). The number of
        channels is also inferred from this parameter. */

    uint32_t                  sample_rate;
    /**< Sampling rate of the port.

         @values
         - #AFE_PORT_SAMPLE_RATE_8K
         - #AFE_PORT_SAMPLE_RATE_16K
         - #AFE_PORT_SAMPLE_RATE_22_05K
         - #AFE_PORT_SAMPLE_RATE_32K
         - #AFE_PORT_SAMPLE_RATE_44_1K
         - #AFE_PORT_SAMPLE_RATE_48K
         - #AFE_PORT_SAMPLE_RATE_96K
         - #AFE_PORT_SAMPLE_RATE_176_4K
         - #AFE_PORT_SAMPLE_RATE_192K

         For the HDMI interface, sampling rates of 22.05 kHz, 44.1 kHz, and
         176.4 kHz are applicable only for compressed data (non-PCM). */

    uint16_t                  bit_width;
    /**< Bit width of the sample.

         @values 16, 24 */

   uint16_t                  reserved;
   /**< This field must be set to 0. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_param_id_hdmi_multi_chan_audio_cfg_t */

/** ID of the parameter used by #AFE_MODULE_AUDIO_DEV_INTERFACE to configure
    the USB audio interface.

    #AFE_PORT_CMD_SET_PARAM_V2 (deprecated), #AFE_PORT_CMD_GET_PARAM_V2
    (deprecated), #AFE_PORT_CMD_SET_PARAM_V3, and #AFE_PORT_CMD_GET_PARAM_V3
    can use this parameter ID.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_USB_AUDIO_CONFIG

    @apr_msgpayload{afe_param_id_usb_audio_cfg_t}
    @table{weak__afe__param__id__usb__audio__cfg__t} @newpage
 */
#define AFE_PARAM_ID_USB_AUDIO_CONFIG                                      0x000102a4

/** Version information used to handle future additions to
    AFE_PARAM_ID_USB_AUDIO_CONFIG processing (for backward compatibility).
 */
#define AFE_API_VERSION_USB_AUDIO_CONFIG                                 0x1

/** Enumeration for setting the data format to linear PCM when
    audio data is transmitted or received over a USB audio device. @newpage
 */
#define AFE_USB_AUDIO_DATA_FORMAT_LINEAR_PCM                             0x0

typedef struct afe_param_id_usb_audio_cfg_v1_t afe_param_id_usb_audio_cfg_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_usb_audio_cfg_t
@{ */
/* Payload of the AFE_PARAM_ID_USB_AUDIO_CONFIG parameter used by
    AFE_MODULE_AUDIO_DEV_INTERFACE.
 */
struct afe_param_id_usb_audio_cfg_v1_t
{
   uint32_t                  minor_version;
   /**< Tracks the configuration of this parameter.

        @values #AFE_API_VERSION_USB_AUDIO_CONFIG */

    uint32_t                  sample_rate;
    /**< Sampling rate of the USB audio port.

         @values
         - #AFE_PORT_SAMPLE_RATE_8K
         - #AFE_PORT_SAMPLE_RATE_11_025K
         - #AFE_PORT_SAMPLE_RATE_12K
         - #AFE_PORT_SAMPLE_RATE_16K
         - #AFE_PORT_SAMPLE_RATE_22_05K
         - #AFE_PORT_SAMPLE_RATE_24K
         - #AFE_PORT_SAMPLE_RATE_32K
         - #AFE_PORT_SAMPLE_RATE_44_1K
         - #AFE_PORT_SAMPLE_RATE_48K
         - #AFE_PORT_SAMPLE_RATE_88_2K
         - #AFE_PORT_SAMPLE_RATE_96K
         - #AFE_PORT_SAMPLE_RATE_176_4K
         - #AFE_PORT_SAMPLE_RATE_192K
         - #AFE_PORT_SAMPLE_RATE_352_8K
         - #AFE_PORT_SAMPLE_RATE_384K @tablebulletend */

    uint16_t                  bit_width;
    /**< Bit width of the sample.

         @values 16, 24, 32 */

    uint16_t                  num_channels;
    /**< Number of channels.

         @values 1, 2, 3, 4, 5, 6, 7, 8 */

    uint16_t                  data_format;
    /**< Data format supported by the USB audio device.

         @values #AFE_USB_AUDIO_DATA_FORMAT_LINEAR_PCM */

   uint16_t                  reserved;
   /**< This field must be set to 0. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup afe_param_id_usb_audio_cfg_v1_t */

/** ID of the parameter used by #AFE_MODULE_AUDIO_DEV_INTERFACE to
    configure USB audio interface device parameters.

    #AFE_PORT_CMD_SET_PARAM_V2 (deprecated), #AFE_PORT_CMD_GET_PARAM_V2
    (deprecated), #AFE_PORT_CMD_SET_PARAM_V3, and #AFE_PORT_CMD_GET_PARAM_V3
    can use this parameter ID.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_USB_AUDIO_DEV_PARAMS

    @apr_msgpayload{afe_param_id_usb_audio_dev_params_t}
    @table{weak__afe__param__id__usb__audio__dev__params__t}
 */
#define AFE_PARAM_ID_USB_AUDIO_DEV_PARAMS                                      0x000102a5

/** Version information used to handle future additions to
    AFE_PARAM_ID_USB_AUDIO_DEV_PARAMS processing (for backward compatibility).
    @newpage
 */
#define AFE_API_VERSION_USB_AUDIO_DEV_PARAMS                                 0x1

typedef struct afe_param_id_usb_audio_dev_params_v1_t afe_param_id_usb_audio_dev_params_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_usb_audio_dev_params_t
@{ */
/* Payload of the AFE_PARAM_ID_USB_AUDIO_DEV_PARAMS parameter used by
    AFE_MODULE_AUDIO_DEV_INTERFACE.
 */
struct afe_param_id_usb_audio_dev_params_v1_t
{
   uint32_t                  minor_version;
   /**< Tracks the configuration of this parameter.

        @values #AFE_API_VERSION_USB_AUDIO_DEV_PARAMS */

   uint32_t                  usb_device_token;
   /**< Valid token for the USB audio device. */

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup afe_param_id_usb_audio_dev_params_v1_t */

/** ID of the parameter used by #AFE_MODULE_AUDIO_DEV_INTERFACE to configure
    the linear PCM format for the USB audio device.

    #AFE_PORT_CMD_SET_PARAM_V2 (deprecated), #AFE_PORT_CMD_GET_PARAM_V2
    (deprecated), #AFE_PORT_CMD_SET_PARAM_V3, and #AFE_PORT_CMD_GET_PARAM_V3
    can use this parameter ID.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_USB_AUDIO_DEV_LPCM_FMT

    @apr_msgpayload{afe_param_id_usb_audio_dev_lpcm_fmt_v1_t}
    @table{weak__afe__param__id__usb__audio__dev__lpcm__fmt__v1__t}
 */
#define AFE_PARAM_ID_USB_AUDIO_DEV_LPCM_FMT                                      0x000102aa

/** Version information used to handle future additions to
    AFE_PARAM_ID_USB_AUDIO_DEV_LPCM_FMT processing (for backward compatibility).
    @newpage
 */
#define AFE_API_VERSION_USB_AUDIO_DEV_LPCM_FMT                                 0x1

typedef struct afe_param_id_usb_audio_dev_lpcm_fmt_v1_t afe_param_id_usb_audio_dev_lpcm_fmt_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_usb_audio_dev_lpcm_fmt_v1_t
@{ */
/* Payload of the AFE_PARAM_ID_USB_AUDIO_DEV_LPCM_FMT parameter used by
    AFE_MODULE_AUDIO_DEV_INTERFACE.
 */
struct afe_param_id_usb_audio_dev_lpcm_fmt_v1_t
{
  uint32_t                  minor_version;
   /**< Tracks the configuration of this parameter.

        @values #AFE_API_VERSION_USB_AUDIO_DEV_LPCM_FMT */

  uint32_t                  media_format;
  /**< Media format of the LPCM data.

         @values

         Bit 0 indicates endianness: @vertspace{-4}
          - 0 -- Little endian
          - 1 -- Big endian

         Bit 1 indicates sign: @vertspace{-4}
          - 0 -- Signed
          - 1 -- Unsigned @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup afe_param_id_usb_audio_dev_lpcm_fmt_v1_t */

/** ID of the parameter used by #AFE_MODULE_AUDIO_DEV_INTERFACE to configure
    the SPDIF interface.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_SPDIF_CONFIG

    @apr_msgpayload{afe_param_id_spdif_audio_cfg_t}
    @table{weak__afe__param__id__spdif__audio__cfg__t}
 */
#define AFE_PARAM_ID_SPDIF_CONFIG                                    0x00010244

/** Version information used to handle future additions to
    AFE_PARAM_ID_SPDIF_CONFIG processing (for backward compatibility). @newpage
 */
#define AFE_API_VERSION_SPDIF_CONFIG                                 0x1

typedef struct afe_param_id_spdif_audio_cfg_t afe_param_id_spdif_audio_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_spdif_audio_cfg_t
@{ */
/* Payload of the AFE_PARAM_ID_SPDIF_CONFIG parameter used by
    AFE_MODULE_AUDIO_DEV_INTERFACE.
 */
struct afe_param_id_spdif_audio_cfg_t
{
   uint32_t                  spdif_cfg_minor_version;
   /**< Tracks the configuration of this parameter.

        @values #AFE_API_VERSION_SPDIF_CONFIG */

   uint32_t                  sample_rate;
   /**< Sampling rate of the port.

        @values
         - #AFE_PORT_SAMPLE_RATE_22_05K; for compressed use case only
         - #AFE_PORT_SAMPLE_RATE_32K
         - #AFE_PORT_SAMPLE_RATE_44_1K
         - #AFE_PORT_SAMPLE_RATE_48K
         - #AFE_PORT_SAMPLE_RATE_88_2K
         - #AFE_PORT_SAMPLE_RATE_96K
         - #AFE_PORT_SAMPLE_RATE_176_4K
         - #AFE_PORT_SAMPLE_RATE_192K @tablebulletend */

   uint16_t                  data_format;
   /**< Data format supported by the SPDIF interface.

        @values
         - #AFE_LINEAR_PCM_DATA
         - #AFE_NON_LINEAR_DATA @tablebulletend */

   uint16_t                  num_channels;
   /**< Number of channels.

        @values 1 to 2 */

   uint16_t                  bit_width;
   /**< Bit width of the sample.

        @values 16, 24 */

   uint16_t                  reserved;
   /**< This field must be set to 0. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_param_id_spdif_audio_cfg_t */

/** ID of the parameter used by #AFE_MODULE_AUDIO_DEV_INTERFACE to
    configure the channel status bits. Only #AFE_PORT_ID_SPDIF_RX can use this
    parameter ID.

    Channel status bits must be called after #AFE_PORT_CMD_SET_PARAM_V2
    (deprecated) or #AFE_PORT_CMD_SET_PARAM_V3 is issued to configure the port
    parameter.

    This parameter ID can support dynamic updates only on the copyright bit
    (bit 2), depending on the hardware version:
    - 0 -- Software for which a copyright is asserted
    - 1 -- Software for which no copyright is asserted
    - For more details, refer to @xhyperref{IEC609583,IEC 60958-3}

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_CHSTATUS_CONFIG

    @apr_msgpayload{afe_param_id_chstatus_cfg_t}
    @table{weak__afe__param__id__chstatus__cfg__t}
 */
#define AFE_PARAM_ID_CH_STATUS_CONFIG                           0x00010245

/** Version information used to handle future additions to
    AFE_PARAM_ID_CH_STATUS_CONFIG processing (for backward compatibility).
 */
#define AFE_API_VERSION_CH_STATUS_CONFIG                        0x1

/* Enumeration for channel type */

/** Channel status bits for channel A. */
#define AFE_CH_STATUS_A                                         0x1

/** Channel status bits for channel B. */
#define AFE_CH_STATUS_B                                         0x2

/** Enumeration for the channel status byte size. @newpage */
#define AFE_CH_STATUS_BYTE_SIZE                                 24

typedef struct afe_param_id_chstatus_cfg_t afe_param_id_chstatus_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_chstatus_cfg_t
@{ */
/* Payload of the AFE_PARAM_ID_CHSTATUS_CONFIG parameter used by
    AFE_MODULE_AUDIO_DEV_INTERFACE.
 */
struct afe_param_id_chstatus_cfg_t
{
   uint32_t                  ch_status_cfg_minor_version;
   /**< Tracks the configuration of this parameter.

        @values #AFE_API_VERSION_CH_STATUS_CONFIG */

   uint32_t                  ch_status_type;
   /**< Indicates whether the channel status is for channel A or B.

        @values
        - #AFE_CH_STATUS_A
        - #AFE_CH_STATUS_B @tablebulletend */

   uint8_t   ch_stat_buf[AFE_CH_STATUS_BYTE_SIZE];
   /**< 192 channel status bits for the channel.

        @values The byte ordering is the same as the ordering defined in
                @xhyperref{IEC609583,IEC 60958-3}. */

   uint8_t   ch_stat_mask[AFE_CH_STATUS_BYTE_SIZE];
   /**< 192 channel status bitmasks for the channel.

        @values The byte ordering is the same as the ordering defined in
                @xhyperref{IEC609583,IEC 60958-3}. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_param_id_chstatus_cfg_t */
/** @} */ /* end_addtogroup afe_module_audio_device */


/** @addtogroup afe_mod_param_id_deprecated
@{ */
/** Deprecated. Use #AFE_PARAM_ID_GROUP_DEVICE_HDMI_MULTISTREAM_CONFIG instead.

    ID of the parameter used by #AFE_MODULE_AUDIO_DEV_INTERFACE to
    configure the HDMI input interface.

    #AFE_PORT_CMD_SET_PARAM_V2 (deprecated), #AFE_PORT_CMD_GET_PARAM_V2
    (deprecated), #AFE_PORT_CMD_SET_PARAM_V3, and #AFE_PORT_CMD_GET_PARAM_V3 can
    use this parameter ID.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_HDMI_CONFIG

    @apr_msgpayload{afe_param_id_hdmi_input_cfg_t}
    @table{weak__afe__param__id__hdmi__input__cfg__t}
 */
#define AFE_PARAM_ID_HDMI_INPUT_CONFIG                              0x00010248

/** Deprecated. Version information used to handle future additions to
    AFE_PARAM_ID_HDMI_INPUT_CONFIG processing (for backward compatibility).
 */
#define AFE_API_VERSION_HDMI_INPUT_CONFIG                           0x1

typedef struct afe_param_id_hdmi_input_cfg_t afe_param_id_hdmi_input_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_hdmi_input_cfg_t
@{ */
/* Payload of the AFE_PARAM_ID_HDMI_INPUT_CONFIG parameter used by
    AFE_MODULE_AUDIO_DEV_INTERFACE.
 */
struct afe_param_id_hdmi_input_cfg_t
{
   uint32_t                  hdmi_input_cfg_minor_version;
   /**< Tracks the configuration of this parameter.

        @values #AFE_API_VERSION_HDMI_INPUT_CONFIG */

   uint16_t                  data_type;
   /**< Valid data type. @tablebulletend */

   uint16_t                  num_channels;
   /**< Number of channels.

        @values 1 to 8 */

   uint32_t                  sample_rate;
   /**< Sampling rate of the port.

         @values
         - #AFE_PORT_SAMPLE_RATE_32K
         - #AFE_PORT_SAMPLE_RATE_44_1K
         - #AFE_PORT_SAMPLE_RATE_48K
         - #AFE_PORT_SAMPLE_RATE_96K
         - #AFE_PORT_SAMPLE_RATE_176_4K
         - #AFE_PORT_SAMPLE_RATE_192K @tablebulletend */

   uint16_t                  bit_width;
   /**< Bit width of the sample.

         @values 16, 24 */

   uint16_t                  reserved;
   /**< This field must be set to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_param_id_hdmi_input_cfg_t */
/** @} */ /* end_addtogroup afe_mod_param_id_deprecated */


/** @addtogroup afe_module_audio_device
@{ */
/** ID of the parameter used by #AFE_MODULE_AUDIO_DEV_INTERFACE to configure
    the Bluetooth FM interface.

    #AFE_PORT_CMD_SET_PARAM_V2 (deprecated), #AFE_PORT_CMD_GET_PARAM_V2
    (deprecated), #AFE_PORT_CMD_SET_PARAM_V3, and #AFE_PORT_CMD_GET_PARAM_V3
    can use this parameter ID.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_INTERNAL_BT_FM_CONFIG

    @apr_msgpayload{afe_param_id_internal_bt_fm_cfg_t}
    @table{weak__afe__param__id__internal__bt__fm__cfg__t}
 */
#define AFE_PARAM_ID_INTERNAL_BT_FM_CONFIG                          0x00010211

/** Version information used to handle future additions to
    AFE_PARAM_ID_INTERNAL_BT_FM_CONFIG processing (for backward compatibility).
    @newpage
 */
#define AFE_API_VERSION_INTERNAL_BT_FM_CONFIG                         0x1

typedef struct afe_param_id_internal_bt_fm_cfg_t afe_param_id_internal_bt_fm_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_internal_bt_fm_cfg_t
@{ */
/* Payload of the AFE_PARAM_ID_INTERNAL_BT_FM_CONFIG parameter used by
    AFE_MODULE_AUDIO_DEV_INTERFACE.
 */
struct afe_param_id_internal_bt_fm_cfg_t
{
    uint32_t                  bt_fm_cfg_minor_version;
    /**< Tracks the configuration of this parameter.

         @values #AFE_API_VERSION_INTERNAL_BT_FM_CONFIG */

    uint16_t                  num_channels;
    /**< Number of channels.

         @values
         - 1 -- Only for Bluetooth SCO (BTSCO)
         - 2 -- Only for Bluetooth FM and A2DP) @tablebulletend */

    uint16_t                  bit_width;
    /**< Bit width of the sample.

         @values 16 */

    uint32_t                  sample_rate;
    /**< Sampling rate of the port.

         @values
         - #AFE_PORT_SAMPLE_RATE_8K -- Only for BTSCO
         - #AFE_PORT_SAMPLE_RATE_16K -- Only for BTSCO
         - #AFE_PORT_SAMPLE_RATE_48K -- Bluetooth FM and A2DP @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_param_id_internal_bt_fm_cfg_t */

/** ID of the parameter used by #AFE_MODULE_AUDIO_DEV_INTERFACE to configure
    the SLIMbus interface utilizing a shared channel approach.

    #AFE_PORT_CMD_SET_PARAM_V2 (deprecated), #AFE_PORT_CMD_GET_PARAM_V2
    (deprecated), #AFE_PORT_CMD_SET_PARAM_V3, and #AFE_PORT_CMD_GET_PARAM_V3
    can use this parameter ID.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_SLIMBUS_CONFIG

    @apr_msgpayload{afe_port_cmd_slimbus_cfg_t}
    @table{weak__afe__port__cmd__slimbus__cfg__t}
 */
#define AFE_PARAM_ID_SLIMBUS_CONFIG                                   0x00010212

/** Version information used to handle future additions to
    AFE_PARAM_ID_SLIMBUS_CONFIG processing (for backward compatibility).
 */
#define AFE_API_VERSION_SLIMBUS_CONFIG                                 0x1

/** Enumeration for setting SLIMbus device ID 1. */
#define AFE_SLIMBUS_DEVICE_1                                         0x0

/** Enumeration for setting SLIMbus device ID 2. */
#define AFE_SLIMBUS_DEVICE_2                                         0x1

/** Enumeration for setting the SLIMbus data formats. */
#define AFE_SB_DATA_FORMAT_NOT_INDICATED                             0x0

/** Enumeration for setting the SLIMbus data format when two 16-bit data
    samples are packed in a 32-bit device sample. The least significant sample
    is mapped channel 0, and the most significant sample is mapped to channel 1.
 */
#define AFE_SB_DATA_FORMAT_LINEAR_PCM_PACKED_16BIT                   0x1

/** Enumeration for setting the data format to DSD when DSD DoP data is
    transmitted or received over SLIMbus lines.

    This format is currently supported on the AFE Rx path only.
 */
#define AFE_SB_DATA_FORMAT_DSD_DOP                                           0x2

/** Enumeration for setting the data format to compressed data for
    transmitting or receiving over SLIMbus lines.
 */
#define AFE_SB_DATA_FORMAT_GENERIC_COMPRESSED                 0x3

/** Enumeration for setting the maximum number of streams per device. */
#define AFE_PORT_MAX_CHANNEL_CNT        0x8

/** Enumeration for setting the maximum number of channels per device @newpage */
#define AFE_PORT_MAX_AUDIO_CHAN_CNT     AFE_PORT_MAX_CHANNEL_CNT

typedef struct afe_param_id_slimbus_cfg_t afe_param_id_slimbus_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_port_cmd_slimbus_cfg_t
@{ */
/* Payload of the AFE_PARAM_ID_SLIMBUS_CONFIG parameter used by
    AFE_MODULE_AUDIO_DEV_INTERFACE.
 */
struct afe_param_id_slimbus_cfg_t
{
    uint32_t                  sb_cfg_minor_version;
    /**< Tracks the configuration of this parameter.

         @values #AFE_API_VERSION_SLIMBUS_CONFIG */

    uint16_t                  slimbus_dev_id;
    /**< ID of the SLIMbus hardware device. This ID is required to handle
         multiple SLIMbus hardware blocks.

         @values
         - #AFE_SLIMBUS_DEVICE_1
         - #AFE_SLIMBUS_DEVICE_2 @tablebulletend */

    uint16_t                  bit_width;
    /**< Bit width of the sample.

         @values 16, 24, 32 */

    uint16_t                  data_format;
    /**< Data format supported by the SLIMbus hardware.

         @values
         - #AFE_SB_DATA_FORMAT_NOT_INDICATED (Default)
         - #AFE_SB_DATA_FORMAT_LINEAR_PCM_PACKED_16BIT
         - #AFE_SB_DATA_FORMAT_DSD_DOP
         - #AFE_SB_DATA_FORMAT_GENERIC_COMPRESSED

         The default of 0 (AFE_SB_DATA_FORMAT_NOT_INDICATED) indicates that the
         hardware does not perform any format conversions before the data
         transfer. */

    uint16_t                  num_channels;
    /**< Number of channels.

         @values 1 to #AFE_PORT_MAX_AUDIO_CHAN_CNT */

    uint8_t                  shared_channel_mapping[AFE_PORT_MAX_AUDIO_CHAN_CNT];
    /**< Array of shared channel IDs (128 to 255) to which the master port
         is connected.

         Shared_channel_mapping[i] represents the shared channel assigned for
         audio channel i in multichannel audio data. */

    uint32_t              sample_rate;
    /**< Sampling rate of the port.

         @values
         - #AFE_PORT_SAMPLE_RATE_8K
         - #AFE_PORT_SAMPLE_RATE_16K
         - #AFE_PORT_SAMPLE_RATE_22_05K
         - #AFE_PORT_SAMPLE_RATE_24K
         - #AFE_PORT_SAMPLE_RATE_32K
         - #AFE_PORT_SAMPLE_RATE_44_1K
         - #AFE_PORT_SAMPLE_RATE_48K
         - #AFE_PORT_SAMPLE_RATE_88_2K
         - #AFE_PORT_SAMPLE_RATE_96K
         - #AFE_PORT_SAMPLE_RATE_176_4K
         - #AFE_PORT_SAMPLE_RATE_192K
         - #AFE_PORT_SAMPLE_RATE_384K
         - #AFE_PORT_SAMPLE_RATE_352_8K

         At 44.1 kHz, the SLIMbus interface supports PCM data
         in both the Tx direction and Rx direction.

         There is no concurrent support of a voice call when the port operates
         at 44.1 kHz because of RTD requirements.

         The AFE and COPP/COPREP must run at the same sampling rate.
         All AFE clients must run at the same sampling rate (44.1 kHz) on the
         same device. This requirement applies to other sampling rates that are
         either multiples or divisors of 44.1 kHz (22.05 kHz, 88.2 kHz,
         176.4 kHz). */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_port_cmd_slimbus_cfg_t */

/** ID of the parameter used by #AFE_MODULE_AUDIO_DEV_INTERFACE to configure
    the Real-time Proxy interface.

    #AFE_PORT_CMD_SET_PARAM_V2 (deprecated), #AFE_PORT_CMD_GET_PARAM_V2
    (deprecated), #AFE_PORT_CMD_SET_PARAM_V3, and #AFE_PORT_CMD_GET_PARAM_V3
    can use this parameter ID.

    @apr_hdr_fields
    Opcode --AFE_PARAM_ID_RT_PROXY_CONFIG

    @apr_msgpayload{afe_param_id_rt_proxy_port_cfg_t}
    @table{weak__afe__param__id__rt__proxy__port__cfg__t}
 */
#define AFE_PARAM_ID_RT_PROXY_CONFIG                                  0x00010213

/** Version information used to handle future additions to
    AFE_PARAM_ID_RT_PROXY_CONFIG processing (for backward compatibility).
    @newpage
 */
#define AFE_API_VERSION_RT_PROXY_CONFIG                             0x1

typedef struct afe_param_id_rt_proxy_port_cfg_t afe_param_id_rt_proxy_port_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_rt_proxy_port_cfg_t
@{ */
/* Payload of the AFE_PARAM_ID_RT_PROXY_CONFIG parameter used by
    AFE_MODULE_AUDIO_DEV_INTERFACE.
 */
struct afe_param_id_rt_proxy_port_cfg_t
{
    uint32_t                  rt_proxy_cfg_minor_version;
    /**< Tracks the configuration of this parameter.

         @values #AFE_API_VERSION_RT_PROXY_CONFIG */

    uint16_t                  bit_width;
    /**< Bit width of the sample.

         @values 16, 24, 32 */

    uint16_t                  interleaved;
    /**< Specifies whether the data exchanged between the AFE interface and
         real-time port is interleaved.

         @values
         - 0 -- Non-interleaved (samples from each channel are contiguous in
                the buffer)
         - 1 -- Interleaved (corresponding samples from each input channel are
                interleaved within the buffer) @tablebulletend */

    uint16_t                  frame_size;
    /**< Size of the frames that are used for PCM exchanges with this port.

         @values > 0, in bytes

         For example, 5 ms buffers of 16 bits and 16 kHz stereo samples
         is 5 ms * 16 samples/ms * 2 bytes/sample * 2 channels = 320 bytes. */

    uint16_t                  jitter_allowance;
    /**< Configures the amount of jitter that the port allows.

         @values > 0

         For example, if +/-10 ms of jitter is anticipated in the
         timing of sending frames to the port, and the configuration is 16 kHz
         mono with 16-bit samples, this field is 10 ms * 16 samples/ms * 2
         bytes/sample = 320. */

    uint16_t                  low_water_mark;
    /**< Low watermark in bytes (including all channels).

         @values
         - 0 -- Do not send any low watermark events
         - > 0 -- Low watermark for triggering an event

         If the number of bytes in an internal circular buffer is lower than
         this low_water_mark parameter, a LOW_WATER_MARK event is sent to
         applications (via #AFE_EVENT_RT_PROXY_PORT_STATUS).

         The use of watermark events is optional for debugging purposes. */

    uint16_t                  high_water_mark;
    /**< High watermark in bytes (including all channels).

         @values
         - 0 -- Do not send any high watermark events
         - > 0 -- High watermark for triggering an event

         If the number of bytes in an internal circular buffer exceeds
         TOTAL_CIRC_BUF_SIZE minus high_water_mark, a high watermark event is
         sent to applications (via #AFE_EVENT_RT_PROXY_PORT_STATUS).

         The use of watermark events is optional and for debugging purposes. */

    uint32_t                    sample_rate;
    /**< Sampling rate of the port.

         @values
         - #AFE_PORT_SAMPLE_RATE_8K
         - #AFE_PORT_SAMPLE_RATE_16K
         - #AFE_PORT_SAMPLE_RATE_22_05K
         - #AFE_PORT_SAMPLE_RATE_24K
         - #AFE_PORT_SAMPLE_RATE_32K
         - #AFE_PORT_SAMPLE_RATE_44_1K
         - #AFE_PORT_SAMPLE_RATE_48K
         - #AFE_PORT_SAMPLE_RATE_88_2K
         - #AFE_PORT_SAMPLE_RATE_96K
         - #AFE_PORT_SAMPLE_RATE_176_4K
         - #AFE_PORT_SAMPLE_RATE_192K
         - #AFE_PORT_SAMPLE_RATE_384K
         - #AFE_PORT_SAMPLE_RATE_352_8K@tablebulletend */

    uint16_t                  num_channels;
    /**< Number of channels.

         @values 1 to #AFE_PORT_MAX_AUDIO_CHAN_CNT */

    uint16_t                  reserved;
    /**< This field must be set to 0. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_param_id_rt_proxy_port_cfg_t */

/** ID of the parameter used by #AFE_MODULE_AUDIO_DEV_INTERFACE to configure
    the pseudoport interface.

    Only #AFE_PORT_CMD_SET_PARAM_V2 (deprecated) and #AFE_PORT_CMD_SET_PARAM_V3
    can use this parameter ID.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_PSEUDO_PORT_CONFIG @vertspace{-3}

    @apr_msgpayload{afe_param_id_pseudo_port_cfg_t}
    @table{weak__afe__param__id__pseudo__port__cfg__t} @newpage
 */
#define AFE_PARAM_ID_PSEUDO_PORT_CONFIG                             0x00010219

/** Version information used to handle future additions to
    AFE_PARAM_ID_PSEUDO_PORT_CONFIG processing (for backward compatibility).
 */
#define AFE_API_VERSION_PSEUDO_PORT_CONFIG                          0x1

/** @} */ /* end_addtogroup afe_module_audio_device */


/** @addtogroup afe_mod_param_id_deprecated
@{ */
/** Deprecated.

    Enumeration for setting the timing_mode field to faster than real time.

    If AFE_PSEUDOPORT_TIMING_MODE_FTRT timing mode is used, the pseudoport
    falls back to use #AFE_PSEUDOPORT_TIMING_MODE_REALTIME.
 */
#define AFE_PSEUDOPORT_TIMING_MODE_FTRT                             0x0

/** Deprecated.

    Enumeration for setting the timing_mode field to real time using
    timers.

    If AFE_PSEUDOPORT_TIMING_MODE_TIMER timing mode is used, the pseudoport
    falls back to use #AFE_PSEUDOPORT_TIMING_MODE_REALTIME. @newpage
 */
#define AFE_PSEUDOPORT_TIMING_MODE_TIMER                            0x1

/** @} */ /* end_addtogroup afe_mod_param_id_deprecated */


/** @addtogroup afe_module_audio_device
@{ */
/** Enumeration for setting the pseudoport to act as a real-time port. @newpage
 */
#define AFE_PSEUDOPORT_TIMING_MODE_REALTIME                         0x2

typedef struct afe_param_id_pseudo_port_cfg_t afe_param_id_pseudo_port_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_pseudo_port_cfg_t
@{ */
/* Payload of the AFE_PARAM_ID_PSEUDO_PORT_CONFIG parameter used by
    AFE_MODULE_AUDIO_DEV_INTERFACE.
 */
struct afe_param_id_pseudo_port_cfg_t
{
    uint32_t                  pseud_port_cfg_minor_version;
    /**< Tracks the configuration of this parameter.

         @values #AFE_API_VERSION_PSEUDO_PORT_CONFIG */

    uint16_t                  bit_width;
    /**< Bit width of the sample.

         @values 16, 24, 32 */

    uint16_t                  num_channels;
    /**< Number of channels.

         @values 1 to 8 */

    uint16_t                  data_format;
    /**< Nonlinear data format supported by the pseudoport (for future use).

         @values #AFE_LINEAR_PCM_DATA */

    uint16_t                  timing_mode;
    /**< Specifies whether the pseudoport synchronizes to the clock or
         operates faster than real time.

         @values #AFE_PSEUDOPORT_TIMING_MODE_REALTIME */

    uint32_t                  sample_rate;
    /**< Sampling rate of the pseudoport.

         @values
         - #AFE_PORT_SAMPLE_RATE_8K
         - #AFE_PORT_SAMPLE_RATE_16K
         - #AFE_PORT_SAMPLE_RATE_22_05K
         - #AFE_PORT_SAMPLE_RATE_24K
         - #AFE_PORT_SAMPLE_RATE_32K
         - #AFE_PORT_SAMPLE_RATE_44_1K
         - #AFE_PORT_SAMPLE_RATE_48K
         - #AFE_PORT_SAMPLE_RATE_88_2K
         - #AFE_PORT_SAMPLE_RATE_96K
         - #AFE_PORT_SAMPLE_RATE_176_4K
         - #AFE_PORT_SAMPLE_RATE_192K
         - #AFE_PORT_SAMPLE_RATE_384K
         - #AFE_PORT_SAMPLE_RATE_352_8K @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_param_id_pseudo_port_cfg_t */
/** @} */ /* end_addtogroup afe_module_audio_device */

/** @addtogroup afe_mod_param_id_deprecated
@{ */
/** Deprecated.

    ID of the parameter used by #AFE_MODULE_AUDIO_DEV_INTERFACE to
    configure the AHB clock for register access to specific hardware.

    #AFE_PORT_CMD_SET_PARAM_V2 (deprecated) and #AFE_PORT_CMD_SET_PARAM_V3 can
    use this parameter ID.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_AHB_CLOCK_CONFIG @vertspace{-3}

    @apr_msgpayload{afe_param_id_ahb_clock_cfg_t}
    @table{weak__afe__param__id__ahb__clock__cfg__t}
 */
#define AFE_PARAM_ID_AHB_CLOCK_CONFIG                           0x00010249

/** Deprecated.

    Version information used to handle future additions to
    AFE_PARAM_ID_AHB_CLOCK_CONFIG processing (for backward compatibility).
 */
#define AFE_API_VERSION_AHB_CLOCK_CONFIG                         0x1

/** Deprecated.

    Enumeration for disabling the AHB clock for register access. */
#define AFE_AHB_CLK_DISABLE                                      0x0

/** Deprecated.

    Enumeration for enabling the AHB clock for register access. @newpage */
#define AFE_AHB_CLK_ENABLE                                       0x1

typedef struct afe_param_id_ahb_clock_cfg_t afe_param_id_ahb_clock_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_ahb_clock_cfg_t
@{ */
/* Payload of the AFE_PARAM_ID_AHB_CLOCK_CONFIG parameter used by
    AFE_MODULE_AUDIO_DEV_INTERFACE.
 */
struct afe_param_id_ahb_clock_cfg_t
{
    uint32_t                  ahb_clock_cfg_minor_version;
    /**< Tracks the configuration of this parameter.

         @values #AFE_API_VERSION_AHB_CLOCK_CONFIG */

    uint32_t                  ahb_clk_enable;
    /**< Specifies whether the AHB clock is enabled for register access.

         @values
         - #AFE_AHB_CLK_DISABLE
         - #AFE_AHB_CLK_ENABLE @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_param_id_ahb_clock_cfg_t */
/** @} */ /* end_addtogroup afe_mod_param_id_deprecated */

/** @addtogroup afe_module_audio_device
@{ */
/** ID of the parameter used by #AFE_MODULE_AUDIO_DEV_INTERFACE to
    configure the LPASS core shared clock for register access to the LPASS
    core.

    Only #AFE_PORT_CMD_SET_PARAM_V2 (deprecated) and #AFE_PORT_CMD_SET_PARAM_V3
    can use this parameter ID.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_LPASS_CORE_SHARED_CLOCK_CONFIG

    @apr_msgpayload{afe_param_id_lpass_core_shared_clk_cfg_t}
    @table{weak__afe__param__id__lpass__core__shared__clk__cfg__t}
 */
#define AFE_PARAM_ID_LPASS_CORE_SHARED_CLOCK_CONFIG                           0x0001028C

/** Version information used to handle future additions to LPASS core
    shared clock configuration (for backward compatibility). @newpage
 */
#define AFE_API_VERSION_LPASS_CORE_SHARED_CLK_CONFIG                         0x1

typedef struct afe_param_id_lpass_core_shared_clk_cfg_t afe_param_id_lpass_core_shared_clk_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_lpass_core_shared_clk_cfg_t
@{ */
/* Payload of the AFE_PARAM_ID_LPASS_CORE_SHARED_CLOCK_CONFIG parameter used by
    AFE_MODULE_AUDIO_DEV_INTERFACE.
 */
struct afe_param_id_lpass_core_shared_clk_cfg_t
{
   uint32_t                  lpass_core_shared_clk_cfg_minor_version;
   /**< Tracks the configuration of this parameter.

        @values #AFE_API_VERSION_LPASS_CORE_SHARED_CLK_CONFIG */

   uint32_t                  enable;
   /**< Specifies whether the LPASS core shared clock is enabled.

        @values
         - 0 -- Disabled
         - 1 -- Enabled @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_param_id_lpass_core_shared_clk_cfg_t */

/** ID of the parameter used by #AFE_MODULE_AUDIO_DEV_INTERFACE to provide
    the device hardware delay.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_DEVICE_HW_DELAY

    @apr_msgpayload{afe_param_id_device_hw_delay_t}
    @table{weak__afe__param__id__device__hw__delay__t}
 */
#define AFE_PARAM_ID_DEVICE_HW_DELAY                             0x00010243

/** Version information used to handle future additions to
    AFE_PARAM_ID_DEVICE_HW_DELAY processing (for backward compatibility).
    @newpage
 */
#define AFE_API_VERSION_DEVICE_HW_DELAY                          0x1

typedef struct afe_param_id_device_hw_delay_t afe_param_id_device_hw_delay_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_device_hw_delay_t
@{ */
/* Payload of the AFE_PARAM_ID_DEVICE_HW_DELAY parameter used by
    AFE_MODULE_AUDIO_DEV_INTERFACE.
 */
struct afe_param_id_device_hw_delay_t
{
    uint32_t                  device_hw_delay_minor_version;
    /**< Tracks the configuration of this parameter.

         @values #AFE_API_VERSION_DEVICE_HW_DELAY */

    uint32_t                  delay_in_us;
    /**< Device hardware delay in microseconds.

         @values Valid uint32 number */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_param_id_device_hw_delay_t */
/** @} */ /* end_addtogroup afe_module_audio_device */


/** @addtogroup afe_cmd_device_start
@{ */
/** Starts the AFE interface specified in the port_id field.

  @apr_hdr_fields
    Opcode -- AFE_PORT_CMD_DEVICE_START

  @apr_msgpayload{afe_port_cmd_device_start_t}
    @table{weak__afe__port__cmd__device__start__t}

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    The port must be configured before this command is sent. Otherwise, an
    error is returned.
 */
#define AFE_PORT_CMD_DEVICE_START                                      0x000100E5

/* Structure for the AFE_PORT_CMD_DEVICE_START command. */
typedef struct afe_port_cmd_device_start_t afe_port_cmd_device_start_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_port_cmd_device_start_t
@{ */
/* Payload of the AFE_PORT_CMD_DEVICE_START command.
 */
struct afe_port_cmd_device_start_t
{
    uint16_t                  port_id;
    /**< ID of the port interface and direction (Rx or Tx).

        @values See <i>Supported Hardware Ports</i> in Appendix
        @xref{chp:portIDs}.
         - An even number represents the Rx direction.
         - An odd number represents the Tx direction @tablebulletend */

    uint16_t                  reserved;
    /**< This field must be set to 0. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_port_cmd_device_start_t */
/** @} */ /* end_addtogroup afe_cmd_device_start */


/** @addtogroup afe_cmd_devices_restart
@{ */
/** Restarts the AFE interface specified in the port_id field.

  @apr_hdr_fields
    Opcode -- AFE_PORT_CMD_DEVICES_RESTART

  @apr_msgpayload{afe_port_cmd_devices_restart_t}
    @table{weak__afe__port__cmd__devices__restart__t}

  @detdesc
    The sampling rate can be changed only for the 48 kHz family group (8 kHz,
    16 kHz, 48 kHz). The rate cannot be changed for the root clock of the I2S
    interface.
    @par
    The over-sampling rate (OSR), or master clock, is not changed during
    device restart. For example, if the OSR is 12288000, it stays as
    12288000.
    If the OSR is to change, it must send the clock configuration, and
    then stop and restart the command.
    @par
    The number of channels for this command are supported only on the mono
    channel.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).
    @newpage

  @dependencies
    The I2S port must be started before this command is sent.
    Otherwise, an error is returned.
    @par
    The root clocks must be in Master mode. If they are in Slave mode, do not
    use this restart command.
    @par
    If the Rx and Tx ports use different interfaces (such as the primary
    Rx port and secondary Tx port), do not use the primary Tx Port and
    secondary Rx port.
 */
#define AFE_PORT_CMD_DEVICES_RESTART                                      0x000100F7

/* Structure for the AFE_PORT_CMD_DEVICE_START command. */
typedef struct afe_port_cmd_devices_restart_t afe_port_cmd_devices_restart_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_port_cmd_devices_restart_t
@{ */
/* Payload of the AFE_PORT_CMD_DEVICES_RESTART command.
 */
struct afe_port_cmd_devices_restart_t
{
    uint16_t                  rx_port_id;
    /**< ID of the I2S Rx port interface.

         @values
         - See <i>Supported Hardware Ports</i> in Appendix @xref{chp:portIDs}
         - #AFE_PORT_ID_INVALID -- If an Rx port is not used

         If only one Rx port is to be restarted, specify the Tx port parameter
         as invalid. */

    uint16_t                  tx_port_id;
    /**<I2S Tx port interface to restart.

         @values
         - See <i>Supported Hardware Ports</i> in Appendix @xref{chp:portIDs}

         - If a Tx port is not used -- #AFE_PORT_ID_INVALID @tablebulletend */

    uint32_t                  sample_rate;
    /**< Sampling rate of the port.

         @values
         - #AFE_PORT_SAMPLE_RATE_8K
         - #AFE_PORT_SAMPLE_RATE_16K
         - #AFE_PORT_SAMPLE_RATE_48K @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_port_cmd_devices_restart_t */
/** @} */ /* end_addtogroup afe_cmd_devices_restart */


/** @addtogroup afe_cmd_device_stop
@{ */
/** Stops the AFE interface specified in the port_id field.

  @apr_hdr_fields
    Opcode -- AFE_PORT_CMD_DEVICE_STOP

  @apr_msgpayload{afe_port_cmd_device_stop_t}
    @table{weak__afe__port__cmd__device__stop__t}

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    None.
 */
#define AFE_PORT_CMD_DEVICE_STOP                                      0x000100E6

/* Payload structure of the AFE_PORT_CMD_DEVICE_STOP command. */
typedef struct afe_port_cmd_device_stop_t afe_port_cmd_device_stop_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_port_cmd_device_stop_t
@{ */
/* Payload of the AFE_PORT_CMD_DEVICE_STOP command.
 */
struct afe_port_cmd_device_stop_t
{
    uint16_t                  port_id;
    /**< ID of the port interface and direction (Rx or Tx).

        @values See <i>Supported Hardware Ports</i> in Appendix
        @xref{chp:portIDs}.
         - An even number represents the Rx direction.
         - An odd number represents the Tx direction @tablebulletend */

    uint16_t                  reserved;
    /**< This field must be set to 0. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_port_cmd_device_stop_t */
/** @} */ /* end_addtogroup afe_cmd_device_stop */


/** @addtogroup afe_cmd_mem_map
@{ */
/** Commands the AFE service to map multiple shared memory regions.
    All regions to be mapped must be from the same memory pool.

  @apr_hdr_fields
    Opcode -- AFE_SERVICE_CMD_SHARED_MEM_MAP_REGIONS

  @apr_msgpayload{afe_service_cmd_shared_mem_map_regions_t}
    @tablens{weak__afe__service__cmd__shared__mem__map__regions__t}

  @mappayload{afe_service_shared_map_region_payload_t}
    @table{weak__afe__service__shared__map__region__payload__t}

  @return
    #AFE_SERVICE_CMDRSP_SHARED_MEM_MAP_REGIONS

  @dependencies
    None.
 */
#define AFE_SERVICE_CMD_SHARED_MEM_MAP_REGIONS                       0x000100EA

/** Enumeration for the shared memory, 8 byte addressable, 4 KB aligned
    pool ID. */
#define ADSP_MEMORY_MAP_SHMEM8_4K_POOL      3

/* AFE_SERVICE shared memory map regions structure */
typedef struct  afe_service_cmd_shared_mem_map_regions_t afe_service_cmd_shared_mem_map_regions_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_service_cmd_shared_mem_map_regions_t
@{ */
/* Memory map regions command payload used by the
    AFE_SERVICE_CMD_SHARED_MEM_MAP_REGIONS command that allows clients to map
    multiple shared memory regions in a single command. */
/** Immediately following this structure are num_regions of the
    <b>Map payload</b> (%afe_service_shared_map_region_payload_t).
 */
struct afe_service_cmd_shared_mem_map_regions_t
{
    uint16_t                  mem_pool_id;
    /**< Type of memory on which this memory region is mapped.

         @values
         - #ADSP_MEMORY_MAP_SHMEM8_4K_POOL
         - Other values are reserved

         The memory pool ID implicitly defines the characteristics of the memory.
         Characteristics can include alignment type, permissions, and so on.

         ADSP_MEMORY_MAP_SHMEM8_4K_POOL is shared memory, 8 byte addressable,
         and 4 KB aligned. */

    uint16_t                  num_regions;
    /**< Number of regions to map.

         @values Any value greater than zero */

    uint32_t                  property_flag;
    /**< Configures one common property for all regions in the payload.

         @values 0x00000000 to 0x00000001

         Bit 0 indicates physical or virtual mapping:
         - 0 -- The shared memory address provided in
           %afe_service_shared_map_region_payload_t is a physical address.
           The shared memory must be mapped (hardware TLB entry), and a
           software entry must be added for internal bookkeeping.
         - 1 -- The shared memory address provided in
           %afe_service_shared_map_region_payload_t is a virtual address.
           The shared memory must not be mapped (because the hardware TLB entry
           is already available), but a software entry must be added for
           internal bookkeeping. \n
           A virtual address is useful if two services within the aDSP are
           communicating through the APR. The services can directly communicate
           through the virtual address instead of the physical address. The
           virtual regions must be contiguous. In this case, num_regions must
           be set to 1.

         Bits 31 to 1 are reserved and must be set to 0. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_service_cmd_shared_mem_map_regions_t */

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_service_shared_map_region_payload_t
@{ */
/* Payload of the map region; used by the
    afe_service_cmd_shared_mem_map_regions_t structure.
 */
struct afe_service_shared_map_region_payload_t
{
    uint32_t                  shm_addr_lsw;
    /**< Lower 32 bits of the shared memory address of the memory region to
         map. */

    uint32_t                  shm_addr_msw;
    /**< Upper 32 bits of the shared memory address of the memory region to
         map.

         The 64-bit number formed by shm_addr_lsw and shm_addr_msw word must be
         contiguous memory, and it must be 4 KB aligned.

         @values
         - For a 32-bit shared memory address, this field must be set to 0.
         - For a 36-bit shared memory address, bits 31 to 4 must be set to 0.
         - For a 64-bit shared memory address, any 32-bit value. @tablebulletend */

    uint32_t                  mem_size_bytes;
    /**< Number of bytes in the region.

         @values Multiples of 4 KB

         The aDSP always maps the regions as virtual contiguous memory, but
         the memory size must be in multiples of 4 KB to avoid gaps in the
         virtual contiguous mapped memory. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_service_shared_map_region_payload_t */
/** @} */ /* end_addtogroup afe_cmd_mem_map */


/** @addtogroup afe_resp_mem_map
@{ */
/** Response sent by the AFE service to the client after
    #AFE_SERVICE_CMD_SHARED_MEM_MAP_REGIONS is processed successfully.

  @apr_hdr_fields
    Opcode -- AFE_SERVICE_CMDRSP_SHARED_MEM_MAP_REGIONS \n

  @apr_msgpayload{afe_service_cmdrsp_shared_mem_map_regions_t}
    @table{weak__afe__service__cmdrsp__shared__mem__map__regions__t}

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    To receive this response from the AFE service, a corresponding
    #AFE_SERVICE_CMD_SHARED_MEM_MAP_REGIONS command must be sent.
 */
#define AFE_SERVICE_CMDRSP_SHARED_MEM_MAP_REGIONS                   0x000100EB

/* AFE_SERVICE shared memory map regions response structure */
typedef struct  afe_service_cmdrsp_shared_mem_map_regions_t afe_service_cmdrsp_shared_mem_map_regions_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_service_cmdrsp_shared_mem_map_regions_t
@{ */
/* Payload of the memory map command response used by the
    AFE_SERVICE_CMDRSP_SHARED_MEM_MAP_REGIONS command.
 */
struct afe_service_cmdrsp_shared_mem_map_regions_t
{
    uint32_t                  mem_map_handle;
    /**< Unique identifier for a shared memory address.

         @values Any 32-bit value

         The aDSP returns the memory map handle through
         #AFE_SERVICE_CMD_SHARED_MEM_MAP_REGIONS. The handle is used as an
         abstract representation of the shared memory regions that are being
         mapped.

         The aDSP uses this handle to retrieve the shared memory attributes.
         AFE_SERVICE_CMD_SHARED_MEM_UNMAP_REGIONS uses this handle
         to unmap the mapped memory. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_service_cmdrsp_shared_mem_map_regions_t */
/** @} */ /* end_addtogroup afe_resp_mem_map */


/** @addtogroup afe_cmd_mem_unmap
@{ */
/** Commands the AFE service to unmap multiple shared memory regions that were
    previously mapped.

  @apr_hdr_fields
    Opcode -- AFE_SERVICE_CMD_SHARED_MEM_UNMAP_REGIONS \n

  @apr_msgpayload{afe_service_cmd_shared_mem_unmap_regions_t}
    @table{weak__afe__service__cmd__shared__mem__unmap__regions__t}

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    A corresponding #AFE_SERVICE_CMD_SHARED_MEM_MAP_REGIONS command must be
    processed successfully.
 */
#define AFE_SERVICE_CMD_SHARED_MEM_UNMAP_REGIONS                      0x000100EC

/* AFE_SERVICE shared memory unmap regions command structure */
typedef struct afe_service_cmd_shared_mem_unmap_regions_t afe_service_cmd_shared_mem_unmap_regions_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_service_cmd_shared_mem_unmap_regions_t
@{ */
/* Payload of the memory map command response used by
    AFE_SERVICE_CMD_SHARED_MEM_UNMAP_REGIONS, which allows clients to unmap
    multiple shared memory regions in a single command.
 */
struct afe_service_cmd_shared_mem_unmap_regions_t
{
    uint32_t                  mem_map_handle;
    /**< Unique identifier for a shared memory address.

         @values Any 32-bit value

         The aDSP returns the memory map handle through
         #AFE_SERVICE_CMD_SHARED_MEM_MAP_REGIONS.

         The aDSP uses this handle to retrieve the associated shared
         memory regions and unmap them. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_service_cmd_shared_mem_unmap_regions_t */
/** @} */ /* end_addtogroup afe_cmd_mem_unmap */


/** @addtogroup afe_service_api_deprecated
@{ */
/** Deprecated. Use #AFE_PORT_CMD_SET_PARAM_V3.

  Configures one or more parameters on an AFE port. This command is
   typically used to apply calibration data to the modules.

  @apr_hdr_fields
    Opcode -- AFE_PORT_CMD_SET_PARAM_V2

  @apr_msgpayload{afe_port_cmd_set_param_v2_t}
    @tablens{weak__afe__port__cmd__set__param__v2__t}

  @par Parameter data variable payload (afe_port_param_data_v2_t)
    @tablens{weak__afe__port__param__data__v2__t}

  @detdesc
    This command is sent from the client to the aDSP to set the parameters of
    a port. A single command can be used to set multiple parameters.
    @par
    Messages can be either in-band or out-of-band via shared memory. The modes
    are:
    - In-band set parameter -- In this mode, the physical address is set to
      NULL. The rest of the payload contains the parameters as shown in the
      parameter data variable payload (%afe_port_param_data_v2_t).
    - Shared memory set parameter -- In this mode, both the physical address to
      the parameter data and the data payload size are sent. The rest of the
      message is in shared memory in the parameter data variable payload. The
      memory content is the same as the in-band payload
      structure (%afe_port_param_data_v2_t).

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    None. @newpage
 */
#define AFE_PORT_CMD_SET_PARAM_V2                                      0x000100EF

/* Structure for the AFE parameter data. */
typedef struct afe_port_param_data_v2_t afe_port_param_data_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_port_param_data_v2_t
@{ */
/* Payload of the AFE parameter data used by
    AFE_PORT_CMD_SET_PARAM_V2, AFE_PORT_CMD_GET_PARAM_V2, and
    AFE_PORT_CMDRSP_GET_PARAM_V2. */
/** Immediately following this structure are param_size bytes of calibration
    data. The structure and size depend on the module_id/param_id combination.
 */
struct afe_port_param_data_v2_t
{
   uint32_t module_id;
   /**< Valid ID of the module.

        @values See Chapter @xref{hdr:CalibrationIDs} */

   uint32_t param_id;
    /**< Valid ID of the parameter.

         @values See Chapter @xref{hdr:CalibrationIDs} */

   uint16_t param_size;
   /**< Size of the parameter data based on the module_id/param_id
        combination.

        @values > 0, in multiples of 4 bytes */

   uint16_t reserved;
   /**< This field must be set to 0. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_port_param_data_v2_t */

/* Structure for configuration/calibration settings for the AFE port. */
typedef struct afe_port_cmd_set_param_v2_t afe_port_cmd_set_param_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_port_cmd_set_param_v2_t
@{ */
/* Payload of the AFE_PORT_CMD_SET_PARAM_V2 command's
    configuration/calibration settings for the AFE port. */
/** For in-band data, immediately following this structure is the
    <b>Parameter data variable payload</b> (%afe_port_param_data_v2_t)
 */
struct afe_port_cmd_set_param_v2_t
{
   uint16_t port_id;
   /**< ID of the port interface and direction (Rx or Tx).

        @values See <i>Supported Hardware Ports</i> in Appendix
        @xref{chp:portIDs}.
         - An even number represents the Rx direction.
         - An odd number represents the Tx direction @tablebulletend */

   uint16_t payload_size;
    /**< Actual size of the variable payload accompanying the message or in
         shared memory. This field is used for parsing both in-band and
         out-of-band data.

         @values > 0 bytes, in multiples of 4 bytes */

   uint32_t payload_address_lsw;
   /**< Lower 32 bits of the payload address. */

   uint32_t payload_address_msw;
   /**< Upper 32 bits of the payload address.

         The 64-bit number formed by payload_address_lsw and
         payload_address_msw must be aligned to a 32-byte boundary and be in
         contiguous memory.

         @values
         - For a 32-bit shared memory address, this field must be set to 0.
         - For a 36-bit shared memory address, bits 31 to 4 of this field must
           be set to 0. @tablebulletend */

   uint32_t mem_map_handle;
   /**< Unique identifier for a shared memory address.

        @values
        - NULL -- The message is in the payload (in-band).
        - Non-NULL -- The parameter data payload begins at the address
          specified by a pointer to the physical address of the payload in
          shared memory (out-of-band).

        @contcell
        The aDSP returns this memory map handle through
        #AFE_SERVICE_CMD_SHARED_MEM_MAP_REGIONS.

        An optional field is available if parameter data is in-band:
        %afe_port_param_data_v2_t param_data[...].
        See <b>Parameter data variable payload</b>. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_port_cmd_set_param_v2_t */
/** @} */ /* end_addtogroup afe_service_api_deprecated */


/** @addtogroup afe_cmd_port_set_paramsv3
@{ */
/** Configures one or more parameters on an AFE port.

    This command supports module instances. It can be called on different
    instances of the same module.

  @apr_hdr_fields
    Opcode -- AFE_PORT_CMD_SET_PARAM_V3

  @apr_msgpayload{afe_port_cmd_set_param_v3_t}
    @tablens{weak__afe__port__cmd__set__param__v3__t}

  @par Parameter data variable payload (afe_port_param_data_v3_t)
    @tablens{weak__afe__port__param__data__v3__t}

  @detdesc
    The instance ID is applicable only for those module IDs that support
    multiple instances. For other modules, the instance_id field must be set
    to 0.
    @par
    This command is used to apply calibration data to the modules.
    The client sends the command to the aDSP to set the parameters of a port.
    A single command can be used to set multiple parameters.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    None.
*/
#define AFE_PORT_CMD_SET_PARAM_V3                                      0x000100FA

/* Structure for configuration/calibration settings for the AFE port. */
typedef struct afe_port_cmd_set_param_v3_t afe_port_cmd_set_param_v3_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_port_cmd_set_param_v3_t
@{ */
/* Payload of the AFE_PORT_CMD_SET_PARAM_V3 command's
    configuration/calibration settings for the AFE port. */
/** For in-band data, immediately following this structure is the
    <b>Parameter data variable payload</b> (%afe_port_param_data_v3_t).
*/
struct afe_port_cmd_set_param_v3_t
{
   uint16_t port_id;
   /**< ID of the port interface and direction (Rx or Tx).

        @values See <i>Supported Hardware Ports</i> in Appendix
        @xref{chp:portIDs}.
        - An even number represents the Rx direction.
        - An odd number represents the Tx direction. @tablebulletend */

   uint16_t reserved;
   /**< This field must be set to 0. */

   uint32_t payload_address_lsw;
   /**< Lower 32 bits of the payload address. */

   uint32_t payload_address_msw;
    /**< Upper 32 bits of the payload address.

         The 64-bit number formed by payload_address_lsw and
         payload_address_msw must be aligned to a 32-byte boundary and be in
         contiguous memory.

         @values
         - For a 32-bit shared memory address, this field must be set to 0.
         - For a 36-bit shared memory address, bits 31 to 4 of this field must
           be set to 0. @tablebulletend */

   uint32_t mem_map_handle;
   /**< Unique identifier for the shared memory address.

        The aDSP returns this handle through
        #AFE_SERVICE_CMD_SHARED_MEM_MAP_REGIONS.

        @values
         - NULL -- In-band
            - The message is in the payload.
            - A series of %afe_port_param_data_v3_t structures immediately
              follow this structure. The total size is payload_size bytes.
              @vertspace{2}
         - Non-NULL -- Out-of-band
            - The payload begins at the specified physical address.
            - The total size is payload_size bytes. @tablebulletend */

   uint32_t payload_size;
    /**< Actual size of the variable payload accompanying the message or in
         shared memory. This field is used for parsing both in-band and
         out-of-band data.

         @values > 0 bytes, in multiples of 4 bytes */
        /* An optional field is available if parameter data is in-band:
        %afe_port_param_data_v3_t param_data[...]. */

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_port_cmd_set_param_v3_t */

/* Structure for the AFE parameter data. */
typedef struct afe_port_param_data_v3_t afe_port_param_data_v3_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_port_param_data_v3_t
@{ */
/* Payload of the AFE parameter data used by
    AFE_PORT_CMD_SET_PARAM_V3, AFE_PORT_CMD_GET_PARAM_V3, and
    AFE_PORT_CMDRSP_GET_PARAM_V3. */
/** Immediately following this structure are param_size bytes of calibration
    data. The structure and size depend on the module_id/param_id combination.
*/
struct afe_port_param_data_v3_t
{
   uint32_t module_id;
   /**< Valid ID of the module.

        @values See Chapter @xref{hdr:CalibrationIDs} */

   uint16_t instance_id;
   /**< Valid ID of the module instance. For more information, see <b>Detailed
        description</b>. */

   uint16_t reserved;
   /**< This field must be set to 0. */

   uint32_t param_id;
   /**< Valid ID of the parameter.

        @values See Chapter @xref{hdr:CalibrationIDs} */

   uint32_t param_size;
   /**< Size of the parameter data based on the module_id/param_id
        combination.

        @values > 0 bytes, in multiples of 4 bytes */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_port_param_data_v3_t */
/** @} */ /* end_addtogroup afe_cmd_port_set_paramsv3 */


/** @addtogroup afe_service_api_deprecated
@{ */
/** Deprecated. Use #AFE_PORT_CMD_GET_PARAM_V3.

  Queries for the parameters of an AFE port.

  @apr_hdr_fields
    Opcode -- AFE_PORT_CMD_GET_PARAM_V2

  @apr_msgpayload{afe_port_cmd_get_param_v2_t}
    @tablens{weak__afe__port__cmd__get__param__v2__t}

  @detdesc
    This command is sent from the client to the aDSP to query the parameters of
    the requested port. If the payload_address is NULL (in-band), the aDSP
    sends the data in the APR payload.
    If the payload_address is non-NULL (out-of-band), data is stored at the
    address specified in the payload_address variable.
    @par
    The client receives a response from the aDSP with the values of the
    requested parameters in AFE_PORT_CMDRSP_GET_PARAM_V2.

  @return
    An AFE_PORT_CMDRSP_GET_PARAM_V2 message.

  @dependencies
    None. @newpage
 */
#define  AFE_PORT_CMD_GET_PARAM_V2                                    0x000100F0

/* Payload structure of the AFE_PORT_CMD_GET_PARAM_V2 command. */
typedef struct afe_port_cmd_get_param_v2_t afe_port_cmd_get_param_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_port_cmd_get_param_v2_t
@{ */
/* Payload of the AFE_PORT_CMD_GET_PARAM_V2 command, which queries for one
    post/preprocessing parameter of a stream. */
/** Immediately following this structure are param_size bytes of calibration data.
    The structure and size depend on the module_id/param_id combination.
 */
struct afe_port_cmd_get_param_v2_t
{
    uint16_t                  port_id;
    /**< ID of the port interface and direction (Rx or Tx).

        @values See <i>Supported Hardware Ports</i> in Appendix
        @xref{chp:portIDs}.
         - An even number represents the Rx direction.
         - An odd number represents the Tx direction @tablebulletend */

    uint16_t                  payload_size;
    /**< Actual size of the variable payload accompanying the message or in
         shared memory. This field is used for parsing both in-band and
         out-of-band data.

         @values > 0 bytes, in multiples of 4 bytes */

    uint32_t payload_address_lsw;
    /**< Lower 32 bits of the payload address. */

    uint32_t payload_address_msw;
    /**< Upper 32 bits of the payload address.

         The 64-bit number formed by payload_address_lsw and
         payload_address_msw must be aligned to a 32-byte boundary and be in
         contiguous memory.

         @values
         - For a 32-bit shared memory address, this field must be set to 0.
         - For a 36-bit shared memory address, bits 31 to 4 of this field must
           be set to 0. @tablebulletend */

    uint32_t mem_map_handle;
    /**< Unique identifier for a shared memory address.

         @values
         - NULL -- The message is in the payload (in-band).
         - Non-NULL -- The parameter data payload begins at the address
           specified by a pointer to the physical address of the payload in
           shared memory (out-of-band).

         The aDSP returns this memory map handle through
         #AFE_SERVICE_CMD_SHARED_MEM_MAP_REGIONS.

         An optional field is available if parameter data is in-band:
         %afe_port_param_data_v2_t param_data[...].
         See <b>Parameter data variable payload</b>. */

    uint32_t                  module_id;
    /**< Valid ID of the module.

         @values See Chapter @xref{hdr:CalibrationIDs} */

    uint32_t                  param_id;
    /**< Valid ID of the parameter.

         @values See Chapter @xref{hdr:CalibrationIDs} */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_port_cmd_get_param_v2_t */

/** Deprecated. Use #AFE_PORT_CMDRSP_GET_PARAM_V3.

    Returns parameter values in response to an AFE_PORT_CMD_GET_PARAM_V2
    command.

    This event supports shared memory if an address is sent with
    AFE_PORT_CMD_GET_PARAM_V2.

  @apr_hdr_fields
    Opcode -- AFE_PORT_CMDRSP_GET_PARAM_V2

  @apr_msgpayload{afe_port_cmdrsp_get_param_v2_t}
    @tablens{weak__afe__port__cmdrsp__get__param__v2__t}

  @par Parameter data variable payload (afe_port_param_data_v2_t)
    @tablens{weak__afe__port__param__data__v2__t}

  @detdesc
    The aDSP sends this command to the client with the parameter values
    requested in response to AFE_PORT_CMD_GET_PARAM_V2.
    @par
    For in-band parameters, the acknowledgment response contains the status,
    followed by the parameter data variable payload. @newpage
    @par
    For out-of-band parameters, the response contains only the status, and the
    requested parameters are stored in the address specified in the
    AFE_PORT_CMD_GET_PARAM_V2. Data is stored in the parameter data variable
    payload of the acknowledgment.
    @par
    The choice of in-band or out-of-band depends on the values of
    payload_address present in %AFE_PORT_CMD_GET_PARAM_V2.

  @return
    None.

  @dependencies
    AFE_PORT_CMD_GET_PARAM_V2 must be sent before this response is sent. @newpage
 */
#define AFE_PORT_CMDRSP_GET_PARAM_V2                                   0x00010106

/* Payload structure of the AFE_PORT_CMDRSP_GET_PARAM_V2 event. */
typedef struct afe_port_cmdrsp_get_param_v2_t afe_port_cmdrsp_get_param_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_port_cmdrsp_get_param_v2_t
@{ */
/* Payload of the AFE_PORT_CMDRSP_GET_PARAM_V2 message,
    which responds to an AFE_PORT_CMD_GET_PARAM_V2 command. */
/** Immediately following this structure is the acknowledgment
    <b>Parameter data variable payload</b> (%afe_port_param_data_v2_t).
    This payload is included for an in-band scenario. For an address/shared
    memory-based set parameter, this payload is not required.
 */
struct afe_port_cmdrsp_get_param_v2_t
{
    uint32_t                  status;
    /**< Status message or error code.

         @values (Refer to @xrefcond{Q3,80-N1463-2,80-NA601-5})
          - APR_EOK -- Success
          - APR_EBADPARAM -- Failure @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_port_cmdrsp_get_param_v2_t */
/** @} */ /* end_addtogroup afe_service_api_deprecated */


/** @addtogroup afe_cmd_port_get_paramsv3
@{ */
/** Queries for the parameters of an AFE port.

    This command supports module instances. It can be called on different
    instances of the same module.

  @apr_hdr_fields
    Opcode -- AFE_PORT_CMD_GET_PARAM_V3

  @apr_msgpayload{afe_port_cmd_get_param_v3_t}
    @tablens{weak__afe__port__cmd__get__param__v3__t}

  @detdesc
    The instance ID is applicable only for those module IDs that support
    multiple instances. For other modules, the instance_id field is 0.
    @par
    The client sends this command to the aDSP to query the parameters of
    the requested port. The client receives a response
    (#AFE_PORT_CMDRSP_GET_PARAM_V3) with the values of the requested
    parameters.

  @return
    #AFE_PORT_CMDRSP_GET_PARAM_V3

  @dependencies
    None.
*/
#define  AFE_PORT_CMD_GET_PARAM_V3                                    0x000100FB

/* Payload structure of the AFE_PORT_CMD_GET_PARAM_V3 command. */
typedef struct afe_port_cmd_get_param_v3_t afe_port_cmd_get_param_v3_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_port_cmd_get_param_v3_t
@{ */
/* Payload of the AFE_PORT_CMD_GET_PARAM_V3 command, which queries for one
   parameter of a port. */
/** Immediately following this structure are param_size bytes of calibration data.
    The structure and size depend on the module_id/param_id combination.
*/
struct afe_port_cmd_get_param_v3_t
{
   uint16_t                  port_id;
   /**< ID of the port interface and direction (Rx or Tx).

        @values See <i>Supported Hardware Ports</i> in Appendix
        @xref{chp:portIDs}.
        - An even number represents the Rx direction.
        - An odd number represents the Tx direction. @tablebulletend */

    uint16_t                  reserved;
    /**< This field must be set to 0. */

    uint32_t payload_address_lsw;
    /**< Lower 32 bits of the payload address. */

    uint32_t payload_address_msw;
    /**< Upper 32 bits of the payload address.

         The 64-bit number formed by payload_address_lsw and
         payload_address_msw must be aligned to a 32-byte boundary and be in
         contiguous memory.

         @values
         - For a 32-bit shared memory address, this field must be set to 0.
         - For a 36-bit shared memory address, bits 31 to 4 of this field must
           be set to 0. @tablebulletend */

    uint32_t mem_map_handle;
    /**< Unique identifier for the shared memory address.

        The aDSP returns this handle through
        #AFE_SERVICE_CMD_SHARED_MEM_MAP_REGIONS.

        @values
         - NULL -- In-band
            - The message is in the payload.
            - A series of %afe_port_param_data_v3_t structures immediately
              follow this structure. The total size is payload_size bytes.
              @vertspace{2}
         - Non-NULL -- Out-of-band
            - The payload begins at the specified physical address.
            - The total size is payload_size bytes. @tablebulletend */

    uint32_t                  module_id;
    /**< Valid ID of the module.

         @values See Chapter @xref{hdr:CalibrationIDs} */

    uint16_t                  instance_id;
    /**< Valid ID of the module instance. For more information, see <b>Detailed
        description</b>. */

    uint16_t                  reserved_2;
    /**< This field must be set to 0. */

    uint32_t                  param_id;
    /**< Valid ID of the parameter.

         @values See Chapter @xref{hdr:CalibrationIDs} */

    uint32_t                  payload_size;
    /**< Actual size of the variable payload accompanying the message or in
         shared memory. This field is used for parsing both in-band and
         out-of-band data.

         @values > 0 bytes, in multiples of 4 bytes */

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_port_cmd_get_param_v3_t */
/** @} */ /* end_addtogroup afe_cmd_port_get_paramsv3 */


/** @addtogroup afe_rsp_port_get_paramsv3
@{ */
/** Returns parameter values in response to an #AFE_PORT_CMD_GET_PARAM_V3
    command.

  @apr_hdr_fields
    Opcode -- AFE_PORT_CMDRSP_GET_PARAM_V3

  @apr_msgpayload{afe_port_cmdrsp_get_param_v3_t}
    @tablens{weak__afe__port__cmdrsp__get__param__v3__t}

  @par Parameter data variable payload (afe_port_param_data_v3_t)
    @tablens{weak__afe__port__param__data__v3__t}

  @detdesc
    The instance ID is applicable only for those module IDs that support
    multiple instances. For other modules, the instance_id field is 0.
    @par
    The aDSP sends this command to the client. If an address is sent with
    #AFE_PORT_CMD_GET_PARAM_V3, the aDSP supports shared memory.

    @par
    The choice of in-band or out-of-band depends on the values of
    payload_address present in AFE_PORT_CMD_GET_PARAM_V3.

  @return
    None.

  @dependencies
    #AFE_PORT_CMD_GET_PARAM_V3 must be sent before this response is sent.
*/
#define AFE_PORT_CMDRSP_GET_PARAM_V3                                  0x00010108

/* Payload structure of the AFE_PORT_CMDRSP_GET_PARAM_V3 event. */
typedef struct afe_port_cmdrsp_get_param_v3_t afe_port_cmdrsp_get_param_v3_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_port_cmdrsp_get_param_v3_t
@{ */
/* Payload of the AFE_PORT_CMDRSP_GET_PARAM_V3 message,
    which responds to an #AFE_PORT_CMD_GET_PARAM_V3 command. */
/** For in-band data, immediately following this structure is the
    <b>Parameter data variable payload</b> (%afe_port_param_data_v3_t).
    The payload depends on the size of the parameter.
*/
struct afe_port_cmdrsp_get_param_v3_t
{
    uint32_t                  status;
    /**< Status message or error code.

         @values Refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}
          - APR_EOK -- Success
          - APR_EBADPARAM -- Failure @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_port_cmdrsp_get_param_v3_t */
/** @} */ /* end_addtogroup afe_rsp_port_get_paramsv3 */


typedef struct afe_algo_log_header_t afe_algo_log_header_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_algo_log_header_t
@{ */
/** Header structure for logging algorithm data. Immediately following this
    structure is the actual algorithm logging data for the module_id/param_id
    pair.
 */
struct afe_algo_log_header_t
{
   uint32_t module_id;
   /**< Valid ID of the module.

        @values See Chapter @xref{hdr:CalibrationIDs} */

   uint32_t param_id;
    /**< Valid ID of the parameter.

         @values See Chapter @xref{hdr:CalibrationIDs} */

   uint16_t param_size;
   /**< Size of the parameter data based on the module_id/param_id
        combination.

        @values > 0, in multiples of 4 bytes */

   uint16_t reserved;
   /**< This field must be set to 0. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_algo_log_header_t */


/** @addtogroup afe_service_api_deprecated
@{ */
/** Deprecated. Use #AFE_SVC_CMD_SET_PARAM_V2.

    Configures one or more parameters to the AFE service. Typically, this
    command is used to apply global calibration data.

  @apr_hdr_fields
    Opcode -- AFE_SVC_CMD_SET_PARAM

  @apr_msgpayload{afe_svc_cmd_set_param_t}
    @tablens{weak__afe__svc__cmd__set__param__t} @newpage

  @par Parameter data variable payload (afe_svc_param_data_t)
    @tablens{weak__afe__svc__param__data__t}

  @description
    This command is sent from the client to the aDSP to set the parameters of
    AFE service. A single command can be used to set multiple parameters.
    @par
    Messages can be either in-band or out-of-band via shared memory. The modes
    are:
    - In-band set parameter -- In this mode, the physical address is set to
      NULL. The rest of the payload contains the parameters as shown in the
      parameter data variable payload (%afe_svc_param_data_t).
    - Shared memory set parameter -- In this mode, both the physical address to
      the parameter data and the data payload size are sent. The rest of the
      message is in shared memory in the parameter data variable payload. The
      memory content is the same as the in-band payload
      structure (%afe_svc_param_data_t).

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    None.
 */
#define AFE_SVC_CMD_SET_PARAM                                      0x000100f3

/* Payload of the AFE parameter data. */
typedef struct afe_svc_param_data_t afe_svc_param_data_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_svc_param_data_t
@{ */
/* Payload of the AFE parameter data used by AFE_SVC_CMD_SET_PARAM. */
/** Immediately following this structure are param_size bytes of calibration
    data. The structure and size depend on the module_id/param_id combination.
 */
struct afe_svc_param_data_t
{
   uint32_t module_id;
   /**< Valid ID of the module.

        @values See Chapter @xref{hdr:CalibrationIDs} */

   uint32_t param_id;
    /**< Valid ID of the parameter.

         @values See Chapter @xref{hdr:CalibrationIDs} */

   uint16_t param_size;
   /**< Size of the parameter data based on the module_id/param_id
        combination.

        @values > 0, in multiples of 4 bytes */

   uint16_t reserved;
   /**< This field must be set to 0. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_svc_param_data_t */

/* Payload of configuration/calibration settings for the AFE Service. */
typedef struct afe_svc_cmd_set_param_t afe_svc_cmd_set_param_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_svc_cmd_set_param_t
@{ */
/* Payload of the AFE_SVC_CMD_SET_PARAM command's
    configuration/calibration settings for the AFE service. */
/** For in-band data, immediately following this structure is the
    <b>Parameter data variable payload</b> (%afe_svc_param_data_t).
 */
struct afe_svc_cmd_set_param_t
{
   uint32_t payload_size;
    /**< Actual size of the variable payload accompanying the message or in
         shared memory. This field is used for parsing both in-band and
         out-of-band data.

         @values > 0 bytes, in multiples of 4 bytes */

   uint32_t payload_address_lsw;
   /**< Lower 32 bits of the 64-bit payload address. */

   uint32_t payload_address_msw;
   /**< Upper 32 bits of the 64-bit payload address.

        The 64-bit number formed by payload_address_lsw and payload_address_msw
        word must be contiguous memory, and it must be 4 KB aligned.

         @values
         - For a 32-bit shared memory address, this field must be set to 0.
         - For a 36-bit shared memory address, bits 31 to 4 must be set to 0.
         - For a 64-bit shared memory address, any 32-bit value. @tablebulletend */

   uint32_t mem_map_handle;
   /**< Unique identifier for a shared memory address.

        @values
        - NULL -- The message is in the payload (in-band).
        - Non-NULL -- The parameter data payload begins at the address
          specified by a pointer to the physical address of the payload in
          shared memory (out-of-band).

        The aDSP returns this memory map handle through
        #AFE_SERVICE_CMD_SHARED_MEM_MAP_REGIONS.

        An optional field is available if parameter data is in-band:
        %afe_port_param_data_v2_t param_data[...].
        For detailed payload content, see %afe_port_param_data_v2_t. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_svc_cmd_set_param_t */
/** @} */ /* end_addtogroup afe_service_api_deprecated */


/** @addtogroup afe_cmd_svc_set_paramsv2
@{ */
/** Configures one or more parameters to the AFE service. Typically, this
    command is used to apply global calibration data.

    This command supports module instances. It can be called on different
    instances of the same module.

  @apr_hdr_fields
    Opcode -- AFE_SVC_CMD_SET_PARAM_V2

  @apr_msgpayload{afe_svc_cmd_set_param_v2_t}
    @tablens{weak__afe__svc__cmd__set__param__v2__t} @newpage

  @par Parameter data variable payload (afe_svc_param_data_v2_t)
    @tablens{weak__afe__svc__param__data__v2__t}

  @description
    The client sends this command to the aDSP to set the parameters of the
    AFE service. A single command can be used to set multiple parameters.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    None.
*/
#define AFE_SVC_CMD_SET_PARAM_V2                                   0x000100FC

/* Payload of configuration/calibration settings for the AFE Service. */
typedef struct afe_svc_cmd_set_param_v2_t afe_svc_cmd_set_param_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_svc_cmd_set_param_v2_t
@{ */
/* Payload of the AFE_SVC_CMD_SET_PARAM_V2 command's
    configuration/calibration settings for the AFE service. */
/** For in-band data, immediately following this structure is the
    <b>Parameter data variable payload</b> (%afe_svc_param_data_v2_t).
*/
struct afe_svc_cmd_set_param_v2_t
{
   uint32_t payload_address_lsw;
   /**< Lower 32 bits of the 64-bit payload address. */

   uint32_t payload_address_msw;
   /**< Upper 32 bits of the 64-bit payload address.

        The 64-bit number formed by payload_address_lsw and payload_address_msw
        word must be contiguous memory, and it must be 4 KB aligned.

         @values
         - For a 32-bit shared memory address, this field must be set to 0.
         - For a 36-bit shared memory address, bits 31 to 4 must be set to 0.
         - For a 64-bit shared memory address, any 32-bit value. @tablebulletend */

   uint32_t mem_map_handle;
   /**< Unique identifier for a shared memory address.

        @values
        - NULL -- The message is in the payload (in-band).
        - Non-NULL -- The parameter data payload begins at the address
          specified by a pointer to the physical address of the payload in
          shared memory (out-of-band).

        The aDSP returns this memory map handle through
        #AFE_SERVICE_CMD_SHARED_MEM_MAP_REGIONS. */

   uint32_t payload_size;
    /**< Actual size of the variable payload accompanying the message or in
         shared memory. This field is used for parsing both in-band and
         out-of-band data.

         @values > 0 bytes, in multiples of 4 bytes */
        /* An optional field is available if parameter data is in-band:
        %afe_svc_param_data_v2_t param_data[...]. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_svc_cmd_set_param_v2_t */

/* Payload of the AFE parameter data. */
typedef struct afe_svc_param_data_v2_t afe_svc_param_data_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_svc_param_data_v2_t
@{ */
/* Payload of the AFE parameter data used by AFE_SVC_CMD_SET_PARAM_V2. */
/** Immediately following this structure are param_size bytes of calibration
    data. The structure and size depend on the module_id/param_id combination.
*/
struct afe_svc_param_data_v2_t
{
   uint32_t module_id;
    /**< Valid ID of the module.

        @values See Chapter @xref{hdr:CalibrationIDs} */

   uint16_t instance_id;
   /**< Instance ID of the module to be configured. Only a zero value is
        supported for the AFE service parameters. */

   uint16_t reserved;
   /**< This field must be set to 0. */

   uint32_t param_id;
    /**< Valid ID of the parameter.

         @values See Chapter @xref{hdr:CalibrationIDs} */

   uint32_t param_size;
   /**< Size of the parameter data based on the module_id/param_id
        combination.

        @values > 0, in multiples of 4 bytes */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_svc_param_data_v2_t */
/** @} */ /* end_addtogroup afe_cmd_svc_set_paramsv2 */

/** @addtogroup afe_module_hw_mad
@{ */
/** ID of the parameter used by AFE_MODULE_HW_MAD to configure a SLIMbus
    slave port.

    Only #AFE_PORT_CMD_SET_PARAM_V2 (deprecated) and #AFE_PORT_CMD_SET_PARAM_V3
    can use this parameter ID.

    @apr_msgpayload{afe_slimbus_slave_port_cfg_param_v1_t}
    @table{weak__afe__slimbus__slave__port__cfg__param__v1__t}
 */
#define AFE_PARAM_ID_SLIMBUS_SLAVE_PORT_CFG                             0x00010233

/** Version information used to handle future additions to
    AFE_PARAM_ID_SLIMBUS_SLAVE_PORT_CFG processing (for backward
    compatibility). @newpage
 */
#define AFE_API_VERSION_SLIMBUS_SLAVE_PORT_CFG                            0x1

typedef struct afe_slimbus_slave_port_cfg_param_v1_t afe_slimbus_slave_port_cfg_param_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_slimbus_slave_port_cfg_param_v1_t
@{ */
/* AFE SLIMbus slave port configuration structure used by the
    AFE_PARAM_ID_SLIMBUS_SLAVE_PORT_CFG parameter (version 1).
 */
struct afe_slimbus_slave_port_cfg_param_v1_t
{
   uint32_t minor_version;
   /**< Tracks the configuration of this parameter.

        @values #AFE_API_VERSION_SLIMBUS_SLAVE_PORT_CFG */

    uint16_t slimbus_dev_id;
    /**< Type of the MAD hardware block.

         Set the respective bits in mad_type to enable (1) or disable (0)
         multiple paths.

         @values
         - #AFE_SLIMBUS_DEVICE_1
         - #AFE_SLIMBUS_DEVICE_2 @tablebulletend*/

   uint16_t slave_dev_pgd_la;
   /**< Logical 8-bit address of the slave port for a ported generic device.

        @values 0 to 255 */

   uint16_t slave_dev_intfdev_la;
   /**< Logical 8-bit address of the slave interface for an interface device.

        @values 0 to 255*/

    uint16_t                  bit_width;
    /**< Bit width of the sample.

         @values 16, 24, 32 */

    uint16_t                  data_format;
    /**< Data format supported by the SLIMbus hardware.

         @values #AFE_SB_DATA_FORMAT_NOT_INDICATED (Default)

         The default value indicates that the hardware does not perform any
         format conversions before the data transfer. */

    uint16_t num_chnnels;
    /**< Number of channels.

        @values 1 to #AFE_PORT_MAX_AUDIO_CHAN_CNT */

    uint16_t slave_port_mapping[AFE_PORT_MAX_AUDIO_CHAN_CNT];
    /**< Mapping of slave port IDs to which the master port is to be
         connected.

        @values 0 to 24*/
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_slimbus_slave_port_cfg_param_v1_t */
/** @} */ /* end_addtogroup afe_module_hw_mad */


/** @addtogroup afe_module_clock_set
@{ */
/** ID of the Clock Set module used to set the clocks in audio use cases.
    This module supports the following parameter ID:
    - #AFE_PARAM_ID_CLOCK_SET

    @apr_hdr_fields
    Opcode -- AFE_MODULE_CLOCK_SET
 */
/* Supported Chipset ID: MSM8996, MSM8953, MSM8937, MSM8917 */
#define AFE_MODULE_CLOCK_SET                               0x0001028F

/** ID of the parameter used by #AFE_MODULE_CLOCK_SET to
    configure the audio clock.

    #AFE_SVC_CMD_SET_PARAM (deprecated) and #AFE_SVC_CMD_SET_PARAM_V2 can use
    this parameter ID.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_CLOCK_SET

    @apr_msgpayload{afe_param_id_clock_set_t}
    @table{weak__afe__param__id__clock__set__t}
 */
/* Supported Chipset ID: MSM8996, MSM8953, MSM8937, MSM8917 */
#define AFE_PARAM_ID_CLOCK_SET                             0x00010290

/** Version information used to handle future additions to audio clock
    configuration (for backward compatibility).
 */
#define AFE_API_VERSION_CLOCK_SET                                     0x1


/* Clock ID Enumeration Define. */

/** Start of the range of MI2S clock IDs. */
#define AFE_CLOCK_SET_CLOCK_ID_MI2S_RANGE_START                       0x100

/** Clock ID of the primary MI2S internal bit clock (IBIT). */
#define AFE_CLOCK_SET_CLOCK_ID_PRI_MI2S_IBIT                          0x100

/** Clock ID of the primary MI2S external bit clock (EBIT). */
#define AFE_CLOCK_SET_CLOCK_ID_PRI_MI2S_EBIT                          0x101

/** Clock ID of the secondary MI2S IBIT. */
#define AFE_CLOCK_SET_CLOCK_ID_SEC_MI2S_IBIT                          0x102

/** Clock ID of the secondary MI2S EBIT. */
#define AFE_CLOCK_SET_CLOCK_ID_SEC_MI2S_EBIT                          0x103

/** Clock ID of the tertiary MI2S IBIT. */
#define AFE_CLOCK_SET_CLOCK_ID_TER_MI2S_IBIT                          0x104

/** Clock ID of the tertiary MI2S EBIT. */
#define AFE_CLOCK_SET_CLOCK_ID_TER_MI2S_EBIT                          0x105

/** Clock ID of the quaternary MI2S IBIT. */
#define AFE_CLOCK_SET_CLOCK_ID_QUAD_MI2S_IBIT                         0x106

/** Clock ID of the quaternary MI2S EBIT. */
#define AFE_CLOCK_SET_CLOCK_ID_QUAD_MI2S_EBIT                         0x107

/** Clock ID of the speaker MI2S IBIT. */
#define AFE_CLOCK_SET_CLOCK_ID_SPEAKER_I2S_IBIT                       0x108

/** Clock ID of the speaker MI2S EBIT. */
#define AFE_CLOCK_SET_CLOCK_ID_SPEAKER_I2S_EBIT                       0x109

/** Clock ID of the speaker MI2S OSR. */
#define AFE_CLOCK_SET_CLOCK_ID_SPEAKER_I2S_OSR                        0x10A

/** Clock ID of the quinary MI2S IBIT. */
#define AFE_CLOCK_SET_CLOCK_ID_QUI_MI2S_IBIT                          0x10B

/** Clock ID of the quinary MI2S EBIT. */
#define AFE_CLOCK_SET_CLOCK_ID_QUI_MI2S_EBIT                          0x10C

/** Clock ID of the senary MI2S IBIT. */
#define AFE_CLOCK_SET_CLOCK_ID_SEN_MI2S_IBIT                          0x10D

/** Clock ID of the senary MI2S EBIT. */
#define AFE_CLOCK_SET_CLOCK_ID_SEN_MI2S_EBIT                          0x10E

/** ID of I2S IBIT clock 0 that is used with integrated codec. */
#define AFE_CLOCK_SET_CLOCK_ID_INT0_I2S_IBIT                          0x10F

/** ID of I2S IBIT clock 1 that is used with integrated codec. */
#define AFE_CLOCK_SET_CLOCK_ID_INT1_I2S_IBIT                          0x110

/** ID of I2S IBIT clock 2 that is used with integrated codec. */
#define AFE_CLOCK_SET_CLOCK_ID_INT2_I2S_IBIT                          0x111

/** ID of I2S IBIT clock 3 that is used with integrated codec. */
#define AFE_CLOCK_SET_CLOCK_ID_INT3_I2S_IBIT                          0x112

/** ID of I2S IBIT clock 4 that is used with integrated codec. */
#define AFE_CLOCK_SET_CLOCK_ID_INT4_I2S_IBIT                          0x113

/** ID of I2S IBIT clock 5 that is used with integrated codec. */
#define AFE_CLOCK_SET_CLOCK_ID_INT5_I2S_IBIT                          0x114

/** ID of I2S IBIT clock 6 that is used with integrated codec. */
#define AFE_CLOCK_SET_CLOCK_ID_INT6_I2S_IBIT                          0x115

/** Clock ID of the quinary MI2S OSR. */
#define AFE_CLOCK_SET_CLOCK_ID_QUI_MI2S_OSR                           0x116

/** End of the range of MI2S clock IDs. */
#define AFE_CLOCK_SET_CLOCK_ID_MI2S_RANGE_END                         0x116

/** Start of the range of PCM clock IDs. */
#define AFE_CLOCK_SET_CLOCK_ID_PCM_RANGE_START                        0x200

/** Clock ID of the primary PCM IBIT. */
#define AFE_CLOCK_SET_CLOCK_ID_PRI_PCM_IBIT                           0x200

/** Clock ID of the primary PCM EBIT. */
#define AFE_CLOCK_SET_CLOCK_ID_PRI_PCM_EBIT                           0x201

/** Clock ID of the secondary PCM IBIT. */
#define AFE_CLOCK_SET_CLOCK_ID_SEC_PCM_IBIT                           0x202

/** Clock ID of the secondary PCM EBIT. */
#define AFE_CLOCK_SET_CLOCK_ID_SEC_PCM_EBIT                           0x203

/** Clock ID of the tertiary PCM IBIT. */
#define AFE_CLOCK_SET_CLOCK_ID_TER_PCM_IBIT                           0x204

/** Clock ID of the tertiary PCM EBIT. */
#define AFE_CLOCK_SET_CLOCK_ID_TER_PCM_EBIT                           0x205

/** Clock ID of the quaternary PCM IBIT. */
#define AFE_CLOCK_SET_CLOCK_ID_QUAD_PCM_IBIT                          0x206

/** Clock ID of the quaternary PCM EBIT. */
#define AFE_CLOCK_SET_CLOCK_ID_QUAD_PCM_EBIT                          0x207 

/** Clock ID of the quinary PCM IBIT. */
#define AFE_CLOCK_SET_CLOCK_ID_QUI_PCM_IBIT                           0x208

/** Clock ID of the quinary PCM EBIT. */
#define AFE_CLOCK_SET_CLOCK_ID_QUI_PCM_EBIT                           0x209

/** Clock ID of the quinary PCM OSR. */
#define AFE_CLOCK_SET_CLOCK_ID_QUI_PCM_OSR                            0x20A

/** End of of the range of PCM clock IDs. */
#define AFE_CLOCK_SET_CLOCK_ID_PCM_RANGE_END                          0x20A

/** Start of the TDM clock ID group. */
#define AFE_CLOCK_SET_CLOCK_ID_TDM_RANGE_START                        0x200

/** Clock ID for the primary TDM IBIT. */
#define AFE_CLOCK_SET_CLOCK_ID_PRI_TDM_IBIT                           0x200

/** Clock ID for the primary TDM EBIT. */
#define AFE_CLOCK_SET_CLOCK_ID_PRI_TDM_EBIT                           0x201

/** Clock ID for the secondary TDM IBIT. */
#define AFE_CLOCK_SET_CLOCK_ID_SEC_TDM_IBIT                           0x202

/** Clock ID for the secondary TDM EBIT. */
#define AFE_CLOCK_SET_CLOCK_ID_SEC_TDM_EBIT                           0x203

/** Clock ID for the tertiary TDM IBIT. */
#define AFE_CLOCK_SET_CLOCK_ID_TER_TDM_IBIT                           0x204

/** Clock ID for the tertiary TDM EBIT. */
#define AFE_CLOCK_SET_CLOCK_ID_TER_TDM_EBIT                           0x205

/** Clock ID for the quaternary TDM IBIT. */
#define AFE_CLOCK_SET_CLOCK_ID_QUAD_TDM_IBIT                          0x206

/** Clock ID for the quaternary TDM EBIT. */
#define AFE_CLOCK_SET_CLOCK_ID_QUAD_TDM_EBIT                          0x207 

/** Clock ID for the quinary TDM IBIT. */
#define AFE_CLOCK_SET_CLOCK_ID_QUI_TDM_IBIT                           0x208

/** Clock ID for the quinary TDM EBIT. */
#define AFE_CLOCK_SET_CLOCK_ID_QUI_TDM_EBIT                           0x209

/** Clock ID for the quinary TDM OSR. */
#define AFE_CLOCK_SET_CLOCK_ID_QUI_TDM_OSR                            0x20A

/** End of the TDM clock ID group. */
#define AFE_CLOCK_SET_CLOCK_ID_TDM_RANGE_END                          0x20A

/** Start of the range of MCLK clock IDs. */
#define AFE_CLOCK_SET_CLOCK_ID_MCLK_RANGE_START                       0x300

/** Clock ID for MCLK 1. */
#define AFE_CLOCK_SET_CLOCK_ID_MCLK_1                                 0x300

/** Clock ID for MCLK 2. */
#define AFE_CLOCK_SET_CLOCK_ID_MCLK_2                                 0x301

/** Clock ID for MCLK 3. */
#define AFE_CLOCK_SET_CLOCK_ID_MCLK_3                                 0x302

/** Clock ID for the internal digital codec core. */
#define AFE_CLOCK_SET_CLOCK_ID_INTERNAL_DIGITAL_CODEC_CORE            0x303

/** Clock ID for MCLK 4. */
#define AFE_CLOCK_SET_CLOCK_ID_MCLK_4                                 0x304

/** Clock ID for MCLK0 that is used with the internal codec. */
#define AFE_CLOCK_SET_CLOCK_ID_INT_MCLK_0                             0x305

/** Clock ID for MCLK1 that is used with the internal codec. */
#define AFE_CLOCK_SET_CLOCK_ID_INT_MCLK_1                             0x306

/** Clock ID for internal soundwire master near-pad logic (NPL). */
#define AFE_CLOCK_SET_CLOCK_ID_SWR_NPL_CLK                            0x307

/** End of the range of MCLK clock IDs. */
#define AFE_CLOCK_SET_CLOCK_ID_MCLK_RANGE_END                         0x307


/** Start of the range of AHB branch clock IDs. */
#define AFE_CLOCK_SET_CLOCK_ID_AHB_RANGE_START                        0x400

/** @} */ /* end_addtogroup afe_module_clock_set */


/** @ingroup afe_service_api_deprecated
    Deprecated.

    Clock ID for AHB HDMI input. */
#define AFE_CLOCK_SET_CLOCK_ID_AHB_HDMI_INPUT                         0x400


/** @addtogroup afe_module_clock_set
@{ */
/** End of the range of AHB branch clock IDs. */
#define AFE_CLOCK_SET_CLOCK_ID_AHB_RANGE_END                          0x400


/** Start of the range of miscellaneous clock IDs. */
#define AFE_CLOCK_SET_CLOCK_ID_MISC_RANGE_START                       0x500

/** Clock ID for the SPDIF core. */
#define AFE_CLOCK_SET_CLOCK_ID_SPDIF_CORE                             0x500

/** End of the range of miscellaneous clock IDs. */
#define AFE_CLOCK_SET_CLOCK_ID_MISC_RANGE_END                         0x500


/* Clock ATTRIBUTE Enumeration Define. 
    divider = A/B   A -- dividend    B -- divisor
 */
/** Clock attribute is invalid (reserved for internal use). */
#define AFE_CLOCK_SET_CLOCK_ATTRIBUTE_INVALID                         0x0

/** Clock attribute for no coupled clocks. */
#define AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_NO                       0x1

/** Clock attribute for the dividend of the coupled clocks. */
#define AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_DIVIDEND                 0x2

/** Clock attribute for the divisor of the coupled clocks. */
#define AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_DIVISOR                  0x3

/** Clock attribute for the invert-and-no-couple case. */
#define AFE_CLOCK_SET_CLOCK_ATTRIBUTE_INVERT_COUPLE_NO                0x4


/* Clock ROOT Enumeration Define. */
/** Default root clock source. */
#define AFE_CLOCK_SET_CLOCK_ROOT_DEFAULT                              0x0

typedef struct afe_param_id_clock_set_t afe_param_id_clock_set_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_clock_set_t
@{ */
/* Payload of the AFE_PARAM_ID_CLOCK_SET parameter.
 */
struct afe_param_id_clock_set_t
{
    uint32_t                 clk_set_minor_version;
    /**< Tracks the clock settings for this parameter.

         @values #AFE_API_VERSION_CLOCK_SET */

    uint32_t                 clk_id;
    /**< Clock ID.

         @values for MI2S clock IDs
          #AFE_CLOCK_SET_CLOCK_ID_MI2S_RANGE_START through
          #AFE_CLOCK_SET_CLOCK_ID_MI2S_RANGE_END

         @values for PCM clock IDs
          #AFE_CLOCK_SET_CLOCK_ID_PCM_RANGE_START through
          #AFE_CLOCK_SET_CLOCK_ID_PCM_RANGE_END

         @values for MCLK clock IDs
          #AFE_CLOCK_SET_CLOCK_ID_MCLK_RANGE_START through
          #AFE_CLOCK_SET_CLOCK_ID_MCLK_RANGE_END

         @values for AHB branch clock IDs
          #AFE_CLOCK_SET_CLOCK_ID_AHB_RANGE_START through
          #AFE_CLOCK_SET_CLOCK_ID_AHB_RANGE_END

         @values for miscellaneous clock IDs
          #AFE_CLOCK_SET_CLOCK_ID_MISC_RANGE_START through
          #AFE_CLOCK_SET_CLOCK_ID_MISC_RANGE_END */

    uint32_t                 clk_freq_in_hz;
    /**< Clock frequency to set.

         @values @ge 0 Hz */

    uint16_t                 clk_attri;
    /**< Divider for two clocks that are coupled, if necessary:
         divider = A/B, where A is the dividend and B is the divisor.

         @values
         - #AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_NO -- For no divider-related
           clocks
         - #AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_DIVIDEND
         - #AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_DIVISOR @vertspace{-28} */

    uint16_t                 clk_root;
    /**< Root clock source.

         @values #AFE_CLOCK_SET_CLOCK_ROOT_DEFAULT

         Currently, only AFE_CLOCK_SET_CLOCK_ROOT_DEFAULT is valid. */

    uint32_t                 enable;
    /**< Specifies whether the clock is enabled.

         @values
          - 0 -- Disabled
          - 1 -- Enabled

         If the clock is disabled, the clk_freq_in_hz, clk_attri, and clk_root
         fields are ignored. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_param_id_clock_set_t */
/** @} */ /* end_addtogroup afe_module_clock_set */


/** @addtogroup afe_module_cdc_device
@{ */
/** ID of the hardware Codec Device Configuration module, which supports the
    following parameter IDs:
    - #AFE_PARAM_ID_CDC_SLIMBUS_SLAVE_CFG
    - #AFE_PARAM_ID_CDC_AANC_VERSION
    - #AFE_PARAM_ID_CLIP_BANK_SEL_CFG
    - #AFE_PARAM_ID_CDC_REG_CFG_INIT
    - #AFE_PARAM_ID_CDC_REG_CFG_V2

    @apr_hdr_fields
    Opcode -- AFE_MODULE_CDC_DEV_CFG
 */
#define AFE_MODULE_CDC_DEV_CFG                                     0x00010234

/** ID of the parameter used by #AFE_MODULE_CDC_DEV_CFG to configure a codec
    SLIMbus slave device.

    Only #AFE_SVC_CMD_SET_PARAM_V2 can use this parameter ID.

    @apr_msgpayload{afe_cdc_slimbus_slave_cfg_param_v1_t}
    @table{weak__afe__cdc__slimbus__slave__cfg__param__v1__t}
 */
#define AFE_PARAM_ID_CDC_SLIMBUS_SLAVE_CFG                        0x00010235

/** Version information used to handle future additions to
    AFE_PARAM_ID_CDC_SLIMBUS_SLAVE_CFG processing (for backward compatibility).
    @newpage
 */
#define AFE_API_VERSION_CDC_SLIMBUS_SLAVE_CFG                            0x1

typedef struct afe_cdc_slimbus_slave_cfg_param_v1_t afe_cdc_slimbus_slave_cfg_param_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_cdc_slimbus_slave_cfg_param_v1_t
@{ */
/* Configuration structure for the AFE_PARAM_ID_CDC_SLIMBUS_SLAVE_CFG
 module (version 1).
 */
struct afe_cdc_slimbus_slave_cfg_param_v1_t
{
    uint32_t slave_dev_minor_version;
    /**< Tracks the configuration of this parameter.

         @values #AFE_API_VERSION_CDC_SLIMBUS_SLAVE_CFG */

    uint32_t device_enum_addr_lsw;
    /**< Lower 16 bits of the SLIMbus slave device 48-bit enumeration address.
         The lower half comes first to be consistent with little endian
         architecture. */

    uint32_t device_enum_addr_msw;
    /**< Upper 16 bits of the SLIMbus slave device 48-bit enumeration address.

         @values
         - Bits 15 to 0 contain the upper 16 bits of the address
         - Bits 31 to 16 must be set to 0 @tablebulletend */

    uint16_t tx_slave_port_offset;
    /**< Offset value for Tx slave ports.

         @values Valid offset value based on the hardware design
                 (refer to the applicable hardware interface specification) */

    uint16_t rx_slave_port_offset;
    /**< Offset value for Rx slave ports.

         @values Valid offset value based on the hardware design
                 (refer to the applicable hardware interface specification) */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_cdc_slimbus_slave_cfg_param_v1_t */
/** @} */ /* end_addtogroup afe_module_cdc_device */


/** @addtogroup afe_mod_param_id_deprecated
@{ */
/** Deprecated. Use #AFE_PARAM_ID_CDC_REG_CFG_V2 and
    #AFE_PARAM_ID_CDC_REG_SLIMBUS_CFG instead.

    ID of the parameter used by AFE_MODULE_CDC_DEV_CFG to configure the
    codec register. Only AFE_SVC_CMD_SET_PARAM can use this parameter ID.

    @apr_msgpayload{afe_cdc_reg_cfg_param_v1_t}
    @table{weak__afe__cdc__reg__cfg__param__v1__t}
 */
#define AFE_PARAM_ID_CDC_REG_CFG                             0x00010236

/** Deprecated. See #AFE_API_VERSION_CDC_REG_CFG_V2.

    Version information used to handle future additions to
    AFE_PARAM_ID_CDC_REG_CFG processing (for backward compatibility). @newpage
 */
#define AFE_API_VERSION_CDC_REG_CFG                                    0x1

/** @} */ /* end_addtogroup afe_mod_param_id_deprecated */


/** @addtogroup afe_module_cdc_device
@{ */
/** ID of the parameter used by #AFE_MODULE_CDC_DEV_CFG to configure the
    codec register.

    Only #AFE_SVC_CMD_SET_PARAM (deprecated) or #AFE_SVC_CMD_SET_PARAM_V2 can
    use this parameter ID.

    @apr_msgpayload{afe_cdc_reg_cfg_param_v2_t}
    @table{weak__afe__cdc__reg__cfg__param__v2__t}
 */
#define AFE_PARAM_ID_CDC_REG_CFG_V2                          0x0001029B

/** Version information used to handle future additions to
    AFE_PARAM_ID_CDC_REG_CFG_V2 processing (for backward compatibility).
 */
#define AFE_API_VERSION_CDC_REG_CFG_V2                                0x1

/** @xreflabel{First}
    AANC_REG_LPF_FF_FB (CDC_ANCn_LPF_B1_CTL) register. */
#define AFE_CRC_AANC_LPF_FF_FB                                        0x1

/** AANC_REG_LPF_COEFF_MSB (CDC_ANCn_LPF_B2_CTL) register. */
#define AFE_CRC_AANC_LPF_COEFF_MSB                                  0x2

/** AANC_REG_LPF_COEFF_LSB (CDC_ANCn_LPF_B3_CTL) register. */
#define AFE_CRC_AANC_LPF_COEFF_LSB                                     0x3

/** Hardware register field used to enable MAD audio detection. */
#define AFE_CRC_HW_MAD_AUDIO_ENABLE                                  0x4

/** Hardware register field used to enable MAD ultrasound detection. */
#define AFE_CRC_HW_MAD_ULTR_ENABLE                                    0x5

/** Hardware register field used to enable MAD beacon detection. */
#define AFE_CRC_HW_MAD_BEACON_ENABLE                                  0x6

/** Hardware register field used to configure MAD audio sleep time. */
#define AFE_CRC_HW_MAD_AUDIO_SLEEP_TIME                                0x7

/** Hardware register field used to configure MAD ultrasound sleep time. */
#define AFE_CRC_HW_MAD_ULTR_SLEEP_TIME                              0x8

/** Hardware register field used to configure MAD beacon sleep time. */
#define AFE_CRC_HW_MAD_BEACON_SLEEP_TIME                            0x9

/** Hardware register field used to switch off the MAD Tx audio path after
    detection. */
#define AFE_CRC_HW_MAD_TX_AUDIO_SWITCH_OFF                            0xA

/** Hardware register field used to switch off the MAD Tx ultrasound path after
    detection. */
#define AFE_CRC_HW_MAD_TX_ULTR_SWITCH_OFF                            0xB

/** Hardware register field used to switch off the MAD Tx beacon path after
    detection. */
#define AFE_CRC_HW_MAD_TX_BEACON_SWITCH_OFF                            0xC

/** Hardware register field used to route the MAD audio interrupt. */
#define AFE_CRC_MAD_AUDIO_INT_DEST_SELECT_REG                        0xD

/** Hardware register field used to route the MAD ultrasound interrupt. */
#define AFE_CRC_MAD_ULT_INT_DEST_SELECT_REG                            0xE

/** Hardware register field used to route the MAD beacon interrupt. */
#define AFE_CRC_MAD_BEACON_INT_DEST_SELECT_REG                        0xF

/** Hardware register field used to route the audio clip interrupt
    of speaker 1. */
#define AFE_CRC_CLIP_INT_DEST_SELECT_REG                            0x10

/** More meaningful macro for AFE_CRC_CLIP_INT_DEST_SELECT_REG.
    This macro helps distinguish between two speakers that are generating
    clip interrupts. */
#define AFE_CRC_SPKR_CLIP_INT_DEST_SELECT_REG                        AFE_CRC_CLIP_INT_DEST_SELECT_REG

/** Hardware register field used to route the VBAT attack interrupt. */
#define AFE_CRC_VBAT_INT_DEST_SELECT_REG                            0x11

/** More meaningful macro for AFE_CRC_VBAT_INT_DEST_SELECT_REG.
    This macro helps distinguish between ATTACK and RELEASE interrupts. */
#define AFE_CRC_VBAT_ATTACK_INT_DEST_SELECT_REG                        AFE_CRC_VBAT_INT_DEST_SELECT_REG

/** Hardware register field used to specify the MAD audio interrupt mask
    field. */
#define AFE_CRC_MAD_AUDIO_INT_MASK_REG                                0x12

/** Hardware register field used to specify the MAD ultrasound interrupt mask
    field. */
#define AFE_CRC_MAD_ULT_INT_MASK_REG                                0x13

/** Hardware register field used to specify the MAD beacon interrupt mask
    field. */
#define AFE_CRC_MAD_BEACON_INT_MASK_REG                                0x14

/** Hardware register field used to specify the audio clip interrupt
    mask field of speaker 1. */
#define AFE_CRC_CLIP_INT_MASK_REG                                    0x15

/** More meaningful macro for AFE_CRC_CLIP_INT_MASK_REG. This macro helps
    distinguish between two speakers that are generating clip interrupts. */
#define AFE_CRC_SPKR_CLIP_INT_MASK_REG                                AFE_CRC_CLIP_INT_MASK_REG

/** Hardware register field used to specify the VBAT attack interrupt mask
    field. */
#define AFE_CRC_VBAT_INT_MASK_REG                                    0x16

/** More meaningful macro for AFE_CRC_VBAT_INT_MASK_REG.
    This macro helps distinguish between ATTACK and RELEASE interrupts. */
#define AFE_CRC_VBAT_ATTACK_INT_MASK_REG                            AFE_CRC_VBAT_INT_MASK_REG

/** Hardware register field used to specify the MAD audio interrupt status
    field. */
#define AFE_CRC_MAD_AUDIO_INT_STATUS_REG                            0x17

/** Hardware register field used to specify the MAD ultrasound interrupt
    status field. */
#define AFE_CRC_MAD_ULT_INT_STATUS_REG                                0x18

/** Hardware register field used to specify the MAD beacon interrupt status
    field. */
#define AFE_CRC_MAD_BEACON_INT_STATUS_REG                            0x19

/** Hardware register field used to specify the audio clip interrupt
    status field of speaker 1. */
#define AFE_CRC_CLIP_INT_STATUS_REG                                    0x1A

/** More meaningful macro for AFE_CRC_SPKR_CLIP_INT_STATUS_REG.
    This macro helps distinguish between two speakers that are generating
    clip interrupts. */
#define AFE_CRC_SPKR_CLIP_INT_STATUS_REG                            AFE_CRC_CLIP_INT_STATUS_REG

/** Hardware register field used to specify the VBAT attack interrupt
    status field. */
#define AFE_CRC_VBAT_INT_STATUS_REG                                    0x1B

/** More meaningful macro for AFE_CRC_VBAT_INT_STATUS_REG.
    This macro helps distinguish between ATTACK and RELEASE interrupts. */
#define AFE_CRC_VBAT_ATTACK_INT_STATUS_REG                            AFE_CRC_VBAT_INT_STATUS_REG

/** Hardware register field used to specify the MAD audio interrupt clear
    field. */
#define AFE_CRC_MAD_AUDIO_INT_CLEAR_REG                                0x1C

/** Hardware register field used to specify the MAD ultrasound interrupt clear
    field. */
#define AFE_CRC_MAD_ULT_INT_CLEAR_REG                                0x1D

/** Hardware register field used to specify the MAD beacon interrupt clear
    field. */
#define AFE_CRC_MAD_BEACON_INT_CLEAR_REG                            0x1E

/** Hardware register field used to specify the audio clip interrupt
    clear field of speaker 1. */
#define AFE_CRC_CLIP_INT_CLEAR_REG                                    0x1F

/** More meaningful macro for AFE_CRC_CLIP_INT_CLEAR_REG. This macro helps
    distinguish between two speakers that are generating clip interrupts. */
#define AFE_CRC_SPKR_CLIP_INT_CLEAR_REG                                AFE_CRC_CLIP_INT_CLEAR_REG

/** Hardware register field used to specify the VBAT attack interrupt clear
    field. */
#define AFE_CRC_VBAT_INT_CLEAR_REG                                    0x20

/** More meaningful macro for AFE_CRC_VBAT_INT_CLEAR_REG.
    This macro helps distinguish between ATTACK and RELEASE interrupts. */
#define AFE_CRC_VBAT_ATTACK_INT_CLEAR_REG                            AFE_CRC_VBAT_INT_CLEAR_REG

/** Hardware register field used to specify the SLIMbus slave port (Tx)
    watermark configuration field. */
#define AFE_CRC_SB_PGD_PORT_TX_WATERMARK_n                            0x21

/** Hardware register field used to specify the SLIMbus slave port (Tx)
    enable/disable configuration field. */
#define AFE_CRC_SB_PGD_PORT_TX_ENABLE_n                                0x22

/** Hardware register field used to specify the SLIMbus slave port (Rx)
    watermark configuration field. */
#define AFE_CRC_SB_PGD_PORT_RX_WATERMARK_n                            0x23

/** Hardware register field used to specify the SLIMbus slave port (Rx)
    enable/disable configuration field. */
#define AFE_CRC_SB_PGD_PORT_RX_ENABLE_n                                0x24

/** Hardware register field used to enable SLIMbus multichannel 0 (Tx) for
    the given slave port. */
#define AFE_CRC_SB_PGD_TX_PORTn_MULTI_CHNL_0 0x25

/** Hardware register field used to enable SLIMbus multichannel 1 (Tx) for
    the given slave port. */
#define AFE_CRC_SB_PGD_TX_PORTn_MULTI_CHNL_1 0x26

/** Hardware register field used to enable SLIMbus multichannel 0 (Rx) for
    the given slave port. */
#define AFE_CRC_SB_PGD_RX_PORTn_MULTI_CHNL_0 0x27

/** Hardware register field used to enable SLIMbus multichannel 1 (Rx) for
    the given slave port. */
#define AFE_CRC_SB_PGD_RX_PORTn_MULTI_CHNL_1 0x28

/** Hardware register field used to enable double banking.

    - For WCD9330 and WCD9320 codecs -- FFGAIN_ADAPTIVE bit
      (CDC_ANCn_IIR_B1_CTL)
    - For WCD9335 codec -- FFGAIN_ADAPTIVE bit (CDC_ANCn_IIR_ADAPT_CTL)
 */
#define AFE_CRC_AANC_FF_GAIN_ADAPTIVE        0x29

/** Hardware register field used to copy AANC gain from the foreground to the
    background register.

    - For WCD9330 and WCD9320 codecs -- FFGAIN_ADAPTIVE_EN bit
      (CDC_ANCn_IIR_B1_CTL)
    - For WCD9335 codec -- FFGAIN_UPDATE_EN bit (CDC_ANCn_IIR_ADAPT_CTL) */
#define AFE_CRC_AANC_FFGAIN_ADAPTIVE_EN      0x2A

/** Hardware register field used to update AANC gain.

    - For WCD9330and WCD9320 codecs -- GAIN bits (CDC_ANCn_GAIN_CTL)
    - For WCD9335 codec -- GAIN bits (CDC_ANCn_FF_A_GAIN_CTL) */
#define AFE_CRC_AANC_GAIN_CONTROL            0x2B

/** Hardware register field used to update the clip PCM-level bank
    selection of speaker 1 (CDC_SPKR_CLIPDET_B1_CTL). */
#define AFE_CRC_CLIP_PIPE_BANK_SEL                0x2C

/** More meaningful macro for AFE_CRC_CLIP_PIPE_BANK_SEL.
    This macro helps distinguish between two speakers that are generating clip
    interrupts. */
#define AFE_CRC_SPKR_CLIP_PIPE_BANK_SEL            AFE_CRC_CLIP_PIPE_BANK_SEL

/** Hardware register field used to update clip PCM-level
    register 0 of speaker 1. */
#define AFE_CRC_CDC_SPKR_CLIPDET_VAL0            0x2D

/** Hardware register field used to update clip PCM-level
    register 1 of speaker 1. */
#define AFE_CRC_CDC_SPKR_CLIPDET_VAL1            0x2E

/** Hardware register field used to update clip PCM-level
    register 2 of speaker 1. */
#define AFE_CRC_CDC_SPKR_CLIPDET_VAL2            0x2F

/** Hardware register field used to update clip PCM-level
    register 3 of speaker 1. */
#define AFE_CRC_CDC_SPKR_CLIPDET_VAL3            0x30

/** Hardware register field used to update clip PCM-level
    register 4 of speaker 1. */
#define AFE_CRC_CDC_SPKR_CLIPDET_VAL4            0x31

/** Hardware register field used to update clip PCM-level
    register 5 of speaker 1. */
#define AFE_CRC_CDC_SPKR_CLIPDET_VAL5            0x32

/** Hardware register field used to update clip PCM-level
    register 6 of speaker 1.*/
#define AFE_CRC_CDC_SPKR_CLIPDET_VAL6            0x33

/** Hardware register field used to update clip PCM-level
    register 7 of speaker 1. */
#define AFE_CRC_CDC_SPKR_CLIPDET_VAL7            0x34

/** Hardware register field used to route the VBAT release interrupt. */
#define AFE_CRC_VBAT_RELEASE_INT_DEST_SELECT_REG                            0x35

/** Hardware register field used to specify the VBAT release interrupt
    mask field. */
#define AFE_CRC_VBAT_RELEASE_INT_MASK_REG                                   0x36

/** Hardware register field used to specify the VBAT release interrupt
    status field. */
#define AFE_CRC_VBAT_RELEASE_INT_STATUS_REG                                 0x37

/** Hardware register field used to specify the VBAT release interrupt clear
    field. */
#define AFE_CRC_VBAT_RELEASE_INT_CLEAR_REG                                  0x38

/** Hardware register field used to route the audio clip
    interrupt of speaker 2. */
#define AFE_CRC_SPKR2_CLIP_INT_DEST_SELECT_REG                              0x39

/** Hardware register field used to specify the audio clip
    interrupt mask field of speaker 2. */
#define AFE_CRC_SPKR2_CLIP_INT_MASK_REG                                     0x3A

/** Hardware register field used to specify the audio clip
    interrupt status field of speaker 2. */
#define AFE_CRC_SPKR2_CLIP_INT_STATUS_REG                                   0x3B

/** Hardware register field used to specify the audio clip
    interrupt clear field of speaker 2. */
#define AFE_CRC_SPKR2_CLIP_INT_CLEAR_REG                                    0x3C

/** Hardware register field used to update the clip PCM-level bank
    selection of speaker 2 (CDC_SPKR_CLIPDET_B1_CTL). */
#define AFE_CRC_SPKR2_CLIP_PIPE_BANK_SEL                                    0x3D

/** Hardware register field used to update clip PCM-level
    register 0 of speaker 2. */
#define AFE_CRC_CDC_SPKR2_CLIPDET_VAL0                                      0x3E

/** Hardware register field used to update clip PCM-level
    register 1 of speaker 2. */
#define AFE_CRC_CDC_SPKR2_CLIPDET_VAL1                                      0x3F

/** Hardware register field used to update clip PCM-level
    register 2 of speaker 2. */
#define AFE_CRC_CDC_SPKR2_CLIPDET_VAL2                                      0x40

/** Hardware register field used to update clip PCM-level
    register 3 of speaker 2. */
#define AFE_CRC_CDC_SPKR2_CLIPDET_VAL3                                      0x41

/** Hardware register field used to update clip PCM-level
    register 4 of speaker 2. */
#define AFE_CRC_CDC_SPKR2_CLIPDET_VAL4                                      0x42

/** Hardware register field used to update clip PCM-level
    register 5 of speaker 2. */
#define AFE_CRC_CDC_SPKR2_CLIPDET_VAL5                                      0x43

/** Hardware register field used to update clip PCM-level
    register 6 of speaker 2.*/
#define AFE_CRC_CDC_SPKR2_CLIPDET_VAL6                                      0x44

/** Hardware register field used to update clip PCM-level
    register 7 of speaker 2. */
#define AFE_CRC_CDC_SPKR2_CLIPDET_VAL7                                      0x45

/** @xreflabel{Last}
    Hardware register field used to control the codec power state. */
#define AFE_CDC_SYS_PWR_STATE_CTRL                                          0x46

typedef struct afe_cdc_reg_cfg_param_v1_t afe_cdc_reg_cfg_param_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_cdc_reg_cfg_param_v1_t
@{ */
/* Hardware codec register configuration structure used by the
    AFE_PARAM_ID_CDC_REG_CFG parameter (version 1).
 */
struct afe_cdc_reg_cfg_param_v1_t
{
    uint32_t cdc_reg_cfg_minor_version;
    /**< Tracks the configuration of this parameter.

         @values #AFE_API_VERSION_CDC_REG_CFG */

    uint32_t reg_logical_addr;
    /**< Logical address of the value element mapped to the SLIMbus codec
         register to which read/write access is required.

         @values 32-bit logical address of the register to be accessed */

    uint32_t reg_field_type;
    /**< Enumeration for the register field to be updated.

         If more than one field in the register is to be updated, the same
         command must be sent for each field.

         @values
         - 0 is reserved
         - See Sections @xref{First} through @xref{Last} @tablebulletend */

   uint32_t reg_field_bit_mask;
   /**< Register bitmask to which the ANC algorithm writes.

        @values Valid register bitmask */

   uint16_t reg_bit_width;
   /**< Bit width of the register.

        @values Valid register bit width */

   uint16_t reg_offset_scale;
   /**< Offset scale factor for codec registers.
        For example, 0x400+1*n, where 1 is reg_offset_scale.

        @values Valid register offset scale, if any (Default = 0) */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_cdc_reg_cfg_param_v1_t */

typedef struct afe_cdc_reg_cfg_param_v2_t afe_cdc_reg_cfg_param_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_cdc_reg_cfg_param_v2_t
@{ */
/*  Hardware codec register configuration structure used by the
    AFE_PARAM_ID_CDC_REG_CFG_V2 parameter (version 2).
 */
struct afe_cdc_reg_cfg_param_v2_t
{
    uint32_t minor_version;
    /**< Tracks the configuration of this parameter.

         @values #AFE_API_VERSION_CDC_REG_CFG_V2 */

    uint32_t reg_addr;
    /**< Address of the register to which read/write access is required.

         @values Valid register address */

    uint32_t reg_field_type;
    /**< Enumeration for the register field to be updated.

         If more than one field in the register is to be updated, the same
         command must be sent for each field.

         @values
         - 0 -- Reserved
         - See Sections @xref{First} through @xref{Last} @tablebulletend */

   uint32_t reg_field_bit_mask;
   /**< Register bitmask to which the AFE writes.

        @values Valid register bitmask */

   uint16_t reg_bit_width;
   /**< Bit width of the register.

        @values Valid register bit width */

   uint16_t reg_offset_scale;
   /**< Offset scale factor for codec registers.
        For example, 0x400+1*n, where 1 is reg_offset_scale.

        @values Valid register offset scale, if any (else Default = 0) */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_cdc_reg_cfg_param_v2_t */

/** ID of the parameter used by #AFE_MODULE_CDC_DEV_CFG to configure codec
    SLIMbus variables.
    Only #AFE_SVC_CMD_SET_PARAM (deprecated) and #AFE_SVC_CMD_SET_PARAM_V2 can
    use this parameter ID.

    @apr_msgpayload{afe_cdc_reg_slimbus_cfg_param_v1_t}
    @table{weak__afe__cdc__reg__slimbus__cfg__param__v1__t}
 */
#define AFE_PARAM_ID_CDC_REG_SLIMBUS_CFG            0x0001029C

/** Version information used to handle future additions to
    AFE_PARAM_ID_CDC_REG_SLIMBUS_CFG processing (for backward compatibility).
    @newpage
 */
#define AFE_API_VERSION_CDC_REG_SLIMBUS_CFG         0x1

typedef struct afe_cdc_reg_slimbus_cfg_param_v1_t afe_cdc_reg_slimbus_cfg_param_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_cdc_reg_slimbus_cfg_param_v1_t
@{ */
/* Configuration structure for the AFE_PARAM_ID_CDC_REG_SLIMBUS_CFG parameter
  (version 1).
 */
struct afe_cdc_reg_slimbus_cfg_param_v1_t
{
    uint32_t minor_version;
    /**< Tracks the configuration of this parameter.

         @values #AFE_API_VERSION_CDC_REG_SLIMBUS_CFG */

    uint32_t sb_la_offset;
    /**< Codec register SLIMbus logical address offset.

         @values Valid uint32 number */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_cdc_reg_slimbus_cfg_param_v1_t */

/** ID of the parameter used by #AFE_MODULE_CDC_DEV_CFG to configure codec
    AANC.

    Only #AFE_SVC_CMD_SET_PARAM (deprecated) and #AFE_SVC_CMD_SET_PARAM_V2 can
    use this parameter ID.

    @apr_msgpayload{afe_cdc_aanc_version_param_v1_t}
    @table{weak__afe__cdc__aanc__version__param__v1__t}
 */
#define AFE_PARAM_ID_CDC_AANC_VERSION                        0x0001023A

/** Version information used to handle future additions to
    AFE_PARAM_ID_CDC_AANC_VERSION processing (for backward compatibility).
 */
#define AFE_API_VERSION_CDC_AANC_VERSION                            0x1

/** @} */ /* end_addtogroup afe_module_cdc_device */


/** @ingroup afe_service_api_deprecated
    Deprecated. Use #AFE_AANC_HW_BLOCK_VERSION_2.

    AANC hardware block version 1.

    Applications must select this version when the
    #AFE_CRC_AANC_LPF_FF_FB, #AFE_CRC_AANC_LPF_COEFF_MSB, and
    #AFE_CRC_AANC_LPF_COEFF_LSB registers are used for gain updates
    (such as the WCD9310 codec). @newpage
 */
#define AFE_AANC_HW_BLOCK_VERSION_1                                 0x1


/** @addtogroup afe_module_cdc_device
@{ */
/** AANC hardware block version 2.

    Applications must select this version when the
    #AFE_CRC_AANC_FF_GAIN_ADAPTIVE, #AFE_CRC_AANC_FFGAIN_ADAPTIVE_EN,
    and #AFE_CRC_AANC_GAIN_CONTROL registers are used for gain updates
    (such as the WCD9320, WCD9330 and WCD9335 codecs). @newpage
 */
#define AFE_AANC_HW_BLOCK_VERSION_2                                 0x2

typedef struct afe_cdc_aanc_version_param_v1_t afe_cdc_aanc_version_param_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_cdc_aanc_version_param_v1_t
@{ */
/* Configuration structure for the AFE_PARAM_ID_CDC_AANC_VERSION parameter
  (version 1).
 */
struct afe_cdc_aanc_version_param_v1_t
{
    uint32_t cdc_aanc_minor_version;
    /**< Tracks the version for this AANC parameter.

         @values #AFE_API_VERSION_CDC_AANC_VERSION */

    uint32_t aanc_hw_version;
    /**< Type of AANC hardware version present in the codec.

         @values
         - #AFE_AANC_HW_BLOCK_VERSION_1
         - #AFE_AANC_HW_BLOCK_VERSION_2 @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_cdc_aanc_version_param_v1_t */


/** ID of the parameter used by #AFE_MODULE_CDC_DEV_CFG to configure
    clip bank select.

    Only #AFE_SVC_CMD_SET_PARAM (deprecated) and #AFE_SVC_CMD_SET_PARAM_V2 can
    use this parameter ID.

    @apr_msgpayload{afe_clip_bank_sel_cfg_v1_t}
    @table{weak__afe__clip__bank__sel__cfg__v1__t}
 */
#define AFE_PARAM_ID_CLIP_BANK_SEL_CFG                        0x00010242

/** Version information used to handle future additions to
    AFE_PARAM_ID_CLIP_BANK_SEL_CFG processing (for backward compatibility).
 */
#define AFE_API_VERSION_CLIP_BANK_SEL_CFG                            0x1

/** Maximum number of banks allowed for reading the PCM levels in the event of
    a clip interrupt. @newpage
 */
#define AFE_CLIP_MAX_BANKS                                             0x4

typedef struct afe_clip_bank_sel_cfg_v1_t afe_clip_bank_sel_cfg_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_clip_bank_sel_cfg_v1_t
@{ */
/* Configuration structure for the
  AFE_PARAM_ID_CLIP_BANK_SEL_CFG parameter (version 1).
 */
struct afe_clip_bank_sel_cfg_v1_t
{
    uint32_t clip_bank_sel_cfg_minor_version;
    /**< Tracks the configuration of this parameter.

         @values #AFE_API_VERSION_CLIP_BANK_SEL_CFG */

    uint32_t num_banks;
    /**< Number of banks to be read.

         @values 2 or 4 */

    uint32_t bank_map[AFE_CLIP_MAX_BANKS];
    /**< Bank number to be read.

         If the number of banks is 2, the bank map must be
         [1, 3, DC, DC] or [0, 2, DC, DC], where DC means ``do not care.''

         @values 0 to 3 */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_clip_bank_sel_cfg_v1_t */

/** ID of the parameter used by #AFE_MODULE_CDC_DEV_CFG to initialize codec
    device configuration.

    Only #AFE_SVC_CMD_SET_PARAM (deprecated) and #AFE_SVC_CMD_SET_PARAM_V2 can
    use this parameter ID.

    This parameter must be sent to the aDSP after sending all device
    configuration parameters so that initialization takes affect in all
    configurations.

    This parameter does not contain an explict payload and must be sent with
    the AFE_MODULE_CDC_DEV_CFG module.
 */
#define AFE_PARAM_ID_CDC_REG_CFG_INIT                            0x00010237

/** ID of the parameter used by #AFE_MODULE_CDC_DEV_CFG to configure the
    paging scheme for codec register access.

    Only #AFE_SVC_CMD_SET_PARAM (deprecated) and #AFE_SVC_CMD_SET_PARAM_V2 can
    use this parameter ID.

    @apr_msgpayload{afe_cdc_reg_page_cfg_t}
    @table{weak__afe__cdc__reg__page__cfg__t}
 */
#define AFE_PARAM_ID_CDC_REG_PAGE_CFG                            0x00010296

/** Version information used to handle future additions to
    AFE_PARAM_ID_CDC_REG_PAGE_CFG processing (for backward compatibility).
 */
#define AFE_API_VERSION_CDC_REG_PAGE_CFG 0x1

/* Enumeration for ADSP processor id assignment in Codec Page Configuration */

/** Assign codec page processor ID 0 to the aDSP. */
#define AFE_CDC_REG_PAGE_ASSIGN_PROC_ID_0 0x0

/** Assign codec page processor ID 1 to the aDSP. */
#define AFE_CDC_REG_PAGE_ASSIGN_PROC_ID_1 0x1

/** Assign codec page processor ID 2 to the aDSP. */
#define AFE_CDC_REG_PAGE_ASSIGN_PROC_ID_2 0x2

/** Assign codec page processor ID 3 to the aDSP. @newpage */
#define AFE_CDC_REG_PAGE_ASSIGN_PROC_ID_3 0x3

typedef struct afe_cdc_reg_page_cfg_t afe_cdc_reg_page_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_cdc_reg_page_cfg_t
@{ */
/* Codec Register Paging scheme configuration structure used by the
      AFE_PARAM_ID_CDC_REG_CFG parameter (version 1).
 */
struct afe_cdc_reg_page_cfg_t
{
   uint32_t minor_version;
   /**< Tracks the configuration of this parameter.

        @values #AFE_API_VERSION_CDC_REG_PAGE_CFG */

    uint32_t enable;
    /**< Flag that specifies whether paging configuration is enabled.

         @values
         - 0 -- FALSE; disable paging
         - 1 -- TRUE; enable paging @tablebulletend */

    uint32_t proc_id;
    /**< Processor ID for the aDSP to use.

         To avoid contention, ensure that unique processors are assigned unique
         IDs.

         @note1hang In Fusion architecture, ensure that each aDSP is assigned
                    a unique ID.

         @values
         - #AFE_CDC_REG_PAGE_ASSIGN_PROC_ID_0
         - #AFE_CDC_REG_PAGE_ASSIGN_PROC_ID_1
         - #AFE_CDC_REG_PAGE_ASSIGN_PROC_ID_2
         - #AFE_CDC_REG_PAGE_ASSIGN_PROC_ID_3 @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_cdc_reg_page_cfg_t */

/** ID of the parameter used by #AFE_MODULE_CDC_DEV_CFG to configure the
    power state control for the codec.

    Only #AFE_SVC_CMD_SET_PARAM (deprecated) and #AFE_SVC_CMD_SET_PARAM_V2 can
    use this parameter ID.

    @apr_msgpayload{afe_cdc_pwr_cntl_cfg_v1_t}
    @table{weak__afe__cdc__pwr__cntl__cfg__v1__t}
 */
#define AFE_PARAM_ID_CDC_PWR_CNTL_CFG                            0x000102A9

/** Version information used to handle future additions to
    AFE_PARAM_ID_CDC_PWR_CNTL_CFG processing (for backward compatibility).
 */
#define AFE_API_VERSION_CDC_PWR_CNTL_CFG 0x1

typedef struct afe_cdc_pwr_cntl_cfg_v1_t afe_cdc_pwr_cntl_cfg_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_cdc_pwr_cntl_cfg_v1_t
@{ */
/* Codec power state control configuration structure used by the
      AFE_PARAM_ID_CDC_PWR_CNTL_CFG parameter (version 1).
 */
struct afe_cdc_pwr_cntl_cfg_v1_t
{
  uint32_t minor_version;
  /**< Minor version used to track codec power state control
       configuration.

       @values #AFE_API_VERSION_CDC_PWR_CNTL_CFG */

  uint32_t enable;
  /**< Specifies whether the aDSP can control the codec power state.

       @values
        - 0 -- FALSE; disable the codec power state control from the aDSP
        - 1 -- TRUE; enable enable the codec power state control from aDSP
        @tablebulletend */

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_cdc_pwr_cntl_cfg_v1_t */

/** @} */ /* end_addtogroup afe_module_cdc_device */


/** @addtogroup afe_mod_param_id_deprecated
@{ */
/** Deprecated. Use #AFE_MODULE_CLOCK_SET.

   ID of the parameter used by #AFE_MODULE_AUDIO_DEV_INTERFACE to configure
    a low power audio interface (LPAIF) clock (I2S/PCM bit clock and OSR).
    - An LPAIF clock must be enabled before #AFE_PORT_CMD_DEVICE_START
      is issued.
    - An LPAIF clock must be disabled after #AFE_PORT_CMD_DEVICE_STOP
      is issued.

    Only #AFE_PORT_CMD_SET_PARAM_V2 (deprecated) and #AFE_PORT_CMD_SET_PARAM_V3
    can use this parameter ID.

    @apr_msgpayload{afe_param_id_lpaif_clk_cfg_t}
    The payload for this parameter must be sent with
    #AFE_MODULE_AUDIO_DEV_INTERFACE.
    @tablens{weak__afe__param__id__lpaif__clk__cfg__t}
 */
#define AFE_PARAM_ID_LPAIF_CLK_CONFIG                0x00010238

/** Deprecated. Version information used to handle future additions to
    AFE_PARAM_ID_LPAIF_CLK_CONFIG processing (for backward compatibility).
 */
#define AFE_API_VERSION_LPAIF_CLK_CONFIG               0x1

/* Enumeration for setting LPAIF clock source clk_src. */

/** Deprecated. LPAIF clock source is external. */
#define AFE_PORT_LPACLK_CLK_SRC_EXTERNAL                                     0x0

/** Deprecated. LPAIF clock source is internal. */
#define AFE_PORT_LPACLK_CLK_SRC_INTERNAL                                     0x1

/* Enumeration for setting LPAIF clock root clk_root. */

/** Deprecated. LPAIF clock is sourced from the audio phase locked loop (PLL). */
#define AFE_PORT_LPACLK_CLK_ROOT_DEFAULT                                     0x0

/** Deprecated. LPAIF clock is sourced from the PXO. */
#define AFE_PORT_LPACLK_CLK_ROOT_PXO                                         0x1

/** Deprecated. LPAIF clock is sourced from the CXO. */
#define AFE_PORT_LPACLK_CLK_ROOT_CXO                                         0x2

/** Deprecated. LPAIF clock is sourced from the LPA PLL. */
#define AFE_PORT_LPACLK_CLK_ROOT_LPAPLL                                      0x3

/** Deprecated. LPAIF clock is sourced from the second PLL. */
#define AFE_PORT_LPACLK_CLK_ROOT_SECPLL                                      0x4

/** Deprecated. LPAIF clock is sourced from the LPA QDSP6 PLL. */
#define AFE_PORT_LPACLK_CLK_ROOT_LPAQ6PLL                                    0x5


/* Enumeration for specifying value1 and value2 valid setting via clk_set_mode.*/

/** Deprecated. Both clk_value1 and clk_value2 in afe_param_id_lpaif_clk_cfg_t are invalid. */
#define AFE_PORT_LPACLK_CLK_VALUE_INVALID                                    0x0

/** Deprecated. Only clk_value1 in afe_param_id_lpaif_clk_cfg_t is valid. */
#define AFE_PORT_LPACLK_CLK_VALUE1_VALID_ONLY                                0x1

/** Deprecated. Only clk_value2 in afe_param_id_lpaif_clk_cfg_t is valid. */
#define AFE_PORT_LPACLK_CLK_VALUE2_VALID_ONLY                                0x2

/** Deprecated. Both clk_value1 and clk_value2 in afe_param_id_lpaif_clk_cfg_t are valid. @newpage */
#define AFE_PORT_LPACLK_CLK_VALUE1_VALUE2_BOTH_VALID                         0x3

typedef struct afe_param_id_lpaif_clk_cfg_t afe_param_id_lpaif_clk_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_lpaif_clk_cfg_t
@{ */
/* Payload of the AFE_PARAM_ID_LPAIF_CLK_CONFIG parameter.
 */
struct afe_param_id_lpaif_clk_cfg_t
{
    uint32_t                  lpaif_cfg_minor_version;
    /**< Tracks the configuration of this parameter.

         @values #AFE_API_VERSION_LPAIF_CLK_CONFIG */

    uint32_t                 clk_value1;
    /**< Clock frequency value 1 (in Hertz) to be set.

         @values
         - I2S bit clock for an MI2S port, or PCM bit clock for a PCM port
         - 0 to disable the clock @tablebulletend */

    uint32_t                 clk_value2;
    /**< Clock frequency value 2 (in Hertz) to be set.

         @values
         - I2S OSR for an MI2S port
         - 0 to disable the clock @tablebulletend */

    uint16_t                 clk_src;
    /**< Specifies I2S WS or PCM SYNC as the clock source.

         @values
         - #AFE_PORT_LPACLK_CLK_SRC_EXTERNAL
         - #AFE_PORT_LPACLK_CLK_SRC_INTERNAL @tablebulletend */

    uint16_t                 clk_root;
    /**< Specifies I2S or PCM as the root clock source.
         Currently, only AFE_PORT_LPACLK_CLK_ROOT_DEFAULT is valid.

         @values
         - #AFE_PORT_LPACLK_CLK_ROOT_DEFAULT
         - #AFE_PORT_LPACLK_CLK_ROOT_PXO
         - #AFE_PORT_LPACLK_CLK_ROOT_CXO
         - #AFE_PORT_LPACLK_CLK_ROOT_LPAPLL
         - #AFE_PORT_LPACLK_CLK_ROOT_SECPLL
         - #AFE_PORT_LPACLK_CLK_ROOT_LPAQ6PLL @tablebulletend */

    uint16_t                 clk_set_mode;
    /**< Valid setting for clk_value1 and clk_value2 fields.

           @values
           - #AFE_PORT_LPACLK_CLK_VALUE_INVALID
           - #AFE_PORT_LPACLK_CLK_VALUE1_VALID_ONLY
           - #AFE_PORT_LPACLK_CLK_VALUE2_VALID_ONLY
           - #AFE_PORT_LPACLK_CLK_VALUE1_VALUE2_BOTH_VALID @vertspace{-28} */

    uint16_t                 reserved;
    /**< This field must be set to 0. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_param_id_lpaif_clk_cfg_t */


/** Deprecated. Use #AFE_MODULE_CLOCK_SET.

    ID of the parameter used by #AFE_MODULE_AUDIO_DEV_INTERFACE to configure
    an SPDIF clock.
    - An SPDIF clock must be enabled before #AFE_PORT_CMD_DEVICE_START
      is issued.
    - An SPDIF clock must be disabled after #AFE_PORT_CMD_DEVICE_STOP
      is issued.

    @apr_msgpayload{afe_param_id_spdif_clk_cfg_t}
    The payload for this parameter must be sent with
    #AFE_MODULE_AUDIO_DEV_INTERFACE.
    @tablens{weak__afe__param__id__spdif__clk__cfg__t}
 */
#define AFE_PARAM_ID_SPDIF_CLK_CONFIG                0x00010246

/** Deprecated. Version information used to handle future additions to
    AFE_PARAM_ID_SPDIF_CLK_CONFIG processing (for backward compatibility).
 */
#define AFE_API_VERSION_SPDIF_CLK_CONFIG               0x1

/* Enumeration for setting clock root clk_root. */

/** Deprecated. Clock is sourced from the PXO. */
#define AFE_PORT_CLK_ROOT_PXO                                         0x1

/** Deprecated. Clock is sourced from the CXO. */
#define AFE_PORT_CLK_ROOT_CXO                                         0x2

/** Deprecated. Clock is sourced from the LPA PLL. */
#define AFE_PORT_CLK_ROOT_LPAPLL                                      0x3

/** Deprecated. Clock is sourced from the LPA QDSP6 PLL. @newpage */
#define AFE_PORT_CLK_ROOT_LPAQ6PLL                                    0x4

typedef struct afe_param_id_spdif_clk_cfg_t afe_param_id_spdif_clk_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_spdif_clk_cfg_t
@{ */
/* Payload of the AFE_PARAM_ID_SPDIF_CLK_CONFIG parameter.
 */
struct afe_param_id_spdif_clk_cfg_t
{
    uint32_t                  spdif_clk_cfg_minor_version;
    /**< Tracks the configuration of this parameter.

         @values #AFE_API_VERSION_SPDIF_CLK_CONFIG */

    uint32_t                 clk_value;
    /**< Clock frequency value 1 (in Hertz) to be set as twice the bit rate.

         - 2 (biphase) * 32 (60958 subframe size) * sampling rate * 2 
            (channels A and B)
         - 0 to disable the clock @tablebulletend */

    uint32_t                 clk_root;
    /**< Specifies I2S or PCM as the root clock source.

         @values
         - #AFE_PORT_CLK_ROOT_LPAPLL
         - #AFE_PORT_CLK_ROOT_LPAQ6PLL  @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_param_id_spdif_clk_cfg_t */


/** Deprecated.

    ID of the parameter used by #AFE_MODULE_AUDIO_DEV_INTERFACE to configure
    the internal digital codec core clock.

    The call sequence for this clock is:
    -# If the client selects MI2S OSR as the internal digital core clock
       source, enable the MI2S OSR clock.
    -# Enable the internal digital core clock.
    -# The clock is running.
    -# Disable the internal digital core clock.
    -# If the client selects MI2S OSR as the internal digital core clock
       source, disable the MI2S OSR clock.

    Only #AFE_PORT_CMD_SET_PARAM_V2 (deprecated) and #AFE_PORT_CMD_SET_PARAM_V3
    can use this parameter ID.

    @apr_msgpayload{afe_param_id_internal_digital_cdc_clk_cfg_t}
    The payload must be sent with #AFE_MODULE_AUDIO_DEV_INTERFACE.
    @tablens{weak__afe__param__id__internal__digital__cdc__clk__cfg__t}
 */
#define AFE_PARAM_ID_INTERNAL_DIGIATL_CDC_CLK_CONFIG                0x00010239

/** Deprecated. Version information used to handle future additions to
    AFE_PARAM_ID_INTERNAL_DIGIATL_CDC_CLK_CONFIG processing (for backward
    compatibility).
 */
#define AFE_API_VERSION_INTERNAL_DIGIATL_CDC_CLK_CONFIG               0x1

/* Enumerations for setting digital codec core clock root. */

/** Deprecated. Digital codec core clock root is invalid. */
#define AFE_PORT_INTERNAL_DIGIATL_CDC_CLK_ROOT_INVALID                  0x0

/** Deprecated. Digital codec core clock root is Primary I2S OSR. */
#define AFE_PORT_INTERNAL_DIGIATL_CDC_CLK_ROOT_PRII2SOSR                0x1

/** Deprecated. Digital codec core clock root is Secondary I2S OSR. */
#define AFE_PORT_INTERNAL_DIGIATL_CDC_CLK_ROOT_SECI2SOSR                0x2

/** Deprecated. Digital codec core clock root is Tertiary I2S OSR. */
#define AFE_PORT_INTERNAL_DIGIATL_CDC_CLK_ROOT_TERI2SOSR                0x3

/** Deprecated. Digital codec core clock root is Quaternary I2S OSR. */
#define AFE_PORT_INTERNAL_DIGIATL_CDC_CLK_ROOT_QUAI2SOSR                0x4

/** Deprecated. Digital codec core clock root is Codec. @newpage */
#define AFE_PORT_INTERNAL_DIGIATL_CDC_CLK_ROOT_CODEC                    0x5

typedef struct afe_param_id_internal_digital_cdc_clk_cfg_t afe_param_id_internal_digital_cdc_clk_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_internal_digital_cdc_clk_cfg_t
@{ */
/* Payload of the AFE_PARAM_ID_INTERNAL_DIGIATL_CDC_CLK_CONFIG command's
    internal digital codec core clock configuration parameter.
 */
struct afe_param_id_internal_digital_cdc_clk_cfg_t
{
    uint32_t                  cdc_clk_cfg_minor_version;
    /**< Tracks the configuration of this parameter.

         @values #AFE_API_VERSION_INTERNAL_DIGIATL_CDC_CLK_CONFIG */

    uint32_t                 clk_value;
    /**< Digital codec core clock frequency in Hertz. */

    uint16_t                 clk_root;
    /**< Source of the digital codec core clock.

         @values
         - #AFE_PORT_INTERNAL_DIGIATL_CDC_CLK_ROOT_INVALID
         - #AFE_PORT_INTERNAL_DIGIATL_CDC_CLK_ROOT_PRII2SOSR
         - #AFE_PORT_INTERNAL_DIGIATL_CDC_CLK_ROOT_SECI2SOSR
         - #AFE_PORT_INTERNAL_DIGIATL_CDC_CLK_ROOT_TERI2SOSR
         - #AFE_PORT_INTERNAL_DIGIATL_CDC_CLK_ROOT_QUAI2SOSR
         - #AFE_PORT_INTERNAL_DIGIATL_CDC_CLK_ROOT_CODEC @vertspace{-28} */

    uint16_t                 reserved;
    /**< This field must be set to 0. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_param_id_internal_digital_cdc_clk_cfg_t */
/** @} */ /* end_addtogroup afe_mod_param_id_deprecated */


/** @addtogroup afe_module_custom_evts
@{ */
/** ID of the Custom Events module, which registers or deregisters an
    event on a specified port or service.

    @apr_hdr_fields
    Opcode -- AFE_MODULE_CUSTOM_EVENTS

    @detdesc
    #AFE_PORT_CMD_SET_PARAM_V2 (deprecated) and #AFE_PORT_CMD_SET_PARAM_V3 use
    this module ID for destination ports.
    #AFE_SVC_CMD_SET_PARAM (deprecated) #AFE_SVC_CMD_SET_PARAM_V2 use this
    module ID for destination services.

    @par
    The APR destination address and port number are obtained from the header of
    this packet and are used for sending different events.
    @par
    By registering, the customer applications will receive
    #AFE_PORT_CUSTOM_EVENT events, depending on the destination.
 */
#define AFE_MODULE_CUSTOM_EVENTS                                    0x00010251

/** Enumeration used to unregister an event. */
#define AFE_CUSTOM_EVENT_UNREGISTER                                 0x0

/** Enumeration used to register an event. */
#define AFE_CUSTOM_EVENT_REGISTER                                   0x1

/** @} */ /* end_addtogroup afe_module_custom_evts */


/** @addtogroup afe_evt_port_custom_event
@{ */
/** Sent when the AFE port reports an event to a client.

  @apr_hdr_fields
    Opcode -- AFE_PORT_CUSTOM_EVENT

  @apr_msgpayload{afe_port_custom_event_t}
    @table{weak__afe__port__custom__event__t}

  @detdesc
    This event communicates information about the AFE port event for which the 
    client registered.
    @par
    To receive this notification, a client must register using
    #AFE_PORT_CMD_SET_PARAM_V2 (deprecated) or #AFE_PORT_CMD_SET_PARAM_V3,
    with #AFE_MODULE_CUSTOM_EVENTS and a corresponding parameter ID.

  @return
    None.

  @dependencies
    #AFE_PORT_CMD_SET_PARAM_V3
 */
#define AFE_PORT_CUSTOM_EVENT                                         0x00010107

/* Structure for the AFE_PORT_CUSTOM_EVENT event. */
typedef struct afe_port_custom_event_t  afe_port_custom_event_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_port_custom_event_t
@{ */
/* Payload of the AFE_PORT_CUSTOM_EVENT message, which sends
    an event from the AFE port to a registered client.
 */
struct afe_port_custom_event_t
{
   uint32_t                  port_id;
   /**< ID of the AFE port from which the event is generated.

        @values Valid AFE port ID */

   uint32_t                  module_id;
   /**< Valid ID of the module.

        @values #AFE_MODULE_CUSTOM_EVENTS */

   uint32_t                  param_id;
   /**< Valid parameter ID of the event. */

   uint16_t event_type;
   /**< Valid event type corresponding to the param_id value. */

   uint16_t payload_size;
   /**< Actual size of the event payload.

        @values
        - Any valid payload size
        - 0 -- There is no payload @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_port_custom_event_t */
/** @} */ /* end_addtogroup afe_evt_port_custom_event */


/** @addtogroup afe_module_feedback
@{ */
/** ID of the Feedback module, which is used to set up the feedback path
    between modules on different AFE ports for communicating custom data.
    To use this framework, these modules must support custom data
    communication.

    This module supports the following parameter ID:
    - #AFE_PARAM_ID_FEEDBACK_PATH_CFG

    @apr_hdr_fields
    Opcode -- AFE_MODULE_FEEDBACK
 */
#define AFE_MODULE_FEEDBACK                                     0x00010257

/** ID of the parameter used by #AFE_MODULE_FEEDBACK to configure
    a feedback connection between modules on different AFE ports.

    Only #AFE_PORT_CMD_SET_PARAM_V2 (deprecated) and #AFE_PORT_CMD_SET_PARAM_V3
    can use this parameter ID.

    @apr_msgpayload{afe_fbsp_feedback_path_cfg_v1_t}
    @table{weak__afe__fbsp__feedback__path__cfg__v1__t}
 */
#define AFE_PARAM_ID_FEEDBACK_PATH_CFG                         0x0001022C

/** Version information used to handle future additions to configuration of
    the feedback connection between modules on different AFE ports
    (for backward compatibility).
 */
#define AFE_API_VERSION_FEEDBACK_PATH_CFG                      0x1

/** Vsens comes from the left speaker. */
#define AFE_FBSP_VSENS_LEFT_CHAN          1

/** Isens comes from the left speaker. */
#define AFE_FBSP_ISENS_LEFT_CHAN          2

/** Vsens comes from the right speaker. */
#define AFE_FBSP_VSENS_RIGHT_CHAN            3

/** Isens comes from the right speaker. */
#define AFE_FBSP_ISENS_RIGHT_CHAN            4

/** Maximum number of Vsens/Isens channels supported. @newpage */
#define AFE_FBSP_VI_CHAN_MAX              4

typedef struct afe_fbsp_feedback_path_cfg_v1_t afe_fbsp_feedback_path_cfg_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_fbsp_feedback_path_cfg_v1_t
@{ */
/* Configuration structure for the
    AFE_PARAM_ID_FEEDBACK_PATH_CFG parameter (version 1).
 */
struct afe_fbsp_feedback_path_cfg_v1_t
{
   uint32_t       feedback_path_cfg_minor_version;
   /**< Tracks the configuration of this parameter.

        @values #AFE_API_VERSION_FEEDBACK_PATH_CFG */

   int32_t        dst_portid;
   /**< Destination Rx port ID.

        @values See <i>Supported Hardware Ports</i> in Appendix
        @xref{chp:portIDs}. */

   int32_t        num_channels;
   /**< Number of Tx port channels. Each Vsens and Isens is considered an
        individual channel.

        @values
        - 2 -- Mono speaker
        - 4 -- Stereo speakers @tablebulletend */

   int32_t        chan_info[AFE_FBSP_VI_CHAN_MAX];
   /**< Channel mapping array that provides information on the order in
        which the Vsens and Isens of different speakers come into a Tx port.

        @values
        - #AFE_FBSP_VSENS_LEFT_CHAN
        - #AFE_FBSP_ISENS_LEFT_CHAN
        - #AFE_FBSP_VSENS_RIGHT_CHAN
        - #AFE_FBSP_ISENS_RIGHT_CHAN

        Some possible configurations are (1,2,3,4), (4,2,1,3), (1,2,0,0),
        (4,3,0,0), and (2,1,0,0).

        All channels must be unique. The order does not matter as long as the
        channels are valid.
        If only two channels are sent (V and I), they must be first two channels:
        (4,3,0,0), (1,2,0,0), etc. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_fbsp_feedback_path_cfg_v1_t */
/** @} */ /* end_addtogroup afe_module_feedback */


/** @addtogroup afe_module_device_group
@{ */
/** ID of the Group Device module, which groups AFE ports that are in the
    same physical device interface (for example, secondary MI2S Rx).

    This module supports following parameter IDs:
     - #AFE_PARAM_ID_GROUP_DEVICE_I2S_CONFIG
     - #AFE_PARAM_ID_GROUP_DEVICE_ENABLE

    To configure the group device (a single device that operates like multiple
    devices), use #AFE_SVC_CMD_SET_PARAM (deprecated) or
    #AFE_SVC_CMD_SET_PARAM_V2 and fill the module ID with the respective
    parameter ID.

    @apr_hdr_fields
    Opcode -- AFE_MODULE_GROUP_DEVICE
 */
#define AFE_MODULE_GROUP_DEVICE                                      0x00010254

/** ID of the parameter used by #AFE_MODULE_GROUP_DEVICE to configure the
    group device.

    Only #AFE_SVC_CMD_SET_PARAM (deprecated) and #AFE_SVC_CMD_SET_PARAM_V2 can
    use this parameter ID.

    Requirements:
    - Configure the group before the member ports in the group are configured
      and started.
    - Enable the group only after it is configured.
    - Stop all member ports in the group before disabling the group.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_GROUP_DEVICE_I2S_CONFIG

    @apr_msgpayload{afe_param_id_group_device_i2s_cfg_v1_t}
    @table{weak__afe__param__id__group__device__i2s__cfg__v1__t}
*/
#define AFE_PARAM_ID_GROUP_DEVICE_I2S_CONFIG                              0x00010286

/** Version information used to handle future additions to
    AFE_PARAM_ID_GROUP_DEVICE_I2S_CONFIG processing (for backward compatibility).
 */
#define AFE_API_VERSION_GROUP_DEVICE_I2S_CONFIG                           0x1

/** Number of AFE ports in the group device. @newpage */
#define AFE_GROUP_DEVICE_NUM_PORTS                                    8

typedef struct afe_param_id_group_device_i2s_cfg_v1_t afe_param_id_group_device_i2s_cfg_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_group_device_i2s_cfg_v1_t
@{ */
/* Payload of the AFE_PARAM_ID_GROUP_DEVICE_I2S_CONFIG parameter ID
    used by AFE_MODULE_GROUP_DEVICE.
 */
struct afe_param_id_group_device_i2s_cfg_v1_t
{
    uint32_t                  minor_version;
   /**< Tracks the configuration of this parameter.

        @values #AFE_API_VERSION_GROUP_DEVICE_I2S_CONFIG */

    uint16_t                  group_id;
   /**< ID for the group device.

        @values
        - #AFE_GROUP_DEVICE_ID_PRIMARY_MI2S_RX
        - #AFE_GROUP_DEVICE_ID_PRIMARY_MI2S_TX
        - #AFE_GROUP_DEVICE_ID_QUATERNARY_MI2S_RX
        - #AFE_GROUP_DEVICE_ID_QUATERNARY_MI2S_TX @tablebulletend */

    uint16_t                  channel_mode;
    /**< I2S lines and multichannel operation.

         @values
         - #AFE_PORT_I2S_SD0
         - #AFE_PORT_I2S_SD1
         - #AFE_PORT_I2S_SD2
         - #AFE_PORT_I2S_SD3
         - #AFE_PORT_I2S_QUAD01
         - #AFE_PORT_I2S_QUAD23
         - #AFE_PORT_I2S_6CHS
         - #AFE_PORT_I2S_8CHS @tablebulletend */

    uint32_t                  sample_rate;
    /**< Sampling rate of the group device.

         @values
         - #AFE_PORT_SAMPLE_RATE_8K
         - #AFE_PORT_SAMPLE_RATE_16K
         - #AFE_PORT_SAMPLE_RATE_24K
         - #AFE_PORT_SAMPLE_RATE_32K
         - #AFE_PORT_SAMPLE_RATE_48K
         - #AFE_PORT_SAMPLE_RATE_96K
         - #AFE_PORT_SAMPLE_RATE_192K @tablebulletend */

    uint16_t                  port_id[AFE_GROUP_DEVICE_NUM_PORTS];
    /**< Array of member port IDs in this group.

         @values group_id = AFE_GROUP_DEVICE_ID_PRIMARY_MI2S_RX
         - #AFE_PORT_ID_PRIMARY_MI2S_RX
         - #AFE_PORT_ID_PRIMARY_MI2S_RX_1
         - #AFE_PORT_ID_PRIMARY_MI2S_RX_2
         - #AFE_PORT_ID_PRIMARY_MI2S_RX_3

         @values group_id = AFE_GROUP_DEVICE_ID_PRIMARY_MI2S_TX
         - #AFE_PORT_ID_PRIMARY_MI2S_TX
         - #AFE_PORT_ID_PRIMARY_MI2S_TX_1
         - #AFE_PORT_ID_PRIMARY_MI2S_TX_2
         - #AFE_PORT_ID_PRIMARY_MI2S_TX_3

         @values group_id = AFE_GROUP_DEVICE_ID_QUATERNARY_MI2S_RX
         - #AFE_PORT_ID_QUATERNARY_MI2S_RX
         - #AFE_PORT_ID_QUATERNARY_MI2S_RX_1
         - #AFE_PORT_ID_QUATERNARY_MI2S_RX_2
         - #AFE_PORT_ID_QUATERNARY_MI2S_RX_3

         @contcell
         @values group_id = AFE_GROUP_DEVICE_ID_PRIMARY_MI2S_TX
         - #AFE_PORT_ID_QUATERNARY_MI2S_TX
         - #AFE_PORT_ID_QUATERNARY_MI2S_TX_1
         - #AFE_PORT_ID_QUATERNARY_MI2S_TX_2
         - #AFE_PORT_ID_QUATERNARY_MI2S_TX_3

         @values for unused location -- #AFE_PORT_ID_INVALID

         Fill these values from index 0. Set an unused index to
         AFE_PORT_ID_INVALID. */

    uint16_t                  bit_width;
    /**< Bit width of the I2S sample.

         @values 16, 24, 32 */

    uint16_t reserved;
    /**< This field must be set to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_param_id_group_device_i2s_cfg_t */

/** ID of the parameter used by #AFE_MODULE_GROUP_DEVICE to configure the
    group device.

    Only #AFE_SVC_CMD_SET_PARAM (deprecated) and #AFE_SVC_CMD_SET_PARAM_V2 can
    use this parameter ID.

    Requirements:
    - Configure the group before the member ports in the group are configured
      and started.
    - Enable the group only after it is configured.
    - Stop all member ports in the group before disabling the group.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_GROUP_DEVICE_TDM_CONFIG

    @apr_msgpayload{afe_param_id_group_device_tdm_cfg_t}
    @table{weak__afe__param__id__group__device__tdm__cfg__t}
 */
#define AFE_PARAM_ID_GROUP_DEVICE_TDM_CONFIG                              0x0001029E

/** Version information used to handle future additions to
    AFE_PARAM_ID_GROUP_DEVICE_TDM_CONFIG processing (for backward compatibility).
 */
#define AFE_API_VERSION_GROUP_DEVICE_TDM_CONFIG                           0x1

typedef struct afe_param_id_group_device_tdm_cfg_t afe_param_id_group_device_tdm_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_group_device_tdm_cfg_t
@{ */
/* Payload of the AFE_PARAM_ID_GROUP_DEVICE_TDM_CONFIG parameter ID
    used by AFE_MODULE_GROUP_DEVICE.
 */
struct afe_param_id_group_device_tdm_cfg_t
{
   uint32_t                  group_device_cfg_minor_version;
   /**< Tracks the configuration of this parameter.

        @values #AFE_API_VERSION_GROUP_DEVICE_TDM_CONFIG */

   uint16_t                  group_id;
   /**< ID for the group device.

        @values
        - #AFE_GROUP_DEVICE_ID_PRIMARY_TDM_RX
        - #AFE_GROUP_DEVICE_ID_PRIMARY_TDM_TX
        - #AFE_GROUP_DEVICE_ID_SECONDARY_TDM_RX
        - #AFE_GROUP_DEVICE_ID_SECONDARY_TDM_TX
        - #AFE_GROUP_DEVICE_ID_TERTIARY_TDM_RX
        - #AFE_GROUP_DEVICE_ID_TERTIARY_TDM_TX
        - #AFE_GROUP_DEVICE_ID_QUATERNARY_TDM_RX
        - #AFE_GROUP_DEVICE_ID_QUATERNARY_TDM_TX @tablebulletend */

   uint16_t                  reserved;
    /**< This field must be set to 0. */

    uint16_t                  port_id[AFE_GROUP_DEVICE_NUM_PORTS];
    /**< Array of member port IDs in this group.

         @values group_id = AFE_GROUP_DEVICE_ID_PRIMARY_TDM_RX
         - #AFE_PORT_ID_PRIMARY_TDM_RX
         - #AFE_PORT_ID_PRIMARY_TDM_RX_1
         - #AFE_PORT_ID_PRIMARY_TDM_RX_2
         - #AFE_PORT_ID_PRIMARY_TDM_RX_3
         - #AFE_PORT_ID_PRIMARY_TDM_RX_4
         - #AFE_PORT_ID_PRIMARY_TDM_RX_5
         - #AFE_PORT_ID_PRIMARY_TDM_RX_6
         - #AFE_PORT_ID_PRIMARY_TDM_RX_7

         @values group_id = AFE_GROUP_DEVICE_ID_PRIMARY_TDM_TX
         - #AFE_PORT_ID_PRIMARY_TDM_TX
         - #AFE_PORT_ID_PRIMARY_TDM_TX_1
         - #AFE_PORT_ID_PRIMARY_TDM_TX_2
         - #AFE_PORT_ID_PRIMARY_TDM_TX_3
         - #AFE_PORT_ID_PRIMARY_TDM_TX_4
         - #AFE_PORT_ID_PRIMARY_TDM_TX_5
         - #AFE_PORT_ID_PRIMARY_TDM_TX_6
         - #AFE_PORT_ID_PRIMARY_TDM_TX_7

         @values group_id = AFE_GROUP_DEVICE_ID_SECONDARY_TDM_RX
         - #AFE_PORT_ID_SECONDARY_TDM_RX
         - #AFE_PORT_ID_SECONDARY_TDM_RX_1
         - #AFE_PORT_ID_SECONDARY_TDM_RX_2
         - #AFE_PORT_ID_SECONDARY_TDM_RX_3
         - #AFE_PORT_ID_SECONDARY_TDM_RX_4
         - #AFE_PORT_ID_SECONDARY_TDM_RX_5
         - #AFE_PORT_ID_SECONDARY_TDM_RX_6
         - #AFE_PORT_ID_SECONDARY_TDM_RX_7

         @values group_id = AFE_GROUP_DEVICE_ID_SECONDARY_TDM_TX
         - #AFE_PORT_ID_SECONDARY_TDM_TX
         - #AFE_PORT_ID_SECONDARY_TDM_TX_1
         - #AFE_PORT_ID_SECONDARY_TDM_TX_2
         - #AFE_PORT_ID_SECONDARY_TDM_TX_3
         - #AFE_PORT_ID_SECONDARY_TDM_TX_4
         - #AFE_PORT_ID_SECONDARY_TDM_TX_5
         - #AFE_PORT_ID_SECONDARY_TDM_TX_6
         - #AFE_PORT_ID_SECONDARY_TDM_TX_7

         @contcell
         @values group_id = AFE_GROUP_DEVICE_ID_TERTIARY_TDM_RX
         - #AFE_PORT_ID_TERTIARY_TDM_RX
         - #AFE_PORT_ID_TERTIARY_TDM_RX_1
         - #AFE_PORT_ID_TERTIARY_TDM_RX_2
         - #AFE_PORT_ID_TERTIARY_TDM_RX_3
         - #AFE_PORT_ID_TERTIARY_TDM_RX_4
         - #AFE_PORT_ID_TERTIARY_TDM_RX_5
         - #AFE_PORT_ID_TERTIARY_TDM_RX_6
         - #AFE_PORT_ID_TERTIARY_TDM_RX_7

         @values group_id = AFE_GROUP_DEVICE_ID_TERTIARY_TDM_TX
         - #AFE_PORT_ID_TERTIARY_TDM_TX
         - #AFE_PORT_ID_TERTIARY_TDM_TX_1
         - #AFE_PORT_ID_TERTIARY_TDM_TX_2
         - #AFE_PORT_ID_TERTIARY_TDM_TX_3
         - #AFE_PORT_ID_TERTIARY_TDM_TX_4
         - #AFE_PORT_ID_TERTIARY_TDM_TX_5
         - #AFE_PORT_ID_TERTIARY_TDM_TX_6
         - #AFE_PORT_ID_TERTIARY_TDM_TX_7

         @values groupd_id = AFE_GROUP_DEVICE_ID_QUATERNARY_TDM_RX
         - #AFE_PORT_ID_QUATERNARY_TDM_RX
         - #AFE_PORT_ID_QUATERNARY_TDM_RX_1
         - #AFE_PORT_ID_QUATERNARY_TDM_RX_2
         - #AFE_PORT_ID_QUATERNARY_TDM_RX_3
         - #AFE_PORT_ID_QUATERNARY_TDM_RX_4
         - #AFE_PORT_ID_QUATERNARY_TDM_RX_5
         - #AFE_PORT_ID_QUATERNARY_TDM_RX_6
         - #AFE_PORT_ID_QUATERNARY_TDM_RX_7

         @contcell
         @values groupd_id = AFE_GROUP_DEVICE_ID_QUATERNARY_TDM_TX
         - #AFE_PORT_ID_QUATERNARY_TDM_TX
         - #AFE_PORT_ID_QUATERNARY_TDM_TX_1
         - #AFE_PORT_ID_QUATERNARY_TDM_TX_2
         - #AFE_PORT_ID_QUATERNARY_TDM_TX_3
         - #AFE_PORT_ID_QUATERNARY_TDM_TX_4
         - #AFE_PORT_ID_QUATERNARY_TDM_TX_5
         - #AFE_PORT_ID_QUATERNARY_TDM_TX_6
         - #AFE_PORT_ID_QUATERNARY_TDM_TX_7

         @values for unused location -- #AFE_PORT_ID_INVALID */

    uint32_t                  num_channels;
    /**< Number of enabled slots for the TDM frame.

         @values 1 to 16 */

    uint32_t                  sample_rate;
    /**< Sampling rate of the port.

         @values
         - #AFE_PORT_SAMPLE_RATE_8K
         - #AFE_PORT_SAMPLE_RATE_16K
         - #AFE_PORT_SAMPLE_RATE_24K
         - #AFE_PORT_SAMPLE_RATE_32K
         - #AFE_PORT_SAMPLE_RATE_48K @tablebulletend */

    uint32_t                  bit_width;
    /**< Bit width of the sample.

         @values 16, 24, (32) */

    uint16_t                  nslots_per_frame;
    /**< Number of slots per frame.

         @values 1 to 32

         Typical values are 1, 2, 4, 8, 16, and 32. */

    uint16_t                  slot_width;
    /**< Width of the slot in a TDM frame. The slot_width value must be @ge
         the bit_width value.

         @values 16, 24, 32 */

    uint32_t                  slot_mask;
    /**< Position of the active slots. Bits 0 to 31 correspond to slots 0
         to 31.

         @values 1 to 2\^32-1

         When a bit is set, the corresponding slot is active. The number of
         active slots can be inferred from the number of bits set in the mask.

         Only eight individual bits can be enabled. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_param_id_group_device_tdm_cfg_t */

/** ID of the parameter used by #AFE_MODULE_GROUP_DEVICE to enable and disable
    the group device.

    Only #AFE_SVC_CMD_SET_PARAM (deprecated) and #AFE_SVC_CMD_SET_PARAM_V2 can
    use this parameter ID.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_GROUP_DEVICE_ENABLE

    @apr_msgpayload{afe_param_id_group_device_enable_t}
    @table{weak__afe__param__id__group__device__enable__t}
 */
#define AFE_PARAM_ID_GROUP_DEVICE_ENABLE                           0x00010256

typedef struct afe_param_id_group_device_enable_t afe_param_id_group_device_enable_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_group_device_enable_t
@{ */
/* Payload of the AFE_PARAM_ID_GROUP_DEVICE_ENABLE parameter ID used by
    AFE_MODULE_GROUP_DEVICE.
 */
struct afe_param_id_group_device_enable_t
{
   uint16_t                  group_id;
   /**< ID of the group device.

        @values
        - #AFE_GROUP_DEVICE_ID_PRIMARY_MI2S_RX
        - #AFE_GROUP_DEVICE_ID_PRIMARY_MI2S_TX
        - #AFE_GROUP_DEVICE_ID_QUATERNARY_MI2S_RX
        - #AFE_GROUP_DEVICE_ID_QUATERNARY_MI2S_TX
        - #AFE_GROUP_DEVICE_ID_HDMI_MULTISTREAM_RX
        - #AFE_GROUP_DEVICE_ID_PRIMARY_TDM_RX
        - #AFE_GROUP_DEVICE_ID_PRIMARY_TDM_TX
        - #AFE_GROUP_DEVICE_ID_SECONDARY_TDM_RX
        - #AFE_GROUP_DEVICE_ID_SECONDARY_TDM_TX
        - #AFE_GROUP_DEVICE_ID_TERTIARY_TDM_RX
        - #AFE_GROUP_DEVICE_ID_TERTIARY_TDM_TX
        - #AFE_GROUP_DEVICE_ID_QUATERNARY_TDM_RX
        - #AFE_GROUP_DEVICE_ID_QUATERNARY_TDM_TX @tablebulletend */

   uint16_t                  enable;
   /**< Specifies whether the group device is enabled.

        @values
        - 0 -- Disabled
        - 1 -- Enabled @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_param_id_group_device_enable_t */
/** @} */ /* end_addtogroup afe_module_device_group */


/** @ingroup afe_module_limiter
    ID of the Limiter module, which sets or gets the param data of
    the Limiter for a requested AFE Tx/Rx port.

    This module supports the following parameter IDs:
    - AFE_PARAM_ID_ENABLE

    @apr_hdr_fields
    Opcode -- AFE_MODULE_LIMITER
*/
#define AFE_MODULE_LIMITER                                    0x000102A8


/** @addtogroup afe_service_api_deprecated
@{ */
/** Deprecated.

    Defines custom topologies in the aDSP for use in an AFE session.
    These topologies are created by specifying the topology IDs in the
    #AFE_PARAM_ID_TOPO_ID parameter.

  @apr_hdr_fields
    Opcode -- AFE_CMD_ADD_TOPOLOGIES

  @apr_msgpayload{afe_cmd_add_topologies_t}
    @table{weak__afe__cmd__add__topologies__t}

  @par Buffer format (afe_custom_topologies_t)
    Immediately following this structure are num_topologies of the
    definitions to be added to the aDSP (see <b>Topology definition</b>).
    @tablens{weak__afe__custom__topologies__t}
    @par
    Calculation of the buffer size:
    - Size of the module information structure = 4 + 4 + 16 = 24 bytes
    - Size of the topology definition structure = 4 + 4 + 16 * (size of the
      module information structure) = 392 bytes
    - Size of the buffer = 4 + num_topologies * 392

  @par Topology definition (afe_topology_definition_t)
    Immediately following this structure are num_modules of
   <b>Module information</b>.
    @tablens{weak__afe__topology__definition__t}

  @par Module information (afe_module_info_t)
    @table{weak__afe__module__info__t}

  @detdesc
    The aDSP stores custom topology definitions internally. Only the
    definitions sent to the AFE are available for AFE sessions.
    @par
    This command must be sent before opening an AFE session with the custom
    topologies. It can be sent multiple times to add new topologies.
    If a subsequent command redefines a previously defined topology ID,
    the previous topology definition is replaced with the new definition.
    However, the changes take effect only when the next AFE session is created.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).
    The client defines the handle as input.

  @dependencies
    None. @newpage
 */
#define AFE_CMD_ADD_TOPOLOGIES                                0x000100f8

/* The payload for the AFE_CMD_ADD_TOPOLOGIES command */
typedef struct afe_cmd_add_topologies_t afe_cmd_add_topologies_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_cmd_add_topologies_t
@{ */
/* Payload of the AFE_CMD_ADD_TOPOLOGIES command.
 */
struct afe_cmd_add_topologies_t
{
    uint32_t                  data_payload_addr_lsw;
    /**< Lower 32 bits of the 64-bit topology buffer address. */

    uint32_t                  data_payload_addr_msw;
    /**< Upper 32 bits of the 64-bit topology buffer address. */

    uint32_t                  mem_map_handle;
    /**< Unique identifier for a shared memory address.

         The aDSP returns the memory map handle through the
         #AFE_SERVICE_CMD_SHARED_MEM_MAP_REGIONS command. */

    uint32_t                  buffer_size;
    /**< Size in bytes of the valid data in the topology buffer. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_cmd_add_topologies_t */

typedef struct afe_module_info_t afe_module_info_t;

#include "adsp_begin_pack.h"

/** Deprecated. Maximum number of initialization parameters for a topology. */
#define AFE_TOPO_MODULE_MAX_INIT_PARAMS  16

/** @weakgroup weak_afe_module_info_t
@{ */
/* Defines the modules used by the AFE_CMD_ADD_TOPOLOGIES command.
 */
struct afe_module_info_t
{
   uint32_t module_id;
    /**< Valid ID of the module.

        @values See Chapter @xref{hdr:CalibrationIDs} */

   uint32_t use_lpm;
   /**< Specifies whether to put this module in low-power memory.

        @values
        - 0 -- Do not put this module in low-power memory
        - 1 -- Put this module in low-power memory

        If there is insufficient low-power memory, this module is placed in
        the default memory pool. */

   uint8_t  init_params[AFE_TOPO_MODULE_MAX_INIT_PARAMS];
   /**< Array of initialization parameters passed to the module when it is
        created.

        This array of 16 bytes is sent to the getsize() and init() methods of
        the module. The interpretation of this information is
        module-specific. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_module_info_t */

typedef struct afe_topology_definition_t afe_topology_definition_t;

#include "adsp_begin_pack.h"

/** Deprecated. Maximum number of modules supported in an AFE custom topology. */
#define AFE_MAX_MODULES_PER_TOPOLOGY 16

/** @weakgroup weak_afe_topology_definition_t
@{ */
/* Defines a custom topology that is to be added to the aDSP. This structure
    is followed by num_modules of afe_module_info_t.
 */
struct afe_topology_definition_t
{
   uint32_t topology_id;
   /**< ID of this custom topology.

        @values 0x10000000 to 0x1FFFFFFF */

   uint32_t num_modules;
   /**< Number of modules in this topology.

        @values 0 to #AFE_MAX_MODULES_PER_TOPOLOGY */

   afe_module_info_t module_info[AFE_MAX_MODULES_PER_TOPOLOGY];
   /**< Array of module information structures with 16 elements.

        The order of the elements in this array specifies the order in which
        the modules are connected in the topology, from source to sink. Only
        the first num_modules elements of this array are required. The rest of
        the elements are ignored. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_topology_definition_t */

typedef struct afe_custom_topologies_t afe_custom_topologies_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_custom_topologies_t
@{ */
/* Header structure for the custom topologies command buffer. This structure
    is followed by num_topologies of afe_topology_definition_t.
 */
struct afe_custom_topologies_t
{
   uint32_t num_topologies;
   /**< Specifies the number of topologies in this command.

        @values Any nonzero number */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_custom_topologies_t */
/** @} */ /* end_addtogroup afe_service_api_deprecated */


/** @addtogroup afe_module_audio_device
@{ */
/** ID of the parameter used by #AFE_MODULE_AUDIO_DEV_INTERFACE to configure
    the topology ID of the AFE session.

    Only #AFE_PORT_CMD_SET_PARAM_V2 (deprecated) and #AFE_PORT_CMD_SET_PARAM_V3
    can use this parameter ID.

    @apr_msgpayload{afe_topo_id_v1_t}
    @table{weak__afe__topo__id__v1__t}
 */
#define AFE_PARAM_ID_TOPO_ID                                         0x0001025A

/** Version information used to handle future additions to
    AFE_PARAM_ID_TOPO_ID processing (for backward compatibility).
 */
#define AFE_API_VERSION_TOPO_ID                                0x1

/* Structure for the AFE_PARAM_ID_TOPO_ID. */
typedef struct afe_topo_id_v1_t  afe_topo_id_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_topo_id_v1_t
@{ */
/* Configuration structure for the AFE_PARAM_ID_TOPO_ID parameter.
 */
struct afe_topo_id_v1_t
{
   uint32_t                  topo_def_minor_version;
   /**< Tracks the configuration of this parameter.

        @values #AFE_API_VERSION_TOPO_ID */

   uint32_t                  topology_id;
   /**< ID of the topology for the AFE session.

        @values See Section @xhyperref{hdr:TopoIDs,AFE Topology IDs} */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_topo_id_v1_t */
/** @} */ /* end_addtogroup afe_module_audio_device */


/** @addtogroup afe_topo_ids
@{ */
/** ID of the AFE Tx topology with no modules. */
#define AFE_TX_TOPOLOGY_ID_NONE                       0x000112FB

/** ID of the no-default AFE Rx topology. */
#define AFE_RX_TOPOLOGY_ID_NONE                        0x000112FC

/** @} */ /* end_addtogroup afe_topo_ids */

/** @endcond */
/** @} */ /* end_addtogroup afe_topo_ids */

/** @addtogroup afe_module_audio_device
@{ */
/** ID of the parameter used by #AFE_MODULE_AUDIO_DEV_INTERFACE to configure
    the operating latency mode of AFE port .

    Only #AFE_PORT_CMD_SET_PARAM_V2 (deprecated) and #AFE_PORT_CMD_SET_PARAM_V3
    can use this parameter ID.

    @apr_msgpayload{afe_param_id_port_lat_mode_cfg_t}
 */
#define AFE_PARAM_ID_PORT_LATENCY_MODE_CONFIG                  0x000102B3

/** AFE port will operate at (or close to) 1ms frame size  */
#define AFE_PORT_DEFAULT_LATENCY_MODE                          0

/**< AFE port will operate at (or close to) 0.5ms frame size */
#define AFE_PORT_LOW_LATENCY_MODE                              1

/** Version information used to handle future additions to
    AFE_PARAM_ID_PORT_LATENCY_MODE_CONFIG processing (for backward compatibility).
 */
#define AFE_API_VERSION_PORT_LATENCY_MODE                      0x1

/** @} */ /* end_addtogroup afe_module_audio_device */

/* Structure for the AFE_PARAM_ID_PORT_LATENCY_MODE_CONFIG. */
typedef struct afe_param_id_port_lat_mode_cfg_t  afe_param_id_port_lat_mode_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_port_lat_mode_cfg_t
@{ */
/* Configuration structure for the AFE_PARAM_ID_PORT_LATENCY_MODE_CONFIG parameter.
 */
struct afe_param_id_port_lat_mode_cfg_t
{
  uint32_t                  minor_version;
  /**< Minor version used to track AFE port latency mode configuration.

        @values #AFE_API_VERSION_PORT_LATENCY_MODE */

  uint32_t                  mode;
  /**< AFE port operating latency modes.

        @values #AFE_PORT_DEFAULT_LATENCY_MODE
                #AFE_PORT_LOW_LATENCY_MODE */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_param_id_port_lat_mode_cfg_t */

/** @addtogroup afe_module_sw_duty_cycle
@{ */
/** ID of the Software Duty Cycle module, which is used to configure the
    duty cycle of the AFE software.

    This module supports the following parameter IDs:
     - #AFE_PARAM_ID_SW_DUTY_CYCLE_CFG
     - AFE_PARAM_ID_ENABLE

    To configure the duty cycle, use #AFE_PORT_CMD_SET_PARAM_V2 (deprecated)
    or #AFE_PORT_CMD_SET_PARAM_V3, and fill the module ID with the parameter
    IDs.

    The AFE_PARAM_ID_SW_DUTY_CYCLE_CFG and AFE_PARAM_ID_ENABLE (enable=1)
    parameters must be sent before the AFE port starts.

    After receiving AFE_PARAM_ID_ENABLE (enable=0), there is no data exchange
    between the codec and AFE.

    @apr_hdr_fields
    Opcode -- AFE_MODULE_SW_DUTY_CYCLE
 */
#define AFE_MODULE_SW_DUTY_CYCLE                                   0x00010258

/** ID of the parameter used by #AFE_MODULE_SW_DUTY_CYCLE to configure the duty
    cycle parameters. 

    AFE_PARAM_ID_SW_DUTY_CYCLE_CFG must be sent before the AFE port starts.
    If the port is in the Run state, this parameter is not accepted.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_SW_DUTY_CYCLE_CFG

    @apr_msgpayload{afe_param_id_sw_duty_cycle_cfg_v1_t}
    @table{weak__afe__param__id__sw__duty__cycle__cfg__v1__t}
 */
#define AFE_PARAM_ID_SW_DUTY_CYCLE_CFG                            0x00010259

/** Version information used to handle future additions to
    AFE_PARAM_ID_SW_DUTY_CYCLE_CFG processing (for backward compatibility).
 */
#define AFE_API_VERSION_SW_DUTY_CYCLE_CFG                         0x1

typedef struct afe_param_id_sw_duty_cycle_cfg_v1_t afe_param_id_sw_duty_cycle_cfg_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_sw_duty_cycle_cfg_v1_t
@{ */
/* Payload of the AFE_PARAM_ID_SW_DUTY_CYCLE_CFG  parameter used by
    AFE_MODULE_SW_DUTY_CYCLE.
 */
struct afe_param_id_sw_duty_cycle_cfg_v1_t
{
    uint32_t                  minor_version;
    /**< Tracks the configuration of this parameter.

         @values #AFE_API_VERSION_SW_DUTY_CYCLE_CFG */

    uint32_t                  sleep_time_ms;
    /**< The amount of time when the AFE does not process any data.

         @values
         - > 0 milliseconds
         - Default value is 200 milliseconds @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_param_id_sw_duty_cycle_cfg_v1_t */
/** @} */ /* end_addtogroup afe_module_sw_duty_cycle */

/** @addtogroup afe_module_device_group
@{ */
/** ID of the multistream HDMI Rx group device. @newpage */
#define AFE_GROUP_DEVICE_ID_HDMI_MULTISTREAM_RX                                (AFE_PORT_ID_HDMI_MULTISTREAM_0_RX + 0x100)

/** ID of the parameter used by #AFE_MODULE_GROUP_DEVICE to
    configure the group device.

    #AFE_PORT_CMD_SET_PARAM_V2 (deprecated), #AFE_PORT_CMD_GET_PARAM_V2
    (deprecated), #AFE_PORT_CMD_SET_PARAM_V3, and #AFE_PORT_CMD_GET_PARAM_V3
    can use this parameter ID.

    Requirements:
    - The group must be configured before configuring and starting any
      of the member ports in the group.
    - The group can be enabled only after it is configured.
    - The group can be disabled only after all member ports of the group
      have been stopped.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_GROUP_DEVICE_HDMI_MULTISTREAM_CONFIG

    @apr_msgpayload{afe_param_id_group_device_hdmi_multistream_cfg_t}
    @table{weak__afe__param__id__group__device__hdmi__multistream__cfg__t}
 */
#define AFE_PARAM_ID_GROUP_DEVICE_HDMI_MULTISTREAM_CONFIG                              0x00010283

/** Version information used to handle future additions to group
    device configuration (for backward compatibility).
 */
#define AFE_API_VERSION_GROUP_DEVICE_HDMI_MULTISTREAM_CONFIG                           0x1

typedef struct afe_param_id_group_device_hdmi_multistream_cfg_t afe_param_id_group_device_hdmi_multistream_cfg_t;

/** ID of an invalid HDMI multistream substream ID. */
#define AFE_HDMI_MULTISTREAM_SUB_STREAM_ID_INVALID            0xFFFF

/** ID of HDMI multistream sub-stream ID 0 for metadata 0. */
#define AFE_HDMI_MULTISTREAM_SUB_STREAM_ID_0                  0x1

/** ID of HDMI multistream sub-stream ID 1 for metadata 1. */
#define AFE_HDMI_MULTISTREAM_SUB_STREAM_ID_1                  0x2

/** ID of HDMI multistream sub-stream ID 2 for metadata 2. */
#define AFE_HDMI_MULTISTREAM_SUB_STREAM_ID_2                  0x3

/** ID of HDMI multistream sub-stream ID 3 for metadata 3. */
#define AFE_HDMI_MULTISTREAM_SUB_STREAM_ID_3                  0x4

/** Maximum number of HDMI multistream sub-stream IDs. */
#define AFE_HDMI_MULTISTREAM_SUB_STREAM_ID_MAX                  AFE_HDMI_MULTISTREAM_SUB_STREAM_ID_3;

/** ID of an invalid HDMI multistream device group update action. */
#define AFE_HDMI_MULTISTREAM_GROUP_UPDATE_ACTION_ID_INVALID                  0x0

/** ID of the HDMI multistream device add group action. */
#define AFE_HDMI_MULTISTREAM_GROUP_UPDATE_ACTION_ID_ADD                      0x1

/** ID of the HDMI multistream device remove group action. */
#define AFE_HDMI_MULTISTREAM_GROUP_UPDATE_ACTION_ID_REMOVE                   0x2

/** ID of the HDMI multistream device start group action. */
#define AFE_HDMI_MULTISTREAM_GROUP_UPDATE_ACTION_ID_START                    0x3

/** ID of the HDMI multistream device stop group action. @newpage */
#define AFE_HDMI_MULTISTREAM_GROUP_UPDATE_ACTION_ID_STOP                     0x4

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_group_device_hdmi_multistream_cfg_t
@{ */
/* Payload of the AFE_PARAM_ID_GROUP_DEVICE_HDMI_MULTISTREAM_CONFIG parameter ID
    used by AFE_MODULE_GROUP_DEVICE.
 */
struct afe_param_id_group_device_hdmi_multistream_cfg_t
{
    uint32_t                  minor_version;
    /**< Tracks the configuration of this parameter.

         @values #AFE_API_VERSION_GROUP_DEVICE_HDMI_MULTISTREAM_CONFIG */

    uint16_t                  group_id;
    /**< ID for the group device.

         @values #AFE_GROUP_DEVICE_ID_HDMI_MULTISTREAM_RX */

    uint16_t                  port_id[AFE_GROUP_DEVICE_NUM_PORTS];
    /**< Array of member port IDs in this group.

         @values
         Where group_id = #AFE_GROUP_DEVICE_ID_HDMI_MULTISTREAM_RX
         - #AFE_PORT_ID_HDMI_MULTISTREAM_0_RX
         - #AFE_PORT_ID_HDMI_MULTISTREAM_1_RX
         - #AFE_PORT_ID_HDMI_MULTISTREAM_2_RX
         - #AFE_PORT_ID_HDMI_MULTISTREAM_3_RX

         For unused slots, set this field to #AFE_PORT_ID_INVALID. */

    uint16_t                  stream_id[AFE_GROUP_DEVICE_NUM_PORTS];
    /**< Array of matched sub-stream IDs and port IDs for this group.

         @values
         Where group_id = #AFE_GROUP_DEVICE_ID_HDMI_MULTISTREAM_RX
         - #AFE_HDMI_MULTISTREAM_SUB_STREAM_ID_0
         - #AFE_HDMI_MULTISTREAM_SUB_STREAM_ID_1
         - #AFE_HDMI_MULTISTREAM_SUB_STREAM_ID_2
         - #AFE_HDMI_MULTISTREAM_SUB_STREAM_ID_3

         For unused slots, set this field to
         #AFE_HDMI_MULTISTREAM_SUB_STREAM_ID_INVALID. */

   uint16_t                 action_id;
   /**< ID of the group update action.

        @values
         - #AFE_HDMI_MULTISTREAM_GROUP_UPDATE_ACTION_ID_START
         - #AFE_HDMI_MULTISTREAM_GROUP_UPDATE_ACTION_ID_ADD
         - #AFE_HDMI_MULTISTREAM_GROUP_UPDATE_ACTION_ID_REMOVE
         - #AFE_HDMI_MULTISTREAM_GROUP_UPDATE_ACTION_ID_STOP @vertspace{-28} */

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_param_id_group_device_hdmi_multistream_cfg_t */

/** ID of the parameter used by #AFE_MODULE_AUDIO_DEV_INTERFACE to log
    continuous buffer latency statistics for playback use cases.

    This parameter ID is currently used for logging the continuous buffer
    latencies. #AFE_PORT_CMD_SET_PARAM_V2 (deprecated),
    #AFE_PORT_CMD_GET_PARAM_V2 (deprecated), #AFE_PORT_CMD_SET_PARAM_V3, and
    #AFE_PORT_CMD_GET_PARAM_V3 do not support this ID.

    @msgpayload{afe_cont_buf_latency_stats_v1_t}
    @table{weak__afe__cont__buf__latency__stats__v1__t}
 */
#define AFE_PARAM_ID_CONT_BUF_LATENCY_STATS                             0x00010295

/** Version information used to handle future additions to AFE continuous
    buffer latency statistics (for backward compatibility). @newpage
 */
#define AFE_API_VERSION_CONT_BUF_LATENCY_STATS                          0x1

typedef struct afe_cont_buf_latency_stats_v1_t afe_cont_buf_latency_stats_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_cont_buf_latency_stats_v1_t
@{ */
/* Configuration structure for the
    AFE_PARAM_ID_CONT_BUF_LATENCY_STATS parameter (version 1).
 */
struct afe_cont_buf_latency_stats_v1_t
{
   uint32_t       cont_buf_latency_stats_minor_version;
   /**< Tracks the statistics for this parameter.

        @values #AFE_API_VERSION_CONT_BUF_LATENCY_STATS */

   uint32_t        buf_time_stamp_lsw;
   /**< Least significant word of timestamp of the PCM buffer to be
        rendered by the AFE.

        @values Any valid uint32 number */

   uint32_t        buf_time_stamp_msw;
   /**< Most significant word of timestamp of the PCM buffer to be
        rendered by the AFE.

        @values Any valid uint32 number */

   uint32_t        rndr_time_stamp_lsw;
   /**< Least significant word of logging timestamp of the PCM buffer
        at rendering to the end device interface.

        @values Any valid uint32 number */

   uint32_t        rndr_time_stamp_msw;
   /**< Most significant word of logging timestamp of the PCM buffer
       at rendering to the end device interface.

       @values Any valid uint32 number */

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_cont_buf_latency_stats_v1_t */
/** @} */ /* end_addtogroup afe_module_device_group */

/** @addtogroup afe_module_device_group
@{ */
/** ID of the parameter used by #AFE_MODULE_AUDIO_DEV_INTERFACE and to specify
    the timing statistics of the corresponding device interface.

    A client can periodically query for the device time statistics to help
    adjust the PLL/detect the drift between sources and sink rates based
    on the drift value. #AFE_PORT_CMD_GET_PARAM_V3 must be sent to the AFE port
    ID that corresponds to the device interface.

    This parameter ID supports #AFE_PORT_CMD_GET_PARAM_V2 (deprecated) and
    #AFE_PORT_CMD_GET_PARAM_V3, but not #AFE_PORT_CMD_SET_PARAM_V2 (deprecated)
    and #AFE_PORT_CMD_SET_PARAM_V3.

    @msgpayload{afe_param_id_dev_timing_stats_t}
    @table{weak__afe__param__id__dev__timing__stats__t}
 */
#define AFE_PARAM_ID_DEV_TIMING_STATS                         0x000102AD

/** Version information used to handle future additions to AFE device
    interface timing statistics (for backward compatibility).
 */
#define AFE_API_VERSION_DEV_TIMING_STATS        0x1

/** Enumeration for specifying a sink (Rx) device. */
#define AFE_SINK_DEVICE                         0x0

/** Enumeration for specifying a source (Tx) device. */
#define AFE_SOURCE_DEVICE                       0x1

/** Enumeration for specifying that the drift reference is of type AV Timer. */
#define AFE_REF_TIMER_TYPE_AVTIMER              0x0

typedef struct afe_param_id_dev_timing_stats_t afe_param_id_dev_timing_stats_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_dev_timing_stats_t
@{ */
/* Message payload structure for the
    AFE_PARAM_ID_DEV_TIMING_STATS parameter.
 */
struct afe_param_id_dev_timing_stats_t
{
    uint32_t       minor_version;
    /**< Tracks the configuration of this parameter.

         @values #AFE_API_VERSION_DEV_TIMING_STATS */

    uint16_t        device_direction;
    /**< Specifies the device interface direction as either source (Tx) or
         sink (Rx).

         @values
         - #AFE_SINK_DEVICE
         - #AFE_SOURCE_DEVICE @tablebulletend */

    uint16_t        reference_timer;
    /**< Reference timer for drift accumulation and timestamp information.

         @values #AFE_REF_TIMER_TYPE_AVTIMER */

    uint32_t        resync_flag;
    /**< Specifies whether resynchronization is required on the client side for
         drift correction.

         @values
         - 0 -- FALSE (Resynchronization is not required)
         - 1 -- TRUE (Resynchronization is required)

         This flag is set to TRUE for the first #AFE_PORT_CMDRSP_GET_PARAM_V3
         after the device interface starts. The client can use the flag as
         follows:
         - To check whether the device interface restart occurred.
         - To check whether any resynchronization for drift correction is
           required on the client side. @tablebulletend */

    int32_t         acc_drift_value;
    /**< Accumulated drift value in microseconds.

         @values Any valid int32 number
         - Positive value -- The reference timer is running faster than the
           device.
         - Negative value -- The reference timer is running slower than the
           device. @tablebulletend */

    uint32_t        ref_timer_abs_ts_lsw;
    /**< Lower 32 bits of the 64-bit absolute timestamp of reference
         timer in microseconds.

        @values Any valid uint32 number

        This timestamp corresponds to the time when the AFE port structure
        updates the drift from the latched register value. */

    uint32_t        ref_timer_abs_ts_msw;
    /**< Upper 32 bits of the 64-bit absolute timestamp of reference
         timer in microseconds.

         This timestamp corresponds to the time as the AFE port structure
         update the drift from latched register value.

         @values Any valid uint32 number */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_param_id_dev_timing_stats_t */
/** @} */ /* end_addtogroup afe_module_device_group */


/** @addtogroup afe_module_port_group
@{ */
/** ID of the AFE Port module, which is used to define AFE port properties.

    This module supports following parameter IDs:
     - #AFE_PARAM_ID_PORT_MEDIA_TYPE

    To configure the port property, the client must use
    #AFE_PORT_CMD_SET_PARAM_V2 (deprecated) or #AFE_PORT_CMD_SET_PARAM_V3, and
    fill the module ID with the supported parameter IDs.

    @apr_hdr_fields
    Opcode -- AFE_MODULE_PORT
 */
#define AFE_MODULE_PORT 0x000102a6

/** ID of the parameter used by #AFE_MODULE_PORT to set the port media type.

    #AFE_PORT_CMD_SET_PARAM_V2 (deprecated) and #AFE_PORT_CMD_SET_PARAM_V3 can
    use this parameter ID.
    #AFE_PORT_CMD_GET_PARAM_V2 (deprecated) and #AFE_PORT_CMD_GET_PARAM_V3 do
    not support this ID.

    This parameter ID is supported only when
    afe_port_cmd_slimbus_cfg_t::data_format is set to
    #AFE_SB_DATA_FORMAT_GENERIC_COMPRESSED (in #AFE_PARAM_ID_SLIMBUS_CONFIG).

    Subsequent #AFE_PORT_CMD_DEVICE_START commands will fail if this parameter
    is set to other data_format field types.

    This parameter is only to be used in encoding/decoding use cases
    where the port media type is expected to be different compared to the
    device media type (see M2 and M4 in the following example).

    Typical audio or voice use cases where the entire port is PCM or
    compressed do not expect this parameter to be set. Subsequent start
    commands will fail.

    @par Example of this AFE_PARAM_ID_PORT_MEDIA_TYPE media type
    (Represented as M2.)
    @par
    Rx client media type: M1 @rarrow AFE topology/processing (AFE port media
    type: M2) @rarrow encoder (enc cfg blk media type: M3) @rarrow COP
    packetizer @rarrow SLIMbus media type: M4
    @par
    Tx client media type: M1 @larrow AFE topology/processing (AFE port media
    type: M2) @larrow Decoder (dec output media type: M3) @larrow- COP
    depacketizer @larrow SLIMbus media type: M4

    @par Port media type (afe_port_media_type_v1_t)
    @table{weak__afe__port__media__type__v1__t}
 */
#define AFE_PARAM_ID_PORT_MEDIA_TYPE                             0x000102a7

/** Macro that defines the linear PCM data format. */
#define AFE_PORT_DATA_FORMAT_LINEAR_PCM                       0x0

/** Macro that defines the generic compressed data format. */
#define AFE_PORT_DATA_FORMAT_GENERIC_COMPRESSED   0x1

/** Version information used to handle future additions to the configuration
    interface (for backward compatibility).
 */
#define AFE_API_VERSION_PORT_MEDIA_TYPE                          0x1

typedef struct afe_port_media_type_v1_t afe_port_media_type_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_port_media_type_v1_t
@{ */
/* Configuration structure for the
    #AFE_PARAM_ID_PORT_MEDIA_TYPE parameter.
 */
struct afe_port_media_type_v1_t
{
    uint32_t                  minor_version;
    /**< Tracks the configuration of this parameter.

         @values #AFE_API_VERSION_PORT_MEDIA_TYPE */

    uint32_t                  sample_rate;
    /**< Sampling rate of the port.

         @values
         - #AFE_PORT_SAMPLE_RATE_8K
         - #AFE_PORT_SAMPLE_RATE_11_025K
         - #AFE_PORT_SAMPLE_RATE_12K
         - #AFE_PORT_SAMPLE_RATE_16K
         - #AFE_PORT_SAMPLE_RATE_22_05K
         - #AFE_PORT_SAMPLE_RATE_24K
         - #AFE_PORT_SAMPLE_RATE_32K
         - #AFE_PORT_SAMPLE_RATE_44_1K
         - #AFE_PORT_SAMPLE_RATE_48K
         - #AFE_PORT_SAMPLE_RATE_88_2K
         - #AFE_PORT_SAMPLE_RATE_96K
         - #AFE_PORT_SAMPLE_RATE_176_4K
         - #AFE_PORT_SAMPLE_RATE_192K
         - #AFE_PORT_SAMPLE_RATE_352_8K
         - #AFE_PORT_SAMPLE_RATE_384K @tablebulletend */

    uint16_t                  bit_width;
    /**< Bit width of the sample.

         @values 16, 24, 32 */

    uint16_t                  num_channels;
    /**< Number of channels.

         @values 1 to #AFE_PORT_MAX_AUDIO_CHAN_CNT */

    uint16_t                  data_format;
    /**< Data format supported by this port.

         Different media types for the port and device indicate an encoding or
         decoding use case.

         @values
         - #AFE_PORT_DATA_FORMAT_LINEAR_PCM
         - #AFE_PORT_DATA_FORMAT_GENERIC_COMPRESSED @tablebulletend */

    uint16_t                  reserved;
    /**< This field must be set to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_port_media_type_t */
/** @} */ /* end_addtogroup afe_module_port_group */


/** @addtogroup afe_module_tdm
@{ */
/** ID of the Time Division Multiplexing (TDM) module, which is used to
    configure AFE TDM.

    This module supports following parameter ID:
     - #AFE_PARAM_ID_PORT_SLOT_MAPPING_CONFIG

    To configure the TDM interface, the client must use
    #AFE_PORT_CMD_SET_PARAM_V2 (deprecated) or #AFE_PORT_CMD_SET_PARAM_V3, and
    fill the module ID with the appropriate parameter IDs.

    @apr_hdr_fields
    Opcode -- AFE_MODULE_TDM
*/
#define AFE_MODULE_TDM                                             0x0001028A

/** ID of the parameter used by AFE_MODULE_TDM to configure TDM slot
    mapping.

    #AFE_PORT_CMD_SET_PARAM_V2 (deprecated) and #AFE_PORT_CMD_SET_PARAM_V3 can
    use this parameter ID.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_PORT_SLOT_MAPPING_CONFIG

    @msgpayload{afe_param_id_slot_mapping_cfg_t}
    @table{weak__afe__param__id__slot__mapping__cfg__t}
 */
#define AFE_PARAM_ID_PORT_SLOT_MAPPING_CONFIG                            0x00010297

/** Version information used to handle future additions to
    #AFE_PARAM_ID_PORT_SLOT_MAPPING_CONFIG configuration (for backward
    compatibility).
*/
#define AFE_API_VERSION_SLOT_MAPPING_CONFIG                           0x1

/** Data is aligned with the most significant bit. */
#define AFE_SLOT_MAPPING_DATA_ALIGN_MSB                          0   //default

/** Data is aligned with the least significant bit. */
#define AFE_SLOT_MAPPING_DATA_ALIGN_LSB                          1   

/** Specified offset is invalid. @newpage */
#define AFE_SLOT_MAPPING_OFFSET_INVALID                                0xFFFF

typedef struct afe_param_id_slot_mapping_cfg_t afe_param_id_slot_mapping_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_slot_mapping_cfg_t
@{ */
/* Payload of the AFE_PARAM_ID_PORT_SLOT_MAPPING_CONFIG command's TDM configuration
    parameter.
 */
struct afe_param_id_slot_mapping_cfg_t
{
    uint32_t                  minor_version;
    /**< Tracks the configuration of this parameter.

         @values #AFE_API_VERSION_SLOT_MAPPING_CONFIG */

    uint16_t                  num_channel;
    /**< Number of channels for the audio sample.

         @values 1, 2, 4, 6, 8 */

    uint16_t                   bitwidth;  
    /**< Slot bit width for each channel.

         @values 16, 24, 32 */

    uint32_t                  data_align_type;
    /**< Indicates how data is packed from slot_offset for a 32-slot bit width
         when the sample bit width is 24.

         @values
         - #AFE_SLOT_MAPPING_DATA_ALIGN_MSB (Default)
         - #AFE_SLOT_MAPPING_DATA_ALIGN_LSB @tablebulletend */

    uint16_t                   offset[AFE_PORT_MAX_AUDIO_CHAN_CNT];
    /**< Array of the slot mapping start offset in bytes for this frame.

         The bytes are counted from 0, and 0 is mapped to the first byte in or
         out of the digital serial data line to which this sub-frame belongs.

         The slot_offset[] setting is per-channel based. The maximum number of
         channels supported is 8.

         The offset array should start with a valid offset. Preset offsets,
         such as AFE_SLOT_MAPPING_OFFSET_INVALID, are not used in the arrays.

         If slot_bitwidth_per_channel = 32 and sample_bitwidth = 24,
         data_align_type is used to indicate how, per channel, the 24-bit
         sample data aligns with the 32-bit slot width. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_param_id_slot_mapping_cfg_t */

/** ID of the parameter used by #AFE_MODULE_TDM to configure the customer
    TDM header.

    #AFE_PORT_CMD_SET_PARAM_V2 (deprecated) and #AFE_PORT_CMD_SET_PARAM_V3 can
    use this parameter ID.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_CUSTOM_TDM_HEADER_CONFIG

    @msgpayload{afe_param_id_custom_tdm_header_cfg_t}
    @table{weak__afe__param__id__custom__tdm__header__cfg__t}

    @par SAD header payload
    @inputfile{group__weakf__sad__header__payload.tex}
*/
#define AFE_PARAM_ID_CUSTOM_TDM_HEADER_CONFIG                              0x00010298

/** Version information used to handle future additions to
    AFE_PARAM_ID_CUSTOM_TDM_HEADER_CONFIG configuration (for backward
    compatibility).
*/
#define AFE_API_VERSION_CUSTOM_TDM_HEADER_CONFIG                           0x1

/** Invalid TDM header. */
#define AFE_CUSTOM_TDM_HEADER_TYPE_INVALID                                 0x0

/** Default TDM header. */
#define AFE_CUSTOM_TDM_HEADER_TYPE_DEFAULT                                 0x1

/** TDM header for an entertainment channel as specified in the MOST
    standard. @newpage
 */
#define AFE_CUSTOM_TDM_HEADER_TYPE_ENTERTAINMENT_MOST                      0x2

typedef struct afe_param_id_custom_tdm_header_cfg_t afe_param_id_custom_tdm_header_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_custom_tdm_header_cfg_t
@{ */
/* Payload of the AFE_MODULE_TDM parameter ID
 */
struct afe_param_id_custom_tdm_header_cfg_t
{
    uint32_t                 minor_version;
    /**< Tracks the configuration of this parameter.

         @values #AFE_API_VERSION_CUSTOM_TDM_HEADER_CONFIG */

    uint16_t                start_offset;
    /**< Offset for slot mapping from the start of the subframe.

         @values 0, 4, 8 bytes

         The bytes are counted from 0, which is mapped to the first byte in or
         out of the digital serial data line where this subframe belongs. */

    uint16_t                header_width; 
    /**< Number of bytes per frame dedicated to the header.

         @values 2 bytes */

    uint16_t                header_type;
    /**< Indicates the type of custom TDM header.

         @values
         - #AFE_CUSTOM_TDM_HEADER_TYPE_INVALID
         - #AFE_CUSTOM_TDM_HEADER_TYPE_DEFAULT (for AAN channel per MOST)
         - #AFE_CUSTOM_TDM_HEADER_TYPE_ENTERTAINMENT_MOST (for the
           entertainment channel per MOST) @vertspace{-28} */

    uint16_t                num_frame_repeat;
    /**< Number of frames required to complete the TDM header.

         @values 8 */

/** @weakgroup weakf_sad_header_payload
  The following payload follows the sound associated (SAD) header for
  MOST/TDM. The size of the SAD header with this payload is
  num_of_frame_repeat * header_width_per_frame, which is 2 * 8 = 16 bytes in
  this example.

@verbatim

uint16_t   header0;   SyncHi 0x3C   Info[4] - CodecType                   -> 0x3C00
uint16_t   header1;   SyncLo 0xB2   Info[5] - SampleWidth                 -> 0xB218
                                              (SampleWidth = 24)
uint16_t   header2;   DTCP Info     Info[6] - Unused                      -> 0x0
uint16_t   header3;   Extension     Info[7] - ASAD-Value                  -> 0x00C0
                                              (ASAD = 0xC0 = 0b1100,0000)
uint16_t   header4;   Reserved      Info[0] - Number of bytes that follow -> 0x0007
uint16_t   header5;   Reserved      Info[1] - Media type @rarrow 0x0
uint16_t   header6;   Reserved      Info[2] - Bitrate[kbps] - High byte   -> 0x0
uint16_t   header7;   Reserved      Info[3] - Bitrate[kbps] - Low  byte   -> 0x0

@endverbatim
*/
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_param_id_custom_tdm_header_cfg_t */
/** @} */ /* end_addtogroup afe_module_tdm */


/** @addtogroup afe_cmd_request_lpass_resrcs
@{ */
/** Used by a client to request LPASS resources. The remote client will
    control the allocated resources until they are released.

  @note1hang Currently, this command supports only the LPAIF DMA resources.

  @apr_hdr_fields
    Opcode -- AFE_CMD_REQUEST_LPASS_RESOURCES

  @apr_msgpayload{afe_cmd_request_lpass_resources}
    @table{weak__afe__cmd__request__lpass__resources}

  @par DMA resouces payload (afe_cmd_request_lpass_dma_resources)
    @tablens{weak__afe__cmd__request__lpass__dma__resources}

  @detdesc
    This command has a variable payload whose size depends on the type of
    resource requested. For example, when the request is for LPAIF DMA
    resources:
@code
< afe_cmd_request_lpass_resources >
     resource_id = AFE_LPAIF_DMA_RESOURCE_ID
< afe_cmd_request_lpass_dma_resources >
     dma_type = AFE_LPAIF_DEFAULT_DMA_TYPE
     num_read_channels
     num_write_channels
     reserved (for 32-bit alignment)
@endcode
  @newpage
  @par
    The number of read and write DMA channels varies from chipset to chipset.
    The HLOS must ensure that it requests LPASS DMA resources, which should not
    impact the concurrencies that are mandatory for a chipset.

  @return
    #AFE_CMDRSP_REQUEST_LPASS_RESOURCES when all the requested resources are
    available.

  @dependencies
    None.
 */
#define AFE_CMD_REQUEST_LPASS_RESOURCES    0x00010109

/** Macro for requesting LPAIF DMA resources. */
#define AFE_LPAIF_DMA_RESOURCE_ID    0x00000001

typedef struct afe_cmd_request_lpass_resources afe_cmd_request_lpass_resources;

#include "adsp_begin_pack.h"
/** @weakgroup weak_afe_cmd_request_lpass_resources
@{ */
struct afe_cmd_request_lpass_resources
{
   uint32_t  resource_id;
   /**< Unique ID of the DMA resource.

        @values #AFE_LPAIF_DMA_RESOURCE_ID */
};
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_afe_cmd_request_lpass_resources */

/** Macro for the LPAIF default DMA data type. */
#define AFE_LPAIF_DEFAULT_DMA_TYPE     0x0

typedef struct afe_cmd_request_lpass_dma_resources afe_cmd_request_lpass_dma_resources;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_cmd_request_lpass_dma_resources
@{ */
/** When the request is for LPAIF DMA resources, this structure immediately
    follows %afe_cmd_request_lpass_resources.
*/
struct afe_cmd_request_lpass_dma_resources
{
   uint8_t dma_type;
   /**< Specifies the DMA resource type.

        @values #AFE_LPAIF_DEFAULT_DMA_TYPE */

   uint8_t num_read_dma_channels;
   /**< Number of read DMA channels.

        @values @ge 0

        A value of 0 indicates that channels are not requested. */

   uint8_t num_write_dma_channels;
   /**< Number of write dma channels.

        @values @ge 0

        A value of 0 indicates that channels are not requested. */

   uint8_t  reserved;
   /**< Used for 4-byte alignment; must be set to 0. */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_afe_cmd_request_lpass_dma_resources */
/** @} *//* end_addtogroup afe_cmd_request_lpass_resrcs */


/** @addtogroup afe_rsp_request_lpass_resrcs
@{ */
/** 
  Returns allocated resources in response to #AFE_CMD_REQUEST_LPASS_RESOURCES.

  @note1hang Currently, this command supports only the LPAIF DMA resources.

  @apr_hdr_fields
    Opcode -- AFE_CMDRSP_REQUEST_LPASS_RESOURCES

  @apr_msgpayload{afe_cmdrsp_request_lpass_resources}
    @table{weak__afe__cmdrsp__request__lpass__resources}

  @par DMA resources payload (afe_cmdrsp_request_lpass_dma_resources)
    @tablens{weak__afe__cmdrsp__request__lpass__dma__resources}
    @newpage

  @detdesc
    When LPAIF DMA resources are successfully allocated and requested:
    - The status field indicates a success
    - The message payload contains the DMA resources response payload followed
      by the variable data payload
    - Data in the payload contains DMA channel indices that were allocated by
      the aDSP.
@code
< message payload >
  afe_cmdrsp_request_lpass_resources
     status
     resource_id = AFE_LPAIF_DMA_RESOURCE_ID
< DMA resources payload >
  afe_cmdrsp_request_lpass_dma_resources
     dma_type = AFE_LPAIF_DEFAULT_DMA_TYPE
     num_read_dma_channels
     num_write_dma_channels
     reserved (for 32-bit alignment)
< variable data payload >
     num_read_dma_channels in bytes; contains the DMA index
     num_write_dma_channels in bytes; contains the DMA index
     variable padded zeros to make payload multiples of 4 bytes
@endcode
    @par
    For example, if two num_read_dma_channels are requested and they are
    successfully allocated, the data in the payload contains a valid DMA
    channel index value in the first two-byte array.
    @par
    The size of the variable data payload is the sum of num_read_dma_channels
    and num_write_dma_channels. If this sum is not in multiples of 4 bytes,
    padded bytes of zeros are added at the end to ensure 32-bit alignment.
    @par
    If requested resources were not allocated successfully, the payload is
    invalid.

  @return
    None.

  @dependencies
    None.
*/
#define AFE_CMDRSP_REQUEST_LPASS_RESOURCES    0x0001010A
typedef struct afe_cmdrsp_request_lpass_resources afe_cmdrsp_request_lpass_resources;

#include "adsp_begin_pack.h"
/** @weakgroup weak_afe_cmdrsp_request_lpass_resources
@{ */
struct afe_cmdrsp_request_lpass_resources
{
   uint32_t status;
   /**< Indicates whether all requested resources were allocated successfully.

        @values
        - ADSP_EOK -- Allocation was successful
        - ADSP_EFAILED -- Allocation failed

        For information on these status messages, see
        @xrefcond{Q5,80-NF774-4,80-NA610-4} */

   uint32_t  resource_id;
   /**< Unique ID of the specified resource.

        @values #AFE_LPAIF_DMA_RESOURCE_ID */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weak_afe_cmdrsp_request_lpass_resources */

typedef struct afe_cmdrsp_request_lpass_dma_resources afe_cmdrsp_request_lpass_dma_resources;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_cmdrsp_request_lpass_dma_resources
@{ */
/* This payload is for AFE_CMDRSP_REQUEST_LPASS_RESOURCES,
  when the LPAIF DMA resources are requested.
*/
/** When the request is for LPAIF DMA resources, this structure immediately
    follows %afe_cmdrsp_request_lpass_resources. After this structure is the
    variable data payload, whose size is the sum of num_read_dma_channels and
    num_write_dma_channels.
*/
struct afe_cmdrsp_request_lpass_dma_resources
{
   uint8_t dma_type;
   /**< Indicates the DMA resource type.

        @values #AFE_LPAIF_DEFAULT_DMA_TYPE */

   uint8_t num_read_dma_channels;
   /**< Number of read DMA channels.

        @values @ge 0 */

   uint8_t num_write_dma_channels;
   /**< Number of write DMA channels.

        @values @ge 0 */

   uint8_t  reserved;
    /**< Used for 4-byte alignment; must be set to 0. */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_afe_cmdrsp_request_lpass_dma_resources */
/** @} *//* end_addtogroup afe_rsp_request_lpass_resrcs */


/** @addtogroup afe_cmd_release_lpass_resrcs
@{ */
/** Releases resources that were allocated through
  #AFE_CMD_REQUEST_LPASS_RESOURCES.

  @note1hang Currently, this command supports only the LPAIF DMA resources.

  @apr_hdr_fields
    Opcode -- AFE_CMD_RELEASE_LPASS_RESOURCES

  @apr_msgpayload{afe_cmd_release_lpass_resources}
    @table{weak__afe__cmd__release__lpass__resources}

  @par DMA resources payload (afe_cmd_release_lpass_dma_resources)
    @tablens{weak__afe__cmd__release__lpass__dma__resources}

  @detdesc
    When allocated LPAIF DMA resources are to be released, the message payload
    contains the DMA resources payload followed by a variable data payload.
    The size of the variable data payload is the sum of num_read_dma_channels
    and num_write_dma_channels. If this sum is not in multiples of 4 bytes,
    padded bytes of zeros are included at the end.
    @par
    The variable payload contains the valid DMA channel indices that were
    allocated as part of #AFE_CMD_REQUEST_LPASS_RESOURCES.
    @newpage
    @par
    For example, if two num_read_dma_channels and one num_write_dma_channels
    are to be released, the variable data payload contains valid DMA channel
    index values in the first two bytes.
@code
< message payload >
  afe_cmd_release_lpass_resources
     resource_id = AFE_LPAIF_DMA_RESOURCE_ID
< DMA resources payload >
  afe_cmd_release_lpass_dma_resources
     dma_type = AFE_LPAIF_DEFAULT_DMA_TYPE
     num_read_dma_channels  = 2
     num_write_dma_channels = 1
     reserved (for 32-bit alignment)
< variable data payload >
     sum of num_read_dma_channels + num_write_dma_channels, in bytes
     valid_rd_dma_idx_1
     valid rd_dma_idx_2
     valid_wr_dma_idx_1
@endcode
    @par
    The client must fill the same DMA channel indices that were returned as
    part of #AFE_CMD_REQUEST_LPASS_RESOURCES. Otherwise, the aDSP returns an
    error.
    @par
    If the HLOS tries to release resources that are not allocated, the aDSP
    returns an error.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    None.
*/
#define AFE_CMD_RELEASE_LPASS_RESOURCES       0x0001010B

typedef struct afe_cmd_release_lpass_resources afe_cmd_release_lpass_resources;

#include "adsp_begin_pack.h"
/** @weakgroup weak_afe_cmd_release_lpass_resources
@{ */
struct afe_cmd_release_lpass_resources
{
   uint32_t  resource_id;
   /**< Unique ID of the specified resource.

        @values #AFE_LPAIF_DMA_RESOURCE_ID */
}

#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_afe_cmd_release_lpass_resources */

typedef struct afe_cmd_release_lpass_dma_resources afe_cmd_release_lpass_dma_resources;

#include "adsp_begin_pack.h"
/** @weakgroup weak_afe_cmd_release_lpass_dma_resources
@{ */
/** When the request is for LPAIF DMA resources, this structure immediately
    follows %afe_cmd_release_lpass_resources. After this structure is the
    variable data payload, whose size is the sum of num_read_dma_channels and
    num_write_dma_channels.
*/
struct afe_cmd_release_lpass_dma_resources
{
  uint8_t dma_type;
   /**< Indicates the DMA resource type.

        @values #AFE_LPAIF_DEFAULT_DMA_TYPE */

   uint8_t num_read_dma_channels;
   /**< Number of read DMA channels.

        @values @ge 0 */

   uint8_t num_write_dma_channels;
   /**< Number of write DMA channels.

        @values @ge 0 */

   uint8_t  reserved;
    /**< Used for 4-byte alignment; must be set to 0. */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_afe_cmd_release_lpass_dma_resources */
/** @} *//* end_addtogroup afe_cmd_release_lpass_resrcs */


#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* ADSP_AFE_SERVICE_COMMANDS_H_ */
