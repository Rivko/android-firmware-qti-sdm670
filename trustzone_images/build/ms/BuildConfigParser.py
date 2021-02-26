#===============================================================================
#
# SSG Build system
#
# Copyright (c) 2016-2017 Qualcomm Technologies, Inc. All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
#===============================================================================
'''
    @authors: lenhardw

    Python Script for parsing & checking build configurations

'''

import logging

VERBOSE = 5
logging.basicConfig()
logger = logging.getLogger("BuildConfigParser")
logger.setLevel(logging.INFO)

import copy
import os
import types

from xml.dom import minidom
from xml.etree import ElementTree

# TODO - debug this enhancement eventually; currently broken due to the
# behavior of resolveXmlDependencies (effective override happens with the
# results of parseFunc(), but we aren't handling it 100% correctly)
OPTIMIZE_MEMORY = False
ENABLE_OS_JOINING = True

DEFAULT_BUILD_VERSION = "40000000"

SUPPORTED_VAR_METHODS = [
    "declare",
    "str-join",
]
DEPENDENT_VAR_METHODS = [
    "join",
    "smart-join",
]
ENV_DEPENDENT_VAR_METHODS = [
    "append",
    "default",
    "prepend",
]
SUPPORTED_VAR_METHODS.extend(DEPENDENT_VAR_METHODS)
SUPPORTED_VAR_METHODS.extend(ENV_DEPENDENT_VAR_METHODS)
DEFAULT_VAR_METHOD = "declare"
DEFAULT_VAR_SEPARATOR = os.sep
VAR_REF_DELIMITER = ";"
DEFAULT_ARTIFACT_EXTENSION = ".mbn"
RESERVED_CLONE_KEY = "is-clone"

TAGS = {
    "argument" : "arg",
    "artifact" : "artifact",
    "branch" : "branch",
    "dependency" : "dependency",
    "environment" : "environment",
    "file" : "alias",
    "mapreport" : "mapreport",
    "os" : "os",
    "parameter" : "param",
    "root" : "build",
    "target" : "chipset",
    "variable" : "var",
}

BOOLEAN_STRINGS = [
    "true",
    "false",
    "True",
    "False",
]

assert DEFAULT_VAR_METHOD in SUPPORTED_VAR_METHODS, \
        "DEFAULT_VAR_METHOD '%s' is unsupported!" % DEFAULT_VAR_METHOD

# NOTE: It is highly recommended to ONLY use addChild() from the user side as
# addParent can result in orphaned parents that SHOULD have been under the
# tree's root node.
class DependencyNode(object):
    """Abstract nodes used to represent & resolve dependencies"""
    RESERVED_ROOT_NODE_NAME = None
    ROOT_NODE_NAME_STRING = "the root node"
    LOGGER = logging.getLogger("DependencyNode")
    LOGGER.setLevel(logging.INFO)
    def __init__(self, name, data = None, parents = None, children = None):
        self.name = name
        self.data = data
        parents = parents or []
        children = children or []
        if self.isRoot():
            assert len(parents) == 0, "%s cannot have parents" \
                    % self.getName()
        self._parents = set([])
        for parent in parents:
            self.addParent(parent)
        self._children = set([])
        for child in children:
            self.addChild(child)
        DependencyNode.debug("new node %s with children: %s" \
                % (self.getQuotedName(), self.childrenAsString()))
#
    @classmethod
    def createRootNode(cls):
        """Returns a root node"""
        return cls(DependencyNode.RESERVED_ROOT_NODE_NAME)
#
    @classmethod
    def debug(cls, msg):
        """debug-level log for DependencyNode"""
        cls.LOGGER.debug(msg)
#
    @classmethod
    def info(cls, msg):
        """info-level log for DependencyNode"""
        cls.LOGGER.info(msg)
#
    @classmethod
    def warn(cls, msg):
        """warning-level log for DependencyNode"""
        cls.LOGGER.warn(msg)
#
    @classmethod
    def error(cls, msg):
        """error-level log for DependencyNode"""
        cls.LOGGER.error(msg)
#
    # assumes that all names are unique within the tree, asserting if node
    # recursion is detected to avoid generating an infinite list
    def getTraversalList(self):
        """Generates a list from nodes in the tree in an order such that
           traversing the list linearly will never violate a dependency."""
        traversedNodes = []
        remainingNodes = [ self ]
        visited = set([])
        toVisit = set([] if self.isRoot() else [ self.name ])
        while len(remainingNodes) > 0:
            node = remainingNodes.pop(0)
            DependencyNode.debug(
                    "Try to visit %s (children = %s & parents = %s)" \
                    % (node.getQuotedName(), node.childrenAsString(), \
                    node.parentsAsString()))
            visitedParents = visited.intersection(node.parents)
            if len(visitedParents) < len(node.parents):
                DependencyNode.debug( \
                        #pylint: disable=C0301
                        "Haven't visited all parents of %s; moving to end of list" \
                        % node.getQuotedName())
                remainingNodes.append(node)
                continue
            if not node.isRoot():
                toVisit.remove(node.name)
                visited.add(node)
                traversedNodes.append(node)
                DependencyNode.debug("Successfully visited %s" \
                        % node.getQuotedName())
            for childNode in node.children:
                assert childNode not in visited, \
                        "Infinite dependency recursion detected at %s -> %s" \
                        % (node.getQuotedName(), childNode.getQuotedName())
                if childNode.name not in toVisit:
                    remainingNodes.append(childNode)
                    toVisit.add(childNode.name)
#            DependencyNode.debug("remaining nodes: %s" \
#                    % ", ".join([ n.name for n in remainingNodes ]))
        return traversedNodes
#
    def addParent(self, parent, updateParent = True):
        """Adds a node as a parent of the current node"""
        DependencyNode.debug("Add parent %s to %s" \
                % (parent.getQuotedName(), self.getQuotedName()))
        assert parent != self, "Cannot add a self-referential child to %s" \
                % self.getQuotedName()
        assert not self.isRoot(), "Cannot add %s as a parent of %s" \
                % (parent.getQuotedName(), self.getQuotedName())
        assert parent.name != self.name, \
                "Cannot add a child with the same name (%s)" % self.getName()
        if not parent.isRoot():
            # the root node does not count as a parent
            self._parents.add(parent)
        if updateParent:
            parent.addChild(self, updateChild = False)
#
    def removeParent(self, parent, updateParent = True):
        """Removes the given parent node from this node's parents"""
        DependencyNode.debug("Remove parent %s from %s" \
                % (parent.getQuotedName(), self.getQuotedName()))
        assert parent in self._parents, \
                "Cannot remove %s as it is not a parent of %s" \
                % (parent.getQuotedName(), self.getQuotedName())
        self._parents.remove(parent)
        if updateParent:
            parent.removeChild(self, updateChild = False)
#
    def addChild(self, child, updateChild = True):
        """Adds a node as a child of the current node"""
        DependencyNode.debug("Add child %s to %s" \
                % (child.getQuotedName(), self.getQuotedName()))
        assert child != self, "Cannot add a self-referential child to %s" \
                % self.getQuotedName()
        assert not child.isRoot(), "Cannot add %s as a child of %s" \
                % (child.getQuotedName(), self.getQuotedName())
        assert child.name != self.name, \
                "Cannot add a child with the same name (%s)" % self.getName()
        self._children.add(child)
        if updateChild:
            child.addParent(self, updateParent = False)
#
    def removeChild(self, child, updateChild = True):
        """Removes the given child node from this node's children"""
        DependencyNode.debug("Remove child %s from %s" \
                % (child.getQuotedName(), self.getQuotedName()))
        assert child in self._children, \
                "Cannot remove %s as it is not a child of %s" \
                % (child.getQuotedName(), self.getQuotedName())
        self._children.remove(child)
        if updateChild:
            child.removeParent(self, updateParent = False)
#
    def toDictionary(self):
        """Converts the tree to a dictionary for compatibility"""
        ret = {}
        if self.isRoot():
            childDict = ret
        else:
            ret["name"] = self.name
            childDict = {}
            ret["children"] = childDict
            ret["parents"] = self.parents
        for child in self._children:
            childDict[child.name] = child.toDictionary()
        return ret
#
    def isRoot(self):
        """Returns True if this is a root node & False otherwise"""
        return self.name == DependencyNode.RESERVED_ROOT_NODE_NAME
