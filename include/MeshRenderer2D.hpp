#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <vector>

#include "Mesh3D.hpp"
#include "View3D.hpp"
#include "Projection3D.hpp"
#include "Drawing2D.hpp"
#include "Shading.hpp"       // lambert01(), phong01(), to_u8()

// Rendering modes
enum class RenderMode {
    Flat,       // face-normal Lambert
    Gouraud,    // per-vertex diffuse (smooth)
    Phong       // per-pixel (interpolated normals)
};

class MeshRenderer2D {
public:
    RasterBuffer<uint8_t>& rb;
    View3D&                camera;
    Projection3D&          projection;
    Point3D                lightDir;

    MeshRenderer2D(RasterBuffer<uint8_t>& rb, View3D& cam, Projection3D& proj,
                   const Point3D& lightDir = Point3D(0,0,-1))
        : rb(rb), camera(cam), projection(proj), lightDir(lightDir) {}

    // In MeshRenderer2D
    void render_phong(const Mesh3D& mesh,
                  const std::vector<Point3D>& vertexNormals,
                  bool drawWire=true,
                  uint8_t base=220, double kd=0.7, double ks=0.3, double shininess=24.0) {
        Drawing2D draw(rb);

        auto viewport=[&](const Point2D& p){
            double x=(p.x+1.0)*0.5*rb.width;
            double y=(1.0-(p.y+1.0)*0.5)*rb.height;
            return Point2D(x,y);
        };

        for (const auto& face : mesh.faces) {
            if (face.indices.size()<3) continue;
        
            Polygon3D poly = mesh.to_polygon(face);
            Point3D n = poly.normal();        // view-space face normal
            if (n.z >= 0) continue;           // backface cull

            std::vector<Point2D> pts2d;
            std::vector<Point3D> vpos;
            std::vector<Point3D> vnorm;

            for (int vi : face.indices) {
                const Point3D& P = mesh.vertices[vi];
                const Point3D& N = vertexNormals[vi];
                vpos.push_back(P);
                vnorm.push_back(N);
                pts2d.push_back(viewport(projection.project(P)));
            }

            draw.fill_polygon_phong(pts2d, vpos, vnorm,
                                base, kd, ks, shininess, lightDir);

            if (drawWire) draw.polygon(pts2d, 255);
        }
    }


    // Render a mesh already in VIEW space (i.e., model->view applied).
    // vertexNormals must be view-space too (use Mesh3D::compute_vertex_normals() after view xform).
    // perspectiveCorrect: if true, do approximate perspective-correct interpolation using 1/z as w.
    void render(const Mesh3D& mesh,
                const std::vector<Point3D>& vertexNormals,
                RenderMode mode = RenderMode::Gouraud,
                bool drawWire = false,
                const Texture2D* tex = nullptr 
                /*
                bool perspectiveCorrect = true,
                uint8_t base = 230, double kd = 0.7, double ks = 0.3, double shininess = 24.0 */)
    {
        Drawing2D draw(rb);

        // Viewport mapping: NDC [-1,1] -> pixel coords
        auto viewport = [&](const Point2D& p) {
            double x = (p.x + 1.0) * 0.5 * rb.width;
            double y = (1.0 - (p.y + 1.0) * 0.5) * rb.height; // flip Y
            return Point2D(x, y);
        };

        // Triangulate a face (fan)
        auto triangulate = [&](const Face& f) -> std::vector<std::array<int,3>> {
            std::vector<std::array<int,3>> tris;
            if (f.indices.size() < 3) return tris;
            for (size_t i = 1; i + 1 < f.indices.size(); ++i) {
                tris.push_back({ f.indices[0], f.indices[i], f.indices[i+1] });
            }
            return tris;
        };

        // Per-face loop -> fan triangulation
        for (const auto& face : mesh.faces) {
            auto tris = triangulate(face);
            if (tris.empty()) continue;

            // Face normal and backface culling in VIEW space
            Polygon3D poly = mesh.to_polygon(face);
            Point3D faceN  = poly.normal();      // view-space
            if (faceN.z >= 0) continue;          // away from camera

            for (const auto& tri : tris) {
                // Collect per-vertex view-space data
                std::array<Point3D,3> P = {
                    mesh.vertices[tri[0]],
                    mesh.vertices[tri[1]],
                    mesh.vertices[tri[2]]
                };
                std::array<Point3D,3> N = {
                    vertexNormals[tri[0]],
                    vertexNormals[tri[1]],
                    vertexNormals[tri[2]]
                };

                // Project to NDC and viewport-mapped screen coords
                std::array<Point2D,3> S;
                std::array<double,3>   zView;
                std::array<double,3>   invW;
                for (int i=0;i<3;++i) {
                    Point2D ndc = projection.project(P[i]);  // our Projection3D returns NDC-like coords
                    S[i] = viewport(ndc);
                    zView[i] = P[i].z;

                    // Approximate perspective correction with 1/z
                    // Clamp to avoid inf if z ~ 0 (behind near plane we shouldn't draw anyway)
                    double z = std::max(1e-6, std::abs(P[i].z));
                    invW[i] = perspectiveCorrect ? (1.0 / z) : 1.0;
                }

                // Backface cull in screen space too (area sign), optional
                double area = edgeFunction(S[0], S[1], S[2]);
                if (area >= 0) continue; // CW vs CCW depending on convention; skip if away

                // Rasterize the triangle according to the selected mode
                switch (mode) {
                    case RenderMode::Flat:
                        draw_triangle_flat(S, P, faceN, zView, base);
                        break;
                    case RenderMode::Gouraud:
                        draw_triangle_gouraud(S, P, N, zView, base);
                        break;
                    case RenderMode::Phong:
                        draw_triangle_phong(S, P, N, zView, invW, base, kd, ks, shininess);
                        break;
                }

                if (drawWire) {
                    draw.line(S[0], S[1], 255);
                    draw.line(S[1], S[2], 255);
                    draw.line(S[2], S[0], 255);
                }
            }
        }
    }



private:
    // Signed area *2 (helper for winding/backface)
    static inline double edgeFunction(const Point2D& a, const Point2D& b, const Point2D& c) {
        return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
    }

