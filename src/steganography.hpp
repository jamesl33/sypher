#include <bitset>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

class steganography {
    public:
        // constructors
        explicit steganography(const std::string& image_path) {
            this -> image_path = image_path;
            this -> image = cv::imread(image_path, -1);
            this -> image_size  = this -> image.rows * this -> image.cols * this -> image.channels();

            if (!this -> image.data) {
                std::cerr << "Failed to open input image no such file or directory" << std::endl;
                exit(1);
            }
        }

        // encode/decode a file to/from an image
        void encode_file(const boost::filesystem::path&);
        void decode_file();

    private:
        // attributes
        cv::Mat image;
        boost::filesystem::path image_path;
        unsigned int image_size;

        // load/save file to/from a bitstring (std::vector<bool>)
        std::vector<bool> load_file(const boost::filesystem::path&);
        void save_file(const std::string&, const std::vector<bool>&);

        // encode/decode bitstrings (std::vector<bool>) to/from and image
        void encode_bitstring(std::vector<bool>&);
        std::vector<bool> decode_bitstring();

        // bit manipulation
        inline unsigned char get_lsb(const unsigned char&);
        inline void set_lsb(unsigned char*, const bool&);

        // filename encoding/decoding
        std::vector<bool> encode_filename(const std::string&);
        std::string decode_filename(const std::vector<bool>&);

        // string/binary conversion
        std::string binary_to_string(const std::vector<bool>&);
};
