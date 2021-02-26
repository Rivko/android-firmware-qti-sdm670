#ifndef _SECBOOT_PKCS7_I_H
#define _SECBOOT_PKCS7_I_H

/** @file SecBoot_pkcs7_i.h

DESCRIPTION:
    Secure Boot PKCS#7 parser internal helper funcitons
 
  Copyright (c)2010 Qualcomm Technologies Incorporated. All Rights Reserved.
**/


#include "com_dtypes.h"
#include "SecBoot_pkcs7.h"
#include "SecBoot_types.h"



/*==========================================================================
                             Macro Definitions
===========================================================================*/
/*

   See http://www.alvestrand.no/objectid/ for details on these OIDS

   ASN.1 spaces we collapse into some IDs for convenience when fetching
   some cert parts. The short answer: pick the value below you want
   and pass it to X509Chain_GetFieldPart.

   There are some limited number of OID sub-spaces that are interesting to
   us. In each of those spaces a few to maybe a few hundred OID's are of
   interest.
   Thus we divide up a 32 bit integeter with the upper 8 bits representing
   a sub space, and the lower 24 the values in the subspace.

   The thing that makes this work is the last byte of an OID is not encoded
   in weird ways like the others so we can pattern match down to a subspace
   and then just OR in the last byte to get the values below (if that makes
   sense)

   Because there's so many OID's that fit into these spaces that are probably
   of interest to a only a few humans on the planet, we don't define constants
   for all of them, but you if you happen to be an interested human, you can
   still take advantage of this code with the ASNOID_MATCH_SUBSPACE macro.
 */
#define ASNOID_UNDEFINED  0x00        // Unknown to us, good luck and have fun!
#define ASNOID_SUBSPACES  0xff000000  // Mask for subspaces
#define ASNOID_MATCH_SUBSPACE(oid, subspace) ((oid & ASNOID_SUBSPACES) == subspace)

#define ASNOID_UNKNOWN    0xff000001
#define ASNOID_ANY        0xff000002
#define ASNOID_END        0xff000003

// 2.5.4 This is the joint ITU/ISO X.500 attribute tree and
// encompases things used in distinguished names
// Binary DER prefix \x06\0x03\x55\x04
#define ASNOID_X500     0x01000000 // The X.500 subspace of ASNOIDs
#define ASNOID_X500_CN  0x01000003 // 2.5.4.3  commonName
#define ASNOID_X500_CO  0x01000006 // 2.5.4.6  country
#define ASNOID_X500_LO  0x01000007 // 2.5.4.7  locality Name
#define ASNOID_X500_ST  0x01000008 // 2.5.4.8  stateOrProvinceName
#define ASNOID_X500_O   0x0100000A // 2.5.4.10 organizationName
#define ASNOID_X500_OU  0x0100000B // 2.5.4.11 organizationUnitName

// 1.2.840.113549.1.1 This is PKCS1 space. Part of PKCS, which is part
// of RSADSI, which is part of the US which is an ISO member body...
// Binary DER prefix is \x06\x09\x2a\x86\x48\x86\xf7\x0d\x01\x01
#define ASNOID_PKCS1            0x02000000 // The PKCS1 subspace of ASNOIDS
#define ASNOID_PKCS1_RSA        (ASNOID_PKCS1 |  1) // 1.2.840.113549.1.1.1 rsaEncryption
#define ASNOID_PKCS1_RSA_MD2    (ASNOID_PKCS1 |  2) // 1.2.840.113549.1.1.2 MD2 with rsaEncryption
#define ASNOID_PKCS1_RSA_MD5    (ASNOID_PKCS1 |  4) // 1.2.840.113549.1.1.4 md5withRSAEncryption
#define ASNOID_PKCS1_RSA_SHA1   (ASNOID_PKCS1 |  5) // 1.2.840.113549.1.1.5 sha1withRSAEncryption
#define ASNOID_PKCS1_RSA_SHA256 (ASNOID_PKCS1 | 11) // 1.2.840.113549.1.1.11 sha256WithRSAEncryption
#define ASNOID_PKCS1_RSA_SHA384 (ASNOID_PKCS1 | 12) // 1.2.840.113549.1.1.12 sha384WithRSAEncryption
#define ASNOID_PKCS1_RSA_SHA512 (ASNOID_PKCS1 | 13) // 1.2.840.113549.1.1.13 sha256WithRSAEncryption
#define ASNOID_PKCS1_RSA_SHA224 (ASNOID_PKCS1 | 14) // 1.2.840.113549.1.1.14 sha256WithRSAEncryption


