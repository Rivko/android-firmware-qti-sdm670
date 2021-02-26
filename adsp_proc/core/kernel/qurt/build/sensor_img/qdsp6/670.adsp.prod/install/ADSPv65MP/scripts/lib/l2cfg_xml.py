#=============================================================================
# GENERAL DESCRIPTION
#     L2 configuration register file. Used for build-time configuration of
#     L2 registers
#
# EXTERNAL FUNCTIONS/CLASSES
#        l2cfg
#
# INITIALIZATION AND SEQUENCING REQUIREMENTS
#        Intended to be invoked from QuRT configurator
#
#   Copyright 2017  by Qualcomm Technologies, Inc.  All Rights Reserved.
#=============================================================================
from ezxml import ParsedElement

class access:
    """Class encapsulating access/permissions
    """
    W = 1 << 0
    R = 1 << 1
    RW = W | R

    # Possible values for register/@access and its binary encoding
    ENCODING = {
         'r' : R,
         'w' : W,
        'rw' : R | W,
    }

    def __init__(self, value):
        """Constructor. Constructs an access from either a string or integer
           (mask)
        """
        if isinstance( value, str ):
            self.__value = access._from_string( value )
        elif isinstance( value, int ):
            self.__value = access._from_int( value )
        else:
            # Usage error
            raise ValueError, '%s: value must be either of type str or int' % __name__

    @staticmethod
    def _from_string (access_str):
        """Parse register/@access and return its binary encoding
        """
        try:
            encoding = access.ENCODING[ access_str ]
        except KeyError, e:
            raise ValueError, '%s: invalid value of "%s" for access. Use one from %s' % ( __name__, access_str, sorted( ENCODING.keys() ) )

        return encoding

    @staticmethod
    def _from_int (access_value):
        if access_value > access.RW:
            raise ValueError, '%s: out-of-range value of 0x%X for access' % ( __name__, access_value )
        return access_value

    def __str__ (self):
        """Convert access value to string
        """
        for string, value in access.ENCODING.iteritems():
            if value == self.__value:
                return string
        raise ValueError, '%s: invalid value of 0x%0X for access' % ( __name__, self.__value )

    def value (self):
        """Return integer value of access
        """
        return self.__value

    def __eq__ (self, other):
        """Equality (==) operator
        """
        return self.value() == other.value()

    def __ne__ (self, other):
        """Equality (!=) operator
        """
        return not (self == other)

