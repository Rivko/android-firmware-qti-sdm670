#! /usr/bin/python
#=============================================================================
#
#                                    qurt-image-build1.py
#
# GENERAL DESCRIPTION
#
# EXTERNAL FUNCTIONS
#        None.
#
# INITIALIZATION AND SEQUENCING REQUIREMENTS
#        None.
#
#             Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
#=============================================================================

from lib.qurt import *
from lib.basefile import *
from StringIO import StringIO
from lib.elf_info_patch import eip_record, eip_build_record, eip_q6cfg
import lib.qurt_consts
import struct
import fnmatch

EIP_BUILD_SYMTAB = 10000
EIP_BUILD_STRTAB = 10001

class QIBPhysAddr(object):
    #
    #  QIBPhysAddr is a class for representing physical addresses known
    #   to the image builder.  Primarily, this is to unify two different
    #   types of addresses known to the builder -- absolute addresses,
    #   and relative addresses (relative to the start of the load image).
    #
    #  If relocatable is set to False, then addr is an absolute physical
    #   address.  The value of align is ignored.
    #  If relocatable is set to True, then addr is an address relative
    #   to the start of the physical load image.  The value "align" is
    #   an indicator of how much we can rely on the loader to choose
    #   an aligned physical address.  The default for "align" is 1MB,
    #   indicating that the loader will choose a load address which
    #   differs from the requested address by a multiple of 1MB.
    #
    def __init__(self, addr, relocatable=False, base=0, align=None):
        addr = int(addr)
        relocatable = bool(relocatable)
        base = int(base)
        if align == None:
            align = (0,0x100000)[relocatable]
        self.iaddr = addr
        self.irelocatable = relocatable
        self.ibase = base
        self.ialign = int(align)
    def __add__(self, delta):
        if delta == 0:
            return self
        return QIBPhysAddr(self.iaddr + delta, self.irelocatable, self.ibase, self.ialign)
    def __sub__(self, other):
        if isinstance(other, QIBPhysAddr):
            if self.irelocatable != other.irelocatable:
                raise Exception('Internal error -- cannot take difference of relocatable and non-relocatable addresses')
            return self.iaddr - other.iaddr
        return self + (-other)
    def __int__(self):
        return self.iaddr
    def __cmp__(self,other):
        #
        #  Pretend that all relocatable addresses are greater than all absolute addresses
        #
        if self.irelocatable != other.irelocatable:
            return self.irelocatable - other.irelocatable
        else:
            return self.iaddr - other.iaddr
    def basevalue(self):
        return self.iaddr+self.ibase
    def align_up_to_vaddr(self, vaddr, alignment=1):
        #
        #  Return the result of aligning the current address up to the
        #   next address which is congruent to (vaddr) modulo (alignment).
        #
        #  Check if the requested alignment is more strict than we
        #   can guarantee due to relocation uncertainty.
        #
        if (alignment-1) & ~(self.ialign-1):
            raise Exception('Requested alignment of %u is more strict than loader alignment of %u' %
                            (alignment, self.ialign))
        return self + ((vaddr-self.iaddr) & (alignment-1))

class MergeException(Exception):
    pass

class ConfigField(object):
    #
    #  ConfigField represents a base class for implementing things like memory
    #   mapping attributes.  A ConfigField object can have a value of DontCare,
    #   or it can have a defined value.
    #
    #  Two objects of ConfigField can be "anded" together using the standard
    #   "&" operator.  If the two objects are compatible, the result will be
    #   the merged value of the two input objects.  In general, we have:
    #
    #    value & value == value
    #    value & DontCare == value
    #    DontCare & DontCare == value
    #    value1 & value2 == raises a ValueError exception if value1 and value2 not equal.
    #
    #  This is used as a shorthand for determining whether two objects are
    #   compatible, by hiding the actual combination logic in the objects.
    #
    #  Optional parameters include:
    #    default     == value returned by a DontCare ConfigField (False if not specified)
    #    description == human-readable string describing the attribute managed by
    #                   this ConfigField (empty if not specified)
    #    driver      == human-readable tuple of strings describing the entities which
    #                   requested this setting for this ConfigField (empty tuple if not specified)
    #
    #  The description and driver fields are used to build the MergeException() string
    #   which we throw when incompatible ConfigField objects are attempted to be merged.
    #
    def __init__(self, *ls, **kw):
        if len(ls) > 1:
            raise ValueError()
        self.driver = ()
        self.description = ''
        self.default = False
        self.ivalue = ls                        # ivalue is either an empty tuple or a one-valued tuple
        self.__dict__.update(kw)                # Allow overriding default
    def value(self):
        return (self.ivalue+(self.default,))[0]
    def driver_msg(self):
        return '\n'.join(['   %s' % D for D in self.driver])
    def failure(self, other):
        msg = 'ConfigField <%s> incompatible values\n%s\n%s\n%s\n%s\n' % (self.description,
                                                                          str(self), self.driver_msg(),
                                                                          str(other), other.driver_msg())
        raise MergeException(msg)
    def combine(self, other):
        self.failure(other)                     # This is only called when we couldn't do a trivial combine
    def __str__(self):
        return str(self.value())
    def __nonzero__(self):
        return bool(self.value())
    def __and__(self, other):
        if not isinstance(other, ConfigField):
            raise Exception('ConfigField merged with non-ConfigField ?')
        if not other.ivalue:
            return self
        if not self.ivalue:
            return other
        if self.ivalue == other.ivalue:
            return ConfigField(self.ivalue[0], description=self.description, driver=self.driver+other.driver)
        if self.__class__ == other.__class__:
            return self.combine(other)
        self.failure(other)

DontCare = ConfigField()

