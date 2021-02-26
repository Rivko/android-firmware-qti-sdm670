# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.common.crypto.functions.eciesad import EciesBase
from sectools.common.utils.c_process import run_command
from six import ensure_str


class EciesImpl(EciesBase):
    def __init__(self, module):
        super(EciesImpl, self).__init__(module)
        self.ecies = module

    def encrypt(self, key, aad, message):
            cmd = [self.ecies,
                   '--encrypt',
                   '--key', ensure_str(key),
                   '--aad', ensure_str(aad),
                   '--msg', ensure_str(message)]
            return run_command(cmd, log=False)

    def decrypt(self, key, cryptogram):
            cmd = [self.ecies,
                   '--decrypt',
                   '--key', ensure_str(key),
                   '--cryptogram', ensure_str(cryptogram)]
            return run_command(cmd, log=False)
