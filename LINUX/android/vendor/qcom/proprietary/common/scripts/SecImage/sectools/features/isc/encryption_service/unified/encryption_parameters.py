# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import struct

from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_misc import hexdump
from sectools.common.utils.c_misc import store_debug_data_to_file
from sectools.features.isc import data_provision_enquirer as key_service_module
from sectools.features.isc import defines
from sectools.features.isc.encryption_service.base.encryption_parameters import BaseEncryptionParameters
from sectools.features.isc.encryption_service.unified.encryption_params_header import EncryptionParamsHeader
from sectools.features.isc.encryption_service.unified.encryption_params_section_body import EncryptionParamsSectionBody
from sectools.features.isc.encryption_service.unified.encryption_params_section_hdr import EncryptionParamsSectionHdr


class EncryptionParameters(BaseEncryptionParameters):
    '''
    Implements the encryption parameters as defined in unified image encryption docs
    '''

    ENCRYPTION_PARAMS_HEADER_LEN_BYTES = 16
    ENCRYPTION_PARAMS_SECTION_HDR_LEN_BYTES = 16
    ENCRYPTION_PARAMS_SECTION_BDY_LEN_BYTES = 176
    SOFTWARE_TYPE_FLD_OFFSET = 10

    class ALGORITHM_TYPE:
        CEML_CIPHER_ALG_AES128 = 0x0
        CEML_CIPHER_ALG_AES256 = 0x1
        CEML_CIPHER_ALG_TRIPLE_DES = 0x2

    class ALGORITHM_MODE_TYPE:
        CEML_CIPHER_MODE_ECB = 0x0
        CEML_CIPHER_MODE_CBC = 0x1
        CEML_CIPHER_MODE_CTR = 0x2
        CEML_CIPHER_MODE_XTS = 0x3
        CEML_CIPHER_MODE_CCM = 0x4
        CEML_CIPHER_MODE_CMAC = 0x5
        CEML_CIPHER_MODE_CTS = 0x6

    class SECURITY_LEVEL:
        INVALID, EL_3, EL_2 = range(3)

    class FIELD_LEN_BYTES:
        MAGIC_FLD = 4
        VERSION_FLD = 4
        SIZE_FLD = 4
        KEY_SOURCE_ID_FLD = 4
        KEY_LADDER_LEN_FLD = 4
        KEY_LADDER_ALG_FLD = 4
        KEY_LADDER_MODE_FLD = 4
        L1_B0_FLG_FLD = 1
        L1_NONCE_FLD = 13
        L1_B0_Q_FLD = 2
        L1_B1_FLD = 16
        L2_ENC_KEY_FLD = 32
        L2_B0_FLG_FLD = 1
        L2_NONCE_FLD = 13
        L2_B0_Q_FLD = 2
        L2_B1_FLD = 16
        L3_ENC_KEY_FLD = 32
        L3_IMAGE_IV_FLD = 16

    class InvalidKeyComboException(RuntimeError):
        pass

    MAGIC_NUM = 0x514D5349
    VERSION = 1

    def __init__(self, encryption_params_blob=None, private_key=None, config=None, debug_dir=None, l3_key_blob=None):
        # Ensure that the SW_ID is in the expected form
        conformal_sw_id = "0x%0.16X" % int(config.signing_attributes.sw_id, 16)
        self.image_id = int(conformal_sw_id[EncryptionParameters.SOFTWARE_TYPE_FLD_OFFSET:], 16)

        if encryption_params_blob is None and private_key is None and l3_key_blob is None:
            self.key_service = key_service_module.KeyService(config)
            encryption_params_blob, private_key = self._generate_new_blob(config, debug_dir)
            # Set the base params
            BaseEncryptionParameters.__init__(self, encryption_params_blob=encryption_params_blob, key=private_key, debug_dir=debug_dir)
        elif encryption_params_blob is not None and l3_key_blob is not None:
            self._set_l3_and_iv(encryption_params_blob, l3_key_blob)
            # Set the base params
            BaseEncryptionParameters.__init__(self, encryption_params_blob=encryption_params_blob, key=None, debug_dir=debug_dir)
        elif encryption_params_blob is not None and private_key is not None:
            # Set the base params
            BaseEncryptionParameters.__init__(self, encryption_params_blob=encryption_params_blob, key=private_key, debug_dir=debug_dir)
            # Get the image encryption key & iv
            self._decode_blob(encryption_params_blob, private_key, debug_dir)
        else:
            raise RuntimeError("ERROR: Invalid configuration of params used to create encryption params.")

        store_debug_data_to_file(defines.DEST_DEBUG_FILE_ENCRYPTION_PARAMETERS, hexdump(self.encryption_params_blob), debug_dir)

    def get_image_encryption_key(self):
        return self.l3_key

    def get_image_encryption_iv(self):
        return self.l3_image_iv

    def get_l2_key(self):
        return self.l2_key

    def extract_encryption_parameters(self, data):
        encryption_parameters = ''
        length = len(self.encryption_params_blob)
        if len(data) > length and struct.unpack_from('I', data)[0] == self.MAGIC_NUM:
            encryption_parameters = data[:length]
            data = data[length:]
        return data, encryption_parameters

    def _set_l3_and_iv(self, encryption_params_blob, l3):
        self.l3_key = l3
        string_end = EncryptionParameters.ENCRYPTION_PARAMS_HEADER_LEN_BYTES + EncryptionParameters.ENCRYPTION_PARAMS_SECTION_HDR_LEN_BYTES + EncryptionParameters.ENCRYPTION_PARAMS_SECTION_BDY_LEN_BYTES
        string_offset = string_end - EncryptionParamsSectionBody.BASE_IV_LEN_BYTES
        self.l3_image_iv = encryption_params_blob[string_offset:string_end]

    def _generate_new_blob(self, config, debug_dir):
        self.enc_key_prov_is_qc = False
        if self.key_service.using_encrypted_key_provider():
            # UIE_key value was set to an encrypted key provider value. ex: "encrypted_qc" or "encrypted_oem"
            logger.info("Attempting to retrieve encrypted L2, encrypted L3, and clear L3 keys from encrypted key provider...")
            self.l1_key = None
            self.l2_key = None
            # get the encrypted key provider
            from sectools.features.isc.encryption_service.unified import get_encrypted_key_provider
            encrypted_key_provider_class = get_encrypted_key_provider(self.key_service.get_encrypted_key_provider_id())
            self.enc_key_prov_is_qc = encrypted_key_provider_class.is_qc()
            if self.enc_key_prov_is_qc:
                encrypted_key_provider = encrypted_key_provider_class(config.signing_attributes.UIE_capability)
            else:
                encrypted_key_provider = encrypted_key_provider_class()

            # get the encrypted keys and clear l3 key from the encrypted key provider
            self.encrypted_l2 = encrypted_key_provider.get_encrypted_l2_key()
            self.encrypted_l3 = encrypted_key_provider.get_encrypted_l3_key()
            self.l3_key = encrypted_key_provider.get_clear_l3_key()
            logger.info("Encrypted L2, encrypted L3, and clear L3 keys were successfully retrieved from encrypted key provider.")
            logger.debug("Encrypted L2 Key: \n" + hexdump(self.encrypted_l2))
            logger.debug("Encrypted L3 Key: \n" + hexdump(self.encrypted_l3))
            logger.debug("L3 Key: \n" + hexdump(self.l3_key))
        elif self.key_service.get_l1_key() is not None and self.key_service.get_l2_key() is None and self.key_service.get_l3_key() is None and self.key_service.no_enc_keys_provided():
            # only l1 key is provided locally. l2 and l3 keys will be set to randomly generated values
            logger.info("Clear L1 key was provided locally but clear L2 and clear L3 keys were not. Generating random values for clear L2 and clear L3 keys...")
            self.l1_key = self.key_service.get_l1_key()
            self.l2_key = key_service_module.KeyService.get_new_random_clear_key()
            self.l3_key = key_service_module.KeyService.get_new_random_clear_key()
            logger.debug("L1 Key: \n" + hexdump(self.l1_key))
            logger.debug("L2 Key: \n" + hexdump(self.l2_key))
            logger.debug("L3 Key: \n" + hexdump(self.l3_key))
            self.encrypted_l2 = None
            self.encrypted_l3 = None
        elif self.key_service.all_clear_keys_provided() and self.key_service.no_enc_keys_provided():
            # all 3 clear keys are provided locally
            logger.info("Clear L1 key, clear L2 key, and clear L3 keys were provided locally.")
            self.l1_key = self.key_service.get_l1_key()
            self.l2_key = self.key_service.get_l2_key()
            self.l3_key = self.key_service.get_l3_key()
            logger.debug("L1 Key: \n" + hexdump(self.l1_key))
            logger.debug("L2 Key: \n" + hexdump(self.l2_key))
            logger.debug("L3 Key: \n" + hexdump(self.l3_key))
            self.encrypted_l2 = None
            self.encrypted_l3 = None
        elif self.key_service.all_enc_keys_provided() and self.key_service.get_l3_key() is not None and self.key_service.get_l1_key() is None and self.key_service.get_l2_key() is None:
            # encrypted l2, encrypted l3, and clear l3 are provided locally
            logger.info("Encrypted L2 key, encrypted L3 key, and clear L3 keys were provided locally.")
            self.l1_key = None
            self.l2_key = None
            self.l3_key = self.key_service.get_l3_key()
            self.encrypted_l2 = self.key_service.get_encrypted_l2_key()
            self.encrypted_l3 = self.key_service.get_encrypted_l3_key()
            logger.debug("Encrypted L2 Key: \n" + hexdump(self.encrypted_l2))
            logger.debug("Encrypted L3 Key: \n" + hexdump(self.encrypted_l3))
            logger.debug("L3 Key: \n" + hexdump(self.l3_key))
        else:
            raise EncryptionParameters.InvalidKeyComboException("The found key configuration is not allowed.\n \
                        Supported configurations: \n \
                        #1 clear L1 key \n \
                        #2 clear L1, L2, and L3 key \n \
                        #3 encrypted L2, encrypted L3, and clear L3 key \n \
                        #4 specify an encrypted key provider.\n \
                        If required files are provided, ensure that the corresponding config file \n \
                        contains the correct METACONFIG tags and values.")

        encryption_params_hdr = EncryptionParamsHeader()
        encryption_params_hdr.add_encryption_param_section()
        eps_hdr1 = EncryptionParamsSectionHdr(None, None, 1 if config.signing_attributes.UIE_root_key_type is None else config.signing_attributes.UIE_root_key_type, 0)
        eps_body1 = EncryptionParamsSectionBody(l1_key=self.l1_key, l2_key=self.l2_key, l3_key=self.l3_key, enc_l2_key=self.encrypted_l2, enc_l3_key=self.encrypted_l3, enc_key_prov_is_qc=self.enc_key_prov_is_qc, image_id_bitmap=self.image_id, debug_dir=debug_dir)
        self.l3_image_iv = eps_body1.get_image_iv()

        self.encryption_params_blob = encryption_params_hdr.get_header_blob() + \
            eps_hdr1.get_header_blob() + \
            eps_body1.get_binary_blob()

        logger.debug("Encryption Params: \n" + hexdump(self.encryption_params_blob))
        return self.encryption_params_blob, self.l1_key

    def _decode_blob(self, encryption_params_blob, l1_key_blob, debug_dir):
        # validate magic number
        string_offset = 0
        string_end = EncryptionParameters.FIELD_LEN_BYTES.MAGIC_FLD
        found_magic_num = struct.unpack_from('I', encryption_params_blob[string_offset: string_end])[0]
        if found_magic_num != self.MAGIC_NUM:
            raise RuntimeError("Image's encryption parameters has invalid magic number: \"{0}\".".format(hex(found_magic_num)))

        string_offset = EncryptionParameters.ENCRYPTION_PARAMS_HEADER_LEN_BYTES + EncryptionParameters.ENCRYPTION_PARAMS_SECTION_HDR_LEN_BYTES
        string_end = string_offset + EncryptionParameters.ENCRYPTION_PARAMS_SECTION_BDY_LEN_BYTES
        eps_body1 = EncryptionParamsSectionBody(enc_param_section_body_blob=encryption_params_blob[string_offset:string_end], image_id_bitmap=None, l1_key=l1_key_blob, debug_dir=debug_dir)
        self.l3_image_iv = eps_body1.get_image_iv()
        self.l3_key = eps_body1.get_l3_key()

    def get_l1_key(self):
        return self.key_service.get_l1_key()