class QIBSection(Elf32SectionHeader):
    SHF_INTERNAL_ONLY      = 0x100000000                # Section should not be pushed out to the final ELF
    SHF_MAP_CAN_BE_SKIPPED = 0x200000000                # Mapping the section is optional
    SHF_TLB_MAPPING        = 0x400000000                # Section represents an actual Hexagon TLB mapping

    def __init__(self, *ls, **kw):
        self.mapping_group = 'default'
        Elf32SectionHeader.__init__(self, *ls, **kw)

    def tlb_encoding(self, is_reloc):
        #
        #  If it's a TLB mapping of the appropriate type (either
        #   relocatable or absolute), return the 8 byte string
        #   representing the binary encoding of the TLB mapping;
        #   otherwise return an empty string.
        #
        if (self.sh_flags & self.SHF_TLB_MAPPING) == 0:
            return ''
        if bool(self.physaddr.irelocatable) != bool(is_reloc):
            return ''
        import struct
        physaddr = self.physaddr
        tlb_entry  = (1 << 63)                                          # Set the valid bit
        tlb_entry |= ((self.G.value() & 1) << 62)                       # Set the global bit
        tlb_entry |= (((int(self.physaddr) >> 35) & 1) << 61)           # Set the EP bit
        tlb_entry |= ((self.A1A0.value() & 3) << 59)                    # Set the bus attribute bits
        tlb_entry |= ((self.ASID.value() & 0x7F) << 52)                 # Set the ASID field
        tlb_entry |= (((self.sh_addr >> 12) & 0xFFFFF) << 32)           # Set the VADDR field
        tlb_entry |= ((self.X.value() & 1) << 31)                       # Set the X bit
        tlb_entry |= ((self.W.value() & 1) << 30)                       # Set the W bit
        tlb_entry |= ((self.R.value() & 1) << 29)                       # Set the R bit
        tlb_entry |= ((self.U.value() & 1) << 28)                       # Set the U bit
        tlb_entry |= ((self.C.value() & 0x0F) << 24)                    # Set the C field
        tlb_entry |= (((int(self.physaddr) >> 12) & 0x7FFFFF) << 1)     # Set the PADDR field
        sz = 1
        while 0x1000*sz*sz < self.sh_size:
            sz *= 2
        tlb_entry |= sz                                                 # Set the TLB entry size
        print '%016X' % tlb_entry
        return struct.pack('<Q', tlb_entry)

    def set_mapping_attributes(self):
        #
        #  Called prior to the step where we compute mappings.
        #   This should set all attributes for the section which
        #   make a difference for mapping purposes.
        #
        if hasattr(self,'memsection'):
            self.p_minus_v = ConfigField(self.physaddr - self.sh_addr,
                                         description='V->P offset', driver=('memsection',))
            self.msvbase = ConfigField(self.sh_addr, description='Virtual Address', driver=('memsection',))
            return
        if not (self.sh_flags & self.SHF_ALLOC):
            return
        if hasattr(self,'cache_policy'):                # TBD -- finish this to support other strings.
            if self.cache_policy == '6:uncached':
                self.C = ConfigField(6, description='Cache Attribute', driver=(self.name,))
        self.G = DontCare
        self.ASID = DontCare
        self.X = DontCare
        self.W = DontCare
        self.R = ConfigField(True, description='Readable', driver=(self.name,))
        self.U = DontCare
        self.SECURE = ConfigField(False, description='Secure', driver=(self.name,))
        self.locked = ConfigField(self.root_elf, description='Locked', driver=(self.name,))
        if hasattr(self,'boot_map_asid'):
            self.U = ConfigField(True, description='User Mode', driver=(self.name,))
            self.G = ConfigField(False, description='Global', driver=(self.name,))
            self.ASID = ConfigField(self.boot_map_asid, description='ASID', driver=(self.name,))
            self.locked = ConfigField(True, description='Locked', driver=(self.name,))
        self.APPNAME = ConfigField(getattr(self,'appname','__tmp__'), description='App Name', driver=(self.name,))
        print '... %s' % self.name
        if self.sh_flags & self.SHF_EXECINSTR:
            #
            #  ELF header requests execution permission
            #  For security reasons, turn off write permission
            #
            self.X &= ConfigField(True, description='Executable', driver=(self.name,))
            self.W &= ConfigField(False, description='Writable', driver=(self.name,))
        if self.sh_flags & self.SHF_WRITE:
            #
            #  ELF header requests write permission
            #  For security reasons, turn off execution permission
            #
            self.W &= ConfigField(True, description='Writable', driver=(self.name,))
            self.X &= ConfigField(False, description='Executable', driver=(self.name,))
        if self.name.startswith('.qskernel_') or self.name == '.qskstart':
            #
            #  Sections whose name starts with .qskernel_ are mapped global
            #   with ASID 0.  They are also mapped user executable and
            #   with no write permission.
            #
            if self.sh_type == self.SHT_NOBITS:
                self.sh_flags &= ~self.SHF_EXECINSTR
            else:
                self.sh_flags |= self.SHF_EXECINSTR
            self.sh_flags &= ~self.SHF_WRITE
            self.G &= ConfigField(True, description='Global', driver=(self.name,))
            self.ASID = ConfigField(0, description='ASID', driver=(self.name,))
            self.X = ConfigField(True, description='Executable', driver=(self.name,))
            if self.name == '.qskernel_eip' or self.name == '.qskernel_exports':
                self.R = ConfigField(True, description='Readable', driver=(self.name,))
            else:
                self.R = ConfigField(False, description='Readable', driver=(self.name,))
            self.W = ConfigField(False, description='Writable', driver=(self.name,))
            self.U &= ConfigField(True, description='User Mode', driver=(self.name,))
            self.SECURE = ConfigField(True, description='Secure', driver=(self.name,))
        self.p_minus_v = ConfigField(self.physaddr - self.sh_addr,
                                     description='V->P offset', driver=('memsection',))

    def overlaps(self, other):
        if self.sh_addr + self.sh_size <= other.sh_addr:
            return False
        if other.sh_addr + other.sh_size <= self.sh_addr:
            return False
        return True

    def merge(self, other, gbounds=None):
        #
        #  Here, self is a provisional mapping in the system, and
        #   other is a defined section which might conflict or
        #   modify self.
        #
        #  If other is compatible with self, update self to
        #   reflect any changes due to other and return True.
        #  If other is not compatible with self, do not update
        #   self, and return False.
        #
        if not self.overlaps(other):
            return True
        if (other.sh_flags & other.SHF_MAP_CAN_BE_SKIPPED):
            return True
        #
        #  Find all attributes of "other" which are of
        #   type ConfigField and merge them into "self".
        #  If anything is incompatible, we will throw a
        #   ValueError; catch it and return False.
        #
        try:
            for k,v in other.__dict__.items():
                if isinstance(v,ConfigField):
                    self.__dict__[k] = self.__dict__.get(k,DontCare) & v
            if gbounds != None:
                if len(gbounds) == 0:
                    gbounds.append(other.sh_addr)
                    gbounds.append(other.sh_addr+other.sh_size-1)
                else:
                    gbounds[0] = min(gbounds[0], other.sh_addr)
                    gbounds[1] = max(gbounds[1], other.sh_addr+other.sh_size-1)
            return True
        except ValueError:
            return False

    def setname(self, strtab):
        self.name = strtab[self.sh_name:].split('\0')[0]
        if self.name == '.qstart':
            self.name = '.start'
        if self.name == '.shadow.srm.build.VSPACE':
            self.sh_flags |= self.SHF_INTERNAL_ONLY
            self.sh_flags |= self.SHF_MAP_CAN_BE_SKIPPED

    def fixup(self):
        #
        # For compatibility with older builds, handle
        #  certain names with special treatment.
        #
        if self.name.startswith('.ukernel.') or self.name == '.start':
            self.sh_flags &= ~self.SHF_WRITE

    def setSRM(self, asid):
        self.boot_map_asid = asid

    def setroot(self, isroot):
        self.root_elf = isroot
        if not isroot:
            self.name = '%s.%s' % (self.name, self.appname)
        if self.isalloc() and self.sh_type != self.SHT_NOBITS and self.sh_type != self.SHT_PROGBITS:
            if isroot:
                self.name = '.x%s' % self.name
            self.sh_type = self.SHT_PROGBITS
            self.sh_link = self.SHN_UNDEF
            self.sh_info = 0
            self.sh_entsize = 0

    def isalloc(self):
        #
        # Return TRUE if the section occupies address space.
        # This means that SHF_ALLOC is set, and that the size is non-zero.
        #
        if self.name == '.qskernel_vspace' or self.name == '.qsvspace' or self.name == '.qskernel_eip_build':
            #
            # This is a special case.  We don't propagate this section through to
            #  the output file or allocate physical space for it.
            #
            return False
        return ((self.sh_flags & self.SHF_ALLOC) != 0) and (self.sh_size != 0)

    def __getitem__(self,key):
        #
        # key[0] will be an address and key[1] will be a length.
        # Return as much of the beginning of the described address range
        #  as possible, or return an empty string.
        #
        if self.isalloc() and key[1] and hasattr(self,'f'):
            offset = key[0] - self.sh_addr
            if offset < self.sh_size:
                f = self.contents()
                f.seek(offset)
                return f.read(key[1])           # The subfile will truncate at the end of the section
        return ''

class Elf_File(object):
    def __init__(self, cfg, appname, filename, restartable):
        self.cfg = cfg
        self.filename = filename
        self.appname = appname
        self.restart_copies = []
        f = open(filename, 'rb')
        self.f = f
        eh = Elf32Header(f)
        if not hasattr(cfg, 'ehdr'):
            cfg.ehdr = eh
        self.program_entry_point = eh.e_entry
        f.seek(eh.e_phoff)
        cfg.phdrs_original.extend([Elf32ProgramHeader(f) for _ in range(eh.e_phnum)])
        f.seek(eh.e_shoff)
        sections = [QIBSection(f, f=f, f_orig=f, cfg=cfg, appname=appname, restartable=restartable, rc=self.restart_copies) for _ in range(eh.e_shnum)]
        strtab = sections[eh.e_shstrndx].contents().read()
        [s.setname(strtab) for s in sections]
        [s.fixup() for s in sections]
        [s.setroot(not cfg.alloc_sections) for s in sections]
        if appname == 'SRM':
            self.SRM_ASID = len(cfg.alloc_sections)
            self.SRM_ENTRY = eh.e_entry
            [s.setSRM(self.SRM_ASID) for s in sections]
            cfg.private_asids.append(self.SRM_ASID)
        cfg.section_list.extend(sections)
        cfg.alloc_sections.append([s for s in sections if s.isalloc()])

