#ifndef QSEE_ESE_SERVICE_H
#define QSEE_ESE_SERVICE_H

/**
 *
 *  @file qsee_ese_service.h
 *
 *  @brief Provide Embedded Secure Element (eSE) Services API wrappers. The eSE QSEE service
 *         resides in the QSEE kernel.
 *         The has the following responsibilities:
 *         - Management of multiple sessions and open channels.
 *         - Sending Command Application Protocol Data Units (C-APDUs) to the eSE driver
 *         - Receiving Response Application Protocol Data Units (R-APDUs) from the eSE driver.
 *
 */

/*===========================================================================
  Copyright (c) 2015 - 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ===========================================================================*/

/*===========================================================================

  EDIT HISTORY FOR FILE

  $Header:
  $DateTime:
  $Author:

  when            who         what, where, why
  --------        ---         -----------------------------------
  11-Oct-2016     dc          Add QSEE_ESE_BASIC_CHANNEL_NUMBER
  15-Dec-2015     dc          Add support for SPI Chip Select slave identifier
  20-Apr-2015     dc          Initial Version

  ===========================================================================*/

#include <stdint.h>
#include <stdbool.h>
#include "qsee_heap.h"
#include "qsee_log.h"
#include "qsee_spi.h"

// clang-format off
#define QSEE_ESE_APDU_CLA_CHANNEL_MASK         0x03
#define QSEE_ESE_APDU_MAX_SIZE_SHORT_LE        256
#define QSEE_ESE_APDU_OFFSET_CLA               0
#define QSEE_ESE_APDU_OFFSET_INS               1
#define QSEE_ESE_APDU_OFFSET_P1                2
#define QSEE_ESE_APDU_OFFSET_P2                3
#define QSEE_ESE_APDU_OFFSET_LC                4
#define QSEE_ESE_APDU_OFFSET_CDATA             5
#define QSEE_ESE_APDU_OFFSET_EXT_CDATA         7
#define QSEE_ESE_APDU_CLA_INTERINDUSTRY        0x00
#define QSEE_ESE_APDU_CLA_FURTHER_INTERINDUSTR 0x40
#define QSEE_ESE_APDU_CLA_GLOBALPLATFORM       0x80
#define QSEE_ESE_APDU_INS_MANAGE_CHANNEL       0x70
#define QSEE_ESE_APDU_INS_SELECT               0xA4
#define QSEE_ESE_APDU_INS_GET_RSP              0xC0
#define QSEE_ESE_APDU_P1_MANAGE_CHANNEL_OPEN   0x00
#define QSEE_ESE_APDU_P1_MANAGE_CHANNEL_CLOSE  0x80
#define QSEE_ESE_APDU_P1_SELECT                0x04
#define QSEE_ESE_APDU_P2_SELECT_NEXT           0x02
#define QSEE_ESE_APDU_CLA_SZ                   1
#define QSEE_ESE_APDU_INS_SZ                   1
#define QSEE_ESE_APDU_P1_SZ                    1
#define QSEE_ESE_APDU_P2_SZ                    1
#define QSEE_ESE_APDU_LC_SZ                    1
#define QSEE_ESE_APDU_LE_SZ                    1
#define QSEE_ESE_APDU_EXT_LC_SZ                3
#define QSEE_ESE_APDU_EXT_LE_SZ_WITH_LC        2
#define QSEE_ESE_APDU_EXT_LE_SZ_WITHOUT_LC     3
#define QSEE_ESE_APDU_HDR_SZ                   (QSEE_ESE_APDU_CLA_SZ + QSEE_ESE_APDU_INS_SZ + QSEE_ESE_APDU_P2_SZ + QSEE_ESE_APDU_P2_SZ)
#define QSEE_ESE_APDU_MIN_SZ                   QSEE_ESE_APDU_HDR_SZ
#define QSEE_ESE_CPDU_PAYLOAD_SZ               0xFFFF  // 7816-4 Maximum extended CAPDU
#define QSEE_ESE_RPDU_PAYLOAD_SZ               0x10000 // 7816-4 Maximum extended RAPDU
#define QSEE_ESE_RPDU_STATUS_SZ                0x02
#define QSEE_ESE_CPDU_MAX_SZ                   (QSEE_ESE_APDU_HDR_SZ + QSEE_ESE_APDU_EXT_LC_SZ + QSEE_ESE_CPDU_PAYLOAD_SZ + QSEE_ESE_APDU_EXT_LE_SZ_WITH_LC)
#define QSEE_ESE_RPDU_MAX_SZ                   (QSEE_ESE_RPDU_PAYLOAD_SZ + QSEE_ESE_RPDU_STATUS_SZ)
#define QSEE_ESE_APDU_SCP_OVERHEAD             100
#define QSEE_ESE_T1_NAD_BYTE_NO_RSP            0x00
#define QSEE_ESE_T1_NAD_BYTE_CMD               0x5A
#define QSEE_ESE_T1_NAD_BYTE_RSP               0xA5
#define QSEE_ESE_T1_OFFSET_NAD_BYTE            0x00
#define QSEE_ESE_T1_OFFSET_PCB_BYTE            0x01
#define QSEE_ESE_T1_OFFSET_LEN_BYTE            0x02
#define QSEE_ESE_T1_OFFSET_DATA                0x03
#define QSEE_ESE_T1_OFFSET_CLA                (QSEE_ESE_T1_OFFSET_DATA + QSEE_ESE_APDU_OFFSET_CLA)
#define QSEE_ESE_T1_OFFSET_INS                (QSEE_ESE_T1_OFFSET_DATA + QSEE_ESE_APDU_OFFSET_INS)
#define QSEE_ESE_T1_OFFSET_P1                 (QSEE_ESE_T1_OFFSET_DATA + QSEE_ESE_APDU_OFFSET_P1)
#define QSEE_ESE_T1_OFFSET_P2                 (QSEE_ESE_T1_OFFSET_DATA + QSEE_ESE_APDU_OFFSET_P2)
#define QSEE_ESE_T1_OFFSET_LC                 (QSEE_ESE_T1_OFFSET_DATA + QSEE_ESE_APDU_OFFSET_LC)
#define QSEE_ESE_T1_MAX_BWT_MILLISECONDS       1624
#define QSEE_ESE_MAX_NUM_OF_SESSIONS           4
#define QSEE_ESE_MAX_NUMBER_OF_CHANNELS        20
#define QSEE_ESE_BASIC_CHANNEL_NUMBER          0x00
#define QSEE_ESE_T1_POLL_TIMEOUT_MS            50
#define QSEE_ESE_T1_POLL_DELAY_SCALER          1
#define QSEE_ESE_T1_MAX_FRAME_SZ               258
#define QSEE_ESE_AID_MIN_LEN                   5
#define QSEE_ESE_AID_MAX_LEN                   16
#define QSEE_ESE_T1_PCB_SZ                     1
#define QSEE_ESE_T1_NAD_SZ                     1
#define QSEE_ESE_T1_LEN_SZ                     1
#define QSEE_ESE_T1_LRC_SZ                     1
#define QSEE_ESE_T1_SEQN                       0x40 // T1 I-Block Sequence number mask
#define QSEE_ESE_T1_CHAINING                   0x20 // T1 I-Block chaining mask
#define QSEE_ESE_T1_CHAINING_WITH_SEQN        (QSEE_ESE_T1_CHAINING | QSEE_ESE_T1_SEQN)
#define QSEE_ESE_T1_SINGLE_FRAME               0x00
#define QSEE_ESE_T1_BLOCK_MASK                 0xC0    /* Mask for the bits identifying the block type */
#define QSEE_ESE_T1_I_BLOCK_MASK               0x80    /* Identifies an I-Block */
#define QSEE_ESE_T1_I_BLOCK                    0x00    /* Identifies an I-Block */
#define QSEE_ESE_T1_R_BLOCK                    0x80    /* Identifies an R-Block */
#define QSEE_ESE_T1_R_BLOCK_ERROR_MASK         0x83
#define QSEE_ESE_T1_R_BLOCK_ERROR_OTHER        0x82
#define QSEE_ESE_T1_R_BLOCK_ERROR_RC_OR_PARITY 0x81
#define QSEE_ESE_T1_S_BLOCK                    0xC0    /* Identifies an S-Block */
#define QSEE_ESE_T1_S_BLOCK_WTX_REQ            0xC3
#define QSEE_ESE_T1_S_BLOCK_WTX_RSP            0xE3
#define QSEE_ESE_T1_S_RESET_REQ                0xC4
#define QSEE_ESE_T1_S_RESET_RSP                0xE4
#define QSEE_ESE_T1_S_END_PDU_SESSION_REQ      0xC5
#define QSEE_ESE_T1_S_END_PDU_SESSION_RSP      0xE5
#define QSEE_ESE_T1_STATUS_SW1_REVERSE_OFFSET  0x03
#define QSEE_ESE_T1_STATUS_SW2_REVERSE_OFFSET  0x02
#define QSEE_ESE_T1_STATUS_SW1_MORE_DATA       0x61
#define QSEE_ESE_T1_STATUS_SW1_WRONG_LE_FIELD  0x6C
#define QSEE_ESE_T1_STATUS_SW1_SUCCESS         0x90
#define QSEE_ESE_T1_STATUS_SW2_SUCCESS         0x00
#define QSEE_ESE_T1_STATUS_SZ                  0x02
#define QSEE_ESE_T1_HDR_SZ                     (QSEE_ESE_T1_PCB_SZ + QSEE_ESE_T1_NAD_SZ + QSEE_ESE_T1_LEN_SZ)
#define QSEE_ESE_T1_FRAMING_OVERHEAD_SZ        (QSEE_ESE_T1_HDR_SZ + QSEE_ESE_T1_LRC_SZ)
#define QSEE_ESE_T1_MAX_INFO_SZ                (QSEE_ESE_T1_MAX_FRAME_SZ - QSEE_ESE_T1_FRAMING_OVERHEAD_SZ)
#define QSEE_ESE_ARR_SZ(a)                     (sizeof((a)) / sizeof(0[(a)]))
#define QSEE_ESE_MALLOC                        qsee_zalloc
#define QSEE_ESE_REALLOC                       qsee_realloc
#define QSEE_ESE_FREE                          QSEE_FREE_PTR

