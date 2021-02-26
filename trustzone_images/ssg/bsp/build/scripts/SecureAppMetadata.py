#!/usr/bin/env python
# Copyright (c) 2015-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.

"""
This module contains python functions used by the SCons build system.

Currently, there is only one exported function, generate_metadata.
generate_metadata is used to implement the AddSecureAppMetadata SCons function
available in SConscript files under the qsapps directory.  generate_metadata adds
metadata about the app to the app binary.

This module includes a self test based on the python UnitTest framework.
To run the self test, execute "python SecureAppMetadata.py" from the command
line.  The self test needs to be run in a TZ build tree.  (The code searches the
build tree to get numbers for ID strings associated with privilege and service names.)
"""

import copy
import operator
import os
import re
import sys
import types
import unittest
import ast

# The default set of privilege IDs.  The IDs will be mapped to values
# once the values are known.  Notable (but not complete) excludes are:
# HwFuse, I2C, SPI, OEMBuf, IntMask, PVC, TransNSAddr and ResetTimer.
_kDefaultPrivilegeIds = {
    'AppMessage',
    'CipherAES128',
    'CipherAES256',
    'CipherTripleDES',
    'Crypto',
    'DataCache',
    'DeviceId',
    'HashNone',
    'HashSHA1',
    'HashSHA256',
    'HashSHA384',
    'HmacSHA1',
    'HmacSHA256',
    'Listener',
    'NSMem',
    'PRNGSource',
    'Sync',
    'SecureChannel',
    'SharedBuffer',
    'DebugPolicySource',
    'System',
    'GPServices',
    'HlosRegionFinder',
    'Uptime',
}

# The directories, relative to the build root, that contains the .idl
# files that define class ids.
_kClassIdDirs = ['apps/securemsm/trustzone/',
                 'ssg/securemsm/trustzone/',
                ]

# Marker string for applications that can be loaded in unprotected memory.
_kMarker1 = '\0p+s}-#Pxh7Z!<+`'

# Marker string for applications that can be loaded in protected memory.
_kMarker2 = '\0obL$`P*8L)~s3L}'

def _encode_uuid(str):
    """Encode a canonical UUID string as a metadata UUID value"""

    m = re.match(r'^([0-9a-fA-F]{8})-([0-9a-fA-F]{4})-([0-9a-fA-F]{4})-([0-9a-fA-F]{4})-([0-9a-fA-F]{12})$', str)

    if m is None:
        raise Exception('UUID syntax invalid; expected "8-4-4-4-12" format:\n'
                        + ' XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX')

    return (_hex_byte_swap(m.group(1)) +
            _hex_byte_swap(m.group(2)) +
            _hex_byte_swap(m.group(3)) +
            m.group(4) +
            m.group(5))

def _encode_id_set(iterable):
    """Encode numeric IDs as a metadata ID set value"""
    s = ''
    # The ID represented by the first bit in the next mask digit (if any)
    mask_pos = 0
    # Current value for the next mask digit to be emitted.  When non-zero,
    # we can assume that we are in mask mode (following a ':').
    mask_digit = 0

    for id in sorted(set(iterable)):

        # Each ID may be emitted as a hex number or as a mask bit.  We
        # estimate the "cost" of each in terms of bytes emitted.  The "cost"
        # of using a mask is not known entirely at this point, since
        # subsequent IDs might benefit from mask encoding, so we apply a
        # small bias toward masks.

        id_text = _hex_str(id)
        num_cost = len(id_text) + (1 if mask_digit else -1)
        mask_cost = (id - mask_pos) // 4

        if s == '' or mask_cost > num_cost:
            # Output id as a hex number.
            if mask_digit:
                s += _hex_str(mask_digit)
            if s != '':
                s += ','
            s += id_text
            mask_pos = id + 1
            mask_digit = 0              # not in a mask

        else:
            # Add id as a mask bit.
            if mask_digit == 0:
               s += ':'
            while id >= mask_pos + 4:
               s += _hex_str(mask_digit)
               mask_digit = 0
               mask_pos += 4
            mask_digit |= 1 << (id - mask_pos)
            # now:  mask_digit > 0   (in a mask)

    if mask_digit:
        s += _hex_str(mask_digit)

    return s


def _hex_byte_swap(str):
    """Reverse the bytes (pairs of hex digits) in a hex-encoded string"""
    return ''.join(re.findall('..', str)[::-1])

def _hex_str(val):
    return '%x' % (val)
      
def _isClassDefFile(path):
    # Looking only for file names that match C*.idl
    name = os.path.basename(path)
    if re.match('^C.*\.idl', name) and os.path.isfile(path):
      return True
    return False

def _extractIds(path):
    # Looking for lines that match something like this:
    #
    #   const uint32 CSharedBuffer_UID = 23;

    idmap = {}
    re0 = re.compile(r'^\s*const\s+uint32\s+C(\w+)_UID\s+=\s+(0x[0-9a-fA-F]+|[0-9]+);\s*')

    with open(path) as f:
        for line in f:
            m = re0.match(line)
            if m:
                n= re0.search(line)
                # TODO: error checking to see if there are duplicate id values?
                if n:
                    id = m.group(1).lower()
                    value = int(n.group(2), 0)
                    idmap[id] = value
    return idmap
    