    // Compute barycentric weights for pixel center (x+0.5,y+0.5)
    static inline bool barycentric(const std::array<Point2D,3>& S, int px, int py,
                                   double& w0, double& w1, double& w2)
    {
        double area = edgeFunction(S[0], S[1], S[2]);
        if (area == 0.0) return false;

        double x = px + 0.5;
        double y = py + 0.5;

        w0 = edgeFunction(S[1], S[2], Point2D(x,y)) / area;
        w1 = edgeFunction(S[2], S[0], Point2D(x,y)) / area;
        w2 = 1.0 - w0 - w1;

        return (w0 >= 0.0 && w1 >= 0.0 && w2 >= 0.0);
    }

    // Triangle bounding box
    static inline void triBounds(const std::array<Point2D,3>& S,
                                 int& minX, int& minY, int& maxX, int& maxY,
                                 int W, int H)
    {
        minX = std::max(0, (int)std::floor(std::min({S[0].x, S[1].x, S[2].x})));
        minY = std::max(0, (int)std::floor(std::min({S[0].y, S[1].y, S[2].y})));
        maxX = std::min(W-1, (int)std::ceil (std::max({S[0].x, S[1].x, S[2].x})));
        maxY = std::min(H-1, (int)std::ceil (std::max({S[0].y, S[1].y, S[2].y})));
    }

    // Flat shading (Lambert with face normal)
    void draw_triangle_flat(const std::array<Point2D,3>& S,
                            const std::array<Point3D,3>& P,  // view-space positions
                            const Point3D& faceN_view,
                            const std::array<double,3>& zView,
                            uint8_t base)
    {
        int minX, minY, maxX, maxY;
        triBounds(S, minX, minY, maxX, maxY, rb.width, rb.height);

        // The same face normal everywhere
        double I = lambert01(faceN_view, lightDir);
        uint8_t Iu = (uint8_t)std::round((base / 255.0) * 255.0 * I);

        for (int y=minY; y<=maxY; ++y) {
            for (int x=minX; x<=maxX; ++x) {
                double w0,w1,w2;
                if (!barycentric(S, x, y, w0, w1, w2)) continue;

                double z = w0*zView[0] + w1*zView[1] + w2*zView[2];
                if (!rb.test_and_set_depth(x,y,z)) continue;

                rb.set_pixel(x,y,Iu);
            }
        }
    }

