#include "RasterBuffer.hpp"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0]
                  << " <input_folder> <ext: png|jpg>\n";
        return 1;
    }
    std::string folder = argv[1];
    std::string outExt = argv[2];
    if (outExt != "png" && outExt != "jpg") {
        std::cout << "Only png or jpg allowed\n";
        return 1;
    }

    for (auto& entry : fs::directory_iterator(folder)) {
        if (entry.is_regular_file()) {
            std::string inFile = entry.path().string();
            if (entry.path().extension() == ".ppm") {
                // load
                std::ifstream ifs(inFile, std::ios::binary);
                std::string header; int w,h,maxval;
                ifs >> header >> w >> h >> maxval;
                ifs.get();
                if (header!="P6" && header!="P5") continue;
                int channels = (header=="P6" ? 3 : 1);
                RasterBuffer<uint8_t> rb(w,h,channels,0,false);
                ifs.read((char*)rb.data.data(), rb.data.size());

                // save
                std::string outFile = entry.path().replace_extension("." + outExt).string();
                rb.save_image(outFile);
                std::cout << "Converted " << inFile << " -> " << outFile << "\n";
            }
        }
    }
    return 0;
}
