#include <fstream>
int main() {
    int w=64,h=64;
    std::ofstream ofs("checker.ppm", std::ios::binary);
    ofs << "P6\n" << w << " " << h << "\n255\n";
    for(int y=0;y<h;y++){
        for(int x=0;x<w;x++){
            bool c = ((x/8)+(y/8))%2;
            unsigned char rgb[3] = { c?255:0, c?255:0, c?255:0 };
            ofs.write((char*)rgb,3);
        }
    }
}
