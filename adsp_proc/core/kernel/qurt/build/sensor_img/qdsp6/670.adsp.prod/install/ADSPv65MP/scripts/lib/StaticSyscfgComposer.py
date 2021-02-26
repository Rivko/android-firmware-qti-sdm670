#=============================================================================
# GENERAL DESCRIPTION
#     Build-time SYSCFG composer. It composes parts of SYSCFG that typically
#     never changes at run-time
#
# EXTERNAL FUNCTIONS/CLASSES
#        StaticSyscfgComposer
#
# INITIALIZATION AND SEQUENCING REQUIREMENTS
#        Intended to be invoked from QuRT configurator
#
#   Copyright 2017  by Qualcomm Technologies, Inc.  All Rights Reserved.
#=============================================================================
import re
import types
import xml.etree.ElementTree

class StaticSyscfgComposer:
    """Class that handles static (i.e build-time) composition of SYSCFG
       from configuration XML
    """
    FIELD_INFO_IDX_PRESENCE   = 0
    FIELD_INFO_IDX_BIT        = 1 
    FIELD_INFO_IDX_LEN        = 2
    FIELD_INFO_IDX_XPATH      = 3
    FIELD_INFO_IDX_ATTR_NAME  = 4
    FIELD_INFO_IDX_ATTR_TYPE  = 5
    FIELD_INFO_IDX_DFLT_VALUE = 6

    REQUIRED = True
    OPTIONAL = False

    # Default Q6 architecture to be used if build environment supplied one is
    # unsupported
    DEFAULT_Q6_ARCH_STR = 'v65'

    def __get_syscfg_field_info (self, q6_arch_str):
        """Given Q6 architecture in string form, return SYSCFG field info
           On success a dictionary of various SYSCFG fields along with
           information about each is returned. On failure, None is returned
        """
        fields = None
        if q6_arch_str in self.___fields:
            # Exact match for Q6 revision
            fields = self.___fields[ q6_arch_str ]
        else:
            # Find a match for architecture family
            for rev in self.___fields.keys():
                pattern = re.compile( rev )
                if pattern:
                    match = pattern.match( q6_arch_str )
                    if match:
                        fields = self.___fields[ rev ]
                        break
        return fields
    def __init__ (self, cfg):
        """Constructor
        """
        # Statically (build-time) configured fields of SYSCFG across Q6
        # architectures. Key can be a specific architecture like v66 or
        # a regular expression for a family like v5d\? that can cover
        # v5, v55 etc... Value is a dictionary of SYSCFG fields applicable
        # to that architecture.
        # @note MAINTENANCE If highest Q6 architecture is greater than
        # DEFAULT_Q6_ARCH_STR, update it and update the regex keys to cover it 
        self.___fields = {
            # v5 family
            'v5\d?' : {
                # Field       XML existence,  Bits, XPath to the XML *node*,             Attribute, Type,           Default
                'bq'      : ( self.OPTIONAL, 13, 1, './machine/bus_resources_shared',    'enabled', bool(),         True  ),
                'dmt'     : ( self.OPTIONAL, 15, 1, './machine/dynamic_multi_threading', 'enabled', bool(),         True  ),
                'l2cfg'   : ( self.REQUIRED, 18, 3, './machine/cache/l2_size',           'value',   lambda x: None, 4     ),
                'l2nwa'   : ( self.OPTIONAL, 21, 1, './machine/cache/l2_write_allocate', 'enabled', lambda x: None, False ),
                'l2wb'    : ( self.OPTIONAL, 23, 1, './machine/cache/l2_writeback',      'enabled', bool(),         False ),
                'l1dp'    : ( self.OPTIONAL, 26, 2, './machine/cache/l1_dpartition',     'main',    lambda x: None, 0     ),
                'l1ip'    : ( self.OPTIONAL, 28, 2, './machine/cache/l1_ipartition',     'main',    lambda x: None, 0     ),
                'l2part'  : ( self.OPTIONAL, 30, 2, './machine/cache/l2_partition',      'main',    lambda x: None, 0     )
            },

            # v6 family
            'v6\d+' : {
                'bq'      : ( self.OPTIONAL, 13, 1, './machine/bus_resources_shared',    'enabled', bool(),         True  ),
                'dmt'     : ( self.OPTIONAL, 15, 1, './machine/dynamic_multi_threading', 'enabled', bool(),         True  ),
                'l2cfg'   : ( self.REQUIRED, 18, 3, './machine/cache/l2_size',           'value',   lambda x: None, 4     ),
                'l2nwa'   : ( self.OPTIONAL, 21, 1, './machine/cache/l2_write_allocate', 'enabled', lambda x: None, False ),
                'l2wb'    : ( self.OPTIONAL, 23, 1, './machine/cache/l2_writeback',      'enabled', bool(),         False ),
                'slv0ctl' : ( self.OPTIONAL, 26, 2, './machine/slave0_control',          'value',   int(),          0     ),
                'slv1ctl' : ( self.OPTIONAL, 28, 2, './machine/slave1_control',          'value',   int(),          0     ),
                'l2part'  : ( self.OPTIONAL, 30, 2, './machine/cache/l2_partition',      'main',    lambda x: None, 0     )
            }
        } 

        # cfg.asic is parsed from build params and is expected to be of the
        # form v$Q6_REV; where Q6_REV is 5, 56, 60 etc...
        self.Q6_ARCH_STR = ( cfg.asic )
        fields = self.__get_syscfg_field_info( cfg.asic )
        if not fields:
            # Try with default Q6 architecture
            self.Q6_ARCH_STR = ( self.DEFAULT_Q6_ARCH_STR )
            fields = self.__get_syscfg_field_info( self.DEFAULT_Q6_ARCH_STR )
        
        if not fields:
            # If you see this, ensure that DEFAULT_Q6_ARCH_STR matches one of
            # the regex keys in self.___fields[]
            raise ValueError, '%s: error: Unrecognized Q6 architecture "%s"' % ( __name__, cfg.asic )

        self.__static_value = 0
        self.__static_value_mask = 0;

        xmltree = xml.etree.ElementTree.parse( cfg.spec_file )
        for field, info in fields.items():
            nodes = xmltree.findall( info[ self.FIELD_INFO_IDX_XPATH ] )
            if nodes:
                # @warning check for types.BooleanType must be *BEFORE* types.IntType
                # as instances of types.BooleanType are also instances of types.IntType
                if isinstance( info[ self.FIELD_INFO_IDX_ATTR_TYPE ], types.BooleanType ):
                    value = nodes[ 0 ].attrib[ info[ self.FIELD_INFO_IDX_ATTR_NAME ] ] == "true"
                elif isinstance( info[ self.FIELD_INFO_IDX_ATTR_TYPE ], types.IntType ):
                    value = int( nodes[ 0 ].attrib[ info[ self.FIELD_INFO_IDX_ATTR_NAME ] ], 0 )
                elif isinstance( info[ self.FIELD_INFO_IDX_ATTR_TYPE ], types.FunctionType ):
                    try:
                        method_name = 'parse_%s' % field
                        method = getattr( self, method_name )
                    except AttributeError:
                        raise NotImplementedError, '%s' % method_name

                    value = method( nodes[ 0 ].attrib[ info[ self.FIELD_INFO_IDX_ATTR_NAME ] ] )
            else:
                if info[ self.FIELD_INFO_IDX_PRESENCE ] == self.REQUIRED:
                    raise ValueError, '%s: error: failed to find configuration for %s of SYSCFG' % ( __name__, field )
                else:
                    # print '%s: warning: no initial value for %s in SYSCFG configured, using default' % ( __name__, field )
                    value = int( info[ self.FIELD_INFO_IDX_DFLT_VALUE ] )

            position = info[ self.FIELD_INFO_IDX_BIT ]
            length   = info[ self.FIELD_INFO_IDX_LEN ]
            shift    = position - length + 1
            mask     = 2**length - 1

            self.__static_value |= ( ( value & mask ) << shift )
            self.__static_value_mask |= ( mask << shift )


    def parse_l1_cache_partition (self, value_str):
        """Common method to parse L1 cache partitioning configuration
        """
        L1_CACHE_PARTITION_ENCODING = {
            'full'          : 0,
            'half'          : 1,
            'three_quarter' : 2,
        }

        try:
            encoding = L1_CACHE_PARTITION_ENCODING[ value_str ]
        except KeyError:
            raise ValueError, 'Unexpected value of %s for L1 cache partitioning. Use one from %s' % ( value_str, sorted( L1_CACHE_PARTITION_ENCODING.keys() ) )

        return encoding

    def parse_l1dp (self, value_str):
        """Parse L1 D$ partitioning configuration
        """
        return self.parse_l1_cache_partition( value_str )

    def parse_l1ip (self, value_str):
        """Parse L1 I$ partitioning configuration
        """
        return self.parse_l1_cache_partition( value_str )

    def parse_l2cfg (self, value_str):
        """Parse L2 size and encode it in L2CFG field
        """
        L2CFG_ENCODING = {
              '0' : 0, # No L2
            '128' : 2, # 128 KiB
            '256' : 3, # 256 KiB
            '512' : 4, # 512 KiB
           '1024' : 5, #   1 MiB
           'full' : 0, # Use full L2 (based on REV.L2)
        }

        try:
            # Value (from XML) is expected to be one of the keys in L2CFG_ENCODING
            encoding = L2CFG_ENCODING[ value_str.strip() ]
        except KeyError:
            raise ValueError, 'Unexpected value of "%s" for L2 cache size. Use one from %s' % ( value_str, sorted( L2CFG_ENCODING.keys() ) )

        return encoding

    def parse_l2nwa (self, value_str):
        """Parse L2 write-allocate configuration. Note that value in SYSCFG
           is inverse/negative while in XML it is positive. For example, for
           <l2_write_allocate enabled="true"/>, SYSCFG.L2NWA must be 0 (zero)
           We therefore invert the translation of XML value to SYSCFG value
        """
        L2NWA_ENCODING = {
             "true" : 0,
            "false" : 1
        }
        try:
            encoding = L2NWA_ENCODING[ value_str.strip().lower() ]
        except KeyError:
            raise ValueError, 'Unexpected value of "%s" for L2 write-allocate. Use one from %s' % ( value_str, sorted( L2NWA_ENCODING.keys() ) )

        return encoding

    def parse_l2part (self, value_str):
        """Parse L2$ parititioning configuration
        """
        L2PART_ENCODING = {
            'full'          : 0,
            'half'          : 1,
            'three_quarter' : 2,
            'seven_eighths' : 3
        }

        try:
            l2_partition = L2PART_ENCODING[ value_str ]
        except KeyError:
            raise ValueError, 'Unexpected value of "%s" for L2 partition. Use one from %s' % ( value_str, sorted( L2PART_ENCODING ) )
        return l2_partition

    def get_value (self):
        """Return value of statically (build-time) composed SYSCFG
        """
        return self.__static_value

    def get_mask (self):
        """Return mask of bits in SYSCFG that are statically composed
        """
        return self.__static_value_mask
