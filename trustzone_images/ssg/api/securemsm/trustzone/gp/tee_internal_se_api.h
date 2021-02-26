#ifndef TEE_INTERNAL_SE_API_H
#define TEE_INTERNAL_SE_API_H

#include <stdint.h>
#include <stdbool.h>
#include "tee_internal_api.h"

// clang-format off

#define TEE_SE_API_MAJOR_VERSION       (1)
#define TEE_SE_API_MINOR_VERSION       (1)
#define TEE_SE_API_MAINTENANCE_VERSION (1)
#define TEE_SE_API_VERSION             (TEE_SE_API_MAJOR_VERSION << 24) + \
                                       (TEE_SE_API_MINOR_VERSION << 16) + \
                                       (TEE_SE_API_MAINTENANCE_VERSION << 8)

#define TEE_SE_API_1_1_1

#define TEE_ERROR_61_SHORT_BUFFER 0xF0240001
#define TEE_ERROR_6C_SHORT_BUFFER 0xF0240002
#define TEE_SE_SESSION_OPEN       0x00240000

// clang-format on

typedef struct __TEE_SEReaderProperties
{
  bool sePresent;            // true if an SE is present in the reader
  bool teeOnly;              // true if this reader is only accessible via
                             // the TEE
  bool selectResponseEnable; // true if the response to a SELECT is available
                             // in the TEE
} TEE_SEReaderProperties;

typedef struct __TEE_SEAID
{
  uint8_t *buffer;    // the value of the applet’s AID
  uint32_t bufferLen; // length of the applet’s AID
} TEE_SEAID;

typedef struct __TEE_SEServiceHandle *TEE_SEServiceHandle;
typedef struct __TEE_SEReaderHandle * TEE_SEReaderHandle;
typedef struct __TEE_SESessionHandle *TEE_SESessionHandle;
typedef struct __TEE_SEChannelHandle *TEE_SEChannelHandle;

TEE_Result TEE_SEServiceOpen(TEE_SEServiceHandle *seServiceHandle // [out]
                             );
void TEE_SEServiceClose(TEE_SEServiceHandle seServiceHandle);
TEE_Result TEE_SEServiceGetReaders(TEE_SEServiceHandle seServiceHandle,
                                   TEE_SEReaderHandle *seReaderHandleList,   // [out]
                                   uint32_t *          seReaderHandleListLen // [inout]
                                   );

void TEE_SEReaderGetProperties(TEE_SEReaderHandle      seReaderHandle,
                               TEE_SEReaderProperties *readerProperties // [out]
                               );
TEE_Result TEE_SEReaderGetName(TEE_SEReaderHandle seReaderHandle,
                               char *             readerName,
                               uint32_t *         readerNameLen // [outstring]
                               );
TEE_Result TEE_SEReaderOpenSession(TEE_SEReaderHandle   seReaderHandle,
                                   TEE_SESessionHandle *seSessionHandle // [out]
                                   );
void TEE_SEReaderCloseSessions(TEE_SEReaderHandle seReaderHandle);

TEE_Result TEE_SESessionGetATR(TEE_SESessionHandle seSessionHandle,
                               void *              atr,
                               uint32_t *          atrLen // [outbuf]
                               );
TEE_Result TEE_SESessionIsClosed(TEE_SESessionHandle seSessionHandle);
void TEE_SESessionClose(TEE_SESessionHandle seSessionHandle);
void TEE_SESessionCloseChannels(TEE_SESessionHandle seSessionHandle);
TEE_Result TEE_SESessionOpenBasicChannel(TEE_SESessionHandle  seSessionHandle,
                                         TEE_SEAID *          seAID,          // [in]
                                         TEE_SEChannelHandle *seChannelHandle // [out]
                                         );
TEE_Result TEE_SESessionOpenLogicalChannel(TEE_SESessionHandle  seSessionHandle,
                                           TEE_SEAID *          seAID,          // [in]
                                           TEE_SEChannelHandle *seChannelHandle // [out]
                                           );

