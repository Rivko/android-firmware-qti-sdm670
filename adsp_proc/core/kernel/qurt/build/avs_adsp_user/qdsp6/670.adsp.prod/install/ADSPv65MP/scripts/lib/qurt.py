#=============================================================================
#
#                                    qurt.py
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
#  File of general utility classes and functions
#   for QuRT Python scripting
#
#  Use one of the following to import:
#     import lib.qurt                   {Imports everything but with lib.qurt needed as a prefix}
#     from lib.qurt import *            {Imports everything, no prefix needed}
#     from lib.qurt import x,y,z        {Imports just the listed identifiers; replace x,y,z with actual identifiers}
#

if __name__ == '__main__':
    raise SystemExit('qurt.py is not an executable script')

class _QurtInf(object):
    #
    # Integer Infinities, usable as QurtInfinity and -QurtInfinity.
    # The intent is that these can be used where an integer is normally
    #  used, and that some basic arithmetic operations work between
    #  infinite values and themselves and with integers -- in particular
    #  comparison, subtraction, and sorting.
    # Note that this implementation differs from the IEEE concept of
    #  floating point infinities in one major way:  infinities of the
    #  same sign compare equal to each other and have a difference of
    #  integer zero.  QurtInfinity - QurtInfinity is equal to zero.
    #  (IEEE floating point defines Inf - Inf as being NaN.)
    #
    # Note that the class name is prepended with an underscore to
    #  indicate that it's not expected to be used directly by
    #  scripts.  There should generally only be two instances of
    #  this class in existence, and they are accessible as QurtInfinity
    #  (or +QurtInfinity) and -QurtInfinity.
    #
    def __init__(self, sign):
        self.sign = sign                            # sign = 1 or -1
    def __repr__(self):
        return self.repr_string
    def __cmp__(self,other):
        if isinstance(other,self.__class__):
            return (self.sign-other.sign)           # Comparison between infinities
        else:
            return self.sign                        # Comparison against non-infinity
    def __sub__(self,other):
        if self == other:
            return 0                                # Two infinities of the same sign have 0 difference
        else:
            return self                             # Subtracting anything else is a no-op
    def __rsub__(self,other):
        return -(self-other)                        # other-self; return -(self-other)
    def __pos__(self):
        return self                                 # +X returns X
    def __neg__(self):
        return self.negated                         # -X returns X.negated

QurtInfinity = _QurtInf(1)
QurtInfinity.repr_string = '+QurtInfinity'
QurtInfinity.negated = QurtInfinity.__class__(-1)   # Initialize the negative of +infinity
QurtInfinity.negated.repr_string = '-QurtInfinity'
QurtInfinity.negated.negated = QurtInfinity         # Initialize the negative of -infinity

