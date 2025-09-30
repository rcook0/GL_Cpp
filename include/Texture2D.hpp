#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include <algorithm>

class Texture2D {
public:
    int width, height;
    std::vector<uint8_t> data; // RGB format, 3 bytes per pixel

    Texture2D() : width(0), height(0) {}
    Texture2D(int w,int h) : width(w),height(h),data(w*h*3,0) {}

    bool valid() const { return (width>0 && height>0 && !data.empty()); }

    // Load from binary PPM (P6)
    void load_ppm(const std::string& filename) {
        std::ifstream ifs(filename, std::ios::binary);
        if (!ifs) throw std::runtime_error("Texture2D: cannot open file " + filename);

        std::string header; int maxval;
        ifs >> header >> width >> height >> maxval;
        if (header != "P6") throw std::runtime_error("Texture2D: not a binary PPM (P6)");
        ifs.get(); // consume newline

        data.resize(width*height*3);
        ifs.read((char*)data.data(), data.size());
        if (!ifs) throw std::runtime_error("Texture2D: file read error");
    }

    // Save to PPM
    void save_ppm(const std::string& filename) const {
        std::ofstream ofs(filename, std::ios::binary);
        if (!ofs) throw std::runtime_error("Texture2D: cannot save " + filename);
        ofs << "P6\n" << width << " " << height << "\n255\n";
        ofs.write((char*)data.data(), data.size());
    }

    // Nearest neighbor sampling
    void sample_nearest(double u, double v, uint8_t& r, uint8_t& g, uint8_t& b) const {
        if (!valid()) { r=g=b=255; return; }
        // wrap UVs
        u = u - std::floor(u);
        v = v - std::floor(v);
        int x = std::clamp((int)(u * width), 0, width-1);
        int y = std::clamp((int)((1.0-v) * height), 0, height-1);
        size_t idx = (y*width + x)*3;
        r=data[idx+0]; g=data[idx+1]; b=data[idx+2];
    }

    // Bilinear sampling
    void sample_bilinear(double u, double v, uint8_t& r, uint8_t& g, uint8_t& b) const {
        if (!valid()) { r=g=b=255; return; }
        u = u - std::floor(u);
        v = v - std::floor(v);

        double fx = u*width - 0.5;
        double fy = (1.0-v)*height - 0.5;
        int x0 = std::clamp((int)std::floor(fx), 0, width-1);
        int y0 = std::clamp((int)std::floor(fy), 0, height-1);
        int x1 = std::min(x0+1,width-1);
        int y1 = std::min(y0+1,height-1);
        double tx = fx - x0;
        double ty = fy - y0;

        auto getPix=[&](int x,int y){
            size_t idx = (y*width+x)*3;
            return std::array<double,3>{ (double)data[idx+0],
                                         (double)data[idx+1],
                                         (double)data[idx+2] };
        };
        auto c00=getPix(x0,y0);
        auto c10=getPix(x1,y0);
        auto c01=getPix(x0,y1);
        auto c11=getPix(x1,y1);

        std::array<double,3> c;
        for(int k=0;k<3;k++) {
            double a = c00[k]*(1-tx) + c10[k]*tx;
            double b = c01[k]*(1-tx) + c11[k]*tx;
            c[k] = a*(1-ty) + b*ty;
        }
        r=(uint8_t)std::clamp((int)c[0],0,255);
        g=(uint8_t)std::clamp((int)c[1],0,255);
        b=(uint8_t)std::clamp((int)c[2],0,255);
    }
};
