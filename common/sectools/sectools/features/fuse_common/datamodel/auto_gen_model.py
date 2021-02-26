"""Data model that represents the fuses that need to be blown.
"""


import abc
import copy
import math

import defines
from sectools.common.utils.c_misc import TablePrinter
from sectools.common.utils.c_logging import logger


BIT_LENGTH_32_BITS = 32
BIT_LENGTH_64_BITS = 64
HEX_START_CHARS = '0x'
HEX_START_CHARS_LEN = len(HEX_START_CHARS)
HEX_CHAR_LEN = 4
FUSE_VALUE_MAX_BITS = BIT_LENGTH_64_BITS

INT_BASE_HEX = 16
INT_BASE_DEC = 10
INT_BIT_LEN = 4


class Cfg_Validations(object):

    @classmethod
    def get_bit_length(cls, bit_length):
        return int(math.ceil(float(bit_length) / INT_BIT_LEN) * INT_BIT_LEN)

    @classmethod
    def get_value_bit_length(cls, value):
        return len(bin(int(value.rstrip("L"), INT_BASE_HEX))[2:])

    @classmethod
    def normalize_value(cls, value, bit_length=None):
        """Returns the normalized value. Converts a decimal value to the
        corresponding hex value.

        :param str value: The value to normalize.
        :param str bit_length: The number of bits to use for justifying the
            value. If this is None, no justification is done.
        :returns: normalized hex value.
        :rtype: str
        """
        value = value.rstrip("L")
        if value.startswith(HEX_START_CHARS):
            retval = int(value, INT_BASE_HEX)
        else:
            retval = int(value, INT_BASE_DEC)
        retval = hex(retval).rstrip("L")

        if bit_length is not None:
            bit_length = cls.get_bit_length(bit_length) / INT_BIT_LEN
            retval = (retval[:HEX_START_CHARS_LEN] +
                      retval[HEX_START_CHARS_LEN:].rjust(bit_length, '0'))
        return retval

    @classmethod
    def check_value(cls, value, bit_length):
        """Checks that the given value is of the correct bit length.

        :param str value: The value to check.
        :raises: RuntimeError if the value is not correct.
        """
        value = value.rstrip("L")
        value_bit_length = cls.get_value_bit_length(value)

        if bit_length < value_bit_length:
            raise RuntimeError('Bit length should be equal or less than: "' + str(bit_length) + '". '
                               'Bit length of value "' + str(value) + '" is: "' + str(value_bit_length) + '"')

class Cfg_Base(object):
    """Base class to be used by all the classes in the data model.
    """

    __metaclass__ = abc.ABCMeta

    @abc.abstractmethod
    def validate(self):
        """(abstractmethod) Validates the contents of the object.

        :raises: RuntimeError in case of validation failure.
        """
        pass

    @abc.abstractmethod
    def sort(self):
        """(abstractmethod) Sorts the contents of the object.

        :raises: RuntimeError in case of validation failure.
        """

    @abc.abstractproperty
    def get_key_for_sort(self):
        """(abstractmethod) Returns the id of the object to be used for sorting
        multiple objects in a list.

        :returns: id of the oject
        """
        pass

    @abc.abstractmethod
    def get_id_for_str(self):
        """(abstractmethod) Returns the id for string representation of this
        object. This can be None.

        :returns: id of the oject
        :rtype: str, None
        """
        pass

    @abc.abstractmethod
    def get_attr_list(self):
        """(abstractmethod) Returns the list of attributes that should be
        included in the repr.

        :returns: list of attributes
        :rtype: lis[str]
        """
        pass

    def get_attr_dict(self):
        """Returns the dict of attributes that should be included in the repr.

        :returns: dict of attributes
        :rtype: dict
        """
        dict_rep = dict()
        for attr in self.get_attr_list():
            dict_rep[attr] = getattr(self, attr)
        return dict_rep

    def validate_list_id(self, obj_list, key):
        """Validates that the keys of the objects are unique.

        :param list obj_list: the list to be validated.
        :param cb key: Callback to obtain the key to use for validation.
        :raises: RuntimeError in case of validation failure.
        """
        assert isinstance(obj_list, list)
        id_list = []
        for obj in obj_list:
            obj_id = key(obj)
            if obj_id in id_list:
                raise RuntimeError('ID "' + obj_id + '" already exists in: ' + str(id_list))
            id_list.append(obj_id)

    def validate_list(self, obj_list):
        """Validates that the sort keys of the objects are unique. The objects
        in this list must support :meth:`get_key_for_sort`. Also calls
        :meth:`validate` on the objects in the given list.

        :param list obj_list: the list to be validated.
        :raises: RuntimeError in case of validation failure.
        """
        assert isinstance(obj_list, list)
        self.validate_list_id(obj_list, lambda x : x.get_key_for_sort())
        for obj in obj_list:
            obj.validate()

    def sort_list(self, obj_list):
        """Sort the given list. The objects in this list must support
        :meth:`get_key_for_sort` which will be used for sorting. The list is
        sorted in place. Also calls :meth:`sort` on the objects in the given
        list.

        :param list obj_list: the list to be sorted.
        :raises: RuntimeError in case of sorting failure.
        """
        assert isinstance(obj_list, list)
        obj_list.sort(key=lambda x : x.get_key_for_sort())
        for obj in obj_list:
            obj.sort()

    def to_JSON(self):
        """Returns a json dump of the object.

        :returns: Json dump of the object.
        :rtype: str
        """
        import json
        return json.dumps(self, default=lambda o: get_attr_dict(o), sort_keys=True, indent=4)

    def __str__(self):
        """Returns a short representation of the object. This will print the id
        returned by :meth:`get_id_for_str` if the id is not None. If id is None,
        it will print the name of the class.

        :returns: Short representation of the object.
        :rtype: str
        """
        obj_id = self.get_id_for_str()
        return ('Instance of ' + self.__class__.__name__) if obj_id is None else obj_id

    def __repr__(self):
        """Returns the detailed string representation of an object. This will
        first print all the bool, int, string, None attributes and then print
        the lists and the representation of the other objects.

        :returns: Detailed representation of the object.
        :rtype: str
        """
        retval = ''

        # Define any constants that we will be using.
        TAB = '    '
        SEP = ': '
        NL = '\n'
        COMMA = ','
        BR_OPEN = '['
        BR_CLOSE = ']'

        # Dictionary for repr
        attrs = self.get_attr_list()

        # First add simple ones
        for attr in sorted(attrs):
            value = getattr(self, attr)
            if isinstance(value, (bool, int, long, str, type(None))):
                value = repr(value)
            else:
                continue
            retval += str(attr) + SEP + value + NL

        # Now add lists and other objects
        for attr in sorted(attrs):
            value = getattr(self, attr)
            if isinstance(value, (bool, int, long, str, type(None))):
                continue
            elif isinstance(value, list):
                value = (BR_OPEN + NL +
                         (COMMA + NL + NL).join([repr(each) for each in value]) + NL +
                         BR_CLOSE + NL)
            else:
                value = repr(value)
            retval += (str(attr) + SEP + NL +
                       NL.join([TAB + line for line in value.split(NL)]))

        # Return the representaion of the object.
        return (self.__class__.__name__ + SEP + NL +
                NL.join([TAB + line for line in retval.split(NL)])).strip()

