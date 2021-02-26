##############################################################################
# Australian Public Licence B (OZPLB)
# 
# Version 1-0
# 
# Copyright (c) 2007, Open Kernel Labs, Inc.
# 
# All rights reserved. 
# 
# Developed by: Embedded, Real-time and Operating Systems Program (ERTOS)
#               National ICT Australia
#               http://www.ertos.nicta.com.au
# 
# Permission is granted by National ICT Australia, free of charge, to
# any person obtaining a copy of this software and any associated
# documentation files (the "Software") to deal with the Software without
# restriction, including (without limitation) the rights to use, copy,
# modify, adapt, merge, publish, distribute, communicate to the public,
# sublicense, and/or sell, lend or rent out copies of the Software, and
# to permit persons to whom the Software is furnished to do so, subject
# to the following conditions:
# 
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimers.
# 
#     * Redistributions in binary form must reproduce the above
#       copyright notice, this list of conditions and the following
#       disclaimers in the documentation and/or other materials provided
#       with the distribution.
# 
#     * Neither the name of National ICT Australia, nor the names of its
#       contributors, may be used to endorse or promote products derived
#       from this Software without specific prior written permission.
# 
# EXCEPT AS EXPRESSLY STATED IN THIS LICENCE AND TO THE FULL EXTENT
# PERMITTED BY APPLICABLE LAW, THE SOFTWARE IS PROVIDED "AS-IS", AND
# NATIONAL ICT AUSTRALIA AND ITS CONTRIBUTORS MAKE NO REPRESENTATIONS,
# WARRANTIES OR CONDITIONS OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
# BUT NOT LIMITED TO ANY REPRESENTATIONS, WARRANTIES OR CONDITIONS
# REGARDING THE CONTENTS OR ACCURACY OF THE SOFTWARE, OR OF TITLE,
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, NONINFRINGEMENT,
# THE ABSENCE OF LATENT OR OTHER DEFECTS, OR THE PRESENCE OR ABSENCE OF
# ERRORS, WHETHER OR NOT DISCOVERABLE.
# 
# TO THE FULL EXTENT PERMITTED BY APPLICABLE LAW, IN NO EVENT SHALL
# NATIONAL ICT AUSTRALIA OR ITS CONTRIBUTORS BE LIABLE ON ANY LEGAL
# THEORY (INCLUDING, WITHOUT LIMITATION, IN AN ACTION OF CONTRACT,
# NEGLIGENCE OR OTHERWISE) FOR ANY CLAIM, LOSS, DAMAGES OR OTHER
# LIABILITY, INCLUDING (WITHOUT LIMITATION) LOSS OF PRODUCTION OR
# OPERATION TIME, LOSS, DAMAGE OR CORRUPTION OF DATA OR RECORDS; OR LOSS
# OF ANTICIPATED SAVINGS, OPPORTUNITY, REVENUE, PROFIT OR GOODWILL, OR
# OTHER ECONOMIC LOSS; OR ANY SPECIAL, INCIDENTAL, INDIRECT,
# CONSEQUENTIAL, PUNITIVE OR EXEMPLARY DAMAGES, ARISING OUT OF OR IN
# CONNECTION WITH THIS LICENCE, THE SOFTWARE OR THE USE OF OR OTHER
# DEALINGS WITH THE SOFTWARE, EVEN IF NATIONAL ICT AUSTRALIA OR ITS
# CONTRIBUTORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH CLAIM, LOSS,
# DAMAGES OR OTHER LIABILITY.
# 
# If applicable legislation implies representations, warranties, or
# conditions, or imposes obligations or liability on National ICT
# Australia or one of its contributors in respect of the Software that
# cannot be wholly or partly excluded, restricted or modified, the
# liability of National ICT Australia or the contributor is limited, to
# the full extent permitted by the applicable legislation, at its
# option, to:
# a.  in the case of goods, any one or more of the following:
# i.  the replacement of the goods or the supply of equivalent goods;
# ii.  the repair of the goods;
# iii. the payment of the cost of replacing the goods or of acquiring
#  equivalent goods;
# iv.  the payment of the cost of having the goods repaired; or
# b.  in the case of services:
# i.  the supplying of the services again; or
# ii.  the payment of the cost of having the services supplied again.
# 
# The construction, validity and performance of this licence is governed
# by the laws in force in New South Wales, Australia.

"""
ezxml provides support to parse_spec_xml. It extends the basic XML
parsing facility in python to something that is simpler to specify and
abstract away the DOM API from the user
"""
from xml.dom.minidom import parse, parseString
from xml.parsers.expat import ExpatError
import types

import os