// 1.3.14.3.2 This is secsig subspace, part of OIW, part of ISO orgs
// Seems like lots of crypto algorithms identified here. Might be worth
// adding more defines than we have...
// Binary DER prefix is \x06\x05\x2b\x0e\x03x02
#define ASNOID_SECSIG         0x03000000 // The secsig subspace of ASNOIDS
#define ASNOID_SECSIG_SHA1    0x0300001A // 1.3.14.3.2.26 SHA-1 hash

// 2.5.29. This is the joint ITU/ISO X.500 directory tree area for certificate extensions
// There is probably more here that would be useful to define constants for
// Binary DER prefix is \x06\x03\x55\x1d
#define ASNOID_CERTEXT              0x04000000 // The cert extensions subspace of ASNOIDS
#define ASNOID_CERTEXT_POLICIES     0x04000003 // 2.5.29.3  Certificate policies
#define ASNOID_CERTEXT_SUBJKEY_ID   0x0400000E // 2.5.29.14 Subject Key Identifier
#define ASNOID_CERTEXT_KEY_USE      0x0400000F // 2.5.29.15 Key usage
#define ASNOID_CERTEXT_SUBJALTNAME  0x04000010 // 2.5.29.16 Subject Alternate name
#define ASNOID_CERTEXT_ISSALTNAME   0x04000011 // 2.5.29.17 Issuer Alternate name
#define ASNOID_CERTEXT_B_CONSTRAIN  0x04000013 // 2.5.29.19 Basic constraints
#define ASNOID_CERTEXT_CRLDIT       0x0400001F // 2.5.29.31 CRL distribution points
#define ASNOID_CERTEXT_POLICIES2    0x04000020 // 2.5.29.32 Cert policies
#define ASNOID_CERTEXT_AUTHKEY_ID   0x04000023 // 2.5.29.35 Authority Key Identifier
#define ASNOID_CERTEXT_EXT_KEY_USE  0x04000025 // 2.5.29.37 Extended Key usage

// 2.16.840.1.113730.1 ISO/ITU joint assignments by country - USA companies, Netscape
// Netscape certificate extensions
// Binary DER prefix is \x06\x09\x60\x86\x48\x01\x86\xf8\x42\x01
#define ASNOID_NETSCAPE_CERT_EXT      0x05000000  // Netscape substapce of ASNOIDS
#define ASNOID_NETSCAPE_CERT_EXT_TYPE 0x05000001  // 2.16.840.1.113730.1.1 - Netscape cert type
#define ASNOID_NETSCAPE_CERT_EXT_GEN  0x0500000D  // 2.16.840.1.113730.1.13 - Generated by

// 2.16.840.1.113733.1.7.1 ISO/ITU joint assignments by country - USA companies, Verisign
// Certificate policy
// Binary DER prefix is \x06\x0B\x60\x86\x48\x01\x86\xf8\x45\x01\x07\x01
#define ASNOID_VERISIGN_POLICY        0x06000000  // Verisign Certificate Policy
#define ASNOID_VERISIGN_POLICY_QUAL1  0x06000001  // Verisign Policy Qualifier
#define ASNOID_VERISIGN_POLICY_QUAL2  0x06000002  // Verisign Policy Qualifier

// 1.3.6.1.4.1.311.10.3 ISO identified orgs, US DoD, Internet assignments, private,
// IANA regestered enterprises, Microsoft
// Binary DER prefix is \x06\x0A\x2b\x06\x01\x04\x01\x82\x37\x01\x03
#define ASNOID_MICROSOFT              0x07000000  // A subspace of Microsoft
#define ASNOID_MICROSOFT_SGC          0x07000003  // Sever Gated Cryptography

// 2.16.840.1.113730.4 ISO/ITU joint assignments by country - USA companies, Netscape
// International Step up
// Binary DER prefix is \x06\x09\x60\x86\x48\x01\x86\xf8\x42\x04
#define ASNOID_NETSCAPE_STEP        0x08000000  // Netscape subpace of ASNOIDS
#define ASNOID_NETSCAPE_STEP_UP     0x08000001  // 2.16.840.1.113730.4 - International step up