#
    def getName(self, formatString = None):
        """Returns this node's proper name"""
        name = DependencyNode.ROOT_NODE_NAME_STRING if self.isRoot() \
                else self.name
        if formatString == None:
            return name
        assert "%s" in formatString, "Invalid format string '%s'" \
                % formatString
        return formatString % name
#
    def getQuotedName(self, quote = "'"):
        """Returns this node's quoted name. Root nodes will not receive quotes
           as their literal string (usually 'the root node') is not a literal
           string like most names"""
        if self.isRoot():
            # the root node is never quoted
            return self.getName()
        formatString = quote + "%s" + quote
        return self.getName(formatString)
#
    def parentsAsString(self):
        """Returns a stringified quoted list of this node's parents"""
        if len(self.parents) == 0:
            # While this is a strange string, our ", ".join() below ensures that
            # this string is impossible to reproduce even with nodes named 'NA'
            # and 'no children' in the tree.
            return "(NA, no parents)"
        return ", ".join([ n.getQuotedName() for n in self.parents ])
#
    def childrenAsString(self):
        """Returns a stringified quoted list of this node's children"""
        if len(self.children) == 0:
            # While this is a strange string, our ", ".join() below ensures that
            # this string is impossible to reproduce even with nodes named 'NA'
            # and 'no children' in the tree.
            return "(NA, no children)"
        return ", ".join([ n.getQuotedName() for n in self.children ])
#
    @property
    def parents(self):
        """Returns a list of this node's parent nodes"""
        # while inefficient, return a shallow copy to prevent modification of
        # the actual internal list
        return list(self._parents)
#
    @property
    def parentCount(self):
        """Returns the number of other nodes with this node as their child"""
        return len(self._parents)
#
    @property
    def children(self):
        """Returns a list of this node's child nodes"""
        # while inefficient, return a shallow copy to prevent modification of
        # the actual internal list
        return list(self._children)
#
    @property
    def childCount(self):
        """Returns the number of other nodes with this node as their parent"""
        return len(self._children)
#
    @property
    def dependencies(self):
        """Returns a set of all of this node's recursive dependencies"""
        # find all (recursive) dependencies of the current node (ie, everything
        # that must already exist before this node is valid)
        deps = set([])
        for parent in self._parents:
            if parent not in deps:
                deps.add(parent)
                deps.update(parent.dependencies)
        return deps

class StrictMap(dict):
    """Extends a dictionary to raise KeyError if we attempt to set a value that
       is not already explicitly defined, either via the initializer or the
       define() method. DOES NOT currently do type checking."""
    def __init__(self, data = None):
        """Initializer for StrictMap class. Any dictionary passed in here will
           be accepted as is without strict key checking."""
        super(StrictMap, self).__init__()
        if data == None:
            return
        dataType = type(data)
        if dataType != dict:
            raise TypeError("Cannot initialize dict from %s" % str(dataType))
        for key in data:
            self.define(key, data[key])

    def __getattr__(self, attrib):
        """Allows fetching values as class attributes"""
        return self[attrib]

    def _storeattr(self, attrib, value, check = True):
        """Generic wrapper for setting values"""
        if check:
            try:
                self[attrib]
            except KeyError:
                raise KeyError("Key '%s' was not explicitly defined" % attrib)
        super(StrictMap, self).__setitem__(attrib, value)

    def __setattr__(self, attrib, value):
        """Allows storing values via '.' operator (ie, instance attributes)"""
        self._storeattr(attrib, value, check = True)

    def __setitem__(self, attrib, value):
        """Overrides default item setter with checked version"""
        self._storeattr(attrib, value, check = True)

    def define(self, attrib, value = None):
        """Allows setting items that are not defined"""
        self._storeattr(attrib, value, check = False)

    def dict(self):
        """Casts this to a conventional dictionary"""
        # Normally, to cast to dict(), we would override __iter__ with a valid
        # generator yielding key, value pairs. Since this class already inherits
        # from dict(), a valid __init__ comes for free & we can directly cast.
        return dict(self)

# PARSING FUNCTIONS

# XML file loader
def loadXml(filename):
    """Reads an XML file & performs basic parsing"""
    logger.debug("Loading %s" % filename)
    try:
        infile = open(filename)
    except (IOError, OSError) as e:
        logger.error("[%s] could not load %s" % (str(e), filename))
        return {}
    xmlContents = infile.read()
    infile.close()
    try:
        tree = ElementTree.fromstring(xmlContents)
    except ElementTree.ParseError as e:
        lineno, column = e.position
        line = xmlContents.splitlines()[lineno - 1]
        caret = '{:=>{}}'.format('^', column)
        e.msg = '{}\n{}\n{}'.format(e, line, caret)
        raise
    ret = parseXml(tree)
    return ret

# basic XML parsing for root tag
def parseXml(rootXml):
    """Primary parsing function for the build config XML"""
    branches = []
    for xmlSubnode in rootXml:
        if xmlSubnode.tag != TAGS.get("branch"):
            unexpectedTag(xmlSubnode, rootXml)
        branches.append(xmlSubnode)
    # this tag does not inherit from anything so we pass nodeMap as None
    branchInfo = resolveXmlDependencies(branches, getParent, parseBranch, None)
    # verify that there is at least one non-virtual branch
    hasProductionBranch = False
    for branch in branchInfo:
        if not branchInfo[branch].get("virtual", False):
            hasProductionBranch = True
            break
    assert hasProductionBranch, \
            "Config must contain at least 1 non-virtual (production) branch"
    # now that all branches are parsed, we are able to properly resolve variable
    # construction, although this logic breaks our nice distributed parsing
    # somewhat by exposing a lot of the map structure
    for branch in branchInfo:
        for target in branchInfo[branch]["targets"]:
            targetEnv = branchInfo[branch]["targets"][target]["environment"]
            for supportedOs in targetEnv:
                osInfo = targetEnv[supportedOs]
                variableNodes = [ osInfo["var-nodes"][k] \
                        for k in osInfo["var-nodes"] ]
                logger.debug("Resolving %d %s env vars for %s on %s" \
                        % (len(variableNodes), supportedOs, target, branch))
                logV("VARIABLE NODE MAP: %s" % str(osInfo["var-nodes"]))
                osInfo["variables"] = resolveXmlDependencies(variableNodes, \
                        getVariableDependencies, parseVariable, None)
                del osInfo["var-nodes"]
    # now that the variables are resolved, apply any last minute cleanup
    for branch in branchInfo:
        for target in branchInfo[branch]["targets"]:
            targetInfo = branchInfo[branch]["targets"][target]
            # 'buildable' targets generally have many more properties than
            # non-buildable ones, so it's not always safe to assume that a key
            # name exists. Use dict.get liberally.
            if targetInfo.get("max_num_root_certs") == 0:
                # use system-default max_num_root_certs if not specified
                targetInfo["max_num_root_certs"] = None
    # finally, apply branch aliases
    # save keys now for iteration since aliases will change them
    branchList = branchInfo.keys()
    for branch in branchList:
        aliases = branchInfo[branch]["aliases"]
        logger.debug("<%s> '%s' has aliases: %s" \
                % (TAGS.get("branch"), branch, ", ".join(aliases)))
        for alias in aliases:
            errorMsg = "'%s' cannot be used as an alias for <%s> '%s'" \
                    % (alias, TAGS.get("branch"), branch)
            assert alias not in branchList, \
                    "%s - it has already been aliased to another <%s>" \
                    % (errorMsg, TAGS.get("branch"))
            assert alias not in branchList, \
                    "%s - another <%s> exists with the same name" \
                    % (errorMsg, TAGS.get("branch"))
            branchInfo[alias] = branchInfo[branch]
    return branchInfo

# basic XML parsing for environment tags
def parseEnvironment(environmentXml):
    """Parser function for <environment> tags"""
    operatingSystems = []
    for xmlSubnode in environmentXml:
        if xmlSubnode.tag != TAGS.get("os"):
            unexpectedTag(xmlSubnode, environmentXml)
        operatingSystems.append(xmlSubnode)
    nodeMap = None # this tag does not inherit from anything
    osInfo = resolveXmlDependencies(operatingSystems, getParent, parseOs, \
            nodeMap)
    return osInfo

# basic XML parsing for artifact tags
def parseArtifact(artifactXml):
    """Parser function for <artifact> tags"""
    name = checkProperty(artifactXml, "name")
    logV("Parsing <%s> '%s'" % (artifactXml.tag, name))
    extension = checkProperty(artifactXml, "ext", DEFAULT_ARTIFACT_EXTENSION)
    if len(extension) > 0 and extension[0] != ".":
        logger.warn( \
                #pylint: disable=C0301
                "Found extension '%s' (no '.') for <%s> '%s' - was this intentional?" \
                % (extension, artifactXml.tag, name))
    return name + extension

