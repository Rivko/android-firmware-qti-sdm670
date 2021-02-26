#===============================================================================
#
# Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
#===============================================================================

import os
import struct
import binascii
from sectools.common.crypto import crypto_functions
from sectools.common.utils.c_misc import hexdump
from sectools.features.isc.encryption_service.base.encryption_parameters import BaseEncryptionParameters
from sectools.features.isc.encryption_service.unified.encryption_params_section_hdr import EncryptionParamsSectionHdr
from sectools.features.isc.encryption_service.unified.encryption_params_header import EncryptionParamsHeader
from sectools.features.isc.encryption_service.unified.encryption_params_section_body import EncryptionParamsSectionBody
from sectools.common.utils.c_logging import logger
from sectools.features.isc.encryption_service import key_service as key_service_module




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

    MAGIC_NUM = 0x514D5349
    VERSION = 1
    def __init__(self, encryption_params_blob=None, l1_key_blob=None, config=None, debug_dir=None):
        self.key_service = key_service_module.KeyService(config)
        # Settle on params

        # Ensure that the SW_ID is in the expected form
        conformal_sw_id = "0x%0.16X" % int(config.signing_attributes.sw_id, 16)
        self.image_id = int(conformal_sw_id[EncryptionParameters.SOFTWARE_TYPE_FLD_OFFSET:], 16)
        if encryption_params_blob == None and l1_key_blob == None:
            encryption_params_blob, l1_key_blob = self._generate_new_encryption_params_blob()
        elif encryption_params_blob == None or l1_key_blob == None:
            raise RuntimeError("ERROR: Both encryption params and private key need to be provided")

        # Set the base params
        BaseEncryptionParameters.__init__(self, encryption_params_blob=encryption_params_blob, key=l1_key_blob, debug_dir=debug_dir)

        # Get the image encryption key & iv
        self._decode_encryption_parameters_blob(encryption_params_blob, l1_key_blob)

        if debug_dir:
            encryption_params_debug_filename = "enc_param.debug"
            with open(os.path.join(debug_dir, encryption_params_debug_filename), 'wb') as debug_file:
                debug_file.write(self.__repr__())

    def get_image_encryption_key(self):
        return self._get_l3_key()

    def get_image_encryption_iv(self):
        return self.l3_image_iv

    def extract_encryption_parameters(self, data):
        encryption_parameters = ''
        length = len(self.encryption_params_blob)
        if len(data) > length and struct.unpack_from('I', data)[0] == self.MAGIC_NUM:
            encryption_parameters = data[:length]
            data = data[length:]
        return data, encryption_parameters

    def _generate_new_encryption_params_blob(self):
        self.l1_key = self._get_l1_key()
        self.l2_key = self._get_l2_key()
        self.l3_key = self._get_l3_key()



        encryption_params_hdr = EncryptionParamsHeader()
        encryption_params_hdr.add_encryption_param_section()
        eps_hdr1 = EncryptionParamsSectionHdr(None, None, 1, 0)
        eps_body1 = EncryptionParamsSectionBody(l1_key_blob=self.l1_key, image_id_bitmap=self.image_id)

        self.encryption_params_blob = encryption_params_hdr.get_header_blob() + \
                                        eps_hdr1.get_header_blob() + \
                                        eps_body1.get_binary_blob()

        self.l3_image_iv = eps_body1.get_image_iv()
        self.l3_key = eps_body1.get_l3_key()

        logger.debug("Encryption Params: \n" + hexdump(self.encryption_params_blob))
        logger.debug("L1 Key: \n" + hexdump(self.l1_key))
        return self.encryption_params_blob, self.l1_key



    def _decode_encryption_parameters_blob(self, encryption_params_blob, l1_key_blob):



        string_offset = 0
        string_end = EncryptionParameters.ENCRYPTION_PARAMS_HEADER_LEN_BYTES

        encryption_params_hdr = EncryptionParamsHeader(encryption_params_blob[string_offset:string_end])

        string_offset = string_end
        string_end += EncryptionParameters.ENCRYPTION_PARAMS_SECTION_HDR_LEN_BYTES
        eps_hdr1 = EncryptionParamsSectionHdr(encryption_params_blob[string_offset:string_end], None, 1, 1)

        string_offset = string_end
        string_end += EncryptionParameters.ENCRYPTION_PARAMS_SECTION_BDY_LEN_BYTES
        eps_body1 = EncryptionParamsSectionBody(enc_param_section_body_blob=encryption_params_blob[string_offset:string_end], image_id_bitmap=None, l1_key_blob=l1_key_blob)

        self.l3_image_iv = eps_body1.get_image_iv()
        self.l3_key = eps_body1.get_l3_key()


    def _get_l1_key(self):
        '''
        Returns L1 key
        '''

        return self.key_service.get_l1_key()

    def _get_l2_key(self):
        '''
        Returns L2 key
        '''

        return self.key_service.get_l2_key()


    def _get_l3_key(self):
        '''
        Returns L3 key
        '''
        return self.key_service.get_l3_key()