class l2cfg:
    """Class representing L2 configuration space registers
    """
    IDX_REGISTER_NAME   = 0
    IDX_REGISTER_ACCESS = 1

    INVALID_OFFSET = -1

    # Maximum size (bytes) of the L2 configuration register file
    REGISTER_FILE_MAX_SIZE = 64 * 1024

    # @warning Atleast Python v2.5 expected
    IS_VALID_OFFSET = staticmethod(lambda offset: True if offset >= 0 else False)

    # Convenient short-form for various accesses
    R = access( access.R )
    W = access( access.W )

    # List of *NAMED* registers whose default values can be configured via QuRT
    # configuration XML. Only these registers can be configured using their
    # name. If both NAME and OFFSET are specified, the pair will be validated
    # using this table. Value for each offset is a tuple of the following...
    # - name
    #       Register name from System-Level Specification
    # - access
    #       Constraint imposed on API access. For example, if a register is
    #       readonly, user cannot change its permission to 'rw' in XML. Doing
    #       so will result in a build-time error
    #   
    SUPPORTED_NAMED_REGS = {
        # off      name,                      access
        0x0000 : ( 'SYNDROME1',                      ),
        0x0004 : ( 'SYNDROME2',                      ),
        0x0008 : ( 'SYNDROME3',                      ),
        0x000C : ( 'SYNDROME4',                      ),
        0x0010 : ( 'BLC_EN',                         ),
        0x0014 : ( 'BLC_ADDR',                       ),
        0x0018 : ( 'BLC_ADDRMASK',                   ),
        0x001C : ( 'BLC_TXNCNT',                     ),
        0x0020 : ( 'BLC_LATCNT0',                    ),
        0x0024 : ( 'BLC_LATCNT1',                    ),
        0x0028 : ( 'COPROC_EGY_CFG',                 ),
        0x002C : ( 'COPROC_EGY_WEIGHTS',             ),
        0x0030 : ( 'SFC_CTRL',                 R     ),
        0x0050 : ( 'CCCC_MAPPING_1_0',         R     ),
        0x0054 : ( 'CCCC_MAPPING_3_2',         R     ),
        0x0058 : ( 'CCCC_MAPPING_5_4',         R     ),
        0x005C : ( 'CCCC_MAPPING_7_6',         R     ),
        0x0060 : ( 'CCCC_MAPPING_9_8',         R     ),
        0x0064 : ( 'CCCC_MAPPING_B_A',         R     ),
        0x0068 : ( 'CCCC_MAPPING_D_C',         R     ),
        0x006C : ( 'CCCC_MAPPING_F_E',         R     ),
        0x0100 : ( 'QOS_MODE',                       ),
        0x0104 : ( 'QOS_MAX_TRANS',                  ),
        0x0108 : ( 'QOS_ISSUE_RATE',                 ),
        0x010C : ( 'QOS_DANGER_ISSUE_RATE',          ),
        0x0110 : ( 'QOS_SCOREBOARD_WATERMARK',       ),
        0x0114 : ( 'QOS_SYS_PRI',                    ),
        0x0118 : ( 'QOS_SLAVE',                      ),
        0x1000 : ( 'QOS_DANGER_0',                   ),
        0x2000 : ( 'QOS_DANGER_1',                   ),
        0x3000 : ( 'QOS_DANGER_2',                   ),
        0x4000 : ( 'QOS_DANGER_3',                   ),
    }

    def offset_of (self, name):
        """Return offset of L2 configuration register
           :return: non-zero offset on success, l2cfg.INVALID_OFFSET if not found
        """
        for offset, info in self.SUPPORTED_NAMED_REGS.iteritems():
            if info[ self.IDX_REGISTER_NAME ] == name.upper():
                return offset
        return self.INVALID_OFFSET

    def from_ezxml (self, l2_cfg_el):
        """Parse from EzXML instance representing <l2_cfg/> XML element
        """
        registers = l2_cfg_el.find_children( 'register' )
        for register in registers:
            name = None
            derived_offset = self.INVALID_OFFSET

            try:
                name = register.name
                derived_offset = self.offset_of( name )
            except AttributeError:
                # Register name is not specified
                pass

            try:
                offset_str = register.offset
                try:
                    offset = int( offset_str, 0 )
                except ValueError:
                    raise ValueError, '%s: invalid value "%s" for offset' % ( __name__, offset_str )

                if name and derived_offset != offset:
                    raise ValueError, '%s: offset "%s" for register "%s" not as per architecture specification' % ( __name__, offset_str, name )

                if offset < 0 or offset >= l2cfg.REGISTER_FILE_MAX_SIZE:
                    raise ValueError, '%s: offset "%s" is out of bounds' % ( __name__, offset_str )
            except AttributeError:
                if l2cfg.IS_VALID_OFFSET( derived_offset ):
                    # offset attribute is absent. Since name is present and
                    # offet derived from it is valid, use that
                    offset = derived_offset

                    offset_str = str( derived_offset )
                else:
                    # Both offset AND name are absent!
                    raise ValueError, '%s: either offset or name *MUST* be specified' % ( __name__ )

            if offset != 0 and offset % 4:
                raise ValueError, '%s: invalid offset "%s". *MUST* be multiple of 4' % ( __name__, offset_str )

            try:
                acc = access( register.access )
            except AttributeError:
                # By default, every register is readable
                acc = access( 'r' )

            try:
                restricted_access = self.SUPPORTED_NAMED_REGS[ offset ][ l2cfg.IDX_REGISTER_ACCESS ]
                if restricted_access != acc:
                    raise ValueError, '%s: register at 0x%04X has HW constraints. Its access must be "%s"' % ( __name__, offset, restricted_access )

            except ( KeyError, IndexError ) as e:
                # Unsupported offet (KeyError) or no restriction imposed (IndexError)
                pass

            # Extract the value this register must be configured to
            try:
                value = register.value
            except AttributeError:
                raise ValueError, '%s: no register value for offset 0x%04X' % ( __name__, offset )

            if offset in self.__registers:
                raise ValueError, '%s: multiple assignment to register at offset "%s"' % ( __name__, offset_str )

            # Stash the offset:value pair
            self.__registers[ offset ] = ( value, acc )

    def get_access (self, offset):
        """Get access permissions for register at the specified offset
        """
        try:
            value, access = self.__registers[ offset ]
        except KeyError, e:
            raise ValueError, 'Unknown offset 0x%04x' % offset
        return access.value()

    def __del__ (self):
        """Destructor
        """
        pass

    def __getitem__ (self, key):
        """Get value of register at offset

           Parameters
           ----------
           key : int
               Offset of L2 configuration register

           Return
           ------
           Value of register at offset
        """
        value, unused = self.__registers[ key ]
        return value

    def __init__ (self):
        """Constructor
        """
        self.__registers = {}

    def __iter__ (self):
        self.__iterator_idx = 0
        self.__iterables    = self.__registers.keys()
        return self

    def __len__ (self):
        return len( self.__registers )

    def next (self):
        if len( self.__iterables ) == 0 or self.__iterator_idx >= len( self.__iterables ):
            raise StopIteration
        else:
            offset = self.__iterables[ self.__iterator_idx ]
            value, unused  = self.__registers[ offset ]
            self.__iterator_idx += 1
            return offset

    def __setitem__ (self, key, value):
        """Assign value to key. Intentionally not implemented. Only way to assign value
           (as of now) is from XML
        """
        raise NotImplementedError