# basic XML parsing for argument tags
def parseArgument(argXml):
    """Parser function for <argument> tags"""
    flag = checkProperty(argXml, "flag")
    logV("Parsing <%s> flag='%s'" % (argXml.tag, flag))
    value = checkProperty(argXml, "value", default = "")
    assert False, "Argument tags are deprecated!!!"
    return (flag, value)

# basic XML parsing for parameter tags
def parseParameter(paramXml):
    """Parser function for <parameter> tags"""
    name = checkProperty(paramXml, "name")
    logV("Parsing <%s> '%s'" % (paramXml.tag, name))
    value = checkProperty(paramXml, "value")
    assert False, "Parameter tags are deprecated!!!"
    return (name, value)

# basic XML parsing for dependency tags
def parseDependency(depXml):
    """Parser function for <dependency> tags"""
    name = checkProperty(depXml, "name")
    logV("Parsing <%s> '%s'" % (depXml.tag, name))
    assert False, "Dependency tags are deprecated!!!"
    return name

# parseFunc takes arguments:
# 1) xml.etree.ElementTree.Element(xmlNode),
# 2) function(getDependenciesFunc) - defined above its implementations
# 3) { str(name) : xml.etree.ElementTree.Element(xmlNode) }
#    where the name -> node mapping is used for inheritance resolution
# returns some form of parsed data structure (dict, list, etc)
def validateParseFunc(f):
    """Validation function for parseFunc parsing function implementations"""
    assert type(f) == types.FunctionType, \
            "parseFunc candidate is not a function"
    # TODO - add additional validation

# parseFunc implementation for branch tags
def parseBranch(branchXml, getDependenciesFunc, nodeMap = None):
    """parseFunc parsing function implementation for <branch> tags"""
    validateGetDependenciesFunc(getDependenciesFunc)
    nodeMap = nodeMap or {}
    dependencies = getDependenciesFunc(branchXml, nodeMap)
    name = checkProperty(branchXml, "name")
    # It is possible that data may already exist for this node due to copying of
    # a higher-level node (if something higher than branch is ever added).
    # In this case, we need to:
    # 1) override the previous data with any locally-inherited data
    # 2) override the inherited data with any locally-defined data
    ret = nodeMap.get(name, {})
    logger.debug("Parsing <%s> '%s' (%s)" % (branchXml.tag, name, \
            "data exists" if len(ret) > 0 else "new"))
    if len(ret) > 0:
        logV("<%s> %s's existing data: %s" % (branchXml.tag, name, str(ret)))
    if len(dependencies) > 0:
        parent = dependencies[0] # branch tags may only have 1 dependency
        assert parent in nodeMap, \
                "Could not resolve <%s> inheritance of %s from %s" \
                % (branchXml.tag, name, parent)
        logger.debug("<%s> '%s' inherits from '%s'" \
                % (branchXml.tag, name, parent))
        if OPTIMIZE_MEMORY:
            # While we technically need to override everything (there is no
            # common data), we can save substantial memory by performing only a
            # shallow copy here, copying internal structures later in the
            # override() function as needed.
            # FIXME documentation is outdated
            override(ret, nodeMap[parent])
            ret[RESERVED_CLONE_KEY] = True # TODO - probably useless now
        else:
            # Perform a full copy as there is no shared data. We can now
            # override any parameter without affecting the parent.
            temp = copy.deepcopy(nodeMap[parent])
            # TODO, probably don't need deepcopy as override can handle this
            override(ret, temp)
        logV("<%s> '%s' after inheritance:\n %s" % (branchXml.tag, name, \
                str(ret)))
    # initialize expected structures
    ret["targets"] = ret.get("targets", {})
    logV("Var list:\n%s" % str(ret["targets"]))
    # aliases cannot be inherited as inheriting branches would always conflict
    # with their parents
    ret["aliases"] = []
    # read properties and override any new ones
    # NOTE: virtual is NOT inheritable to prevent confusion when inheriting from
    # virtual branches
    ret["virtual"] = parseBooleanProperty(branchXml, "virtual", default = False)
    aliases = branchXml.attrib.get("aliases")
    if not ret["virtual"]:
        logger.debug("branch '%s' is a production branch" % name)
        # These checks will assert if either one is not found AND no value was
        # inherited. Reminder: a default of None passed into checkProperty()
        # triggers assertion if no value is found.
        ret["default_target"] = checkProperty(branchXml, "default_target", \
                ret.get("default_target"))
        if aliases != None:
            ret["aliases"] = aliases.split(",")
    else:
        assert aliases == None, "'%s': cannot have aliases for a virtual <%s>" \
                % (name, branchXml.tag)
    targets = []
    for xmlSubnode in branchXml:
        if xmlSubnode.tag != TAGS.get("target"):
            unexpectedTag(xmlSubnode, branchXml)
        targets.append(xmlSubnode)
    targetInfo = resolveXmlDependencies(targets, getParent, parseTarget, \
            ret["targets"])
    # ensure that for non-virtual branches, at least 1 target is buildable
    if not ret["virtual"]:
        hasBuildableTarget = False
        for target in targetInfo:
            if targetInfo[target]["buildable"]:
                hasBuildableTarget = True
                break
        #pylint: disable=C0301
        assert hasBuildableTarget, \
                "Non-virtual <%s> '%s' must contain at least 1 buildable target" \
                % (branchXml.tag, name)
        #pylint: enable=C0301
    ret["targets"] = targetInfo
#    override(ret["targets"], targetInfo)
    if OPTIMIZE_MEMORY and RESERVED_CLONE_KEY in ret:
        del ret[RESERVED_CLONE_KEY]
    return ret

# parseFunc implementation for target tags
def parseTarget(targetXml, getDependenciesFunc, nodeMap = None):
    """parseFunc parsing function implementation for <target> tags"""
    validateGetDependenciesFunc(getDependenciesFunc)
    nodeMap = nodeMap or {}
    dependencies = getDependenciesFunc(targetXml, nodeMap)
    name = checkProperty(targetXml, "name")
    # It is possible that data may already exist for this node due to copying of
    # a higher-level node (if something higher than branch is ever added).
    # In this case, we need to:
    # 1) override the previous data with any locally-inherited data
    # 2) override the inherited data with any locally-defined data
    ret = nodeMap.get(name, {})
    logger.debug("Parsing <%s> '%s' (%s)" % (targetXml.tag, name, \
            "data exists" if len(ret) > 0 else "new"))
    if len(ret) > 0:
        logV("<%s> %s's existing data: %s" % (targetXml.tag, name, str(ret)))
    if len(dependencies) > 0:
        parent = dependencies[0] # target tags may only have 1 dependency
        assert parent in nodeMap, \
                "Could not resolve <%s> inheritance of %s from %s" \
                % (targetXml.tag, name, parent)
        logger.debug("<%s> '%s' inherits from '%s'" \
                % (targetXml.tag, name, parent))
        if OPTIMIZE_MEMORY:
            # While we technically need to override everything (there is no
            # common data), we can save substantial memory by performing only a
            # shallow copy here, copying internal structures later in the
            # override() function as needed.
            # FIXME documentation is outdated
            override(ret, nodeMap[parent])
            ret[RESERVED_CLONE_KEY] = True # TODO - probably useless now
        else:
            # Perform a full copy as there is no shared data. We can now
            # override any parameter without affecting the parent.
            temp = copy.deepcopy(nodeMap[parent])
            # TODO, probably don't need deepcopy as override can handle this
            override(ret, temp)
        logV("<%s> '%s' after inheritance:\n %s" % (targetXml.tag, name, \
                str(ret)))
    # initialize expected structures
    ret["files"] = ret.get("files", {})
    logV("File list:\n%s" % str(ret["files"]))
    ret["environment"] = ret.get("environment", {})
    # read properties and override any new ones within 'ret'
    parseTargetProperties(targetXml, ret)
    logV("%s's children: %s" % (name, str([sn for sn in targetXml])))
    lastFilename = None
    for xmlSubnode in targetXml:
        if xmlSubnode.tag == TAGS.get("environment"):
            tempEnv = parseEnvironment(xmlSubnode)
            override(ret["environment"], tempEnv)
        elif xmlSubnode.tag == TAGS.get("file"):
            tempFile = parseFile(xmlSubnode, getFileDependencies, ret["files"])
            # parseFile always returns { name : <data> }, so it's ok to directly
            # reference the 1st key
            if lastFilename != None:
                # pylint: disable=C0301
                assert tempFile.keys()[0] >= lastFilename, \
                        "<{}> tags must be listed in alphabetical order ({} < {})".format( \
                        xmlSubnode.tag, tempFile.keys()[0], lastFilename)
            override(ret["files"], tempFile)
            lastFilename = tempFile.keys()[0]
        else:
            unexpectedTag(xmlSubnode, targetXml)
    logV("<%s> %s files: %s" % (targetXml.tag, name, \
            ", ".join(ret["files"].keys())))
    # Now that we have an updated file list, we need to create a file ordering
    # that obeys their dependencies.
    getdeps = lambda f, n = None: f["dependencies"]
    fileOrder = [ node.name for node in buildDictDependencyTree( \
            ret["files"], getdeps, False, None).getTraversalList() ]
    logV("File order for <%s> %s: %s" % (targetXml.tag, name, \
            ", ".join(fileOrder)))
