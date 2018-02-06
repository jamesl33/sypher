#include "steganography.hpp"

/* Breakdown of how the files are stored in images.

   1. Read a file into a string of binary (in this case I am using a vector<bool>).
   2. Store the length of the string of binary in 32bit binary form.
   3. Prefix the string of binary with its length in binary. (This is how we know how much data to read when decoding).
   4. Loop through every channel of every pixel and set the least significant bit to one in the string of binary in order.

   Breakdown of how the filename is stored.
   1. Convert the filename into its binary representation.
   2. As with determining the length of the string of binary data; create a 32bit binary number which represents the length of the filename.
   3. Prefix the binary filename with its length.
   4. Add this to the beginning of the string of binary data before the first 32bit number is created. (The single string of binary which is encoded contains both the file data and filename)
*/

// Produce a string the string of binary detailed in the breakdown above ^^ and encode it in the image using 'encode_bitstring'
void steganography::encode_file(const std::string& path_to_file) {
    boost::filesystem::path file_path(path_to_file);
    std::vector<bool> bitstring;
    std::vector<bool> bitstring_file_name = this -> string_to_binary(file_path.filename().string());
    std::vector<bool> bitstring_file_data = this -> load_file(path_to_file);
    std::bitset<32> bitstring_file_name_length = bitstring_file_name.size();

    for (unsigned int i = 0; i < bitstring_file_name_length.size(); i++) {
        bitstring.emplace_back(bitstring_file_name_length.test(i));
    }

    for (bool b : bitstring_file_name) {
        bitstring.emplace_back(b);
    }

    for (bool b : bitstring_file_data) {
        bitstring.emplace_back(b);
    }

    this -> encode_bitstring(bitstring);
};

// Decode the file from the image by doing the inverse of the breakdown at the beginning of this file.
void steganography::decode_file() {
    std::bitset<32> bitstring_file_name_length;
    std::vector<bool> bitstring = this -> decode_bitstring();
    std::vector<bool> bitstring_file_data;
    std::vector<bool> bitstring_file_name;

    try {
        for (int i = 0; i < 32; i++) {
            bitstring_file_name_length.set(i, bitstring.at(i));
        }

        for (unsigned int i = 32; i < bitstring_file_name_length.to_ulong() + 32; i++) {
            bitstring_file_name.emplace_back(bitstring.at(i));
        }

        for (unsigned int i = 32 + bitstring_file_name_length.to_ulong(); i < bitstring.size(); i++) {
            bitstring_file_data.emplace_back(bitstring.at(i));
        }
    } catch (std::out_of_range) {
        std::cerr << "There doesn't appear to be an file stored in this image" << std::endl;
        exit(1);
    }
    if (this -> input_image_path.parent_path().string().size() <= 0) {
        this -> save_file(this -> input_image_path.parent_path().string() + "steg-" + this -> binary_to_string(bitstring_file_name), bitstring_file_data);
    } else {
        this -> save_file(this -> input_image_path.parent_path().string() + "/" + "steg-" + this -> binary_to_string(bitstring_file_name), bitstring_file_data);
    }
};

// Encode the binary representation of a file into an image. This is the step where a 32bit number is created and prepended to the string of binary.
void steganography::encode_bitstring(std::vector<bool>& bitstring) {
    std::bitset<32> length = bitstring.size();
    std::vector<bool> encode_bitstring;

    for (unsigned int i = 0; i < length.size(); i++) {
        encode_bitstring.emplace_back(length.test(i));
    }

    for (bool b : bitstring) {
        encode_bitstring.emplace_back(b);
    }

    int encode_bitstring_size = encode_bitstring.size();

    if (encode_bitstring.size() > this -> image_size) {
        std::cerr << "Not enough room in image to store this file" << std::endl;
        exit(1);
    }

    int encode_bitstring_index = 0;

    for (int row = 0; row < this -> image.rows; row++) {
        if (encode_bitstring_index == encode_bitstring_size) {
            break;
        }
        for (int col = 0; col < this -> image.cols; col++) {
            if (encode_bitstring_index == encode_bitstring_size) {
                break;
            }
            for (int cha = 0; cha < this -> image.channels(); cha++) {
                if (encode_bitstring_index == encode_bitstring_size) {
                    break;
                }

                if (this -> image.channels() == 3) {
                    this -> set_least_significant_bit(&this -> image.at<cv::Vec3b>(row, col)[cha], encode_bitstring.at(encode_bitstring_index));
                } else {
                    this -> set_least_significant_bit(&this -> image.at<cv::Vec4b>(row, col)[cha], encode_bitstring.at(encode_bitstring_index));
                }

                encode_bitstring_index++;
            }
        }
    }

    if (this -> input_image_path.parent_path().string().size() <= 0) {
        cv::imwrite(this -> input_image_path.parent_path().string() + "steg-" + this -> input_image_path.filename().string(), this -> image);
    } else {
        cv::imwrite(this -> input_image_path.parent_path().string() + "/" + "steg-" + this -> input_image_path.filename().string(), this -> image);
    }
};

