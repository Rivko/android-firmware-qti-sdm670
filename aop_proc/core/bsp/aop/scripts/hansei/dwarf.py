#  ===========================================================================
#
#  Copyright (c) 2011-2016 Qualcomm Technologies, Inc.  
#  All Rights Reserved.
#  QUALCOMM Proprietary and Confidential.
#
#  ===========================================================================


import mmap
import struct
from collections import namedtuple
import logging
logger = logging.getLogger(__name__)

from elftools.elf.elffile import ELFFile
from elftools.dwarf.die import AttributeValue

from elftools.common.py3compat import ( ifilter, byte2int, bytes2str, itervalues, str2bytes) 

  
Variable = namedtuple('Variable', 'name address vartype die')
Function = namedtuple('Function', 'name address die')

class DebugInfo(object):
    def __init__(self, path_to_elf):
        logger.debug('Opening ELF...')
        self._raw_file = open(path_to_elf, 'rb')
        self._map = mmap.mmap(self._raw_file.fileno(), 0, access=mmap.ACCESS_READ)
        self.elf = ELFFile(self._map)
        assert self.elf.has_dwarf_info()
        self.dwarf = self.elf.get_dwarf_info()

        self.variables = {}
        self._concrete_functions = []
        self._abstract_functions = []
        self.types = {}
        self.dies = {}

        logger.debug('Parsing DIE\'s...')
        for cu in self.dwarf.iter_CUs():
            for die in cu.iter_DIEs():
                self._parse_die(die)

        self.functions = {self._get_func_name(die) : die.attributes['DW_AT_low_pc'].value for die in self._concrete_functions}
        
        logger.debug('Debug information loaded!')

    def _get_func_name(self, die):
        if 'DW_AT_abstract_origin' in die.attributes:
            offset = die.attributes['DW_AT_abstract_origin'].value
            if die.attributes['DW_AT_abstract_origin'].form == 'DW_FORM_ref4':
				offset += die.cu.cu_offset
            origin = self.dies[offset]
            return self._get_func_name(origin)
        
        if 'DW_AT_specification' in die.attributes:
            v = die.attributes['DW_AT_specification'].value
            if die.attributes['DW_AT_specification'].form == 'DW_FORM_ref4':
				v += die.cu.cu_offset
            spec = self.dies[v]
            return self._get_func_name(spec)

        name = die.attributes['DW_AT_name'].value
        parent = die.get_parent()
        if parent.tag == 'DW_TAG_structure_type':
            return parent.attributes['DW_AT_name'].value + '::' + name
        return name

    def _parse_variable(self, die):
        name = die.attributes.get('DW_AT_name')
        if not name:
            return # skip anonymous stack variables and the like
        name = name.value

        loc = die.attributes.get('DW_AT_location')
        if not loc:
            return # skip implicitly created return variable and the like
	 # form could be DW_FORM_block1, DW_FORM_block2, DW_FORM_block4 or DW_FORM_block
        if not loc.form.startswith('DW_FORM_block') or not loc.value:
            return # skip hard-to-parse but so far uninteresting variable types
        try:
            is_block1 = loc.form == 'DW_FORM_block1' and len(loc.value) > 5
            assert loc.value[0] == 0x3
            loc = loc.value
            address = (loc[1] | loc[2] << 8 | loc[3] << 16 | loc[4] << 24)
            if is_block1:
			    address += loc[6] if is_block1 else 0
                #print "%s %x" %(name, address)
				#import pdb; pdb.set_trace()
        except (AssertionError, IndexError):
            return # skip oddly formed address types

        vartype = die.attributes.get('DW_AT_type')
        if not vartype:
            return # no way to interpret this data

        self.variables[name] = Variable(name, address, vartype, die)

    def _parse_subprogram(self, die):
        if 'DW_AT_low_pc' in die.attributes and 'DW_AT_high_pc' in die.attributes:
            self._concrete_functions.append(die)
        else:
            self._abstract_functions.append(die)

    def _parse_type(self, die):
        if 'DW_AT_name' not in die.attributes:
            return

        type_name = die.attributes['DW_AT_name'].value
        if type_name in self.types and len(die.attributes) < len(self.types[type_name].attributes):
            return # ignore less descriptive definitions
            
        self.types[type_name] = die

    def _parse_die(self, die):
        self.dies[die.offset] = die

        parsers = {
            'DW_TAG_variable'       : self._parse_variable,
            'DW_TAG_subprogram'     : self._parse_subprogram,
            'DW_TAG_typedef'        : self._parse_type,
            'DW_TAG_structure_type' : self._parse_type,
            'DW_TAG_class_type'     : self._parse_type,
        }

        p = parsers.get(die.tag, lambda p: None)
        p(die)

        for d in die.iter_children():
            self._parse_die(d)
    
    def get_section_data(self, section_name):
        section = self.elf.get_section_by_name(str2bytes(section_name))
        if section != None:
          data = section.data()
          size = section['sh_size']
          '''
            Return [ADDR, DATA, SIZE]
          '''
          return [section['sh_addr'], data, size]
        else:
          return [None, None, None]