// 1.2.840.113549.1.9 This is PKCS9 space. Part of PKCS, which is part
// of RSADSI, which is part of the US which is an ISO member body...
// Binary DER prefix is \x06\x09\x2a\x86\x48\x86\xf7\x0d\x01\x09
#define ASNOID_PKCS9          0x09000000 // The PKCS9 subspace of ASNOIDS
#define ASNOID_PKCS9_EMAIL    0x09000001 // 1.2.840.113549.1.9.1 email address

#define ASNOID_PKCS9_CONTENTTYPE   0x09000003 // 1.2.840.113549.1.9.3 Content Type
#define ASNOID_PKCS9_MESSAGEDIGEST 0x09000004 // 1.2.840.113549.1.9.4 Message Digest

// 1.3.6.1.5.5.7.1 This ISO identified orgs, US Dod, Internet assignments,
// IANA security, mechanisms, pkix, private cert extensions
// Binary DER prefix is \x06\x08\x2B\x06\x01\x05\x05\x07\x01
#define ASNOID_PKIXPRIV           0x0a000000 // The Private cert extension PKIX subspace of ASNOIDS
#define ASNOID_PKIXPRIV_AUTHINFO  0x0a000001 // 1.3.6.1.5.5.7.1.1 The Authority Info Access field

// 2.16.840.1.113733.1.6 ISO/ITU joint assignments by country - USA companies, Verisign
// Binary DER prefix is \x06\x0a\x60\x86\x48\x01\x86\xf8\x45\x01\x06
#define ASNOID_VERISIGNPRIV          0x0b000000  // Verisign privately assigned
#define ASNOID_VERISIGNPRIV_BREWPERM 0x0b000011  // Brew digital sig permissions

// 1.2.840.113549.2 These are digest algorithms in PKCS1 space. Part of PKCS, which is part
// of RSADSI, which is part of the US which is an ISO member body...
// Binary DER prefix is  \x06\x08\x2a\x86\x48\x86\xf7\x0d\x02
#define ASNOID_PKCS1DIGEST         0x0c000000
#define ASNOID_PKCS1DIGEST_MD2     0x0c000002 // The MD2 algorithm
#define ASNOID_PKCS1DIGEST_MD5     0x0c000005 // The MD5 algorithm
//      The following OID is wrong, RSA did not define an OID
//      for SHA-1, it defined an OID for HMAC-SHA1.
#define ASNOID_PKCS1DIGEST_SHA1    0x0c000007 // DO NOT USE with new software
#define ASNOID_PKCS1DIGEST_HMACSHA1 0x0c000007 // The HMAC-SHA1 algorithm

// 1.3.6.1.5.5.7.3 This ISO identified orgs, US Dod, Internet assignments,
// IANA security, mechanisms, pkix, extended key purpose
// Binary DER prefix is \x06\x08\x2B\x06\x01\x05\x05\x07\x03
#define ASNOID_EXTKEYUSE           0x0d000000 // Extened key use of PKIX subspace of ASNOIDS
#define ASNOID_EXTKEYUSE_SRVRAUTH  0x0d000001 // 1.3.6.1.5.5.7.3.1 TLS Server Authentication
#define ASNOID_EXTKEYUSE_CLNTAUTH  0x0d000002 // 1.3.6.1.5.5.7.3.2 TLS Client Authentication
#define ASNOID_EXTKEYUSE_CODESIGN  0x0d000003 // 1.3.6.1.5.5.7.3.3 Code Signing
#define ASNOID_EXTKEYUSE_EMAIL     0x0d000004 // 1.3.6.1.5.5.7.3.4 Email Protection
#define ASNOID_EXTKEYUSE_IPSECEND  0x0d000005 // 1.3.6.1.5.5.7.3.5 IPsec End System
#define ASNOID_EXTKEYUSE_IPSECTUNN 0x0d000006 // 1.3.6.1.5.5.7.3.6 IPsec Tunnel
#define ASNOID_EXTKEYUSE_IPSECUSER 0x0d000007 // 1.3.6.1.5.5.7.3.7 IPsec User
#define ASNOID_EXTKEYUSE_IPSEC ASNOID_EXTKEYUSE_IPSECUSER // backwards compatibility
#define ASNOID_EXTKEYUSE_TIMESTAMP 0x0d000008 // 1.3.6.1.5.5.7.3.8 Time Stamping
#define ASNOID_EXTKEYUSE_OCSPSIG   0x0d000009 // 1.3.6.1.5.5.7.3.9 OCSP Signing