class ParsedElement:
    """This represents an element once it has been parsed."""
    def __init__(self, tagname):
        self.tag = tagname
        self.children = []
    
    def __repr__(self):
        """Return marked up parsed element"""
        base = "<%s " % self.tag
        for attr in [key for key in self.__dict__ if key
                     not in ["children", "tag"]]:
            val = getattr(self, attr)
            if type(val) == long:
                base += '%s="0x%x" ' % (attr, val)
            else:
                base += '%s="%s" ' % (attr, val)
        base += ">"
        return base

    def dump_internal(self, indent):
        """
        Internal helper function for dump().  Returns a XML
        representation of the element and its children.  The current
        element is indented by <indent> characters and children are
        intended by <indent> + 4 characters.
        """
        XML_INDENT = 4 # Number if chars to indent children.
        text = ' ' * indent + self.__repr__() + '\n'

        for el in self.children:
            text += el.dump_internal(indent + XML_INDENT)

        text += ' ' * indent + '</%s>\n' % self.tag

        return text

    def dump(self):
        """Return a XML representation of the element and its children."""
        return self.dump_internal(0)

    def find_children(self, tagname):
        return [el for el in self.children if el.tag ==  tagname]

    def find_child(self, tagname):
        els = self.find_children(tagname)
        if len(els) == 0:
            return None
        else:
            return els[0]

class EzXMLError(Exception):
    """This exception is raised if an error occurs during traversing
    a given dom."""

def process_includes(dom):
    """
    This method walks the DOM and replaces any 'include' elements
    with the parsed contents of the file to which it refers.

    The files included must contain XML fragments, not full XML
    documents.
    """
    def do_incs(elem):
        """
        If the current element is an include then return a new
        include element whose children are the contents of the
        file.

        Otherwise, recursively process the children of this
        element.  Afterwards, is the child is an include element
        replace it with its own children.
        """
        # The file attribute vanishes when the contents are included,
        # so its presence must be checked for.
        if elem.tagName == "include" and elem.hasAttribute('file'):
            include_filename = elem.getAttribute('file')
            if not os.path.isfile(include_filename):
                # Can't find relative to current directory; try relative to scripts/Input
                import sys
                try_filename = os.path.join(os.path.dirname(sys.argv[0]),'Input',include_filename)
                if os.path.isfile(try_filename):
                    include_filename = try_filename
            include_file = file(include_filename)
            # The parser requires a root element, so wrap the file
            # contents in another include element.  This will be
            # stripped out below.
            new_dom = parseString('<include>' + include_file.read() + '</include>')
            include_file.close()
            return new_dom.documentElement

        # Examine children
        for child in elem.childNodes:
            if child.nodeType == child.ELEMENT_NODE:
                new_child = do_incs(child)

                # Replace the include element 
                if new_child.tagName == "include":
                    elem.removeChild(child)

                    for kid in new_child.childNodes:
                        if kid.nodeType == child.ELEMENT_NODE:
                            elem.appendChild(kid)

        return elem
            
    # Do the work recursively from the top of the tree.
    try:
        do_incs(dom.documentElement)
    except EnvironmentError, ex:
        raise EzXMLError, 'XML include error: "%s": %s.' % (ex.filename, ex.strerror)