    // Gouraud shading (interpolate per-vertex diffuse intensities)
    void draw_triangle_gouraud(const std::array<Point2D,3>& S,
                               const std::array<Point3D,3>& P,
                               const std::array<Point3D,3>& N,
                               const std::array<double,3>& zView,
                               uint8_t base)
    {
        int minX, minY, maxX, maxY;
        triBounds(S, minX, minY, maxX, maxY, rb.width, rb.height);

        // Per-vertex intensity (0..1)
        std::array<double,3> Iv = {
            lambert01(N[0], lightDir),
            lambert01(N[1], lightDir),
            lambert01(N[2], lightDir)
        };

        // Per-vertex UVs
        std::array<Point2D,3> UV = {
            mesh.uv[tri[0]], mesh.uv[tri[1]], mesh.uv[tri[2]]
        };

        for (int y=minY; y<=maxY; ++y) {
            for (int x=minX; x<=maxX; ++x) {
                double w0,w1,w2;
                if (!barycentric(S, x, y, w0, w1, w2)) continue;

                double z = w0*zView[0] + w1*zView[1] + w2*zView[2];
                if (!rb.test_and_set_depth(x,y,z)) continue;

                double I = w0*Iv[0] + w1*Iv[1] + w2*Iv[2];
                rb.set_pixel(x,y, to_u8((base/255.0) * I));
            }
        }
    }

    // Phong per-pixel (interpolate normals & positions; optional perspective-ish correction using 1/z)
    void draw_triangle_phong(const std::array<Point2D,3>& S,
                             const std::array<Point3D,3>& P,
                             const std::array<Point3D,3>& N,
                             const std::array<double,3>& zView,
                             const std::array<double,3>& invW,   // 1/z if perspectiveCorrect, else {1,1,1}
                             uint8_t base, double kd, double ks, double shininess)
    {
        int minX, minY, maxX, maxY;
        triBounds(S, minX, minY, maxX, maxY, rb.width, rb.height);

        for (int y=minY; y<=maxY; ++y) {
            for (int x=minX; x<=maxX; ++x) {
                double w0,w1,w2;
                if (!barycentric(S, x, y, w0, w1, w2)) continue;

                // Perspective-ish correction: interpolate with 1/z weights
                double iw = w0*invW[0] + w1*invW[1] + w2*invW[2];
                if (iw <= 1e-12) continue;
                double a0 = (w0*invW[0]) / iw;
                double a1 = (w1*invW[1]) / iw;
                double a2 = (w2*invW[2]) / iw;

                double z = a0*zView[0] + a1*zView[1] + a2*zView[2];
                if (!rb.test_and_set_depth(x,y,z)) continue;
                
                // Interpolate view-space position & normal
                Point3D Ppix(
                    a0*P[0].x + a1*P[1].x + a2*P[2].x,
                    a0*P[0].y + a1*P[1].y + a2*P[2].y,
                    a0*P[0].z + a1*P[1].z + a2*P[2].z
                );
                Point3D Npix(
                    a0*N[0].x + a1*N[1].x + a2*N[2].x,
                    a0*N[0].y + a1*N[1].y + a2*N[2].y,
                    a0*N[0].z + a1*N[1].z + a2*N[2].z
                );
                Point3D Cpix(
                    a0*mesh.colors[i0].x + a1*mesh.colors[i1].x + a2*mesh.colors[i2].x,
                    a0*mesh.colors[i0].y + a1*mesh.colors[i1].y + a2*mesh.colors[i2].y,
                    a0*mesh.colors[i0].z + a1*mesh.colors[i1].z + a2*mesh.colors[i2].z
                );
                // View direction in view space: camera at origin → -P
                Point3D Vdir(-Ppix.x, -Ppix.y, -Ppix.z);
                
                Point2D uvPix(
                    a0*mesh.uv[i0].x + a1*mesh.uv[i1].x + a2*mesh.uv[i2].x,
                    a0*mesh.uv[i0].y + a1*mesh.uv[i1].y + a2*mesh.uv[i2].y
                )
                
                double I = phong01(Npix, lightDir, Vdir, kd, ks, shininess);
                uint8_t R,G,B;
                if (tex) {
                    tex->sample_bilinear(uvPix.x, uvPix.y, R,G,B);
                    R = (uint8_t)(R * I);
                    G = (uint8_t)(G * I);
                    B = (uint8_t)(B * I);
                } else {
                    uint8_t c = (uint8_t)(255 * I);
                    R=G=B=c;
                }
                rb.set_pixel(x, y, R, G, B);
            }
        }
    }
};
