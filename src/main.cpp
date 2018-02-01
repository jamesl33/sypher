#include <iostream>
#include "optparse.hpp"
#include "steganography.hpp"

int main(int argc, const char** argv) {
    optparse::OptionParser parser = optparse::OptionParser()
        .prog("steganography")
        .description("Simple C++ program to allow the manipulation of 'Last Bit Steganographic' image");

    parser.add_option("-i", "--input-image")
        .action("store")
        .dest("input-image")
        .help("Path to 'PNG'/'BMP' image which you will manipulate *THIS OPTION IS MANDITORY*");

    parser.add_option("-e", "--encode-file")
        .action("store")
        .dest("encode-file")
        .help("Path to the file you want to hide");

    parser.add_option("-d", "--decode")
        .action("store_true")
        .help("Decodes '--input-file'");

    const optparse::Values options = parser.parse_args(argc, argv);
    const std::vector<std::string> args = parser.args();

    if (!std::string(options.get("input-image")).size()) {
        std::cout << "Error: You must specify an input image" << std::endl;
        std::cout << parser.format_help() << std::endl;
        exit(0);
    }

    steganography steg = steganography(options["input-image"]);

    if (options["encode-file"].size() && options.get("decode")) {
        std::cerr << "You can't both decode and encode at the same time" << std::endl;
        exit(1);
    }
    else if (std::string(options.get("encode-file")).size()) {
        steg.encode_file(std::string(options.get("encode-file")));
    }
    else if (options.get("decode")) {
        steg.decode_file();
    }
}
