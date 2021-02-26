# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from abc import ABCMeta, abstractmethod

class BaseAttribute(object):
    __metaclass__=ABCMeta

    def __init__(self, value=None, string=None):
        if value is not None:
            self.value = value
        elif string is not None:
            self.str = string
        else:
            raise RuntimeError("value and value_str cannot be both None!")

    @abstractmethod
    def convert_to_num_func(self, string, base):
        raise NotImplementedError()

    def validate_bit_len(self, n, bits):
        isValid = False
        try:
            bitstring=bin(n)
        except (TypeError, ValueError):
            return isValid

        if len(bitstring[2:]) <= bits:
            isValid = True

        return isValid


    def __str__(self):
        return self.str

    @property
    def str(self):
        return self.STRING_FORMAT % self.value

    @str.setter
    def str(self, string):
        base = 16
        if (string.lower().find("0x")!=0):
            base = 10
        self.value = self.convert_to_num_func(string, base)

    @property
    def value(self):
        return self._value

    @value.setter
    def value(self, value):
        if self.validate_bit_len(value, self.NUM_BITS) is False:
            raise RuntimeError("{0} is not a valid {2} bit integer".
                               format(value, self.NUM_BITS))
        self._value = value

    @property
    def unpadded_str(self):
        return "0x%x" % self.value

class Attribute16BIT(BaseAttribute):
    STRING_FORMAT = "0x%.4X"
    NUM_BITS = 16

    def convert_to_num_func(self, string, base):
        return int(string, base)

class Attribute32BIT(BaseAttribute):
    STRING_FORMAT = "0x%.8X"
    NUM_BITS = 32

    def convert_to_num_func(self, string, base):
        return int(string, base)

class Attribute64BIT(BaseAttribute):
    STRING_FORMAT = "0x%.16X"
    NUM_BITS = 64

    def convert_to_num_func(self, string, base):
        return long(string, base)

class Attribute(object):
    FUNC_REGISTRY = {16:Attribute16BIT,
                     32:Attribute32BIT,
                     64:Attribute64BIT
                     }

    @classmethod
    def init(cls, num_bits, value=None, string=None):
        if (value is None) and (string is None):
            return None

        if cls.FUNC_REGISTRY.has_key(num_bits) is False:
            raise RuntimeError("Bits={0} not supported".format(num_bits))

        attribute = None
        try:
            attribute = cls.FUNC_REGISTRY[num_bits](value, string)
        except:
            printString = string if string is not None else str(value)
            raise RuntimeError("{0} is not a valid {1} bit integer".\
                               format(printString, num_bits))

        return attribute

    @classmethod
    def validate(cls, num_bits, value=None, string=None):
        isValid = True

        try:
            Attribute.init(num_bits, value=value, string=string)
        except:
            isValid = False

        return isValid


