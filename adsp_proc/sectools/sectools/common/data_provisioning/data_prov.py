# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

"""
Created on May 29, 2014

@author: forrestm

Code for everything DataProvisioning related.

This module is meant to ease file management for its clients (everything under sectools is a potential client).
The general concept is a three tier directory system underneath some base path (specified in the config.xml).
The first tier specifies the Security Policy (Encryption, Signing, Validation, etc). The second specifies the
Security Policy Type (SSD, unified, CASS). The third level specifies the chipset name.

When you need your data, just do the following::

    data_prov = DataProvisioner()
    data = data_prov.query('Encryption','Unified','8994')
    data[0].files['l1.key']
    data[0].files['l2.key']
    data[0].files['l3.key']

Meta data files are a very easy feature to use. Their output can be found in the 'config' attribute of
DataProvObject (which is what the query method returns). The structure is very similar to what
generateDS output. The structure is as follows:

<METACONFIG>
    <Tag1>value</Tag1>
    <Tag2>
        <Tag3>value3</Tag3>
    </Tag2>
    ...
</METACONFIG>

There may only be one root tag and it MUST be <METACONFIG>.

"""

import os
import pickle
import difflib

import xmltodict
from sectools.common.utils import c_path
from sectools.common.utils.c_logging import logger


# database handling defines
FILE_INTERFACE_STR = 'file_interface'
DATABASE_STR = 'database'
SECURITY_POLICY_STR = 'security_policy'
SECURITY_POLICY_TYPE_STR = 'security_policy_type'
DATA_PROV_ID_STR = 'data_prov_id'
BINARY_FILE_STR = 'binary_file'
BINARY_FILE_LEVEL = 4


