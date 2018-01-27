#!/usr/bin/python3
""" Author: James Lee
Email: jamesl33info@gmail.com
Supported Python version: 3.5.2+

Python script to allow the manipulation of steganographic images
"""

import sys

import bitstring
from PIL import Image


class Steganography():
    """ Create and reads steganographic images made using last bit steganography

    Keyword arguments:
    image_path -- The location of the image which this class will manipulate
    """

    def __init__(self, image_path):
        try:
            self.image = Image.open(image_path)
        except FileNotFoundError:
            print('File not found')
            sys.exit(1)

        self.pixels = self.image.load()
        self.image_path = image_path
        self.width = self.image.size[0]
        self.height = self.image.size[1]
        self.channels = len(self.pixels[0, 0])

    def encode_file(self, file_path):
        """ Encode a file into a 'png' or 'bmp' image

        Keyword arguments:
        file_path -- Path to file which will be hidden in the image
        """
        data = bitstring.ConstBitStream(filename=file_path)
        data = data.read('bin')
        self._encode_bitstring(data)

    def decode_file(self):
        """ Decode a file hidden in a 'png' or 'bmp' image """
        data = self._decode_bitstring()

        # TODO Make output filename match name of original file
        with open('steg-output', 'wb+') as output_file:
            data.tofile(output_file)

    def _encode_bitstring(self, bstring):
        """ Encode a 'bitstring' object into a 'png' or 'bmp' image

        Keyword arguments:
        bstring -- 'bitstring' object which will be hidden the image
        """
        index = 0
        size = '{0:032b}'.format(len(bstring))
        bstring = size + bstring
        bstring_length = len(bstring)

        if bstring_length > self.width * self.height * self.channels:
            raise ValueError('No enough room in image to store this message')

        for row in range(self.width):
            for col in range(self.height):
                for cha in range(self.channels):
                    try:
                        new_pixel_val = list(self.pixels[row, col])
                        new_pixel_val[cha] = self._set_lsb(new_pixel_val[cha], int(bstring[index]))
                        self.pixels[row, col] = tuple(new_pixel_val)
                        index += 1
                    except IndexError:
                        break

        self.image.save('steg-' + self.image_path)

    def _decode_bitstring(self):
        """ Decode a 'bitstring' object from a 'png' or 'bmp' image

        Returns:
        bitstring -- binary representation of decoded file
        """

        def image_loop(end):
            """ Loop through image pixels to obtain 'bitstring' object

            Keyword arguments:
            end -- How many bits to decode

            Returns:
            str -- binary
            """
            data = ''
            current_length = 0
            for row in range(self.width):
                for col in range(self.height):
                    for cha in range(self.channels):
                        if current_length == end:
                            break
                        data += str(self._get_lsb(self.pixels[row, col][cha]))
                        current_length += 1
            return data

        bstring_length = ((int(image_loop(32), 2)) + 32)
        bstring = image_loop(bstring_length)

        return bitstring.Bits('0b' + bstring[32:])

    @classmethod
    def _get_lsb(cls, number):
        """ Get the least significant bit on an integer

        Keyword arguments:
        number -- Integer value to fetch the least significant bit of

        Returns:
        Int -- Least significant bit on input iteger
        """
        return number & 1

    @classmethod
    def _set_lsb(cls, number, bit_value):
        """ Modifys and integer and sets the least significant bit

        Keyword arguments:
        number -- Integer value to set the least significant bit of
        bit_value -- Boolean value depending if you want to set the bit to '1' or '0'

        Returns:
        Int -- Value with least significant bit set
        """
        if bit_value:
            return number | bit_value
        return (number & ~1) | bit_value