#define QSEE_ESE_LOG_INF(xx_fmt, ...)          qsee_log(QSEE_LOG_MSG_DEBUG, " %s " xx_fmt, __FUNCTION__, ##__VA_ARGS__)
#define QSEE_ESE_LOG_ERR(xx_fmt, ...)          qsee_log(QSEE_LOG_MSG_ERROR, " %s " xx_fmt, __FUNCTION__, ##__VA_ARGS__)
#define QSEE_ESE_ENTER(xx_fmt, ...)            qsee_log(QSEE_LOG_MSG_DEBUG, "+%s " xx_fmt, __FUNCTION__, ##__VA_ARGS__)
#define QSEE_ESE_EXIT(xx_fmt, ...)             qsee_log(QSEE_LOG_MSG_DEBUG, "-%s " xx_fmt, __FUNCTION__, ##__VA_ARGS__)
// clang-format on

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*
 * Feature mask enabling/disabling several ESE Service functionality
 */
typedef enum {
  QSEE_ESE_OEM_SEAC                 = (1 << 0),
  QSEE_ESE_OEM_SEAC_APDU_FILTERING  = (1 << 1),
  QSEE_ESE_OEM_TRANSMIT_SELECT_CMD  = (1 << 2),
  QSEE_ESE_OEM_SPI_SOFT_RESET       = (1 << 3),
  QSEE_ESE_OEM_SPI_END_APDU_SESSION = (1 << 4),
  QSEE_ESE_OEM_DENY_BASIC_CHANNEL   = (1 << 5),
  QSEE_ESE_OEM_DENY_DEFAULT_APPLET  = (1 << 6),
  QSEE_ESE_OEM_MULTI_BYTE_RSP_NAD   = (1 << 7),
  QSEE_ESE_OEM_ESESERVICE_TA_LOGS   = (1 << 8),
  QSEE_ESE_OEM_FEATURE_MAX          = (1 << 31),
  QSEE_ESE_OEM_FEATURE_MASK         = 0xFFFFFFFF,
  QSEE_ESE_OEM_BWT_SHIFT            = 0,
  QSEE_ESE_OEM_POLL_TIMEOUT_SHIFT   = 12,
  QSEE_ESE_OEM_POLL_DELAY_SHIFT     = 20,
  QSEE_ESE_OEM_POLL_DELAY_MI_SHIFT  = 24,
  QSEE_ESE_OEM_BWT_MASK             = (0x7FF << QSEE_ESE_OEM_BWT_SHIFT),
  QSEE_ESE_OEM_POLL_TIMEOUT_MASK    = (0xFF << QSEE_ESE_OEM_POLL_TIMEOUT_SHIFT),
  QSEE_ESE_OEM_POLL_DELAY_MASK      = (0xF << QSEE_ESE_OEM_POLL_DELAY_SHIFT),
  QSEE_ESE_OEM_POLL_DELAY_MI_MASK   = (0xF << QSEE_ESE_OEM_POLL_DELAY_MI_SHIFT),
} teOemEseMask;

