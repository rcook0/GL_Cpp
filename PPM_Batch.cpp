#include "RasterBuffer.hpp"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0]
                  << " <input_folder> <ext: png|jpg> [--delete]\n";
        return 1;
    }
    std::string folder = argv[1];
    std::string outExt = argv[2];
    bool doDelete = (argc >= 4 && std::string(argv[3]) == "--delete");
    
    if (outExt != "png" && outExt != "jpg") {
        std::cout << "Only png or jpg allowed\n";
        return 1;
    }

    for (auto& entry : fs::directory_iterator(folder)) {
        if (!entry.is_regular_file()) continue; 
        if (entry.path().extension() != ".ppm") continue;
        
        std::string inFile = entry.path().string();
        std::ifstream ifs(inFile, std::ios::binary);
        if (!ifs) {
            std::cerr << "Error: cannot open " << inFile << "\n";
            continue;
        }

        int channels = (header=="P6" ? 3 : 1);
        RasterBuffer<uint8_t> rb(w,h,channels,0,false);
        ifs.read((char*)rb.data.data(), rb.data.size());

        std::string outFile = entry.path().replace_extension("." + outExt).string();
        rb.save_image(outFile);

        std::cout << "Converted " << inFile << " -> " << outFile << "\n";

        if (doDelete) {
            try {
                fs::remove(entry.path());
                std::cout << "Deleted " << inFile << "\n";
            } catch (const std::exception& e) {
                std::cerr << "Failed to delete " << inFile << ": " << e.what() << "\n";
            }
        }
    }
    
    return 0;
}
