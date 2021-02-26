# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.common.core.plugin import SecPluginMgr


# API to get a signer
def get_signer(config):
    s = signer_mgr.get_signer(config.general_properties.selected_signer)
    return s(config)


class SignerMgr(SecPluginMgr):
    def get_signer(self, signer_id):
        s = self.get_map()
        if signer_id not in s:
            raise RuntimeError('Signer unavailable for signer id: ' + str(signer_id) + '\n' +
                               '    ' + 'Available signer ids are: ' + str(s.keys()))
        return s[signer_id]


signer_mgr = SignerMgr()

# Import all the signers
from sectools.features.isc.signer import cass_signer
from sectools.features.isc.signer import openssl_signer
from sectools.features.isc.signer import secimage_remote_client
from sectools.features.isc.signer import secimage_remote_server
from sectools.features.isc.signer import qti_remote_signer
from sectools.features.isc.signer import base_signer
try:
    from plugin import signer
except ImportError:
    signer = None

# Set the signers in the config file
try:
    from sectools.features.isc.cfgparser import defines

    defines.set_signers(signer_mgr.get_map().keys())
except Exception as e:
    signers = []