def get_attr_dict(obj):
    """Returns the dict of attributes for an object.

    :returns: dict of attributes
    :rtype: dict
    """
    if isinstance(obj, (Cfg_Fuseblower,
                        Cfg_Module,
                        Cfg_Fuse_Region,
                        Cfg_Fuse,
                        Cfg_Field)):
        return obj.get_attr_dict()
    else:
        return obj.__dict__


class Cfg_Fuseblower(Cfg_Base):
    """Container for all the data in the data model.
    """

    def __init__(self):
        """Initializations and any checks.
        """
        # Internal properties
        self._module = []

    @property
    def module(self):
        """(list) List of the modules inside the data model. These modules are
        of type :class:`Cfg_Module`
        """
        return self._module

    @module.setter
    def module(self, value):
        # Check the types of all the values being set
        assert isinstance(value, list)
        for module in value:
            assert isinstance(module, Cfg_Module)
        self._module = value

    def get_attr_list(self):
        """See :meth:`Cfg_Base.get_attr_list`
        """
        return ['module']

    def reset(self):
        """Resets the data model to be empty.
        """
        self.module = []

    def validate(self):
        """See :meth:`Cfg_Base.validate`
        Validates all the modules.
        """
        self.validate_list(self.module)

    def sort(self):
        """See :meth:`Cfg_Base.sort`
        Sorts the modules by their id.
        """
        self.sort_list(self.module)

    def get_key_for_sort(self):
        """See :meth:`Cfg_Base.get_key_for_sort`
        Does not support a key for sorting.
        """
        raise NotImplemented

    def get_id_for_str(self):
        """See :meth:`Cfg_Base.get_id_for_str`
        """
        return None

    def add_module(self, module):
        """Adds a module to the :meth:`module`

        :param :class:`Cfg_Module` module: Module to be added to the list.
        """
        assert isinstance(module, Cfg_Module)
        self.module.append(module)
        self.sort()

    def get_module(self, _id):
        """Return the module corresponding to the given _id.

        :param _id: Id of the module to be returned.
        :return: Module
        :rtype: :class:`Cfg_Module`
        :raises: RuntimeError if the _id is not in the list of modules.
        """
        for module in self.module:
            if module.id == _id:
                return module
        else:
            raise RuntimeError('Module not found for id: ' + _id)

    def set_fuses_owner(self, owner=None):
        """Set the owner of all the fuses in the data model to the given owner
        value

        :param str owner: Owner to be set for all the fuses.
        """
        assert isinstance(owner, (str, type(None)))
        if owner is not None and owner not in defines.SUPPORTED_OWNERS:
            raise RuntimeError('Unknown owner: ' + owner + '\n'
                               '    ' + 'Supported owners are: ' + str(defines.SUPPORTED_OWNERS))

        # Go throught the data model chain and update the owner for all fields
        for module in self.module:
            for fuse_region in module.fuse_region:
                for fuse in fuse_region.fuse:
                    for field in fuse.field:
                        field.owner = owner

    def set_fuses_values(self, value):
        assert isinstance(value, str)

        # Go throught the data model chain and update the values for all fuses
        for module in self.module:
            for fuse_region in module.fuse_region:
                for fuse in fuse_region.fuse:
                    fuse.value = value

    def __add__(self, other):
        """Add another data model to self. This will basically combine the two
        data models together.

        :param :class:`Cfg_Fuseblower` other: The second data model to add to
            self.
        :returns: An object with the merged data model.
        :rtype: :class:`Cfg_Fuseblower`
        """
        assert isinstance(other, Cfg_Fuseblower)

        # The return value is a new instance of Cfg_Fuseblower
        retval = Cfg_Fuseblower()

        # List to keep track of the modules from self which have been added to
        # retval
        modules_covered = []

        # Loop through the modules in other and merge the modules when they
        # match.
        for other_module in other.module:
            other_module_id = other_module.id

            # Look for the module in self
            try:
                self_module = self.get_module(other_module_id)
            except:
                self_module = None

            # Add the module to the retval
            if self_module is None:
                retval.add_module(other_module)
            else:
                modules_covered.append(self_module)
                retval.add_module(self_module + other_module)

        # Loop through the remaining modules from self
        for self_module in self.module:
            if self_module not in modules_covered:
                retval.add_module(self_module)

        return retval

    def _compare(self, other):
        """Compares the other data model to self.

        :param :class:`Cfg_Fuseblower` other: The second data model to compare
            to self.
        :returns: The differences between the data models
        :rtype: tuple(list)
        """
        assert isinstance(other, Cfg_Fuseblower)
        from sectools.features.fuse_common.parsegen.secdat import SecdatParseGen

        # Get a list of the values that should be matched.
        fuse_dict_self = dict()
        fuse_dict_other = dict()

        # Create the dict from self
        for module in self.module:
            for fuse_region in module.fuse_region:
                for fuse in fuse_region.fuse:
                    if fuse.ignore:
                        continue
                    fuse_dict_self[fuse.address] = (SecdatParseGen._get_region_type_enum(fuse_region.id), fuse.operation, fuse.value)

        # Create the dict from other
        for module in other.module:
            for fuse_region in module.fuse_region:
                for fuse in fuse_region.fuse:
                    if fuse.ignore:
                        continue
                    fuse_dict_other[fuse.address] = (SecdatParseGen._get_region_type_enum(fuse_region.id), fuse.operation, fuse.value)

        # Get a list of all the keys in both the dicts
        all_fuse_addresses = list(set(fuse_dict_self.keys() + fuse_dict_other.keys()))

        # List of errors
        mismatching = []
        missing_self = []
        missing_other = []

        # Loop through all the fuse addresses
        for fuse_address in all_fuse_addresses:
            self_fuse_data = fuse_dict_self.get(fuse_address, None)
            other_fuse_data = fuse_dict_other.get(fuse_address, None)

            if self_fuse_data is None:
                if other_fuse_data is None:
                    raise RuntimeError('Both the self and other returned no address. This should not have happened')
                else:
                    missing_self.append((fuse_address, other_fuse_data))
            else:
                if other_fuse_data is None:
                    missing_other.append((fuse_address, self_fuse_data))
                else:
                    if self_fuse_data != other_fuse_data:
                        mismatching.append((fuse_address, self_fuse_data, other_fuse_data))

        # Return the differences
        return (mismatching, missing_self, missing_other)

    def _get_compare_error_string(self, mismatching, missing_self, missing_other,
                                  dm_1_name='data_model_1', dm_2_name='data_model_2'):
        """Returns the error string for differences.
        """
        error_string = ''
        idx = 0
        if missing_self or missing_other or mismatching:
            error_string += 'Following differences were found between ' + dm_1_name + ' and ' + dm_2_name + '\n\n'
        if missing_self:
            idx += 1
            error_string += logger.increase_indent(self._get_compare_error_string_missing(idx, missing_self, dm_1_name, dm_2_name)).rstrip() + '\n\n'
        if missing_other:
            idx += 1
            error_string += logger.increase_indent(self._get_compare_error_string_missing(idx, missing_other, dm_2_name, dm_1_name)).rstrip() + '\n\n'
        if mismatching:
            idx += 1
            error_string += logger.increase_indent(self._get_compare_error_string_mismatching(idx, mismatching, dm_1_name, dm_2_name)).rstrip() + '\n\n'
        return error_string

    def _get_compare_error_string_missing(self, idx, missing_list, dm_1_name, dm_2_name):
        """Returns error string for the missing data.
        """
        self_error_string = str(idx) + '. Following fuses are in ' + str(dm_2_name) + ' but not in ' + dm_1_name + ':' + '\n\n'

        # Create a table of the missing fuses
        tp = TablePrinter([0])

        # Header for the table
        tp.insert_data(0, 0, 'S.No.')
        tp.insert_data(0, 1, 'Address')
        tp.insert_data(0, 2, 'Region_ID')
        tp.insert_data(0, 3, 'Operation')
        tp.insert_data(0, 4, 'Value')

        for i, fuse_data in enumerate(missing_list):
            idx = i + 1

            # Insert the info in the table
            tp.insert_data(idx, 0, str(i + 1))
            tp.insert_data(idx, 1, str(fuse_data[0]))
            tp.insert_data(idx, 2, str(fuse_data[1][0]))
            tp.insert_data(idx, 3, str(fuse_data[1][1]))
            tp.insert_data(idx, 4, str(fuse_data[1][2]))

        self_error_string += tp.get_data()
        return self_error_string

    def _get_compare_error_string_mismatching(self, idx, mismatching, dm_1_name, dm_2_name):
        """Returns error string for the list of mismatching data.
        """
        self_error_string = str(idx) + '. Following fuses are mismatching between ' + dm_1_name + ' and ' + dm_2_name + ':' + '\n\n'

        # Create a table of the missing fuses
        tp = TablePrinter([0])

        # Header for the table
        tp.insert_data(0, 0, 'S.No.')
        tp.insert_data(0, 1, 'DataModel')
        tp.insert_data(0, 2, 'Address')
        tp.insert_data(0, 3, 'Region_ID')
        tp.insert_data(0, 4, 'Operation')
        tp.insert_data(0, 5, 'Value')

        for i, fuse_data in enumerate(mismatching):
            idx = (i * 3) + 1

            pre_r_id = '*' if (fuse_data[1][0] != fuse_data[2][0]) else ''
            pre_op = '*' if (fuse_data[1][1] != fuse_data[2][1]) else ''
            pre_val = '*' if (fuse_data[1][2] != fuse_data[2][2]) else ''

            # Insert info from dm_1 in the table
            tp.insert_data(idx, 0, str(i + 1))
            tp.insert_data(idx, 1, dm_1_name)
            tp.insert_data(idx, 2, str(fuse_data[0]))
            tp.insert_data(idx, 3, pre_r_id + str(fuse_data[1][0]))
            tp.insert_data(idx, 4, pre_op + str(fuse_data[1][1]))
            tp.insert_data(idx, 5, pre_val + str(fuse_data[1][2]))

            # Increase index for second data model
            idx += 1

            # Insert info from dm_1 in the table
            tp.insert_data(idx, 1, dm_2_name)
            tp.insert_data(idx, 2, str(fuse_data[0]))
            tp.insert_data(idx, 3, pre_r_id + str(fuse_data[2][0]))
            tp.insert_data(idx, 4, pre_op + str(fuse_data[2][1]))
            tp.insert_data(idx, 5, pre_val + str(fuse_data[2][2]))

        self_error_string += tp.get_data()
        return self_error_string

    def __eq__(self, other):
        """Compare another data model to self.

        :param :class:`Cfg_Fuseblower` other: The second data model to compare
            to self.
        :returns: True if the data models, raise RuntimeError otherwise
        :rtype: bool
        """
        mismatching, missing_self, missing_other = self._compare(other)
        error_string = self._get_compare_error_string(mismatching,
                                                      missing_self,
                                                      missing_other)
        if error_string:
            raise RuntimeError(error_string.rstrip())
        return True

    def compare(self, other, dm_1_name, dm_2_name):
        """Compare another data model to self. Allows user to provide the names
        of the data models.

        :param :class:`Cfg_Fuseblower` other: The second data model to compare
            to self.
        :returns: True if the data models, raise RuntimeError otherwise
        :rtype: bool
        """
        mismatching, missing_self, missing_other = self._compare(other)
        error_string = self._get_compare_error_string(mismatching,
                                                      missing_self,
                                                      missing_other,
                                                      dm_1_name,
                                                      dm_2_name)
        if error_string:
            raise RuntimeError(error_string.rstrip())
        return True


