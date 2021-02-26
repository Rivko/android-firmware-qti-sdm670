#! /usr/bin/python
#=============================================================================
#
#                                    elf_info_patch.py
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

#
#  New format of ELF Info Patch area:
#
#    Consists of one or more variable length records of the following format:
#
#      uint16_t length;
#      uint16_t type;
#      ...variable length contents...
#
#    The length field is a multiple of 4.  A value of zero indicates the end
#     of the Elf Info Patch area.  Any other value indicates the total length
#     of the record in bytes.  (If a record would have a size which is not
#     a multiple of 4, it must be rounded up to a multiple of 4.)
#    The type field describes what type of record is present.
#
#    Type 3 (BOOT_COPY):
#       Contents are three words:
#          uint32_t dest;
#          uint32_t source;
#          uint32_t size;
#    Type 4 (BOOT_COPY4):
#       Contents are three words:
#          uint32_t dest;
#          uint32_t source;
#          uint32_t size;
#    Type 5 (CLADE REGISTERS):
#       Contents are 5 words:
#          uint32_t clade_comp_start;
#          uint32_t clade_exception_high_start;
#          uint32_t clade_exception_low_small_start;
#          uint32_t clade_exception_low_large_start;
#          uint32_t clade_base_paddr;
#    Type 16 (USER_APP_NAME):
#       Contents are a variable length zero-terminated process name:
#          char proc_name[];
#    Type 17 (USER_APP_MAPPING):
#       Contents are four words:
#          uint32_t virtual_address_start;
#          uint32_t physical_address_start;
#          uint32_t length_in_bytes;
#          uint32_t permissions;
#    Type 18 (USER_APP_INIT):
#       Contents are three words:
#          uint32_t dest;
#          uint32_t source;
#          uint32_t size;
#    Type 19 (USER_APP_ENTRYPOINT):
#       Contents are one word:
#          uint32_t app_entry_point;
#    Type 20 (SRM_INFO):
#       Contents are two words:
#          uint32_t ASID;
#          uint32_t entry_point;
#          uint32_t program_number;            // Can this be cleaned up?
#    Type 21 (CACHE_REGION_POLICIES):
#       Contents are a variable length set of tuples:
#          int8_t pool_number, region_number, cache_policy;
#          int8_t terminator = -1;
#
#    Type 17, 18, and 19 entries refer to the most recent Type 16 entry
#     in the table.
#
import struct

BOOT_COPY = 3
BOOT_COPY4 = 4
REGISTER = 5
USER_APP_NAME = 16
USER_APP_MAPPING = 17
USER_APP_INIT = 18
USER_APP_ENTRYPOINT = 19
SRM_INFO = 20
CACHE_REGION_POLICIES = 21

class eip_build_record:
    """Base (abstract) class for EIP build record
    """
    Q6CFG = 5

    def __init__(self, record_type, record_len):
        """Constructor
        """
        pass

class eip_record:
    """Base (abstract) class for EIP records
    """
    Q6CFG = 6
    def __init__ (self, record_type, record_len):
        self._record_type = record_type
        self._record_len  = 4 + record_len

    def pack (self):
        return struct.pack( '<HH', self._record_len, self._record_type )

class eip_q6cfg (eip_record):
    """Class representing Q6CFG EIP record. It provides boot-time (crt0)
       instruction for patching physical address of the associated memory mapping
    """
    # Associated memory map is locked in TLB. Lock may be transient (ie boot
    # lock), but for the purpose of this EIP record, we dont care
    LOCKED = True

    # Associated memory map is not locked in TLB
    UNLOCKED = False

    def __init__ (self, index, is_locked):
        """Constructor
        """
        self._index = index
        self._is_locked = is_locked
        eip_record.__init__( self, eip_record.Q6CFG, 4 )

    def pack (self):
        """Serialize this record
        """
        return eip_record.pack( self ) + struct.pack( '<HH', self._is_locked, self._index )

