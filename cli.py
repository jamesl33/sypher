#!/usr/bin/python3
""" Author: James Lee
Email: jamesl33info@gmail.com
Supported Python version: 3.5.2+

Python script to enable the use of pysteganography from the command line
"""

import argparse

from pysteganography import Steganography


def command_line():
    """ Parse command line arguments and allow easy use of pysteganography """
    parser = argparse.ArgumentParser(description='Simple Python script to use "Last Bit Steganography" to hide messages in images')
    parser.add_argument('-i', '--input-file', action='store', type=str, help='Path to "PNG"/"BMP" which your file will be hidden in or is hidden in *THIS OPTION IS MANDITORY*')
    parser.add_argument('-e', '--encode-file', action='store', type=str, help='Path to the file you want to hide')
    parser.add_argument('-d', '--decode-file', action='store_true', help='Decodes "--input-file"')
    arguments = parser.parse_args()

    if not arguments.input_file:
        raise ValueError('TODO')

    if arguments.encode_file and arguments.decode_file:
        raise ValueError('TODO')

    if arguments.encode_file:
        steg = Steganography(arguments.input_file)
        steg.encode_file(arguments.encode_file)

    if arguments.decode_file:
        steg = Steganography(arguments.input_file)
        steg.decode_file()


if __name__ == '__main__':
    command_line()
