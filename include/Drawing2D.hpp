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

    void fill_polygon_z(const std::vector<Point2D>& pts2d,
                        const std::vector<double>& depths,
                        uint8_t c=255) {
        if (pts2d.size()<3 || pts2d.size()!=depths.size()) return;

        int minY = (int)std::floor(pts2d[0].y), maxY = minY;
        for (auto& p : pts2d) {
            minY = std::min(minY,(int)std::floor(p.y));
            maxY = std::max(maxY,(int)std::floor(p.y));
        }

        for (int y=minY; y<=maxY; ++y) {
            struct Node { int x; double z; };
            std::vector<Node> nodes;
            size_t n=pts2d.size();
            for (size_t i=0,j=n-1; i<n; j=i++) {
                const Point2D& pi = pts2d[i];
                const Point2D& pj = pts2d[j];
                double zi = depths[i], zj = depths[j];
                if ((pi.y<y && pj.y>=y) || (pj.y<y && pi.y>=y)) {
                    double t = (y - pi.y) / (pj.y - pi.y);
                    int x = (int)(pi.x + t*(pj.x - pi.x));
                    double z = zi + t*(zj - zi);
                    nodes.push_back({x,z});
                }
            }
            std::sort(nodes.begin(), nodes.end(),
                      [](auto&a,auto&b){return a.x<b.x;});
            for (size_t k=0;k+1<nodes.size();k+=2) {
                int x0=nodes[k].x, x1=nodes[k+1].x;
                double z0=nodes[k].z, z1=nodes[k+1].z;
                for (int x=x0;x<=x1;++x) {
                    double t = (x1==x0)?0.0:(double)(x-x0)/(x1-x0);
                    double z = z0 + t*(z1-z0);
                    if (rb.test_and_set_depth(x,y,z)) {
                        rb.set_pixel(x,y,c);
                    }
                }
            }
        }
    }

    void fill_polygon_shaded(const   std::vector<Point2D>& pts2d,
                             const std::vector<double>& depths,
                             const std::vector<uint8_t>& intensities) {
        if (pts2d.size()<3) return;
        if (pts2d.size()!=depths.size() || pts2d.size()!=intensities.size()) return;

        int minY=(int)std::floor(pts2d[0].y), maxY=minY;
        for (auto& p : pts2d) {
            minY=std::min(minY,(int)std::floor(p.y));
            maxY=std::max(maxY,(int)std::floor(p.y));
        }

        for (int y=minY; y<=maxY; ++y) {
            struct Node { int x; double z; double i; };
            std::vector<Node> nodes;
            size_t n=pts2d.size();
            for (size_t i=0,j=n-1;i<n;j=i++) {
                const Point2D& pi=pts2d[i]; const Point2D& pj=pts2d[j];
                double zi=depths[i], zj=depths[j];
                double ii=intensities[i], ij=intensities[j];
                if ((pi.y<y && pj.y>=y)||(pj.y<y && pi.y>=y)) {
                    double t=(y-pi.y)/(pj.y-pi.y);
                    int x=(int)(pi.x+t*(pj.x-pi.x));
                    double z=zi+t*(zj-zi);
                    double I=ii+t*(ij-ii);
                    nodes.push_back({x,z,I});
                }
            }
            std::sort(nodes.begin(),nodes.end(),[](auto&a,auto&b){return a.x<b.x;});
            for (size_t k=0;k+1<nodes.size();k+=2) {
                int x0=nodes[k].x, x1=nodes[k+1].x;
                double z0=nodes[k].z, z1=nodes[k+1].z;
                double i0=nodes[k].i, i1=nodes[k+1].i;
                for (int x=x0;x<=x1;++x) {
                    double t=(x1==x0)?0.0:(double)(x-x0)/(x1-x0);
                    double z=z0+t*(z1-z0);
                    double I=i0+t*(i1-i0);
                    if (rb.test_and_set_depth(x,y,z)) {
                        rb.set_pixel(x,y, clamp255(I));
                    }
                }
            }
        }
    }

    // Per-pixel Phong shading with Z-buffer
    void fill_polygon_phong(const std::vector<Point2D>& pts2d,
                            const std::vector<Point3D>& viewPos,    // per-vertex view-space positions
                            const std::vector<Point3D>& viewNorm,   // per-vertex view-space normals
                            uint8_t base=200,
                            double kd=0.7, double ks=0.3, double shininess=16.0,
                            const Point3D& lightDir = Point3D(0,0,-1)) {
        if (pts2d.size()<3 || pts2d.size()!=viewPos.size() || pts2d.size()!=viewNorm.size()) return;

        // y-range
        int minY=(int)std::floor(pts2d[0].y), maxY=minY;
        for (auto& p : pts2d) {
            minY=std::min(minY,(int)std::floor(p.y));
            maxY=std::max(maxY,(int)std::floor(p.y));
        }

        for (int y=minY; y<=maxY; ++y) {
            struct Node { int x; double z; Point3D n; Point3D pos; };
            std::vector<Node> nodes;
            size_t n=pts2d.size();
            for (size_t i=0,j=n-1;i<n;j=i++) {
                const Point2D& A=pts2d[i]; const Point2D& B=pts2d[j];
                const Point3D& pA=viewPos[i]; const Point3D& pB=viewPos[j];
                const Point3D& nA=viewNorm[i]; const Point3D& nB=viewNorm[j];

                bool crosses = (A.y<y && B.y>=y) || (B.y<y && A.y>=y);
                if (!crosses) continue;

                double t = (B.y == A.y) ? 0.0 : (y - A.y) / (B.y - A.y);
                int x   = (int)std::floor(A.x + t*(B.x - A.x));
                double z= pA.z + t*(pB.z - pA.z);
                Point3D N( nA.x + t*(nB.x - nA.x),
                           nA.y + t*(nB.y - nA.y),
                           nA.z + t*(nB.z - nA.z) );
                Point3D P( pA.x + t*(pB.x - pA.x),
                           pA.y + t*(pB.y - pA.y),
                           pA.z + t*(pB.z - pA.z) );
                nodes.push_back({x,z,N,P});
            }

            std::sort(nodes.begin(),nodes.end(),[](auto&a,auto&b){return a.x<b.x;});
            for (size_t k=0;k+1<nodes.size();k+=2) {
                int x0=nodes[k].x, x1=nodes[k+1].x;
                double z0=nodes[k].z, z1=nodes[k+1].z;
                Point3D n0=nodes[k].n, n1=nodes[k+1].n;
                Point3D p0=nodes[k].pos, p1=nodes[k+1].pos;

                for (int x=x0; x<=x1; ++x) {
                    double t = (x1==x0)?0.0:(double)(x-x0)/(x1-x0);
                    double z = z0 + t*(z1 - z0);
                    if (!rb.test_and_set_depth(x,y,z)) continue;

                    Point3D N( n0.x + t*(n1.x - n0.x),
                               n0.y + t*(n1.y - n0.y),
                               n0.z + t*(n1.z - n0.z) );
                    Point3D P( p0.x + t*(p1.x - p0.x),
                               p0.y + t*(p1.y - p0.y),
                               p0.z + t*(p1.z - p0.z) );

                    // View-space: camera at origin, looking -Z → viewDir = -P
                    Point3D viewDir(-P.x, -P.y, -P.z);
                    double I01 = phong01(N, lightDir, viewDir, kd, ks, shininess);
                    uint8_t I = (uint8_t)std::round((base/255.0)*255.0*I01);
                    rb.set_pixel(x,y,I);
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