class QurtAddressRange(object):
    #
    # Define range description.  This is a left boundary, a right boundary,
    #  and a set of attributes.  Attributes which are not present
    #  are returned as None.  Zero-length ranges evaluate as boolean False and
    #  have no attributes.  Non-empty ranges evaluate as boolean True and
    #  may have attributes.
    #
    __slots__ = ['left','right',
                 'addr','size',
                 'container',
                 'full','__dict__']                     # Keep these attributes out of __dict__

    def __getattr__(self, a):
        return None                                     # If an attribute is missing, it's None

    def __nonzero__(self):
        return (self.size > 0)                          # Allows testing non-zero range with "if range:"

    def __repr__(self):
        tmp = '%s(' % self.__class__.__name__
        if self.size > 0:
            leftrepr = '%r' % self.left
            rightrepr = '%r' % self.right
            try:
                leftrepr = '0x%X' % self.left
            except TypeError:
                pass
            try:
                rightrepr = '0x%X' % self.right
            except TypeError:
                pass
            tmp += 'left=%s,right=%s' % (leftrepr,rightrepr)
            for k in self.__dict__:
                tmp += ',%s=%r' % (k,self.__dict__[k])
        return tmp + ')'

    def __init__(self, addr=0, size=0, left=0, right=0, full=False, templates=None, container=None, **kw):
        if full:
            left,right = (-QurtInfinity,QurtInfinity)   # Range is full (infinite)
        elif right > left:
            pass                                        # Range already passed as left,right
        elif size > 0:
            left,right = (addr,addr+size)               # Range was (addr,size) and non-empty
        else:
            left,right = (0,0)                          # Range is empty

        self.left = left
        self.right = right
        self.addr = left
        self.size = right-left
        self.full = (left == -QurtInfinity and right == QurtInfinity)
        self.container = container

        if self.size > 0:
            if templates:
                for t in templates:
                    self.__dict__.update(t.__dict__)    # Add each template's attributes in order
            self.__dict__.update(kw)                    # Add new attributes passed with the creation request
            for k,v in self.__dict__.items():
                if v == None:
                    del self.__dict__[k]                # Delete attributes which are set to None

    def accepts(self, other):
        #
        # Called to check if self and other can be merged
        #
        # If they can be merged, should return the template
        #  array for merging the two.  Generally the template
        #  array will be either [self,other] to merge the
        #  attributes of other on top of self, or it will be
        #  [other] to replace the attributes.
        #
        # If they cannot be merged, throw an appropriate
        #  exception.
        #
        return [other]                                  # Base class behavior if not overridden is
                                                        #  to accept all merges, and to replace the
                                                        #  attributes rather than merging.

    def merge(self, other):
        #
        # Merge other over top of self
        #
        if self.left >= other.right:
            return                                      # No overlap
        if other.left >= self.right:
            return                                      # No overlap
        templates = self.accepts(other)
        newobjs = [self.__class__(left=self.left,
                                  right=other.left,
                                  templates=[self],
                                  container=self.container),
                   templates[0].__class__(left=max(self.left, other.left),
                                          right=min(self.right, other.right),
                                          templates=templates,
                                          container=self.container),
                   self.__class__(left=other.right,
                                  right=self.right,
                                  templates=[self],
                                  container=self.container)]
        ix = self.container.ranges.index(self)
        self.container.ranges[ix:ix+1] = [obj for obj in newobjs if obj]

class AllocatedRange(QurtAddressRange):
    def accepts(self, other):
        if isinstance(other,AllocatedRange):
            if self.reserve_only:
                return [other]
            if other.reserve_only:
                return [self]
            raise Exception('Two entities attempting to allocate same address:\n%r\n%r' % (self,other))
        return [self,other]

class QurtAddressSpace(object):
    #
    # Describe an entire address space.  An address space is basically
    #  represented by an array of QurtAddressRange objects (or objects
    #  derived from QurtAddressRange).  The individual range objects
    #  cover the entire range from address -QurtInfinity to +QurtInfinity,
    #  thus making no assumptions about the size of the address space.
    #  There are no gaps; undescribed or unfilled areas in an address
    #  space are filled with ranges with no attributes.
    #
    def __init__(self):
        self.ranges = [QurtAddressRange(full=True, container=self)]
    def __iadd__(self, other):
        for x in self.ranges[:]:
            x.merge(other)
        return self
    def alloc(self, **kw):
        tmp = AllocatedRange(**kw)
        self += tmp
        return tmp
    def report(self):
        for x in self.ranges:
            print x
    def alloc_first_fit(self, minsize = 0x1000, maxsize = 0xFFFFF000, **kw):
        for r in self.ranges:
            if not isinstance(r,AllocatedRange):
                if r.size >= minsize:
                    return self.alloc(addr = r.addr, size = min(r.size, maxsize), **kw)
        return None
    def find(self, size, align, **kw):
        # Same as alloc but find the "best fit" range with size and align
        alist = [r for r in self.ranges if not isinstance(r,AllocatedRange)]
        alist.sort(key=lambda r: (r.size,r.addr))
        for r in alist:
            padleft = alignup(r.addr,align) - r.addr
            if r.size >= size + padleft:
                return self.alloc(addr = r.addr+padleft,
                                  size = size,
                                  **kw)
        return None
    def find_optimal_match(self, size, match_addr=0, **kw):
        #
        # Find the biggest TLB entry used in the specified input range
        #
        tlb_size = 1 << 24
        while tlb_size > 0x1000:
            if match_addr + size >= alignup(match_addr, tlb_size) + tlb_size:
                #
                # We use a TLB entry of this size
                #
                break
            tlb_size >>= 2
        alist = [r for r in self.ranges if not isinstance(r,AllocatedRange)]
        alist.sort(key=lambda r: (r.size,r.addr))
        for r in alist:
            base_addr = alignup(r.addr,0x1000)
            misalign = (match_addr - base_addr) & (tlb_size-1)
            base_addr += misalign
            if base_addr + size <= r.right:
                return self.alloc(addr = base_addr,
                                  size = size,
                                  **kw)
        return None
    def find_optimal(self, size, tlblimit=0x100000, **kw):
        # Same as alloc but find the "most optimal fit" range for the given
        #  size.  Here, "most optimal" means the one that uses the fewest
        #  TLB entries.
        total_size = alignup(size, 0x1000)
        largest_tlb = 0x1000
        while 4*largest_tlb <= min(tlblimit,total_size):
            largest_tlb *= 4
        alist = [r for r in self.ranges if not isinstance(r,AllocatedRange)]
        alist.sort(key=lambda r: (r.size,r.addr))
        for r in alist:
            base_addr = alignup(r.addr,0x1000)
            esize = 0x1000
            while esize < largest_tlb:
                # Compute the number of TLB entries we want to see of size (esize)
                needed = (total_size / esize) & 3
                # Compute the number of TLB entries needed from base_addr to reach
                #  the next size
                delta = (-(base_addr / esize)) & 3
                if delta > needed:
                    base_addr += esize * (delta-needed)
                esize *= 4
            if base_addr + total_size <= r.right:
                return self.alloc(addr = base_addr,
                                  size = total_size,
                                  **kw)
        return None