def parse_build_elements(cfg, els):
    #
    #  parse_build_elements() must be a top level function in this file;
    #   the qurt_config.py script finds it and imports it.
    #
    #  This function is called with a list of XML Element nodes and is
    #   expected to parse the nodes appropriately and update the provided
    #   cfg structure with a parsed representation of the nodes.  It is
    #   expected that other code elsewhere will know what to do with the
    #   information attached to the cfg structure.
    #
    #  This function is called once by qurt_config.py and again by
    #   qurt-image-build.py.  It should perform the same parsing and
    #   updates in both cases, although not all of the information
    #   will be needed in both passes.
    #
    def BadBuildElem(s):
        raise Exception('Failure in <build> configuration: %s' % s)

    class generic_xml:
        def __init__(self, cfg):
            setattr(cfg, self.__class__.__name__, self)
            for k,v in self.defaults:
                setattr(self, k, v)
        def add_attr(self, k, v):
            setattr(self, k, v)
        def finalize(self):
            pass
        force_init = True
        defaults = []

    # For each "simple" XML element, define a class with the same name
    #  as the XML element, have it inherit from generic_xml, and set
    #  the class variable "defaults" to a list of default attributes.

    class max_elf_tlb(generic_xml):
        defaults = [('value', 0x100000)]
        def finalize(self):
            self.value = int(self.value,0)
    class relocatable_image(generic_xml):
        defaults = [('value', True), ('align', 0x100)]                  # Default:  True, 1MB alignment
        def finalize(self):
            if self.value.lower() == 'false':
                (self.value, self.align) = (False, 0x1000)              # False, 16MB alignment
            elif self.value.lower() == 'true':
                (self.value, self.align) = (True, 0x100)                # True, 1MB alignment
            else:
                (self.value, tmp) = (True, convert_size(self.value))
                if tmp < 0x1000 or (tmp & (tmp-1)) != 0:
                    raise Exception('relocatable_image value must be a power of two >= 4K')
                # Convert self.align to a number of 4K pages which can be represented in a TLB entry
                self.align = 0x1000
                while (self.align << 12) > tmp:
                    self.align /= 4
    class tlb_limit(generic_xml):
        defaults = [('value', 10000)]
        def finalize(self):
            self.value = int(self.value)
    class altscript(generic_xml):
        defaults = [('value', '')]
    class use_phdrs(generic_xml):
        defaults = [('value', 0)]
        def finalize(self):
            self.value = int(self.value,0)
    class gap_reclaim(generic_xml):
        defaults = [('value', 'normal')]
    class tlbdump_usage(generic_xml):
        defaults = [('value', 'normal')]
    class mapping_method(generic_xml):
        defaults = [('value', 'normal')]
    class allow_map_wx(generic_xml):
        defaults = [('value', 'false')]

    class export_symbol(generic_xml):
        def __init__(self,cfg):
            #  Get the list that we'll add symbols to
            #  It will be accessible as cfg.export_symbol
            #  Create it if it doesn't exist yet
            self.symbol_list = cfg.__dict__.setdefault('export_symbol',list())
        def add_attr(self, k, v):
            if k == 'name':
                self.symbol_list.extend(v.split())
            else:
                BadBuildElem('export_symbol only supports the "name" attribute')
        def finalize(self):
            #
            #  Actually push a synthetic symbol list into the source code.
            #
            #  What we do here is construct the .symtab and .strtab sections
            #   in binary ready format, and push them into EIP build records.
            #
            #  The .symtab section is built as a record of type EIP_BUILD_SYMTAB,
            #   and the .strtab section is built as a record of type EIP_BUILD_STRTAB.
            #
            #  After linking, when we build the final boot image, we will use
            #   the EIP_BUILD_SYMTAB and EIP_BUILD_STRTAB records to synthesize
            #   the .symtab and .strtab sections in the boot image.
            #
            symtab_code = [
                '   .pushsection .rodata.eip_build_symtab,"a",@progbits',
                '   .align 4',
                '   .short 99f-.',
                '   .short %u' % EIP_BUILD_SYMTAB,
                '   .long 0,0,0,0'
                ]
            strtab_code = [
                '   .pushsection .rodata.eip_build_strtab,"a",@progbits',
                '   .align 4',
                '   .short 99f-.',
                '   .short %u' % EIP_BUILD_STRTAB,
                '   .byte 0'
                ]
            strloc = 1
            for NAME in sorted(set(self.symbol_list)):
                #
                #  Symbol table entry, from ELF Format:
                #    4 byte index into string table pointing at name
                #    4 byte address
                #    4 byte size (we encode zero, since we don't know sizes)
                #    1 byte info (we encode 0x10 = STB_GLOBAL and STT_NOTYPE)
                #    1 byte other (zero)
                #    2 byte section header index (we encode 0xFFF1 = SHN_ABS)
                #
                symtab_code.append('   .long %u' % strloc)
                symtab_code.append('   .weak %s' % NAME)
                symtab_code.append('   .long %s' % NAME)
                symtab_code.append('   .long 0')
                symtab_code.append('   .byte 0x10')
                symtab_code.append('   .byte 0')
                symtab_code.append('   .short 0xFFF1')
                strtab_code.append('   .ascii "%s"' % NAME)
                strtab_code.append('   .byte 0')
                strloc += len(NAME)+1
            symtab_code.append('   .align 4')
            symtab_code.append('99:')
            symtab_code.append('   .popsection')
            strtab_code.append('   .align 4')
            strtab_code.append('99:')
            strtab_code.append('   .popsection')
            cfg.symtab_code = asm_to_C(symtab_code)
            cfg.strtab_code = asm_to_C(strtab_code)

    class section:
        def __init__(self,cfg):
            self.match = lambda x, sname: (sname==None)
            self.attrs = {}
            cfg.section_info.append(self)
        def finalize(self):
            # Allow all section elements and attributes for now.
            # In the future, check here for conflicting attributes,
            #  illegal attributes, etc.
            pass
        def add_attr(self,k,v):
            from re import compile
            if k == 'tlb_lock' and v == '1':
                self.attrs['TLBLOCK'] = lambda s: setattr(s,'locked',True)
                self.attrs['TLBLOCK'] = lambda s: setattr(s,'reclaim',False)
            elif k == 'tlb_lock' and v == '0':
                self.attrs['TLBLOCK'] = lambda s: setattr(s,'locked',False)
                self.attrs['TLBLOCK'] = lambda s: setattr(s,'reclaim',False)
            elif k == 'tlb_lock' and v.lower() == 'root':
                self.attrs['TLBLOCK'] = lambda s: setattr(s,'locked',s.root_elf)
                self.attrs['TLBLOCK'] = lambda s: setattr(s,'reclaim',False)
            elif k == 'tlb_lock' and v.lower() == 'boot':
                self.attrs['TLBLOCK'] = lambda s: setattr(s,'locked',s.root_elf)
                self.attrs['TLBLOCK'] = lambda s: setattr(s,'reclaim',s.root_elf)
            elif k == 'mapping' and v.lower() == 'none':
                self.attrs['MAPPING'] = lambda s: setattr(s,'mapping','none')
                self.attrs['PERMS'] = lambda s: setattr(s,'sh_flags',s.sh_flags & ~(s.SHF_WRITE | s.SHF_EXECINSTR))
                self.attrs['CACHE'] = lambda s: setattr(s,'cache_policy','-1')
            elif k == 'mapping' and v.lower() == 'r':
                self.attrs['MAPPING'] = lambda s: setattr(s,'mapping','r')
                self.attrs['PERMS'] = lambda s: setattr(s,'sh_flags',s.sh_flags & ~(s.SHF_WRITE | s.SHF_EXECINSTR))
            elif k == 'mapping' and v.lower() == 'rx':
                self.attrs['MAPPING'] = lambda s: setattr(s,'mapping','rx')
                self.attrs['PERMS'] = lambda s: setattr(s,'sh_flags',(s.sh_flags | s.SHF_EXECINSTR) & ~s.SHF_WRITE)
            elif k == 'mapping' and v.lower() == 'rw':
                self.attrs['MAPPING'] = lambda s: setattr(s,'mapping','rw')
                self.attrs['PERMS'] = lambda s: setattr(s,'sh_flags',(s.sh_flags | s.SHF_WRITE) & ~s.SHF_EXECINSTR)
            elif k == 'mapping' and v.lower() == 'overlay':
                self.attrs['MAPPING'] = lambda s: setattr(s,'mapping','overlay')
            elif k == 'user_mode' and v.lower() == 'false':
                self.attrs['UMODE'] = lambda s: setattr(s,'user_mode',not s.root_elf)
            elif k == 'user_mode' and v.lower() == 'true':
                self.attrs['UMODE'] = lambda s: setattr(s,'user_mode',True)
            elif k == 'cache_policy':
                self.attrs['CACHE'] = lambda s,v=v: setattr(s,'cache_policy',v)
            elif k == 'mapping_group':
                self.attrs['MAPGROUP'] = lambda s,v=v: setattr(s,'mapping_group',v)
            elif k == 'physpool':
                self.attrs['PHYSPOOL'] = lambda s,v=v: setattr(s,'physpool', v)
            elif k == 'name' and not '*' in v:
                self.name = v
                self.match = lambda x, sname: (sname== x.name)
            elif k == 'name' and '*' in v:
                tmp = v.replace('.','\\.')
                tmp = v.replace('*','.*')
                self.regex = compile(tmp + '$')
                self.match = lambda x, sname: (sname and x.regex.match(sname) != None)
            elif k == 'match':
                self.regex = compile(v + '$')
                self.match = lambda x, sname: (sname and x.regex.match(sname) != None)
            else:
                BadBuildElem('%s cannot set %s to %s' % (self,k,v))

    section_attributes = section

    cfg.section_info = []

    for k,v in locals().items():
        if hasattr(v,'force_init'):
            v(cfg)

    cfg.cachepolicydict = {}
    for oe in els:
        if oe.tagName == 'cache_policy':
            cfg.cachepolicydict[oe.getAttribute('name')] = int(oe.getAttribute('value'),0)

    cfg.program_maps = {}
    for oe in els:
        if oe.tagName == 'program_name_map':
            cfg.program_maps[oe.getAttribute('name')] = int(oe.getAttribute('number'),0)

    for oe in els:
        for ie in oe.childNodes:
            if ie.nodeType == ie.ELEMENT_NODE:
                this = locals()[ie.tagName](cfg)
                if ie.attributes:
                    for k,v in ie.attributes.items():
                        this.add_attr(k,v)
                this.finalize()

def splitname(s, restart):
    from os import path
    names = s.split('=')
    if len(names) == 1:
        return (path.basename(s).replace('elf','pbn'), s, restart)
    if len(names) == 2:
        return (names[0], names[1], restart)
    raise Exception("input file argument cannot contain multiple '=' characters")

class PhysPool:
    def __init__(self, tuple):
        self.ranges = []
        self.name = tuple[0].split('\0')[0]
        for base,size in zip(tuple[1::2],tuple[2::2]):
            if size == 0:
                break
            if (size & 0xFFF) != 0:
                raise Exception("Pool %s has size which is not 4K aligned" % self.name)
            self.ranges.append([base,size >> 12])

