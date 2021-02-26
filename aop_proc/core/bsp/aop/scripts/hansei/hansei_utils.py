#  ===========================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.  
#  All Rights Reserved.
#  QUALCOMM Proprietary and Confidential.
#
#  ===========================================================================

'''
print_p - pretty print function
'''
def print_p(string, fill=" ", format="<", tab=4):
    format = {
        format   : format[0],
        "left"   : "<",
        "right"  : ">",
        "center" : "^",
    }[format]
    f_string = "#{0:%s%s78}#"%(fill,format)
    print(f_string.format(string.expandtabs(tab)))


from dwarf import decode_object, Structure

def check_member(member, object):
    if hasattr(object, 'members'):
        for x in object.members():
            if member in x:
                return True
    return False
    
def cast(obj, typename, memory, debug_info, address=None):
    type_die = debug_info.types[typename]
    if hasattr(obj, '_name'):
        return decode_object(obj._name, obj.address, None, memory, debug_info, die=type_die)
    elif address:
        return decode_object('', address, None, memory, debug_info, die=type_die)
    else:
        return decode_object('', 0, None, memory, debug_info, die=type_die)

def pointer_to_str(ptr):
    x = 0
    output = ""
    try:
        while ptr[x] != 0x0:
            output+=chr(ptr[x])
            x+=1
        return output
    except:
        #import pdb; pdb.set_trace()
        return "Bad String Pointer"

def update_logger(new_level, logging):
    save_logger_level = logging.getLogger('dwarf').getEffectiveLevel()
    logging.getLogger('dwarf').setLevel(new_level)
    print("NEW LOGGING LEVEL: %d" % logging.getLogger('dwarf').getEffectiveLevel())
    return save_logger_level
    
def type_to_str(type_int):
    type_str = ''
    while type_int > 0:
        type_str += chr(type_int & 0xFF)
        type_int >>= 8

    if not type_str:
        return '<empty>'
    return type_str
    
def find_value(value, length, mem_start, mem_end, memory):
    output = ""
    #import pdb; pdb.set_trace()
    for addr in range(mem_start, mem_end, 4):
        if (memory.read(addr, length) == str_name):
            return addr
    return 0x0
