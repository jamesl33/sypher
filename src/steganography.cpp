#include "steganography.hpp"

void steganography::encode_file(const std::string& file_name) {
    boost::filesystem::path file_path(file_name);
    std::vector<bool> bitstring;
    std::vector<bool> bitstring_file_data = this -> load_file(file_name);
    std::vector<bool> bitstring_file_name = this -> string_to_binary(file_path.filename().string());
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

void steganography::save_file(const std::string& file_name, const std::vector<bool>& bitstring) {
    std::bitset<8> buffer;
    std::ofstream file(file_name.data(), std::ios::binary);

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

unsigned char steganography::get_least_significant_bit(const unsigned char& byte) {
    return byte & 1;
};

void steganography::set_least_significant_bit(unsigned char* number, const bool& bit_value) {
    if (bit_value) {
        *number = *number | 1;
    } else {
        *number = (*number & ~1) | 0;
    }
};

std::vector<bool> steganography::string_to_binary(const std::string& string) {
    std::bitset<8> buffer;
    std::vector<bool> binary;

    for (unsigned int i = 0; i < string.size(); i++) {
        buffer = char(string[i]);

        for (unsigned int i = 0; i < buffer.size(); i++) {
            binary.emplace_back(buffer.test(i));
        }
    }

    return binary;
};

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
