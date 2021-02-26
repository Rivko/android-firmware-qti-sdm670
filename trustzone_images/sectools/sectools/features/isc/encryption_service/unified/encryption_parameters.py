# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import struct

from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_misc import hexdump
from sectools.common.utils.c_misc import store_debug_data_to_file
from sectools.features.isc import advanced_defines
from sectools.features.isc.encryption_service.base.encryption_parameters import BaseEncryptionParameters
from sectools.features.isc.encryption_service.unified.encrypted_key_provider_mgr import \
    get_encrypted_key_provider
from sectools.features.isc.encryption_service.unified.encryption_params_info_hdr \
    import EncryptionParamsInfoHdr_1_0, EncryptionParamsInfoHdr_2_0
from sectools.features.isc.encryption_service.unified.encryption_params_section_body \
    import EncryptionParamsSectionBody_1_0_L2_1_0, EncryptionParamsSectionBody_1_0_L2_1_1, \
    EncryptionParamsSectionBody_2_0_L2_1_2
from sectools.features.isc.encryption_service.unified.encryption_params_section_hdr \
    import EncryptionParamsSectionHdr_1_0, EncryptionParamsSectionHdr_2_0
from sectools.features.isc.key_service_enquirer import KeyService, AsymmetricKeyService


class EncryptionParameters_1_0_L2_1_0(BaseEncryptionParameters):

    ENCRYPTION_PARAMS_HEADER_LEN_BYTES = 16
    ENCRYPTION_PARAMS_SECTION_HDR_LEN_BYTES = 16
    ENCRYPTION_PARAMS_SECTION_BDY_LEN_BYTES = 176
    SOFTWARE_TYPE_FLD_OFFSET = 10

    class InvalidKeyComboException(RuntimeError):
        pass

    MAGIC_NUM = 0x514D5349

    def __init__(self, encryption_params_blob=None, private_key=None, config=None, debug_dir=None, l3_key_blob=None,
                 validating=False, encrypted_segments_indices=None):
        self.image_id = int(config.signing_attributes.sw_id[EncryptionParameters_1_0_L2_1_0.SOFTWARE_TYPE_FLD_OFFSET:],16)
        if encryption_params_blob is None and private_key is None and l3_key_blob is None:
            self.key_service = KeyService(config)
            encryption_params_blob, private_key = self._generate_new_blob(config, debug_dir, encrypted_segments_indices)
            # Set the base params
            BaseEncryptionParameters.__init__(self, encryption_params_blob=encryption_params_blob, key=private_key, debug_dir=debug_dir, validating=validating)
        elif encryption_params_blob is not None and l3_key_blob is not None:
            self._set_l3_and_iv(encryption_params_blob, l3_key_blob)
            # Set the base params
            BaseEncryptionParameters.__init__(self, encryption_params_blob=encryption_params_blob, key=None, debug_dir=debug_dir, validating=validating)
        elif encryption_params_blob is not None and private_key is not None:
            # Set the base params
            BaseEncryptionParameters.__init__(self, encryption_params_blob=encryption_params_blob, key=private_key, debug_dir=debug_dir, validating=validating)
            # Get the image encryption key & iv
            self._decode_blob(encryption_params_blob, private_key, debug_dir)
        else:
            raise RuntimeError("ERROR: Invalid configuration of params used to create encryption params.")

        store_debug_data_to_file(advanced_defines.DEST_DEBUG_FILE_ENCRYPTION_PARAMETERS, hexdump(self.encryption_params_blob), debug_dir)

    def get_image_encryption_key(self):
        return self.l3_key

    def get_image_encryption_iv(self):
        return self.l3_image_iv

    def get_l1_key(self):
        return self.key_service.get_l1_key()

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
        string_end = EncryptionParameters_1_0_L2_1_0.ENCRYPTION_PARAMS_HEADER_LEN_BYTES + EncryptionParameters_1_0_L2_1_0.ENCRYPTION_PARAMS_SECTION_HDR_LEN_BYTES + EncryptionParameters_1_0_L2_1_0.ENCRYPTION_PARAMS_SECTION_BDY_LEN_BYTES
        string_offset = string_end - EncryptionParamsSectionBody_1_0_L2_1_0.BASE_IV_LEN_BYTES
        self.l3_image_iv = encryption_params_blob[string_offset:string_end]

    def _generate_new_blob(self, config, debug_dir, encrypted_segments_indices=None):
        is_enc_key_provider = False
        if self.key_service.using_encrypted_key_provider():
            # UIE_key value was set to an encrypted key provider value. ex: "encrypted_qti" or "encrypted_oem"
            logger.info("Attempting to retrieve encrypted L2, encrypted L3, and clear L3 keys from encrypted key provider...")
            self.l1_key = None
            self.l2_key = None
            # get the encrypted key provider
            encrypted_key_provider_class = get_encrypted_key_provider(
                self.key_service.get_encrypted_key_provider_id())
            is_enc_key_provider = True
            if encrypted_key_provider_class.is_qti():
                sa = config.signing_attributes
                encrypted_key_provider = encrypted_key_provider_class(sa.UIE_capability,
                                                                      sa.UIE_server_url,
                                                                      sa.UIE_server_cert_path)
            else:
                encrypted_key_provider = encrypted_key_provider_class()

            # get the encrypted keys and clear l3 key from the encrypted key provider
            self.encrypted_l2 = encrypted_key_provider.get_encrypted_l2_key()
            self.encrypted_l3 = encrypted_key_provider.get_encrypted_l3_key()
            self.l3_key = encrypted_key_provider.get_clear_l3_key()
            logger.info("Encrypted L2, encrypted L3, and clear L3 keys were successfully retrieved from encrypted key provider.")
            logger.debug("Encrypted L2 Key: \n" + hexdump(self.encrypted_l2))
            logger.debug("Encrypted L3 Key: \n" + hexdump(self.encrypted_l3))
        elif self.key_service.get_l1_key() is not None and self.key_service.get_l2_key() is None and self.key_service.get_l3_key() is None and self.key_service.no_enc_keys_provided():
            # only l1 key is provided locally. l2 and l3 keys will be set to randomly generated values
            logger.debug("Clear L1 key was provided locally but clear L2 and clear L3 keys were not. Generating random values for clear L2 and clear L3 keys...")
            self.l1_key = self.key_service.get_l1_key()
            self.l2_key = KeyService.get_new_random_clear_key()
            self.l3_key = KeyService.get_new_random_clear_key()
            logger.debug("L1 Key: \n" + hexdump(self.l1_key))
            logger.debug("L2 Key: \n" + hexdump(self.l2_key))
            logger.debug("L3 Key: \n" + hexdump(self.l3_key))
            self.encrypted_l2 = None
            self.encrypted_l3 = None
        elif self.key_service.all_clear_keys_provided() and self.key_service.no_enc_keys_provided():
            # all 3 clear keys are provided locally
            logger.debug("Clear L1 key, clear L2 key, and clear L3 keys were provided locally.")
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
            logger.debug("Encrypted L2 key, encrypted L3 key, and clear L3 keys were provided locally.")
            self.l1_key = None
            self.l2_key = None
            self.l3_key = self.key_service.get_l3_key()
            self.encrypted_l2 = self.key_service.get_encrypted_l2_key()
            self.encrypted_l3 = self.key_service.get_encrypted_l3_key()
            logger.debug("Encrypted L2 Key: \n" + hexdump(self.encrypted_l2))
            logger.debug("Encrypted L3 Key: \n" + hexdump(self.encrypted_l3))
            logger.debug("L3 Key: \n" + hexdump(self.l3_key))
        else:
            raise EncryptionParameters_1_0_L2_1_0.InvalidKeyComboException("The found key configuration is not allowed.\n \
                        Supported configurations: \n \
                        #1 clear L1 key \n \
                        #2 clear L1, L2, and L3 key \n \
                        #3 encrypted L2, encrypted L3, and clear L3 key \n \
                        #4 specify an encrypted key provider.\n \
                        If required files are provided, ensure that the corresponding config file \n \
                        contains the correct METACONFIG tags and values.")

        encryption_params_hdr = EncryptionParamsInfoHdr_1_0()
        encryption_params_hdr.add_encryption_param_section()
        eps_hdr = EncryptionParamsSectionHdr_1_0(root_key_type=config.signing_attributes.UIE_root_key_type)
        eps_body = self._get_encryption_params_section_body_class()(self.image_id,
                                                                    l1_key=self.l1_key,
                                                                    l2_key=self.l2_key,
                                                                    l3_key=self.l3_key,
                                                                    enc_l2_key=self.encrypted_l2,
                                                                    enc_l3_key=self.encrypted_l3,
                                                                    is_enc_key_provider=is_enc_key_provider,
                                                                    debug_dir=debug_dir,
                                                                    encrypted_segments_indices=encrypted_segments_indices)

        self.l3_image_iv = eps_body.get_image_iv()

        self.encryption_params_blob = encryption_params_hdr.get_header_blob() + \
                                      eps_hdr.get_header_blob() + \
                                      eps_body.get_binary_blob()

        logger.debug("Encryption Params: \n" + hexdump(self.encryption_params_blob))
        return self.encryption_params_blob, self.l1_key

    def _decode_blob(self, encryption_params_blob, l1_key_blob, debug_dir):
        # Decode Encryption Parameter Info Header
        string_offset = 0
        string_end = EncryptionParameters_1_0_L2_1_0.ENCRYPTION_PARAMS_HEADER_LEN_BYTES
        EncryptionParamsInfoHdr_1_0(encryption_params_blob[string_offset:string_end])

        # Decode Encryption Parameter Section skipping Encryption Parameters Section Header
        string_offset = string_end + EncryptionParameters_1_0_L2_1_0.ENCRYPTION_PARAMS_SECTION_HDR_LEN_BYTES
        string_end = string_offset + EncryptionParameters_1_0_L2_1_0.ENCRYPTION_PARAMS_SECTION_BDY_LEN_BYTES
        eps = self._get_encryption_params_section_body_class()(self.image_id,
                                                               l1_key=l1_key_blob,
                                                               enc_param_section_body_blob=encryption_params_blob[string_offset:string_end],
                                                               validating=self.validating,
                                                               debug_dir=debug_dir)

        self.l3_image_iv = eps.get_image_iv()
        self.l3_key = eps.get_l3_key()

    @staticmethod
    def _get_encryption_params_section_body_class():
        return EncryptionParamsSectionBody_1_0_L2_1_0


