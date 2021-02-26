# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
"""Provides data handling capabilities for hex data.

**Class Inheritance Diagram:**

.. inheritance-diagram:: coreutils.datautils.hex_handler
    :parts: 2

.. data::  type_hex

    Global instance of the TypeHex class.
"""

import re
import sys

from base_handler import BaseValidator, BaseTransferMgr, BaseDetailer, \
    BaseCodeGenerator, BaseHandler, BaseNormalizer, _CHAR_NEWLINE
from data_detail import DataDetail, DataType


# Delimiter used for splitting a string into a list of hex values.
_HEX_DELIM = _CHAR_NEWLINE


class TypeHex(object):
    """Custom type to specify Hex"""
    pass


# Global instance of the TypeHex class.
type_hex = TypeHex()


class HexHandler(BaseHandler):
    """Data handler for hex data.

    :params: Same as :class:`.base_handler.BaseHandler`.
    """

    def __init__(self, data_handler):
        """Initialized the various features."""
        BaseHandler.__init__(self, data_handler)
        self._i_normalizer = HexNormalizer(self)
        self._i_validator = HexValidator(self)
        self._i_transfer_mgr = HexTransferMgr(self)
        self._i_code_generator = HexCodeGenerator(self)
        self._i_detailer = HexDetailer(self)

    #--------------------------------------------------------------------------
    # Various properties that must be defined by the data handler if they
    # support the feature.
    #--------------------------------------------------------------------------
    @property
    def _normalizer(self):
        """Returns the :class:`HexNormalizer` object for the data type."""
        return self._i_normalizer

    @property
    def _validator(self):
        """Returns the :class:`HexValidator` object for the data type."""
        return self._i_validator

    @property
    def _transfer_mgr(self):
        """Returns the :class:`HexTransferMgr` object for the data type."""
        return self._i_transfer_mgr

    @property
    def _code_generator(self):
        """Returns the :class:`HexCodeGenerator` object for the data type."""
        return self._i_code_generator

    @property
    def _detailer(self):
        """Returns the :class:`HexDetailer` object for the data type."""
        return self._i_detailer

    #--------------------------------------------------------------------------
    # Methods that must be implemented by the data handler
    #--------------------------------------------------------------------------
    @classmethod
    def get_data_handler_data_type(cls):
        """Returns the data type that is handled by this data handler"""
        return TypeHex

    @classmethod
    def get_data_handler(cls):
        """Returns the class reference of this data handler"""
        return HexHandler


class HexNormalizer(BaseNormalizer):
    """Normalizer for Hex data.

    :params: Same as :class:`BaseNormalizer`.
    """

    def normalize(self, i_value):
        """See :meth:`.Data_Handler.BaseNormalizer.normalize`"""
        o_value = i_value

        # Ensure that the value is a string
        try: assert isinstance(o_value, str)
        except Exception: raise TypeError('o_value is not a hex value. o_value: ' + str(o_value) + ', i_value: ' + str(i_value))

        # Run string normalization
        o_value = self.generic_normalize(o_value)

        # Ensure that the value starts with 0x
        if not o_value.startswith('0x'):
            raise TypeError('o_value is not a hex value (must start with "0x"). o_value: ' + str(o_value) + ', i_value: ' + str(i_value))

        # Ensure that the value is hex
        try: int(o_value, 16)
        except Exception: raise TypeError('o_value is not a hex value. o_value: ' + str(o_value) + ', i_value: ' + str(i_value))

        # Return the normalized hex value
        return o_value

    def normalize_list(self, i_value_list):
        """See :meth:`.Data_Handler.BaseNormalizer.normalize_list`"""
        o_value_list = i_value_list

        # If i_value_list is a string, split it on the delimiter
        if isinstance(o_value_list, str):
            o_value_list = re.split(_HEX_DELIM, self.generic_normalize(o_value_list))

        # Ensure that the value is a list
        try: assert isinstance(o_value_list, list)
        except Exception: raise TypeError('o_value_list is not a list. o_value_list: ' + str(o_value_list) + ', i_value_list: ' + str(i_value_list))

        # Normalize the values in the list
        try: o_value_list = [self.normalize(val) for val in o_value_list]
        except Exception as e:
            raise type(e), type(e)(str(e) + '\n'
                                   '    ' + 'o_value_list: ' + str(o_value_list) + '\n'
                                   '    ' + 'i_value_list: ' + str(i_value_list)), sys.exc_info()[2]

        # Return the normalized hex list
        return o_value_list


class HexValidator(BaseValidator):
    """Validator for Hex data.

    :params: Same as :class:`BaseValidator`.
    """

    def validate(self, i_value, i_format):
        """See :meth:`.Data_Handler.BaseValidator.validate`"""
        assert isinstance(i_format, TypeHex)
        return self.data_handler.normalize(i_value)

    def validate_list(self, i_value_list, i_format):
        """See :meth:`.Data_Handler.BaseValidator.validate_list`"""
        assert isinstance(i_format, list)
        o_value_list = i_value_list

        # Normalize the i_value_list
        o_value_list = self.data_handler._normalizer.normalize_list(o_value_list)

        # Validate the values in the list
        try: o_value_list = [self.validate(val, i_format[0]) for val in o_value_list]
        except Exception as e:
            raise type(e), type(e)(str(e) + '\n'
                                   '    ' + 'i_value_list: ' + str(i_value_list) + '\n'
                                   '    ' + 'o_value_list: ' + str(o_value_list) + '\n'
                                   '    ' + 'i_format: ' + str(i_format)), sys.exc_info()[2]

        # Return the normalized hex list
        return o_value_list


class HexTransferMgr(BaseTransferMgr):
    """TransferMgr for Hex data.

    :params: Same as :class:`BaseTransferMgr`.
    """

    def transfer(self, i_value_from, i_value_to, i_structure, i_structure_parent):
        """See :meth:`.Data_Handler.BaseTransferMgr.transfer`"""
        assert isinstance(i_structure, TypeHex)
        o_value = i_value_from
        if o_value is not None:
            assert isinstance(o_value, str)
        return o_value


class HexCodeGenerator(BaseCodeGenerator):
    """CodeGenerator for Hex data.

    :params: Same as :class:`BaseCodeGenerator`.
    """

    def generate_code(self, attr_key, attr_value):
        """See :meth:`.Data_Handler.BaseCodeGenerator.generate_code`"""
        assert isinstance(attr_key, str)
        assert isinstance(attr_value, TypeHex)
        return attr_key + ' = \'\''


class HexDetailer(BaseDetailer):
    """Detailer for Hex data.

    :params: Same as :class:`BaseDetailer`.
    """

    def detail(self, i_format):
        """See :meth:`.Data_Handler.BaseDetailer.detail`"""
        assert isinstance(i_format, TypeHex)
        return DataDetail(DataType.Hex, self.data_handler, i_format)


#------------------------------------------------------------------------------
# Restrict all import
#------------------------------------------------------------------------------
__all__ = ['TypeHex',
           'type_hex',
           'HexHandler',
           'HexNormalizer',
           'HexValidator',
           'HexTransferMgr',
           'HexCodeGenerator',
           'HexDetailer',]