class DataProvisioner(object):
    """ DataProvisioning for binary files (certs,keys,etc) needed by different security policies
    """

    # used for sharing state for Borg (singleton) pattern
    __shared_state = {}

    @classmethod
    def reset(cls):
        """
        Deletes the current instance in order to force a newly constructed instance to be returned the next time the constructor is called.
        """
        cls.__shared_state = {}

    def __init__(self, data_prov_base=None):
        """ DataProvisioner constructor. DataProvisioner is a singleton. Takes parameter only when
        initializing the DataProvisioning database

        :param data_prov_base: path to the base folder system where all data provisioning files are
        :type data_prov_base: str
        """

        self.__dict__ = self.__shared_state
        if data_prov_base is not None:
            # TODO: handle other sources (backend) here, decide how we should merge/handle the resulting databases
            self.database = FileInterfaceHandler.get_database(c_path.join(data_prov_base, ''))
            self.verify()

    def _validate_query(self, security_policy, security_policy_type, data_prov_id):
        """ Validates the query to make sure the given params are in the database. Returns True
        if the query is valid, an error string otherwise. Should only be called internally.
        """

        if security_policy is None:
            return True
        current_obj = self.database
        if security_policy not in current_obj.children.keys():
            return self._fuzzy_match(security_policy)

        if security_policy_type is None:
            return True
        current_obj = current_obj.children[security_policy]
        if security_policy_type not in current_obj.children.keys():
            return self._fuzzy_match(security_policy, security_policy_type)

        if data_prov_id is None:
            return True
        current_obj = current_obj.children[security_policy_type]
        if data_prov_id not in current_obj.children.keys():
            return self._fuzzy_match(security_policy, security_policy_type, data_prov_id)

        return True

    def _fuzzy_match(self, security_policy, security_policy_type=None, data_prov_id=None):
        """ Suggests correct query values close to provided values if invalid query values are provided. Should only be called by the validator.
        """

        close_matches = None
        level = ''
        search_term = ''

        if data_prov_id is not None:
            possible_matches = self.database.children[security_policy].children[security_policy_type].children.keys()
            close_matches = difflib.get_close_matches(data_prov_id, possible_matches)
            level = DATA_PROV_ID_STR
            search_term = data_prov_id
        elif security_policy_type is not None:
            possible_matches = self.database.children[security_policy].children.keys()
            close_matches = difflib.get_close_matches(security_policy_type, possible_matches)
            level = SECURITY_POLICY_TYPE_STR
            search_term = security_policy_type
        elif security_policy is not None:
            possible_matches = self.database.children.keys()
            close_matches = difflib.get_close_matches(security_policy, possible_matches)
            level = SECURITY_POLICY_STR
            search_term = security_policy
        else:
            raise RuntimeError("fuzzy_match must be called with at least 1 actual parameter")

        ret_str = "DataProvisioner: query term {0} for {1} is not in the DataProvisioner database. ".format(search_term, level)
        if len(close_matches) > 0:
            ret_str += "Did you mean any of the following? {0} ".format(close_matches)

        return ret_str

    def query(self, security_policy=None, security_policy_type=None, data_prov_id=None):
        """ Searches and returns the corresponding DataProvObject for the given search parameters.
        supports wildcard parameters by passing None as the wildcard parameter. Does not allow for
        more specific parameters to be specified if any less specific parameters are wildcard. For
        example::
            query("encryption",None,None)
        is valid::

            query("encryption",None,"8994")
        is not.

        Returns a list of corresponding DataProvObjects. The resulting DataProvObjects will
        vary in at least one of the four fields (security_policy, security_policy_type, data_prov_id, source).
        For 3 param queries the only field that will vary is source. Until multiple sources is implemented,
        3 param queries will always return a list of size one

        :param security_policy: the security policy to search for
        :type security_policy: str / None (for wildcards)
        :param security_policy_type: the security policy type to search for
        :type security_policy_type: str / None (for wildcards)
        :param data_prov_id: the data prov id to search for
        :type data_prov_id: str / None (for wildcards)
        :rtype: list(DataProvObject)
        :raises: KeyError,RuntimeError
        """

        ret = self._validate_query(security_policy, security_policy_type, data_prov_id)
        if ret is not True:
            raise KeyError(ret)

        result_list = []

        if security_policy is None:
            for param in (security_policy_type, data_prov_id):
                if param is not None:
                    raise RuntimeError("DataProvisioner: {0} is not a wildcard, whereas security_policy is".format(param))
            for lvl1_name, lvl1_obj in self.database.children.items():
                for lvl2_name, lvl2_obj in lvl1_obj.children.items():
                    for lvl3_name, lvl3_obj in lvl2_obj.children.items():
                        result_list.append(self.update_data_prov(lvl3_obj, lvl1_name, lvl2_name))

        elif security_policy_type is None:
            if data_prov_id is not None:
                raise RuntimeError("DataProvisioner: data_prov_id is not a wildcard, whereas security_policy is")
            lvl1_obj = self.database.children[security_policy]
            for lvl2_name, lvl2_obj in lvl1_obj.children.items():
                for lvl3_name, lvl3_obj in lvl2_obj.children.items():
                    result_list.append(self.update_data_prov(lvl3_obj, security_policy, lvl2_name))

        elif data_prov_id is None:
            lvl2_obj = self.database.children[security_policy].children[security_policy_type]
            for lvl3_name, lvl3_obj in lvl2_obj.children.items():
                result_list.append(self.update_data_prov(lvl3_obj, security_policy, security_policy_type))

        else:
            lvl3_obj = self.database.children[security_policy].children[security_policy_type].children[data_prov_id]
            result_list.append(self.update_data_prov(lvl3_obj, security_policy, security_policy_type))

        # TODO: in the future this is where we would get data from a different source (backend/server/whatever) and tack
        # the entries on to result_list

        return result_list

    def verify(self):
        """ Verifies self.database is correct. Checks for type/bin file mismatch, bin files not
           in level 3, lack of bin files, too many bin files, etc. Some things are implicitly verified
           by the file handler, like existence of files/folders and correct paths, since it
           can't create the database with bad paths

           :raises: RuntimeError
           """

        if len(self.database.files) > 0:
            raise RuntimeError("DataProvisioner:  binary files {0} exist within security_policy level of Data Provisioning file structure".format(self.database.files))
        for lvl1_name, lvl1_obj in self.database.children.items():

            if len(lvl1_obj.files) > 0 :
                raise RuntimeError("DataProvisioner:  binary files {0} exists within security_policy_type level of Data Provisioning file structure".format(lvl1_obj.files))

            for lvl2_name, lvl2_obj in lvl1_obj.children.items():

                if len(lvl2_obj.files) > 0 :
                    raise RuntimeError("DataProvisioner:  binary files {0} exists within data_prov_id level of Data Provisioning file structure".format(lvl2_obj.files))

                for lvl3_name, lvl3_obj in lvl2_obj.children.items():

