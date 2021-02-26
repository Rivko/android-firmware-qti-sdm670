# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import json

from . import _data_handlers_list
from .base_handler import BaseCodeGenerator
from .base_handler import BaseDetailer
from .base_handler import BaseHandler
from .base_handler import BaseNormalizer
from .base_handler import BaseTransferMgr
from .base_handler import BaseValidator
from .data_detail import DataDetail
from .data_detail import DataType
from .enumerated_types import DISABLE
from .enumerated_types import ENUMERATED_DISABLE_ENABLE_BOUND_HEX64_TAG
from .enumerated_types import ENUMERATED_ENABLE_DISABLE_HEX64_TAG
from .enumerated_types import ENUMERATED_ENABLE_DISABLE_TAG
from .enumerated_types import ENUMERATED_NOP_ENABLE_DISABLE_TAG
from .enumerated_types import ENUMERATED_ROOT_KEY_TAG
from .enumerated_types import ENUMERATED_TYPES
from .enumerated_types import enumerated_values
from .hex_handler import HexNormalizer, HexHandler


class EnumeratedNormalizer(BaseNormalizer):

    def __init__(self, data_handler, tag=None):
        super(EnumeratedNormalizer, self).__init__(data_handler)
        self.tag = tag

    def normalize(self, i_value):
        super(EnumeratedNormalizer, self).normalize(i_value)

    def normalize_list(self, i_value_list):
        super(EnumeratedNormalizer, self).normalize_list(i_value_list)


class EnumeratedCodeGenerator(BaseCodeGenerator):

    def __init__(self, data_handler, tag=None):
        super(EnumeratedCodeGenerator, self).__init__(data_handler)
        self.tag = tag

    def generate_code(self, attr_key, attr_value):
        """See :meth:`.Data_Handler.BaseCodeGenerator.generate_code`"""
        assert isinstance(attr_key, str)
        assert isinstance(attr_value, ENUMERATED_TYPES[self.tag])
        return attr_key + ' = \'\''

    def get_dict_rep(self, attr_key, attr_value):
        pass


class EnumeratedDetailer(BaseDetailer):

    def __init__(self, data_handler, tag=None):
        super(EnumeratedDetailer, self).__init__(data_handler)
        self.tag = tag

    def detail(self, i_format):
        assert isinstance(i_format, ENUMERATED_TYPES[self.tag])
        return DataDetail(DataType.Enumerated, self.data_handler, i_format)


class EnumeratedValidator(BaseValidator):

    def __init__(self, data_handler, tag=None):
        super(EnumeratedValidator, self).__init__(data_handler)
        self.tag = tag

    def validate(self, i_value, i_format):
        return convert_enumerated_to_integer(self.tag, i_value)

    def validate_list(self, i_value_list, i_format):
        super(EnumeratedValidator, self).validate_list(i_value_list, i_format)


class EnumeratedTransferMgr(BaseTransferMgr):

    def __init__(self, data_handler, tag=None):
        super(EnumeratedTransferMgr, self).__init__(data_handler)
        self.tag = tag

    def transfer(self, i_value_from, i_value_to, i_structure, i_structure_parent):
        assert isinstance(i_structure, ENUMERATED_TYPES[self.tag])
        return convert_enumerated_to_integer(self.tag, i_value_from)


