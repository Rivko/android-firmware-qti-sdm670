#!/usr/bin/env python2.7
"""
meta_lib_shim.py

Shim between sectools (possibly running under python 3) and meta_lib.py (supplied
by the metabuild, which is not python3 compliant)
"""
import argparse
import json
import sys

RESULT_TOKEN = "Result >>>"


def parse_args():
    """ Parse command line """
    parser = argparse.ArgumentParser(
        description="Retrieve a meta lib object from the given meta path"
    )
    parser.add_argument(
        'lib_path', action='store',
        help='Path to the metabuild'
    )
    parser.add_argument(
        'contents_xml', action='store',
        help='Path to contents.xml'
    )

    subparsers = parser.add_subparsers(help='command')

    # get_build_path
    cmd = 'get_build_path'
    gbp_parser = subparsers.add_parser(cmd)
    gbp_parser.add_argument(
        'component_name', action='store',
    )
    gbp_parser.set_defaults(cmd=cmd)

    # get_file_vars
    cmd = 'get_file_vars'
    gfv_parser = subparsers.add_parser(cmd)
    gfv_parser.add_argument(
        'val', action='store',
    )
    gfv_parser.add_argument(
        '--flavor', action='store',
    )
    gfv_parser.set_defaults(cmd=cmd)

    # get_files
    cmd = 'get_files'
    gf_parser = subparsers.add_parser(cmd)
    gf_parser.add_argument(
        'val', action='store',
    )
    gf_parser.add_argument(
        '--flavor', action='store',
    )
    gf_parser.set_defaults(cmd=cmd)

    # get_files_detailed
    cmd = 'get_files_detailed'
    gfd_parser = subparsers.add_parser(cmd)
    gfd_parser.add_argument(
        'val', action='store',
    )
    gfd_parser.set_defaults(cmd=cmd)

    # get_product_detail
    cmd = 'get_product_info'
    gpi_parser = subparsers.add_parser(cmd)
    gpi_parser.set_defaults(cmd=cmd)

    return parser.parse_args()

def run(args):
    """ Process command line request """
    lib_path = args.lib_path

    sys.path.append(lib_path)
    # noinspection PyUnresolvedReferences
    import meta_lib  # pylint: disable=F0401

    meta_info = meta_lib.meta_info()

    ret = None
    if args.cmd == 'get_build_path':
        ret = meta_info.get_build_path(args.component_name)

    elif args.cmd == 'get_file_vars':
        kwargs = {}
        if args.flavor:
            kwargs['flavor'] = args.flavor
        ret = meta_info.get_file_vars(args.val, **kwargs)

    elif args.cmd == 'get_files':
        kwargs = {'file_types': args.val}
        if args.flavor:
            kwargs['flavor'] = args.flavor
        ret = meta_info.get_files(**kwargs)

    elif args.cmd == 'get_files_detailed':
        ret = meta_info.get_files_detailed(args.val)

    elif args.cmd == 'get_product_info':
        ret = meta_info.get_product_info()

    print(RESULT_TOKEN)
    return json.dumps(ret)


if __name__ == "__main__":
    print(run(parse_args()))