/*
 * Unique identifier for OEM Property
 */
typedef enum {
  QSEE_ESE_OEM_PROP_SPI_DEV_ID,
  QSEE_ESE_OEM_PROP_SPI_CS_ID,
  QSEE_ESE_OEM_PROP_SPI_MAX_FREQ,
  QSEE_ESE_OEM_PROP_SPI_BITS_PER_WORD,
  QSEE_ESE_OEM_PROP_SPI_NAD_CMD,
  QSEE_ESE_OEM_PROP_SPI_NAD_RSP,
  QSEE_ESE_OEM_PROP_FEATURE_MASK,
  QSEE_ESE_OEM_PROP_TIMERS,
  QSEE_ESE_OEM_PROP_POLL_TIMEOUT,
  QSEE_ESE_OEM_PROP_BWT,
  QSEE_ESE_OEM_PROP_MULTI_BYTE_NAD
} teOemEseProperty;

/**
 * @brief QCOM return codes for QSEE eSE Service. These need to be mapped to
 *        appropriate codes for GPTEE SE API interface. The can be done from
 *        within the TA app or TEE library
 */
typedef enum {
  QSEE_ESE_SERVICE_STATUS_SUCCESS,               /**< Operation has been successful */
  QSEE_ESE_SERVICE_STATUS_ERROR_BAD_STATE,       /**< QCOM: Tried to resume a blocked API when in unexpected state */
  QSEE_ESE_SERVICE_STATUS_ERROR_GENERIC,         /**< Generic error code for some unknown failure */
  QSEE_ESE_SERVICE_STATUS_ERROR_BAD_PARAMETERS,  /**< Bad arguments supplied to the API call */
  QSEE_ESE_SERVICE_STATUS_ERROR_ITEM_NOT_FOUND,  /**< Handle supplied cannot be found */
  QSEE_ESE_SERVICE_STATUS_ERROR_OUT_OF_MEMORY,   /**< Out of memory when creating new handles */
  QSEE_ESE_SERVICE_STATUS_ERROR_COMMUNICATION,   /**< Problems with SPI communication */
  QSEE_ESE_SERVICE_STATUS_ERROR_SHORT_BUFFER,    /**< Read buffer supplied not big enough for message received */
  QSEE_ESE_SERVICE_STATUS_ERROR_ACCESS_CONFLICT, /**< Multiple entities trying to access same resource */
  QSEE_ESE_SERVICE_STATUS_ERROR_ACCESS_DENIED,   /**< Multiple entities trying to access same resource */
  QSEE_ESE_SERVICE_STATUS_ERROR_TIMEOUT          /**< Timeout waiting for a response */
} qsee_ese_service_status_t;

