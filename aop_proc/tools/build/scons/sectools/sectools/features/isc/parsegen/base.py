# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
'''
Created on Dec 9, 2013

@author: hraghav

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

    def __init__(self, data, imageinfo=None, general_properties=None, encdec=None,
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
        #. data (str): Data to be parsed.
        #. imageinfo (obj): Config object.

        Attributes:
        #. cert_chain_size (int): Estimated maximum size of the cert chain.
        #. max_cert_size (int): Estimated maximum size of one cert.
        #. sig_size (int): Size of the signature.
        #. num_certs_in_certchain(int): Number of certs in the cert chain.
        #. num_root_certs (int): Number of root certs used in the cert chain.
        #. hash_pageseg_as_segment (bool): Hash the page segments as segments
        """
        # Check the arguments
        if imageinfo is not None:
            general_properties = imageinfo.general_properties
            encdec = imageinfo.encdec
        if general_properties is None:
            raise RuntimeError('General properties must not be None.')

        # Initialize internal properties
        self._max_cert_size = 0
        self._sig_size = 0
        self._num_certs_in_certchain = 0
        self._num_root_certs = 0
        self._hash_pageseg_as_segment = False

        # Public properties
        self.encdec = encdec
        self.debug_dir = debug_dir
        self.debug_prefix = debug_prefix
        self.debug_suffix = debug_suffix
        self.pad_cert_chain = True
        self.secboot_version = general_properties.secboot_version
        self.qti_signing_enabled = general_properties.qti_sign
        self.oem_signing_enabled = general_properties.oem_sign
        self.oem_encryption_enabled = self.encdec is not None
        self.authority = None
        self.validating = validating
        self.signing = signing
        self.imageinfo = general_properties
        self.dependent_parsegens = parsegens
        self.add_signing_attr_to_hash_seg = sign_attr

        # Variables for security data
        self._data_signature = ''
        self._cert_chain = ''
        self._encryption_params = ''

        # Options for what security mechanism to apply
        self._integrity_check = False
        self._sign = False
        self._encrypt = False

        # Set properties from the config file
        self.segment_hash_algorithm = general_properties.segment_hash_algorithm
        self.max_cert_size = general_properties.max_cert_size
        if general_properties.dsa_type == "ecdsa":
            self.sig_size = ecdsa_functions.max_sig_size_map[general_properties.ecdsa_curve]
        else:
            self.sig_size = general_properties.key_size / 8
        self.num_certs_in_certchain = general_properties.num_certs_in_certchain
        self.num_root_certs = general_properties.num_root_certs
        self.max_num_root_certs = general_properties.max_num_root_certs
        if general_properties.hash_pageseg_as_segment is not None:
            self.hash_pageseg_as_segment = general_properties.hash_pageseg_as_segment

    def _repr_properties(self):
        properties = [
                      ('Integrity Check', self.contains_integrity_check()),
                      ('Signed', self.is_signed()),
                      ('Encrypted', self.is_encrypted()),
                      ('Size of signature', self.sig_size),
                      ('Size of one cert', self.max_cert_size),
                      ('Num of certs in cert chain',  self.num_certs_in_certchain),
                      ('Number of root certs', self.num_root_certs),
                      ('Maximum number of root certs', self.max_num_root_certs if self.max_num_root_certs is not None else self.num_root_certs),
                      ('Hash Page Segments as segments', self.hash_pageseg_as_segment),
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
    def get_data(self, integrity_check, sign, encrypt):
        #if encrypt and not self.is_signed():
        #    raise RuntimeError('Image is not signed. Cannot generate encrypted image.')
        if sign and not self.is_signed():
            raise RuntimeError('Image does not contain the required security metadata (signature & cert chain) needed to generate signed image.')

    @abc.abstractproperty
    def data_to_sign(self):
        """
        Return the binary data blob that is used for signing (creating the data
        signature).
        """
        raise RuntimeError('Method must be implemented by the derived class')

    def get_hash_segment_metadata(self, authority=None):
        """
        Return the signing attributes (of the provided authority) contained within the hash segment.
        Only applies to parsers that have a hash segment with metadata.
        """
        return None

    def get_multi_image_entry_data(self, authority):
        """
        Return the data whose hash will be added to the Multi-Image Sign & Integrity image. Data of the other authority must be masked per spec.
        Only applies to parsers that return True for supports_multi_image().
        """
        return None

    @classmethod
    def supports_multi_image(cls):
        """
        Return True if the parsegen supports the hash of its hash segment (excluding sig/certs) being placed within a Multi-Image Sign & Integrity image. False otherwise
        """
        return False

    def validate_hash_segment_metadata(self, authority=None):
        """
        Raise exception if parsegen's hash segment contains signing attributes that do not match the configured values.
        """
        pass

    #--------------------------------------------------------------------------
    # Properties that can be overloaded by the derived classes (optional)
    #--------------------------------------------------------------------------
    def validate_authority(self):
        pass

    def get_other_authority(self, authority):
        if authority == defines.AUTHORITY_OEM:
            return defines.AUTHORITY_QTI
        elif authority == defines.AUTHORITY_QTI:
            return defines.AUTHORITY_OEM
        else:
            raise RuntimeError("Cannot get other authority for unknown authority")

    @property
    def data_signature(self):
        return self._data_signature

    @data_signature.setter
    def data_signature(self, signature):
        if signature:
            self.validate_data_signature(signature)
        self._data_signature = signature

    def validate_data_signature(self, signature):
        if not isinstance(signature, str):
            raise RuntimeError('Signature must be of type "str". Given signature type is: "' + type(signature) + '"')
        #This is commented because this check fails 2k key to 4k key resigning
        #if len(signature) != self.sig_size:
        #    raise RuntimeError('Length of signature "' + str(len(signature)) + '" does not match the expected signature size "' + str(self.sig_size) + '"')

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
        if not isinstance(cert_chain, str):
            raise RuntimeError('Cert chain must be of type "str". Given cert chain type is: "' + type(cert_chain) + '"')
        if len(cert_chain) > self.get_cert_chain_size(self.authority):
            raise RuntimeError('Length of cert chain "' + str(len(cert_chain)) + '" is greater than the maximum cert_chain size "' + str(self.get_cert_chain_size(self.authority)) + '"')

    @property
    def encryption_params(self):
        return self._encryption_params

    @encryption_params.setter
    def encryption_params(self, encryption_params):
        if encryption_params:
            self.validate_encryption_params(encryption_params)
        self._encryption_params = encryption_params

    def validate_encryption_params(self, encryption_params):
        if not isinstance(encryption_params, str):
            raise RuntimeError('Encryption params must be of type "str". Given encryption params type is: "' + type(encryption_params) + '"')

    def get_wrapped_data(self):
        raise NotImplementedError

    def set_wrapped_data(self, data):
        raise NotImplementedError

    def get_signing_assets(self):
        return [(defines.AUTHORITY_OEM, self.data_to_sign, self.data_signature, self.cert_chain, None)]

    #--------------------------------------------------------------------------
    # Tests to check various attributes of the data
    #--------------------------------------------------------------------------
    @property
    def is_data_hash(self):
        """ Returns True if data is a hash """
        return False

    def is_signed(self, authority=defines.AUTHORITY_OEM):
        """ Returns True if data contains all the security metadata """
        if authority != defines.AUTHORITY_OEM:
            return False
        return self.contains_data_signature() and self.contains_cert_chain()

    def is_encrypted(self):
        """ Returns True if data contains all the encryption metadata """
        return self.contains_encryption_params()

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

    def contains_encryption_params(self):
        """ Returns True if data contains the encryption params """
        return self.encryption_params != ''

    #--------------------------------------------------------------------------
    # Get function for cert_chain_size
    #--------------------------------------------------------------------------
    def get_cert_chain_size(self, authority):
        if authority is None:
            raise RuntimeError("Cannot get certificate chain size of unknown authority")
        # get cert_chain_size of qti chert chain
        if authority == defines.AUTHORITY_QTI:
            # qti sign does not currently support MRC
            return self.max_cert_size * self.num_certs_in_certchain
        # get cert_chain_size of oem chert chain
        else:
            # exclusive sign case
            if self.max_num_root_certs is None:
                return self.max_cert_size * (self.num_certs_in_certchain + self.num_root_certs - 1)
            # double sign case
            else:
                return self.max_cert_size * (self.num_certs_in_certchain + self.max_num_root_certs - 1)

    #--------------------------------------------------------------------------
    # Get, Set & Validate functions for 'max_cert_size'
    #--------------------------------------------------------------------------
    @property
    def max_cert_size(self):
        return self._max_cert_size

    @max_cert_size.setter
    def max_cert_size(self, value):
        self.validate_max_cert_size(value)
        self._max_cert_size = value

    def validate_max_cert_size(self, value):
        if value <= 0:
            raise AttributeError('Invalid size of one cert "' + str(value) + '" provided. Must be greater than 0')

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
    # Get, Set & Validate functions for 'hash_pageseg_as_segment'
    #--------------------------------------------------------------------------
    @property
    def hash_pageseg_as_segment(self):
        return self._hash_pageseg_as_segment

    @hash_pageseg_as_segment.setter
    def hash_pageseg_as_segment(self, value):
        self.validate_hash_pageseg_as_segment(value)
        self._hash_pageseg_as_segment = value

    def validate_hash_pageseg_as_segment(self, value):
        pass

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
    # Get, Set functions for 'encrypt'
    #--------------------------------------------------------------------------
    @property
    def encrypt(self):
        return self._encrypt

    @encrypt.setter
    def encrypt(self, value):
        self._encrypt = True if value else False

    #--------------------------------------------------------------------------
    # Helper functions
    #--------------------------------------------------------------------------
    def store_debug_data(self, file_name, data, prefix=None, suffix=None):
        if prefix is None:
            prefix = self.debug_prefix
        if suffix is None:
            suffix = self.debug_suffix
        if prefix is not None and suffix is not None:
            store_debug_data_to_file(prefix + '_' + file_name + suffix,
                                     data, self.debug_dir)

    def encrypt_segment(self, segment_data, segment_num):
        return self.encdec.get_encryptor().encrypt_segment(segment_data, segment_num)

