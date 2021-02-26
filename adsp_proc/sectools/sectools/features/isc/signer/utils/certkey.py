# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
'''
Created on Sep 28, 2015

@author: hraghav
'''

from sectools.features.isc.signer.signerutils.certconfigparser import CertProperty


class CertKeyPair(object):

    def __init__(self, cert=None, priv_key=None, pub_key=None, params=None, extfile=None):
        # Initialize values to None before calling setters
        self._cert = None
        self._priv_key = None
        self._pub_key = None
        self._params = None
        self._extfile = None

        # Call our setters for validation
        self.cert = cert
        self.priv_key = priv_key
        self.pub_key = pub_key
        self.params = params
        self.extfile = extfile

    def check_type(self, param, param_str, t_str=False, t_params=False,
                   optional=True):
        if param is None:
            if not optional:
                raise RuntimeError("None given for " + param_str)
        elif t_str and not isinstance(param, str):
            raise RuntimeError("Expected type 'str' for " + param_str)
        elif t_params and not isinstance(param, CertProperty):
            raise RuntimeError("Expected type 'CertProperty' for " + param_str)

    @property
    def cert(self):
        return self._cert

    @cert.setter
    def cert(self, cert):
        self.check_type(cert, 'cert', t_str=True)
        self._cert = cert

    @property
    def priv_key(self):
        return self._priv_key

    @priv_key.setter
    def priv_key(self, priv_key):
        self.check_type(priv_key, 'priv_key', t_str=True)
        self._priv_key = priv_key

    @property
    def pub_key(self):
        return self._pub_key

    @pub_key.setter
    def pub_key(self, pub_key):
        self.check_type(pub_key, 'pub_key', t_str=True)
        self._pub_key = pub_key

    @property
    def params(self):
        return self._params

    @params.setter
    def params(self, params):
        self.check_type(params, 'params', t_params=True)
        self._params = params

    @property
    def extfile(self):
        return self._extfile

    @extfile.setter
    def extfile(self, extfile):
        self.check_type(extfile, 'extfile', t_str=True)
        self._extfile = extfile