// Decode a binary string from an image using the 32bit number at the beginning to determine when to stop looping through the pixel (This saves alot of time)
std::vector<bool> steganography::decode_bitstring() {
    unsigned int index = 0;
    unsigned int length = 32;
    std::bitset<32> bitstring_length;
    std::vector<bool> bitstring;

    for (int row = 0; row < this -> image.rows; row++) {
        if (index == length) {
            break;
        }
        for (int col = 0; col < this -> image.cols; col++) {
            if (index == length) {
                break;
            }
            for (int cha = 0; cha < this -> image.channels(); cha++) {
                if (index == length) {
                    break;
                }
                if (index < 32) {
                    if (this -> image.channels() == 3) {
                        bitstring_length.set(index, this -> get_least_significant_bit(this -> image.at<cv::Vec3b>(row, col)[cha]));
                    } else {
                        bitstring_length.set(index, this -> get_least_significant_bit(this -> image.at<cv::Vec4b>(row, col)[cha]));
                    }

                    if (index == 31) {
                        length += bitstring_length.to_ulong();
                    }

                } else {
                    if (this -> image.channels() == 3) {
                        bitstring.emplace_back(this -> get_least_significant_bit(this -> image.at<cv::Vec3b>(row, col)[cha]));
                    } else {
                        bitstring.emplace_back(this -> get_least_significant_bit(this -> image.at<cv::Vec4b>(row, col)[cha]));
                    }
                }

                index++;
            }
        }
    }

    return bitstring;
};

// Load a file into a binary string (vector<bool>)
std::vector<bool> steganography::load_file(const std::string& file_name) {
    std::bitset<8> byte;
    std::ifstream file(file_name, std::ios::binary);
    std::vector<bool> bitstring;

    while (file.good() && !file.eof()) {
        byte = file.get();
        for (unsigned int i = 0; i < byte.size(); i++) {
            bitstring.emplace_back(byte[i]);
        }
    }

    file.close();

    return bitstring;
};

// TODO fix very subtle logic error

// This is the inverse of 'load_file'.
void steganography::save_file(const std::string& file_name, const std::vector<bool>& bitstring) {
    std::bitset<8> buffer;
    std::ofstream file(file_name, std::ios::binary);

    if (file.good()) {
        for (unsigned int i = 1; i < bitstring.size() + 1; i++) {
            if (i % 8 == 0) {
                file.put(buffer.to_ulong());
            }
            buffer[i % 8] = bitstring[i];
        }
    } else {
        std::cerr << strerror(errno) << std::endl;
    }

    file.close();
};

// Get the least significant bit from a byte
inline unsigned char steganography::get_least_significant_bit(const unsigned char& byte) {
    return byte & 1;
};

// Set the least significant bit in a byte depending on 'bit_value'
inline void steganography::set_least_significant_bit(unsigned char* number, const bool& bit_value) {
    if (bit_value) {
        *number = *number | 1;
    } else {
        *number = (*number & ~1) | 0;
    }
};

// Convert a filename into it's binary representation and store it in a 'vector<bool>'
std::vector<bool> steganography::string_to_binary(const std::string& filename) {
    std::bitset<8> buffer;
    std::vector<bool> binary;

    for (unsigned int i = 0; i < filename.size(); i++) {
        buffer = char(filename[i]);

        for (unsigned int i = 0; i < buffer.size(); i++) {
            binary.emplace_back(buffer.test(i));
        }
    }

    return binary;
};

// Convert a binary string into a normal string. This is the inverse of 'string_to_binary'
std::string steganography::binary_to_string(const std::vector<bool>& binary) {
    std::bitset<8> buffer;
    std::string string;

    for (unsigned int i = 0; i < binary.size() + 1; i++) {
        if (i % 8 == 0 && i != 0) {
            string += buffer.to_ulong();
        }
        buffer[i % 8] = binary[i];
    }

    return string;
};