class IDLScanner:
 
  def __init__(self):
    pass
    
  def create_class_id_map(self):
      # Get all of the class IDs.  Create the self._classIdMap.
      _classIdMap = {}
      for rel in _kClassIdDirs:
          root = os.path.abspath(os.path.join(os.path.dirname(__file__),'..','..','..','..',rel))
          if not os.path.isdir(root):
            raise Exception('Class ID directory %s not found.  Are you in the build tree?' % ( rel ))
          for dirName, subdirList, fileList in os.walk(root):
              for fname in fileList:
                  # Check if this file is likely to hold IDL ids.
                  path = os.path.join(dirName, fname)
                  if not _isClassDefFile(path):
                      continue
                  idmap = _extractIds(path)
                  # Add the ids we just found to the global id map.
                  # Do some error checking along the way.
                  for key in idmap:
                      for key2, value in _classIdMap.items():
                          # The key is duplicate.  This is OK as long as the value is the same in both cases.
                          if key == key2:
                              if idmap[key] != value:
                                  raise Exception('Duplicate values for class id %s (%d and %d).' % ( key, value, idmap[key] ))
                              else:
                                  continue
                          #  The value is in both maps with a different key.  This is an error.
                          if idmap[key] == value:
                              raise Exception('Duplicate definitions for value %d (%s and %s).' % ( value, key, key2 ))
                      # No errors?  Add it.
                      _classIdMap[key] = idmap[key]
      return _classIdMap

    