class Element:
    """Defines a type of element, rather than a particular element
    in an XML file. E.g: Can be used to define what attributes and
    child a given node has, rather than representing a particular node
    with specific attributes and elements."""
    
    def __init__(self, __name, *children, **attributes):
        """Construct an Element with a given tagname 'name'. Attributes
        is a dictionary of valid attributes. Children is a last of
        child elements."""
        self.autoxml = attributes.pop('autoxml', False)
        self.name = __name
        self.attributes = attributes
        for attr in self.attributes.values():
            if type(attr) != types.TupleType:
                raise EzXMLError("Attribute descriptors must be tuples")
            if len(attr) != 2:
                raise EzXMLError("Attribute descriptors must " + \
                                 "be 2-element tuples")
            if not callable(attr[0]):
                raise EzXMLError("First element in descriptor " + \
                                 "must be callable")
            constraints = ( "required", "optional", "text" )
            if attr[1] not in constraints:
                raise EzXMLError( 'Constraint for an attribute of "%s" must be one of %s' % ( self.name, str( constraints ) ) )

        self.elements = dict([(el.name, el) for el in children])
	self.extras = None
	self.extra_func = None

    def parse_xml_str(self, data):
        """Parse a given XML string. Note this should only be used on the
        top level Element. Not child elements."""
        try:
            dom = parseString(data)
            process_includes(dom)
        except ExpatError, x:
            raise EzXMLError("Failed to parse: %s" % x)
        return self.walkdom(dom.documentElement)

    def parse_xml_file(self, filename):
        """Parse a given XML file. Note this should only be used on the
        top level Element. Not child elements."""
        try:
            dom = parse(filename)
            process_includes(dom)
        except ExpatError, x:
            raise EzXMLError("Failed to parse: %s" % x)
        return self.walkdom(dom.documentElement)

    def walkdom(self, el):
        """This methods walks the DOM and extracts attributes and children
        as it goes. It returns <something>, that provides a high level, pythonic
        representation of the XML file."""
        ret = ParsedElement(self.name)

        if el.tagName != self.name:
            raise EzXMLError("Names don't match: %s %s" % \
                             (el.tagName, self.name))


        # check that attributes are the expected. Allowable attributes
        # are specified on instantiation of the Element Object
        if el.attributes:
            for attr in el.attributes.keys():
                if attr not in self.attributes:
                    raise EzXMLError('Unknown attribute in "%s" element: "%s".' % (el.tagName, attr))
                # and set the attributes value as seen
                setattr(ret, attr,
                        self.attributes[attr][0](el.attributes[attr].value))

        # Ensure required attributes are present
        for attr_name, descriptor in self.attributes.items():
            if descriptor[1] == "required" and not hasattr(ret, attr_name):
                raise EzXMLError('The required attribute "%s" is not specified in element "%s".' % \
                                 (attr_name, el.tagName))
            # Process attributes sourced from text-type child-node
            if descriptor[ 1 ] == "text":
                if hasattr( ret, attr_name ):
                    raise EzXMLError( 'Attribute "%s" is expected to be assigned from a text child node of element <%s/>' % \
                                      ( attr_name, el.tagName ) )
                else:
                    child = el.firstChild
                    if child:
                        setattr( ret, attr_name,
                                 self.attributes[ attr_name ][ 0 ]( child.nodeValue ) )
                    else:
                        raise EzXMLError( 'Text child-node expected for element <%s/>' % el.tagName )

        #print os.path.dirname(os.popen("which elfweaver").read()),"/","weaver.py" 

        #execfile( "%s/../weaver.py"%os.path.dirname(__file__))

        # Examine children
        for child in el.childNodes:
            if child.nodeType == child.ELEMENT_NODE:
	       # if child.tagName == "weavertags":
		   #for attr in child.attributes.keys():
		   #    #execfile("weaver.py")
		   #continue

                # parse the child and add an item to the children
                # attribute of the parsed element containing the parsed
                # contents    
		if child.tagName in self.elements.keys():
                    ret.children.append(self.elements[child.tagName].walkdom(child))
		elif self.extras and child.tagName in self.extras.keys():
                    ret.children.append(self.extras[child.tagName].walkdom(child))
		else:
                    raise EzXMLError("Unknown child node: %s of %s" % (child,self))



            elif child.nodeType == child.TEXT_NODE:
                # Ignore text nodes for now
                pass
            elif hasattr(child, "COMMENT_NODE") and child.nodeType == child.COMMENT_NODE:
                # ignore comments
                pass
            else:
                raise EzXMLError("Unhandled node type: %d\n" % child.nodeType)
                    
        if self.autoxml:
            self.cfg.build_elements.append(el)

        return ret

    def __call__(self, *children, **attributes):
        new = ParsedElement(self.name)
        for attr in attributes:
            setattr(new, attr, attributes[attr])
        for child in children:
            new.children.append(child)
        return new

        
def str_attr(attr):
    """Parses a string attribute."""
    return str(attr)

def long_attr(attr):
    """Parse a long attribute"""
    try:
        val = long(attr, 0)
    except ValueError:
        raise EzXMLError("%s did not parse as an integer" % attr)
    return val

def bool_attr(attr):
    """Parse a boolean attribute"""
    if attr.lower() == "true":
        val = True
    elif attr.lower() == "false":
        val = False
    else:
        raise EzXMLError("Must be "\
                         "'true' or 'false'. Not %s" % (attr))
    return val

def size_attr(attr):
    """
    Parse a size attribute.  These attributes are longs in either
    base 10 or base 16.  If there is a suffix of 'K' then the value is
    in kilobytes and if there is a suffix of 'M' then the value is in
    megabytes.
    """
    suffix = attr[-1]
    multiple = 1

    # Detect and strip off the suffix.
    if suffix == 'K' or suffix == 'k':
        multiple = 1024L
        attr_num = attr[:-1]
    elif suffix == 'M' or suffix == 'm':
        multiple = 1024L * 1024L
        attr_num = attr[:-1]
    elif suffix == 'G' or suffix == 'g':
        multiple = 1024L * 1024L * 1024L
        attr_num = attr[:-1]
    else:
        attr_num = attr

    try:
        val = long(attr_num, 0) * multiple
    except ValueError:
        raise EzXMLError('"%s" did not parse as a size value.' % attr)
    return val
