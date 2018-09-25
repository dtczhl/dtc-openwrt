#!/usr/bin/env python3

import argparse

parser = argparse.ArgumentParser()
parser.add_argument('-i', '--install', help='install my modifications',
                    action='store_true')
parser.add_argument('-u', '--uninstall', help='system recovers to original',
                    action='store_true')

args = parser.parse_args()

if args.install:
    print('install')

if args.uninstall:
    print('uninstall')