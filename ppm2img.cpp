#include "RasterBuffer.hpp"

// Only in *one* translation unit:
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main(int argc, char** argv) {
    if (argc < 3) {
        printf("Usage: %s input.ppm output.png|output.jpg\n", argv[0]);
        return 1;
    }

    std::string inFile = argv[1];
    std::string outFile = argv[2];

    // Load PPM (P6 or P5) into RasterBuffer
    std::ifstream ifs(inFile, std::ios::binary);
    if (!ifs) {
        printf("Error: cannot open %s\n", inFile.c_str());
        return 1;
    }

    std::string header; int w,h,maxval;
    ifs >> header >> w >> h >> maxval;
    ifs.get(); // eat newline
    if (header != "P6" && header != "P5") {
        printf("Error: only PPM P6/P5 supported\n");
        return 1;
    }

    int channels = (header=="P6" ? 3 : 1);
    RasterBuffer<uint8_t> rb(w,h,channels,0,false);
    ifs.read((char*)rb.data.data(), rb.data.size());

    // Save as requested format
    rb.save_image(outFile);

    printf("Converted %s -> %s\n", inFile.c_str(), outFile.c_str());
    return 0;
}
