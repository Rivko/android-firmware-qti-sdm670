# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

SECBOOT_VERSION_2_0 = '2.0'
SECBOOT_VERSION_3_0 = '3.0'

DEST_DEBUG_DIR_ENCDEC = 'encdec'

DEST_DEBUG_FILE_PARSEGEN_ENCRYPTED = '_encrypted'
DEST_DEBUG_FILE_ENCRYPTION_PARAMETERS = "enc_param.debug"
DEST_DEBUG_FILE_ENCRYPTED_L2_KEY = "l2_enc.bin"
DEST_DEBUG_FILE_ENCRYPTED_L3_KEY = "l3_enc.bin"
DEST_DEBUG_FILE_L2_IMAGE_IV = "l2_img_iv.bin"
DEST_DEBUG_FILE_L3_IMAGE_IV = "l3_img_iv.bin"
DEST_DEBUG_FILE_L2_ADD = "l2_aad.bin"
DEST_DEBUG_FILE_L3_ADD = "l3_aad.bin"

DEST_DEBUG_FILE_SIGNER_QTI_SIGNATURE = 'signature_qti.dat'
DEST_DEBUG_FILE_SIGNER_QTI_CERT_CHAIN = 'cert_chain_qti.cer'

DEST_FILE_DECRYPTED = '_decrypted'

AUTHORITY_QTI = 'QTI'
HW_ID_TAG = "hw_id"
SOC_VERS_TAG = "soc_vers"
SERIAL_NUMBERS = "serial_numbers"
MULTI_SERIAL_NUMBERS_TAG = "multi_serial_numbers"
SERIAL_BOUND_TAG = "serial_bound"
SECBOOT_VERSION_TAG = "secboot_version"

PLATFORM_BINDING_JTAG_ID = "JTAG_ID"
PLATFORM_BINDING_SOC_VERS = "SOC_VERS"
PLATFORM_BINDING_INDEPENDENT = "INDEPENDENT"
PLATFORM_BINDINGS = [PLATFORM_BINDING_JTAG_ID, PLATFORM_BINDING_SOC_VERS, PLATFORM_BINDING_INDEPENDENT]

SP_MCP_SW_ID = 0x6
SP_APPS_SW_ID = 0x8

TEST_SIGNED = "TEST"
PROD_SIGNED = "PROD"
LOCAL_SIGNED = "LOCAL"
TEST_SIGNED_ROOTS = "TEST_ROOTS"
PROD_SIGNED_ROOTS = "PROD_ROOTS"

rch_string = [TEST_SIGNED, PROD_SIGNED]

