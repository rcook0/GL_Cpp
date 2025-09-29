#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include <type_traits>
#include <cstdint>

template<typename PixelT=uint8_t>
class RasterBuffer {
public:
    int width, height, channels; // channels = 1 (gray) or 3 (RGB) or 4 (RGBA)
    std::vector<PixelT> data;
    std::vector<double> depth; // Z-buffer

    RasterBuffer(int w, int h, int c=1, PixelT clear=0, bool enableDepth=false)
        : width(w), height(h), channels(c), data(w*h*c, clear),
          depth(enableDepth ? std::vector<double>(w*h, 1e9) : std::vector<double>()) {
        if (w<=0 || h<=0 || (c!=1 && c!=3 && c!=4)) {
            throw std::runtime_error("RasterBuffer: invalid dimensions or channels");
        }
    }

/*
    RasterBuffer(int w, int h, int c=1, PixelT clear=0)
        : width(w), height(h), channels(c), data(w*h*c, clear) {
        if (w<=0 || h<=0 || (c!=1 && c!=3 && c!=4)) {
            throw std::runtime_error("RasterBuffer: invalid dimensions or channels");
        }
    }
*/

    inline bool has_depth() const { return !depth.empty(); }
    inline void clear_depth(double val=1e9) {
        if (has_depth()) std::fill(depth.begin(), depth.end(), val);
    }

    inline bool test_and_set_depth(int x, int y, double z) {
        if (!has_depth() || !in_bounds(x,y)) return true;
        size_t idx = (size_t)y*width + x;
        if (z < depth[idx]) { depth[idx] = z; return true; }
        return false; // occluded
    }

    inline bool in_bounds(int x, int y) const {
        return (x>=0 && x<width && y>=0 && y<height);
    }

    inline void set_pixel(int x, int y, PixelT r, PixelT g=0, PixelT b=0, PixelT a=255) {
        if (!in_bounds(x,y)) return;
        size_t idx = static_cast<size_t>(y)*width + x;
        idx *= channels;
        if (channels==1) {
            data[idx] = r;
        } else if (channels==3) {
            data[idx+0]=r; data[idx+1]=g; data[idx+2]=b;
        } else {
            data[idx+0]=r; data[idx+1]=g; data[idx+2]=b; data[idx+3]=a;
        }
    }

    inline void get_pixel(int x, int y, PixelT& r, PixelT& g, PixelT& b, PixelT& a) const {
        r=g=b=0; a=255;
        if (!in_bounds(x,y)) return;
        size_t idx = (static_cast<size_t>(y)*width + x) * channels;
        if (channels==1) {
            r = data[idx];
            g = b = r; a = 255;
        } else if (channels==3) {
            r=data[idx]; g=data[idx+1]; b=data[idx+2]; a=255;
        } else {
            r=data[idx]; g=data[idx+1]; b=data[idx+2]; a=data[idx+3];
        }
    }

    // Simple portable dump for quick inspection (PPM for 1/3-channel, PPM ignores alpha)
    void save_ppm(const std::string& path) const {
        std::ofstream f(path, std::ios::binary);
        if (!f) throw std::runtime_error("Failed to open PPM file");
        f << "P6\n" << width << " " << height << "\n255\n";
        for (int y=0; y<height; ++y) {
            for (int x=0; x<width; ++x) {
                PixelT r,g,b,a;
                get_pixel(x,y,r,g,b,a);
                unsigned char rgb[3] = {
                    static_cast<unsigned char>(r),
                    static_cast<unsigned char>(g),
                    static_cast<unsigned char>(b)
                };
                f.write(reinterpret_cast<char*>(rgb), 3);
            }
        }
    }
};