#    ret["file_order"] = fileOrder
    # TODO - ideally, we should validate that any file without 'strip' should
    # never be dependent on a file with 'strip', such that we could potentially
    # skip building a required dependency
    if OPTIMIZE_MEMORY and RESERVED_CLONE_KEY in ret:
        del ret[RESERVED_CLONE_KEY]
    return ret

def parseTargetProperties(targetXml, data):
    """helper function for parsing the XML properties of buildable <target>s"""
    # name was already validated, but to minimize arguments, we parse it again
    name = checkProperty(targetXml, "name")
    data["chipset"] = checkProperty(targetXml, "chipset", name)
    if data["chipset"] != name:
        logger.debug("<%s> '%s' redefines chipset = '%s'" \
                % (targetXml.tag, name, data["chipset"]))
    # NOTE: 'buildable' is NOT inherited to prevent confusion when inheriting
    # from non-buildable targets
    data["buildable"] = parseBooleanProperty(targetXml, "buildable", \
            default = True)
    logger.debug("target '%s' is buildable" % name)
    data["exclusive"] = parseBooleanProperty(targetXml, "exclusive", \
            default = False)
    #pylint: disable=C0301
    assert not (data["exclusive"] and not data["buildable"]), \
            "<%s> '%s': the 'exclusive' property only applies to buildable <%s>s" \
            % (targetXml.tag, name, targetXml.tag)
    #pylint: enable=C0301
    if not data["buildable"]:
        return
    # These checks will assert if either one is not found AND no value was
    # inherited. Reminder: a default of None passed into checkProperty()
    # triggers assertion if no value is found.
    data["build_id"] = checkProperty(targetXml, "build_id", \
            data.get("build_id"))
    data["build_ver"] = checkProperty(targetXml, "build_ver", \
            data.get("build_ver", DEFAULT_BUILD_VERSION))
    data["target_family"] = checkProperty(targetXml, "target_family", \
            data.get("target_family"))
    data["soc_hw_version"] = checkProperty(targetXml, "soc_hw_version", \
            data.get("soc_hw_version", ""))
    data["soc_vers"] = checkProperty(targetXml, "soc_vers", \
            data.get("soc_vers", ""))
    data["tz_exec_mode"] = checkProperty(targetXml, "tz_exec_mode", \
            data.get("tz_exec_mode"))
    data["pbl_version"] = checkProperty(targetXml, "pbl_version", \
            data.get("pbl_version"))                        
    data["secimage_config"] = checkProperty(targetXml, "secimage_config", \
            data.get("secimage_config"))
    data["max_num_root_certs"] = parseIntegerProperty(targetXml, \
            "max_num_root_certs", data.get("max_num_root_certs", 0), \
            allowNegative = False)
    # These 3 default to the chipset number (stripPrefix() output) if not
    # present. We assert if stripPrefix() returns None and no value was
    # explicitly supplied for the property.
    targetProperties = [
        "build_asic",
        "hal_platform",
        "msm_id",
    ]
    for propName in targetProperties:
        errorMsg = "<%s> '%s': '%s' property must be explicitly specified" \
                 % (targetXml.tag, name, propName)
        # stripPrefix() will return None if there is no digit in the target's
        # 'name' property
        data[propName] = checkProperty(targetXml, propName, \
                stripPrefix(data["chipset"]), \
                forbidden = [ None ], errorMsg = errorMsg)
        logV("<%s> '%s': '%s' = '%s'" % (targetXml.tag, name, propName, \
                data[propName]))

# parseFunc implementation for os tags
def parseOs(osXml, getDependenciesFunc, nodeMap = None):
    """parseFunc parsing function implementation for <os> tags"""
    validateGetDependenciesFunc(getDependenciesFunc)
    nodeMap = nodeMap or {}
    dependencies = getDependenciesFunc(osXml, nodeMap)
    name = checkProperty(osXml, "name")
    # It is possible that data may already exist for this node due to copying of
    # a higher-level node (if something higher than branch is ever added).
    # In this case, we need to:
    # 1) override the previous data with any locally-inherited data
    # 2) override the inherited data with any locally-defined data
    ret = nodeMap.get(name, {})
    logger.debug("Parsing <%s> '%s' (%s)" % (osXml.tag, name, \
            "data exists" if len(ret) > 0 else "new"))
    if len(ret) > 0:
        logV("<%s> %s's existing data: %s" % (osXml.tag, name, str(ret)))
    if len(dependencies) > 0:
        parent = dependencies[0] # os tags may only have 1 dependency
        assert parent in nodeMap, \
                "Could not resolve <%s> inheritance of %s from %s" \
                % (osXml.tag, name, parent)
        logger.debug("<%s> '%s' inherits from '%s'" % (osXml.tag, name, parent))
        if OPTIMIZE_MEMORY:
            # While we technically need to override everything (there is no
            # common data), we can save substantial memory by performing only a
            # shallow copy here, copying internal structures later in the
            # override() function as needed.
            # FIXME documentation is outdated
            override(ret, nodeMap[parent])
            ret[RESERVED_CLONE_KEY] = True # TODO - probably useless now
        else:
            # Perform a full copy as there is no shared data. We can now
            # override any parameter without affecting the parent.
            temp = copy.deepcopy(nodeMap[parent])
            # TODO, probably don't need deepcopy as override can handle this
            override(ret, temp)
        logV("<%s> '%s' after inheritance:\n %s" % (osXml.tag, name, str(ret)))
    # initialize expected structures
    ret["var-nodes"] = ret.get("var-nodes", {})
    # variables are special - if one value is overridden, every child must also
    # be updated. If we resolve values too soon, we end up inheriting the
    # constructed values vs the actual construction method (eg we get a raw
    # string instead of the way to make that string). To resolve this, we only
    # parse the raw variable nodes for now, allowing overriding as needed. Then
    # we perform a mass resolution of variable nodes once all other parsing is
    # complete.
    for xmlSubnode in osXml:
        if xmlSubnode.tag != TAGS.get("variable"):
            unexpectedTag(xmlSubnode, osXml)
        varName = checkProperty(xmlSubnode, "name")
        ret["var-nodes"][varName] = xmlSubnode
    if OPTIMIZE_MEMORY and RESERVED_CLONE_KEY in ret:
        del ret[RESERVED_CLONE_KEY]
    return ret