def convert_size(s):
    #
    # Convert the string to an integer.
    # Permit suffixes of 'K', 'KB', 'M', 'MB', 'G', 'GB'
    #  which correspond to the appropriate multipliers.
    # The suffix matching is non-case-sensitive.
    #
    import re
    p = re.split('(?i)([KMG]B?)', s, 1)      # Split with regex that matches K, M, G with an optional B, case-insensitive
    shift_count = 0
    if len(p) > 1:
        if p[2]:                             # The size suffix didn't come at the end of the string
            raise ValueError('Improper size format: %s' % s)
        shift_count = {'K':10,'M':20,'G':30}[p[1][0]]
    try:
        ret = int(p[0],0) << shift_count
    except ValueError:
        # Allow things like 1.5M or 7.5K
        ret = int(round(float(p[0]) * (2**shift_count)))
    return ret

def aligndown(n, a):
    return n & ~(a-1)

def alignup(n, a):
    return (n+(a-1)) & ~(a-1)

def run_script(main):
    from traceback import format_exc
    from sys import argv, stderr
    try:
        raise SystemExit(main(argv))
    except (SystemExit, KeyboardInterrupt):
        raise
    except Exception, err:
        print >> stderr, "%s\n%s\n%s: FAILED\n%s" % ('*'*72, format_exc(), argv[0], '*'*72)
        raise SystemExit(1)

_BookmarkDict = dict()

def BookmarkWrite(bookmark, f, s):
    #
    # BookmarkWrite() executes f.write(s).  The argument "bookmark"
    #  is expected to be either a string not beginning with an @ sign,
    #  which sets a bookmark before writing, or a string beginning
    #  with an @ sign, which rewinds to a bookmark before writing.
    #
    # Example:
    #   BookmarkWrite("HEADER1", f, 'foo')        # Writes the string 'foo'
    #   ... followed later by ...
    #   BookmarkWrite("@HEADER1", f, 'bar')       # Overwrites 'foo' with 'bar'
    #
    if bookmark.startswith('@'):
        f.flush()
        loc = _BookmarkDict[bookmark[1:]]
        f.seek(loc)
    else:
        loc = f.tell()
        _BookmarkDict[bookmark] = loc
    f.write(s)
    f.flush()
    return (loc,len(s))

