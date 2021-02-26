"""Collection of tools/methods for safe generation, deployment & verification of
   various devcfg images."""

import inspect
import os

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
    return env.Detect('ssg_devcfg_tools')


def generate(env):
    try:
        # Any method added with AddMethod() is actually a SCons MethodWrapper
        # class. The original method is stored as the 'method' property.
        orig_add_devcfg_info = env.AddDevCfgInfo.method
    except AttributeError:
        assert False, \
                "ssg_devcfg_tools must always be loaded after devcfg_builder"

    env.AddMethod(orig_add_devcfg_info, "AddInternalDevCfgInfo")
    env.AddMethod(add_shippable_info, "AddShippableDevCfgInfo")
    env.AddMethod(set_debug_params, "SetDevcfgDebugParams")

    # override the default AddDevCfg info with our safe (shippable) one
    env.AddMethod(add_shippable_info, "AddDevCfgInfo")

    # now use DeclareFeature to declare this script itself
    env.DeclareFeature("SSG_DEVCFG_TOOLS", uses_prefix=True)

    # set default debug params
    env.SetDevcfgDebugParams(print_info=False)


def add_shippable_info(env, targets, thread_input_dict):
    """Wraps the usual AddDevCfgInfo call, adding env.Deploy() calls for all
       sources and the calling SConscript."""
    target = env.Flatten(targets)
    calling_sconscript = inspect.stack()[2][1]

    deploy_units = get_deployable_devcfg_units(env, targets, thread_input_dict)

    if len(deploy_units) > 0:
        # some source is getting deployed, so add the calling SConscript
        deploy_units.append(calling_sconscript)

    env.Deploy(deploy_units)
    env.Alias("deploy", deploy_units)

    # finally, call the actual AddDevCfgInfo method
    env.AddInternalDevCfgInfo(targets, thread_input_dict)

    filter_func = env.get("SSG_DEVCFG_FILTER")
    if filter_func != None and not filter_func(calling_sconscript):
        return

    if env.get("SSG_DEVCFG_PRINT"):
        env.PrintInfo("{0} START DEVCFG DEBUG {0}".format(10 * "*"))
        env.PrintInfo("DEVCFG: caller: {}".format(calling_sconscript))
        env.PrintInfo("targets = {}".format(targets))
        env.PrintInfo("build_tags = {}".format(env.get('BUILD_TAGS')))
        env.PrintInfo("deploy_units = {}".format(deploy_units))
        env.PrintInfo("thread_input_dict = {}".format(thread_input_dict))
        env.PrintInfo("{0} END DEVCFG DEBUG {0}".format(10 * "*"))

    if env.get("SSG_DEVCFG_STOP"):    
        assert False, "break - matched devcfg stop filter"


def set_debug_params(env, print_info=True, filter_func = None, stop=False):
    """Sets global devcfg debug parameters:
       * 'print_info' - if True, prints devcfg source & deployment information
       * 'filter_func' - if non-None, executes the given function on the calling
                         SConscript's path; only applies print/stop if the
                         function returns True
       * 'stop' - asserts after printing once the filter_func returns True"""
    assert filter_func == None or callable(filter_func), \
            "filter_func must be a callable function returning bool"

    env.Replace(SSG_DEVCFG_PRINT=print_info)
    env.Replace(SSG_DEVCFG_FILTER=filter_func)
    env.Replace(SSG_DEVCFG_STOP=stop)


def get_deployable_devcfg_units(env, targets, thread_input_dict):
    """Returns a list of valid input .xml & .c devcfg source files that will be
       needed for customer recompilation"""
    # If this target doesn't use devcfg, bail out
    if env.GetUsesFlag('USES_DEVCFG') is False:
        return []

    # Is the config passed to this API relevant to this image?
    if len(set(targets) & set(env.get('BUILD_TAGS'))) == 0:
        return []

    deploy_units = []
    tag_list = env.get('DEVCFG').getFeature('devcfgXMLTagList')

    thread_input_dict = _resolve_soc_all_tag(env, thread_input_dict, tag_list)

    for target_image in targets:
        if target_image not in env.get('BUILD_TAGS'):
            continue

        for each_xml_tag in tag_list:     
            if each_xml_tag in thread_input_dict:
                xml_file_list = _find_file_name_list(env, thread_input_dict, \
                        each_xml_tag, '.xml')
                xml_file_list = _check_and_ignore_xml_file_duplicates(env, \
                        xml_file_list, each_xml_tag, target_image)
                deploy_units.extend(xml_file_list)

                c_file_list = _find_file_name_list(env, thread_input_dict, \
                        each_xml_tag, '.c')
                deploy_units.extend(c_file_list)

    return deploy_units


# helper methods copied/adapter from devcfg_builder
def _find_file_name_list(env, thread_input_dict, each_xml_tag, ext):  
    file_list = []
    for filename in thread_input_dict[each_xml_tag]:
         fullpath = env.RealPath(str(filename))
         fname = os.path.basename(fullpath)
         fname_ext = os.path.splitext(fname)[1]
         if ext.lower() in fname_ext.lower():
            file_list.append(fullpath)

    return set(file_list)


def _check_and_ignore_xml_file_duplicates(env, xml_file_list, each_xml_tag, \
        target_image):
    feature = env.get('DEVCFG').getFeature('DEVCFG_INFO_COLLECTOR')
    existing_pp_xml_node_list = feature.get(target_image, {}).get( \
            'pp_' + each_xml_tag, [])
    existing_pp_xml_file_list = \
            [node.name for node in existing_pp_xml_node_list]

    for xml_file in xml_file_list:
        if ((os.path.basename(xml_file) + '.i') in existing_pp_xml_file_list):
            xml_file_list.remove(xml_file)

    return xml_file_list 


def _resolve_soc_all_tag(env, thread_input_dict, tag_list):
    thread_input_dict = dict(thread_input_dict)
    key = 'NULL'
    if thread_input_dict.has_key('SOC_XML'):
       key = 'SOC_XML'
    elif thread_input_dict.has_key('soc_xml'):
       key = 'soc_xml'   
    elif thread_input_dict.has_key('devcfg_xml'):
        key = 'devcfg_xml'
        
    if key is not 'NULL':   
       soc_list = []
       for tags in tag_list:
          if 'PLATFORM' not in tags:
             soc_list.append(tags)
    
       cfglist = thread_input_dict[key]
       if(type(cfglist) is not list):
          cfglist = [cfglist]

       for soc in soc_list:
          thread_input_dict[soc] = cfglist

       del thread_input_dict[key]
  
    return thread_input_dict  