void TEE_SEChannelClose(TEE_SEChannelHandle seChannelHandle);
TEE_Result TEE_SEChannelSelectNext(TEE_SEChannelHandle seChannelHandle);
TEE_Result TEE_SEChannelGetSelectResponse(TEE_SEChannelHandle seChannelHandle,
                                          void *              response,
                                          uint32_t *          responseLen // [outbuf]
                                          );
TEE_Result TEE_SEChannelTransmit(TEE_SEChannelHandle seChannelHandle,
                                 void *              command,
                                 uint32_t            commandLen, // [inbuf]
                                 void *              response,
                                 uint32_t *          responseLen // [outbuf]
                                 );
TEE_Result TEE_SEChannelGetResponseLength(TEE_SEChannelHandle seChannelHandle,
                                          uint32_t *          responseLen // [outbuf]
                                          );

/* --------------------
 * Discovery API
 * -------------------- */
typedef struct __TEE_SEDiscoveryHandle *TEE_SEDiscoveryHandle;

TEE_Result TEE_SEDiscoveryByAIDInit(TEE_SEServiceHandle    seServiceHandle,
                                    TEE_SEDiscoveryHandle *seDiscoveryHandle, // [out]
                                    TEE_SEAID *            seAID              // [in]
                                    );
TEE_Result TEE_SEDiscoveryByHistoricalBytesInit(TEE_SEServiceHandle    seServiceHandle,
                                                TEE_SEDiscoveryHandle *seDiscoveryHandle, // [out]
                                                void *                 histBytes,
                                                uint32_t               histBytesLen // [inbuf]
                                                );
TEE_Result TEE_SEDiscoveryByATRInit(TEE_SEServiceHandle    seServiceHandle,
                                    TEE_SEDiscoveryHandle *seDiscoveryHandle, // [out]
                                    void *                 atr,
                                    uint32_t               atrLen, // [inbuf]
                                    void *                 mask,
                                    uint32_t               maskLen // [inbuf]
                                    );
TEE_Result TEE_SEDiscoveryFirstMatch(TEE_SEDiscoveryHandle seDiscoveryHandle,
                                     TEE_SEReaderHandle *  seReaderHandle // [out]
                                     );
TEE_Result TEE_SEDiscoveryNextMatch(TEE_SEDiscoveryHandle seDiscoveryHandle,
                                    TEE_SEReaderHandle *  seReaderHandle // [out]
                                    );
bool TEE_SEDiscoveryIsDone(TEE_SEDiscoveryHandle seDiscoveryHandle);
void TEE_SEDiscoveryClose(TEE_SEDiscoveryHandle seDiscoveryHandle);

/* --------------------
 * Secure Channel API
 * -------------------- */
typedef struct __TEE_SC_OID
{
  uint8_t *buffer;    // the value of the OID
  uint32_t bufferLen; // length of the SC OID
} TEE_SC_OID;

typedef enum {
  TEE_SC_NO_SECURE_MESSAGING = 0x00,  // Nothing will be applied
  TEE_SC_AUTHENTICATE        = 0x80,  // Command, Response APDU not secured
  TEE_SC_C_MAC               = 0x01,  // Command APDU shall be MAC protected
  TEE_SC_R_MAC               = 0x10,  // Response APDU shall be MAC protected
  TEE_SC_CR_MAC              = 0x11,  // Command, Response APDU shall be MAC protected
  TEE_SC_C_ENC               = 0x02,  // Command APDU shall be encrypted
  TEE_SC_R_ENC               = 0x20,  // Response APDU shall be encrypted
  TEE_SC_CR_ENC              = 0x22,  // Command, Response APDU shall be encrypted
  TEE_SC_C_ENC_MAC           = 0x03,  // Command APDU shall be encrypted and MAC protected
  TEE_SC_R_ENC_MAC           = 0x30,  // Response APDU encrypted, MAC protected
  TEE_SC_CR_ENC_MAC          = 0x33,  // Command, Response APDU encrypted, MAC protected
  TEE_SC_C_ENC_CR_MAC        = 0x13   // Command APDU encrypted,
                                      // Command and Response APDUs MAC protected
} TEE_SC_SecurityLevel;