// 1.3.6.1.4.1.1449.9.3 This ISO identified orgs, US Dod, Internet assignments,
// IANA regestered enterprises, Qualcomm, QIS, Brew private cert hierarchy X.509 extensions
// Binary DER prefix is \x06\x08\x2B\x06\x01\x04\x01\xb8\x29\x09\x03
#define ASNOID_BREWX509             0x0e000000 // Brew private X.509 extensions
#define ASNOID_BREWX509_CARRIERID   0x0e00000a // 1.3.6.1.4.1.1449.9.3.10 Brew-assigned carriers IDs
// Note that ASNOID_VERISIGNPRIV_BREWPERM above is the former incarnation of this
#define ASNOID_BREWX509_PLATFORMID  0x0e00000b // 1.3.6.1.4.1.1449.9.3.11 Brew-assigned platform IDs
#define ASNOID_BREWX509_BREWCLASSID 0x0e00000c // 1.3.6.1.4.1.1449.9.3.12 Brew class IDs
// -- following are extended key usage in Brew --
#define ASNOID_BREWX509_BREWAPP     0x0e000001 // 1.3.6.1.4.1.1449.9.3.1 Brew extended key usage, app signed permissions
#define ASNOID_BREWX509_TESTENABLE  0x0e000002 // 1.3.6.1.4.1.1449.9.3.2 Brew test enable signature
#define ASNOID_BREWX509_ADSAUTH     0x0e000003 // 1.3.6.1.4.1.1449.9.3.3 Authentication of ADS's
#define ASNOID_BREWX509_TRUEBREW    0x0e000004 // 1.3.6.1.4.1.1449.9.3.4 True Brew
// -- some server-side extensions --
#define ASNOID_BREWX509_S2SVBPAYID  0x0e000010 // 1.3.6.1.4.1.1449.9.3.16 Payee ID for server-server value billing
#define ASNOID_BREWX509_S2SVBCARID  0x0e000011 // 1.3.6.1.4.1.1449.9.3.17 Carrier ID for server-server value billing


// 2.16.840.1.101.3.4.2 These are the NIST digest algorithms
#define ASNOID_NIST_DIGEST          0x0F000000 // NIST Digest algorithms subspace of ASNOIDS
#define ASNOID_NIST_DIGEST_SHA256   (ASNOID_NIST_DIGEST | 1)
#define ASNOID_NIST_DIGEST_SHA384   (ASNOID_NIST_DIGEST | 2)
#define ASNOID_NIST_DIGEST_SHA512   (ASNOID_NIST_DIGEST | 3)
#define ASNOID_NIST_DIGEST_SHA224   (ASNOID_NIST_DIGEST | 4)

// 1.3.6.1.4.1.1449.9.4 Component Services arc
// 1.3.6.1.4.1.1449.9.4.1 Component Services arc for ASN1 data structures
#define ASNOID_APIONE_STRUCTURES          0x10000000
#define ASNOID_APIONE_SIGFILEENV          0x10000001 // 1.3.6.1.4.1.1449.9.4.1.1 SigFileEnvelope (unused?)
#define ASNOID_APIONE_CODESIGINFO         0x10000002 // 1.3.6.1.4.1.1449.9.4.1.2
#define ASNOID_APIONE_SIGBLOCKINFO        0x10000003 // 1.3.6.1.4.1.1449.9.4.1.3
#define ASNOID_APIONE_CODESIGINFO2        0x10000004 // 1.3.6.1.4.1.1449.9.4.1.4

// Component Services certificate extensions
#define ASNOID_APIONE_TRUSTEDCODEGROUPS   0x1000000a // 1.3.6.1.4.1.1449.9.4.1.10
#define ASNOID_APIONE_PRIVILEGES          0x1000000b // 1.3.6.1.4.1.1449.9.4.1.11
#define ASNOID_APIONE_CODESIGCAPABILITIES 0x1000000c // 1.3.6.1.4.1.1449.9.4.1.12
// Extended key use indicating Component Services code signing
#define ASNOID_APIONE_CODESIGNING         0x10000014 // 1.3.6.1.4.1.1449.9.4.1.20

