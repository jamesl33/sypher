#include "steganography.hpp"

void steganography::load_image() {
    this -> image = cv::imread(this -> input_image, -1);
    if (!this -> image.data) {
        std::cout << "No such file or directory" << std::endl;
        exit(1);
    }
};

void steganography::encode_file(const std::string& file_name) {
    std::vector<bool> bitstring_file_data = this -> load_file(file_name);
    std::vector<bool> bitstring_file_name = this -> string_to_binary(file_name);
    std::vector<bool> bitstring;
    std::bitset<32> bitstring_file_name_length = bitstring_file_name.size();

    for (int i = 0; i < bitstring_file_name_length.size(); i++) {
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

void steganography::decode_file() {
    std::vector<bool> bitstring = this -> decode_bitstring();
    std::bitset<32> bitstring_file_name_length;
    std::vector<bool> bitstring_file_name;
    std::vector<bool> bitstring_file_data;

    for (int i = 0; i < 32; i++) {
        bitstring_file_name_length.set(i, bitstring.at(i));
    }

    for (int i = 32; i < bitstring_file_name_length.to_ulong() + 32; i++) {
        bitstring_file_name.emplace_back(bitstring.at(i));
    }

    std::string file_name = this -> binary_to_string(bitstring_file_name);

    for (int i = 32 + bitstring_file_name_length.to_ulong(); i < bitstring.size(); i++) {
        bitstring_file_data.emplace_back(bitstring.at(i));
    }

    this -> save_file("steg-" + file_name, bitstring_file_data);
};

void steganography::encode_bitstring(std::vector<bool>& bitstring) {
    // TODO refactor
    if (bitstring.size() >  this -> image.rows * this -> image.cols * this -> image.channels()) {
        std::cout << "Not enough room in image to store this message" << std::endl;
        exit(1);
    }

    cv::Mat steg_image;
    this -> image.copyTo(steg_image);

    std::vector<bool> encode_bitstring;
    std::bitset<32> length = bitstring.size();

    for (int i = 0; i < length.size(); i++) {
        encode_bitstring.emplace_back(length.test(i));
    }

    for (bool b : bitstring) {
        encode_bitstring.emplace_back(b);
    }

    int encode_bitstring_size = encode_bitstring.size();
    int encode_bitstring_index = 0;

    for (int row = 0; row < steg_image.rows; row++) {
        if (encode_bitstring_index == encode_bitstring_size) {
            break;
        }
        for (int col = 0; col < steg_image.cols; col++) {
            if (encode_bitstring_index == encode_bitstring_size) {
                break;
            }
            for (int cha = 0; cha < steg_image.channels(); cha++) {
                if (encode_bitstring_index == encode_bitstring_size) {
                    break;
                }

                this -> set_least_significant_bit(&steg_image.at<cv::Vec3b>(row, col)[cha], encode_bitstring.at(encode_bitstring_index));
                encode_bitstring_index++;
            }
        }
    }

    cv::imwrite("steg-" + this -> input_image, steg_image);
};

std::vector<bool> steganography::decode_bitstring() {
    // TODO refactor
    std::vector<bool> bitstring;
    std::bitset<32> bitstring_length;
    int index = 0;

    for (int row = 0; row < this -> image.rows; row++) {
        if (index == 32) {
            break;
        }
        for (int col = 0; col < this -> image.cols; col++) {
            if (index == 32) {
                break;
            }
            for (int cha = 0; cha < this -> image.channels(); cha++) {
                if (index == 32) {
                    break;
                }

                bitstring_length.set(index, this -> get_least_significant_bit(this -> image.at<cv::Vec3b>(row, col)[cha]));
                index++;
            }
        }
    }

    index = 0;
    std::vector<bool> bitstring_and_metadata;

    for (int row = 0; row < this -> image.rows; row++) {
        if (index == 32 + bitstring_length.to_ulong()) {
            break;
        }
        for (int col = 0; col < this -> image.cols; col++) {
            if (index == 32 + bitstring_length.to_ulong()) {
                break;
            }
            for (int cha = 0; cha < this -> image.channels(); cha++) {
                if (index == 32 + bitstring_length.to_ulong()) {
                    break;
                }

                bitstring_and_metadata.emplace_back(this -> get_least_significant_bit(this -> image.at<cv::Vec3b>(row, col)[cha]));
                index++;
            }
        }
    }

    for (int i = 32; i < bitstring_and_metadata.size(); i++) {
        bitstring.emplace_back(bitstring_and_metadata.at(i));
    }

    return bitstring;
};

std::vector<bool> steganography::load_file(const std::string& file_name) {
    std::ifstream file(file_name, std::ios::binary);
    std::vector<bool> bitstring;
    std::bitset<8> byte;

    while (file.good() && !file.eof()) {
        byte = file.get();
        for (int i = 0; i < byte.size(); i++) {
            bitstring.emplace_back(byte[i]);
        }
    }

    file.close();

    return bitstring;
};

void steganography::save_file(const std::string& file_name, const std::vector<bool>& bitstring) {
    std::ofstream file(file_name.data(), std::ios::binary);
    std::bitset<8> buffer;

    if (file.good()) {
        for (int i = 1; i < bitstring.size() + 1; i++) {
            if (i % 8 == 0) {
                file.put(buffer.to_ulong());
            }
            buffer[i % 8] = bitstring[i];
        }
    } else {
        std::cout << strerror(errno) << std::endl;
    }

    file.close();
};

unsigned char steganography::get_least_significant_bit(const unsigned char& number) {
    return number & 1;
};

void steganography::set_least_significant_bit(unsigned char* number, const bool& bit_value) {
    if (bit_value) {
        *number = *number | 1;
    } else {
        *number = (*number & ~1) | 0;
    }
};

std::vector<bool> steganography::string_to_binary(const std::string& string) {
    std::vector<bool> binary;
    std::bitset<8> buffer;

    for (int i = 0; i < string.size(); i++) {
        buffer = char(string[i]);

        for (int i = 0; i < buffer.size(); i++) {
            binary.emplace_back(buffer.test(i));
        }
    }

    return binary;
};

std::string steganography::binary_to_string(const std::vector<bool>& binary) {
    std::string string;
    std::bitset<8> buffer;

    for (int i = 0; i < binary.size() + 1; i++) {
        if (i % 8 == 0 && i != 0) {
            string += buffer.to_ulong();
        }
        buffer[i % 8] = binary[i];
    }

    return string;
};