/**
 * Type of timeout variable
 */
typedef uint32_t qsee_ese_service_timeout_t;

/**
 * @brief C-APDU
 */
typedef struct
{
  uint32_t size_write;                     /**< number of bytes to write */
  uint8_t  buff[QSEE_ESE_T1_MAX_FRAME_SZ]; /**< address for write data */
} qsee_ese_service_capdu_t;

/**
 * @brief R-APDU
 */
typedef struct
{
  uint32_t size_read; /**< number of bytes read which is initialised to zero  */
  uint8_t  buff[QSEE_ESE_T1_MAX_FRAME_SZ]; /**< address for read data */
} qsee_ese_service_rapdu_t;

/**
 * Checks if Secure Element Access Control is enabled in QSEE
 *
 * @return true if enabled, otherwise disable
 */
bool qsee_ese_service_seac_enabled(void *pContext);

/**
 * Checks if Secure Element Access Control APDU Filtering is enabled in QSEE
 *
 * @return true if enabled, otherwise disable
 */
bool qsee_ese_service_seac_apdu_filtering_enabled(void *pContext);

/**
 * Checks if Secure Element GP API is allowed to send SELECT command using
 * the TEE_SEChannelTransmit API
 *
 * @return true if enabled, otherwise disable
 */
bool qsee_ese_service_transmit_select_cmd_enabled(void *pContext);

