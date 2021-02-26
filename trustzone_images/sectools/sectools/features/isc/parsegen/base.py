# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
'''
This module contains the ParserBase class which is the base class for any
secure data parsing classes.
'''

import abc

from sectools.common.core.plugin import SecPluginIntf_Abs
from sectools.common.crypto import ecdsa as ecdsa_functions
from sectools.common.parsegen import PAD_BYTE_1
from sectools.common.utils.c_misc import properties_repr, store_debug_data_to_file, abstractclassmethod
from sectools.features.isc import defines
from sectools.features.isc.parsegen import parsegen_mgr


class SecParseGenBase(SecPluginIntf_Abs):

    def __init__(self, data,
                 imageinfo=None,
                 general_properties=None,
                 encdec=None,
                 debug_dir=None,
                 debug_prefix=None,
                 debug_suffix=None,
                 validating=False,
                 signing=False,
                 parsegens=None,
                 sign_attr=False):
        """
        Initializes properties and methods which are expected in all the
        parsers. These properties and methods can be overloaded by the derived
        class for parser specific logic. Also sets some properties based on the
        config object.

        Parameters:
        #. data (str): Data to be parsed, but not used in SecParseGenBase.
        #. imageinfo (obj): Config object.

        Attributes:
        #. cert_chain_size (int): Estimated maximum size of the cert chain.
        #. max_cert_size (int): Estimated maximum size of one cert.
        #. sig_size (int): Size of the signature.
        #. num_certs_in_certchain(int): Number of certs in the cert chain.
        #. num_root_certs (int): Number of root certs used in the cert chain.
        """
        # Check the arguments
        if imageinfo is not None:
            general_properties = imageinfo.general_properties
            encdec = imageinfo.encdec
        if general_properties is None:
            raise RuntimeError('General properties must not be None.')

        # Initialize internal properties
        self._sig_size = 0
        self._num_certs_in_certchain = 0
        self._num_root_certs = 0

        # Public properties
        self.encdec = encdec
        self.debug_dir = debug_dir
        self.debug_prefix = debug_prefix
        self.debug_suffix = debug_suffix
        self.pad_cert_chain = True
        self.secboot_version = general_properties.secboot_version
        self.oem_signing_enabled = general_properties.oem_sign
        self.authority = None
        self.validating = validating
        self.signing = signing
        self.imageinfo = general_properties
        self.dependent_parsegens = parsegens
        self.add_signing_attr_to_hash_seg = sign_attr

        # Variables for security data
        self._data_signature = ''
        self._cert_chain = ''

        # Options for what security mechanism to apply
        self._integrity_check = False
        self._sign = False

        # Set properties from the config file
        self.segment_hash_algorithm = general_properties.segment_hash_algorithm
        self.max_cert_size = defines.MAX_CERT_SIZE
        if general_properties.dsa_type == "ecdsa":
            self.sig_size = ecdsa_functions.max_sig_size_map[general_properties.ecdsa_curve]
        else:
            self.sig_size = int(general_properties.key_size / 8)
        self.num_certs_in_certchain = general_properties.num_certs_in_certchain
        self.num_root_certs = general_properties.num_root_certs
        self.max_num_root_certs = general_properties.max_num_root_certs
        self.max_num_certs_in_certchain = general_properties.max_num_certs_in_certchain

    def _repr_properties(self):
        properties = [
                      ('Integrity Check', self.contains_integrity_check()),
                      ('Signed', self.is_signed()),
                      ('Size of signature', self.sig_size),
                      ('Size of one cert', self.max_cert_size),
                      ('Num of certs in cert chain',  self.num_certs_in_certchain),
                      ('Number of root certs', self.num_root_certs),
                      ('Maximum number of root certs', self.max_num_root_certs),
                      ('Maximum number of cert levels', self.max_num_certs_in_certchain),
                      ('Cert chain size', self.get_cert_chain_size(self.authority)),
                     ]
        return properties

    def __repr__(self):
        return properties_repr(self._repr_properties())

    @classmethod
    def get_plugin_id(cls):
        return cls.file_type()

    @classmethod
    def get_plugin_manager(cls):
        return parsegen_mgr

    #--------------------------------------------------------------------------
    # Properties & Functions that must be implemented by the derived classes
    #--------------------------------------------------------------------------
    @abstractclassmethod
    def file_type(cls):
        raise RuntimeError('Class ' + cls.__name__ + ' must implement the class method file_type.')

    @abc.abstractmethod
    def get_data(self, integrity_check, sign, **kwargs):
        if sign and not self.is_signed():
            raise RuntimeError(
                'Image does not contain the required security metadata '
                '(signature & cert chain) needed to generate signed image.')

    @abc.abstractproperty
    def data_to_sign(self):
        """
        Return the binary data blob that is used for signing (creating the data
        signature).
        """
        raise RuntimeError('Method must be implemented by the derived class')

    #--------------------------------------------------------------------------
    # Properties that can be overloaded by the derived classes (optional)
    #--------------------------------------------------------------------------
    def validate_authority(self):
        pass

    @property
    def data_signature(self):
        return self._data_signature

    @data_signature.setter
    def data_signature(self, signature):
        if signature:
            self.validate_data_signature(signature)
        self._data_signature = signature

    def validate_data_signature(self, signature):
        if not isinstance(signature, bytes):
            raise RuntimeError('Signature must be of type "bytes". Given signature type is: "{}"'.format(type(signature)))

    @property
    def cert_chain(self):
        return self._cert_chain

    @cert_chain.setter
    def cert_chain(self, cert_chain):
        if cert_chain:
            self.validate_cert_chain(cert_chain)
            if self.pad_cert_chain:
                cert_chain = cert_chain.ljust(self.get_cert_chain_size(self.authority), PAD_BYTE_1)
        self._cert_chain = cert_chain

    def validate_cert_chain(self, cert_chain):
        if not isinstance(cert_chain, bytes):
            raise RuntimeError('Cert chain must be of type "bytes". Given cert chain type is: '
                               '"{}"'.format(type(cert_chain)))
        if self.validating and len(cert_chain) > self.get_cert_chain_size(self.authority):
            raise RuntimeError('Length of cert chain "' + str(len(cert_chain)) +
                               '" is greater than the maximum cert_chain size "' +
                               str(self.get_cert_chain_size(self.authority)) + '"')

    def get_wrapped_data(self):
        raise NotImplementedError

    def set_wrapped_data(self, data):
        raise NotImplementedError

    def get_signing_assets(self):
        return [(defines.AUTHORITY_OEM, self.data_to_sign, self.data_signature, self.cert_chain, None)]

    @property
    def data_to_sign_format(self):
        """ Return the format of the binary data blob that is signed """
        return None

    #--------------------------------------------------------------------------
    # Tests to check various attributes of the data
    #--------------------------------------------------------------------------
    @property
    def has_mbn_parsegen(self):
        return False

    @property
    def is_data_hash(self):
        """ Returns True if data is a hash """
        return False

    def is_signed(self, authority=defines.AUTHORITY_OEM):
        """ Returns True if data contains all the security metadata """
        if authority != defines.AUTHORITY_OEM:
            return False
        return self.contains_data_signature() and self.contains_cert_chain()

    @abc.abstractmethod
    def contains_integrity_check(self):
        """ Returns True if data contains integrity check metadata """
        pass

    def contains_data_signature(self):
        """ Returns True if data contains the data signature """
        return self.data_signature != ''

    def contains_cert_chain(self):
        """ Returns True if data contains the certificate chain """
        return self.cert_chain != ''

    #--------------------------------------------------------------------------
    # Get function for cert_chain_size
    #--------------------------------------------------------------------------
    def get_cert_chain_size(self, authority):
        if authority is None:
            raise RuntimeError("Cannot get certificate chain size of unknown authority")
        num_certs_in_certchain = (self.num_certs_in_certchain if self.max_num_certs_in_certchain is None
                                  else self.max_num_certs_in_certchain)
        num_root_certs = self.num_root_certs if self.max_num_root_certs is None else self.max_num_root_certs
        return self.max_cert_size * (num_certs_in_certchain + num_root_certs - 1)

    #--------------------------------------------------------------------------
    # Get, Set & Validate functions for 'sig_size'
    #--------------------------------------------------------------------------
    @property
    def sig_size(self):
        return self._sig_size

    @sig_size.setter
    def sig_size(self, value):
        self.validate_sig_size(value)
        self._sig_size = value

    def validate_sig_size(self, value):
        if value <= 0:
            raise AttributeError('Invalid size of signature "' + str(value) + '" provided. Must be greater than 0')

    #--------------------------------------------------------------------------
    # Get, Set & Validate functions for 'num_certs_in_certchain'
    #--------------------------------------------------------------------------
    @property
    def num_certs_in_certchain(self):
        return self._num_certs_in_certchain

    @num_certs_in_certchain.setter
    def num_certs_in_certchain(self, value):
        self.validate_num_certs_in_certchain(value)
        self._num_certs_in_certchain = value

    def validate_num_certs_in_certchain(self, value):
        pass

    #--------------------------------------------------------------------------
    # Get, Set & Validate functions for 'num_root_certs'
    #--------------------------------------------------------------------------
    @property
    def num_root_certs(self):
        return self._num_root_certs

    @num_root_certs.setter
    def num_root_certs(self, value):
        self.validate_num_root_certs(value)
        self._num_root_certs = value

    def validate_num_root_certs(self, value):
        if value <= 0:
            raise AttributeError('Invalid number of root certs "' + str(value) + '" provided. Must be greater than 0')

    #--------------------------------------------------------------------------
    # Get, Set functions for 'integrity_check'
    #--------------------------------------------------------------------------
    @property
    def integrity_check(self):
        return self._integrity_check

    @integrity_check.setter
    def integrity_check(self, value):
        self._integrity_check = True if value else False

    #--------------------------------------------------------------------------
    # Get, Set functions for 'sign'
    #--------------------------------------------------------------------------
    @property
    def sign(self):
        return self._sign

    @sign.setter
    def sign(self, value):
        self._sign = True if value else False

    #--------------------------------------------------------------------------
    # Helper functions
    #--------------------------------------------------------------------------
    def store_debug_data(self, file_name, data, debug_dir=None, prefix=None, suffix=None):
        if prefix is None:
            prefix = getattr(self, "debug_prefix", None)
        if suffix is None:
            suffix = getattr(self, "debug_suffix", None)
        if debug_dir is None:
            debug_dir = getattr(self, "debug_dir", None)
        if prefix is not None and suffix is not None:
            store_debug_data_to_file(prefix + '_' + file_name + suffix, data, debug_dir)