class SubFile(object):
    #
    # Returns a read-only file-like object which returns only the data beginning
    #  at offset start, and extending for len bytes.
    # Raises an exception during reading if the data cannot be properly read.
    #
    def __init__(self, f, start, len):
        self._fbase = f
        self._start = start
        self._end = start+len
        self.seek(0)
    def close(self):
        self._fbase = None
    def seek(self, offset, whence=0):
        if whence == 0:
            self._loc = self._start + offset
        elif whence == 1:
            self._loc = self._loc + offset
        elif whence == 2:
            self._loc = self._end + offset
        else:
            self._loc = self._end + 1   # Force an error
        if self._loc < self._start or self._loc > self._end:
            raise IOError()
    def tell(self):
        return self._loc - self._start
    def read(self, count = -1):
        if count < 0:
            count = self._end
        count = min(count,self._end - self._loc)
        self._fbase.seek(self._loc)
        ret = self._fbase.read(count)
        N = len(ret)
        if N != count:
            raise IOError('SubFile of %s: premature EOF' % self._fbase.name)
        self._loc += N
        return ret

QurtHeaderDict = dict()

class QurtHeaderCodec(object):
    #
    # A base class from which specific header coder/decoders can be built.
    # Developed to easily build ELF header coder/decoders.
    #
    # The easiest way to do this is to define the variable _format
    #  in the sub-class.
    #
    # _format can be a string which is separated on whitespace, and
    #  which gives in order any number of repeats of: attribute name,
    #  struct.unpack format, and printable format.  The string can
    #  span multiple lines for readability.
    #
    # Example which parses the C struct:
    #   struct foo {
    #     int16_t a;
    #     int16_t b;
    #     uint32_t c;
    #   };
    #
    # _format = 'a h %d b h %d c L %u'
    #
    # Another important option in the sub-class is the ability to
    #  add the _legal() function, which should self-inspect the
    #  object and throw an appropriate exception if the fields
    #  are inconsistent or malformed.
    #

    def __init__(self,data,**kw):
        if isinstance(data,dict):
            self.__dict__.update(data)
        else:
            import struct
            val = QurtHeaderDict.get(self.__class__,None)
            if val == None:
                tmp = self._format.split()
                attrnames = tmp[0::3]
                structfmt = tmp[1::3]
                printfmt = tmp[2::3]
                fmt = '<'+''.join(structfmt)
                val = dict(_decode = fmt,
                           _size = struct.calcsize(fmt),
                           _attrnames = attrnames,
                           _printfmt = printfmt)
                QurtHeaderDict[self.__class__] = val
            self.__dict__.update(val)
            if hasattr(data,'read'):
                # File like object; read from it
                data = data.read(self._size)
            if data != None:
                tup = struct.unpack(self._decode,data)
                self.__dict__.update(zip(self._attrnames,tup))
            self.__dict__.update(kw)
            self._legal()               # Check for legality after decoding
    def _legal(self):
        pass
    def copy(self):
        return self.__class__(self.__dict__)
    def output(self):
        v = ''
        if not getattr(self,'temp',False):
            import struct
            self._legal()               # Check for legality before encoding
            v = struct.pack(self._decode,*[getattr(self,a) for a in self._attrnames])
        return v

class Elf32Header(QurtHeaderCodec):
    _format = '''
        e_ident     16s     %r
        e_type        H     %u
        e_machine     H     %u
        e_version     L     %u
        e_entry       L   0x%X
        e_phoff       L     %u
        e_shoff       L     %u
        e_flags       L   0x%X
        e_ehsize      H     %u
        e_phentsize   H     %u
        e_phnum       H     %u
        e_shentsize   H     %u
        e_shnum       H     %u
        e_shstrndx    H     %u
    '''
    def _legal(self):
        if self.e_ehsize != 52:
            raise Exception('Bad ELF format')
        if self.e_phentsize != 32 and self.e_phnum != 0:
            raise Exception('Bad ELF format')
        if self.e_shentsize != 40 and self.e_shnum != 0:
            raise Exception('Bad ELF format')

