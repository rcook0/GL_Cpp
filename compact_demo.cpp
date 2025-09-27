#include "MeshBuilders.hpp"
#include "MeshRenderer2D.hpp"
#include "Transformation3D.hpp"
#include "View3DParameters.hpp"
#include "RasterBuffer.hpp"
#include <iostream>

int main() {
    // Camera
    View3DParameters params(Point3D(0,0,5), Point3D(0,0,0), Point3D(0,1,0),
                            60.0*M_PI/180.0, 1.0, 0.1, 100.0);
    View3D cam = params.make_view();
    Projection3D proj = params.make_projection();

    // UV sphere
    Mesh3D sphere = make_uv_sphere(32, 48, 1.2);

    // Spin it a bit for drama
    Transformation3D model = Transformation3D::rotation_y(0.6) * Transformation3D::rotation_x(-0.3);

    // To VIEW space (camera at origin)
    Transformation3D V = cam.view_matrix();
    for (auto& p : sphere.vertices) p = V.apply(model.apply(p));

    // True vertex normals (view space)
    auto vnorm = sphere.compute_vertex_normals();

    // Target buffer w/ Z
    RasterBuffer<uint8_t> rb(600,600,1,0,true);

    // Renderer
    MeshRenderer2D renderer(rb, cam, proj, Point3D(0.3, 0.4, -1.0)); // lightDir

    // Gouraud
    rb.clear_depth(); rb.data.assign(rb.data.size(),0);
    renderer.render_gouraud(sphere, vnorm, false);
    rb.save_ppm("sphere_gouraud.ppm");

    // Phong per-pixel
    rb.clear_depth(); rb.data.assign(rb.data.size(),0);
    renderer.render_phong(sphere, vnorm, false, /*base*/230, /*kd*/0.7, /*ks*/0.4, /*shininess*/32.0);
    rb.save_ppm("sphere_phong.ppm");

    std::cout << "Saved sphere_gouraud.ppm and sphere_phong.ppm\n";
    return 0;
}