def decode_object(name, address, vartype, system, debug_info, die = None):
    obj = DwarfObject(system, debug_info)

    if not die:
        #try to get die
        if name in debug_info.variables:
            die = debug_info.variables[name].die
        cu_offset = die.cu.cu_offset if die else 0
        vartype_addr = vartype.value
        if vartype.form == 'DW_FORM_ref4':
            vartype_addr += cu_offset
        type_die = debug_info.dies[vartype_addr]
        type_die = obj._find_basic_type(type_die)
    else:
        type_die = obj._find_basic_type(die)
    return obj._decode(name, address, type_die)

class DwarfObject(object):
    def __init__(self, system, debug_info):
        self._system = system
        self._debug_info = debug_info
        self._struct_fmts = {
            'signed' : {
                1 : '<b',
                2 : '<h',
                4 : '<i',
                8 : '<q',
            },
            'float' : {
                4 : '<f',
                8 : '<d',
            },
        }
        self._struct_fmts['unsigned'] = {k : v.upper() for k,v in self._struct_fmts['signed'].iteritems()}

    def _calculate_upper_bound(self, array_type):
        for c in array_type.iter_children():
            if c.tag == 'DW_TAG_subrange_type':
                bound = c.attributes.get('DW_AT_upper_bound')
                if not bound:
                    bound = c.attributes.get('DW_AT_count')
                if bound:
                    return bound.value
        logger.warning('Couldn\'t find an upper bound for %s' % array_type)
        return None

    def _decode(self, name, address, vartype, host = None):
        # Structures and unions are basically just containers of other stuff.
        if vartype.tag == 'DW_TAG_structure_type' or vartype.tag == 'DW_TAG_union_type' or vartype.tag == 'DW_TAG_class_type':
            logger.debug('%s is a structure/union (@ 0x%0.8x), returning new object' % (name, address))
            return Structure(name, address, vartype, self._system, self._debug_info)

        # Base types are nice and easy to parse.
        if vartype.tag == 'DW_TAG_base_type':
            logger.debug('%s is a basic type @ 0x%0.8x, parsing and returning directly' % (name, address))
            return self._parse_basic_type(vartype, address)

        # Enumerations are kind of fun and happy.
        if vartype.tag == 'DW_TAG_enumeration_type':
            length = self._get_size(vartype)
            data = self._system.read(address, length)
            raw_val, = struct.unpack(self._struct_fmts['unsigned'][len(data)], data)
            enum_vals = {}
            for enumerator in vartype.iter_children():
                if enumerator.tag != 'DW_TAG_enumerator':
                    continue
                enum_name = enumerator.attributes['DW_AT_name'].value
                enum_val  = enumerator.attributes['DW_AT_const_value'].value
                enum_vals[enum_val] = enum_name
            return enum_vals.get(raw_val, raw_val)

        # Arrays make AJ sad, but that's ok, because we have a container class for them.
        if vartype.tag == 'DW_TAG_array_type':
            elem_type = self._unwrap(vartype)
            # I'm not sure why AJ used 'host' here, 'vartype' allows us to actually get the array bounds.
            #upper_bound = self._calculate_upper_bound(host)
            upper_bound = self._calculate_upper_bound(vartype)
            bstr = '%s elements' % upper_bound if upper_bound else 'unknown bound'
            logger.debug('%s is an array of %s (@ 0x%0.8x, %s), returning new object' % (name, elem_type.tag, address, bstr))
            return Array(name, address, elem_type, upper_bound, self._system, self._debug_info)

        # Whoever thought having pointers and arrays be the same thing should be shot.
        # I made a special class to handle it, but it makes stuff look weird.
        if vartype.tag == 'DW_TAG_pointer_type':
            pointee_type = self._find_basic_type(self._unwrap(vartype))
            pointer, = struct.unpack('<L', self._system.read(address, 4))
            logger.debug('%s is a pointer (@ 0x%0.8x, to a %s @ 0x%0.8x)' % (name, address, pointee_type.tag, pointer))
            if name == 'xsleep_stats':
                #import pdb; pdb.set_trace()
				#pointer = address
                print('%s is a pointer (@ 0x%0.8x, to a %s @ 0x%0.8x)' % (name, address, pointee_type.tag, pointer))
            return Pointer(name, pointer, pointee_type, self._system, self._debug_info)

        logger.critical('found an unknown basic type')
        #import pdb; pdb.set_trace()

    def _unwrap(self, some_type):
        unwrap_functions = {
            16 : lambda t, v: v,                  # this is a direct address reference
            17 : lambda t, v: t.cu.cu_offset + v, # this is a reference from the cu base
            18 : lambda t, v: t.cu.cu_offset + v, # this is a reference from the cu base
        }

        logging.debug('Unwrapping %s' % some_type.tag)
        if some_type.tag == 'DW_TAG_const_type' and 'DW_AT_type' not in some_type.attributes :
            return self._debug_info.types['uint32_t']
        # Handle void*
        if some_type.tag == 'DW_TAG_pointer_type' and  'DW_AT_type' not in some_type.attributes:
            return self._debug_info.types['uint8_t']
        typeinfo  = some_type.attributes['DW_AT_type']
        if typeinfo.form == 'DW_FORM_ref4':
            typeaddr = typeinfo.value + some_type.cu.cu_offset
        else:
            typeaddr  = unwrap_functions[typeinfo.raw_value](some_type, typeinfo.value)
        return self._debug_info.dies[typeaddr]

    def _find_basic_type(self, some_type):
        basic_types = [
            'DW_TAG_structure_type',
            'DW_TAG_class_type',
            'DW_TAG_base_type',
            'DW_TAG_union_type',
            'DW_TAG_array_type',
            'DW_TAG_pointer_type',
            'DW_TAG_enumeration_type',
            'DW_TAG_subroutine_type',
        ]

        wrapper_types = [
            'DW_TAG_typedef',
            'DW_TAG_member',
            'DW_TAG_const_type',
            'DW_TAG_volatile_type',
            'DW_TAG_reference_type',
            'DW_TAG_variable',
        ]

        while some_type.tag in wrapper_types:
            some_type = self._unwrap(some_type)

        assert some_type.tag in basic_types, 'unwrapped to unknown type [%x] %s' % (some_type.offset, some_type)
        return some_type

    def _parse_address(self, base, data):
        # So dwarf defines this very complex language for expressions which is
        # basically list a minature stack machine.  It's very neat, but luckily
        # we don't have to actually emulate it because ARMCT just generates one
        # instruction over and over (DW_OP_plus_uconst).
        assert data[0] == 0x23, 'Got an address operation other than DW_OP_plus_uconst!'
        data = data[1:]

        uconst = 0
        shift = 0
        while True:
            byte = data[0] & 0x7F
            uconst += (byte << shift)
            if 0 == (data[0] & 0x80):
                break
            data   = data[1:]
            shift += 7

        return base + uconst

    def _parse_basic_type(self, vartype, address):
        parsers = {
            0x1 : lambda data: struct.unpack('<L', data)[0],                                     # DW_ATE_address
            0x2 : lambda data: struct.unpack('?', data)[0],                                      # DW_ATE_boolean
            # skipping "complex float" because I'm not sure how to parse it
            0x4 : lambda data: struct.unpack(self._struct_fmts['float'][len(data)], data)[0],    # DW_ATE_float
            0x5 : lambda data: struct.unpack(self._struct_fmts['signed'][len(data)], data)[0],   # DW_ATE_signed
            0x6 : lambda data: struct.unpack(self._struct_fmts['signed'][len(data)], data)[0],   # DW_ATE_signed_char
            0x7 : lambda data: struct.unpack(self._struct_fmts['unsigned'][len(data)], data)[0], # DW_ATE_unsigned
            0x8 : lambda data: struct.unpack(self._struct_fmts['unsigned'][len(data)], data)[0], # DW_ATE_unsigned_char
        }

        length   = vartype.attributes['DW_AT_byte_size'].value
        encoding = vartype.attributes['DW_AT_encoding'].value
        typename = vartype.attributes['DW_AT_name'].value
        logger.debug('Parsing %s [%d bytes|%d] @ 0x%0.8x' % (typename, length, encoding, address))

        data = self._system.read(address, length)
        return parsers[encoding](data)

    def _get_name(self, die):
        name = die.attributes.get('DW_AT_name')
        if name:
            return name.value

        sibling = die.attributes.get('DW_AT_sibling')
        if sibling:
            cu_base = die.cu.cu_offset
            sibling_offset = sibling.value
            return self._get_name(self._debug_info.dies[cu_base + sibling_offset])

        assert False, 'No method to look up name.'

    def _get_address(self, die, base):
        location = die.attributes.get('DW_AT_data_member_location')
        if location:
            return self._parse_address(base, location.value)

        sibling = die.attributes.get('DW_AT_sibling')
        if sibling:
            cu_base = die.cu.cu_offset
            sibling_offset = sibling.value
            return self._get_address(self._debug_info.dies[cu_base + sibling_offset], base)

        assert False, 'No method to look up address.'

    def _get_size(self, die):
        die = self._find_basic_type(die)

        if die.tag == 'DW_TAG_base_type' or die.tag == 'DW_TAG_enumeration_type':
            return die.attributes['DW_AT_byte_size'].value

        if die.tag == 'DW_TAG_pointer_type':
            return 4

        if die.tag == 'DW_TAG_array_type':
            count = self._calculate_upper_bound(die)
            elem_type = self._unwrap(die)
            return count * self._get_size(elem_type)

        if die.tag == 'DW_TAG_structure_type' or die.tag == 'DW_TAG_union_type':
            return die.attributes['DW_AT_byte_size'].value

        assert False, 'No method to find the size of a %s.' % die.tag