class Elf32ProgramHeader(QurtHeaderCodec):
    _format = '''
        p_type        L   0x%X
        p_offset      L   0x%X
        p_vaddr       L   0x%X
        p_paddr       L   0x%X
        p_filesz      L   0x%X
        p_memsz       L   0x%X
        p_flags       L   0x%X
        p_align       L   0x%X
    '''
    PT_NULL = 0
    PT_LOAD = 1
    PT_PHDR = 6
    PT_GNU_STACK = 0x6474E551
    def contents(self):
        #
        # Return a file-like object which returns the file contents associated with the program header.
        # In order for this to work, the containing file object must have been installed as an
        #  attribute under self.f.
        #
        return SubFile(self.f, self.p_offset, self.p_filesz)
    def oneline(p):
        return 'Vaddr %08X-%08X, Paddr %08X-%08X' % (p.p_vaddr, p.p_vaddr + p.p_memsz-1, p.p_paddr, p.p_paddr + p.p_memsz-1)

class Elf32SectionHeader(QurtHeaderCodec):
    _format = '''
        sh_name       L   0x%X
        sh_type       L   0x%X
        sh_flags      L   0x%X
        sh_addr       L   0x%X
        sh_offset     L   0x%X
        sh_size       L   0x%X
        sh_link       L   0x%X
        sh_info       L   0x%X
        sh_addralign  L   0x%X
        sh_entsize    L   0x%X
    '''

    SHF_WRITE = 1
    SHF_ALLOC = 2
    SHF_EXECINSTR = 4
    SHF_MERGE = 16
    SHF_STRINGS = 32
    SHN_UNDEF = 0
    SHT_NULL = 0
    SHT_PROGBITS = 1
    SHT_SYMTAB = 2
    SHT_STRTAB = 3
    SHT_RELA = 4
    SHT_NOBITS = 8

    def contents(self):
        #
        # Return a file-like object which returns the file contents associated with the section header.
        # In order for this to work, the containing file object must have been installed as an
        #  attribute under self.f.
        #
        # Note that SHT_NOBITS sections have no file contents and return an empty string.  If it
        #  becomes desirable in the future, we might consider having SHT_NOBITS return a string
        #  of zero bytes of the appropriate section length.
        #
        if self.sh_type == self.SHT_NOBITS:
            return SubFile(self.f, 0, 0)
        else:
            return SubFile(self.f, self.sh_offset, self.sh_size)
    def oneline(s):
        extra = ''
        if getattr(s,'s_internal',None):
            extra = ', internal'
        return 'Addr %08X-%08X, name %s%s' % (s.sh_addr, s.sh_addr+s.sh_size-1, s.name, extra)

class Elf32Sym(QurtHeaderCodec):
    _format = '''
        st_name       L   0x%X
        st_value      L   0x%X
        st_size       L   0x%X
        st_info       B   0x%X
        st_other      B   0x%X
        st_shndx      H   0x%X
    '''
    STB_LOCAL = 0
    STB_GLOBAL = 1
    SHN_COMMON = 0xFFF2
    def get_binding(self):
        return (self.st_info >> 4)
    def set_binding(self, newval):
        self.st_info += (newval-self.get_binding()) << 4

class Elf32Rela(QurtHeaderCodec):
    _format = '''
        r_offset      L   0x%X
        r_info        L   0x%X
        r_addend      L   0x%X
    '''
    def get_sym(self):
        return (self.r_info >> 8)
    def set_sym(self, newval):
        self.r_info += (newval-self.get_sym()) << 8

def strtabify(shdr):
    ret = '\0'
    for s in sorted(shdr, key=lambda s: -len(s.name)):
        if s.output():
            key = s.name+'\0'
            N = ret.find(key)
            if N < 0:
                ret += key
                N = ret.find(key)
            s.sh_name = N
    return ret

compiler_loc_cache = dict()

def qurt_get_compiler(path=None):
    #
    #  Given a path to a Hexagon tools directory, return the name of the
    #   executable representing the compiler for that tool chain.  The
    #   path provided is expected to be one of these:
    #     path to root of tools directory tree
    #     path to the directory containing the compiler executable
    #     name of a file in the same directory as the executable
    #      (the file need not exist)
    #
    #  For instance, if the executable is:
    #     m:\hexagon\7.7.7\Tools\bin\hexagon-clang.exe
    #  any of the following paths should find the right executable:
    #     m:\hexagon\7.7.7            <---  This is the preferred way
    #     m:\hexagon\7.7.7\Tools\bin
    #     m:\hexagon\7.7.7\Tools\bin\foo.bar
    #
    #  This function caches its result in the directory compiler_loc_cache,
    #   so it is not a performance issue to call it often.
    #
    if path in compiler_loc_cache:
        return compiler_loc_cache[path]

    ret = qurt_find_compiler(path)
    if ret:
        compiler_loc_cache[path] = ret
        return ret

    raise Exception('Cannot find compiler for path %s' % path)