# parseFunc implementation for variable tags
def parseVariable(varXml, getDependenciesFunc, nodeMap = None):
    """parseFunc parsing function implementation for <var> tags"""
    validateGetDependenciesFunc(getDependenciesFunc)
    nodeMap = nodeMap or {}
    # we only call this function for parsing-validation purposes, thus its
    # output is not saved locally
    getDependenciesFunc(varXml, nodeMap)
    name = checkProperty(varXml, "name") # required property
    logger.debug("Parsing <%s> '%s'" % (varXml.tag, name))
    value = checkProperty(varXml, "value") # required property
    method = checkProperty(varXml, "method", default = DEFAULT_VAR_METHOD, \
            allowed = SUPPORTED_VAR_METHODS)
    exported = not parseBooleanProperty(varXml, "intermediate", default = False)
    ret = {
        "export" : exported,
    }
    if exported:
        # required property for exported vars
        ret["description"] = checkProperty(varXml, "description", \
                forbidden = [ "" ])
    if method == "declare":
        ret["value"] = value
        return ret
    elif method == "default":
        current = os.environ.get(name, "")
        if len(current) > 0:
            # system var is already set, nothing more to do
            ret["value"] = current
            ret["export"] = False
            return ret
    separator = checkProperty(varXml, "separator", \
            default = DEFAULT_VAR_SEPARATOR)
    varParts = value.split(";")
    for i in range(len(varParts)):
        replacementValue = nodeMap.get(varParts[i], {}).get("value")
        if replacementValue == None:
            assert method not in ("append", "join", "prepend"), \
                    "<%s> %s cannot find dependency '%s'" \
                    % (varXml.tag, name, varParts[i])
        else:
            varParts[i] = replacementValue
    if len(varParts) > 1 and ENABLE_OS_JOINING \
            and separator == DEFAULT_VAR_SEPARATOR:
        #pylint: disable=W0142
        ret["value"] = os.path.join(*varParts)
    else:
        ret["value"] = separator.join(varParts)
    if method == "append":
        if len(varParts) > 1 and ENABLE_OS_JOINING \
                and separator == DEFAULT_VAR_SEPARATOR:
            ret["value"] = os.path.join(os.environ.get(name, ""), ret["value"])
        else:
            ret["value"] = separator.join([os.environ.get(name, ""), \
                    ret["value"]])
    elif method == "prepend":
        if len(varParts) > 1 and ENABLE_OS_JOINING \
                and separator == DEFAULT_VAR_SEPARATOR:
            ret["value"] = os.path.join(ret["value"], os.environ.get(name, ""))
        else:
            ret["value"] = separator.join([ ret["value"], \
                    os.environ.get(name, "") ])
    return ret

# parseFunc implementation for file tags
def parseFile(fileXml, getDependenciesFunc, nodeMap = None):
    """parseFunc parsing function implementation for <file> tags"""
    nodeMap = nodeMap or {}
    fileInfo = fileTemplate()
    name = checkProperty(fileXml, "name")
    logger.debug("Parsing <%s> '%s'" % (fileXml.tag, name))
    arguments = {}
    artifacts = set([])
    mapreportFiles = []
    parameters = {}
    dependencies = getDependenciesFunc(fileXml, nodeMap)
    if len(dependencies) == 0:
        logV("<%s> %s has no dependencies" % (fileXml.tag, name))
    else:
        logger.debug("<%s> %s depends on %s" % (fileXml.tag, name, \
                ", ".join(dependencies)))
    for xmlSubnode in fileXml:
        if xmlSubnode.tag == TAGS.get("argument"):
            # returns a (key, value) tuple
            arg = parseArgument(xmlSubnode)
            assert arg[0] not in arguments, \
                    "<%s> '%s' defines <%s> '%s' multiple times" \
                    % (fileXml.tag, name, xmlSubnode.tag, arg[0])
            arguments[arg[0]] = arg[1]
        elif xmlSubnode.tag == TAGS.get("artifact"):
            artifactName = parseArtifact(xmlSubnode)
            assert artifactName not in artifacts, \
                    "Duplicate <%s> '%s' defined for <%s> %s" \
                    % (xmlSubnode.tag, artifactName, fileXml.tag, name)
            artifacts.add(artifactName)
        elif xmlSubnode.tag == TAGS.get("dependency"):
            # this was already parsed in getDependenciesFunc(); ignore it but
            # leave this case so we don't treat <dependency> as an unknown tag
            pass
        elif xmlSubnode.tag == TAGS.get("mapreport"):
            # this is very simple, so we just parse it here
            mrPath = checkProperty(xmlSubnode, "path")
            mapreportFiles.append(convertPath(mrPath))
        elif xmlSubnode.tag == TAGS.get("parameter"):
            # returns a (key, value) tuple
            param = parseParameter(xmlSubnode)
            assert param[0] not in arguments, \
                    "<%s> '%s' defines <%s> '%s' multiple times" \
                    % (fileXml.tag, name, xmlSubnode.tag, param[0])
            parameters[param[0]] = param[1]
        else:
            assert False, "Illegal tag <%s> encountered within <%s> '%s'" \
                    % (xmlSubnode.tag, fileXml.tag, name)
    # There is some implicit magic going on here. The class returned by
    # fileTemplate() does not allow us to set any variable that is not already
    # defined (in the template). This ensures that any external module calling
    # fileTemplate() will get a complete/valid <file> template. To add a new
    # property, define it in fileTemplate() (along with its default value) &
    # then set it here.
    fileInfo["arguments"] = arguments
    fileInfo["artifacts"] = list(artifacts)
    fileInfo["build-once"] = parseBooleanProperty(fileXml, "build-once", \
            default = fileInfo["build-once"])
    fileInfo["dependencies"] = dependencies
    fileInfo["disable"] = parseBooleanProperty(fileXml, "disable", \
            default = fileInfo["disable"])
    fileInfo["internal-test"] = parseBooleanProperty(fileXml, "internal-test", \
            default = fileInfo["internal-test"])
    assert not fileInfo["internal-test"], "The internal-test <%s> property is deprecated!!!" % fileXml.tag
    fileInfo["mapreport"] = mapreportFiles
    fileInfo["params"] = parameters
    fileInfo["recompile"] = parseBooleanProperty(fileXml, "recompile", \
            default = fileInfo["recompile"])
    fileInfo["strip"] = parseBooleanProperty(fileXml, "strip", default = False)
    assert not (fileInfo["recompile"] and fileInfo["strip"]), \
            "'strip' & 'recompile' cannot both be true for <%s> '%s'" \
            % (fileXml.tag, name)
    return { name : fileInfo.dict() }

# WRITING FUNCTIONS

# XML file writer
def dumpXml(data, filename, validate = True):
    """Writes 'data' (python json) into XML file 'filename'. If 'validate' is
       True (default), reparses the generated file to ensure compliance."""
    logger.debug("Writing {}".format(filename))
    tree = generateTree(data)
    # any IOError/OSError should be caught by the caller
    tree.write(filename, encoding="UTF-8", xml_declaration = True)
    # apply formatting
    reparsed_xml = minidom.parse(filename)
    with open(filename, "w") as outfile:
        outfile.write(reparsed_xml.toprettyxml())
    if not validate:
        return
    loadXml(filename) # asserts on parse failure

def generateTree(data):
    """Builds a build config XML tree from the given 'data' (Python json
       structure)"""
    root = ElementTree.Element(TAGS["root"])
    root.extend(generateBranches(data))
    tree = ElementTree.ElementTree(root)
    return tree

def generateBranches(data):
    """Create branch tag hierarchy"""
    ret = []
    for branchName in data:
        branchInfo = dict(data[branchName])

        # build the attribute list for this node
        targetData = branchInfo["targets"]
        del branchInfo["targets"]
        branchInfo["name"] = branchName
        sanitizeAttributes(branchInfo)

        # generate element & subelements
        branchNode = ElementTree.Element(TAGS["branch"], branchInfo)
        branchNode.extend(generateTargets(targetData))

        ret.append(branchNode)
    return ret

def generateTargets(targetData):
    """Create target tag hierarchy"""
    ret = []
    for targetName in targetData:
        targetInfo = dict(targetData[targetName])

        # build the attribute list for this node
        fileData = targetInfo["files"]
        del targetInfo["files"]
        environmentData = targetInfo["environment"]
        del targetInfo["environment"]
        targetInfo["name"] = targetName

        sanitizeAttributes(targetInfo)

        # generate element & subelements
        targetNode = ElementTree.Element(TAGS["target"], targetInfo)
        targetNode.extend(generateEnvironment(environmentData))
        targetNode.extend(generateFiles(fileData))

        ret.append(targetNode)
    return ret

def generateEnvironment(environmentData):
    """Create environment tag hierarchy"""
    # generate element & subelements
    environmentNode = ElementTree.Element(TAGS["environment"])
    environmentNode.extend(generateOs(environmentData))

    return [ environmentNode ]

def generateFiles(fileData):
    """Create file tag hierarchy"""
    ret = []
    for aliasName in sorted(fileData):
        aliasInfo = dict(fileData[aliasName])

        # build the attribute list for this node
        argumentData = aliasInfo["arguments"]
        del aliasInfo["arguments"]
        artifactData = aliasInfo["artifacts"]
        del aliasInfo["artifacts"]
        dependencyData = aliasInfo["dependencies"]
        del aliasInfo["dependencies"]
        mapreportData = aliasInfo["mapreport"]
        del aliasInfo["mapreport"]
        paramData = aliasInfo["params"]
        del aliasInfo["params"]
        aliasInfo["name"] = aliasName
        sanitizeAttributes(aliasInfo)

        # generate element & subelements
        environmentNode = ElementTree.Element(TAGS["file"], aliasInfo)
        environmentNode.extend(generateArguments(argumentData))
        environmentNode.extend(generateArtifacts(artifactData))
        environmentNode.extend(generateDependencies(dependencyData))
        environmentNode.extend(generateMapreport(mapreportData))
        environmentNode.extend(generateParams(paramData))

        ret.append(environmentNode)
    return ret