#                     if len(lvl3_obj.files) == 0:
#                         raise RuntimeError("DataProvisioner:  there are no binary files in data_prov_id of Data Provisioning file structure. {0}".format(lvl3_obj.path))

                    for file_name in lvl3_obj.files:
                        if c_path.validate_dir(file_name):
                            raise RuntimeError("DataProvisioner:  Directories are not allowed within the data_prov_id directory of the Data Provisioning file structure")

        logger.debug("DataProvisioner database verification passed")
        return True

    def update_data_prov(self, data_prov, security_policy, security_policy_type):
        data_prov.security_policy = security_policy
        data_prov.security_policy_type = security_policy_type
        return data_prov

    # Database serialization is useful for debugging and testing purposes
    def _pickle(self, dump_path):
        dump = open(dump_path, "wb+")
        pickle.dump(self.database, dump)

    def _unpickle(self, dump_path):
        dump = open(dump_path, "rb")
        return pickle.load(dump)

    def _pickles(self):
        return pickle.dumps(self.database)

    def _unpickles(self, pickle_data):
        return pickle.loads(pickle_data)

    def _unpickle_and_update_database(self, dump_path):
        self.database = self._unpickle(dump_path)
        return self.database

    def _unpickles_and_update_database(self, pickle_data):
        self.database = self._unpickles(pickle_data)
        return self.database


class DataProvObject(object):
    """ A data provisioning object that corresponds to a certain folder within the data provisioning directory structure
    """

    def __init__(self, data_prov_id, path):
        """ DataProvObject constructor
        :param data_prov_id: the ID of this DataProvObject. Normally the last folder in the path
        :type data_prov_id: str
        :param path: absolute path to the directory this data prov object corresponds to
        :type path: str
        """
        self.security_policy = None
        self.security_policy_type = None
        self.id = data_prov_id
        self.path = path
        self.files = {}
        self.children = {}
        self.config = None
        # TODO: source should not be hardcoded if in the future support is added for other dataprov providers
        self.source = FILE_INTERFACE_STR

    def equals_ignoring_children(self, data_prov_2):
        """ compares two DataProvObjects ignores children.
        True if security_policy, security_policy_type, id, path, files, config, and source are equal.
        False otherwise.

        :param data_prov_2: the DataProvObject to compare self against
        :type data_prov_2: DataProvObject
        :rtype: bool
        """

        return (self.security_policy == data_prov_2.security_policy) \
               and (self.security_policy_type == data_prov_2.security_policy_type) \
               and (self.id == data_prov_2.id) \
               and (self.path == data_prov_2.path) \
               and (self.files == data_prov_2.files) \
               and (self.config == data_prov_2.config) \
               and (self.source == data_prov_2.source)

    def equals(self, data_prov_2):
        """ compares two DataProvObjects and compares all corresponding children.
        True if the objects and all of their recursive children are equal.
        False otherwise.

        :param data_prov_2: the DataProvObject to compare self against
        :type data_prov_2: DataProvObject
        :rtype: bool
        """

        if not self.equals_ignoring_children(data_prov_2):
            return False
        if len(self.children) != len(data_prov_2.children):
            return False
        for key in self.children.keys():
            if key not in data_prov_2.children.keys():
                return False
            if not self.children[key].equals(data_prov_2.children[key]):
                return False
        return True

    def __str__(self):

        divider = "***********************************"

        def to_string(data_prov, level):
            """ Returns a formatted version of the DataProvObject and it's children.
            :param level: the depth of the current object being processed. This is used for indenting the output of children DataProvObjects
            :type level: int

            :rtype: str
            """

            ret_val = ""
            if level == 0:
                ret_val += divider
            else:
                ret_val += "\n" + ("\t" * level) + divider
            if data_prov.security_policy is not None:
                "\n" + ("\t" * level) + "security_policy: {0} ".format(data_prov.security_policy)
            if data_prov.security_policy_type is not None:
                ret_val += "\n" + ("\t" * level) + "security_policy_type: {0} ".format(data_prov.security_policy_type)
            ret_val += "\n" + ("\t" * level) + "id: {0} ".format(data_prov.id)
            ret_val += "\n" + ("\t" * level) + "path: {0} ".format(data_prov.path)
            if data_prov.files is not None and len(data_prov.files) > 0:
                ret_val += "\n" + ("\t" * level) + "files: {0} ".format(data_prov.files)
            if data_prov.config is not None:
                ret_val += "\n" + ("\t" * level) + "config: " + config_to_string(data_prov.config, data_prov.config.METACONFIG)
            ret_val += "\n" + ("\t" * level) + "source: {0} ".format(data_prov.source)
            if self.children is not None and len(data_prov.children) > 0:
                ret_val += "\n" + ("\t" * level) + "children:"
                for child_data_prov_obj in data_prov.children.values():
                    ret_val += to_string(child_data_prov_obj, level + 1)

            return ret_val

        return to_string(self, 0) + "\n" + divider


