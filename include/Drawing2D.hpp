#pragma once
#include "Point2D.hpp"
#include "RasterBuffer.hpp"
#include "Bresenham.hpp"
#include "Bezier2D.hpp"
#include <vector>
#include <cstdint>
#include <cmath>

class Drawing2D {
public:
    explicit Drawing2D(RasterBuffer<uint8_t>& rb) : rb(rb) {}

    void point(const Point2D& p, uint8_t c=255) {
        rb.set_pixel(static_cast<int>(std::lround(p.x)), static_cast<int>(std::lround(p.y)), c);
    }

    void line(const Point2D& a, const Point2D& b, uint8_t c=255) {
        Bresenham::line(rb, (int)std::lround(a.x), (int)std::lround(a.y),
                            (int)std::lround(b.x), (int)std::lround(b.y), c);
    }

    void polyline(const std::vector<Point2D>& pts, uint8_t c=255) {
        if (pts.size()<2) return;
        for (size_t i=0;i+1<pts.size();++i) line(pts[i], pts[i+1], c);
    }

    void polygon(const std::vector<Point2D>& pts, uint8_t c=255) {
        if (pts.size()<2) return;
        for (size_t i=0;i<pts.size();++i) {
            const auto& a = pts[i];
            const auto& b = pts[(i+1)%pts.size()];
            line(a,b,c);
        }
    }

    void fill_polygon(const std::vector<Point2D>& pts, uint8_t c=255) {
        if (pts.size() < 3) return;

        // Find y-range
        int minY = (int)std::floor(pts[0].y), maxY = minY;
        for (auto& p : pts) {
            minY = std::min(minY, (int)std::floor(p.y));
            maxY = std::max(maxY, (int)std::floor(p.y));
        }

        // Scanline
        for (int y = minY; y <= maxY; ++y) {
            std::vector<int> nodes;
            size_t n = pts.size();
            for (size_t i=0,j=n-1; i<n; j=i++) {
                const Point2D& pi = pts[i];
                const Point2D& pj = pts[j];
                if ((pi.y < y && pj.y >= y) || (pj.y < y && pi.y >= y)) {
                    int x = (int)(pi.x + (y - pi.y) * (pj.x - pi.x) / (pj.y - pi.y));
                    nodes.push_back(x);
                }
            }
            std::sort(nodes.begin(), nodes.end());
            for (size_t k=0; k+1 < nodes.size(); k+=2) {
                for (int x=nodes[k]; x<nodes[k+1]; ++x) {
                    rb.set_pixel(x,y,c);
                }
            }
        }
    }


    void circle(const Point2D& center, int radius, uint8_t c=255) {
        Bresenham::circle(rb, (int)std::lround(center.x), (int)std::lround(center.y), radius, c);
    }

    void quadratic_bezier(const Point2D& p0, const Point2D& p1, const Point2D& p2, int segments, uint8_t c=255) {
        auto pts = Bezier2D::tesselate_quadratic(p0,p1,p2,segments);
        polyline(pts, c);
    }

    void cubic_bezier(const Point2D& p0, const Point2D& p1, const Point2D& p2, const Point2D& p3, int segments, uint8_t c=255) {
        auto pts = Bezier2D::tesselate_cubic(p0,p1,p2,p3,segments);
        polyline(pts, c);
    }

private:
    RasterBuffer<uint8_t>& rb;
};