/**
 * Checks if ESE Service can send SPI soft reset to the ESE. This is inorder to
 * reset SPI T=1 protocol parameters, SPI interface and JCOP context w.r.t to
 * SPI interface
 *
 * @return true if enabled, otherwise disable
 */
bool qsee_ese_service_spi_soft_reset_enabled(void *pContext);

/**
 * Checks if ESE Service can send end of APDU session message to indicate the
 * end of an active APDU session over SPI interface. JCOP may put the chip to
 * low power mode if no APDU session is active over all the supported interfaces
 *
 * @return true if enabled, otherwise disable
 */
bool qsee_ese_service_spi_end_apdu_session_enabled(void *pContext);

/**
 * Checks if TEE SE API library is configured to block access to basic channel.
 *
 * Access to basic channel is allowed by default. So it must be explicitly blocked
 * via devcfg oem_config.xml
 *
 * @return true if blocked, otherwise allowed
 */
bool qsee_ese_service_block_access_basic_channel(void *pContext);

/**
 * Checks if TEE SE API library is configured to block access to default applet
 * selection if no AID is specified
 *
 * Access to default applet is allowed by default. So it must be explicitly blocked
 * via devcfg oem_config.xml
 *
 * @return true if blocked, otherwise allowed
 */
bool qsee_ese_service_block_access_default_applet(void *pContext);

/**
 * Gets the feature mask enabling/disabling several ESE Service functionality
 *
 * @return a value of the ESE feature mask from OEM configuration file
 */
uint32_t qsee_ese_service_feature_mask(void *pContext);

/**
 * Gets the timers mask the ESE timers
 *
 * @return a value of the ESE timer mask from OEM configuration file
 *
 */
uint32_t qsee_ese_service_get_timer(void *pContext);

/**
 * Gets the Secure Element Maximum Block Waiting (BWT)
 *
 * @return BWT value
 *
 */
uint32_t qsee_ese_service_get_bwt(void *pContext);

/**
 * Gets the SPI polling time for the Secure Element Response messahe
 *
 * @return polling timer value
 *
 */
uint32_t qsee_ese_service_get_poll_timeout(void *pContext);

/**
 * Gets the T=1 NAD used for the C-APDU send to the Secure Element
 *
 * @return NAD byte used in commands sent to the Secure Element
 */

uint8_t qsee_ese_service_get_cmd_nad(void *pContext);

/**
 * Gets the T=1 NAD used for the R-APDU expected from the Secure Element
 *
 * @return NAD byte used in response from Secure Element
 */

uint8_t qsee_ese_service_get_rsp_nad(void *pContext);

/**
 * Function retrieves the SPI device identifier used by Secure Element from devcfg
 *
 * @return Valid SPI identifier
 */