class SecureAppMetadataClass:

  def __init__(self, id_map):
    # Metadata field handlers.
    #    Each field name includes:
    #        - single letter abbreviation
    #        - processing function
    #        - default value if not specified
    self._metadataHandlers = {
          'privileges':    ('p', self._gen_class_set_string, set(['default'])),
          'services':      ('s', self._gen_class_set_string, None),
          'appName':       ('n', lambda val: self._check_str(r'.{1,31}', val), None),
          'UUID':          ('u', _encode_uuid, None),
          'objectLimit':   ('o', lambda val: self._get_non_negative(val), None),
          'memoryType':    ('m', lambda val: self._get_memory_type(val), 'protected'),
          'acceptBufSize': ('b', lambda val: self._get_accept_buf_size(val), 4096),
          'multiSession':  ('l', lambda val: self._check_boolean(val), True),
          'properties':    ('r', lambda val: self._get_properties(val), None),
          'version':       ('v', lambda val: self._check_str(r'.{1,15}', val), '1.0'),
          'description':   ('d', lambda val: self._get_description(val), None),
          'cryptoSelfTest':('t', lambda val: self._check_boolean(val), False),
          'stackSize':     ('S', lambda val: self._get_non_negative(val), 0x8000),
          'heapSize':      ('H', lambda val: self._get_non_negative(val), 0xE000),
          'storageFilesNoPersist' :('g', lambda val: self._check_boolean(val), False),
          'totalStorageFiles' : ('f', lambda val: self._get_non_negative(val), 60)
      }
      
    # Maps strings, e.g. "I2C", to class IDs.
    self._classIdMap = id_map
    self.metadata_current_field = None
    
    # The default privilege set (numbers).  This is the translation to numbers
    # of the elements in the _kDefaultPrivilegeIds set.
    self._defaultPrivileges = self._gen_class_set(_kDefaultPrivilegeIds)


  def _dumpClassMapById(self):
      header = "=" * 80
      print header
      print "Class map by id (%d ids)" % ( len(self._classIdMap))
      print header

      max = len(max(self._classIdMap, key=len));

      formatString = "%%-%d.%ds %%6d" % ( max, max )
      for id in sorted( self._classIdMap.keys()):
          print formatString % ( id, self._classIdMap[id] )
      print ""


  def _dumpClassMapByValue(self):
      revmap = dict((v,k) for k,v in self._classIdMap.iteritems())

      header = "=" * 80
      print header
      print "Class map by value (%d values)" % ( len(revmap))
      print header

      for value in sorted( revmap.keys()):
          print "%-6d %s" % ( value, revmap[value] )
      print ""


  def extractField(self, data, field):
      self.metadata_current_field = field
      if field in self._metadataHandlers:
          func = self._metadataHandlers[field][1]
          # Fetch the data for the given field if it exists, otherwise use the
          # default.
          if field in data:
              value = data[field]
          else:
              value = self._metadataHandlers[field][2]

          # If we have a value, process it now.  If no value exists, such as the
          # case where there is no default, then return the value which is None
          if value != None:
              s = func(value)
          else:
              s = value
          return s
      else:
          for valid_name in self._metadataHandlers:
              if valid_name.lower()[:3] == field.lower()[:3]:
                  raise Exception('Invalid metadata field name "%s" - should be "%s"' %
                                   (field, valid_name))

          valid_names = ', '.join(sorted(self._metadataHandlers))
          raise Exception('Invalid metadata field name "%s" - should be one of: %s' %
                          (field, valid_names))

  def generate_metadata(self, data):
      """
      Turns a set of program attributes in human readable form into a
      string parsed by the mink subsystem.

      Args:

          There is one required argument, md (metadata).  The md argument
          contains program attributes in a human readable form.  md is a
          dictionary that contains the following fields:

          md = {
            'privileges':  <iterable of string ids>,
            'services':    <iterable of string ids>,
            'appName':     <string containing 1-31 characters>
            'UUID':        <string containing 1-32 hex digits>
            'objectLimit': <non-negative number>
            'memoryType':  <Protected|Unprotected>,
          }

          Description of each of the fields:

              privileges:  Iterable of strings corresponding to class IDs or
                           services IDs.

                           There are some pseudo IDs that correspond to a
                           set of IDs.  There is currently only one pseudo ID,
                           'default'.  'default' contains a minimal set of sensible
                           privileges.  There may be more pseudo IDs in the future,
                           e.g. 'crypto'.

                           The ID fields are are the same as the corresponding
                           define for the class ID, minus the leading 'C' and
                           trailing '_UID'.  The ID for the CI2C_UID is 'I2C';
                           the ID for the CDeviceID_UID is 'DeviceID', etc.

                           If the privileges field is ommitted, it will
                           behave as if 'privileges': {'default'} is passed
                           in.

              services:    Iterable of strings corresponding to class IDs of services.
                           It's the set of IDs of services that the app exports.
                           Other apps requesting access to the service will need
                           to add the same ID to its list of privileges.

              appName:     A string of 1 to 31 characters.

              UUID:        A UUID in cannonical format (8-4-4-4-12), for example:

                               12345678-1234-1234-1234-123456789abc

                           The string is 32 hex digits and uses 4 hyphens for
                           a total string length of 36 characters.

              objectLimit: A non-negative integer.

              memoryType:  The string 'Protected' or 'Unprotected'.

                           If the privileges field is ommitted, it will
                           behave as if 'memoryType': 'Protected' is passed
                           in.

          None of the fields is required.  If a field is present, the name
          must be exactly as above, including the case.

          The IDs used in the privileges and services fields are case independent -
          'DEVICEID', 'deviceid', and 'DeviceId' are all the same ID.

          Likewise, the value for memoryType, either 'Protected' or 'Unprotected',
          is case independent.

      Examples:

          md = {
              'appName':     'SampleApp',
              'privileges':  ['default','I2C'],
              'services':    ['OEMUnwrapKeys'],
              'UUID':        '12345678-9abc-def0-1234-56789abcdef0',
              'objectLimit': 10,
              'memoryType':  'Protected',
          }

          md = {
              'appName':     'SampleApp2',
              'privileges':  ['I2C'],
          }

      Returns:

          A string that is only meaningful to the mink subsystem.  It's basically the same
          information that's in the input metadata but in a more easily parseable, although
          more cryptic, format.  Some sample values:

              p=2b;s=3c;n=SampleApp;u=78563412bc9af0de123456789abcdef0;o=10

              p=2b;n=SampleApp2

      Raises:

          The base level Exception class is the only exception raised by this function.

          This function tries to do as much validation as possible.  Some conditions
          that will raise an Exception:

              1) If the input metadata has an attribute that is not recognized.
              2) If a field has a value that is not of the correct type.
              2) If a field has a value that is not in range.
              3) If any ID in the privileges or services field is not recognized.
              4) Other conditions.

      """

      metadata = {}
      is_protected_memory = True
      # If no privileges have been specified then give the app the default
      # privileges.
      #
      # "By default, applications are loaded into PIMEM, which is backed by DDR
      # but employs encryption to protect against physical attacks."
      #
      # Add the default privileges to the input md object.  Create a copy
      # because it's bad practice to change the user's input data.

      if (not 'privileges' in data) or (not 'memoryType' in data) or (not 'acceptBufSize' in data) or \
         (not 'stackSize' in data) or (not 'heapSize' in data):
          data = copy.copy(data)
          if not 'privileges' in data:
              data['privileges'] = self._metadataHandlers['privileges'][2]
          if not 'memoryType' in data:
              data['memoryType'] = self._metadataHandlers['memoryType'][2]
          if not 'acceptBufSize' in data:
              data['acceptBufSize'] = self._metadataHandlers['acceptBufSize'][2]
          if (not 'stackSize' in data):
              data['stackSize'] = self._metadataHandlers['stackSize'][2]
          if (not 'heapSize' in data):
              data['heapSize'] = self._metadataHandlers['heapSize'][2]

      # Process the metadata.
      for field in data:
          # These properties are not added to metadata string for now
          # Buffer size not added to metadata string for now
          # GP related fields not added to metadata string
          if field in ['acceptBufSize', 'multiSession', 'properties', 'version',
                       'description', 'stackSize', 'heapSize',
                       'storageFilesNoPersist', 'cryptoSelfTest']:
             continue

          if field in self._metadataHandlers:
              abbr = self._metadataHandlers[field][0]
              s = self.extractField(data, field)
              if s is None:
                  continue
              if field == 'memoryType' and s == 'x':
                  is_protected_memory = False
              if abbr in metadata:
                  raise Exception('Field "%s" appears multiple times.' % ( field ))
              metadata[abbr] = s
          else:
              for valid_name in self._metadataHandlers:
                  if valid_name.lower()[:3] == field.lower()[:3]:
                      raise Exception('Invalid metadata field name "%s" - should be "%s"' %
                                       (field, valid_name))

              valid_names = ', '.join(sorted(self._metadataHandlers))
              raise Exception('Invalid metadata field name "%s" - should be one of: %s' %
                              (field, valid_names))

      # Concatenate the result for each metadata field into one big string.

      output_values = ['%s=%s' % (abbr, metadata[abbr]) for abbr in sorted(metadata.keys())]

      str = ';'.join(output_values)

      if is_protected_memory:
          str += _kMarker2;
      else:
          str += _kMarker1;

      return str

  def check_class_ids(self):
    '''
    verifies that all class IDs are within Qualcomm range
    Qualcomm: 0 - 0xffffff
    OEM:      0x1000000 - 0x1ffffff
    
    This should only be invoked in internal builds, as external builds
    may have .idl files written by oems that contain class IDs in their
    range
    '''
    for k,v in self._classIdMap.items():
      if not 0 <= v < 0x1000000:
        raise Exception('Class ID {} with value {} is outside of the Qualcomm range \
        Please give it a new class id within 0 - 0xffffff'.format(k, hex(v)))
      
  def _gen_class_set(self, idset):
      """
      Returns a set of class numbers
      """
     
      values = set()
      for id in idset:
          #  Identifier - alphanumeric beginning with a non-digit.
          classId = self._check_str(r'[a-zA-Z_][a-zA-Z_0-9]*', id)
          key = classId.lower()

          if key == 'default':
              values.update(self._defaultPrivileges)
          elif key in self._classIdMap:
              classNum = self._classIdMap[key]
              values.add(classNum)
          else:
              raise Exception('Invalid metadata class id: %s' % (id))

      return values


  def _gen_class_set_string(self, idset):
      values = self._gen_class_set(idset)

      if len(values) == 0:
          return None

      return _encode_id_set(values)


  def _get_non_negative(self, value):
      self._check_number(value)
      if value < 0:
          raise Exception('Metadata %s value "%s" is not non-negative' %
              (self.metadata_current_field, str(value)))
      return value


  def _get_accept_buf_size(self, value):
      self._check_number(value)
      # We have to have enough buffer to survive, go with minimum of 1k
      if value < 1024:
         raise Exception('Accept buffer size %s is less than the minimum of 1024' % str(value))
      return value


  def _get_properties(self, value):
      props = ast.literal_eval(str(value))
      if not isinstance(props, dict):
          raise Exception('properties has to be a dictionary')
      # properties is a dictionary {<string>: <string>}
      for k in props:
          if props[k] == None:
              raise Exception('Empty element for key %s' % str(k))
          if (not isinstance(k, basestring)) or (not isinstance(props[k], basestring)):
              raise Exception('Only string keys/values supported: %s: %s' % (str(k), str(props[k])))
          if k.startswith("gpd."):
              raise Exception('"gpd." is a reserved domain: %s' % str(k))
      return props


  def _get_description(self, value):
      if value == None:
          return None
      return self._check_str(r'.{1,31}', value)


  def _get_memory_type(self, value):
      if value.lower() == "unprotected":
          return 'x'
      elif value.lower() == "protected":
          return None

      raise Exception('Metadata %s value "%s" is invalid.  Valid values are "protected", "unprotected"' %
              (self.metadata_current_field, value))


  def _check_number(self, value):
      return self._check_str(r'-?\d+', str(value))


  def _check_str(self, regex, str):
      if re.match('^(' + regex + ')$', str):
          return self._escape(str)
      else:
          raise Exception('Metadata %s value "%s" does not conform to pattern /%s/' %
              (self.metadata_current_field, str, regex))


  def _check_boolean(self, value):
      if isinstance(value, bool):
          return 'true' if value == True else 'false'
      elif value.lower() in ['true', 'false']:
          return value.lower()
      else:
          raise Exception('Metadata %s value is not of boolean type' % (self.metadata_current_field))


  def _escape(self, value):
      return value.replace('%', '%25'). \
                   replace(',', '%2C'). \
                   replace(';', '%3B'). \
                   replace('=', '%3D')