class Cfg_Module(Cfg_Base):
    """Container for all the module specific data.
    """

    def __init__(self):
        """Initializations and any checks.
        """
        # Internal properties
        self._id = ''
        self._base_address = ''
        self._description = ''
        self._fuse_region = []

    @property
    def id(self):
        """(str) ID of the module.
        """
        return self._id

    @id.setter
    def id(self, value):
        assert isinstance(value, str)
        if not value:
            raise RuntimeError('ID must be defined')
        self._id = value

    @property
    def base_address(self):
        """(str) Base Address of the module.
        """
        return self._base_address

    @base_address.setter
    def base_address(self, value):
        assert isinstance(value, str)
        try:
            Cfg_Validations.check_value(value, BIT_LENGTH_32_BITS)
        except Exception as e:
            e.args += (' Error while setting base_address to: ' + value,)
            raise
        self._base_address = Cfg_Validations.normalize_value(value, BIT_LENGTH_32_BITS)

    @property
    def description(self):
        """(str) Description of the module.
        """
        return self._description

    @description.setter
    def description(self, value):
        assert isinstance(value, (str, type(None)))
        self._description = value

    @property
    def fuse_region(self):
        """(list) List of the fuse_regions inside the module. These fuse_regions
        are of type :class:`Cfg_Fuse_Region`
        """
        return self._fuse_region

    @fuse_region.setter
    def fuse_region(self, value):
        # Check the types of all the values being set
        assert isinstance(value, list)
        for fuse_region in value:
            assert isinstance(fuse_region, Cfg_Fuse_Region)
        self._fuse_region = value

    def get_attr_list(self):
        """See :meth:`Cfg_Base.get_attr_list`
        """
        return ['id',
                'base_address',
                'description',
                'fuse_region',]

    def reset(self):
        """Resets the module by removing all the fuse regions.
        """
        self.fuse_region = []

    def validate(self):
        """See :meth:`Cfg_Base.validate`
        """
        self.validate_list(self.fuse_region)

    def sort(self):
        """See :meth:`Cfg_Base.sort`
        """
        self.sort_list(self.fuse_region)

    def get_key_for_sort(self):
        """See :meth:`Cfg_Base.get_key_for_sort`
        """
        return self.id

    def get_id_for_str(self):
        """See :meth:`Cfg_Base.get_id_for_str`
        """
        return self.id

    def add_fuse_region(self, fuse_region):
        """Adds a fuse_region to the :meth:`fuse_region`

        :param :class:`Cfg_Fuse_Region` fuse_region: Fuse region to be added
            to the list
        """
        assert isinstance(fuse_region, Cfg_Fuse_Region)
        self.fuse_region.append(fuse_region)
        self.sort()

    def get_fuse_region(self, _id):
        """Return the fuse region corresponding to the given _id.

        :param _id: Id of the fuse region to be returned.
        :return: Fuse Region
        :rtype: :class:`Cfg_Fuse_Region`
        :raises: RuntimeError if the _id is not in the list of fuse regions.
        """
        for fuse_region in self.fuse_region:
            if fuse_region.id == _id:
                return fuse_region
        else:
            raise RuntimeError('Fuse Region not found for id: ' + _id)

    def __add__(self, other):
        """Add data from another module to the data in self.

        :param :class:`Cfg_Module` other: The second module to add to self.
        :returns: An object with the merged module data.
        :rtype: :class:`Cfg_Module`
        """
        assert isinstance(other, Cfg_Module)

        # The return value is a new instance of Cfg_Module
        retval = Cfg_Module()

        # Ensure that the id matches
        if other.id != self.id:
            raise RuntimeError('ID for the module does not match: ' + '\n'
                               '    ' + 'ID 1: ' + self.id + '\n'
                               '    ' + 'ID 2: ' + other.id)
        else:
            retval.id = self.id

        # Ensure that the base address matches
        if other.base_address != self.base_address:
            raise RuntimeError('Base Address for the module does not match: ' + '\n'
                               '    ' + 'Base Address 1: ' + self.base_address + '\n'
                               '    ' + 'Base Address 2: ' + other.base_address)
        else:
            retval.base_address = self.base_address

        # Put the description
        if self.description:
            retval.description = self.description
        else:
            retval.description = self.description

        # List to keep track of the fuse regions from self which have been added
        # to retval
        fuse_regions_covered = []

        # Loop through the fuse regions in other and merge the fuse regions
        # when they match.
        for other_fuse_region in other.fuse_region:
            other_fuse_region_id = other_fuse_region.id

            # Look for the fuse_region in self
            try:
                self_fuse_region = self.get_fuse_region(other_fuse_region_id)
            except:
                self_fuse_region = None

            # Add the fuse_region to the retval
            if self_fuse_region is None:
                retval.add_fuse_region(other_fuse_region)
            else:
                fuse_regions_covered.append(self_fuse_region)
                retval.add_fuse_region(self_fuse_region + other_fuse_region)

        # Loop through the remaining fuse regions from self
        for self_fuse_region in self.fuse_region:
            if self_fuse_region not in fuse_regions_covered:
                retval.add_fuse_region(self_fuse_region)

        return retval