class Array(DwarfObject):
    def __init__(self, name, address, elemtype, upper_bound, system, debug_info):
        DwarfObject.__init__(self, system, debug_info)
        self._name = name
        self.address = address
        self._elemtype = self._find_basic_type(elemtype)
        self._upper_bound = upper_bound

    def __getitem__(self, key):
        if self._upper_bound and key > self._upper_bound:
            raise IndexError('%d exceeds array "%s" upper bound %d.' % (key, self._name, self._upper_bound))

        # if the key is '0' we don't care about the size, we already have the pointer
        # so we special case this here to let us dereference more things quickly
        offset = 0
        if key != 0:
            elemsize = self._get_size(self._elemtype)
            offset   = key * elemsize
        return self._decode('%s[%d]' % (self._name, key), self.address + offset, self._elemtype)

    def __iter__(self):
        current_elem = 0
        while not self._upper_bound or current_elem <= self._upper_bound:
            yield self[current_elem]
            current_elem += 1
        raise StopIteration

    def __len__(self):
        return 0 if self._upper_bound is None else self._upper_bound

class Structure(DwarfObject):
    def __init__(self, name, address, vartype, system, debug_info):
        DwarfObject.__init__(self, system, debug_info)
        self._name = name
        self.address = address
        self._vartype = vartype

        self._type = vartype

    def __getattr__(self, name):
        logger.info('Attempting to parse field %s.%s...' % (self._name, name))

        this_type = self._find_basic_type(self._type)

        is_struct = this_type.tag == 'DW_TAG_structure_type'
        is_union  = this_type.tag == 'DW_TAG_union_type'
        is_class  = this_type.tag == 'DW_TAG_class_type'

        if is_struct or is_union or is_class:
            logger.debug('This is a structure, hunting for a member named %s' % name)
            for member in this_type.iter_children():
                if member.tag == 'DW_TAG_member':
                    if self._get_name(member) == name:
                        logger.debug('Found member %s!' % name)
                        address = self.address if is_union else self._get_address(member, self.address)
                        member_type = self._find_basic_type(member)
                        value = self._decode(name, address, member_type, member)
                        if 'DW_AT_bit_size' in member.attributes and 'DW_AT_bit_offset' in member.attributes:
                            bit_size   = member.attributes['DW_AT_bit_size'].value
                            bit_mask   = (1 << bit_size) - 1
                            bit_offset = member.attributes['DW_AT_byte_size'].value *8 - member.attributes['DW_AT_bit_offset'].value
                            value = (value >> (bit_offset - bit_size))  & bit_mask
                        return value

        print 'unknown top level or no such member'
        #import pdb; pdb.set_trace()

    def members(self):
        this_type = self._find_basic_type(self._type)
        is_struct = this_type.tag == 'DW_TAG_structure_type'
        is_union  = this_type.tag == 'DW_TAG_union_type'

        for member in this_type.iter_children():
            if member.tag == 'DW_TAG_member':
                name = self._get_name(member)
                address = self.address if is_union else self._get_address(member, self.address)
                member_type = self._find_basic_type(member)
                value = self._decode(name, address, member_type, member)
                if 'DW_AT_bit_size' in member.attributes and 'DW_AT_bit_offset' in member.attributes:
                    total_width = 8 * member.attributes['DW_AT_byte_size'].value
                    bit_size    = member.attributes['DW_AT_bit_size'].value
                    bit_mask    = (1 << bit_size) - 1
                    bit_offset  = member.attributes['DW_AT_bit_offset'].value
                    value = ((value << bit_offset) >> (total_width - bit_size)) & bit_mask
                yield (name, value)


# Pointers in C are frustrating--without external information, you cannot tell
# the difference between a pointer and an Array.
#
# So, this class is both.  Access a member directly and we'll assume it's a
# pointer to a structure and try to dereference into it.  Access via an index
# and we'll assume its an array and pass the access along appropriately.
class Pointer(Array):
    def __init__(self, name, pointer, pointee_type, system, debug_info):
        Array.__init__(self, name, pointer, pointee_type, None, system, debug_info)
        self.pointer = pointer
        self.pointee_type = pointee_type

    def __getattr__(self, name):
        return getattr(self[0], name)