roots_value =   \
    {
    TEST_SIGNED_ROOTS :
        ('931895bb3e58ad2c0363fd79fe6ed4f198c5db3ef1b3f82221d85430f27137eb',
         'a38eb19e30a4e073b4a593605517b45c6139f5613355b91c96383e13058c74766acba51ad7fda118abef3d439af41c4f', #QSEE Attestation Test Root CA 2 RSA
         'ed51423a8ab2dbb54f8b8d8ae051e666148b0602a9e5c74c76ce1b6101fbe399f44adf09f30416f1aee4d160dc227995', #QSEE Attestation Test Root CA 4 ECDSA
         'af658dfdaef4aa92cde2eccdff42418247b135a92caaa92dd2caf39258ee6309',
         '46c34572f4aae3704c8b7ecbcae8ed83643ab26ef669701a14167b3f09ff677847fc05e8f2f1b2cc801aae14b03971d3',
         'a0307f7078580f47dd79b0212f0de6beb273184e9c5a10c134587a8db44572d18542c02a11b52ccc5b69ac0ed36f7a9a', #QSEE Application Attestation Test Root
         '067b9e4a85bbab8ee5b93fea3cf58c57fc8ce799588c757855b74f2bd7538bc7020f8ed28b77115e7aa673a4b47779df', #QSEE Attestation Test Root CA
         '5231653ec43606610155171f1f6a85ec58458231549a7fb22adbb711cb4ea0db38c937e5b8a69df4a37a13544084cd05', #QSEE Communication Test Root CA
         '0db75e87b8697b14512dda7ff91277344e0ba137ba01766a900ecc5e30c56073efb592dc5a971de307acbd94b0ae319b', #SPSS Attestation Test Root CA
         '3afa5c8f2f47ca8afeb785eec1df6cdcc06a4f00a7e118f465fa64ff177717ae804ef27fef60a67fc2bd30a75f766047', #SPSS Attestation Test Root CA 4
         ),

    PROD_SIGNED_ROOTS :
         ('1f0e88690022bc4ff9a144d7ccb4a603f031b4d8545af488dbbc0eaf3d72f42c',
          'a641fd4536c59094527b71ab7de1f12b502f26aece466544cc7960b93c3cdb3c53c496ad83707341987278271fea7756', #QSEE Attestation Root CA 2
          'd09c616912f75727393196b43d3e624a74aecd7058c7ce4fbaf51849fbd06da5',
          '09f2df7160d4a7c36fbfd1a25489acbb59d04e4625a5284798427368365d572a2570052ed5a03cd530cf26d3e3950ef3',
          'd40eee56f3194665574109a39267724ae7944134cd53cb767e293d3c40497955bc8a4519ff992b031fadc6355015ac87', #QMC Attestation Root CA 3
          'a2514bb85111bc6625a189448b9d460a64d33e1581bad7f348d20b5ca597f4e780668706c2e793703f2a3c0a5e52a497', #XBL Sec Attestation Root CA 2
          'e247429cfe5df29c77b681ee48aaf7c68cb30eeaa1feca7d6a11dd1d2982f1e1ed85a84d4548c54a167853ef266657a9', #XBL Sec Attestation Root CA 4
          '69acb56b82894b9a1eb345ae4c1525120a6896dad52727e8c6a0fca9aa0555378f390462b420761e73cd7c076570d9bf', #Feature License Attestation Root CA
          '03ca192014aecff1999ded569001ca4a3d4fe56c04b5b462521129d2784c1c28f5f377955e2660d764826a1fe95464d4', #Feature License Communication Root CA
          '72afad7ba5ac6ce9c86e74fb4f53528eb922a8a2d01e0f4fe0ebca2a09a42aea89430fe62b831ab486e9c5bb1a8a7c80', #QMC Attestation Root CA 2
          '0e6b4f06e0fd844ee03af5c543d27042e7757e637796d6a3c85986f5b98d4562d46297804931dc870d8835d23a806db7', #QSEE Application Attestation Root
          'be1844ccd978d6b1e8e020be98237bc91f944337f3c7aff0c2304276bab835c755eb7f54fa054c507dedea3b4b2406eb', #QSEE Attestation Root CA
          'a4cd9bb6f5d657f847ec32364d3dfcce0e9c3498677fe67d32868018f57632d5759fd050e58f7540e6a20d5ffefb5da0', #QSEE Communication Root CA
          'a97038aa9db218d8dd38d39fa6468cde1bbe9a1a45650177255978e3aab509fce2381910e392cb593dc5ab103bbd7584', #SPSS Attestation Root CA
          '88b0124e060e9815069f7ee4e8990b2a864adcdfcaa4fbe55f8de48f998eca98a0793351766174339718efe3d180acd1', #XBL Sec Attestation Root CA
          '98c3d8118da73ac9f1768810786f7420978fde6573fba0bd848a675d1e7f453a50bf49a32ad9e5f056227134af6e74da', #QMC Attestation Root CA 4
          '7f72806b6b787536bdf56b251f9ffdd481886adca5e4f73eb919623d82a3d7f14324de12592f6e85a7e64b3520deed3f', #QSEE Attestation Root CA 4
          'f5fc055b82b586c4b1eec94b00ddabe5fb3c3da148866cb95b9d40144871d9addb8194198f1324dce03b89ea07013337', #SPSS Attestation Root CA 4
          '0099c3458b25dc543b62db851fc5bedb41248264bd9238420bab464269db8cd73a5de82179da617de5f7ecff9f20a028'  #QDSR Root CA
          )
    }

def multi_image_string():
    return "Multi-Image Sign & Integrity"