def encode_eip_record(record_type, encoding, *values):
    from qurt import alignup

    if encoding.startswith('<'):
        # Assume the encoding is for struct.pack
        payload = struct.pack(encoding, *values)
    else:
        # Assume the encoding is for string modulo
        payload = (encoding % values)

    payload_len = alignup(len(payload), 4)
    if payload_len > 0xFFFF:
        raise Exception()
    ret = struct.pack('<HH%us' % payload_len, payload_len+4, record_type, payload)
    return ret

def _build_eip_info(physaddr, f, infiles, island_moves, clade_registers, opt_mappings, mappings=None, q6cfg_records=[]):
    from qurt import aligndown, alignup
    if opt_mappings:
        # New style of doing things; use the optimized mappings we already computed
        for asid in range(1,len(infiles)):
            segs = []
            for M in opt_mappings:
                if M.C >= 0 and M.ASID == asid:
                    segs += [(M.vpage << 12, (M.vpage+M.size) << 12, (M.ppage-M.vpage) << 12, (M.X != 0), (M.W != 0), M.island, M.reloc)]
            infiles[asid].segs = segs
    eip_data = ''
    for m in island_moves:
        if m[-1] == "4":
            eip_data += encode_eip_record(BOOT_COPY4, '<LLL', *m[:-1])
        else:
            eip_data += encode_eip_record(BOOT_COPY, '<LLL', *m[:-1])
    if None not in clade_registers:
        eip_data += encode_eip_record(REGISTER, '<LLLLL', *clade_registers)
    for record in q6cfg_records:
        eip_data += record.pack()
    for i in infiles[1:]:
        if hasattr(i,'SRM_ASID'):
            srm_program_number = i.cfg.program_maps.get(i.appname,-1)
            eip_data += encode_eip_record(SRM_INFO, '<LLL', i.SRM_ASID, i.SRM_ENTRY, srm_program_number & 0xFFFFFFFF)
            print 'SRM info : %u, 0x%08X, %d' % (i.SRM_ASID, i.SRM_ENTRY, srm_program_number)
            #
            #  We're done.  Don't insert the standard EIP records for user processes.  The SRM process
            #   is statically started by the micro-kernel and can't be spawned through normal mechanisms.
            #
            continue
        eip_data += encode_eip_record(USER_APP_NAME, '%s\0', i.appname)
        if mappings != None:
            this_maps = []
            for m in mappings:
                if hasattr(m,'APPNAME'):
                    if m.APPNAME.value() == i.appname:
                        this_maps.append(m)
            for m in this_maps:
                perms = 1
                if m.X.value():
                    perms += 4
                if m.W.value():
                    perms += 2
                pa = int(m.physaddr)
                if m.physaddr.irelocatable:
                    # Relocated relative to physical load address
                    perms += 0x200
                eip_data += encode_eip_record(USER_APP_MAPPING, '<LLLL', m.sh_addr, pa, m.sh_size, perms)
            for rcopy in i.restart_copies:
                print 'USER_APP_INIT copy %X bytes from %X to %X' % (rcopy[2], rcopy[1], rcopy[0])
                eip_data += encode_eip_record(USER_APP_INIT, '<LLL', *rcopy)
            eip_data += encode_eip_record(USER_APP_ENTRYPOINT, '<L', this_maps[0].sh_addr)
            continue
        segs = i.segs
        segs = sorted(segs, key=lambda s: (s[5], s[0]+s[2]))
        for s in segs:
            perms = 1           # Read permission
            ''' Add in exec and write permissions '''
            if s[3]:
                perms += 4        # Execute permission
            if s[4]:
                perms += 2        # Write permission
            if s[5]:
                perms += 0x100    # Island status
            pa = s[0]+s[2]
            if s[6]:
                # Relocated relative to physical load address
                perms += 0x200
                pa -= physaddr
            eip_data += encode_eip_record(USER_APP_MAPPING, '<LLLL', s[0], pa, s[1]-s[0], perms)
        for rcopy in i.restart_copies:
            print 'USER_APP_INIT copy %X bytes from %X to %X' % (rcopy[2], rcopy[1], rcopy[0])
            eip_data += encode_eip_record(USER_APP_INIT, '<LLL', *rcopy)
        eip_data += encode_eip_record(USER_APP_ENTRYPOINT, '<L', segs[0][0])
    cache_region_array = map(int,infiles[0].cfg.region_cache_policies.split()) + [-1]
    cache_region_format_string = '<%ub' % len(cache_region_array)
    eip_data += encode_eip_record(CACHE_REGION_POLICIES, cache_region_format_string, *cache_region_array)
    eip_data += '\0\0\0\0'
    return eip_data

