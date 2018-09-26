#!/usr/bin/env python3
""" Customized OpenWrt for research and fun.

Long Live This Project!!!
"""

import os
import argparse
import configparser
import shutil


def main():
    """main function."""
    args = process_arg()
    configs = process_config()

    if args.install_all:
        print('>>> Install all modifications (kernel, packages, etc.)')
        install_kernel(configs)
        install_package(configs)

    if args.uninstall_all:
        print('<<< Uninstall all and recover OS to original')
        uninstall_kernel(configs)
        uninstall_package(configs)

    if args.kernel_install:
        print('>>> Modify kernel')
        install_kernel(configs)

    if args.kernel_uninstall:
        print('<<< Remove kernel modification')
        uninstall_kernel(configs)

    if args.package_install:
        print('>>> Install customized packages')
        install_package(configs)

    if args.package_uninstall:
        print('<<< Remove customized packages')
        uninstall_package(configs)

    return 0


def process_arg():
    """argument processing."""
    parser = argparse.ArgumentParser(description=__doc__, allow_abbrev=False)
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument('--install-all', help='install all modifications (kernel, packages, etc.)', action='store_true')
    group.add_argument('--uninstall-all', help='uninstall all and recover OS to original', action='store_true')
    group.add_argument('--kernel-install', help='modify kernel', action='store_true')
    group.add_argument('--kernel-uninstall', help='remove kernel modification', action='store_true')
    group.add_argument('--package-install', help='install customized packages', action='store_true')
    group.add_argument('--package-uninstall', help='remove customized packages', action='store_true')
    return parser.parse_args()


def process_config():
    """process config file from main.ini."""
    config_dict = {}
    config = configparser.ConfigParser()
    path_to_file = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'main.ini')
    print('Reading config file: ' + path_to_file)
    config.read(path_to_file)
    target_board = config['Board']['TargetBoard']
    print('--- Target board: ' + target_board)
    if target_board == 'apu2':
        config_dict["KERNEL_PATCH_DST"] = 'target/linux/x86/patches-4.14'
        config_dict["KERNEL_PATCH_SRC"] = 'Patch/target/linux/x86/patches-4.14'

    return config_dict


def install_kernel(configs):
    """install kernel patch."""
    print('>>>>>> Install kernel patch to ' + configs["KERNEL_PATCH_DST"])
    src_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), configs["KERNEL_PATCH_SRC"])
    dst_dir = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), configs["KERNEL_PATCH_DST"])
    src_files = os.listdir(src_dir)
    for src_filename in src_files:
        full_file_name = os.path.join(src_dir, src_filename)
        if os.path.isfile(full_file_name):
            shutil.copy2(full_file_name, dst_dir)


def uninstall_kernel(configs):
    """uninstall kernel patch"""
    print('<<<<<< Uninstall kernel patch from ' + configs["KERNEL_PATCH_DST"])
    src_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), configs["KERNEL_PATCH_SRC"])
    dst_dir = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), configs["KERNEL_PATCH_DST"])
    src_files = os.listdir(src_dir)
    dst_files = os.listdir(dst_dir)
    for dst_filename in dst_files:
        full_file_name = os.path.join(dst_dir, dst_filename)
        if os.path.isfile(full_file_name):
            if dst_filename in src_files:
                os.remove(full_file_name)


def install_package(configs):
    pass


def uninstall_package(configs):
    pass


if __name__ == '__main__':
    main()
