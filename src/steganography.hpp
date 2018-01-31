#include <string>
#include <vector>
#include <bitset>
#include <fstream>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

class steganography {
    public:
        explicit steganography(const std::string& input_image) {
            this -> input_image = input_image;
            this -> load_image();
        }

        void encode_file(const std::string&);
        void decode_file();

    private:
        std::string input_image;
        cv::Mat image;

        void load_image();

        std::vector<bool> load_file(const std::string&);
        void save_file(const std::string&, const std::vector<bool>&);

        void encode_bitstring(std::vector<bool>&);
        std::vector<bool> decode_bitstring();

        unsigned char get_least_significant_bit(const unsigned char&);
        void set_least_significant_bit(unsigned char*, const bool&);

        std::vector<bool> string_to_binary(const std::string&);
        std::string binary_to_string(const std::vector<bool>&);
};