def generateOs(osData):
    """Create os tag hierarchy"""
    ret = []
    for osName in osData:
        osInfo = dict(osData[osName])

        # build the attribute list for this node
        varData = osInfo["variables"]
        del osInfo["variables"]
        osInfo["name"] = osName
        sanitizeAttributes(osInfo)

        # generate element & subelements
        osNode = ElementTree.Element(TAGS["os"], osInfo)
        osNode.extend(generateVars(varData))

        ret.append(osNode)
    return ret

def generateVars(varData):
    """Create variable tags"""
    ret = []
    for varName in varData:
        varInfo = dict(varData[varName])
        varInfo["intermediate"] = not varInfo["export"]
        del varInfo["export"]
        varInfo["name"] = varName
        sanitizeAttributes(varInfo)
        ret.append(ElementTree.Element(TAGS["variable"], varInfo))

    return ret

def generateArtifacts(artifactData):
    """Create artifact tags"""
    ret = []
    for artifactName in artifactData:
        name, extension = os.path.splitext(artifactName)
        artifactInfo = {
            "name" : name,
        }
        if extension != ".mbn":
            artifactInfo["ext"] = extension
        sanitizeAttributes(artifactInfo)
        ret.append(ElementTree.Element(TAGS["artifact"], artifactInfo))

    return ret

def generateArguments(argData):
    """Create argument tags"""
    ret = []
    for argName in argData:
        argInfo = {
            "flag" : artifactName,
            "value" : argData[argName],
        }
        sanitizeAttributes(argInfo)
        ret.append(ElementTree.Element(TAGS["argument"], argInfo))

    return ret

def generateDependencies(dependencyData):
    """Create dependency tags"""
    ret = []
    for dependencyName in dependencyData:
        dependencyInfo = {
            "name" : dependencyName,
        }
        sanitizeAttributes(dependencyInfo)
        ret.append(ElementTree.Element(TAGS["dependency"], dependencyInfo))

    return ret

def generateMapreport(mapreportData):
    """Create mapreport tags"""
    ret = []
    for path in mapreportData:
        mrInfo = {
            "path" : path,
        }
        sanitizeAttributes(mrInfo)
        ret.append(ElementTree.Element(TAGS["mapreport"], mrInfo))

    return ret

def generateParams(paramData):
    """Create param tags"""
    ret = []
    for paramName in paramData:
        paramInfo = {
            "name" : paramName,
            "value" : paramValue,
        }
        sanitizeAttributes(paramInfo)
        ret.append(ElementTree.Element(TAGS["parameter"], paramInfo))

    return ret

# HELPER FUNCTIONS

def sanitizeAttributes(attributes):
    """Removes non-serializable types from XML attribute lists"""
    keysToRemove = set([])
    for attrname in attributes:
        assert isinstance(attrname, basestring), \
                "Invalid attribute name ({}): '{}'".format( \
                type(attrname), str(attrname))
        value = attributes[attrname]
        if isinstance(value, basestring):
            # strings are already serialized - nothing to do
            pass
        elif isinstance(value, list):
            if len(value) == 0:
                # empty lists should have their keys removed entirely
                keysToRemove.add(attrname)
            else:
                # otherwise, lists should be joined with a comma
                attributes[attrname] = ",".join(value)
        elif isinstance(value, bool):
            # booleans must be converted to their (XML) string form
            attributes[attrname] = "true" if value else "false"
        elif isinstance(value, (int, long)):
            # integers should be converted back to string
            attributes[attrname] = str(value)
        elif value is None:
            # None implies that the parameter is optional & can be entirely
            # removed from the attributes dictionary
            keysToRemove.add(attrname)
        else:
            # Anything else is invalid. This includes dictionaries, which should
            # be parsed as part of the dump/serialization logic outside of this
            # function.
            assert False, \
                    "Invalid attribute value for '{}' ({}): '{}'".format( \
                    attrname, type(value), str(value))
    for key in keysToRemove:
        del attributes[key]


def fileTemplate():
    """Defines a template file info instance as a PropertyWrapper. The resulting
       class will raise an exception if we attempt to set a value that is not
       explicitly defined here in the template."""
    defaults = {
        "arguments" : {},
        "artifacts" : [],
        "build-once" : False,
        "dependencies" : [],
        "disable" : False,
        "internal-test" : False,
        "mapreport" : [],
        "params" : [],
        "recompile" : False,
        "strip" : False,
    }
    ret = StrictMap(defaults)
    return ret

# getDependenciesFunc takes arguments:
# 1) xml.etree.ElementTree.Element(xmlNode)
# 2) set([ str(nodeName), ... ]) or dict( str(nodeName) : <anything> )
# returns [ str(dependencyName), ... ]
def validateGetDependenciesFunc(f):
    """Validation function for getDependenciesFunc dependency generation
       implementations"""
    assert type(f) == types.FunctionType, \
            "getDependenciesFunc candidate is not a function"
    # TODO - add additional validation

# getDependenciesFunc implementation for 'inherits-from' property
def getParent(xmlNode, \
        #pylint: disable=W0613
        nodeNames = None):
    """getDependenciesFunc dependency generation implementation for the XML
       'inherits-from' property"""
    # Only supports a single parent
    parent = xmlNode.attrib.get("inherits-from")
    if parent != None:
        return [ parent ]
    return []

# getDependenciesFunc implementation for variable 'value' property
def getVariableDependencies(varXml, nodeNames = None):
    """getDependenciesFunc dependency generation implementation for the XML
       <var> 'value' property"""
    name = checkProperty(varXml, "name") # required property
    value = checkProperty(varXml, "value") # required property
    method = checkProperty(varXml, "method", default = DEFAULT_VAR_METHOD, \
            allowed = SUPPORTED_VAR_METHODS)
    ret = []
    if method not in DEPENDENT_VAR_METHODS \
            and method not in ENV_DEPENDENT_VAR_METHODS:
        return ret
    nodeNames = nodeNames or set([])
    substrings = value.split(VAR_REF_DELIMITER)
    for substr in substrings:
#        logger.debug("Checking if '%s' is in %s" % (s, str(list(nodeNames))))
        if substr in nodeNames:
            if method == "str-join":
                logger.warn("<%s> '%s' uses str-join but references <%s> '%s'" \
                        % (varXml.tag, name, varXml.tag, substr))
            ret.append(substr)
        elif method in ("append", "default", "join", "prepend"):
            assert False, \
                    "<%s> '%s' is referenced in <%s> '%s' but not defined" \
                    % (varXml.tag, substr, varXml.tag, name)
    if method == "smart-join":
        #pylint: disable=C0301
        assertMsg = \
                "No defined vars detected in value of <%s> '%s'; use str-join instead" \
                % (varXml, name)
    else:
        assertMsg = "No dependencies detected for '%s' detected in <%s> '%s'" \
                % (method, varXml, name)
    assert len(ret) >= 1, assertMsg
    return ret

# getDependenciesFunc implementation for file <dependency> subnodes
def getFileDependencies(fileXml, nodeNames = None):
    """getDependenciesFunc dependency generation implementation for XML
       <dependency> nodes"""
    name = checkProperty(fileXml, "name") # required property
    dependencies = set([])
    nodeNames = nodeNames or {}
    for xmlSubnode in fileXml:
        if xmlSubnode.tag == TAGS.get("dependency"):
            depName = parseDependency(xmlSubnode)
            assert depName not in dependencies, \
                    "Duplicate <%s> '%s' defined for <%s> %s" \
                    % (xmlSubnode.tag, depName, fileXml.tag, name)
            dependencies.add(depName)
    return list(dependencies)