class Cfg_Fuse_Region(Cfg_Base):
    """Container for all the fuse region specific data.
    """

    def __init__(self):
        """Initializations and any checks.
        """
        # Internal properties
        self._id = ''
        self._description = ''
        self._fuse = []

    @property
    def id(self):
        """(str) ID of the fuse region.
        """
        return self._id

    @id.setter
    def id(self, value):
        assert isinstance(value, str)
        if not value:
            raise RuntimeError('ID must be defined')
        self._id = value

    @property
    def description(self):
        """(str) Description of the fuse region.
        """
        return self._description

    @description.setter
    def description(self, value):
        assert isinstance(value, (str, type(None)))
        self._description = value

    @property
    def fuse(self):
        """(list) List of the fuses inside the fuse region. These fuses are of
        type :class:`Cfg_Fuse`
        """
        return self._fuse

    @fuse.setter
    def fuse(self, value):
        # Check the types of all the values being set
        assert isinstance(value, list)
        for fuse in value:
            assert isinstance(fuse, Cfg_Fuse)
        self._fuse = value

    def get_attr_list(self):
        """See :meth:`Cfg_Base.get_attr_list`
        """
        return ['id',
                'description',
                'fuse',]

    def reset(self):
        """Resets the fuse region by removing all the fuses.
        """
        self.fuse = []

    def validate(self):
        """See :meth:`Cfg_Base.validate`
        """
        self.validate_list(self.fuse)

    def sort(self):
        """See :meth:`Cfg_Base.sort`
        """
        self.sort_list(self.fuse)

    def get_key_for_sort(self):
        """See :meth:`Cfg_Base.get_key_for_sort`
        """
        return self.id

    def get_id_for_str(self):
        """See :meth:`Cfg_Base.get_id_for_str`
        """
        return self.id

    def add_fuse(self, fuse):
        """Adds a fuse to the :meth:`fuse`

        :param :class:`Cfg_Fuse` fuse: Fuse to be added to the list
        """
        assert isinstance(fuse, Cfg_Fuse)
        self.fuse.append(fuse)
        self.sort()

    def get_fuse(self, n):
        """Return the fuse corresponding to the given n (row).

        :param n: N (Row) of the fuse to be returned.
        :return: Fuse
        :rtype: :class:`Cfg_Fuse`
        :raises: RuntimeError if the n is not in the list of fuses.
        """
        for fuse in self.fuse:
            if fuse.n == n:
                return fuse
        else:
            raise RuntimeError('Fuse not found for n: ' + n)

    def __add__(self, other):
        """Add data from another fuse region to the data in self.

        :param :class:`Cfg_Fuse_Region` other: The second fuse region to add to
            self.
        :returns: An object with the merged fuse region data.
        :rtype: :class:`Cfg_Fuse_Region`
        """
        assert isinstance(other, Cfg_Fuse_Region)

        # The return value is a new instance of Cfg_Fuse_Region
        retval = Cfg_Fuse_Region()

        # Ensure that the id matches
        if other.id != self.id:
            raise RuntimeError('ID for the fuse region does not match: ' + '\n'
                               '    ' + 'ID 1: ' + self.id + '\n'
                               '    ' + 'ID 2: ' + other.id)
        else:
            retval.id = self.id

        # Put the description
        if self.description:
            retval.description = self.description
        else:
            retval.description = self.description

        # List to keep track of the fuses from self which have been added to
        # retval
        fuses_covered = []

        # Loop through the fuses in other and merge the fuses when they match.
        for other_fuse in other.fuse:
            fuse_n = other_fuse.n

            # Look for the fuse in self
            try:
                self_fuse = self.get_fuse(fuse_n)
            except:
                self_fuse = None

            # Add the fuse to the return value
            if self_fuse is None:
                retval.add_fuse(other_fuse)
            else:
                fuses_covered.append(self_fuse)
                retval.add_fuse(self_fuse + other_fuse)

        # Add the fuses from self
        for self_fuse in self.fuse:
            if self_fuse not in fuses_covered:
                retval.add_fuse(self_fuse)

        return retval