//   id-aa-timeStampToken OBJECT IDENTIFIER ::= { iso(1) member-body(2)
//   us(840) rsadsi(113549) pkcs(1) pkcs-9(9) smime(16) aa(2) 14 }
// Binary DER prefix is \x06\x0b\x2a\x86\x48\x86\xf7\x0d\x01\x09\x10\x02
#define ASNOID_ID_AA                0x11000000 // 1.2.840.113549.1.9.16.2
#define ASNOID_ID_AA_TIMESTAMPTOKEN 0x1100000E // 1.2.840.113549.1.9.16.2.14 Message Digest

//  id-ct-TSTInfo  OBJECT IDENTIFIER ::= { iso(1) member-body(2)
//  us(840) rsadsi(113549) pkcs(1) pkcs-9(9) smime(16) ct(1) 4}
// Binary DER prefix is \x06\x0b\x2a\x86\x48\x86\xf7\x0d\x01\x09\x10\x01
#define ASNOID_ID_CT                0x12000000 // 1.2.840.113549.1.9.16.1
#define ASNOID_ID_CT_TSTINFO        0x12000004 // 1.2.840.113549.1.9.16.1.4 TimeStampToken Info

// 1.2.840.113549.1.7 This is PKCS7 space. Part of PKCS, which is part
// of RSADSI, which is part of the US which is an ISO member body...
// Binary DER prefix is \x06\x09\x2a\x86\x48\x86\xf7\x0d\x01\x07
#define ASNOID_PKCS7                        0x13000000 // 1.2.840.113549.1.7   The PKCS7 subspace of ASNOIDS
#define ASNOID_PKCS7_DATA                   0x13000001 // 1.2.840.113549.1.7.1 Data Content Type
#define ASNOID_PKCS7_SIGNEDDATA             0x13000002 // 1.2.840.113549.1.7.2 Signed-data Content Type
#define ASNOID_PKCS7_ENVELOPEDDATA          0x13000003 // 1.2.840.113549.1.7.3 Enveloped-data Content Type
#define ASNOID_PKCS7_SIGNEDANDENVELOPEDDATA 0x13000004 // 1.2.840.113549.1.7.4 Signed and Enveloped-data Content Type
#define ASNOID_PKCS7_DIGESTEDDATA           0x13000005 // 1.2.840.113549.1.7.5 Digested-data Content Type
#define ASNOID_PKCS7_ENCRYPTEDDATA          0x13000006 // 1.2.840.113549.1.7.6 Encrypted-data Content Type

// These are not actually ASNOIDs because they're not integers representing OIDS, but
// they're used and defined in the same context. These are the bit position or named
// bit values for KeyUse.
#define ASNOID_KEYUSE_DIGSIG       0x0000001
#define ASNOID_KEYUSE_NONREP       0x0000002
#define ASNOID_KEYUSE_KEYENC       0x0000004
#define ASNOID_KEYUSE_DATAENC      0x0000008
#define ASNOID_KEYUSE_KEYAGREE     0x0000010
#define ASNOID_KEYUSE_CERTSIGN     0x0000020
#define ASNOID_KEYUSE_CRLSIGN      0x0000040
#define ASNOID_KEYUSE_ENCONLY      0x0000080
#define ASNOID_KEYUSE_DECONLY      0x0000100

#define PKCS7_SHA256_HASH_LEN SHA256_HASH_LEN

#define ASN1_SEQUENCE_TYPE         (0x30)

typedef secx509_pubkey_type     pkcs7_pubkey_type;
typedef hash_alg_t          pkcs7_sig_algo_type;



uint32 secpkcs7_element_value_length
(
  const uint8 **element_ptr
);

uint32 secpkcs7_element_length
(
  const uint8 *element_ptr
);

uint32 secpkcs7_element_value
(
  const uint8 *element_ptr,
  const uint8 **blob_ptr
);

uint32 secpkcs7_map_oid
(
  const uint8   *oid_ptr,
  uint32        oid_len
);

secpkcs7_errno_enum_type secpkcs7_get_sig_and_hash_alg
(
uint32  sig_oid,
uint32  hash_oid,
uint32  *hash_alg_ptr,
uint32  *sig_alg_ptr
);

secpkcs7_errno_enum_type secpkcs7_pubkey_verify
(
pkcs7_pubkey_type         *pubkey,
uint8                     *sig_value_ptr, 
uint16                    sig_len,
uint8                     *hash_value,
uint16                    hash_len, 
pkcs7_sig_algo_type       sig_algo
);

secpkcs7_errno_enum_type secpkcs7_secx509_check_issued
(
  void* issuer,
  void* subject
);


#endif