class EnumeratedHandler(BaseHandler):
    """ Handler for enumerated data type. """
    tag = None

    def __init__(self, data_handler, tag=None):
        """Initialized the various features."""
        super(EnumeratedHandler, self).__init__(data_handler)
        self._i_detailer = EnumeratedDetailer(self, tag=tag)
        self._i_validator = EnumeratedValidator(self, tag=tag)
        self._i_normalizer = EnumeratedNormalizer(self, tag=tag)
        self._i_transfer_mgr = EnumeratedTransferMgr(self, tag=tag)
        self._i_code_generator = EnumeratedCodeGenerator(self, tag=tag)

    #--------------------------------------------------------------------------
    # Various properties that must be defined by the data handler if they
    # support the feature.
    #--------------------------------------------------------------------------
    @property
    def _normalizer(self):
        """Returns the :class:`EnumeratedNormalizer` object for the data type."""
        return self._i_normalizer

    @property
    def _validator(self):
        """Returns the :class:`EnumeratedValidator` object for the data type."""
        return self._i_validator

    @property
    def _detailer(self):
        """Returns the :class:`EnumeratedDetailer` object for the data type."""
        return self._i_detailer

    @property
    def _transfer_mgr(self):
        """Returns the :class:`EnumeratedTransferMgr` object for the data type."""
        return self._i_transfer_mgr

    @property
    def _code_generator(self):
        """Returns the :class:`HexCodeGenerator` object for the data type."""
        return self._i_code_generator

    #--------------------------------------------------------------------------
    # Suppress setting of these properties
    #--------------------------------------------------------------------------
    @_normalizer.setter
    def _normalizer(self, value):
        pass

    @_validator.setter
    def _validator(self, value):
        pass

    @_transfer_mgr.setter
    def _transfer_mgr(self, value):
        pass

    @_code_generator.setter
    def _code_generator(self, value):
        pass

    @_detailer.setter
    def _detailer(self, value):
        pass

    #--------------------------------------------------------------------------
    # Methods that must be implemented by the data handler
    #--------------------------------------------------------------------------
    @classmethod
    def get_data_handler_data_type(cls):
        """Returns the data type that is handled by this data handler"""
        return ENUMERATED_TYPES[cls.tag]

    @classmethod
    def get_data_handler(cls):
        """Returns the class reference of the data handler"""
        return EnumeratedHandler


def convert_enumerated_to_integer(tag, i_value):
    """ Convert an enumerated string to integer.

    There is a complication with some enumerated data types that use this method.
    It is best explained through a use case. One of the enumerated data types that
    use this method is in_use_soc_hw_version (in_use). A test starts with in_use
    set to "disable." Then the same test updates other config variables and writes
    the resulting config to a XML file. So in_use in the updated XML file turns out
    to be an empty string. This is the result of the following steps:

      1. Enumerated value "disable" equates to 0. So 0 is returned by this method.
      2. Since all enumerated data are string types in schema, as the config values
         are exported to the XML file, 0 is interpreted as an empty string or ""
         by the auto-generated Python function, quote_xml.

    Now the test continue by reading back the updated XML file. Variable in_use is
    now an empty string. The auto-generated Python code interprets it as None.

    Fortunately, all enumerated data types have the same mapping. That is "disable"
    maps to 0. They are synonymous. In other words, if an enumerated data type does
    not have this mapping, then it needs another conversion method.
    """
    if i_value is None:
        return 0

    enumerated_dict = enumerated_values[tag]
    try:
        if isinstance(i_value, str):
            if i_value in enumerated_dict.keys():
                o_value = enumerated_dict[i_value]
            elif int(i_value) in enumerated_dict.values():
                o_value = int(i_value)
            else:
                assert False
        elif isinstance(i_value, int):
            assert i_value in enumerated_dict.values()
            o_value = i_value
        else:
            assert False
    except (AssertionError, ValueError):
        raise RuntimeError("i_value: %s\nUse one of these values: %s" % (
            repr(i_value), str(sorted(
                [x for x in enumerated_dict.keys() if x not in ("true", "false")]))))

    assert isinstance(o_value, int)
    return o_value


def convert_enumerated_to_hex64(tag, i_value):
    """ Convert an enumerated string to a normalized 64-bit hex string.

    Similar to convert_enumerated_to_integer counterpart, return "disable" string
    when input value is None.
    """
    if i_value is None:
        return DISABLE

    hex_length = 16
    format_string = "%%0%dX" % hex_length
    enumerated_dict = enumerated_values[tag]
    try:
        if isinstance(i_value, int):
            assert i_value in enumerated_dict.values()
            o_value = format_string % i_value
        elif isinstance(i_value, str):
            if i_value in enumerated_dict.keys():
                o_value = format_string % enumerated_dict[i_value]
            elif i_value.startswith("0x"):
                o_value = format_string % int(i_value, 16)
            elif int(i_value) in enumerated_dict.values():
                o_value = format_string % int(i_value)
            else:
                assert False
        else:
            assert False
    except (AssertionError, ValueError):
        raise RuntimeError("i_value: %s\nUse one of these values: %s" % (
            repr(i_value), str(sorted(
                [x for x in enumerated_dict.keys() if x not in ("true", "false")]))))

    assert len(o_value) == hex_length, \
        "%s value is larger than 64-bit max: %d" % (tag, i_value)
    return "0x" + o_value