def qurt_find_compiler(path):
    def enumlocs(path, os):
        if path:
            yield os.path.join(path,'Tools','bin','hexagon-clang.exe')
            yield os.path.join(path,'Tools','bin','hexagon-clang')
            yield os.path.join(path,'gnu','bin','hexagon-gcc.exe')
            yield os.path.join(path,'gnu','bin','hexagon-gcc')
            yield os.path.join(path,'hexagon-clang.exe')
            yield os.path.join(path,'hexagon-clang')
            yield os.path.join(path,'hexagon-gcc.exe')
            yield os.path.join(path,'hexagon-gcc')
            yield os.path.join(os.path.dirname(path),'hexagon-clang.exe')
            yield os.path.join(os.path.dirname(path),'hexagon-clang')
            yield os.path.join(os.path.dirname(path),'hexagon-gcc.exe')
            yield os.path.join(os.path.dirname(path),'hexagon-gcc')
        else:
            import distutils.spawn
            yield distutils.spawn.find_executable('hexagon-clang')
            yield distutils.spawn.find_executable('hexagon-gcc')

    import os
    for loc in enumlocs(path, os):
        if loc and os.path.isfile(loc):
            return loc
    return None

def run_command(*ls):
    #
    #  Run a subprocess.  Handles string arguments, and
    #   iterable arguments such as lists and tuples, which
    #   are recursively expanded.  Non-strings and
    #   non-iterables are added to the list after having
    #   str() called on them, with the notable exception
    #   of None, which is silently dropped.
    #
    #  The first argument should be the program to run:
    #
    #  Example:
    #   PROGRAM = 'gcc'
    #   CFLAGS = ['-g', '-mv5']
    #   EXTRAFLAGS = ()
    #   OUTPUT = 'foo.elf'
    #   INPUTS = glob.glob('*.c')
    #   TRAILER = None
    #   run_command(PROGRAM, CFLAGS, EXTRAFLAGS, '-o', OUTPUT, INPUTS, TRAILER)
    #
    #  which basically executes this (as would be given to a shell):
    #   gcc -g -mv5 -o foo.elf *.c
    #
    #  Print the command before executing it.
    #
    #  If the process can't be executed, or if it returns a
    #   non-zero exit code, an Exception is raised.
    #

    from subprocess import call
    from sys import stdout

    def expand(src, dst = None):
        if dst == None:
            dst = list()
        if hasattr(src,'__iter__'):
            for arg in src:
                expand(arg,dst)
        elif src != None:
            dst.append(str(src))
        return dst

    args = expand(ls)
    print ' '.join(args)
    stdout.flush()
    if call(args):
        raise Exception('Subprocess returned with non-zero exit code')

def multiscript(G, **kw):
    import imp, os
    for k,v in kw.items():
        G[k] = imp.load_source(k, os.path.join(os.path.dirname(G['__file__']), v))
    if G['__name__'] == '__main__':
        import sys
        return sys.argv
    else:
        return None