class Cfg_Fuse(Cfg_Base):
    """Container for all the fuse specific data.
    """

    def __init__(self):
        """Initializations and any checks.
        """
        # Internal properties
        self._n = 0
        self._address = ''
        self._ignore = False
        self._operation = ''
        self._value = ''
        self._field = []

        # Set default value
        self.value = '0x0000000000000000'

    @property
    def n(self):
        """(str) N (row) of the fuse.
        """
        return self._n

    @n.setter
    def n(self, value):
        assert isinstance(value, (int, type(None)))
        self._n = value

    @property
    def address(self):
        """(str) Address of the Fuse.
        """
        return self._address

    @address.setter
    def address(self, value):
        assert isinstance(value, str)
        try:
            Cfg_Validations.check_value(value, BIT_LENGTH_32_BITS)
        except Exception as e:
            e.args += (' Error while setting address to: ' + value,)
            raise
        self._address = Cfg_Validations.normalize_value(value, BIT_LENGTH_32_BITS)

    @property
    def description(self):
        """(str) Description of the fuse.
        """
        return self._description

    @description.setter
    def description(self, value):
        assert isinstance(value, (str, type(None)))
        self._description = value

    @property
    def ignore(self):
        """(bool) If the fuse should be ignored and not be included.
        """
        return self._ignore

    @ignore.setter
    def ignore(self, value):
        assert isinstance(value, (bool, type(None)))
        self._ignore = value if value is not None else False

    @property
    def operation(self):
        """(str) Operation for the fuse.
        """
        return self._operation

    @operation.setter
    def operation(self, value):
        assert isinstance(value, str)
        if value not in defines.SUPPORTED_OPERATIONS:
            raise RuntimeError('Unknown operation: ' + value + '\n'
                               '    ' + 'Supported operations are: ' + str(defines.SUPPORTED_OPERATIONS))
        self._operation = value

    @property
    def value(self):
        """(str) Value of the Fuse.
        """
        return self._value

    @value.setter
    def value(self, value):
        assert isinstance(value, str)
        try:
            Cfg_Validations.check_value(value, BIT_LENGTH_64_BITS)
        except Exception as e:
            e.args += (' Error while setting value to: ' + value,)
            raise
        self._value = Cfg_Validations.normalize_value(value, BIT_LENGTH_64_BITS)

    @property
    def field(self):
        """(list) List of the fields inside the fuse. These fields are of type
        :class:`Cfg_Field`
        """
        return self._field

    @field.setter
    def field(self, value):
        # Check the types of all the values being set
        assert isinstance(value, list)
        for field in value:
            assert isinstance(field, Cfg_Field)
            field.set_parent_attributes(self.get_value_by_bits,
                                        self.set_value_by_bits)
        self._field = value

    def get_attr_list(self):
        """See :meth:`Cfg_Base.get_attr_list`
        """
        return ['n',
                'address',
                'ignore',
                'operation',
                'value',
                'field',]

    def get_value_by_bits(self, start_bit, end_bit):
        """Returns the value for the bits that are queried.

        :param int start_bit: The starting bit of the value
        :param int end_bit: The ending bit of the value
        :returns: Value in the [end_bit : start_bit] range
        :rtype: str
        """
        value = bin(int(Cfg_Validations.normalize_value(self.value), 16))
        value = value[2:]
        value = value.rjust(BIT_LENGTH_64_BITS, '0')
        value = value[len(value) - end_bit - 1 : len(value) - start_bit]
        return Cfg_Validations.normalize_value(hex(int(value, 2)), end_bit - start_bit + 1)

    def set_value_by_bits(self, value, start_bit, end_bit):
        """Sets the value value of the fuse using the start and end bits.

        :param str value: Value in the [end_bit : start_bit] range
        :param int start_bit: The starting bit of the value
        :param int end_bit: The ending bit of the value
        """
        value = bin(int(Cfg_Validations.normalize_value(value), 16))
        value = value[2:]
        value += start_bit * '0'
        value = value.rjust(BIT_LENGTH_64_BITS, '0')

        mask = bin(0)[2:] * (end_bit - start_bit + 1)
        mask += start_bit * '1'
        mask = mask.rjust(BIT_LENGTH_64_BITS, '1')

        self_value = bin(int(Cfg_Validations.normalize_value(self.value), 16))
        self_value = self_value[2:]
        self_value = self_value.rjust(BIT_LENGTH_64_BITS, '0')

        # Clear the bits to be set
        self_value = int(self_value, 2) & int(mask, 2) | int(value, 2)

        # Set the value back to self
        self.value = hex(self_value)

    def reset(self):
        """Resets the fuse by removing all the fields.
        """
        self.field = []

    def validate(self):
        """See :meth:`Cfg_Base.validate`
        """
        self.validate_list(self.field)

    def sort(self):
        """See :meth:`Cfg_Base.sort`
        """
        self.sort_list(self.field)

    def get_key_for_sort(self):
        """See :meth:`Cfg_Base.get_key_for_sort`
        """
        return self.address

    def get_id_for_str(self):
        """See :meth:`Cfg_Base.get_id_for_str`
        """
        return self.address

    def add_field(self, field):
        """Adds a field to the :meth:`field`

        :param :class:`Cfg_Fuse` field: Field to be added to the list
        """
        assert isinstance(field, Cfg_Field)
        field.set_parent_attributes(self.get_value_by_bits,
                                    self.set_value_by_bits)
        self.field.append(field)
        self.sort()

    def get_field(self, _id):
        """Return the field corresponding to the given _id.

        :param _id: ID of the field to be returned.
        :return: Field
        :rtype: :class:`Cfg_Field`
        :raises: RuntimeError if the _id is not in the list of fields.
        """
        for field in self.field:
            if field.id == _id:
                return field
        else:
            raise RuntimeError('Field not found for id: ' + _id)

    def get_field_by_start_bit(self, start_bit):
        """Returns the field from the field list whose start bit matches the
        given start bit. If no such field is found, None is returned.

        :param int start_bit: start bit to search for
        :returns: Field matching the start_bit
        :rtype: :class:`Cfg_Field`, None
        """
        for field in self.field:
            if field.start_bit == start_bit:
                return field
        return None

    def __add__(self, other):
        """Add data from another fuse to the data in self.

        :param :class:`Cfg_Fuse` other: The second fuse to add to self.
        :returns: An object with the merged fuse data.
        :rtype: :class:`Cfg_Fuse`
        """
        assert isinstance(other, Cfg_Fuse)

        # The return value is a new instance of Cfg_Fuse
        retval = Cfg_Fuse()

        # Ensure that the n matches
        if other.n != self.n:
            raise RuntimeError('N for the fuse does not match: ' + '\n'
                               '    ' + 'N 1: ' + self.n + '\n'
                               '    ' + 'N 2: ' + other.n)
        else:
            retval.n = self.n

        # Ensure that the address matches
        if other.address != self.address:
            raise RuntimeError('Address for the fuse does not match: ' + '\n'
                               '    ' + 'Address 1: ' + self.address + '\n'
                               '    ' + 'Address 2: ' + other.address)
        else:
            retval.address = self.address

        # Take the operation
        retval.operation = other.operation

        # Take the ignore operation
        retval.ignore = other.ignore

        # Put all the fields together
        self._create_breakdown(other, retval)

        return retval

    def _create_breakdown(self, other, retval):
        """Adds a list of fields which is a combination of the fields from
        self and from other to the retval fuse.

        :param Cfg_Fuse other: The second fuse to use for getting the breakdown.
        :param Cfg_Fuse retval: The fuse to which the fields should be added.
        """
        # Last start bit
        last_start_bit = 0
        chosen_field = None

        # Handle to the fields
        self_field = None
        self_prev = False
        self_cur = False
        self_dump = False

        other_field = None
        other_prev = False
        other_cur = False
        other_dump = False

        get_status_cur = lambda x, y: (x is not None and
                                       x.start_bit <= y and
                                       x.end_bit >= y)

        # Loop from 0-63 which is the length of the fuse value
        for cur_bit in range(FUSE_VALUE_MAX_BITS + 1):

            # Get handle to self and other field
            if self_field is None:
                self_field = self.get_field_by_start_bit(cur_bit)
            if other_field is None:
                other_field = other.get_field_by_start_bit(cur_bit)

            # Update the cur for both fields
            self_cur = get_status_cur(self_field, cur_bit)
            other_cur = get_status_cur(other_field, cur_bit)

            # If this is the first row set prev, same as cur
            if cur_bit == 0:
                self_prev = self_cur
                other_prev = other_cur

            # Check if self should be dumped
            if ((self_prev and (not self_cur) and other_cur) or
                    (self_prev and (not self_cur) and (not other_prev)) or
                    (self_prev and (not other_prev) and other_cur)):
                self_dump = True
                chosen_field = self_field

            # Check if other should be dumped
            if ((other_prev and (not other_cur) and self_cur) or
                    (other_prev and (not other_cur) and (not self_prev)) or
                    (other_prev and (not self_prev) and self_cur)):
                other_dump = True
                chosen_field = other_field

            # Check special case
            if (other_prev and (not other_cur) and self_prev and (not self_cur)):
                if self_dump or other_dump:
                    raise RuntimeError('Special case but already dumping somethin. This should not happen.')

                if self_field.start_bit > other_field.start_bit:
                    self_dump = True
                    chosen_field = self_field
                else:
                    other_dump = True
                    chosen_field = other_field

            # Dump anything as needed
            if self_dump and other_dump:
                # Ensure both dont need to be dumped
                raise RuntimeError('Both fileds breakdown are true. This should not have happened.')

            # Dump the data
            if self_dump or other_dump:
                # Create new field
                end_bit = cur_bit - 1
                int_field = Cfg_Field()
                int_field.id = chosen_field.id
                int_field.description = chosen_field.description
                int_field.owner = (other_field if get_status_cur(other_field, end_bit) else chosen_field).owner

                # Compile the correct bit pattern
                if end_bit == last_start_bit:
                    int_field.bits = str(end_bit)
                else:
                    int_field.bits = str(end_bit) + int_field.BIT_SEPARATOR + str(last_start_bit)

                # Update the name of the field
                if (chosen_field.start_bit != last_start_bit or
                        chosen_field.end_bit != end_bit):
                    int_field.id += '-BITS_' + int_field.bits.replace(':', '_')

                # Add the field before setting the value
                retval.add_field(int_field)
                int_field.value = (other_field if get_status_cur(other_field, end_bit) else chosen_field).get_value_by_bits(last_start_bit, end_bit)

            # Update the last start bit
            if (((not self_prev) and self_cur) or
                    ((not other_prev) and other_cur) or
                    (self_prev and (not self_cur) and other_cur) or
                    (other_prev and (not other_cur) and self_cur)):
                last_start_bit = cur_bit

            if self_dump or other_dump:
                # Reset the fields if something was dumped
                if self_field and self_field.end_bit == end_bit:
                    self_field = self.get_field_by_start_bit(cur_bit)
                    self_prev = self_cur
                    self_cur = get_status_cur(self_field, cur_bit)
                if other_field and other_field.end_bit == end_bit:
                    other_field = other.get_field_by_start_bit(cur_bit)
                    other_prev = other_cur
                    other_cur = get_status_cur(other_field, cur_bit)

                chosen_field = None
                self_dump = False
                other_dump = False

            # Update the last start bit again based on the new input
            if (((not self_prev) and self_cur) or
                    ((not other_prev) and other_cur) or
                    (self_prev and (not self_cur) and other_cur) or
                    (other_prev and (not other_cur) and self_cur)):
                last_start_bit = cur_bit

            # Update the prevs
            self_prev = self_cur
            other_prev = other_cur