qsee_spi_device_id_t qsee_ese_service_oem_get_spi_id(void *pContext);

/**
 * Function retrieves the chip select identifier used by Secure Element from devcfg
 *
 * @return Valid value for the chip select identifier used by Secure Element
 */
uint8_t qsee_ese_service_oem_get_chip_select_id(void *pContext);

/**
 * Function retrieves the maximum SPI frequency used by Secure Element from devcfg
 *
 * @return Valid frequency. Defaults to 9600000 if cannot be retrieved from devcfg
 */
uint32_t qsee_ese_service_oem_get_spi_max_frequency(void *pContext);

/**
 * Function retrieves the SPI number of bits per work used by Secure Element from devcfg
 *
 * @return Valid number of SPI bits per work. Defaults 8 if cannot retrieved from devcfg
 */
uint8_t qsee_ese_service_oem_get_spi_bits_per_word(void *pContext);

/**
 * Waits for the specified number of milliseconds
 *
 * @param [in]  milliseconds: The number of milliseconds to wait
 */
uint32_t qsee_ese_service_wait(qsee_ese_service_timeout_t milliseconds);

/**
 * Open a logical connection using the handle to communicate over SPI.
 *
 * @param [in,out] ppContext The ESE pContext opaque pointer
 *
 * @return QSEE_ESE_SERVICE_STATUS_SUCCESS on success, otherwise failure
 *
 * @see qsee_ese_service_close
 *
 */
int qsee_ese_service_open(void **ppContext);

/**
 * Close a logical connection using the handle.
 *
 * @param [in,out] pContext The ESE pContext opaque pointer
 *
 * @return QSEE_ESE_SERVICE_STATUS_SUCCESS on success, otherwise failure
 *
 * @see qsee_ese_service_open
 *
 */
int qsee_ese_service_close(void *pContext);

/**
 * Perform the Secure Element Access Control based on Application Identifier and/or
 * Application Protocol Data Unit
 *
 * @param [in]     pContext The ESE pContext opaque pointer
 * @param [in]     aid     The Applet Application Identifier
 *                         NULL means no AID filtering should not be applied
 * @param [in]     apdu    The APDU in which SEAC filtering should be applied
 *                         NULL means no APDU filtering should not be applied
 *
 * @return Object_OK on success. Otherwise access is denied
 */
int qsee_ese_service_seac(void *         pContext,
                          const uint8_t *aid,
                          uint32_t       aid_size,
                          const uint8_t *apdu,
                          uint32_t       apdu_size);

/**
 * Writes to slave data on the logical connection handle.
 *
 * @param [in]     pContext  The ESE pContext opaque pointer
 * @param [in]     c_apdu   Write buffer information.
 *
 * @return
 *       QSEE_ESE_SERVICE_STATUS_PENDING_IRQ successful write of C-APDU \n
 *       QSEE_ESE_SERVICE_STATUS_PENDING_CHANNEL Did not write since waiting for R-APDU \n
 *       Any other eason code as per TEE SE API interface \n
 *
 * @see qsee_ese_service_transceive
 *
 * @pre qsee_ese_service_open
 *
 */
int qsee_ese_service_write(void *pContext, qsee_ese_service_capdu_t *c_apdu);

/**
 * Reads from slave data on the logical connection handle.
 *
 * @param [in]      pContext       The ESE pContext opaque pointer
 * @param [in,out]  r_apdu        Read buffer information.
 *
 * @return
 *       QSEE_ESE_SERVICE_STATUS_SUCCESS If successfully read the data \n
 *       Any other eason code as per TEE SE API interface \n
 *
 * @see qsee_ese_service_transceive
 *
 * @pre qsee_ese_service_open
 *
 */
int qsee_ese_service_read(void *pContext, qsee_ese_service_rapdu_t *r_apdu);

/*
 * Checks if T=1 Protocol Control Byte has error indications
 *
 * @return true if error bits set. false otherwise
 */
bool qsee_ese_service_rsp_pcb_error(uint8_t pcb);

#endif // QSEE_ESE_SERVICE_H