class QurtElfContainer(object):
    #
    #  Given a file name, create an ELF container object.
    #  This can contain object files or libraries (archives)
    #    of multiple object files.
    #
    #  Example usage:
    #    for libname, objname, fi in QurtElfContainer('foo.a'):
    #        print 'Library name is %s' % libname
    #        print 'Object name is %s' % objname
    #        print 'File object is %s' % fi
    #
    def __init__(self, fname):
        self.objs = None
        try:
            self.fi = open(fname, 'rb')
            self.fname = fname
        except IOError:
            #
            #  Can't open file -- silently create an empty container
            #
            self.objs = []
        if self.objs == None:
            #
            #  Could open file -- go parse it
            #
            self.objs = []
            self.startup()
    def __getitem__(self, *ls, **kw):
        return self.objs.__getitem__(*ls, **kw)
    def __len__(self):
        return len(self.objs)
    def importfile(self, fi, fname, allow_ar=False, libname=None):
        import struct
        if allow_ar:
            fi.seek(0)
            if fi.read(8) == '!<arch>\n':
                longnames = ''
                loc = 8
                while True:
                    if (loc & 1):
                        loc += 1
                    fi.seek(loc)
                    z=fi.read(60)
                    if len(z) != 60:
                        break
                    baseloc = loc+60
                    name, timestamp, owner, group, mode, size, magic = struct.unpack('16s12s6s6s8s10s2s', z)
                    if magic != '`\n':
                        break
                    size = int(size)
                    loc = baseloc+size
                    name = name.rstrip(' ')
                    if name == '/':
                        # ranlib info -- skip it
                        continue
                    elif name == '//':
                        # long name table -- read it
                        longnames = fi.read(size)
                        continue
                    elif name.startswith('/'):
                        name = longnames[int(name[1:]):].split('/')[0]
                    else:
                        name = name.split('/')[0]
                    self.importfile(SubFile(fi, baseloc, size), name, allow_ar=False, libname=fname)
        fi.seek(0)
        if fi.read(4) == '\177ELF':
            self.objs.append((libname, fname, fi))
    def startup(self):
        self.importfile(self.fi, self.fname, allow_ar=True)

class Elf32Object(object):
    #
    #  Represent a relocatable ELF object file (.o) in
    #   a Pythonic form.
    #
    def __init__(self, fi, **kw):
        self.__dict__.update(kw)
        fi.seek(0)
        self.ehdr = Elf32Header(fi)
        fi.seek(self.ehdr.e_shoff)
        self.shdr = [Elf32SectionHeader(fi, f=fi, obj=self) for _ in range(self.ehdr.e_shnum)]
        strtab = self.shdr[self.ehdr.e_shstrndx].contents().read()
        for s in self.shdr:
            s.name = strtab[s.sh_name:].split('\0')[0]
    def defined_globals(self):
        #
        #  Return an iterator which returns:
        #    (symbol name, section)
        #  for every defined global symbol in the object
        #
        for s in self.shdr:
            if s.sh_type == s.SHT_SYMTAB:
                strtab = self.shdr[s.sh_link].contents().read()
                fi = s.contents()
                for _ in range(s.sh_size / s.sh_entsize):
                    sym = Elf32Sym(fi)
                    if sym.get_binding() == sym.STB_GLOBAL:
                        if (sym.st_shndx > 0 and sym.st_shndx < len(self.shdr)):
                            sym.name = strtab[sym.st_name:].split('\0')[0]
                            yield sym.name, self.shdr[sym.st_shndx], sym.st_size
    def defined_common(self):
        #
        #  Return an iterator which returns:
        #    (symbol name, section)
        #  for every defined common symbol in the object
        #
        for s in self.shdr:
            if s.sh_type == s.SHT_SYMTAB:
                strtab = self.shdr[s.sh_link].contents().read()
                fi = s.contents()
                for _ in range(s.sh_size / s.sh_entsize):
                    sym = Elf32Sym(fi)
                    if sym.get_binding() == sym.STB_GLOBAL:
                        if (sym.st_shndx == sym.SHN_COMMON):
                            sym.name = strtab[sym.st_name:].split('\0')[0]
                            yield sym.name, sym.st_size