class EnumeratedUieRootKeyTypeHandler(EnumeratedHandler):

    tag = ENUMERATED_ROOT_KEY_TAG

    def __init__(self, data_handler):
        super(EnumeratedUieRootKeyTypeHandler, self).__init__(
            data_handler, self.tag)

    @classmethod
    def get_data_handler(cls):
        """Returns the class reference of this data handler"""
        return EnumeratedUieRootKeyTypeHandler


class EnumeratedHex64Validator(EnumeratedValidator):

    def validate(self, i_value, i_format):
        return convert_enumerated_to_hex64(self.tag, i_value)


class EnumeratedHex64TransferMgr(EnumeratedTransferMgr):

    def transfer(self, i_value_from, i_value_to, i_structure, i_structure_parent):
        assert isinstance(i_structure, ENUMERATED_TYPES[self.tag])
        return convert_enumerated_to_hex64(self.tag, i_value_from)


class EnumeratedHex64Handler(EnumeratedHandler):

    def __init__(self, data_handler):
        super(EnumeratedHex64Handler, self).__init__(
            data_handler, self.tag)
        self._i_validator = EnumeratedHex64Validator(self, self.tag)
        self._i_transfer_mgr = EnumeratedHex64TransferMgr(self, self.tag)


class EnumeratedDisableEnableBoundHex64Handler(EnumeratedHex64Handler):

    tag = ENUMERATED_DISABLE_ENABLE_BOUND_HEX64_TAG

    @classmethod
    def get_data_handler(cls):
        """Returns the class reference of this data handler"""
        return EnumeratedDisableEnableBoundHex64Handler


class EnumeratedEnableDisableHex64Handler(EnumeratedHex64Handler):

    tag = ENUMERATED_ENABLE_DISABLE_HEX64_TAG

    @classmethod
    def get_data_handler(cls):
        """Returns the class reference of this data handler"""
        return EnumeratedEnableDisableHex64Handler


class EnumeratedEnableDisableHandler(EnumeratedHandler):

    tag = ENUMERATED_ENABLE_DISABLE_TAG

    def __init__(self, data_handler):
        super(EnumeratedEnableDisableHandler, self).__init__(
            data_handler, self.tag)

    @classmethod
    def get_data_handler(cls):
        """Returns the class reference of this data handler"""
        return EnumeratedEnableDisableHandler


class EnumeratedNopEnableDisableNormalizer(HexNormalizer):

    def __init__(self, data_handler):
        super(EnumeratedNopEnableDisableNormalizer, self).__init__(data_handler)
        self.tag = ENUMERATED_NOP_ENABLE_DISABLE_TAG

    @staticmethod
    def get_hex_string(integer, hex_str_length):
        hex_str = ("%%0%dX" % hex_str_length) % integer
        if len(hex_str) != hex_str_length:
            raise ValueError("Hex string is too long: 0x" + hex_str)
        return "0x" + hex_str

    def _normalize_str(self, i_value):
        try:
            # This is needed, because the normalize method is called again
            # with a dictionary returned from the earlier call.
            i_value_dict = json.loads(i_value)
            assert isinstance(i_value_dict, dict)
            assert i_value_dict.keys()  # an empty dictionary is not allowed
            return i_value
        except ValueError:
            i_value = super(EnumeratedNopEnableDisableNormalizer, self).normalize(i_value)
            return self.get_hex_string(int(i_value, 16), 16)

    def _normalize_dict(self, o_value):
        config = {}
        for key, value in o_value.items():
            config[key] = self.get_hex_string(value, 16)
        return json.dumps(config)

    def normalize(self, i_value):
        try:
            o_value = enumerated_values[self.tag][i_value]
        except (KeyError, TypeError):
            o_value = i_value

        try:
            if isinstance(o_value, str):
                # The default use case, where the input is a hex string:
                # The string is normalized to 64-bit to be consumed by downstream.
                return self._normalize_str(o_value)
            elif isinstance(o_value, dict):
                # The new enumerated use case:
                # Return the JSON-stringified Python dictionary. The dictionary is key-value pair
                # of all secboot versions mapped to their respective 64-bit hex value strings.
                # This dictionary will in turn be used by secimageinfo to look up the corresponding
                # enumeration values of the secboot version according to config.
                return self._normalize_dict(o_value)
            elif isinstance(o_value, int):
                # Part of the new enumerated use case:
                # The enumerated values are integers. See cfgparser/defines.py for details.
                return self.get_hex_string(o_value, 16)
            else:
                assert False
        except Exception:
            raise RuntimeError("i_value: %s\nUse one of these values: %s" % (
                repr(i_value), str(sorted(
                    [x for x in enumerated_values[self.tag].keys() if x not in ("true", "false")]))))


