#
#  qiblog.py
#
#  qurt-image-build log functionality goes here.
#
#  The qurt-image-build script should create an instance
#   of the QIBLogger class and use it to manage the
#   XML log which describes qurt-image-build's behavior.
#
#  See the individual class methods for documentation.
#

class QIBLog(object):

    def __init__(self, cfg, filename):
        import xml.dom.minidom
        self.cfg = cfg
        self.filename = filename
        self.doc = xml.dom.minidom.getDOMImplementation().createDocument(None,'qurt_image_build',None)
        self.doc_el = self.doc.documentElement

        self.fixed_boot_el =    self.new_element('boot_layout',    reloc='false', parent=self.doc_el)
        self.reloc_boot_el =    self.new_element('boot_layout',    reloc='true',  parent=self.doc_el)
        self.fixed_runtime_el = self.new_element('runtime_layout', reloc='false', parent=self.doc_el)
        self.reloc_runtime_el = self.new_element('runtime_layout', reloc='true',  parent=self.doc_el)
        self.fixed_mmu_el =     self.new_element('mmu_layout',     reloc='false', parent=self.doc_el)
        self.reloc_mmu_el =     self.new_element('mmu_layout',     reloc='true',  parent=self.doc_el)

    def mkaddr(self, N):
        return '0x%08X' % (N,)

    def mksize(self, N):
        return '0x%X' % (N,)

    #
    #  Simple Python accessor for creating XML elements.
    #   First argument is the element name.
    #   Second through Nth unnamed arguments are child elements.
    #   Named arguments are attributes.
    #   If the parent named argument is present, the new element is added as a child of parent.
    #

    def new_element(self, *ls, **kw):
        el = self.doc.createElement(ls[0])
        parent = kw.pop('parent', None)
        for k,v in kw.iteritems():
            el.setAttribute(k,v)
        for c in ls[1:]:
            el.appendChild(c)
        if parent:
            parent.appendChild(el)
        return el

    #
    #  Add a memory range to the XML file.
    #  The range should be an object which has the
    #   base class of QurtAddressRange as implemented
    #   in lib/qurt.py.
    #

    def add_range(self, range):
        if getattr(range, 'gap_recovery'):
            if self.cfg.relocatable_image.value:
                el = self.new_element('gap',
                                      parent=self.reloc_runtime_el,
                                      addr=self.mkaddr(range.addr - self.cfg.physaddr),
                                      size=self.mksize(range.size))
            else:
                el = self.new_element('gap',
                                      parent=self.fixed_runtime_el,
                                      addr=self.mkaddr(range.addr),
                                      size=self.mksize(range.size))
            self.new_element('readable', parent=el)
            if not range.X:
                self.new_element('writable', parent=el)
            if not range.W:
                self.new_element('executable', parent=el)

    #
    #  Finish the XML file.
    #  Will create the file on disk and dump the
    #   XML representation into it.
    #

    def finish(self):
        f = open(self.filename, 'w')
        f.write(self.doc.toprettyxml(indent='  ',encoding='utf-8'))
        f.close()