class Elf32TrialLink(object):
    #
    #  This class simulates some of the behavior of a linker.
    #  Given a set of objects, it finds the "cross-reference"
    #   list of all of the relocations -- for instance,
    #    section foo.a:bar.o(.text) has a reference to plugh.o(.data)
    #
    def __init__(self):
        self.ispecs = []
        self.objs = []
    def add_object(self, obj):
        self.objs.append(obj)
    def add_ispec(self, ispec):
        temp = ispec.split()
        if not temp:
            return
        if temp[0].startswith('#'):
            return
        if len(temp) != 3:
            raise Exception('Bad island specification: %s' % (temp,))
        self.ispecs.append(tuple(temp))
    def run(self):
        import fnmatch, os
        #
        #  First create an empty COMMON section for every object
        #
        for obj in self.objs:
            obj.shdr.append(Elf32SectionHeader(None,
                                               sh_name = 0,
                                               sh_type = Elf32SectionHeader.SHT_NOBITS,
                                               sh_flags = Elf32SectionHeader.SHF_ALLOC,
                                               sh_addr = 0,
                                               sh_offset = 0,
                                               sh_size = 0,
                                               sh_link = 0,
                                               sh_info = 0,
                                               sh_addralign = 0,
                                               sh_entsize = 0,
                                               name = 'COMMON'))
        #
        #  Next mark all sections as island resident or not.
        #
        for obj in self.objs:
            for section in obj.shdr:
                section.resident = False
        for libmatch, objmatch, secmatch in self.ispecs:
            for obj in self.objs:
                for match, target in [(libmatch, obj.libname),
                                      (objmatch, obj.objname)]:
                    if match == '*':
                        continue                # Asterisk matches everything
                    if target == None:
                        break                   # Target None only matches asterisk
                    if fnmatch.fnmatch(target, match):
                        continue                # It matches
                    if fnmatch.fnmatch(os.path.basename(target), match):
                        continue                # It matches
                    else:
                        break
                else:
                    #
                    #  The lib/obj match so far.  Now check sections
                    #
                    for section in obj.shdr:
                        if section.sh_flags & section.SHF_ALLOC:
                            if fnmatch.fnmatch(section.name, secmatch):
                                section.resident = True
        #
        #  Next make a dictionary associating globally defined symbols
        #   with their ELF section.
        #
        global_symdefs = dict()
        for obj in self.objs:
            for name, section, size in obj.defined_globals():
                if name in global_symdefs:
                    if global_symdefs[name][0].name == section.name and section.name.startswith('.gnu.linkonce.'):
                        #
                        #  Ignore multiple definitions in .gnu.linkonce.* for now.
                        #  TBD -- what if this ends up island resident?
                        #
                        continue
                    print '%s defined at %s %s %s' % (name,
                                                      global_symdefs[name][0].obj.libname,
                                                      global_symdefs[name][0].obj.objname,
                                                      global_symdefs[name][0].name)
                    print '%s defined at %s %s %s' % (name,
                                                      section.obj.libname,
                                                      section.obj.objname,
                                                      section.name)
                    raise Exception('Multiple definitions of %s' % name)
                global_symdefs[name] = (section, size)
        #
        #
        #
        print 'Trial Link!'

def asm_to_C(asm_lines):
    #
    #  Pass this function a list of assembly language lines.
    #  It will return a properly formatted __asm__() directive
    #   including terminating semicolon which is suitable for
    #   inclusion in a C file.
    #
    #  Basically, this just takes care of making sure that the
    #   concatenated list of assembly language lines is
    #   properly quoted for inclusion in __asm__().
    #
    #  Uses unichr(1600) internally to represent backslash;
    #   converts them back to actual backslashes at the end.
    #
    BSLASH = unichr(1600)
    #
    #  First build the concatenated assembly code in Unicode.  We use unicode so that
    #   the internal representation of backslash (BSLASH) is distinct from all other
    #   characters.
    #
    S = unicode('\n'.join(asm_lines))
    #
    #  Next replace all backslashes with double BSLASH
    #
    S = S.replace('\\', BSLASH+BSLASH)
    #
    #  Next replace all double quotation marks with (BSLASH+double quote)
    #
    S = S.replace('"', BSLASH+'"')
    #
    #  Next replace all tab characters with (BSLASH+t)
    #
    S = S.replace('\t', BSLASH+'t')
    #
    #  Next replace all newlines with (BSLASH+n+double quote+newline+space+space+space+space+double quote)
    #
    S = S.replace('\n', BSLASH+'n"\n    "')
    #
    #  Next replace all BSLASH with backslashes
    #
    S = S.replace(BSLASH, '\\')
    #
    #  Convert it back to a non-Unicode string
    #
    S = str(S)
    #
    #  Wrap it in an __asm__ construct and return it.
    #
    return '  __asm__(\n    "%s");\n' % S