# applies all new keys from newMap into masterMap, overriding the values of any
# keys that already exist and adding key/value pairs for those that do not
# isClone is used internally (along with OPTIMIZE_MEMORY) to detemine when to
# create copies structures in the old map that may be shared with other objects
def override(masterMap, newMap, isClone = False):
    """Applies all new keys from newMap into masterMap, overriding the values of
       existing keys & adding new ones"""
    if len(newMap) == 0:
        return masterMap
    if OPTIMIZE_MEMORY and isClone:
        # something in this dictionary will be modified; perform a shallow copy
        # so that our changes don't affect the original
        masterMap = dict(masterMap)
    # now, check if the children are also cloned
    isClone = masterMap.get(RESERVED_CLONE_KEY) or isClone
    for key in newMap:
        newValue = newMap[key]
        existingValue = masterMap.get(key)
        if existingValue == None:
            # the value didn't exist; add it (no copy necessary)
            masterMap[key] = newValue
        else:
            # This value already exists & we will be overriding some part of it.
            # First, check if this structure is cloned; note that the
            # RESERVED_CLONE_KEY will never be set if OPTIMIZE_MEMORY is False.
            if type(existingValue) == dict:
                if logger.getEffectiveLevel() <= logging.DEBUG:
                    oldKeys = set(existingValue.keys())
                    newKeys = set(newValue.keys()).difference(oldKeys)
                    if len(newKeys) > 0:
                        logger.debug("Update dict %s with keys: " \
                                % ", ".join([ str(k) for k in newKeys ]))
                # this structure is a dictionary, so we can recurse
                masterMap[key] = override(existingValue, newValue, isClone)
            elif type(existingValue) == set:
                # copy the set (if needed) and add the new values
                if OPTIMIZE_MEMORY and isClone:
                    masterMap[key] = set(existingValue)
                oldLen = len(masterMap[key])
                masterMap[key].update(existingValue)
                added = len(masterMap[key]) - oldLen
                logger.debug("Added %d values to set %s" % (added, str(key)))
            elif type(existingValue) == list:
                # copy the list (if needed) and add the new unique values
                if OPTIMIZE_MEMORY and isClone:
                    masterMap[key] = list(existingValue)
                for item in newValue:
                    if item not in masterMap[key]:
                        logger.debug("Adding %s to list %s" \
                                % (str(item), str(key)))
                        masterMap[key].append(item)
            elif masterMap[key] != newValue:
                # this is some other (mutable) data (eg string, number, tuple),
                # so just set the new value
                logger.debug("Overriding key '%s' (%s: %s --> %s)" \
                        % (str(key), str(type(newValue)), \
                        str(existingValue), str(newValue)))
                masterMap[key] = newValue
            else:
                pass # value has not changed
    return masterMap

# Iterates through xmlNodeList and builds a dependency tree based on the
# 'inherits-from' property. Uses this dependency tree to generate an xml node
# list in which dependency resolution is guaranteed when traversed linearly.
# getDependenciesFunc is defined above its implementations
# parseFunc is defined above its implementations
# Note: xmlNodeMap is somewhat of a misnomer as it actually contains parsed data
# (and any data inherited may be passed in). It maps from node name -> data.
# WARNING: xmlNodeMap may be None if no node map is desired for proper
# inheritance but it is NOT a default argument. The caller must explicitly pass
# it as None in this case.
def resolveXmlDependencies(xmlNodeList, getDependenciesFunc, parseFunc, \
            xmlNodeMap):
    """Iterates through xmlNodeList and builds a dependency tree based on the
       'inherits-from' property. Uses this dependency tree to generate an xml
       node list in which dependency resolution is guaranteed when traversed
       linearly."""
    validateGetDependenciesFunc(getDependenciesFunc)
    validateParseFunc(parseFunc)
    xmlNodeMap = xmlNodeMap or {}
    if OPTIMIZE_MEMORY:
        # perform a shallow copy to handle structures shared via inheritance
        xmlNodeMap = dict(xmlNodeMap)
    iTree = buildXmlDependencyTree(xmlNodeList, getDependenciesFunc, False, \
            xmlNodeMap)
    # Building the traversal list also validates that there are no infinite
    # inheritance loops, thus why we skipped validation above
    traversalList = iTree.getTraversalList()
    if len(xmlNodeList) > 0:
        logV("resolveXmlDependencies():\ninput: %s\noutput: %s" \
                % (", ".join([ node.attrib.get("name", "<unknown name>") \
                for node in xmlNodeList ]), ", ".join([ node.name for node in \
                traversalList])))
    for node in traversalList:
        # if we passed in (inherited) existing nodes, this functions about the
        # same as the override() function
        parsedData = parseFunc(node.data, getDependenciesFunc, xmlNodeMap)
        existingData = xmlNodeMap.get(node.name)
        if existingData == None:
            xmlNodeMap[node.name] = parsedData
        else:
            # TODO - is this override really necessary anymore?
            logV("Override existing <%s> '%s' with new data" \
                    % (node.data.tag, node.name))
            override(existingData, parsedData, isClone = OPTIMIZE_MEMORY)
        logV("After parsing <%s> '%s', map = %s" \
                % (node.data.tag, node.name, str(xmlNodeMap)))
    return xmlNodeMap

# getTypeFunc implementation for XML nodes
def getXmlTagType(xmlNode):
    """getTypeFunc typing implementation for XML nodes"""
    return "<%s>" % xmlNode.tag

def buildDictDependencyTree(dictionary, getDependenciesFunc, \
        validateTraversal = True, existingObjectMap = None):
    """builds a dependency tree from dictionary values"""
    getname = lambda o: o
    return buildDependencyTree(dictionary, getname, dictionary.get, \
            getDependenciesFunc, validateTraversal, existingObjectMap)

def buildXmlDependencyTree(xmlNodes, getDependenciesFunc, \
        validateTraversal = True, xmlNodeMap = None):
    """builds a dependency tree from XML nodes"""
    getname = lambda x: checkProperty(x, "name")
    getdata = lambda x: x
    gettype = lambda x: "<%s>" % x.tag
    return buildDependencyTree(xmlNodes, getname, getdata, \
            getDependenciesFunc, validateTraversal, xmlNodeMap, gettype)

# getDependenciesFunc is defined above its implementations
def buildDependencyTree(objectList, getNameFunc, getDataFunc, \
        getDependenciesFunc, validateTraversal = True, \
        existingObjectMap = None, getTypeFunc = None):
    """Generic function to build dependency trees based on the output of its
       interface-driven input functions"""
    # TODO - @redesign to reduce arguments & local vars
    #pylint: disable=R0913, R0914
    validateGetDependenciesFunc(getDependenciesFunc)
    rootNode = DependencyNode.createRootNode()
    existingObjectMap = existingObjectMap or {}
    nodeLookup = {}
    nodeNames = set([ getNameFunc(obj) for obj in objectList ])
    for obj in existingObjectMap:
        nodeNames.add(obj)
    objectType = ""
    for obj in objectList:
        if getTypeFunc != None:
            objectType = "%s " % getTypeFunc(obj)
        name = getNameFunc(obj)
        myNode = nodeLookup.get(name)
        if myNode != None:
            # either a child created this node or it is a duplicate name
            assert myNode.data == None, \
                    "Two %sobjects exist with the same name: %s" \
                    % (objectType, name)
            # replace the dummy xml with our actual xml
            myNode.data = getDataFunc(obj)
        else:
            # we'll need a new node
            myNode = DependencyNode(name, getDataFunc(obj))
        parents = getDependenciesFunc(getDataFunc(obj), nodeNames)
        if len(parents) == 0:
            # no parents, so create a new top-level node
            rootNode.addChild(myNode)
        else:
            # we have 1+ valid parents
            for parent in parents:
                parentNode = nodeLookup.get(parent)
                if parentNode == None:
                    # the parent has not been parsed yet; create a placeholder
                    # in the lookup table, but don't update the tree yet
                    parentNode = DependencyNode(parent, children = \
                            set([ myNode ]))
                    nodeLookup[parent] = parentNode
                else:
                    parentNode.addChild(myNode)
        # update the lookup for all cases
        nodeLookup[name] = myNode
    # validate that all parents were resolved
    for name in nodeLookup:
        node = nodeLookup[name]
        if node.data == None:
            # We have unresolved data but we might have the inherited data
            # already parsed; if so, we can ignore the resolution issue
            if name not in existingObjectMap:
                logger.debug("%s'%s' has children: %s" \
                        % (objectType, name, node.childrenAsString()))
                #pylint: disable=C0301
                assert node.childCount > 0, \
                        "BUG: Unresolved inheritance detected from in %sobject but '%s' has no children!" \
                        % (objectType, name)
                #pylint: enable=C0301
                assert node.data != None, \
                        "Unresolved %sinheritance from '%s' -> %s" \
                        % (objectType, name, node.childrenAsString())
            else:
                # The parent node is resolved via inheritance, so for the
                # purpose of traversal, we need to cut this 'dummy' node out of
                # the tree and attach its children directly to its parents
                logger.debug( \
                        #pylint: disable=C0301
                        "Inheritance of %s%s from '%s' is resolved via inherited values" \
                        % (objectType, node.childrenAsString(), name))
                parents = node.parents
                if len(node.parents) == 0:
                    # no parents --> attach to root node
                    parents = [ rootNode ]
                # attach children to our parents
                for child in node.children:
                    node.removeChild(child)
                    for parent in parents:
                        # since the structure uses sets for parent/child lists,
                        # there is no issue if this link already existed
                        parent.addChild(child)
                # If this node was inherited, it was never attached as a child
                # of the root node. We only need to remove links to actual
                # parent nodes, so we use 'node.parents' instead of 'parents'
                for parent in node.parents:
                    parent.removeChild(node)
    if validateTraversal:
        # validate that there are no recursive references by generating a
        # discrete traversal order for each root node
        # Note: names are guaranteed to be unique
        rootNode.getTraversalList()
    return rootNode