class Cfg_Field(Cfg_Base):
    """Container for all the field specific data.
    """

    BIT_SEPARATOR = ':'

    def __init__(self):
        """Initializations and any checks.
        """
        # Internal properties
        self._id = ''
        self._description = ''
        self._bits = ''
        self._owner = ''
        self._value = '0x0'
        self._value_changed = False
        self.get_value_cb = None
        self.set_value_cb = None

    def set_parent_attributes(self, get_value_cb, set_value_cb):
        """Sets the attributes that come from the parent

        :param get_value_cb: A call back to get the value of a certain field range.
        :param set_value_cb: A call back to set the value of a certain field range.
        """
        self.get_value_cb = get_value_cb
        self.set_value_cb = set_value_cb
        if self._value_changed:
            self.value = self._value

    @property
    def id(self):
        """(str) ID of the field.
        """
        return self._id

    @id.setter
    def id(self, value):
        assert isinstance(value, str)
        if not value:
            raise RuntimeError('ID must be defined')
        self._id = value

    @property
    def description(self):
        """(str) Description of the field.
        """
        return self._description

    @description.setter
    def description(self, value):
        assert isinstance(value, (str, type(None)))
        self._description = value

    @property
    def bits(self):
        """(str) Bits of the Fuse.
        """
        return self._bits

    @bits.setter
    def bits(self, value):
        assert isinstance(value, str)

        # Check the format of the bits
        if value.find(self.BIT_SEPARATOR) == -1:
            try: int(value)
            except Exception:
                raise RuntimeError('Bits format is invalid: ' + value + '. Must be a number.')
        else:
            bits_split = value.split(self.BIT_SEPARATOR)
            if (len(bits_split) != 2):
                raise RuntimeError('Bits format is invalid: ' + value + '. There should only be one ":".')
            else:
                try: start_bit = int(bits_split[1])
                except Exception:
                    raise RuntimeError('Bits format is invalid: ' + value + '. "' + str(bits_split[0]) + '" must be a number.')
                try: end_bit = int(bits_split[0])
                except Exception:
                    raise RuntimeError('Bits format is invalid: ' + value + '. "' + str(bits_split[1]) + '" must be a number.')
                if (start_bit < 0 or start_bit >= FUSE_VALUE_MAX_BITS or
                    end_bit < 0 or end_bit >= FUSE_VALUE_MAX_BITS or
                        start_bit > end_bit):
                    raise RuntimeError('Invalid start bit: ' + str(start_bit) + ' and end bit: ' + str(end_bit))

        self._bits = value

    @property
    def owner(self):
        """(str) Owner of the fuse
        """
        return self._owner

    @owner.setter
    def owner(self, value):
        assert isinstance(value, (str, type(None)))
        if value is not None:
            if value not in defines.SUPPORTED_OWNERS:
                raise RuntimeError('Unknown owner: ' + value + '\n'
                                   '    ' + 'Supported owners are: ' + str(defines.SUPPORTED_OWNERS))
        self._owner = value

    @property
    def value(self):
        """(str) Value of the Fuse.
        """
        # Return the temporary value till the get cb is set.
        if self.get_value_cb is None:
            return self._value

        # Get the value from the fuse
        return self.get_value_cb(self.start_bit, self.end_bit)

    @value.setter
    def value(self, value):
        assert isinstance(value, str)

        # Set the temporary value till the set cb is set.
        if self.set_value_cb is None:
            self._value = value
            self._value_changed = True
            return

        # Set the value of the fuse from here
        bit_length = self.end_bit - self.start_bit + 1
        try:
            Cfg_Validations.check_value(value, bit_length)
        except Exception as e:
            e.args += (' Error while setting value to: ' + value,)
            raise
        self.set_value_cb(Cfg_Validations.normalize_value(value, bit_length),
                          self.start_bit, self.end_bit)

    def get_attr_list(self):
        """See :meth:`Cfg_Base.get_attr_list`
        """
        return ['id',
                'description',
                'bits',
                'owner',
                'value',]

    def validate(self):
        """See :meth:`Cfg_Base.validate`
        """
        if self.get_value_cb is None:
            raise RuntimeError('get_value_cb is None')
        if self.set_value_cb is None:
            raise RuntimeError('set_value_cb is None')

    def sort(self):
        """See :meth:`Cfg_Base.sort`
        """
        pass

    def get_key_for_sort(self):
        """See :meth:`Cfg_Base.get_key_for_sort`
        """
        return self.start_bit

    def get_id_for_str(self):
        """See :meth:`Cfg_Base.get_id_for_str`
        """
        return self.id

    @property
    def start_bit(self):
        """(int) The start bit of this field in the fuse.
        """
        if self.bits.find(self.BIT_SEPARATOR) == -1:
            return int(self.bits)
        else:
            return int(self.bits.split(self.BIT_SEPARATOR)[1])

    @property
    def end_bit(self):
        """(int) The end bit of this field in the fuse.
        """
        if self.bits.find(self.BIT_SEPARATOR) == -1:
            return int(self.bits)
        else:
            return int(self.bits.split(self.BIT_SEPARATOR)[0])

    def get_value_by_bits(self, start_bit, end_bit):
        """Returns the value for the bits that are queried.

        :param int start_bit: The starting bit of the value
        :param int end_bit: The ending bit of the value
        :returns: Value in the [end_bit : start_bit] range
        :rtype: str
        """
        if self.start_bit > start_bit or self.end_bit < end_bit:
            raise RuntimeError('Value queried for the bits from field are out of range.' + '\n'
                               '    ' + 'Queried Start Bit: ' + str(start_bit) + '\n'
                               '    ' + 'Queried End Bit: ' + str(end_bit) + '\n'
                               '    ' + 'Field being queried:' + '\n'
                               '' + repr(self))

        value = bin(int(Cfg_Validations.normalize_value(self.value), 16))
        value = value[2:].rjust(self.end_bit - self.start_bit + 1, '0')
        value = value[self.end_bit - end_bit : len(value) - (start_bit - self.start_bit)]
        return hex(int(value, 2))


