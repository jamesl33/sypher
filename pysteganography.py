#!/usr/bin/python3
"""pysteganography: Python script for the management of steganographic images

References:
text_to_bits - https://stackoverflow.com/questions/7396849/convert-binary-to-ascii-and-vice-versa
text_from_bits - https://stackoverflow.com/questions/7396849/convert-binary-to-ascii-and-vice-versa
"""

from PIL import Image

class Steganography():
    """Steganography: Class which allows the creation and reading of steganographic images

    :param image_path: image which will be read using 'python-pillow'
    :param save_path: path to where output images will be saved
    """
    def __init__(self, image_path, save_path=None):
        if save_path is None:
            self.save_path = 'steg-'
            self.save_path += image_path
        else:
            self.save_path = save_path

        self.image = Image.open(image_path)
        self.image = self.image.convert('RGBA')
        self.pixels = self.image.load()

    def encode_text(self, message):
        """encode_text: Encode 'message' into a 'png' or 'bmp' image

        :param message: string to be hidden in the image
        """
        bit = 0
        message = self.text_to_bits(message)
        size = '{0:032b}'.format(len(message))
        message = size + message

        if len(message) > self.image.size[0] * self.image.size[1] * 4:
            raise ValueError('No enough room in image to store this message')

        for row in range(self.image.size[0]):
            for col in range(self.image.size[1]):
                for cha in range(3):
                    if bit >= len(message):
                        break
                    if cha == 0:
                        self.pixels[row, col] = (self.set_lsb(self.pixels[row, col][0], int(message[bit])),
                                                 self.pixels[row, col][1],
                                                 self.pixels[row, col][2],
                                                 self.pixels[row, col][3])
                    elif cha == 1:
                        self.pixels[row, col] = (self.pixels[row, col][0],
                                                 self.set_lsb(self.pixels[row, col][1], int(message[bit])),
                                                 self.pixels[row, col][2],
                                                 self.pixels[row, col][3])
                    elif cha == 2:
                        self.pixels[row, col] = (self.pixels[row, col][0],
                                                 self.pixels[row, col][1],
                                                 self.set_lsb(self.pixels[row, col][2], int(message[bit])),
                                                 self.pixels[row, col][3])
                    elif cha == 3:
                        self.pixels[row, col] = (self.pixels[row, col][0],
                                                 self.pixels[row, col][1],
                                                 self.pixels[row, col][2],
                                                 self.set_lsb(self.pixels[row, col][3], int(message[bit])))
                    bit += 1

        self.image.save(self.save_path)

    def decode_text(self):
        """decode_text: Attempt to read an image for a hidden message
        """
        message = ''
        message_len = ''
        for row in range(self.image.size[0]):
            for col in range(self.image.size[1]):
                for cha in range(3):
                    if len(message_len) == 32:
                        break
                    message_len += str(self.get_lsb(self.pixels[row, col][cha]))

        message_len = ((int(message_len, 2)) + 32)

        for row in range(self.image.size[0]):
            for col in range(self.image.size[1]):
                for cha in range(3):
                    if len(message) == message_len:
                        break
                    message += str(self.get_lsb(self.pixels[row, col][cha]))

        message = message[32:]
        return self.text_from_bits(message)

    @classmethod
    def get_lsb(cls, num):
        """get_lsb: Get the least significant bit of input number

        :param num: Number to get the lsb of
        """
        return num & 1

    @classmethod
    def set_lsb(cls, number, bit_value):
        """set_lsb: Set the least significant bit of a number *Does not actually modify the number*

        :param number: Number which to set the lsb
        :param bit_value: Boolean value of what to set the lsb too
        """
        if bit_value:
            return number | bit_value
        return (number & ~1) | bit_value

    @classmethod
    def text_to_bits(cls, text, encoding='utf-8', errors='surrogatepass'):
        """text_to_bits:
        Convert text to binary

        :param text: Text to be converted
        """
        bits = bin(int.from_bytes(text.encode(encoding, errors), 'big'))[2:]
        return bits.zfill(8 * ((len(bits) + 7) // 8))

    @classmethod
    def text_from_bits(cls, bits, encoding='utf-8', errors='surrogatepass'):
        """text_from_bits:
        Convert binary into a string

        :param bits: Binary to convert to a string
        """
        try:
            bits = int(bits, 2)
            return bits.to_bytes((bits.bit_length() + 7) // 8, 'big').decode(encoding,
                                                                             errors) or '\0'
        except ValueError:
            pass

def main():
    """main: Driver code for the commandline portion of this script
    """
    parser = argparse.ArgumentParser(description='Simple Python script to use \
                                     "Last Bit Stegonography" to hide messages in images')

    parser.add_argument('-e',
                        '--encode-text',
                        action="store",
                        type=str,
                        help='Text which you want to hide in the specified image')

    parser.add_argument('-d',
                        '--decode-text',
                        action="store_true",
                        help='Attempt to decode hidden text from the specified image')

    parser.add_argument('-f',
                        '--input-file',
                        action="store",
                        type=str,
                        help='File which you want to operate with. This must be a "png" or a "bmp"')

    parser.add_argument('-o',
                        '--output-file',
                        action="store",
                        type=str,
                        help='Filename which output will be saved too')

    arguments = parser.parse_args()

    if not arguments.input_file:
        raise ValueError('You must specify a file using either "-f" or "--file"')

    if arguments.encode_text and arguments.decode_text:
        raise ValueError("You can't both encode and decode at the same time")

    steg = Steganography(arguments.input_file, arguments.output_file)

    if arguments.encode_text:
        steg.encode_text(arguments.encode_text)

    if arguments.decode_text:
        message = steg.decode_text()
        if not message:
            print("This image file doesn't contain a message hidden using 'Last Bit Steganography'")
        else:
            print(message)

if __name__ == '__main__':
    import argparse
    main()
