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
void steganography::encode_file(const boost::filesystem::path& file_path) {
    std::vector<bool> bitstring = this -> encode_filename(file_path.filename().string());
    std::vector<bool> file_data = this -> load_file(file_path);

    for (int i = 0; i < file_data.size(); i++) {
        bitstring.emplace_back(file_data.at(i));
    }

    this -> encode_bitstring(bitstring);
};

// Decode the file from the image by doing the inverse of the breakdown at the beginning of this file.
void steganography::decode_file() {
    std::vector<bool> bitstring = this -> decode_bitstring();
    std::string filename;
    std::vector<bool> bitstring_file_data;

    try {
        filename = this -> decode_filename(bitstring);

        for (unsigned int i = 32 + filename.size() * 8; i < bitstring.size(); i++) {
            bitstring_file_data.emplace_back(bitstring.at(i));
        }
    } catch (std::out_of_range) {
        std::cerr << "There doesn't appear to be an file stored in this image" << std::endl;
        exit(1);
    }

    if (this -> image_path.parent_path().string().size() <= 0) {
        this -> save_file(this -> image_path.parent_path().string() + "steg-" + filename, bitstring_file_data);
    } else {
        this -> save_file(this -> image_path.parent_path().string() + "/" + "steg-" + filename,  bitstring_file_data);
    }
};

// // Encode the binary representation of a file into an image. This is the step where a 32bit number is created and prepended to the string of binary.
void steganography::encode_bitstring(std::vector<bool>& bitstring) {
    std::bitset<32> bitstring_length = bitstring.size();

    if (bitstring_length.to_ulong() + 32 > this -> image_size) {
        std::cerr << "Not enough room in image to store this file" << std::endl;
        exit(1);
    }

    int index = 0;
    for (int row = 0; row < this -> image.rows; row++) {
        if (index == bitstring_length.to_ulong() + 32) {
            break;
        }
        for (int col = 0; col < this -> image.cols; col++) {
            if (index == bitstring_length.to_ulong() + 32) {
                break;
            }
            for (int cha = 0; cha < this -> image.channels(); cha++) {
                if (index == bitstring_length.to_ulong() + 32) {
                    break;
                }

                if (index < 32) {
                    if (this -> image.channels() == 3) {
                        this -> set_lsb(&this -> image.at<cv::Vec3b>(row, col)[cha], bitstring_length.test(index));
                    } else {
                        this -> set_lsb(&this -> image.at<cv::Vec4b>(row, col)[cha], bitstring_length.test(index));
                    }
                } else {
                    if (this -> image.channels() == 3) {
                        this -> set_lsb(&this -> image.at<cv::Vec3b>(row, col)[cha], bitstring.at(index - 32));
                    } else {
                        this -> set_lsb(&this -> image.at<cv::Vec4b>(row, col)[cha], bitstring.at(index - 32));
                    }
                }

                index++;
            }
        }
    }

    if (this -> image_path.parent_path().string().size() <= 0) {
        cv::imwrite(this -> image_path.parent_path().string() + "steg-" + this -> image_path.filename().string(), this -> image);
    } else {
        cv::imwrite(this -> image_path.parent_path().string() + "/" + "steg-" + this -> image_path.filename().string(), this -> image);
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
                        bitstring_length.set(index, this -> get_lsb(this -> image.at<cv::Vec3b>(row, col)[cha]));
                    } else {
                        bitstring_length.set(index, this -> get_lsb(this -> image.at<cv::Vec4b>(row, col)[cha]));
                    }

                    if (index == 31) {
                        length += bitstring_length.to_ulong();
                    }

                } else {
                    if (this -> image.channels() == 3) {
                        bitstring.emplace_back(this -> get_lsb(this -> image.at<cv::Vec3b>(row, col)[cha]));
                    } else {
                        bitstring.emplace_back(this -> get_lsb(this -> image.at<cv::Vec4b>(row, col)[cha]));
                    }
                }

                index++;
            }
        }
    }

    return bitstring;
};

// Load a file into a binary string (vector<bool>)
std::vector<bool> steganography::load_file(const boost::filesystem::path& file_path) {
    std::ifstream file(file_path.string(), std::ios::binary);
    std::vector<bool> bitstring;

    std::bitset<8> byte;
    if (file.good()) {
        while (!file.eof()) {
            byte = file.get();
            for (unsigned int i = 0; i < 8; i++) {
                bitstring.emplace_back(byte[i]);
            }
        }
    } else {
        std::cerr << "Failed to open file: " << strerror(errno) << std::endl;
        exit(1);
    }

    file.close();

    return bitstring;
};

// This is the inverse of 'load_file'.
void steganography::save_file(const std::string& file_name, const std::vector<bool>& bitstring) {
    std::ofstream file(file_name, std::ios::binary);

    std::bitset<8> buffer;
    if (file.good()) {
        for (unsigned int i = 0; i < bitstring.size(); i++) {
            if (i % 8 == 0 && i != 0) {
                file.put(buffer.to_ulong());
            }
            buffer[i % 8] = bitstring[i];
        }
    } else {
        std::cerr << "Failed to save file: " << strerror(errno) << std::endl;
        exit(1);
    }

    file.close();
};

// Get the least significant bit from a byte
inline unsigned char steganography::get_lsb(const unsigned char& byte) {
    return byte & 1;
};

// Set the least significant bit in a byte depending on 'bit_value'
inline void steganography::set_lsb(unsigned char* number, const bool& bit_value) {
    if (bit_value) {
        *number = *number | 1;
    } else {
        *number = (*number & ~1) | 0;
    }
};

// Prepare the filename to be encoded into the image (prepend a 32bit numebr detailing how long the filename is)
std::vector<bool> steganography::encode_filename(const std::string& filename) {
    std::vector<bool> bitstring;
    std::bitset<32> filename_length = filename.size() * 8;

    for (int i = 0; i < 32; i++) {
        bitstring.emplace_back(filename_length.test(i));
    }

    std::bitset<8> buffer;
    for (unsigned int i = 0; i < filename.size(); i++) {
        buffer = char(filename[i]);
        for (unsigned int i = 0; i < buffer.size(); i++) {
            bitstring.emplace_back(buffer.test(i));
        }
    }

    return bitstring;
};

// This is the inverse of 'encode_filename'
std::string steganography::decode_filename(const std::vector<bool>& bitstring) {
    std::vector<bool> filename;
    std::bitset<32> filename_length;

    for (int i = 0; i < 32; i++) {
        filename_length.set(i, bitstring.at(i));
    }

    for (unsigned int i = 32; i < filename_length.to_ulong() + 32; i++) {
        filename.emplace_back(bitstring.at(i));
    }

    return this -> binary_to_string(filename);
};

// Convert a binary string into a normal string. This is the inverse of 'string_to_binary'
std::string steganography::binary_to_string(const std::vector<bool>& binary) {
    std::string string;

    std::bitset<8> buffer;
    for (unsigned int i = 0; i < binary.size() + 1; i++) {
        if (i % 8 == 0 && i != 0) {
            string += buffer.to_ulong();
        }
        buffer[i % 8] = binary[i];
    }

    return string;
};
