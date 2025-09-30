/*
Scene 1: a 4×4 grid of cubes with per-vertex colors, no texture bound → fallback to colored Phong shading.

Scene 2: a UV-mapped cube-sphere with a checker texture → textured Phong shading.
*/

#include "RasterBuffer.hpp"
#include "Mesh3D.hpp"
#include "MeshRenderer2D.hpp"
#include "View3D.hpp"
#include "Projection3D.hpp"
#include "Texture2D.hpp"
#include "Transformation3D.hpp"

// Helpers from your mesh generators
extern Mesh3D make_cube_grid_custom(int m,int n,
    double w,double h,double d,double spacing,
    const Transformation3D* local,
    std::function<Point3D(int,int)> colorFn);

extern Mesh3D make_cube_sphere(int subdiv,double radius);

int main() {
    const int W=800,H=600;
    RasterBuffer<uint8_t> rb(W,H,3,0,true);

    // Camera & projection
    View3D cam(Point3D(0,2,6), Point3D(0,0,0), Point3D(0,1,0));
    Projection3D proj(60.0, (double)W/H, 0.1, 100.0);

    MeshRenderer2D renderer(rb, cam, proj, Point3D(0.3,0.4,-1.0));

    // ---------- Scene 1: Colored cube grid ----------
    rb.data.assign(rb.data.size(),0); rb.clear_depth();

    Mesh3D grid = make_cube_grid_custom(
        4, 4, 0.8, 0.8, 0.8, 0.1,
        nullptr,
        [&](int i,int j){
            // rainbow-like vertex colors
            return Point3D((i%4)/3.0, (j%4)/3.0, 0.5);
        });

    Transformation3D V = cam.view_matrix();
    for (auto& v : grid.vertices) v = V.apply(v);
    auto vnorm = grid.compute_vertex_normals();

    renderer.render(grid, vnorm, RenderMode::Phong, true /*wire*/, nullptr);
    rb.save_ppm("scene1_colored_grid.ppm");

    // ---------- Scene 2: Textured cube-sphere ----------
    rb.data.assign(rb.data.size(),0); rb.clear_depth();

    Mesh3D sphere = make_cube_sphere(16,1.5);
    Texture2D checker;
    checker.load_ppm("checker.ppm"); // make a simple PPM checkerboard

    for (auto& v : sphere.vertices) v = V.apply(v);
    auto snorm = sphere.compute_vertex_normals();

    renderer.render(sphere, snorm, RenderMode::Phong, false, &checker);
    rb.save_ppm("scene2_textured_sphere.ppm");

    return 0;
}
