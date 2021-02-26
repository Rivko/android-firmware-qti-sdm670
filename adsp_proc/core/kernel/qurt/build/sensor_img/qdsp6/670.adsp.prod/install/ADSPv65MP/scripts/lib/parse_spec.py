#=============================================================================
#
#                                    parse_spec.py
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
from ezxml import Element

def c_stringify(s, double=False):
    # Prepend the string with its own length encoded
    #  as a 32-bit integer
    import struct
    s = struct.pack('<L', len(s)) + s
    col = 0
    ret = ''
    pre = ''
    for c in s:
        o = ord(c)
        if c == '\n':
            e = '\\n'               # Encode newline as \n
        elif o < 32 or o > 126:
            e = '\\%03o' % o        # Encode other non-printables as \<octal>
        elif c in r'\"':
            e = '\\' + c            # Encode backslash and double quote as \<char>
        else:
            e = c                   # Other printable characters are used directly
        if col+len(e) > 71:
            ret += '"\n'            # Terminate current line
            col = 0
        if col == 0:
            ret += pre+'"'          # Start new line
            col = len(pre)+1
        ret += e                    # Add encoded character
        col += len(e)
        if c == '\n':
            col = 80                # If we just added a new-line, force line break
        pre = '   '
    if col > 0:
        ret += '"'

    # If double, return with all backslashes doubled so that the resulting string
    #  is appropriate as a regular expression substitution.

    if double:
        ret = '\\\\'.join(ret.split('\\'))

    return (ret,len(s))

class XMLPassThrough(object):
    def __init__(self, __name, el=None):
        self.name = __name
        self.tag = __name
        self.el = el
    def walkdom(self, el):
        self.cfg.build_elements.append(el)
        return XMLPassThrough(self.name, el)

class QurtImageParse(Element):

    # Our top level wrapper around the Element class.
    # This allows us to selectively modify the behavior
    #  of the top level of the parser, and allows us
    #  to do arbitrary reads and writes of the raw XML
    #  parse data.

    def __init__(self,cfg):
        from machine_xml  import Machine_el
        from kernel_xml   import Kernel_el
        from physpool_xml import PhysicalPool_el, SrmResources_el
        from program_xml  import Program_el

        cfg.c_stringify = c_stringify

        Element.__init__(self,
                         'image',
                         XMLPassThrough('build'),
                         Machine_el,
                         Kernel_el,
                         PhysicalPool_el,
                         SrmResources_el,
                         Program_el)

        # Set "cfg" correctly on the entire tree, as an attribute
        #  on each parsing object.
        empty = dict()
        elist = [self]
        while elist:
            e = elist.pop()
            e.cfg = cfg
            elist.extend(getattr(e,'elements',empty).values())

    # Our walkdom() function gets control after the
    #  raw XML has been parsed but before we've done
    #  any specific interpretation of the XML.
    # This is the spot where we can do out-of-band
    #  parsing or modifying of the XML contents.
    # For now, what we do is pretty simple...  If
    #  no <build> element is found at the top level,
    #  we do nothing special.
    # If one or more <build> elements are found at
    #  the top level, we create a string representation
    #  for the XML file <build> elements in C syntax and store
    #  it along with its length into these attributes:
    #   self.cfg['XMLCSTRING']
    #   self.cfg['XMLLENGTH']
    #  After creating that, we strip the <build>
    #  elements from the XML tree.  It is expected
    #  that all parsing of <build> elements will
    #  be done at a later point by qurt-image-build.py.

    def walkdom(self, el):

        pcount = 0
        for program in el.getElementsByTagName('program'):
            pmap = program.ownerDocument.createElement('program_name_map')
            pmap.setAttribute('name', program.getAttribute('name'))
            pmap.setAttribute('number', '%d' % pcount)
            self.cfg.build_elements.append(pmap)
            pcount += 1

        ret = Element.walkdom(self, el)

        (cstr,len) = c_stringify(''.join(['<image>'] +
                                         [e.toxml('utf-8') for e in self.cfg.build_elements] +
                                         ['</image>']), double=True)

        self.cfg['XMLCSTRING'] = cstr
        self.cfg['XMLLENGTH'] = '%u' % len

        return ret

    # Our do_parse() function is the "public" method
    #  that the upper level calls which actually
    #  does the parsing.  All of the needed inputs
    #  are attached to self.cfg already.

    def do_parse(self):
        s = self.cfg.spec_file
        if not self.cfg.spec_is_string:
            f = open(s, 'rb')
            s = f.read()
            f.close()
        self.cfg.parsed = self.parse_xml_str(s)