#-------------------------------------------------------------------------------
#  _MetadataTest
#-------------------------------------------------------------------------------

#  We can turn off interpretation / running of the unit tests completely
#  if we're running in the SConscript environment.  If we're running as
#  a standalone script, add the following code.

if __name__ == '__main__':

    #  Some of the unit tests expressions, e.g. with self.assertRaises(Exception),
    #  are relying on 2.7 features of the python unit test framework.  It causes
    #  python interpreter errors on 2.6.8.

    if sys.hexversion >= 0x2070000:

        class _MetadataTest(unittest.TestCase):

            #  If privileges is not specified, you get the default privileges.
            #  Specify them so it's easier to write tests.

            kDefault = {'privileges':['I2C']}

            #-------------------------------------------------------------------
            #  test_privileges
            #-------------------------------------------------------------------

            def setUp(self):
                self.md_class = SecureAppMetadataClass()

            def tearDown(self):
                self.md_class = None

            def test_privileges(self):

                # Test generating the default.
                self.assertEqual('p=2b' + _kMarker2, self.md_class.generate_metadata( self.kDefault ))

                # Test default is added if no privileges specified.
                md = {}
                pset = _kDefaultPrivilegeIds;
                psetstr = self.md_class._gen_class_set_string(pset)
                self.assertEqual("p={0}{1}".format(psetstr, _kMarker2), self.md_class.generate_metadata( md ))

                # Test adding some valid privileges.
                md = {'privileges': ['DeviceID','NSMem']}
                self.assertEqual("p=c:4{0}".format(_kMarker2), self.md_class.generate_metadata( md ))

                # Test empty privilege list.
                md = { 'privileges': [],}
                self.assertEqual("{0}".format(_kMarker2), self.md_class.generate_metadata( md ))

                # Test using an invalid id.
                with self.assertRaises(Exception):
                    self.md_class.generate_metadata({ 'privileges': ['invalid']})

                # Test using an invalid type for the privileges.  It should be a set of strings, not a string.
                with self.assertRaises(Exception):
                    self.md_class.generate_metadata({ 'privileges': '1:a1'})

                # Test using an invalid type for the privileges.  It should be a set of strings, not a set of int.
                with self.assertRaises(Exception):
                    self.md_class.generate_metadata({ 'privileges': [100]})

            #-------------------------------------------------------------------
            #  test_services
            #-------------------------------------------------------------------

            def test_services(self):

                # Test some valid services.  Explicitly add privileges so default is not added.
                md = copy.copy( self.kDefault )
                md.update({'services': ['OEMUnwrapKeys']})
                self.assertEqual('p=2b;s=400' + _kMarker2, self.md_class.generate_metadata( md ))

                # Test empty services list.
                md = copy.copy( self.kDefault )
                md.update({'services': []})
                self.assertEqual('p=2b' + _kMarker2, self.md_class.generate_metadata( md ))

                # Test adding an invalid service.
                with self.assertRaises(Exception):
                    self.md_class.generate_metadata({ 'services': ['invalid']})

                # Test services type is valid.  It should be a set of strings, not a string.
                with self.assertRaises(Exception):
                    self.md_class.generate_metadata({ 'services': 'abc' })

                # Test services type is valid.  It should be a set of strings, not a set of int.
                with self.assertRaises(Exception):
                    self.md_class.generate_metadata({ 'services': [100]})

            #-------------------------------------------------------------------
            #  test_appname
            #-------------------------------------------------------------------

            def test_appname(self):

                # Test valid appName.
                md = copy.copy( self.kDefault )
                md.update({'appName': 'Foo'})
                self.assertEqual('n=Foo;p=2b' + _kMarker2, self.md_class.generate_metadata( md ))

                # Test valid appName with min length.
                md = copy.copy( self.kDefault )
                md.update({'appName': 'X'})
                self.assertEqual('n=X;p=2b' + _kMarker2, self.md_class.generate_metadata( md ))

                # Test valid appName with max length.
                md = copy.copy( self.kDefault )
                md.update({'appName': '1234567890123456789012345678901'})
                self.assertEqual('n=1234567890123456789012345678901;p=2b' + _kMarker2, self.md_class.generate_metadata( md ))

                # Test invalid appName - name too short.
                with self.assertRaises(Exception):
                    self.md_class.generate_metadata({ 'appName': '' })

                # Test invalid appName - name too long.
                with self.assertRaises(Exception):
                    self.md_class.generate_metadata({ 'appName': '12345678901234567890123456789012' })

            #-------------------------------------------------------------------
            #  test_uuid
            #-------------------------------------------------------------------

            def test_uuid(self):

                # Test valid UUID data.
                md = copy.copy( self.kDefault )
                md.update({'UUID': '12345678-1234-1234-1234-123456789abc'})
                self.assertEqual('p=2b;u=78563412341234121234123456789abc' + _kMarker2, self.md_class.generate_metadata( md ))

                # Test invalid data - invalid hex digit.
                with self.assertRaises(Exception):
                    self.md_class.generate_metadata({ 'UUID': 'X2345678-1234-1234-1234-123456789abc' })

                # Test invalid data - invalid format
                with self.assertRaises(Exception):
                    self.md_class.generate_metadata({ 'UUID': '1234-5678-1234-1234-1234-123456789abc' })

            #-------------------------------------------------------------------
            #  test_object_limit
            #-------------------------------------------------------------------

            def test_object_limit(self):

                # Test valid data.
                md = copy.copy( self.kDefault )
                md.update({'objectLimit': 100 })
                self.assertEqual('o=100;p=2b' + _kMarker2, self.md_class.generate_metadata( md ))

                # Test valid data - min value.
                md = copy.copy( self.kDefault )
                md.update({'objectLimit': 0 })
                self.assertEqual('o=0;p=2b' + _kMarker2, self.md_class.generate_metadata( md ))

                # Test invalid data - less than min.
                with self.assertRaises(Exception):
                    self.md_class.generate_metadata({ 'objectLimit': -20 })

                # Test invalid data - invalid type.
                with self.assertRaises(Exception):
                    self.md_class.generate_metadata({ 'objectLimit': 'ABC' })

            #-------------------------------------------------------------------
            #  test_memory_type
            #-------------------------------------------------------------------

            def test_memory_type(self):


                # Test valid data.
                md = copy.copy( self.kDefault )
                md.update({'memoryType': 'Unprotected'})
                expected = 'm=x;p=2b' + _kMarker1
                self.assertEqual( expected, self.md_class.generate_metadata( md ))

                # Test valid data.  If memoryType not specified, then it's protected (i.e. not m=x).
                self.assertEqual('p=2b' + _kMarker2, self.md_class.generate_metadata({ 'privileges': ['I2C']}))

                # Test invalid data.  'Unprotected' (case independent) is the only valid value.
                with self.assertRaises(Exception):
                    self.md_class.generate_metadata({ 'memoryType': 'invalid' })

            #-------------------------------------------------------------------
            #  test_bad_fields
            #-------------------------------------------------------------------

            def test_bad_fields(self):

                with self.assertRaises(Exception) as context:
                    self.md_class.generate_metadata({ 'appname': 'Foo' })
                self.assertEquals('Invalid metadata field name "appname" - should be "appName"', context.exception.message)

                with self.assertRaises(Exception) as context:
                    self.md_class.generate_metadata({ 'MEM': 'x' })
                self.assertEquals('Invalid metadata field name "MEM" - should be "memoryType"', context.exception.message)

                with self.assertRaises(Exception) as context:
                    self.md_class.generate_metadata({ 'random': 'Foo' })
                self.assertEquals('Invalid metadata field name "random" - should be one of: ' + ', '.join(sorted(self.md_class._metadataHandlers.keys())), context.exception.message)

            #-------------------------------------------------------------------
            #  test_all_fields
            #-------------------------------------------------------------------

            def test_all_fields(self):

                md = {
                    'privileges':  ['I2C','DeviceId'],
                    'services':    ['OEMUnwrapKeys'],
                    'appName':     'SampleApp',
                    'UUID':        '12345678-1234-1234-1234-123456789ABC',
                    'objectLimit': 100,
                    'memoryType':  'protected'
                }

                self.assertEqual('n=SampleApp;o=100;p=c,2b;s=400;u=78563412341234121234123456789ABC' + _kMarker2, self.md_class.generate_metadata( md ))

            #-------------------------------------------------------------------
            #  test_escaping
            #-------------------------------------------------------------------

            def test_escaping(self):

                md = copy.copy( self.kDefault )
                md.update({'appName': '=%,;ZZZ' })
                self.assertEqual('n=%3D%25%2C%3BZZZ;p=2b' + _kMarker2, self.md_class.generate_metadata( md ))

            #-------------------------------------------------------------------
            #  test_extract_field
            #-------------------------------------------------------------------
            def test_extract_field(self):
                md = {
                    'privileges':  ['I2C','DeviceId'],
                    'services':    ['OEMUnwrapKeys'],
                    'appName':     'FooBar',
                    'UUID':        '12345678-1234-1234-1234-123456789ABC',
                    'objectLimit': 100,
                    'memoryType':  'protected',
                    'acceptBufSize': 8192,
                    'multiSession': False,
                    'version': '2.1',
                    'description': 'My TA',
                    'properties': { 'key0': 'value0', 'key1': 'value1'},
                    'stackSize': 0x20000,
                    'heapSize': 0x20000,
                    'cryptoSelfTest': True,
                    'storageFilesNoPersist': True,
                    'totalStorageFiles': 2000
                }
                self.assertEqual('FooBar', self.md_class.extractField( md, 'appName' ))
                self.assertEqual(None, self.md_class.extractField( md, 'memoryType' ))
                self.assertEqual('c,2b', self.md_class.extractField( md, 'privileges' ))
                self.assertEqual('400', self.md_class.extractField( md, 'services' ))
                self.assertEqual(8192, self.md_class.extractField( md, 'acceptBufSize' ))
                self.assertEqual('false', self.md_class.extractField( md, 'multiSession' ))
                self.assertEqual(100, self.md_class.extractField( md, 'objectLimit' ))
                self.assertEqual('78563412341234121234123456789ABC', self.md_class.extractField( md, 'UUID' ))
                self.assertEqual('2.1', self.md_class.extractField( md, 'version' ))
                self.assertEqual('My TA', self.md_class.extractField( md, 'description' ))
                self.assertEqual(0x20000, self.md_class.extractField( md, 'stackSize' ))
                self.assertEqual(0x20000, self.md_class.extractField( md, 'heapSize' ))
                self.assertEqual('true', self.md_class.extractField( md, 'cryptoSelfTest' ))
                self.assertEqual('true', self.md_class.extractField( md, 'storageFilesNoPersist' ))
                self.assertEqual(2000, self.md_class.extractField( md, 'totalStorageFiles' ))

                md.update({'memoryType': 'Unprotected' })
                self.assertEqual('x', self.md_class.extractField( md, 'memoryType' ))

                md = {}
                self.assertEqual(None, self.md_class.extractField( md, 'memoryType' ))
                psetstr = self.md_class._gen_class_set_string(_kDefaultPrivilegeIds)
                self.assertEqual(psetstr, self.md_class.extractField( md, 'privileges' ))
                md = {}
                self.assertEqual(4096, self.md_class.extractField( md, 'acceptBufSize' ))
                self.assertEqual('true', self.md_class.extractField( md, 'multiSession' ))
                md.update({'multiSession': False })
                self.assertEqual('false', self.md_class.extractField( md, 'multiSession' ))
                self.assertEqual(None, self.md_class.extractField( md, 'properties' ))
                self.assertEqual('1.0', self.md_class.extractField( md, 'version' ))
                self.assertEqual(None, self.md_class.extractField( md, 'description' ))

            #-------------------------------------------------------------------
            #  test_extract_field_fail
            #-------------------------------------------------------------------
            def test_extract_field_fail(self):
                md = {
                    'privileges':  ['I2C','DeviceId'],
                    'services':    ['OEMUnwrapKeys'],
                    'appName':     'FooBar',
                    'UUID':        '12345678-1234-1234-1234-123456789ABC',
                    'objectLimit': 100,
                    'memoryType':  'protected',
                    'acceptBufSize': 8192,
                    'multiSession': False,
                    'version': '2.1',
                    'description': 'My TA',
                    'properties': { 'key0': 'value0', 'key1': 'value1'},
                    'stackSize': 0x20000,
                    'heapSize': 0x20000,
                    'cryptoSelfTest': True,
                    'storageFilesNoPersist': True,
                    'totalStorageFiles': 2000
                }
                with self.assertRaises(Exception) as context:
                    self.md_class.extractField( md, 'appname' )
                self.assertEquals('Invalid metadata field name "appname" - should be "appName"', context.exception.message)

                with self.assertRaises(Exception) as context:
                    self.md_class.extractField( md, 'appnaMe' )
                self.assertEquals('Invalid metadata field name "appnaMe" - should be "appName"', context.exception.message)

                with self.assertRaises(Exception) as context:
                    self.md_class.extractField( md, 'memory_type' )
                self.assertEquals('Invalid metadata field name "memory_type" - should be "memoryType"', context.exception.message)

                md.update({'acceptBufSize': 20 })
                with self.assertRaises(Exception) as context:
                    self.md_class.extractField( md, 'acceptBufSize' )
                self.assertEquals('Accept buffer size 20 is less than the minimum of 1024', context.exception.message)

                md.update({'multiSession': 'false_' })
                with self.assertRaises(Exception) as context:
                    self.md_class.extractField( md, 'multiSession' )
                self.assertEquals("Metadata {} value is not of boolean type".format('multiSession'), context.exception.message)

                md.update({'properties': ['test'] })
                with self.assertRaises(Exception) as context:
                    self.md_class.extractField( md, 'properties' )
                self.assertEquals('properties has to be a dictionary', context.exception.message)

                md.update({'properties': {'key0': None }})
                with self.assertRaises(Exception) as context:
                    self.md_class.extractField( md, 'properties' )
                self.assertEquals('Empty element for key key0', context.exception.message)

                md.update({'properties': {'key0': 42 }})
                with self.assertRaises(Exception) as context:
                    self.md_class.extractField( md, 'properties' )
                self.assertEquals('Only string keys/values supported: key0: 42', context.exception.message)

                md.update({'properties': {42: 'value0' }})
                with self.assertRaises(Exception) as context:
                    self.md_class.extractField( md, 'properties' )
                self.assertEquals('Only string keys/values supported: 42: value0', context.exception.message)

                md.update({'properties': {'gpd.testProp': 'value0' }})
                with self.assertRaises(Exception) as context:
                    self.md_class.extractField( md, 'properties' )
                self.assertEquals('"gpd." is a reserved domain: gpd.testProp', context.exception.message)

                md.update({'cryptoSelfTest': 'true_' })
                with self.assertRaises(Exception) as context:
                    self.md_class.extractField( md, 'cryptoSelfTest' )
                self.assertEquals("Metadata {} value is not of boolean type".format('cryptoSelfTest'), context.exception.message)

                md.update({'storageFilesNoPersist': '0' })
                with self.assertRaises(Exception) as context:
                    self.md_class.extractField( md, 'storageFilesNoPersist' )
                self.assertEquals("Metadata {} value is not of boolean type".format('storageFilesNoPersist'), context.exception.message)

            #-------------------------------------------------------------------
            #  test_encode_id_set
            #-------------------------------------------------------------------

            def test_encode_id_set(self):

                self.assertEqual('1,a,14', _encode_id_set({1,10,20}))

                #  Just for grins, verify that you can use a list or
                #  a tuple (any iterable really) instead of a set.
                #
                #  Make sure _encode_id_set is well behaved if you use
                #  non set properties (e.g. the you can have multiple
                #  copies of a given element.)

                self.assertEqual('1,a,14', _encode_id_set( [1,10,20,1,10,20] ))
                self.assertEqual('1,a,14', _encode_id_set( (1,10,20,1,10,20) ))

                self.assertEqual('1:1',    _encode_id_set({1,2}))
                self.assertEqual('1:2',    _encode_id_set({1,3}))
                self.assertEqual('1:4',    _encode_id_set({1,4}))
                self.assertEqual('1:8',    _encode_id_set({1,5}))
                self.assertEqual('1,6',    _encode_id_set({1,6}))
                self.assertEqual('1:18',   _encode_id_set({1,2,9}))
                self.assertEqual('1:28',   _encode_id_set({1,3,9}))
                self.assertEqual('1:48',   _encode_id_set({1,4,9}))
                self.assertEqual('1:88',   _encode_id_set({1,5,9}))
                self.assertEqual('1,6:4',  _encode_id_set({1,6,9}))
                self.assertEqual('1,7:2',  _encode_id_set({1,7,9}))
                self.assertEqual('1,8:1',  _encode_id_set({1,8,9}))

                # Many encodings are equivalent and acceptable, but since we know
                # the implementation, we can simply check for one expected output.

                self.assertEqual('102',            _encode_id_set({258}))
                self.assertEqual('100,200:1',      _encode_id_set({256, 512, 513}))
                self.assertEqual('1:2,20',         _encode_id_set({1, 3, 32}))
                self.assertEqual('1:981101',       _encode_id_set({1,2,5,9,10,14,22}))
                self.assertEqual('10000000:0001',  _encode_id_set({0x10000000, 0x1000000d}))

                # TODO: out of range (IDs negative, too large)

            #-------------------------------------------------------------------
            #  test_encode_uuid
            #-------------------------------------------------------------------

            def test_encode_uuid(self):
                #  Test some UUIDs in the GP apps.
                self.assertEqual('52414d532d5453434c542d54412d3031', _encode_uuid('534d4152-542d-4353-4c54-2d54412d3031'))
                self.assertEqual('52414d5343544c535441544346494341', _encode_uuid('534d4152-5443-534c-5441-544346494341'))
                self.assertEqual('52414d5343544c535454434649434132', _encode_uuid('534d4152-5443-534c-5454-434649434132'))
                self.assertEqual('52414d5343544c534d4c54494e535443', _encode_uuid('534d4152-5443-534c-4d4c-54494e535443'))
                self.assertEqual('52414d5343544c5353474c494e535443', _encode_uuid('534d4152-5443-534c-5347-4c494e535443'))
                self.assertEqual('4205d1cae4342d456156e979aa6e61bc', _encode_uuid('cad10542-34e4-452d-6156-e979aa6e61bc'))

                self.assertEqual('52414d532d5453434c542d54412d4552', _encode_uuid('534d4152-542d-4353-4c54-2d54412d4552'))
                self.assertEqual('52414d5343544c5354455252544f4f53', _encode_uuid('534d4152-5443-534c-5445-5252544f4f53'))
                self.assertEqual('52414d532d5453434c542d54412d5355', _encode_uuid('534d4152-542d-4353-4c54-2d54412d5355'))
                self.assertEqual('52414d5343544c53544f53345041524d', _encode_uuid('534d4152-5443-534c-544f-53345041524d'))
                self.assertEqual('52414d532d5453434c542d54412d5354', _encode_uuid('534d4152-542d-4353-4c54-2d54412d5354'))
                self.assertEqual('52414d5343544c535443525950544f31', _encode_uuid('534d4152-5443-534c-5443-525950544f31'))
                self.assertEqual('52414d5343544c535444415441535431', _encode_uuid('534d4152-5443-534c-5444-415441535431'))
                self.assertEqual('52414d5343544c535f54494d45415049', _encode_uuid('534d4152-5443-534c-5f54-494d45415049'))

    #---------------------------------------------------------------------------
    #  Run unit tests
    #---------------------------------------------------------------------------

    #_dumpClassMapById()
    #_dumpClassMapByValue()

    if sys.hexversion >= 0x2070000:
        unittest.main()
    else:
        print 'Need python >= 2.7 to run unit tests'
