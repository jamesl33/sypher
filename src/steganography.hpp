#include <string>
#include <vector>
#include <bitset>
#include <fstream>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

class steganography {
    public:
        // constructors
        explicit steganography(const std::string& input_image) {
            this -> input_image = input_image;
            this -> image = cv::imread(input_image, -1);
            this -> image_size  = this -> image.rows * this -> image.cols * this -> image.channels();

            if (!this -> image.data) {
                std::cerr << "Failed to open input image no such file or directory" << std::endl;
                exit(1);
            }
        }

        // encode/decode a file to/from an image
        void encode_file(const std::string&);
        void decode_file();

    private:
        // attributes
        cv::Mat image;
        std::string input_image;
        unsigned int image_size;

        // load/save file to/from a bitstring (std::vector<bool>)
        std::vector<bool> load_file(const std::string&);
        void save_file(const std::string&, const std::vector<bool>&);

        // encode/decode bitstrings (std::vector<bool>) to/from and image
        void encode_bitstring(std::vector<bool>&);
        std::vector<bool> decode_bitstring();

        // bit manipulation
        unsigned char get_least_significant_bit(const unsigned char&);
        void set_least_significant_bit(unsigned char*, const bool&);

        // convert between a string and binary
        std::vector<bool> string_to_binary(const std::string&);
        std::string binary_to_string(const std::vector<bool>&);
};