class EnumeratedNopEnableDisableDetailer(BaseDetailer):

    def __init__(self, data_handler):
        super(EnumeratedNopEnableDisableDetailer, self).__init__(data_handler)
        self.tag = ENUMERATED_NOP_ENABLE_DISABLE_TAG

    def detail(self, i_format):
        """See :meth:`.Data_Handler.BaseDetailer.detail`"""
        assert isinstance(i_format, ENUMERATED_TYPES[self.tag])
        return DataDetail(DataType.Enumerated, self.data_handler, i_format)


class EnumeratedNopEnableDisableValidator(BaseValidator):

    def __init__(self, data_handler):
        super(EnumeratedNopEnableDisableValidator, self).__init__(data_handler)
        self.tag = ENUMERATED_NOP_ENABLE_DISABLE_TAG

    def validate(self, i_value, i_format):
        """See :meth:`.Data_Handler.BaseValidator.validate`"""
        assert isinstance(i_format, ENUMERATED_TYPES[self.tag])
        return self.data_handler.normalize(i_value)

    def validate_list(self, i_value_list, i_format):
        super(EnumeratedNopEnableDisableValidator, self).validate_list(i_value_list, i_format)


class EnumeratedNopEnableDisableTransferMgr(BaseTransferMgr):
    """TransferMgr for Enumerated data.

    :params: Same as :class:`BaseTransferMgr`.
    """
    def __init__(self, data_handler):
        super(EnumeratedNopEnableDisableTransferMgr, self).__init__(data_handler)
        self.tag = ENUMERATED_NOP_ENABLE_DISABLE_TAG

    def transfer(self, i_value_from, i_value_to, i_structure, i_structure_parent):
        """See :meth:`.Data_Handler.BaseTransferMgr.transfer`"""
        assert isinstance(i_structure, ENUMERATED_TYPES[self.tag])
        o_value = i_value_from
        if o_value is not None:
            assert isinstance(o_value, str)
        return o_value


class EnumeratedNopEnableDisableCodeGenerator(BaseCodeGenerator):
    """CodeGenerator for Hex data.

    :params: Same as :class:`BaseCodeGenerator`.
    """
    def generate_code(self, attr_key, attr_value):
        """See :meth:`.Data_Handler.BaseCodeGenerator.generate_code`"""
        assert isinstance(attr_key, str)
        assert isinstance(attr_value, ENUMERATED_TYPES[ENUMERATED_NOP_ENABLE_DISABLE_TAG])
        return attr_key + ' = \'\''

    def get_dict_rep(self, attr_key, attr_value):
        pass


class EnumeratedNopEnableDisableHandler(HexHandler):

    tag = ENUMERATED_NOP_ENABLE_DISABLE_TAG

    def __init__(self, data_handler):
        """Initialized the various features."""
        super(EnumeratedNopEnableDisableHandler, self).__init__(data_handler)
        self._i_detailer = EnumeratedNopEnableDisableDetailer(self)
        self._i_validator = EnumeratedNopEnableDisableValidator(self)
        self._i_normalizer = EnumeratedNopEnableDisableNormalizer(self)
        self._i_transfer_mgr = EnumeratedNopEnableDisableTransferMgr(self)
        self._i_code_generator = EnumeratedNopEnableDisableCodeGenerator(self)

    #--------------------------------------------------------------------------
    # Methods that must be implemented by the data handler
    #--------------------------------------------------------------------------
    @classmethod
    def get_data_handler_data_type(cls):
        """Returns the data type that is handled by this data handler"""
        return ENUMERATED_TYPES[cls.tag]

    @classmethod
    def get_data_handler(cls):
        """Returns the class reference of this data handler"""
        return EnumeratedNopEnableDisableHandler


_data_handlers_list.extend([
    EnumeratedNopEnableDisableHandler,
    EnumeratedEnableDisableHandler,
    EnumeratedUieRootKeyTypeHandler,
    EnumeratedEnableDisableHex64Handler,
    EnumeratedDisableEnableBoundHex64Handler,
])
