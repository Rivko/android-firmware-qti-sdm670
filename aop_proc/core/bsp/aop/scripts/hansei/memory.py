#  ===========================================================================
#
#  Copyright (c) 2011 Qualcomm Technologies Incorporated.  
#  All Rights Reserved.
#  QUALCOMM Proprietary and Confidential.
#
#  ===========================================================================



class Memory:
    def __init__(self):
        self.hunks = []

    def load_hunk(self, address, data):
        overlapping_hunks = [hunk for hunk in self.hunks if hunk.overlaps(address, address + len(data) - 1)]

        # don't bother doing anything about overlaps yet; we'll just avoid them
        # we can worry about combining overlaid hunks later if we need to
        assert not overlapping_hunks

        hunk = MemoryHunk(address, data)
        self.hunks.append(hunk)
        
    def overwrite(self, address, data):
        overlapping_hunks = [hunk for hunk in self.hunks if hunk.overlaps(address, address + len(data) - 1)]
        
        # only support replacing a portion of a single hunk, do not deal with overwriting multiple hunks.
        assert len(overlapping_hunks) <= 1
        
        if overlapping_hunks:
            overlapping_hunks[0].overwrite(address, data)
        else:
            self.load_hunk(address,data)

    def read(self, address, count):
        data = ''
        while count > 0:
            # Find what hunk we start in.
            matching_hunks = [hunk for hunk in self.hunks if address in hunk]
            if not matching_hunks:
                raise IndexError('Address (0x%0.8x) does not fall inside a known memory hunk!' % address) 
            
            # Compute how much can come from this hunk.
            hunk = matching_hunks[0]
            limit = hunk.limit()
            readable = min(count, limit - address + 1)

            # Pull the data from the hunk and advance to whatever comes next.
            data    += hunk.read(address, readable)
            address += readable
            count   -= readable
        return data

class MemoryHunk:
    def __init__(self, address, data):
        self.data = data
        self.address = address

    def limit(self):
        return self.address + len(self.data) - 1

    def read(self, address, count):
        assert address in self, 'Base address not in this hunk.'
        assert (address + count - 1) in self, 'Entire range not within this hunk.'

        offset = address - self.address
        return self.data[offset:offset+count]

    def __contains__(self, address):
        return self.address <= address <= self.limit()

    # tests if the interval of addresses [start, end] overlaps this hunk
    #   (note that both start and end are *inclusive*)
    def overlaps(self, start, end):
        my_start = self.address
        my_end   = self.limit()
        return (start <= my_end and end >= my_start)
        
    def overwrite(self, start, data):
        # overwrite a piece of a given hunk
        start_addr = start-self.address
        self.data = self.data[:start_addr]+data+self.data[len(data)+start_addr:]


# Small unit test
if __name__ == '__main__':
    m = Memory()

    m.load_hunk(10, '\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13')
    m.load_hunk(30, '\x1e\x1f\x20\x21\x22\x23\x24\x25\x26\x27')

    assert m.read(10, -5) == ''
    assert m.read(10,  0) == ''
    assert m.read(10,  2) == '\x0a\x0b'
    assert m.read(10, 10) == '\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13'
    try:
        m.read(10, 11)
        raise
    except IndexError:
        pass

    assert m.read(35, -5) == ''
    assert m.read(35,  0) == ''
    assert m.read(35,  2) == '\x23\x24'
    assert m.read(36,  4) == '\x24\x25\x26\x27'
    try:
        m.read(38, 5)
        raise
    except IndexError:
        pass