/* The following allows a typo in version 1.1 of the GP specification        */
/* to be corrected in version 1.1.1, without breaking backward compatibility */
#define TEE_AUTHENTICATE TEE_SC_AUTHENTICATE

typedef struct __TEE_SC_CardKeyRef
{
  uint8_t scKeyID;      // key identifier of the SC card key
  uint8_t scKeyVersion; // key version of the SC card key
} TEE_SC_CardKeyRef;

typedef enum {
  TEE_SC_BASE_KEY = 0, // A base key acc. to SCP02
  TEE_SC_KEY_SET  = 1  // A key set (key-ENC, key-MAC) acc. to SCP02, SCP03
} TEE_SC_KeyType;

typedef struct __TEE_SC_KeySetRef
{
  TEE_ObjectHandle scKeyEncHandle; // the Key-ENC (static encryption key)
  TEE_ObjectHandle scKeyMacHandle; // the Key-MAC (static MAC key)
} TEE_SC_KeySetRef;

typedef struct __TEE_SC_DeviceKeyRef
{
  TEE_SC_KeyType scKeyType; // type of SC keys
  union {
    TEE_ObjectHandle scBaseKeyHandle; // SC base key (acc. to SCP02)
    TEE_SC_KeySetRef scKeySetRef;     // Key-ENC, Key-MAC (acc. to SCP02, SCP03)
  } __TEE_key;
} TEE_SC_DeviceKeyRef;

typedef struct __TEE_SC_Params
{
  uint8_t              scType;          // the SC type
  TEE_SC_OID           scOID;           // the SC type defined by OID
  TEE_SC_SecurityLevel scSecurityLevel; // the SC security level
  TEE_SC_CardKeyRef    scCardKeyRef;    // reference to SC card keys
  TEE_SC_DeviceKeyRef  scDeviceKeyRef;  // reference to SC device keys
} TEE_SC_Params;

#define TEE_SC_TYPE_SCP02             0x00
#define TEE_SC_TYPE_SCP03             0x01
#define TEE_SC_TYPE_SCP11a            0x02
#define TEE_SC_TYPE_SCP11b            0x03

#define TEE_SC_LEVEL_NO_SECURITY      0x00
#define TEE_SC_LEVEL_AUTHENTICATED    0x80
#define TEE_SC_LEVEL_C_MAC            0x01
#define TEE_SC_LEVEL_R_MAC            0x10
#define TEE_SC_LEVEL_CR_MAC           0x11
#define TEE_SC_LEVEL_C_ENC            0x02
#define TEE_SC_LEVEL_R_ENC            0x20
#define TEE_SC_LEVEL_CR_ENC           0x22
#define TEE_SC_LEVEL_C_ENC_MAC        0x03
#define TEE_SC_LEVEL_R_ENC_MAC        0x30
#define TEE_SC_LEVEL_CR_ENC_MAC       0x33
#define TEE_SC_LEVEL_C_ENC_CR_MAC     0x13

/**
 * An alternative way to define the SCP type is using an OID
 */
#define TEE_OID_SCP02                                          \
  {                                                            \
    0x06, 0x08, 0x2A, 0x86, 0x48, 0x86, 0xFC, 0x6B, 0x04, 0x02 \
  }
#define TEE_OID_SCP03                                          \
  {                                                            \
    0x06, 0x08, 0x2A, 0x86, 0x48, 0x86, 0xFC, 0x6B, 0x04, 0x03 \
  }
#define TEE_OID_SCP11a                                               \
  {                                                                  \
    0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xFC, 0x6B, 0x04, 0x0B, 0x01 \
  }
#define TEE_OID_SCP11b                                               \
  {                                                                  \
    0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xFC, 0x6B, 0x04, 0x0B, 0x02 \
  }

TEE_Result TEE_SESecureChannelOpen(TEE_SEChannelHandle seChannelHandle,
                                   TEE_SC_Params *     scParams // [in]
                                   );
int TEE_SESecureChannelGetSecurityLevel(TEE_SEChannelHandle seChannelHandle);
void TEE_SESecureChannelClose(TEE_SEChannelHandle seChannelHandle);

#endif /* ifndef TEE_INTERNAL_SE_API_H */