class ElfInfoPatch:
    def __init__(self):
        #
        # List here the symbols we need to extract from the build
        #
        # To declare a symbol as optional (weak), prepend the name
        #  of the symbol with [w] with no intervening white space.
        #  For example:  [w]Optional_Symbol
        #
        # Weak symbols that don't exist in the build are reported
        #  as having an address of zero.
        #
        self.symbols_spec = '''
            QURTK_tlb_dump
            TLB_LAST_REPLACEABLE_ENTRY
            qurtos_mmap_table
            QURTK_tlblock_entries
            QURTK_tlb_dump_relocs
            qurtos_boot_mappings
            QURTK_quick_load_MMU
            qurtos_xml_passthrough
            qurtos_virt_reserved
            [w]QURTK_root_prog_no
            [w]qurtos_ramfs_directory
            [w]qurtos_extra_instructions
            [w]qurtos_slots
            [w]qurtos_tlb_reclaim
            [w]pool_configs
            [w]__hexagon_bsp_init
            [w]QURTK_hbi
            [w]QURTK_tbb
            [w]QURTK_vprotect_start
            [w]QURTK_vprotect_end
            [w]QURTK_pprotect_start
            [w]QURTK_pprotect_end
            [w]QURTK_srmimage_start
            [w]QURTK_srmimage_end
            QURT_EIP_BUILD
        '''
        self.flags_spec = '''
            CONFIG_ADVANCED_MEMORY
        '''
        self.symbols = self.symbols_spec.split()
        self.flags = self.flags_spec.split()
        self.symbols.sort()
        self.flags.sort()
        self.total_words = len(self.symbols)+1
        # List of eip_record objects
        self.records = []

    def append (self, record):
        if not isinstance( record, eip_record ):
            raise TypeError( 'argument expected to be of eip_record type' )
        self.records.append( record )

    def Signature(self):
        #
        # For now, have the signature be the actual strings themselves
        #
        r = '\n'.join(self.flags+['']+self.symbols)
        while len(r)%4:
            r+='\n'
        return r
    def FindSig(self, cfg, addr, size):
        #
        #  Class method which is supposed to find and return the
        #   signature string found in the .start section.
        #
        #  The signature string will always:
        #   1.  Start with the string "CONFIG_"
        #   2.  Consist of only printable characters (Python includes newlines as printable)
        #   3.  Have a length which is a multiple of 4
        #   4.  Occur exactly twice within the .start section
        #   5.  Have the first instance in .start be preceded by four zero bytes which are 8-byte-aligned
        #   6.  Have both instances in .start be 4-byte aligned
        #   7.  Except for 0-3 trailing "padding" newlines, have exactly one
        #        instance of two newlines in a row.
        #
        #  Within those constraints, return the longest such signature
        #   which works.  If two tie for the longest such signature
        #   which works, use the one found earliest in the section.
        #
        #  Note that ties should never happen, but we're prepared to behave deterministically.
        #
        #  The reason we go to this trouble is to allow for parsing an ELF which was compiled
        #   against a different version of this file -- one which might export more, fewer,
        #   or different symbols.  This won't happen often during normal builds, but can
        #   be very useful for debugging.
        #

        import string
        zeroes = '\0\0\0\0'
        sig_start = 'CONFIG_'
        sig_start_with_zeroes = zeroes+sig_start
        start_contents = cfg[(addr,size)]
        try_these = start_contents.split(sig_start_with_zeroes)
        i = 0
        best_return = (0,)
        while i+1 < len(try_these):
            i += 1
            # Reconstruct the portion up to the signature we're going to try
            #  and the portion after the signature.
            before = sig_start_with_zeroes.join(try_these[:i])+zeroes
            after = sig_start + sig_start_with_zeroes.join(try_these[i:])
            if len(before) % 8 != 4:
                continue                  # Violates #6 above

            try_siglen = 8                # Base size from which to start trying
            while True:
                try_siglen += 4           # Enforce #3 above
                try_sig = after[:try_siglen]
                stripped = try_sig.rstrip('\n')
                fsplit = stripped.split('\n\n')
                if len(try_sig) < try_siglen:
                    break                 # Not enough chars, and longer won't work
                if [c for c in try_sig if not c in string.printable]:
                    break                 # Violates #2 above, and longer won't work
                if len(stripped.split('\n\n\n')) != 1:
                    break                 # Violates #7 above, and longer won't work
                if len(fsplit) != 2:
                    continue              # Violates #7 above, but longer might work
                start_splits = start_contents.split(try_sig)
                if len(start_splits) != 3:
                    continue              # Violates #4 above, but longer might work
                if not start_splits[0].endswith(zeroes):
                    continue              # Violates #5 above, but longer might work
                if len(start_splits[1]) % 4 != 0:
                    continue              # Violates #6 above, but longer might work?

                # One last thing -- the space between the two signatures must have
                #  at least 4 bytes for every symbol after the double-newline, plus
                #  an extra 4 bytes for the flags
                # Check that here.

                needed_length = (len(fsplit[1].split('\n'))+1)*4
                have_length = len(start_splits[1])
                if have_length < needed_length:
                    continue              # Not enough data, but longer might work?

                eip_offset = len(before)-4
                eip_size = have_length+try_siglen*2+4

                if eip_size % 8 != 0:
                    continue              # Not 8-byte-aligned, but longer might work?

                if try_siglen > best_return[0]:
                    best_return = (len(try_sig), try_sig, eip_offset, eip_size, start_splits[1][:needed_length])

        if best_return == (0,):
            raise Exception('Could not find a valid ELF Info Patch area')

        return best_return

    def Parse(self, cfg, addr, size):
        (_, sig, offset, total_size, data) = self.FindSig(cfg, addr, size)
        temp = sig.split('\n\n')
        flagnames = temp[0].split('\n')
        symbolnames = temp[1].strip('\n').split('\n')
        temp = struct.unpack('<'+'L'*(len(data)/4), data)
        bitpos = 1

        def safehasattr(cfg, aname):
            if aname.startswith('_'):
                aname = 'x'+aname
            return hasattr(cfg, aname)

        def safesetattr(cfg, aname, value):
            if aname.startswith('_'):
                aname = 'x'+aname
            setattr(cfg, aname, value)

        for x in flagnames:
            if temp[0] & bitpos:
                safesetattr(cfg, x, True)
            else:
                safesetattr(cfg, x, False)
            bitpos *= 2
        ix = 1
        for x in symbolnames:
            if x.startswith('[w]'):
                safesetattr(cfg, x[3:], temp[ix])
            else:
                safesetattr(cfg, x, temp[ix])
            ix += 1
        for x in self.flags:
            if not hasattr(cfg, x):
                safesetattr(cfg, x, False)
        for x in self.symbols:
            if x.startswith('[w]'):
                x = x[3:]
            if not safehasattr(cfg, x):
                safesetattr(cfg, x, 0)
        #
        # Now fill in the legacy variables
        #
        cfg.eip_addr_boot_maps = cfg.qurtos_boot_mappings
        cfg.eip_addr_tlb_dump = cfg.QURTK_tlb_dump
        cfg.eip_size_tlb_dump = 1+cfg.TLB_LAST_REPLACEABLE_ENTRY
        cfg.eip_addr_tlblock = cfg.QURTK_tlblock_entries
        cfg.eip_addr_mmap_table = cfg.qurtos_mmap_table
        cfg.eip_addr_tlb_relocs = cfg.QURTK_tlb_dump_relocs
        cfg.eip_addr_quick_mmu = cfg.QURTK_quick_load_MMU
        cfg.eip_size = total_size
        cfg.eip_location = addr + offset

        addr = cfg.qurtos_virt_reserved
        vr = []
        while True:
            z = cfg[(addr,'<l')][0]
            if z < 0:
                break
            vr.append(z)
            addr += 4
        cfg.vrlist = zip(vr[::2],vr[1::2])

        # Go read XML passthrough

        if cfg.qurtos_xml_passthrough:
            sz = struct.unpack('<L', cfg[(cfg.qurtos_xml_passthrough, 4)])[0]
            xml = cfg[(cfg.qurtos_xml_passthrough+4,sz)]
        else:
            xml = '<image></image>'
        from xml.dom.minidom import parseString
        cfg.xml = parseString(xml.decode('utf-8'))
        cfg.eip = self
        self.cfg = cfg

        #
        # Go read the eip build XML content
        #
        import xml.dom.minidom
        for length, data in cfg.eip_build_records(2, 'L'):
            if length > len(data):
                raise Exception('EIP XML record is malformed')
            doc = xml.dom.minidom.parseString(data[:length])
            if doc.documentElement.tagName == 'build_vars':
                # Variables passed from qurt_config
                for el in doc.documentElement.childNodes:
                    dictname = el.getAttribute('dict')
                    key = el.getAttribute('key')
                    if dictname and key:
                        X = (lambda:None)
                        X.__dict__.update(el.attributes.items())
                        D = getattr(cfg, dictname, dict())
                        D[key] = X
                        setattr(cfg, dictname, D)
                        continue
                    setattr(cfg, el.tagName, el.getAttribute('value'))

        return (cfg, cfg.xml.documentElement.childNodes)

    def Build_Asm(self, outfilename, req_eipsize, flags):
        bitpos = 1
        flags_numeric = 0
        for x in self.flags:
            if ('-D'+x) in flags:
                flags_numeric += bitpos
            bitpos *= 2
        sig = self.Signature()
        #
        # Calculate total space used for our contents (minus padding)
        #  4 bytes for zero word at the beginning
        #  4 bytes for the flags word
        #  4 bytes for each symbol we need
        #  2 signatures
        #
        used_space = 4 + 4 + len(self.symbols)*4 + len(sig)*2
        eipsize = req_eipsize
        if eipsize < used_space:
            eipsize = used_space
        while eipsize % 8:
            eipsize += 1
        if eipsize != req_eipsize:
            print 'Adjusting EIP size from requested %s to actual %s' % (req_eipsize, eipsize)
        unused_space = eipsize - used_space
        fo = open(outfilename, 'w')
        fo.write('/* This file is automatically generated */\n')
        fo.write('#ifdef GENERATE_ELF_INFO_PATCH\n')
        fo.write('  .align 8\n')
        fo.write('ELF_INFO_PATCH:\n')
        fo.write('  .word 0\n')
        for x in sig:
            fo.write('  .byte %u\n' % ord(x))
        fo.write('  .word %u\n' % flags_numeric)
        for x in self.symbols:
            if x.startswith('[w]'):
                fo.write('  .weak %s\n' % x[3:])
                fo.write('  .word %s\n' % x[3:])
            else:
                fo.write('  .word %s\n' % x)
        if unused_space:
            fo.write('  .space %u,0\n' % unused_space)
        for x in sig:
            fo.write('  .byte %u\n' % ord(x))
        fo.write('#endif /* GENERATE_ELF_INFO_PATCH */\n')
        fo.close()
        return 0

    def writeback(self, *args):
        data = _build_eip_info(self.cfg.physaddr, *args, q6cfg_records=self.records )
        if len(data) > self.cfg.eip_size:
            raise Exception('ELF Info Patch area overflow.  Cannot complete image build.')
        if len(data) < self.cfg.eip_size:
            data += '\177' * (self.cfg.eip_size - len(data))
        self.cfg[(self.cfg.eip_location,len(data))] = data

if __name__ == '__main__':
    from sys import argv
    ret = 1
    if 'build_eip' in argv[1:2]:
        ret = ElfInfoPatch().Build_Asm(argv[2], int(argv[3]), argv[4:])
    raise SystemExit(ret)