class EncryptionParameters_1_0_L2_1_1(EncryptionParameters_1_0_L2_1_0):

    @staticmethod
    def _get_encryption_params_section_body_class():
        return EncryptionParamsSectionBody_1_0_L2_1_1


class EncryptionParameters_2_0_L2_1_2(EncryptionParameters_1_0_L2_1_1):

    # UIE 2.0 Encryption Parameters class only supports encryption (generating encryption params).  It does not
    # support parsing/decrypting existing encryption params because there is no off-target use-case for decryption
    # using asymmetric L1 keys.

    def __init__(self, encryption_params_blob=None, private_key=None, config=None, debug_dir=None, l3_key_blob=None,
                 validating=False, encrypted_segments_indices=None):
        self.image_id = int(config.signing_attributes.sw_id[EncryptionParameters_1_0_L2_1_0.SOFTWARE_TYPE_FLD_OFFSET:], 16)
        self.feature_id = int(config.signing_attributes.UIE_feature_id, 16)
        if encryption_params_blob is None:
            self.key_service = AsymmetricKeyService(config)
            encryption_params_blob, l1_public_key = self._generate_new_blob(config, debug_dir, encrypted_segments_indices)
            BaseEncryptionParameters.__init__(self, encryption_params_blob=encryption_params_blob, key=l1_public_key, debug_dir=debug_dir)
        else:
            if validating:
                raise RuntimeError("Validation of UIE 2.0 encrypted image in not supported.\n"
                                   "       Try again with validation disabled.")
            raise RuntimeError("Cannot operate on UIE 2.0 encrypted image "
                               "because decryption is only supported on device.")

    @staticmethod
    def _get_encryption_params_section_body_class():
        return EncryptionParamsSectionBody_2_0_L2_1_2

    def _generate_new_blob(self, config, debug_dir, encrypted_segments_indices=None):
        if self.key_service.get_l1_public_key() is not None:
            logger.debug("L1 public key was provided locally")
            self.l1_key = self.key_service.get_l1_public_key()
            logger.debug("Generating random values for L2 and L3 keys...")
            self.l2_key = KeyService.get_new_random_clear_key()
            self.l3_key = KeyService.get_new_random_clear_key()
            logger.debug("L1 Public Key: \n" + hexdump(self.l1_key))
            logger.debug("L2 Key: \n" + hexdump(self.l2_key))
            logger.debug("L3 Key: \n" + hexdump(self.l3_key))
        else:
            raise EncryptionParameters_1_0_L2_1_0.InvalidKeyComboException("\nMissing L1 public key.\n"
                "L1 public key must be provided when encrypting an image using UIE 2.0.\n"
                "If L1 public key is provided, ensure that the corresponding config file \n"
                "contains the correct l1_public_file_name METACONFIG tag and value.")

        encryption_params_hdr = EncryptionParamsInfoHdr_2_0()
        encryption_params_hdr.add_encryption_param_section()
        eps_hdr = EncryptionParamsSectionHdr_2_0(root_key_type=config.signing_attributes.UIE_root_key_type)
        eps_body = self._get_encryption_params_section_body_class()(self.image_id,
                                                                    l1_key=self.l1_key,
                                                                    l2_key=self.l2_key,
                                                                    l3_key=self.l3_key,
                                                                    debug_dir=debug_dir,
                                                                    encrypted_segments_indices=encrypted_segments_indices,
                                                                    feature_id=self.feature_id)

        self.l3_image_iv = eps_body.get_image_iv()

        self.encryption_params_blob = encryption_params_hdr.get_header_blob() + \
                                      eps_hdr.get_header_blob() + \
                                      eps_body.get_binary_blob()

        logger.debug("Encryption Params: \n" + hexdump(self.encryption_params_blob))
        return self.encryption_params_blob, self.l1_key