class QurtElfPatch:
    """Class for doing ELF patch processing on the merged ELF file."""
    #
    # When we call start() on this class, the class becomes basically like
    #  a 4GB bytearray in the ELF's virtual address space.
    # Slices of the virtual address space can be accessed using standard
    #  slice notation.  For instance, to access the 4 bytes at address ADDR
    #  you can use self[ADDR:ADDR+4].
    # To read the same 4 bytes into X as an unsigned 32-bit quantity:
    #  X = struct.unpack('<L', self[ADDR:ADDR+4])
    # To write the same 4 bytes with X which is an unsigned 32-bit quantity:
    #  self[ADDR:ADDR+4] = struct.pack('<L', X)
    #
    # Note that accesses outside of the defined virtual image of the ELF cause
    #  an exception.  Portions of the virtual image which are not backed by
    #  file data (such as bss sections, etc.) can be read, but return zeroes;
    #  attempts to write non-zero data to those cause an exception.
    #
    def __init__(self):
        self.phdrs_original = []
        self.section_list = []
        self.alloc_sections = []
        self.opt_mappings = []
        self.island_moves = []
        self.private_asids = []
        self.vfile = VAddrFile()
        self.patchable_mmaps = {}
    def __getitem__(self, key):
        from struct import unpack, calcsize
        cvt = False
        if isinstance(key,slice):
            sz = key.stop - key.start
            addr = key.start
        elif isinstance(key[1], basestring):
            sz = calcsize(key[1])
            addr = key[0]
            cvt = True
        else:
            sz = key[1]
            addr = key[0]
        self.vfile.seek(addr)
        ret = self.vfile.read(sz)
        if cvt:
            ret = unpack(key[1], ret)
        return ret
    def __setitem__(self, key, value):
        # If value is not a string, assume it's an iterable (most likely a tuple)
        #  appropriate for being passed as the argument list to struct.pack
        if not isinstance(value, basestring):
            from struct import pack
            value = pack(*value)
        sz = len(value)
        if isinstance(key,slice):
            addr = key.start
        else:
            addr = key[0]
        self.vfile.insert(addr, StringIO(value), 0, sz, new=False, overwrite=True)
    def parse_args(self,argv):
        from optparse import OptionParser, OptionGroup
        parser = OptionParser(usage='%prog [parameters] [[flags...] input_file]...')

        def add_flag(parser, key):
            if len(parser.largs) == 0 or not isinstance(parser.largs[-1],list):
                parser.largs.append(list())
            parser.largs[-1].append(key)

        def opt_callback_align(option, opt_str, value, parser):
            try:
                value = convert_size(value.upper())
            except ValueError:
                parser.error('Invalid format for --align')
            add_flag(parser, 'align')
            add_flag(parser, value)

        def opt_callback_file(option, opt_str, value, parser):
            add_flag(parser, 'ramfs')

        def opt_callback_contig(option, opt_str, value, parser):
            add_flag(parser, 'contig')

        def opt_callback_restart(option, opt_str, value, parser):
            add_flag(parser, 'restartable')

        group = OptionGroup(parser, 'Required Parameters')
        group.add_option('-o', action='store', dest='outfilename',
                         help='write output to FILENAME', metavar='FILENAME')
        group.add_option('-p', action='store', dest='physaddr',
                         help='set base physical address of output file to PHYSADDR', metavar='PHYSADDR')
        parser.add_option_group(group)

        group = OptionGroup(parser, 'Optional Parameters')
        group.add_option('-f', action='store_true', dest='no_altscript',
                         help='skip check for an alternate builder script')
        group.add_option('-n', action='store_true', dest='no_adjust',
                         help='skip writing _reloc versions of input files')
        group.add_option('--assert_free_microimage', action='store', dest='free_uimg', default='0',
                         help='fail the build if the amount of microimage free space falls below SIZE', metavar='SIZE')
        parser.add_option_group(group)

        group = OptionGroup(parser, 'Flags affecting the following input file')
        group.add_option('-R', action='callback', callback=opt_callback_restart,
                         help='mark the next input file as a restartable ELF')
        group.add_option('--file', action='callback', callback=opt_callback_file,
                         help='mark the next input file as a RAMFS file')
        group.add_option('--align', action='callback', callback=opt_callback_align, type='string',
                         help='mark the next input file as being aligned to BOUNDARY in physical memory', metavar='BOUNDARY')
        group.add_option('--contiguous', action='callback', callback=opt_callback_contig,
                         help='mark the next input file as being contiguous in physical memory')
        parser.add_option_group(group)

        group = OptionGroup(parser, 'Deprecated Parameters',
                            'These are allowed but ignored')
        group.add_option('-e', action='store_true',
                         help='request old EIP format')
        group.add_option('-t', action='store',
                         help='use Hexagon tools at TOOLSDIR', metavar='TOOLSDIR')
        parser.add_option_group(group)

        (opts, args) = parser.parse_args()

        # Put all of the arguments where they belong:

        self.outfilename = opts.outfilename
        if self.outfilename == None:
            parser.error('Must provide an output file name using -o')

        try:
            if opts.physaddr == None:
                raise ValueError()
            self.physaddr = convert_size(opts.physaddr.upper())
            if (self.physaddr & 0xFFFFF000) != self.physaddr:
                raise ValueError()
        except ValueError:
            parser.error('Must provide a valid 4K-aligned physical address using -p')

        try:
            self.free_uimg = convert_size(opts.free_uimg.upper())
        except ValueError:
            parser.error('Invalid format for --assert_free_microimage')

        self.write_reloc = not opts.no_adjust

        if len(args) == 0:
            parser.error('At least one input ELF must be provided')

        if isinstance(args[-1],list):
            parser.error('Flags found with no file name following')

        self.infiles = []
        self.ramfs = []
        flags = []
        srm = []
        for x in args:
            if isinstance(x,list):
                flags = x
            elif 'ramfs' in flags:
                self.ramfs.append((x,flags))
                flags = []
            elif x.startswith('SRM='):
                srm.append(x)
            else:
                self.infiles.append(Elf_File(self, *splitname(x, 'restartable' in flags)))
                flags = []
        for x in srm:
            self.infiles.append(Elf_File(self, *splitname(x, 'restartable' in flags)))

    def unpack_from(self, fmt, offset):
        return self[(offset,fmt)]

    def read_pool_configs(self):
        self.pools = []
        addr = self.pool_configs
        if addr == 0:
            return
        while True:
            tmp = self.unpack_from('<32s32L', addr)
            if tmp[0].startswith('\0'):
                break
            addr += 160
            self.pools.append(PhysPool(tmp))

    def set_root_prog_no(self):
        if self.QURTK_root_prog_no:
            current_value = self.unpack_from('<i', self.QURTK_root_prog_no)[0]
            if current_value < -1:
                new_value = self.program_maps.get(self.infiles[0].appname,-1)
                self[(self.QURTK_root_prog_no, 4)] = ('<i', new_value)
    def setup_elf_sections(self):
        #
        # We've collected all of the ELF sections.  Now make a list
        #  of all of those which actually take up address space,
        #  sorted by address.
        #
        self.alloc_sections_sorted = []
        for a in self.alloc_sections:
            self.alloc_sections_sorted.extend(a)
            for s in a:
                if s.sh_type == s.SHT_NOBITS:
                    f = self.vfile.insert(s.sh_addr, ZeroFile, 0, s.sh_size)
                else:
                    f = self.vfile.insert(s.sh_addr, s.f, s.sh_offset, s.sh_size)
                s.f = f
                s.sh_offset = 0
        self.alloc_sections_sorted.sort(key=lambda s:s.sh_addr)
    def srm_fixup(self):
        #
        # This step goes through all SRM alloc sections and replaces QDI trap
        #  instructions with the fixup instruction -- which should be "R0 = #-1"
        #
        for s in self.alloc_sections_sorted:
            if not hasattr(s,'boot_map_asid'):
                continue
            if (s.sh_flags & s.SHF_ALLOC) == 0:
                continue
            if (s.sh_flags & s.SHF_EXECINSTR) == 0:
                continue
            if s.sh_type == s.SHT_NOBITS:
                continue
            a = s.sh_addr
            qdi_trap = lib.qurt_consts.QDI_TRAP_ENCODING
            qdi_fixup = lib.qurt_consts.QDI_FIXUP_ENCODING
            while (a+4) <= s.sh_addr + s.sh_size:
                I = self[(a,'<L')][0]
                if I == qdi_trap:
                    print '%s %08X: Instruction is 0x%08X\n' % (s.name,a,I)
                    self[(a,4)] = ('<L', qdi_fixup)
                a+=4
    def mark_secure_programs(self):
        for progname, prog in getattr(self, 'secure_program_info', dict()).items():
            matches = [i for i in self.infiles if fnmatch.fnmatch(i.filename, prog.match)]
            if len(matches) == 0:
                print 'Warning -- no input file matches secure program %s' % prog.match
            elif len(matches) > 1:
                print 'Warning -- more than one file matches secure program %s' % prog.match
            else:
                #
                #  Found a secure program ELF file.  Mark it appropriately.
                #
                i = matches[0]
                i.SECURE_ASID = self.infiles.index(i)
                i.SECURE_SLOT = int(prog.index,0)
                i.SECURE_SID = int(prog.sid,0)
                i.SECURE_NAME = progname
                for s in self.alloc_sections_sorted:
                    if s.f_orig == i.f:
                        s.boot_map_asid = i.SECURE_ASID
                self.private_asids.append(i.SECURE_ASID)

    def extract_qurt_info(self):
        from lib.elf_info_patch import ElfInfoPatch
        section_names_to_try = ('.qskernel_eip', '.qskernel_main', '.start')
        for N in section_names_to_try:
            for s in self.alloc_sections[0]:
                if s.name == N:
                    parse_build_elements(*ElfInfoPatch().Parse(self, s.sh_addr, s.sh_size))
                    return
    def extract_patchable_mmap (self):
        for variable_vaddr, tlb_size, mmap_idx, unused in self.eip_build_records( eip_build_record.Q6CFG, 'LHH' ):
            self.patchable_mmaps[ mmap_idx ] = {}
            self.patchable_mmaps[ mmap_idx ][ 'variable_vaddr' ] = variable_vaddr
    def extract_memsections(self):
        lock_count = 0
        lockloc = getattr(self, 'QURTK_tlblock_entries', 0)
        if lockloc:
            lock_count = self.unpack_from('<L', lockloc)[0]
        print 'lock_count == %X' % lock_count
        addr = getattr(self, 'qurtos_mmap_table', 0)
        if addr:
            data, fmt = (), '<Q'
            while data[-1:] != (0,):
                data, fmt = self.unpack_from(fmt, addr), fmt+'Q'
            #
            #  Create sections for each TLB entry that we read.
            #
            for TLB in data[:-1]:
                flags = 0
                print '%016X' % TLB
                rsz = TLB & (-TLB)
                sz = 0x1000 * rsz * rsz
                vaddr = ((TLB >> 32) & 0xFFFFF) << 12
                driver = ('memsection:%08X' % (vaddr,),)
                paddr = (((TLB-rsz) >>  1) & 0x7FFFFF) << 12
                paddr |= (((TLB-rsz) >> 61) & 1) << 35

                if data.index( TLB ) in self.patchable_mmaps:
                    # Patch virtual address
                    vaddr = self.allocate_kernel_vspace( sz, sz )

                    self.patchable_mmaps[ data.index( TLB ) ][ 'vaddr' ] = vaddr

                    symbol_vaddr = self.patchable_mmaps[ data.index( TLB ) ][ 'variable_vaddr' ]
                    self[ ( symbol_vaddr, 4 ) ] = ( '<L', vaddr )

                    # This is an actual TLB mapping
                    flags = QIBSection.SHF_TLB_MAPPING

                s = QIBSection(None,
                               name = '',
                               physaddr = QIBPhysAddr(paddr),
                               G = ConfigField(bool((TLB >> 62) & 1), description='Global', driver=driver),
                               A1A0 = ConfigField(int((TLB >> 59) & 3), description='Bus Attribute', driver=driver),
                               ASID = ConfigField(int((TLB >> 52) & 0x7F), description='ASID', driver=driver),
                               X = ConfigField(bool((TLB >> 31) & 1), description='Executable', driver=driver),
                               W = ConfigField(bool((TLB >> 30) & 1), description='Writable', driver=driver),
                               R = ConfigField(bool((TLB >> 29) & 1), description='Readable', driver=driver),
                               U = ConfigField(bool((TLB >> 28) & 1), description='User Mode', driver=driver),
                               C = ConfigField(int((TLB >> 24) & 0xF), description='Cache Attribute', driver=driver),
                               memsection = True,
                               locked = ConfigField(list(data).index(TLB) < lock_count,
                                                    description='Locked', driver=driver),
                               root_elf = True,
                               sh_name = 0,
                               sh_type = QIBSection.SHT_NOBITS,
                               sh_flags = QIBSection.SHF_ALLOC + QIBSection.SHF_INTERNAL_ONLY + flags,
                               sh_addr = vaddr,
                               sh_offset = None,
                               sh_size = sz,
                               sh_link = QIBSection.SHN_UNDEF,
                               sh_info = 0,
                               sh_addralign = 0,
                               sh_entsize = 0)
                self.alloc_sections[0].append(s)
                self.alloc_sections_sorted.append(s)

                if data.index( TLB ) in self.patchable_mmaps:
                    # Update qurtos_mmap_table[] entry whose VPN was patched.
                    # @warning This is not necessary, but done for "completeness"
                    encoding = s.tlb_encoding( False )
                    tlb_entry, = struct.unpack( '<Q', encoding )
                    self[ ( addr +( data.index( TLB )*8 ), 8 ) ] = ( '<Q', tlb_entry )

                print '%X %X %X' % (s.sh_addr, int(s.physaddr), s.sh_size)
            self.alloc_sections_sorted.sort(key=lambda s:s.sh_addr)
    def extract_island_config(self):
        self.phys_island_regions = []
        addr = getattr(self, 'QURTK_phys_island', 0)
        if addr:
            data, fmt = (), '<L'
            while data[-1:] != (0xFFFFFFFF,):
                data, fmt = self.unpack_from(fmt, addr), fmt+'LLL'
            while len(data) > 1:
                region = data[:3]
                print 'Island region at %05X000 - %05XFFF, cache policy %s' % region
                self.phys_island_regions.append(list(region))
                data = data[3:]
    def apply_xml_to_sections(self):
        #
        #  To every alloc section, apply all XML section
        #   attributes which are global.
        #
        for s in self.alloc_sections_sorted:
            for o in self.section_info:
                if o.match(o,None):
                    for v in o.attrs.values():
                        v(s)
        #
        #  To every alloc section, apply all XML section
        #   attributes which match the XML section name.
        #
        for s in self.alloc_sections_sorted:
            for o in self.section_info:
                if o.match(o,s.name):
                    for v in o.attrs.values():
                        v(s)
        #
        #  A few other legacy things here to do to
        #   every alloc section...
        #
        for s in self.alloc_sections_sorted:
            mapattr = getattr(s,'mapping','normal')
            s.overlay = int(mapattr == 'overlay')

    def phdr_translate(self,addr):
        #
        #  Use the program headers to translate an address (optional mechanism)
        #
        for p in self.phdrs_original:
            if p.p_type == p.PT_LOAD:
                offset = addr - p.p_vaddr
                if offset >= 0 and offset < p.p_memsz:
                    return QIBPhysAddr(p.p_paddr + offset)
        raise Exception('Cannot translate virtual address 0x%X using program headers' % addr)

    def allocate_phys_space(self):
        #
        #  For all alloc sections, allocate physical
        #   space for them.
        #
        kernel_sections = []
        root_sections_by_name = {}
        restartable_sections = []
        physaddr_current = QIBPhysAddr(0, True, self.physaddr)         # Relocatable, start at beginning of load image
        for a in self.alloc_sections:
            for s in self.alloc_sections_sorted:
                if s in a:
                    if hasattr(s,'memsection'):
                        continue
                    if s.root_elf:
                        if s.name.startswith('.qskernel_'):
                            kernel_sections.append(s)
                            continue
                        root_sections_by_name[s.name] = s
                    else:
                        if ((s.sh_flags & s.SHF_WRITE) != 0 and
                            (s.sh_type == s.SHT_PROGBITS) and
                            (s.restartable)):
                            restartable_sections.append(s)
                    if self.use_phdrs.value:
                        s.physaddr = self.phdr_translate(s.sh_addr)
                        continue
                    s.physaddr = physaddr_current.align_up_to_vaddr(s.sh_addr, 0x100000)   # Align up to 1MB boundary
                    physaddr_current = s.physaddr + s.sh_size
        for s in kernel_sections:
            lookup_names = []
            if s.name == '.qskernel_main':
                lookup_names.append('.start')
            lookup_names.append(s.name.replace('.qskernel','.qskshadow',1))
            s.physaddr = root_sections_by_name[lookup_names[0]].physaddr
            for N in lookup_names:
                root_sections_by_name[N].sh_flags |= QIBSection.SHF_INTERNAL_ONLY
                root_sections_by_name[N].sh_flags |= QIBSection.SHF_MAP_CAN_BE_SKIPPED
        if restartable_sections:
            curoff = 0
            restart_data = []
            for s in restartable_sections:
                z = (s.sh_addr - curoff) & 7
                if z:
                    restart_data.append(StringIO('\0'*z))
                    curoff += z
                s.restart_offset = curoff
                restart_data.append(s.contents())
                curoff += s.sh_size
            restart_raw = ''.join([f.read() for f in restart_data])
            # We have a new section to add
            # Get a physical address for it by stealing from the end of DEFAULT_PHYSPOOL
            for pool in self.pools:
                if pool.name == 'DEFAULT_PHYSPOOL':
                    restart_paddr = QIBPhysAddr(0, True, self.physaddr)
                    loc = int(self.effective_memsize)
                    loc -= len(restart_raw)
                    loc -= loc & 7
                    print 'Effective memsize was %X' % int(self.effective_memsize)
                    self.effective_memsize = loc
                    print 'Effective memsize is now %X' % int(self.effective_memsize)
                    restart_paddr += loc
                    break
            else:
                raise Exception('Could not allocate physical space for restart data')
            for s in self.section_list:
                if s.name == '.qsvspace':
                    restart_vaddr = s.sh_addr + s.sh_size
                    restart_vaddr -= len(restart_raw)
                    adjust = (restart_vaddr - int(restart_paddr)) & 0xFFF
                    restart_vaddr -= adjust
                    break
            else:
                raise Exception('Could not allocate virtual addresses for restart data')
            print 'Restart goes at physaddr %X' % int(restart_paddr)
            print 'Restart goes at virtaddr %X' % int(restart_vaddr)
            driver = ('.rodata.qrestart',)
            ns = QIBSection(None,
                            name = '.rodata.qrestart',
                            physaddr = restart_paddr,
                            G = ConfigField(False, description='Global', driver=driver),
                            A1A0 = ConfigField(0, description='Bus Attribute', driver=driver),
                            ASID = ConfigField(0, description='ASID', driver=driver),
                            X = ConfigField(False, description='Executable', driver=driver),
                            W = ConfigField(False, description='Writable', driver=driver),
                            R = ConfigField(True, description='Readable', driver=driver),
                            U = ConfigField(False, description='User Mode', driver=driver),
                            C = ConfigField(7, description='Cache Attribute', driver=driver),
                            locked = ConfigField(False,description='Locked', driver=driver),
                            root_elf = True,
                            f = StringIO(restart_raw),
                            f_orig = None,
                            sh_name = 0,
                            sh_type = QIBSection.SHT_PROGBITS,
                            sh_flags = QIBSection.SHF_ALLOC,
                            sh_addr = restart_vaddr,
                            sh_offset = 0,
                            sh_size = len(restart_raw),
                            sh_link = QIBSection.SHN_UNDEF,
                            sh_info = 0,
                            sh_addralign = 0,
                            sh_entsize = 0)
            self.alloc_sections[0].append(ns)
            self.alloc_sections_sorted.append(ns)
            self.alloc_sections_sorted.sort(key=lambda s:s.sh_addr)
            for s in restartable_sections:
                s.sh_type = s.SHT_NOBITS
                s.rc.append((s.sh_addr, restart_vaddr+s.restart_offset, s.sh_size))

    def try_mapping(self,addr,sz):
        map = QIBSection(None,
                         name = '',
                         physaddr = None,
                         p_minus_v = DontCare,                  # Don't know the P->V offset yet
                         G = DontCare,                          # Don't know yet if we're global
                         A1A0 = DontCare,                       # Don't have bus attributes yet
                         ASID = DontCare,                       # Don't have an ASID yet
                         X = DontCare,                          # Don't know yet if we're executable
                         W = DontCare,                          # Don't know yet if we're write-enabled
                         R = DontCare,                          # Don't know yet if we're readable
                         U = DontCare,                          # Don't know yet if we're user-mode visible
                         C = ConfigField(default=7),            # Don't have cache attributes yet, but default to 7
                         sh_name = 0,
                         sh_type = QIBSection.SHT_NULL,
                         sh_flags = QIBSection.SHF_TLB_MAPPING,
                         sh_addr = addr & ~(sz-1),
                         sh_offset = None,
                         sh_size = sz,
                         sh_link = QIBSection.SHN_UNDEF,
                         sh_info = 0,
                         sh_addralign = 0,
                         sh_entsize = 0)

        gb=[]
        for s in self.alloc_sections_sorted:
            if not map.merge(s,gb):
                return None

        if len(gb):
            gb[0] &= ~0xFFF             # Round down to page boundary
            gb[1] |= 0xFFF              # Round up to page boundary
            if (gb[0] ^ gb[1]) < sz/4:
                return None

        map.physaddr = map.p_minus_v.value() + map.sh_addr

        if map.p_minus_v.value().irelocatable:
            #
            #  If it's relocatable, anything > 1MB won't work
            #
            if sz > 0x100000:
                return None

        if int(map.p_minus_v.value()) & (sz-1):
            #
            #  The physical and virtual offset won't work with this size
            #
            return None

        return map

    def virt_to_physload(self, addr):
        #
        #  Given a virtual address, return its assigned physical load address.
        #  May return None if the virtual address doesn't have an associated
        #   physical load address.
        #
        for s in self.alloc_sections_sorted:
            offset = addr - s.sh_addr
            if offset >= 0 and offset < s.sh_size:
                return s.physaddr + offset
        return None

    def allocate_kernel_vspace(self, size, align):
        for s in self.section_list:
            if s.name == '.qskernel_vspace':
                retaddr = alignup(s.sh_addr, align)
                endaddr = retaddr + size
                if endaddr <= s.sh_addr + s.sh_size:
                    s.sh_size = (s.sh_addr + s.sh_size) - endaddr
                    s.sh_addr = endaddr
                    return retaddr
        raise Exception('Cannot allocate 0x%X bytes of virtual address space' % size)

    def add_build_mappings(self):
        import xml.dom.minidom
        #
        # Walk for each DEFINE_MAPPING (EIP_BUILD_MAP_RANGE) record
        #
        range_mappings = []
        for addr, size, xmlstr in self.eip_build_records(3, 'LL'):
            #
            # We are requested to remap the ELF (addr,size) with an alias mapping
            #  at a newly allocated virtual address and with the xmlstr being used
            #  to override the section attributes.
            #
            if size == 0:
                continue
            virtlo = addr
            virthi = addr+(size-1)
            physlo = self.virt_to_physload(virtlo)
            physhi = self.virt_to_physload(virthi)
            tlbsize = 0x1000
            # Keep increasing tlbsize until we get the whole area covered
            #  or until we hit 1MB TLB size; don't go beyond 1MB.
            while tlbsize < 0x100000 and (int(physlo) ^ int(physhi)) >= tlbsize:
                tlbsize *= 4
            tlbmask = tlbsize-1
            range_offset = (int(physlo) & tlbmask)      # How far into the physical range do we actually start?
            physlo -= int(physlo) & tlbmask
            physhi += (int(physhi) ^ tlbmask) & tlbmask
            for s in self.alloc_sections[0]:
                if virtlo >= s.sh_addr and virthi < s.sh_addr + s.sh_size:
                    orig_section = s
                    break
            else:
                raise Exception('Cannot find original section for alias map of %X-%X' % (virtlo, virthi))
            ns = orig_section.__class__(orig_section.__dict__)
            ns.physaddr = physlo
            ns.sh_flags |= ns.SHF_INTERNAL_ONLY
            ns.sh_size = (physhi - physlo) + 1
            ns.sh_addr = self.allocate_kernel_vspace(ns.sh_size, tlbsize)
            xmlstr = xmlstr.split('\0',1)[0]
            xml_to_parse = '<build>%s</build>' % (xmlstr,)
            els = [xml.dom.minidom.parseString(xml_to_parse.decode('utf-8')).documentElement]
            overrides = (lambda: None)          # Object to hang attributes on
            parse_build_elements(overrides, els)
            for o in overrides.section_info:
                for v in o.attrs.values():
                    v(ns)
            self.alloc_sections[0].append(ns)
            self.alloc_sections_sorted.append(ns)
            self.alloc_sections_sorted.sort(key=lambda s:s.sh_addr)
            range_mappings.append((virtlo, virthi, ns.sh_addr + range_offset))
        for addr, size, data in self.eip_build_records(4, 'LL'):
            # Get the original addresses from the ELF
            original_addrs = [self.unpack_from('<L', a)[0] for a in range(addr, addr+size, 4)]
            if original_addrs:
                if max(original_addrs) == 0:
                    #
                    #  All addresses are NULL.  Do nothing.
                    #
                    continue
                #
                #  List is non-empty.  Find which range mapping to use
                #
                match = [T for T in range_mappings if original_addrs[0] >= T[0] and original_addrs[0] <= T[1]]
                if len(match) != 1:
                    raise Exception('Mapping for %X should have exactly one match; match == %s' % (original_addrs[0], match))
                adjust = match[0][2] - match[0][0]
                new_addrs = [a+adjust for a in original_addrs]
                new_addrs = ('<'+'L'*len(new_addrs),)+tuple(new_addrs)
                #
                #  Write out the modified addresses
                #
                self[(addr,size)] = new_addrs

    def set_kernel_boundaries(self):
        first_vpage = QurtInfinity
        last_vpage = -QurtInfinity
        first_ppage = QurtInfinity
        last_ppage = -QurtInfinity
        for s in self.section_list:
            if s.root_elf:
                if s.name.startswith('.qskernel_'):
                    if s.sh_flags & s.SHF_ALLOC:
                        first_vpage = min(first_vpage, s.sh_addr >> 12)
                        last_vpage = max(last_vpage, (s.sh_addr + s.sh_size - 1) >> 12)
        for s in self.alloc_sections_sorted:
            if s.root_elf:
                if s.name.startswith('.qskernel_'):
                    first_ppage = min(first_ppage, int(s.physaddr) >> 12)
                    last_ppage = max(last_ppage, (int(s.physaddr) + s.sh_size - 1) >> 12)
        if first_vpage == QurtInfinity:
            return None
        self[(self.QURTK_vprotect_start,4)] = ('<L', first_vpage)
        self[(self.QURTK_vprotect_end,4)] = ('<L', last_vpage + 1)
        self[(self.QURTK_pprotect_start,4)] = ('<L', first_ppage)
        self[(self.QURTK_pprotect_end,4)] = ('<L', last_ppage + 1)

    def set_srm_boundaries(self):
        if self.QURTK_srmimage_start == 0:
            return None
        first_ppage = QurtInfinity
        last_ppage = -QurtInfinity
        for s in self.alloc_sections_sorted:
            if hasattr(s,'boot_map_asid'):
                if s.sh_flags & s.SHF_MAP_CAN_BE_SKIPPED:
                    continue
                first_ppage = min(first_ppage, int(s.physaddr) >> 12)
                last_ppage = max(last_ppage, (int(s.physaddr) + s.sh_size - 1) >> 12)
        if first_ppage == QurtInfinity:
            return None
        self[(self.QURTK_srmimage_start,4)] = ('<L', first_ppage)
        self[(self.QURTK_srmimage_end,4)] = ('<L', last_ppage+1)
        print "SRM (start,end) is (%X,%X)" % (first_ppage, last_ppage+1)

    def optimize_mappings(self):
        #
        #  From our section list, come up with a set of optimized TLB
        #   mappings.
        #
        for s in self.alloc_sections_sorted:
            s.set_mapping_attributes()

        mapped_through = 0                      # We've mapped everything up to and including this address
        self.mappings = []
        while True:
            next_address = 0x100000000          # Provisional next address to map
            for s in self.alloc_sections_sorted:
                if s.sh_flags & s.SHF_MAP_CAN_BE_SKIPPED:
                    continue                    # Mapping this section is optional
                if s.sh_addr + s.sh_size <= mapped_through:
                    continue                    # We're already past this section
                if s.sh_addr <= mapped_through:
                    next_address = mapped_through
                    break                       # We need to map at "mapped_through"
                if next_address > s.sh_addr:
                    next_address = s.sh_addr
            if next_address == 0x100000000:
                break
            sz = 0x1000000                      # Try a 16MB mapping
            msg_err = ''
            while sz >= 0x1000:
                try:
                    map = self.try_mapping(next_address,sz)
                except MergeException as e:
                    map = None
                    msg_err = str(e)
                if map:
                    break
                sz /= 4
            if map == None:
                raise MergeException(msg_err)
            self.mappings.append(map)
            mapped_through = map.sh_size + map.sh_addr
        all_mappings = ''
        for s in self.mappings:
            print '%X -> %X, size %X' % (s.sh_addr, int(s.physaddr), s.sh_size)
        for s in self.mappings:
            if s.locked.value():
                encoding = s.tlb_encoding(True)
                if encoding:
                    for key, values in self.patchable_mmaps.iteritems():
                        if s.sh_addr == values[ 'vaddr' ]:
                            # Emit Q6CFG
                            import pdb; pdb.set_trace()

                all_mappings += encoding
        reloc_map_count = len(all_mappings)/8

        for s in self.mappings:
            if s.locked.value():
                encoding = s.tlb_encoding(False)
                if encoding:
                    for key, values in self.patchable_mmaps.iteritems():
                        if s.sh_addr == values[ 'vaddr' ]:
                            # Emit Q6CFG
                            self.eip.append( eip_q6cfg( len( all_mappings )/8, eip_q6cfg.LOCKED ) )
                all_mappings += encoding

        print 'maps: %r' % all_mappings
        print 'reloc_map_count: %r' % reloc_map_count
        self[(self.QURTK_tlb_dump,len(all_mappings))] = all_mappings
        self[(self.QURTK_tlb_dump_relocs,4)] = ('<L', reloc_map_count)
        tlb_dump_physaddr = self.virt_to_physload(self.QURTK_tlb_dump)
        if tlb_dump_physaddr.irelocatable:
            self[(self.QURTK_quick_load_MMU,4)] = ('<L', int(tlb_dump_physaddr)+1)
        else:
            self[(self.QURTK_quick_load_MMU,4)] = ('<L', int(tlb_dump_physaddr))

    def last_fixups(self):
        if self.QURTK_hbi:
            if self.x__hexagon_bsp_init:
                hbi_physaddr = self.virt_to_physload(self.x__hexagon_bsp_init)
                if hbi_physaddr.irelocatable:
                    self[(self.QURTK_hbi,4)] = ('<L', int(hbi_physaddr)+1)
                else:
                    self[(self.QURTK_hbi,4)] = ('<L', int(hbi_physaddr))

    def eip_build_filelike(self):
        for s in self.section_list:
            if s.name == '.qskernel_eip_build':
                return s.contents()
        #
        #  For now, tolerate the lack of an EIP build section;
        #   return an empty one.
        #
        return StringIO('\0' * 4)
        raise Exception('Cannot find EIP build section')

    def eip_build_records(self, matchtype, decode=''):
        import struct
        decodelen = struct.calcsize('<'+decode)
        f = self.eip_build_filelike()
        while True:
            try:
                recordlength, recordtype = struct.unpack('<HH', f.read(4))
            except struct.error:
                raise Exception('EIP build section is malformed')
            if (recordlength, recordtype) == (0,0):
                break
            payload_length = recordlength - 4
            if (payload_length & 3) != 0 or payload_length < 0:
                raise Exception('EIP build section is malformed')
            if payload_length == 0:
                payload = ''
            else:
                payload = f.read(payload_length)
                if len(payload) != payload_length:
                    raise Exception('EIP build section is malformed')
            if recordtype != matchtype:
                continue
            datalen = payload_length - decodelen
            if datalen < 0:
                raise Exception('EIP build section is malformed')
            yield struct.unpack('<%s%us' % (decode,datalen), payload)

    def eip_build_do_copies(self):
        for addr, length, data in self.eip_build_records(1, 'LL'):
            if length > len(data):
                raise Exception('EIP copy record is malformed')
            self[(addr,length)] = data[:length]

    def build_phdrs(self):
        self.output_sections = [s for s in self.alloc_sections_sorted if not (s.sh_flags & s.SHF_INTERNAL_ONLY)]
        self.output_sections.sort(key=lambda s: s.physaddr)
        self.phdr = []
        p = None
        for s in self.output_sections:
            # See if we can add the section to the last program header we had
            if p and p.secure.value() == s.SECURE.value() and p.f_orig == s.f_orig:
                if s.sh_type == s.SHT_NOBITS:
                    # Can always add a NOBITS section
                    p.p_memsz = (s.sh_addr + s.sh_size) - p.p_vaddr
                    p.section_list.append(s)
                    continue
                else:
                    # Can add a PROGBITS section if the program header has
                    #  filesz == memsz and if the (vaddr-physaddr) matches
                    # (But only if the section is within 16K of the last section)
                    if p.p_filesz == p.p_memsz:
                        if (s.sh_addr - p.p_vaddr) == (s.physaddr - p.p_paddr):
                          if s.sh_addr <= p.p_vaddr + p.p_filesz + 0x4000:
                            p.p_memsz = (s.sh_addr + s.sh_size) - p.p_vaddr
                            p.p_filesz = p.p_memsz
                            p.section_list.append(s)
                            continue
            if s.sh_type == s.SHT_NOBITS:
                filesz = 0
            else:
                filesz = s.sh_size
            secureflags = 0
            if s.SECURE:
                secureflags = (1 << 21)
            p = Elf32ProgramHeader(None,
                                   p_type = Elf32ProgramHeader.PT_LOAD,
                                   p_offset = s.sh_offset,
                                   p_vaddr = s.sh_addr,
                                   p_paddr = s.physaddr,
                                   p_filesz = filesz,
                                   p_memsz = s.sh_size,
                                   p_flags = 4 + secureflags,
                                   p_align = 4096,
                                   secure = s.SECURE,
                                   temp = False,
                                   f_orig = s.f_orig,
                                   f = s.f)
            self.phdr.append(p)
            p.section_list = [s]
        for p in self.phdr:
            for s in p.section_list:
                if s.sh_flags & s.SHF_WRITE:
                    p.p_flags |= 2
                if s.sh_flags & s.SHF_EXECINSTR:
                    p.p_flags |= 1
        #
        #  Now fill the gaps so that all physical addresses are covered.
        #

        if not self.use_phdrs.value:
          for pool in self.pools:
            if pool.name == "DEFAULT_PHYSPOOL":
                total_phys_size = pool.ranges[0][1] << 12
                p = Elf32ProgramHeader(None,
                                       p_type = Elf32ProgramHeader.PT_LOAD,
                                       p_offset = 0x1000,
                                       p_vaddr = 0,
                                       p_paddr = self.phdr[0].p_paddr + total_phys_size,
                                       p_filesz = 0,
                                       p_memsz = 1,
                                       p_flags = 4,
                                       p_align = 4096,
                                       secure = ConfigField(False, description='Secure', driver=('<ELF PADDING>')),
                                       temp = True,
                                       f = self.phdr[0].f)
                p.section_list = []
                self.phdr.append(p)

        #
        #  Next expand each PHDR to either the start of the next PHDR
        #   or to the next 4K boundary, whichever comes first.
        #
        if not self.use_phdrs.value:
          for left,right in zip(self.phdr[:-1],self.phdr[1:]):
            new_end_address = (left.p_paddr + left.p_memsz).align_up_to_vaddr(0,0x1000)
            new_end_address = min(new_end_address, right.p_paddr)
            left.p_memsz = new_end_address - left.p_paddr
            gapsize = right.p_paddr - new_end_address
            if gapsize:
                for s in self.section_list:
                    if s.name == '.qsvspace':
                        missing = (int(new_end_address) - s.sh_addr) & 0xFFF
                        vaddr = s.sh_addr + missing
                        s.sh_addr += gapsize + missing
                        s.sh_size -= gapsize + missing
                        if s.sh_size < 0:
                            s.sh_addr -= gapsize + missing
                            s.sh_size += gapsize + missing
                            #raise Exception('Not enough virtual address space in .qsvspace to fill gaps')
                            continue
                        print 'Adding PHDR with vaddr %X' % vaddr
                        p = Elf32ProgramHeader(None,
                                               p_type = Elf32ProgramHeader.PT_LOAD,
                                               p_offset = 0x1000,
                                               p_vaddr = vaddr,
                                               p_paddr = new_end_address,
                                               p_filesz = 0,
                                               p_memsz = gapsize,
                                               p_flags = 4,
                                               p_align = 4096,
                                               secure = ConfigField(False, description='Secure', driver=('<ELF PADDING>')),
                                               temp = False,
                                               f = left.f)
                        self.phdr.append(p)
                        p.section_list = []
                        break
                else:
                    pass
                    #raise Exception('No .qsvspace section?')
        self.phdr.sort(key=lambda p: p.p_paddr)
        if not self.use_phdrs.value:
            self.phdr.remove(self.phdr[-1])

    def handle_debug_sections(self):
        #
        #  Insert any needed debug sections into self.output_sections[] list.
        #
        #  For now, look at the EIP build records for SYMTAB and STRTAB records.
        #
        symtab_records = list(self.eip_build_records(EIP_BUILD_SYMTAB))
        strtab_records = list(self.eip_build_records(EIP_BUILD_STRTAB))
        counts = (len(symtab_records), len(strtab_records))
        if counts == (0,0):
            return
        if counts != (1,1):
            raise Exception('Count of EIP build records is bogus: (symtab = %u, strtab = %u)' % counts)

        symtab_string = symtab_records[0][0]      # It's a list containing a tuple containing a string
        strtab_string = strtab_records[0][0]
        syms = []
        while symtab_string:
            syms.append(Elf32Sym(symtab_string[:16]))
            symtab_string = symtab_string[16:]
        #
        #  Strip out symbols which have zero value; those are weak symbols which didn't resolve.
        #  Start at index 1 because index 0 is reserved and all zeroes.
        #
        syms[1:] = [x for x in syms[1:] if x.st_value != 0]
        symtab_string = ''.join([x.output() for x in syms])

        #
        #  Okay, the symtab_string and strtab_string are ready to get inserted.
        #

        self.output_sections.insert(2,QIBSection(None,
                                                 name = '.symtab',
                                                 f = StringIO(symtab_string),
                                                 sh_name = 0,
                                                 sh_type = QIBSection.SHT_SYMTAB,
                                                 sh_flags = 0,
                                                 sh_addr = 0,
                                                 sh_offset = 0,
                                                 sh_size = len(symtab_string),
                                                 sh_link = 3,           # Index where we'll insert strtab
                                                 sh_info = 1,           # Index of first non-local symbol
                                                 sh_addralign = 4,      # Alignment requirement
                                                 sh_entsize = 16))      # Each entry is 16 bytes
        self.output_sections.insert(3,QIBSection(None,
                                                 name = '.strtab',
                                                 f = StringIO(strtab_string),
                                                 sh_name = 0,
                                                 sh_type = QIBSection.SHT_STRTAB,
                                                 sh_flags = 0,
                                                 sh_addr = 0,
                                                 sh_offset = 0,
                                                 sh_size = len(strtab_string),
                                                 sh_link = QIBSection.SHN_UNDEF,
                                                 sh_info = 0,
                                                 sh_addralign = 1,
                                                 sh_entsize = 0))

    def write_output_file(self):
        #
        #  Write output file in this order:
        #   ELF header
        #   Program headers (only PT_LOAD, sorted by physical address)
        #   Section headers (index 0 is empty slot, index 1 is the string table, indexes 2 onward are alloc sections)
        #   Section string table (pointed to by index 1 of the section table)
        #   Padding to 4K boundary
        #   Output data, sorted by physical address, with appropriate padding between pieces.
        import shutil
        #
        #  Insert the special sections
        #
        self.output_sections.insert(0,QIBSection(None,
                                                 name = '',
                                                 sh_name = 0,
                                                 sh_type = QIBSection.SHT_NULL,
                                                 sh_flags = 0,
                                                 sh_addr = 0,
                                                 sh_offset = 0,
                                                 sh_size = 0,
                                                 sh_link = QIBSection.SHN_UNDEF,
                                                 sh_info = 0,
                                                 sh_addralign = 0,
                                                 sh_entsize = 0))
        self.output_sections.insert(1,QIBSection(None,
                                                 name = '.shstrtab',
                                                 sh_name = 0,
                                                 sh_type = QIBSection.SHT_STRTAB,
                                                 sh_flags = 0,
                                                 sh_addr = 0,
                                                 sh_offset = 0,
                                                 sh_size = 0,
                                                 sh_link = QIBSection.SHN_UNDEF,
                                                 sh_info = 0,
                                                 sh_addralign = 1,
                                                 sh_entsize = 0))
        self.handle_debug_sections()
        #
        #  Build the string table
        #
        strtab = '\0'
        for s in sorted(self.output_sections, key=lambda s: -len(s.name)):
            zname = s.name + '\0'
            if strtab.find(zname) < 0:
                strtab += zname
            s.sh_name = strtab.find(zname)
        self.output_sections[1].f = StringIO(strtab)
        self.output_sections[1].sh_size = len(strtab)
        for s in self.output_sections[1:]:
            s.writedata = s.contents()
        #
        #  Calculate the number of bytes needed for the header portion up to and including the string table
        #
        output_location = 52                                    # ELF header
        self.ehdr.e_phoff = output_location
        self.ehdr.e_phnum = len(self.phdr)
        output_location += 32*len(self.phdr)                    # Program headers
        self.ehdr.e_shoff = output_location
        self.ehdr.e_shnum = len(self.output_sections)
        self.ehdr.e_shstrndx = 1
        output_location += 40*len(self.output_sections)         # Section headers
        for s in self.output_sections[1:]:
            if (s.sh_flags & s.SHF_ALLOC) == 0:
                # It's a non-alloc section.  Compute its file location here
                floc = output_location
                if s.sh_addralign:
                    floc = alignup(floc, s.sh_addralign)
                s.sh_offset = floc
                output_location = floc + s.sh_size
        self.ehdr.e_entry = self.physaddr

        for p in self.phdr:
            p.p_paddr = p.p_paddr.basevalue()

        for p in self.phdr:
            offset = output_location
            alignment = 0x1000
            misalign = (output_location - int(p.p_paddr)) & (alignment-1)
            if misalign:
                offset += alignment-misalign
            p.p_offset = offset
            output_location = offset + p.p_filesz
            for s in p.section_list:
                s.sh_offset = p.p_offset + (s.sh_addr - p.p_vaddr)

        fo = open(self.outfilename,'wb')
        fo.write(self.ehdr.output())
        for p in self.phdr:
            fo.write(p.output())
        for s in self.output_sections:
            fo.write(s.output())
        for s in self.output_sections[1:]:
            if s.sh_type != s.SHT_NOBITS:
                delta = s.sh_offset - fo.tell()
                if delta:
                    fo.write('\0' * delta)
                shutil.copyfileobj(s.writedata, fo)
        self.file = fo
        fo.flush()
        fo.close()

    def main(self, argv):
        self.parse_args(argv)
        self.setup_elf_sections()
        self.extract_qurt_info()
        self.mark_secure_programs()
        self.extract_patchable_mmap();
        self.extract_memsections()
        self.extract_island_config()
        self.read_pool_configs()
        self.apply_xml_to_sections()
        self.allocate_phys_space()
        self.set_kernel_boundaries()
        self.set_srm_boundaries()
        self.add_build_mappings()
        self.optimize_mappings()
        self.last_fixups()
        self.eip_build_do_copies()
        self.set_root_prog_no()
        self.build_phdrs()
        self.eip.writeback(None, self.infiles, self.island_moves, [None], self.opt_mappings, self.mappings)
        self.write_output_file()
        return 0

if __name__ == '__main__':
    from lib.qurt import run_script
    run_script(QurtElfPatch().main)
