# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
from sectools.common.utils import c_path
from sectools.features.isc.signer.cass.base_keystore import BaseKeystore


class TrustKeystore(BaseKeystore):
    MESG_PROMPT_PASSWORD = '\nPlease enter the password for the trust keystore (server authentication):'

    def __init__(self, filepath=None, password="", keystoreType="JKS"):
        BaseKeystore.__init__(self, password)
        self._file = c_path.normalize(filepath)
        self._type = keystoreType

    @property
    def file(self):
        return self._file

    @property
    def type(self):
        return self._type