def filter_by_owner(data_model, owner):
    """Returns the data model filtered by the given owner.

    :param str owner: The owner to filter the data model with.
    :returns obj: datamodel
    """
    # Check that the owner is correct
    if owner not in defines.SUPPORTED_OWNERS:
        raise RuntimeError('Unknown owner: ' + owner + '\n'
                           '    ' + 'Supported owners are: ' + str(defines.SUPPORTED_OWNERS))

    # Create the new data model
    retval = Cfg_Fuseblower()

    # Go over all the modules
    for module in data_model.module:
        new_module = copy.deepcopy(module)
        new_module.reset()

        # Go over all the fuse regions
        for fuse_region in module.fuse_region:
            new_fuse_region = copy.deepcopy(fuse_region)
            new_fuse_region.reset()

            # Go over all the fuses
            for fuse in fuse_region.fuse:
                new_fuse = copy.deepcopy(fuse)
                new_fuse.reset()

                # Go over all the fields
                for field in fuse.field:
                    # Add the ones with a matching owner
                    if field.owner == owner:
                        new_field = copy.deepcopy(field)
                        new_fuse.add_field(new_field)

                # If there was atleast one field under the given owner, add this
                # fuse to the fuse region
                if new_fuse.field:
                    new_fuse_region.add_fuse(new_fuse)

            # If there was atleast one fuse under the given owner, add this
            # fuse_region to the module
            if new_fuse_region.fuse:
                new_module.add_fuse_region(new_fuse_region)

        # If there was atleast one fuse region under the given owner, add this
        # module to the data model
        if new_module.fuse_region:
            retval.add_module(new_module)

    return retval