class FileInterfaceHandler(object):
    """ Discovers all data provisioning information and turns it into a nested DataProvObject for DataProvisioner to use
    """

    @classmethod
    def get_database(cls, data_prov_base):
        """ Handles database creation and data discovery. Should only be called by the DataProvisioning constructor.
        Creates the database recursively, to facilitate for expansion later on if we add levels.

        :param data_prov_base: path to the base folder for the data provisioning directory structure
        :type data_prov_base: str
        :rtype: DataProvObject
        :raises: RuntimeError
        """

        database = DataProvObject(DATABASE_STR, c_path.join(data_prov_base, ''))
        cls._populate_database(database, 1, data_prov_base)
        return database

    @classmethod
    def _populate_database(cls, data_prov_object, level, path):
        """ Recursively creates and fills in the DataProvObject including all of it's children

        :param data_prov_object: the highest level of the DataProvObject hierarchy
        :type data_prov_object: DataProvObject
        :param level: the level depth of the current data prov hierarchy. Should be set to 1 when initially calling _populate_database
        :type level: int
        :param path: the path in the file system corresponding to the current data prov level through which to scan and populate the DataProvObject
        :type path: str
        :rtype: None
        :raises: RuntimeError
        """

        for item in os.listdir(path):
            if item.endswith(".xml") and level == BINARY_FILE_LEVEL:
                data_prov_object.config = cls._get_config(c_path.join(path, item))
            # allow for unix style hidden folders to exist
            elif item.startswith("."):
                continue
            elif c_path.validate_dir(c_path.join(path, item)) and level == BINARY_FILE_LEVEL:
                raise RuntimeError("DataProvisioner:  found directory {0} in binary_file level of DataProvisioner. \
                Directories are not allowed in the binary_file level".format(item))
            elif not c_path.validate_dir(c_path.join(path, item)) and level != BINARY_FILE_LEVEL:
                raise RuntimeError("DataProvisioner:  found binary file {0} in level {1} of DataProvisioner. \
                Binary files are only allowed at the binary_file level".format(item, cls._level_num_to_name(level)))
            elif level == 4:
                try:
                    with open(c_path.join(path, item), "r") as data_file:
                        data_prov_object.files[item] = data_file.read()
                except:
                    raise RuntimeError("DataProvisioner:  Can't open {0}".format(c_path.join(path, item)))

            # recursively populate database children
            if level < BINARY_FILE_LEVEL:
                child_data_prov_object = DataProvObject(item, c_path.join(path, item))
                data_prov_object.children[item] = child_data_prov_object
                cls._populate_database(child_data_prov_object, level + 1, c_path.join(path, item))

    @classmethod
    def _get_config(cls, path_to_meta_config_file):
        """ Opens the meta config file, parses it using xml to dict, then converts the dict to a python object.
        """

        conf_file = open(path_to_meta_config_file, 'r')
        out_dict = xmltodict.parse(conf_file.read())
        config = DictToObject(out_dict)
        return config

    @classmethod
    def _level_num_to_name(cls, level):
        """ Given an integer in the range 0 to 4, returns the string name of the corresponding data prov level
        """

        level_strings = [DATABASE_STR, SECURITY_POLICY_STR, SECURITY_POLICY_TYPE_STR, DATA_PROV_ID_STR, BINARY_FILE_STR]
        if level >= len(level_strings) or level < 0:
            raise RuntimeError("DataProvisioner: converting a bad level number")
        return level_strings[level]


class DictToObject(object):
    """ Converts a dictionary to a python object. Should only ever be called by the _get_config handler
    """

    def wrap_recursive_data(self, value):
        if isinstance(value, (set, tuple, list)):
            return type(value)([self.wrap_recursive_data(val) for val in value])
        else:
            if isinstance(value, dict):
                return DictToObject(value)
            else:
                return value

    def __init__(self, data):
        for key, value in data.items():
            setattr(self, key, self.wrap_recursive_data(value))
            obj = getattr(self, key)
            if isinstance(obj, DictToObject):
                self.value = obj

    def __eq__(self, other):
        return self.__dict__ == other.__dict__

    def __ne__(self, other):
        return not self.__eq__(other)


def config_to_string(parent_object, object, tab=''):
    """ Used to recursively traverse a python data structure created with DictToObject and pretty print it.
    Should only ever be called when debugging meta config objects."""
    ret = ""
    for var in vars(object):
        obj = getattr(object, var)
        ret += "{0}{1}: {2} ".format(tab, var, str(obj))
        if isinstance(obj, DictToObject):
            ret += "\n" + config_to_string(object, obj, tab)
        elif isinstance(obj, list):
            for l in obj:
                ret += tab
                config_to_string(object, l, tab + '\t')
                ret += tab
    return ret