def checkProperty(xmlNode, propertyName, default = None, allowed = None, \
        forbidden = None, errorMsg = None):
    """Check that the provided XML node has something defined for the given
       property. If 'default' is provided, will use this value as the default if
       the property is not found (still validates 'default' with 'allowed' &
       'forbidden' lists). If 'allowed' is given, checks to ensure that the
       value is contained within 'allowed'. If 'forbidden' is given, checks to
       ensure that the value is NOT contained within 'forbidden'. 'allowed' &
       'forbidden' may also be functions that accept a string input & return
       True or False. Error message is generic unless explicitly specified via
       'errorMsg'."""
    # Returns the property value
    value = xmlNode.attrib.get(propertyName, default)
    nameHeader = ""
    if propertyName != "name":
        nodeName = xmlNode.attrib.get("name", "")
        if len(nodeName) > 0:
            nameHeader = "[%s] " % nodeName
    msg = errorMsg or "%sAll <%s> tags must have a '%s' property" \
            % (nameHeader, xmlNode.tag, propertyName)
    assert value != None, msg
    if allowed != None:
        msg = errorMsg or "%s'%s' is not a valid value for <%s> property '%s'" \
                % (nameHeader, value, xmlNode.tag, propertyName)
        if callable(allowed):
            assert allowed(value), msg
        else:
            assert value in allowed, msg
    if forbidden != None:
        msg = errorMsg or "%s'%s' is a forbidden value for <%s> property '%s'" \
                % (nameHeader, value, xmlNode.tag, propertyName)
        if callable(forbidden):
            assert not forbidden(value), msg
        else:
            assert value not in forbidden, msg
    return value

def parseBooleanProperty(xmlNode, propertyName, default):
    """Helper function for parsing boolean properties"""
    assert type(default) in (bool, str), "default must be a string or boolean"
    if type(default) == bool:
        default = "true" if default else "false"
    else: # must be str
        assert default in BOOLEAN_STRINGS, "invalid boolean default '%s'" \
                % default
    rawString = checkProperty(xmlNode, propertyName, default, \
            allowed = BOOLEAN_STRINGS)
    return rawString.lower() == "true"

def parseIntegerProperty(xmlNode, propertyName, default, allowNegative = True, \
        allowZero = True):
    """Helper function for parsing integer properties. If 'allowNegative' is
       False (default True), considers negative values forbidden. If 'allowZero'
       is False (default True), considers '0' forbidden. Note that the default
       value IS subject to these same restrictions."""
    try:
        default = int(default)
    except TypeError, ValueError:
        assert False, "default must be an integer or integer string"
    rawString = checkProperty(xmlNode, propertyName, default, allowed = \
            lambda x: isValidInt(x, allowNegative, allowZero))
    return int(rawString)

def unexpectedTag(unexpectedNode, parentXml = None):
    """Special handling for unexpected XML tags"""
    name = unexpectedNode.attrib.get("name")
    tag = "<%s%s>" % (unexpectedNode.tag, " name='%s'" % name if name else "")
    if parentXml != None:
        parentName = parentXml.attrib.get("name")
        parentTag = "<%s%s>" % (parentXml.tag, " name='%s'" % parentName \
                if parentName else "")
        assert False, "Encounted unexpected XML tag %s within <%s>" \
                % (tag, parentTag)
    else:
        assert False, "Encounted unexpected XML tag <%s>" % tag

def isValidInt(value, allowNegative, allowZero):
    """Returns True if the given integer value is a 'valid' integer or integer
       string based on booleans 'allowNegative' & 'allowZero', else False."""
    try:
        value = int(value)
    except TypeError, ValueError:
        return False
    if not allowNegative and value < 0:
        return False
    if not allowZero and value == 0:
        return False
    return True

def stripPrefix(chipset):
    """Removes non-numeric prefixes from chipset names (eg msm8996 -> 8996 or
       apq8974ac -> 8974ac)"""
    i = 0
    for i in range(len(chipset)):
        if chipset[i].isdigit():
            break
    ret = chipset[i:]
    if len(ret) == 0:
        return None
    return ret

# TODO - this is a slight hack to handle mapreport paths nicely. ideally, we
# should find a cleaner way to do this in the future
# Takes an arbitrary path of arbitrary OS format and attempts to resolve it to
# the local machine's path format
def convertPath(path):
    """Converts paths to the current OS's path style. Specifically intended to
       convert unix-style mapreport paths to the current OS style"""
    if '/' in path: # # '/' is not an escape character, so:
        if os.sep == '/':
            # assume that the path is already converted
            return path
        else: # assume os.sep == "\\"
            # this path needs converted
            return path.replace('/', os.sep)
    if os.sep == "/":
        # this path either needs converted or is a single-level file/dir
        return path.replace("\\", os.sep)
    else: # assume os.sep == "\\"
        # our separator is '\\' and there are no '/' chars in the path; just
        # assume that the path is already converted
        return path

def loadAliasMap(data):
    """Creates a lookup table of all aliases in the data set"""
    # Build a map of all aliases; these are very lightweight within the data
    # structure but will cause full replication if we dump them.
    aliasMap = {}
    for key in data:
        aliases = data[key].get("aliases", [])
        # an item is an alias if it has its own name in its alias list
        if key in aliases:
            continue
        for alias in aliases:
            aliasMap[alias] = key
    return aliasMap

def dumpConfig(data, replaceAliases = True):
    """Debugging function that dumps the loaded, expanded XML config as JSON"""
    if replaceAliases:
        data = dict(data) # create a shallow copy so we don't affect the source
        aliasMap = loadAliasMap(data)
        logV("alias map = %s" % aliasMap)
        # Now, before we dump, overwrite all aliases with "<alias of 'name'>" to
        # minimize duplicated space
        for key in data:
            source = aliasMap.get(key)
            if source == None:
                continue
            logger.debug("Replacing alias '%s' before dump" % key)
            data[key] = "<alias of '%s'>" % source
    print(json.dumps(data, sort_keys = True, indent = 4, \
                separators = (',', ': ')))

def logV(msg):
    """verbose-level logging for the module"""
    logger.log(VERBOSE, msg)

def parseArguments():
    """Basic argument definition & parsing"""
    import optparse
    parser = optparse.OptionParser()
    parser.add_option("-o", "--output", \
            action = "store_true", \
            dest = "dump", \
            default = False, \
            help = "Build this branch")
    parser.add_option("-v", "--verbose", \
            action = "store_true", \
            dest = "verbose", \
            default = False, \
            help = "Verbose output")
    return parser.parse_args()

if __name__ == "__main__":
    import json
    (OPTIONS, ARGS) = parseArguments()
    if OPTIONS.verbose and logger.getEffectiveLevel() > logging.DEBUG:
        logger.setLevel(logging.DEBUG)
    assert len(ARGS) > 0, "Must provide an XML file"
    if len(ARGS) > 1:
        logger.warn("Skipping unexpected arguments: %s" \
                % ", ".join(ARGS[1:]))
    try:
        XML = loadXml(ARGS[0])
    except Exception as EXC:
        logger.critical("Error encountered while parsing %s" % ARGS[0])
        raise
    logger.info("Successfully parsed %s" % ARGS[0])
    if OPTIONS.dump:
        dumpConfig(XML)
    exit(0)
