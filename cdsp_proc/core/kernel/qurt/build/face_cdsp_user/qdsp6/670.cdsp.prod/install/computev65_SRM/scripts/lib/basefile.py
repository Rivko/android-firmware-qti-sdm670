#=============================================================================
#
#                        basefile.py
#
# GENERAL DESCRIPTION
#
# EXTERNAL FUNCTIONS
#        None.
#
# INITIALIZATION AND SEQUENCING REQUIREMENTS
#        None.
#
#   Copyright 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.
#=============================================================================

#
#  File-like objects for use by QuRT build scripts.
#

class ZeroFileImpl(object):
    #
    #  A file-like object that returns an endless stream of zeroes.
    #
    #  Note that because the virtual file size is infinite, you can't
    #   use the read() method to read the rest of the file.  If you
    #   call read() with no argument supplied, 64K of zeroes are
    #   returned.
    #
    #  If created with flag is_empty set to True, the file-like
    #   object acts as a placeholder, and throws an exception if
    #   you actually attempt to read it.
    #

    ZeroFileData = '\0' * 0x10000           # 64K of zeroes

    def __init__(self, is_empty=False):
        self.is_empty = is_empty
    def close(self):
        pass
    def read(self,len=0x10000):
        if self.is_empty:
            raise IOError('EmptyFile: Attempt to read uninitialized content')
        return self.ZeroFileData[:max(0,len)]
    def seek(self,offset,whence=0):
        pass
    def tell(self):
        return 0

#
#  Create two "singleton" file objects of class ZeroFileImpl.
#
#  ZeroFile is intended to be used for .bss type of content;
#   uninitialized content which is intended to be treated as
#   all zeroes.
#
#  EmptyFile is intended to be used for unallocated space;
#   it behaves like ZeroFile except that it throws an exception
#   if you actually try to read it.
#

ZeroFile = ZeroFileImpl()

EmptyFile = ZeroFileImpl(is_empty=True)

class ConcatFile(object):

    #
    #  ConcatFile is a class that represents a file-like object which consists
    #   of a concatenation of an arbitrary number of file "slices" -- where
    #   a file slice is represented by a file-like object, a seek offset, and
    #   a non-negative length.  (A zero length ConcatFile is possible but not
    #   particularly useful.)
    #
    #  Note that when a ConcatFile is created, its length is set.  It can't
    #   grow or shrink.  New slices can be overlaid on top of the existing
    #   file, and those new slices replace whatever was there before.
    #
    #  To create a ConcatFile, three arguments are needed:  the file-like
    #   object, seek offset, and length of the initial slice content.
    #
    #  Note that some of the slices of a ConcatFile may refer to other portions
    #   of the same ConcatFile; this is desirable and intentional.  Care should
    #   should be taken by the user not to have a "reference loop" in the
    #   data structure.  For instance, if the first 1K of the ConcatFile is
    #   a reference to the second 1K, and the second 1K is a reference to the
    #   first 1K, none of that content will be readable -- it will cause a
    #   RunTime recursion error if reading it is attempted.
    #

    def __init__(self, fileobj, offset, length):
        self.contents = [(fileobj, offset, length)]
        self.loc = 0
        self.size = length
    def insert(self, loc, fileobj, offset, length, new=True, overwrite=False):
        #
        #  Insert at the given (loc) the file slice described by (fileobj,offset,length)
        #
        #  If new is True (the default), throw an exception if this overwrites any
        #   existing non-empty content.
        #
        #  If overwrite is True, throw an exception is this overwrites any
        #   contents which is not actually backed by real data.
        #
        #  Returns a ConcatFile which overlays the inserted portion exactly.
        #
        if not (0 <= loc <= loc+length <= self.size):
            raise IOError('Improper ConcatFile insertion')

        if length > 0:
            if new:
                for fi, off, count in self.subfiles(loc, loc+length):
                    if not (fi is EmptyFile):
                        raise IOError('ConcatFile insertion would overwrite existing content')

            if overwrite:
                for fi, off, count in self.subfiles(loc, loc+length):
                    if isinstance(fi,ZeroFileImpl):
                        raise IOError('ConcatFile overwrite does not overlap existing content')

            newcontents = list(self.subfiles(0,loc))
            newcontents.append((fileobj, offset, length))
            newcontents.extend(self.subfiles(loc+length,self.size))
            self.contents = newcontents

        return ConcatFile(self,loc,length)
    def close(self):
        del self.contents               # Don't close subfiles, just delete our reference to them
    def read(self,count=None):
        #
        #  Be careful in read() with the current seek offset (self.loc);
        #   The read() calls that we make here may change self.loc, if the
        #   file has references within itself.  Read self.loc once at the
        #   beginning of this method, and write it back once at the end.
        #   In between, don't make any assumptions about its value.
        #
        end = self.size
        start = min(end, self.loc)
        if count != None and count >= 0:
            end = min(end, start+count)
        data = []
        if end > start:
            for fi, off, count in self.subfiles(start,end):
                fi.seek(off)
                while count:
                    s = fi.read(count)
                    if s == '':
                        raise IOError('Premature EOF on subfile')
                    data.append(s)
                    count -= len(s)
            self.loc = end
        return ''.join(data)
    def seek(self,loc,whence=0):
        if whence == 1:
            loc += self.loc
        elif whence == 2:
            loc += self.size
        self.loc = max(0,loc)
    def tell(self):
        return self.loc
    def subfiles(self,start,stop):
        skip = max(0,start)
        need = max(0,stop-skip)
        for fi, off, count in self.contents:
            tmp = min(skip,count)               # How many bytes of this extent do we skip?
            skip -= tmp
            count -= tmp
            off += tmp
            tmp = min(need,count)               # How many bytes of this extent do we need?
            need -= tmp
            if tmp:
                yield (fi, off, tmp)
        if need:
            raise IOError('Improper subfile construct')

def VAddrFile():

    #
    #  VAddrFile() returns a new empty virtual 4GB ConcatFile which covers an
    #   entire virtual address range from 0 - 0xFFFFFFFF.
    #

    return ConcatFile(EmptyFile,0,2**32)

if __name__ == '__main__':
    m = ConcatFile(EmptyFile,0,16)
    try:
        m.read()
        raise Exception('EmptyFile should have raised an IOError')
    except IOError:
        pass

    z = ConcatFile(ZeroFile,0,16)
    s = z.read()
    assert s == '\0' * 16
    s = z.read()
    assert s == ''

    import StringIO

    z = ConcatFile(StringIO.StringIO('abcdefgh'),0,8)
    s = z.read(4)
    assert s == 'abcd'
    s = z.read()
    assert s == 'efgh'
    s = z.read()
    assert s == ''

    z = ConcatFile(StringIO.StringIO('0123abcdefgh4567'),4,8)
    s = z.read(4)
    assert s == 'abcd'
    s = z.read()
    assert s == 'efgh'
    s = z.read()
    assert s == ''

    z = ConcatFile(z,2,4)
    s = z.read(4)
    assert s == 'cdef'
    s = z.read()
    assert s == ''

    z = ConcatFile(EmptyFile,0,16)
    r = z.insert(8,StringIO.StringIO('abcdefgh'),0,8)
    z.seek(8)
    print repr(z.read(8))
    r.seek(0)
    print repr(r.read())
    z.insert(9,StringIO.StringIO('B'),0,1, new=False)
    z.seek(8)
    print repr(z.read(8))
    r.seek(0)
    print repr(r.read())
